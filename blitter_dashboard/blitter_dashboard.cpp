#include "blitter_dashboard.h"
#include "gpk_bitmap_target.h"
#include "gpk_string.h"

#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::blt::SBlitterDashboard, "Blitter Dashboard");

using namespace ::gpk;

namespace
{
	struct SDashboardCommand {
		::gpk::vcst_t		Label;
		::blt::DASHBOARD_COMMAND	Command;
	};

	static const SDashboardCommand DASHBOARD_COMMANDS[] =
		{ {"Load configuration", ::blt::DASHBOARD_COMMAND_Load}
		, {"Refresh"           , ::blt::DASHBOARD_COMMAND_Refresh}
		, {"Flush"             , ::blt::DASHBOARD_COMMAND_Flush}
		, {"Exit"              , ::blt::DASHBOARD_COMMAND_Exit}
		};

	static ::gpk::vcst_t hostLabel(::blt::DATABASE_HOST host) {
		switch(host) {
		default:
		case ::blt::DATABASE_HOST_LOCAL			: return "local";
		case ::blt::DATABASE_HOST_REMOTE			: return "remote";
		case ::blt::DATABASE_HOST_DEFLATE		: return "local+deflate";
		case ::blt::DATABASE_HOST_REMOTE_DEFLATE	: return "remote+deflate";
		}
	}

	static ::gpk::err_t statusSet(::blt::SBlitterDashboard & app, ::gpk::vcst_t text) {
		app.StatusText = text;
		return ::gpk::controlTextSet(app.Framework.GUI->Controls, app.IdStatus, app.StatusText);
	}

	static ::gpk::err_t databaseRowsUpdate(::blt::SBlitterDashboard & app) {
		::gpk::SGUI			& gui		= *app.Framework.GUI;
		const ::gpk::u2_t		rowCount	= ::gpk::max(1U, app.Blitter.Databases.size()) + 1;
		if_fail_fe(app.DatabaseRowText.resize(rowCount));
		app.DatabaseRowText[0] = "DATABASE                 HOST               BLOCK      LOADED      DISK      DIRTY MAP      ENCRYPTED";
		if(0 == app.Blitter.Databases.size())
			app.DatabaseRowText[1] = "No Blitter configuration loaded.";
		else
			for(::gpk::u2_t iDatabase = 0; iDatabase < app.Blitter.Databases.size(); ++iDatabase) {
				const ::blt::TNamedBlitterDB	& namedDatabase	= app.Blitter.Databases[iDatabase];
				const ::blt::SDatabase			& database		= namedDatabase.Val;
				char						line[512]		= {};
				::sprintf_s(line, sizeof(line), "%-24.*s %-18.*s %10u %11u %9u %14u %14s"
					, GPK_VIEW_ARGS(namedDatabase.Key)
					, GPK_VIEW_ARGS(::hostLabel(database.HostType))
					, (unsigned)database.BlockSize
					, (unsigned)database.Blocks.size()
					, (unsigned)database.BlocksOnDisk.size()
					, (unsigned)database.BlockDirty.size()
					, database.EncryptionKey.size() ? "yes" : "no"
					);
				app.DatabaseRowText[iDatabase + 1] = line;
			}

		for(::gpk::u2_t iRow = 0; iRow < rowCount; ++iRow) {
			if(iRow >= app.DatabaseRows.IdControls.size()) {
				if_fail_fe(::gpk::controlListPush(gui, app.DatabaseRows, app.DatabaseRowText[iRow]));
			}
			else {
				if_fail_fe(::gpk::controlTextSet(gui.Controls, app.DatabaseRows.IdControls[iRow], app.DatabaseRowText[iRow]));
			}
			gui.Controls.SetHidden(app.DatabaseRows.IdControls[iRow], false);
		}
		for(::gpk::u2_t iRow = rowCount; iRow < app.DatabaseRows.IdControls.size(); ++iRow)
			gui.Controls.SetHidden(app.DatabaseRows.IdControls[iRow], true);

		if_fail_fe(::gpk::controlListArrange(gui, app.DatabaseRows));
		gui.Controls.Placement[app.DatabaseRows.IdControl].Area.Size.x = 1120;
		return 0;
	}

	static ::gpk::err_t configurationLoad(::blt::SBlitterDashboard & app) {
		::gpk::vcst_t configPath = app.ConfigInput.Text;
		::gpk::trim(configPath);
		if(0 == configPath.size())
			return ::statusSet(app, "Enter a Blitter configuration path before loading.");

		const ::gpk::err_t result = ::blt::loadConfig(app.Blitter, configPath);
		char status[512] = {};
		if(::gpk::failed(result))
			::sprintf_s(status, sizeof(status), "Load failed (%i): %.*s", result, GPK_VIEW_ARGS(configPath));
		else
			::sprintf_s(status, sizeof(status), "Loaded %u database(s) from %.*s. Database root: %.*s"
				, (unsigned)app.Blitter.Databases.size()
				, GPK_VIEW_ARGS(configPath)
				, GPK_VIEW_ARGS(app.Blitter.Folder)
				);
		if_fail_fe(::statusSet(app, status));
		return ::databaseRowsUpdate(app);
	}

	static ::gpk::err_t databasesFlush(::blt::SBlitterDashboard & app) {
		if(0 == app.Blitter.Databases.size())
			return ::statusSet(app, "No loaded databases to flush.");
		const ::gpk::err_t result = ::blt::blitterFlush(app.Blitter);
		char status[256] = {};
		if(::gpk::failed(result))
			::sprintf_s(status, sizeof(status), "Flush failed (%i).", result);
		else
			::sprintf_s(status, sizeof(status), "Flushed %u database(s).", (unsigned)app.Blitter.Databases.size());
		if_fail_fe(::statusSet(app, status));
		return ::databaseRowsUpdate(app);
	}

	static ::gpk::err_t dashboardCommand(::blt::SBlitterDashboard & app, ::gpk::cid_t idControl) {
		for(::gpk::u2_t iCommand = 0; iCommand < app.Commands.IdControls.size(); ++iCommand) {
			if(idControl != app.Commands.IdControls[iCommand])
				continue;
			switch((::blt::DASHBOARD_COMMAND)app.Commands.Events[iCommand]) {
			default: return 0;
			case ::blt::DASHBOARD_COMMAND_Load		: return ::configurationLoad(app);
			case ::blt::DASHBOARD_COMMAND_Refresh	: return ::databaseRowsUpdate(app);
			case ::blt::DASHBOARD_COMMAND_Flush		: return ::databasesFlush(app);
			case ::blt::DASHBOARD_COMMAND_Exit		: return APPLICATION_STATE_EXIT;
			}
		}
		return 0;
	}
}

::gpk::err_t cleanup(::blt::SBlitterDashboard & app) {
	return ::gpk::mainWindowDestroy(app.Framework.RootWindow);
}

::gpk::err_t setup(::blt::SBlitterDashboard & app) {
	::gpk::SFramework	& framework	= app.Framework;
	::gpk::SWindow		& mainWindow	= framework.RootWindow;
	framework.GUI						= app.DialogMain.GUI;
	app.DialogMain.Input					= mainWindow.Input;
	mainWindow.Size						= {1280, 720};
	if_true_e(::gpk::failed(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)));

	::gpk::SGUI			& gui		= *framework.GUI;
	::gpk::SControlTable	& controls	= gui.Controls;
	gui.ColorModeDefault					= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault						= ::gpk::ASCII_COLOR_Dark_green * 16 + 7;

	if_fail_fe(app.IdTitle = ::gpk::controlCreate(gui));
	controls.Placement[app.IdTitle].Area		= {{24, 18}, {480, 24}};
	controls.Text[app.IdTitle].Align			= ::gpk::ALIGN_CENTER_LEFT;
	if_fail_fe(::gpk::controlTextSet(gui.Controls, app.IdTitle, "BLITTER DASHBOARD"));

	if_fail_fe(::gpk::controlListInitialize(gui, app.Commands));
	app.Commands.Orientation					= ::gpk::DIRECTION_RIGHT;
	controls.Placement[app.Commands.IdControl].Area.Offset = {24, 52};
	for(const SDashboardCommand & command : DASHBOARD_COMMANDS)
		if_fail_fe(::gpk::controlListPush(gui, app.Commands, command.Label, command.Command));

	if_fail_fe(app.IdStatus = ::gpk::controlCreate(gui));
	controls.Placement[app.IdStatus].Area		= {{24, 82}, {1180, 22}};
	controls.Text[app.IdStatus].Align			= ::gpk::ALIGN_CENTER_LEFT;

	::gpk::pobj<::gpk::SDialogViewport> viewport = {};
	if_fail_fe(app.Viewport = ::gpk::viewportCreate(app.DialogMain, viewport));
	controls.Placement[viewport->IdGUIControl].Area.Size	= {1180, 360};
	controls.Placement[viewport->IdGUIControl].Area.Offset	= {0, 116};
	controls.Placement[viewport->IdGUIControl].Align		= ::gpk::ALIGN_CENTER_TOP;
	if_fail_fe(::gpk::controlTextSet(gui.Controls, viewport->IdTitle, "DATABASE STATE"));

	if_fail_fe(::gpk::controlListInitialize(gui, app.DatabaseRows));
	app.DatabaseRows.Orientation					= ::gpk::DIRECTION_DOWN;
	controls.Placement[app.DatabaseRows.IdControl].Area.Offset = {8, 8};
	controls.Placement[app.DatabaseRows.IdControl].Area.Size.x = 1120;
	if_fail_fe(::gpk::controlSetParent(gui.Controls, app.DatabaseRows.IdControl, viewport->IdClient));

	if_fail_fe(::gpk::inputBoxCreate(app.ConfigInput, gui));
	app.ConfigInput.MaxLength					= 1024;
	controls.Placement[app.ConfigInput.IdRoot].Align		= ::gpk::ALIGN_CENTER_BOTTOM;
	controls.Placement[app.ConfigInput.IdRoot].Area.Offset	= {};
	if_fail_fe(app.ConfigInput.SetText(gui, "./blitter.json"));
	if_fail_fe(app.ConfigInput.Edit(gui, true));

	if_fail_fe(::statusSet(app, "Enter a Blitter configuration path and press Enter or Load configuration."));
	return ::databaseRowsUpdate(app);
}

::gpk::err_t update(::blt::SBlitterDashboard & app, bool exitSignal) {
	if_true_vi(APPLICATION_STATE_EXIT, exitSignal);
	{
		::gpk::lock_guard lock(app.LockRender);
		app.Framework.RootWindow.BackBuffer = app.Offscreen;
	}
	if_true_vi(APPLICATION_STATE_EXIT, APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework));

	::gpk::SGUI	& gui = *app.Framework.GUI;
	::gpk::acid	controlsToProcess = {};
	if_fail_fe(::gpk::controlActions(gui.Controls, controlsToProcess));
	if(const ::gpk::s2_t inputResult = app.ConfigInput.Update(gui, app.ConfigInput.VirtualKeyboard, app.Framework.RootWindow.EventQueue, controlsToProcess))
		if(INT_MAX == inputResult)
			if_fail_fe(::configurationLoad(app));

	::gpk::err_t commandResult = 0;
	if_fail_fe(::gpk::controlHandle(gui.Controls, controlsToProcess, [&app, &commandResult](::gpk::cid_t idControl) {
		for(::gpk::u2_t iCommand = 0; iCommand < app.Commands.IdControls.size(); ++iCommand)
			if(idControl == app.Commands.IdControls[iCommand]) {
				commandResult = ::dashboardCommand(app, idControl);
				return 1;
			}
		return 0;
	}));
	if(APPLICATION_STATE_EXIT == commandResult)
		return APPLICATION_STATE_EXIT;
	if_fail_fe(commandResult);

	return app.DialogMain.Update();
}

::gpk::err_t draw(::blt::SBlitterDashboard & app) {
	::gpk::pobj<::gpk::rtbgra8d32> target = {};
	if_true_fe(0 == target.create());
	if_fail_fe(target->resize(app.Framework.RootWindow.Size, ::gpk::LIGHTGRAY, 0xFFFFFFFFU));
	{
		::gpk::lock_guard lock(app.Framework.LockGUI);
		if_fail_fe(::gpk::guiDraw(*app.DialogMain.GUI, target->Color.View));
	}
	{
		::gpk::lock_guard lock(app.LockRender);
		app.Offscreen = target;
	}
	return 0;
}
