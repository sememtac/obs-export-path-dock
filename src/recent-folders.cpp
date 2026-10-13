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

#include "recent-folders.hpp"
#include "recording-path.hpp"

#include <obs-module.h>
#include <util/platform.h>

namespace RecentFolders {

namespace {

constexpr qsizetype MaxFolders = 10;
constexpr const char *FileName = "recent-folders.json";

} // namespace

QStringList Load()
{
	QStringList folders;

	char *path = obs_module_config_path(FileName);
	obs_data_t *data = obs_data_create_from_json_file_safe(path, "bak");
	bfree(path);
	if (!data)
		return folders;

	obs_data_array_t *array = obs_data_get_array(data, "folders");
	size_t count = obs_data_array_count(array);
	for (size_t i = 0; i < count; i++) {
		obs_data_t *item = obs_data_array_item(array, i);
		QString folder = QString::fromUtf8(obs_data_get_string(item, "path"));
		if (!folder.isEmpty() && folders.size() < MaxFolders)
			folders.append(folder);
		obs_data_release(item);
	}
	obs_data_array_release(array);
	obs_data_release(data);
	return folders;
}

void Save(const QStringList &folders)
{
	char *dir = obs_module_config_path("");
	os_mkdirs(dir);
	bfree(dir);

	obs_data_t *data = obs_data_create();
	obs_data_array_t *array = obs_data_array_create();
	for (const QString &folder : folders) {
		obs_data_t *item = obs_data_create();
		obs_data_set_string(item, "path", folder.toUtf8().constData());
		obs_data_array_push_back(array, item);
		obs_data_release(item);
	}
	obs_data_set_array(data, "folders", array);

	char *path = obs_module_config_path(FileName);
	obs_data_save_json_safe(data, path, "tmp", "bak");
	bfree(path);

	obs_data_array_release(array);
	obs_data_release(data);
}

QStringList Add(QStringList folders, const QString &folder)
{
	folders.removeIf([&folder](const QString &existing) { return RecordingPath::SamePath(existing, folder); });
	folders.prepend(folder);
	while (folders.size() > MaxFolders)
		folders.removeLast();
	return folders;
}

} // namespace RecentFolders
