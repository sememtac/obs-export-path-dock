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

#include <QStringList>

// Recently used recording folders, newest first. Stored in the plugin's own config directory so
// the list is shared by every OBS profile.
namespace RecentFolders {

QStringList Load();
void Save(const QStringList &folders);

// Moves (or inserts) folder to the front and drops the oldest entries past the limit.
QStringList Add(QStringList folders, const QString &folder);

} // namespace RecentFolders
