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

#include <QString>

namespace RecordingPath {

// Which recording folder setting OBS is currently using. Each output mode keeps its own.
enum class Target {
	Simple,            // Output mode: Simple
	AdvancedStandard,  // Output mode: Advanced, recording type "Standard"
	AdvancedFFmpeg,    // Output mode: Advanced, recording type "Custom Output (FFmpeg)" writing to a file
	AdvancedFFmpegUrl, // Output mode: Advanced, recording type "Custom Output (FFmpeg)" sending to a URL
};

Target CurrentTarget();
bool HasFolder(Target target);

// Reads/writes the folder in the active profile. OBS reads it again every time a recording or the
// replay buffer starts, so a new folder applies to the next recording.
QString Get(Target target);
bool Set(Target target, const QString &folder);

QString Normalize(const QString &folder);
bool SamePath(const QString &a, const QString &b);

} // namespace RecordingPath
