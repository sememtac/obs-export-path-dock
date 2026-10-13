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

#include "recording-path.hpp"

#include <obs-frontend-api.h>
#include <util/config-file.h>
#include <util/dstr.h>

#include <QDir>

namespace RecordingPath {

namespace {

struct ConfigKey {
	const char *section;
	const char *name;
};

// Keys in the profile's basic.ini, matching OBS's SimpleOutput.cpp and AdvancedOutput.cpp.
ConfigKey KeyFor(Target target)
{
	switch (target) {
	case Target::AdvancedStandard:
		return {"AdvOut", "RecFilePath"};
	case Target::AdvancedFFmpeg:
	case Target::AdvancedFFmpegUrl:
		return {"AdvOut", "FFFilePath"};
	case Target::Simple:
	default:
		return {"SimpleOutput", "FilePath"};
	}
}

} // namespace

Target CurrentTarget()
{
	config_t *config = obs_frontend_get_profile_config();
	if (!config)
		return Target::Simple;

	if (astrcmpi(config_get_string(config, "Output", "Mode"), "Advanced") != 0)
		return Target::Simple;

	if (astrcmpi(config_get_string(config, "AdvOut", "RecType"), "FFmpeg") != 0)
		return Target::AdvancedStandard;

	return config_get_bool(config, "AdvOut", "FFOutputToFile") ? Target::AdvancedFFmpeg : Target::AdvancedFFmpegUrl;
}

bool HasFolder(Target target)
{
	return target != Target::AdvancedFFmpegUrl;
}

QString Get(Target target)
{
	config_t *config = obs_frontend_get_profile_config();
	if (!config)
		return QString();

	ConfigKey key = KeyFor(target);
	return QString::fromUtf8(config_get_string(config, key.section, key.name));
}

bool Set(Target target, const QString &folder)
{
	config_t *config = obs_frontend_get_profile_config();
	if (!config || !HasFolder(target))
		return false;

	ConfigKey key = KeyFor(target);
	config_set_string(config, key.section, key.name, folder.toUtf8().constData());
	return config_save_safe(config, "tmp", nullptr) == CONFIG_SUCCESS;
}

QString Normalize(const QString &folder)
{
	return QDir::cleanPath(QDir::fromNativeSeparators(folder.trimmed()));
}

bool SamePath(const QString &a, const QString &b)
{
#ifdef _WIN32
	return Normalize(a).compare(Normalize(b), Qt::CaseInsensitive) == 0;
#else
	return Normalize(a) == Normalize(b);
#endif
}

} // namespace RecordingPath
