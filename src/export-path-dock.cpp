/*
Export Path Dock
Copyright (C) 2026 Andy Xiao

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include "export-path-dock.hpp"
#include "recent-folders.hpp"

#include <obs-module.h>
#include <plugin-support.h>

#include <QAbstractItemView>
#include <QComboBox>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSignalBlocker>
#include <QTemporaryFile>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>

namespace {

constexpr int RefreshIntervalMs = 1000;
constexpr int MinimumPathLength = 12;             // characters; keeps long paths from forcing the dock wider
constexpr int ClearRecentRole = Qt::UserRole + 1; // marks the "Clear Recent Folders" entry in the list

QString Text(const char *key)
{
	return QString::fromUtf8(obs_module_text(key));
}

QString ModeText(RecordingPath::Target target)
{
	switch (target) {
	case RecordingPath::Target::AdvancedStandard:
		return Text("ExportPathDock.Mode.AdvancedStandard");
	case RecordingPath::Target::AdvancedFFmpeg:
	case RecordingPath::Target::AdvancedFFmpegUrl:
		return Text("ExportPathDock.Mode.AdvancedFFmpeg");
	case RecordingPath::Target::Simple:
	default:
		return Text("ExportPathDock.Mode.Simple");
	}
}

// Actually creating a file is the only reliable check, since Windows ACLs and network shares
// don't always match what QFileInfo::isWritable() reports.
bool IsWritable(const QString &folder)
{
	QTemporaryFile probe(QDir(folder).filePath(".export-path-dock-XXXXXX"));
	return probe.open();
}

} // namespace

ExportPathDock::ExportPathDock(QWidget *parent)
	: QWidget(parent),
	  modeLabel(new QLabel(this)),
	  pathCombo(new QComboBox(this)),
	  browseButton(new QPushButton(Text("ExportPathDock.Browse"), this)),
	  openButton(new QPushButton(Text("ExportPathDock.OpenFolder"), this)),
	  statusLabel(new QLabel(this)),
	  refreshTimer(new QTimer(this))
{
	modeLabel->setWordWrap(true);
	statusLabel->setWordWrap(true);
	statusLabel->hide();

	pathCombo->setEditable(true);
	// Typed folders get inserted into the list so Enter emits activated() just like picking an item;
	// RebuildFolderList() takes them out again.
	pathCombo->setInsertPolicy(QComboBox::InsertAtTop);
	pathCombo->setCompleter(nullptr);
	pathCombo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
	pathCombo->setMinimumContentsLength(MinimumPathLength);
	pathCombo->view()->setTextElideMode(Qt::ElideMiddle);
	pathCombo->lineEdit()->setPlaceholderText(Text("ExportPathDock.PathPlaceholder"));

	auto *buttons = new QHBoxLayout();
	buttons->addWidget(browseButton);
	buttons->addWidget(openButton);
	buttons->addStretch();

	auto *layout = new QVBoxLayout(this);
	layout->addWidget(modeLabel);
	layout->addWidget(pathCombo);
	layout->addLayout(buttons);
	layout->addWidget(statusLabel);
	layout->addStretch();

	refreshTimer->setInterval(RefreshIntervalMs);
	connect(refreshTimer, &QTimer::timeout, this, [this]() { Refresh(); });
	connect(pathCombo, &QComboBox::activated, this, [this](int index) {
		bool clearRecent = pathCombo->itemData(index, ClearRecentRole).toBool();
		QString text = pathCombo->itemText(index);
		// Wait until QComboBox is done handling the click or Enter: both branches rebuild its list,
		// and applying a folder can open a dialog.
		QTimer::singleShot(0, this, [this, clearRecent, text]() {
			if (clearRecent)
				ClearRecentFolders();
			else
				ApplyFolder(text);
		});
	});
	connect(browseButton, &QPushButton::clicked, this, &ExportPathDock::Browse);
	connect(openButton, &QPushButton::clicked, this, &ExportPathDock::OpenFolder);

	recentFolders = RecentFolders::Load();
}

void ExportPathDock::HandleFrontendEvent(enum obs_frontend_event event)
{
	switch (event) {
	case OBS_FRONTEND_EVENT_FINISHED_LOADING:
		loaded = true;
		if (isVisible())
			refreshTimer->start();
		Refresh(true);
		break;
	case OBS_FRONTEND_EVENT_PROFILE_CHANGED:
		Refresh(true);
		break;
	case OBS_FRONTEND_EVENT_RECORDING_STARTED:
	case OBS_FRONTEND_EVENT_RECORDING_STOPPED:
	case OBS_FRONTEND_EVENT_REPLAY_BUFFER_STARTED:
	case OBS_FRONTEND_EVENT_REPLAY_BUFFER_STOPPED:
		UpdateStatus();
		break;
	case OBS_FRONTEND_EVENT_EXIT:
		loaded = false;
		refreshTimer->stop();
		break;
	default:
		break;
	}
}

void ExportPathDock::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);
	if (!loaded)
		return;

	refreshTimer->start();
	Refresh(true);
}

void ExportPathDock::hideEvent(QHideEvent *event)
{
	QWidget::hideEvent(event);
	refreshTimer->stop();
}

void ExportPathDock::Refresh(bool force)
{
	if (!loaded)
		return;

	RecordingPath::Target newTarget = RecordingPath::CurrentTarget();
	QString newFolder = RecordingPath::Get(newTarget);

	if (force || newTarget != target || newFolder != currentFolder) {
		target = newTarget;
		currentFolder = newFolder;
		// Only checked when something changes: a disconnected network drive can block for seconds.
		folderExists = !currentFolder.isEmpty() && QFileInfo(currentFolder).isDir();

		bool hasFolder = RecordingPath::HasFolder(target);
		modeLabel->setText(ModeText(target));
		pathCombo->setEnabled(hasFolder);
		browseButton->setEnabled(hasFolder);
		openButton->setEnabled(hasFolder && folderExists);
		RebuildFolderList();
	}

	// Leave the text alone while the user is typing a new folder; otherwise drop unapplied edits.
	if (!pathCombo->hasFocus() && pathCombo->currentText() != QDir::toNativeSeparators(currentFolder))
		ShowCurrentFolder();

	UpdateStatus();
}

void ExportPathDock::UpdateStatus()
{
	if (!loaded)
		return;

	QStringList lines;
	if (!RecordingPath::HasFolder(target)) {
		lines << Text("ExportPathDock.Status.UrlOutput");
	} else {
		if (currentFolder.isEmpty())
			lines << Text("ExportPathDock.Status.NoFolder");
		else if (!folderExists)
			lines << Text("ExportPathDock.Status.Missing");

		if (obs_frontend_recording_active() || obs_frontend_replay_buffer_active())
			lines << Text("ExportPathDock.Status.Active");
	}

	QString text = lines.join("\n");
	if (statusLabel->text() != text)
		statusLabel->setText(text);
	statusLabel->setVisible(!lines.isEmpty());
}

void ExportPathDock::ShowCurrentFolder()
{
	QString display = QDir::toNativeSeparators(currentFolder);
	pathCombo->setEditText(display);
	pathCombo->setToolTip(display);
}

void ExportPathDock::RebuildFolderList()
{
	// The current folder is always listed first, followed by the recent ones.
	QStringList folders = recentFolders;
	if (!currentFolder.isEmpty())
		folders = RecentFolders::Add(folders, currentFolder);
	bool hasHistory = folders.size() > (currentFolder.isEmpty() ? 0 : 1);

	QSignalBlocker blocker(pathCombo);
	QString text = pathCombo->currentText();
	pathCombo->clear();

	for (const QString &folder : folders) {
		QString display = QDir::toNativeSeparators(folder);
		pathCombo->addItem(display);
		pathCombo->setItemData(pathCombo->count() - 1, display, Qt::ToolTipRole);
	}

	if (hasHistory) {
		pathCombo->insertSeparator(pathCombo->count());
		pathCombo->addItem(Text("ExportPathDock.Recent.Clear"));
		pathCombo->setItemData(pathCombo->count() - 1, true, ClearRecentRole);
	}

	pathCombo->setCurrentIndex(currentFolder.isEmpty() ? -1 : 0);
	pathCombo->setEditText(text);
}

void ExportPathDock::ClearRecentFolders()
{
	recentFolders.clear();
	RecentFolders::Save(recentFolders);
	RebuildFolderList();
	ShowCurrentFolder();
}

void ExportPathDock::ShowError(const QString &message)
{
	QMessageBox::warning(this, Text("ExportPathDock.Title"), message);
	ShowCurrentFolder();
}

void ExportPathDock::ApplyFolder(const QString &input)
{
	RebuildFolderList();
	if (!RecordingPath::HasFolder(target))
		return;

	QString folder = RecordingPath::Normalize(input);
	if (folder.isEmpty() || RecordingPath::SamePath(folder, currentFolder)) {
		ShowCurrentFolder();
		return;
	}

	QString display = QDir::toNativeSeparators(folder);
	if (!QDir::isAbsolutePath(folder)) {
		ShowError(Text("ExportPathDock.Error.NotAbsolute"));
		return;
	}

	QFileInfo info(folder);
	if (info.exists() && !info.isDir()) {
		ShowError(Text("ExportPathDock.Error.NotAFolder").arg(display));
		return;
	}

	if (!info.exists()) {
		auto answer = QMessageBox::question(this, Text("ExportPathDock.Title"),
						    Text("ExportPathDock.CreatePrompt").arg(display));
		if (answer != QMessageBox::Yes) {
			ShowCurrentFolder();
			return;
		}
		if (!QDir().mkpath(folder)) {
			ShowError(Text("ExportPathDock.Error.CreateFailed").arg(display));
			return;
		}
	}

	if (!IsWritable(folder)) {
		ShowError(Text("ExportPathDock.Error.NotWritable").arg(display));
		return;
	}

	if (!RecordingPath::Set(target, folder)) {
		ShowError(Text("ExportPathDock.Error.SaveFailed"));
		return;
	}

	obs_log(LOG_INFO, "recording folder set to '%s'", folder.toUtf8().constData());

	// Keep the folder being replaced too, so switching back is one click.
	if (!currentFolder.isEmpty())
		recentFolders = RecentFolders::Add(recentFolders, currentFolder);
	recentFolders = RecentFolders::Add(recentFolders, folder);
	RecentFolders::Save(recentFolders);

	pathCombo->clearFocus();
	Refresh(true);
}

void ExportPathDock::Browse()
{
	QString start = folderExists ? currentFolder : QDir::homePath();
	QString folder =
		QFileDialog::getExistingDirectory(this, Text("ExportPathDock.Browse.Title"), start,
						  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!folder.isEmpty())
		ApplyFolder(folder);
}

void ExportPathDock::OpenFolder()
{
	if (folderExists)
		QDesktopServices::openUrl(QUrl::fromLocalFile(currentFolder));
}
