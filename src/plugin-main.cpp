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

#include <obs-frontend-api.h>
#include <obs-module.h>
#include <plugin-support.h>

#include <QPointer>

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

namespace {

constexpr const char *DockId = "export-path-dock";

// OBS owns the dock once it's added and deletes it on shutdown; QPointer notices when that happens.
QPointer<ExportPathDock> dock;

void OnFrontendEvent(enum obs_frontend_event event, void *)
{
	if (dock)
		dock->HandleFrontendEvent(event);
}

} // namespace

const char *obs_module_name(void)
{
	return "Export Path Dock";
}

const char *obs_module_description(void)
{
	return obs_module_text("ExportPathDock.Description");
}

bool obs_module_load(void)
{
	dock = new ExportPathDock(static_cast<QWidget *>(obs_frontend_get_main_window()));
	if (!obs_frontend_add_dock_by_id(DockId, obs_module_text("ExportPathDock.Title"), dock)) {
		obs_log(LOG_ERROR, "could not add dock '%s'", DockId);
		delete dock;
		return false;
	}

	obs_frontend_add_event_callback(OnFrontendEvent, nullptr);
	obs_log(LOG_INFO, "plugin loaded successfully (version %s)", PLUGIN_VERSION);
	return true;
}

void obs_module_unload(void)
{
	obs_frontend_remove_event_callback(OnFrontendEvent, nullptr);
	obs_log(LOG_INFO, "plugin unloaded");
}
