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

#pragma once

#include "recording-path.hpp"

#include <obs-frontend-api.h>

#include <QStringList>
#include <QWidget>

class QComboBox;
class QLabel;
class QPushButton;
class QThreadPool;
class QTimer;

class ExportPathDock : public QWidget {
	Q_OBJECT

public:
	explicit ExportPathDock(QWidget *parent = nullptr);
	~ExportPathDock() override;

	void HandleFrontendEvent(enum obs_frontend_event event);

protected:
	void showEvent(QShowEvent *event) override;
	void hideEvent(QHideEvent *event) override;

private:
	// Re-reads the folder from OBS. Polled because OBS has no event for "settings changed", so
	// this is how edits made in Settings > Output show up here.
	void Refresh(bool force = false);
	// Checks in the background that the folder still exists, so a drive that disconnects or
	// reconnects shows up within a refresh.
	void CheckFolder();
	void FolderChecked(const QString &folder, bool exists);
	void UpdateStatus();
	void ShowCurrentFolder();
	void ShowError(const QString &message);
	void RebuildFolderList();

	void ApplyFolder(const QString &input);
	void Browse();
	void OpenFolder();
	void ClearRecentFolders();

	QLabel *modeLabel;
	QComboBox *pathCombo; // editable: shows the current folder, drops down to recent ones
	QPushButton *browseButton;
	QPushButton *openButton;
	QLabel *statusLabel;
	QTimer *refreshTimer;
	QThreadPool *folderCheckPool;

	bool loaded = false;
	RecordingPath::Target target = RecordingPath::Target::Simple;
	QString currentFolder; // as stored in the OBS profile
	bool folderExists = false;
	bool folderCheckPending = false;
	QStringList recentFolders;
};
