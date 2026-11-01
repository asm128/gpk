#include "dryer_app.h"
#include "dryer_palette.h"

#include "gpk_png.h"

static const ::dry::SControlParams			control_list_hud			[]	=
	{	{DRY_MENU_COLOR_MODE		, ::dry::SCREEN_CONTROL_HUD_DIALOG		, -1								,  true, true, false, false, ::gpk::ALIGN_CENTER_TOP	, {{}, {0, 40}}, {1, 1, 1, 1}, {4, 4, 4, 4}}
	,	{::gpk::GUI_COLOR_MODE_Flat	, ::dry::SCREEN_CONTROL_HUD_DOOR		, ::dry::SCREEN_CONTROL_HUD_DIALOG	, false, true, false, false, ::gpk::ALIGN_CENTER_RIGHT	, {{ 10, 0}, {48, 40}}, {}, {}, ""				, ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, "../smart-data/icons/icon_lock-off.024.png"		, "../smart-data/icons/icon_lock-on.024.png"		, {}}
	,	{::gpk::GUI_COLOR_MODE_Flat	, ::dry::SCREEN_CONTROL_HUD_ETHERNET	, ::dry::SCREEN_CONTROL_HUD_DIALOG	, false, true, false, false, ::gpk::ALIGN_CENTER_LEFT	, {{  0, 0}, {48, 40}}, {}, {}, ""				, ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, "../smart-data/icons/icon_ethernet-off.024.png"	, "../smart-data/icons/icon_ethernet-on.024.png"	, {}}
	,	{::gpk::GUI_COLOR_MODE_Flat	, ::dry::SCREEN_CONTROL_HUD_WIFI		, ::dry::SCREEN_CONTROL_HUD_DIALOG	, false, true, false, false, ::gpk::ALIGN_CENTER_LEFT	, {{ 48, 0}, {48, 40}}, {}, {}, ""				, ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, "../smart-data/icons/icon-wifi-01.024.png"		, "../smart-data/icons/icon-wifi-01.024.png"		, {}}
	,	{::gpk::GUI_COLOR_MODE_Flat	, ::dry::SCREEN_CONTROL_HUD_RFID		, ::dry::SCREEN_CONTROL_HUD_DIALOG	, false, true, false, false, ::gpk::ALIGN_CENTER_LEFT	, {{ 96, 0}, {48, 40}}, {}, {}, ""				, ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, "../smart-data/icons/icon_rfid.024.png"			, "../smart-data/icons/icon_rfid.024.png"			, {}}
	,	{::gpk::GUI_COLOR_MODE_Flat	, ::dry::SCREEN_CONTROL_HUD_SERIAL		, ::dry::SCREEN_CONTROL_HUD_DIALOG	, false, true, false, false, ::gpk::ALIGN_CENTER_LEFT	, {{144, 0}, {48, 40}}, {}, {}, "Serial"		, ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, "", "", {true,}}
	,	{::gpk::GUI_COLOR_MODE_Flat	, ::dry::SCREEN_CONTROL_HUD_CALENDAR	, ::dry::SCREEN_CONTROL_HUD_DIALOG	, false, true, false, false, ::gpk::ALIGN_CENTER		, {{  0, 0}, {32, 40}}, {}, {}, "Jan 01 | 23:59", ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, "", "", {true,}}
	};

static const ::dry::SControlParams			control_list_menu			[]	=
	{	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MENU_DIALOG			, -1								,  true, true				, false, false, ::gpk::ALIGN_CENTER_BOTTOM	, {{}, {  0, 24}}, {0, 1, 0, 1}, {}}
	//,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MENU_HOME			, ::dry::SCREEN_CONTROL_MENU_DIALOG	, false, BACKGROUND_3D_ONLY , false, false, ::gpk::ALIGN_CENTER_LEFT	, {{0, 0}			, {128, 40}}, {2, 2, 2, 2}, {6, 6, 10, 6}, "Simplified View", ::gpk::ALIGN_CENTER}
	//,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MENU_HOME			, ::dry::SCREEN_CONTROL_MENU_DIALOG	, false, BACKGROUND_3D_ONLY , false, false, ::gpk::ALIGN_CENTER_LEFT	, {{0, 0}			, { 88, 40}}, {2, 2, 2, 2}, {6, 6, 10, 6}, "Home"			, ::gpk::ALIGN_CENTER_RIGHT, ::gpk::ALIGN_LEFT, "../smart-data/icons/icon_menu-home.024.png"	}
	//,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MENU_LOAD_FILAMENT	, ::dry::SCREEN_CONTROL_MENU_DIALOG	, false, BACKGROUND_3D_ONLY , false, false, ::gpk::ALIGN_CENTER_LEFT	, {{88, 0}			, {168, 40}}, {2, 2, 2, 2}, {6, 6, 10, 6}, "Load filament"	, ::gpk::ALIGN_CENTER_RIGHT, ::gpk::ALIGN_LEFT, "../smart-data/icons/icon_menu-filament.024.png"}
	//,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MENU_INFO			, ::dry::SCREEN_CONTROL_MENU_DIALOG	, false, BACKGROUND_3D_ONLY , false, false, ::gpk::ALIGN_CENTER_LEFT	, {{88+168, 0}		, { 88, 40}}, {2, 2, 2, 2}, {6, 6, 10, 6}, "Info"			, ::gpk::ALIGN_CENTER_RIGHT, ::gpk::ALIGN_LEFT, "../smart-data/icons/icon_menu-info.024.png"	}
	//,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MENU_CONFIG			, ::dry::SCREEN_CONTROL_MENU_DIALOG	, false, BACKGROUND_3D_ONLY , false, false, ::gpk::ALIGN_CENTER_LEFT	, {{88+168+88, 0}	, {104, 40}}, {2, 2, 2, 2}, {6, 6, 10, 6}, "Config"			, ::gpk::ALIGN_CENTER_RIGHT, ::gpk::ALIGN_LEFT, "../smart-data/icons/icon_menu-config.024.png"	}
	};

static const ::dry::SControlParams			control_list_modal			[]	=
	{	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MODAL_DIALOG		, -1								, false, false,  true,  true, ::gpk::ALIGN_CENTER		, {{}, {}}, {2, 2, 2, 2}, {}, "Close the door correctly to start drying"	, ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MODAL_ICON_START	, ::dry::SCREEN_CONTROL_MODAL_DIALOG, true,  true, false, false	, ::gpk::ALIGN_CENTER_TOP	, {{  0, 16}, {48, 48}}, {}, {}, "", ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, "../smart-data/icons/icon_play.032.png"}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MODAL_ICON_STOP		, ::dry::SCREEN_CONTROL_MODAL_DIALOG, true,  true, false, false	, ::gpk::ALIGN_CENTER_TOP	, {{  0, 16}, {48, 48}}, {}, {}, "", ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, "../smart-data/icons/icon_stop.032.png"}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MODAL_YES			, ::dry::SCREEN_CONTROL_MODAL_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_BOTTOM, {{-64, 24}, {72, 40}}, {3, 3, 3, 3}, {}, "Yes"	, ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MODAL_NO			, ::dry::SCREEN_CONTROL_MODAL_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_BOTTOM, {{ 64, 24}, {72, 40}}, {3, 3, 3, 3}, {}, "No"		, ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MODAL_CLOSE			, ::dry::SCREEN_CONTROL_MODAL_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_BOTTOM, {{  0, 24}, {72, 40}}, {3, 3, 3, 3}, {}, "OK"		, ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MODAL_LOAD			, ::dry::SCREEN_CONTROL_MODAL_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_BOTTOM, {{-96, 24}, {72, 40}}, {3, 3, 3, 3}, {}, "Load"	, ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MODAL_UNLOAD		, ::dry::SCREEN_CONTROL_MODAL_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_BOTTOM, {{-96, 24}, {72, 40}}, {3, 3, 3, 3}, {}, "Unload"	, ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_MODAL_CANCEL		, ::dry::SCREEN_CONTROL_MODAL_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_BOTTOM, {{ 96, 24}, {72, 40}}, {3, 3, 3, 3}, {}, "Cancel"	, ::gpk::ALIGN_CENTER}
	};

static const ::dry::SControlParams			control_list_spool_info		[]	=
	{ 	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG			, -1,  true, true, true, true, ::gpk::ALIGN_CENTER_TOP}
	, 	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_TITLE			, ::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG, true, false, true, true, ::gpk::ALIGN_CENTER_TOP, {{0, 24 * 0		}, { 64, 30}}, {3, 3, 3, 3}, {}, "Spool Scanned!", ::gpk::ALIGN_CENTER}
	, 	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_BRAND		, ::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_TOP, {{0, 24 * 1 + 6	}, {256, 24}}, {1, 1, 1, 1}, {2,}, "Brand"		, ::gpk::ALIGN_CENTER_LEFT}
	, 	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_MATERIAL	, ::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_TOP, {{0, 24 * 2 + 6	}, {256, 24}}, {1, 1, 1, 1}, {2,}, "Material"	, ::gpk::ALIGN_CENTER_LEFT}
	, 	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_COLOR		, ::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_TOP, {{0, 24 * 3 + 6	}, {256, 24}}, {1, 1, 1, 1}, {2,}, "Color"		, ::gpk::ALIGN_CENTER_LEFT}
	, 	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_SPOOL_SIZE	, ::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_TOP, {{0, 24 * 4 + 6	}, {256, 24}}, {1, 1, 1, 1}, {2,}, "Spool Size"		, ::gpk::ALIGN_CENTER_LEFT}
	, 	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_DENSITY	, ::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_TOP, {{0, 24 * 5 + 6	}, {256, 24}}, {1, 1, 1, 1}, {2,}, "Density"	, ::gpk::ALIGN_CENTER_LEFT}
	, 	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_DIAMETER	, ::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_TOP, {{0, 24 * 6 + 6	}, {256, 24}}, {1, 1, 1, 1}, {2,}, "Diameter"	, ::gpk::ALIGN_CENTER_LEFT}
	, 	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_NOTES		, ::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG, false, false, false, false, ::gpk::ALIGN_CENTER_TOP, {{0, 24 * 7 + 6	}, {256, 24}}, {1, 1, 1, 1}, {2,}, "Notes"	, ::gpk::ALIGN_CENTER_LEFT}
	, 	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_BRAND		, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_BRAND		, true, false, true, false, ::gpk::ALIGN_CENTER_RIGHT, {{}, {72, 22}}, {1, 1, 1, 1}, {}, "Kodak"				, ::gpk::ALIGN_CENTER}
	, 	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_MATERIAL	, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_MATERIAL	, true, false, true, false, ::gpk::ALIGN_CENTER_RIGHT, {{}, {72, 22}}, {1, 1, 1, 1}, {}, "ABS"					, ::gpk::ALIGN_CENTER}
	, 	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_COLOR		, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_COLOR		, true, false, true, false, ::gpk::ALIGN_CENTER_RIGHT, {{}, {72, 22}}, {1, 1, 1, 1}, {}, "Red"					, ::gpk::ALIGN_CENTER}
	, 	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_NOTES		, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_NOTES		, true, false, true, false, ::gpk::ALIGN_CENTER_RIGHT, {{}, {72, 22}}, {1, 1, 1, 1}, {}, "Filament prototype"	, ::gpk::ALIGN_CENTER}
	, 	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_DENSITY	, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_DENSITY	, true, false, true, false, ::gpk::ALIGN_CENTER_RIGHT, {{}, {72, 22}}, {1, 1, 1, 1}, {}, "?"					, ::gpk::ALIGN_CENTER}
	, 	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_DIAMETER	, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_DIAMETER	, true, false, true, false, ::gpk::ALIGN_CENTER_RIGHT, {{}, {72, 22}}, {1, 1, 1, 1}, {}, "1.75mm"				, ::gpk::ALIGN_CENTER}
	, 	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_SPOOL_SIZE	, ::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_SPOOL_SIZE	, true, false, true, false, ::gpk::ALIGN_CENTER_RIGHT, {{}, {72, 22}}, {1, 1, 1, 1}, {}, "750g"					, ::gpk::ALIGN_CENTER}
	};

static const ::dry::SControlParams			control_list_load_filament	[]	=
	{	{::gpk::GUI_COLOR_MODE_Flat, ::dry::SCREEN_CONTROL_LOAD_FILAMENT_DIALOG	, -1,  true, true}
	};

static const ::dry::SControlParams			control_list_info			[]	=
	{	{::gpk::GUI_COLOR_MODE_Flat, ::dry::SCREEN_CONTROL_INFO_DIALOG	, -1,  true, true}
	};

static const ::dry::SControlParams			control_list_config			[]	=
	{	{::gpk::GUI_COLOR_MODE_Flat, ::dry::SCREEN_CONTROL_CONFIG_DIALOG, -1,  true, true}
	};

static	::gpk::error_t						setupScreenHUD				(::dry::SApplication & app)	{
	::dry::setupScreenFromControlParams(*app.Framework.GUI, app.Screens.RootControlIndex, app.Screens.HUD , ::control_list_hud);
	return ::dry::handleResizeScreenHUD (app, app.Framework.RootWindow.Size.s1_t());
}

static	::gpk::error_t						setupScreenMenu				(::dry::SApplication & app)	{
	::dry::setupScreenFromControlParams(*app.Framework.GUI, app.Screens.RootControlIndex, app.Screens.Menu, ::control_list_menu);
	const ::gpk::cid_t								indexControlMenuItem		= app.Screens.Menu.ControlIds[::dry::SCREEN_CONTROL_MENU_HOME];
	if(-1 == indexControlMenuItem)
		return 1;
	app.Framework.GUI->Controls.SetSelected(indexControlMenuItem, true);
	return ::dry::handleResizeScreenMenu(app, app.Framework.RootWindow.Size.s1_t());
}
static	::gpk::error_t						setupScreenSpoolInfo		(::dry::SApplication & app)	{
	::gpk::SGUI								& gui						= *app.Framework.GUI;
	::dry::SScreenSpoolInfo					& screen					= app.Screens.SpoolInfo;
	::dry::setupScreenFromControlParams(*app.Framework.GUI, app.Screens.RootControlIndex, screen, ::control_list_spool_info);
	::gpk::controlSetParent(*app.Framework.GUI, screen.ControlIds[::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG], app.Screens.Modal.ControlIds[::dry::SCREEN_CONTROL_MODAL_DIALOG]);
	gui.Controls.SetHidden(screen.ControlIds[::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG], true);
	return ::dry::handleResizeScreenSpoolInfo(app, app.Framework.RootWindow.Size.s1_t());
}

static	::gpk::error_t						setupScreenModal			(::dry::SApplication & app)	{
	::dry::setupScreenFromControlParams(*app.Framework.GUI, app.Screens.RootControlIndex, app.Screens.Modal, ::control_list_modal);
	const ::dry::SCREEN_CONTROL_MODAL				controlsToHide	[]			=
		{ ::dry::SCREEN_CONTROL_MODAL_DIALOG
		, ::dry::SCREEN_CONTROL_MODAL_ICON_STOP
		, ::dry::SCREEN_CONTROL_MODAL_YES
		, ::dry::SCREEN_CONTROL_MODAL_NO
		};
	for(uint32_t iControl = 0; iControl < ::gpk::size(controlsToHide); ++iControl) {
		const ::gpk::cid_t								indexControl			= app.Screens.Modal.ControlIds[controlsToHide[iControl]];
		if(indexControl == -1)
			continue;
		app.Framework.GUI->Controls.SetHidden(indexControl, true);
	}
	return ::dry::handleResizeScreenModal(app, app.Framework.RootWindow.Size.s1_t());
}

::gpk::error_t								dry::setupScreenLoadFilament		(::dry::SApplication & app)	{
	::dry::setupScreenFromControlParams(*app.Framework.GUI, app.Screens.RootControlIndex, app.Screens.LoadFilament, ::control_list_load_filament);
	const ::dry::SCREEN_CONTROL_LOAD_FILAMENT		controlsToHide	[]			=
		{ ::dry::SCREEN_CONTROL_LOAD_FILAMENT_DIALOG
		};
	for(uint32_t iControl = 0; iControl < ::gpk::size(controlsToHide); ++iControl) {
		const ::gpk::cid_t								indexControl			= app.Screens.LoadFilament.ControlIds[controlsToHide[iControl]];
		if(indexControl == -1)
			continue;
		app.Framework.GUI->Controls.SetHidden(indexControl, true);
	}
	return ::dry::handleResizeScreenHome(app, app.Framework.RootWindow.Size.s1_t());
}

::gpk::error_t								dry::setupScreenInfo		(::dry::SApplication & app)	{
	::dry::setupScreenFromControlParams(*app.Framework.GUI, app.Screens.RootControlIndex, app.Screens.Info, ::control_list_info);
	const ::dry::SCREEN_CONTROL_INFO				controlsToHide	[]			=
		{ ::dry::SCREEN_CONTROL_INFO_DIALOG
		};
	for(uint32_t iControl = 0; iControl < ::gpk::size(controlsToHide); ++iControl) {
		const ::gpk::cid_t								indexControl			= app.Screens.Info.ControlIds[controlsToHide[iControl]];
		if(indexControl == -1)
			continue;
		app.Framework.GUI->Controls.SetHidden(indexControl, true);
	}
	return ::dry::handleResizeScreenHome(app, app.Framework.RootWindow.Size.s1_t());
}

::gpk::error_t								dry::setupScreenConfig		(::dry::SApplication & app)	{
	::dry::setupScreenFromControlParams(*app.Framework.GUI, app.Screens.RootControlIndex, app.Screens.Config, ::control_list_config);
	const ::dry::SCREEN_CONTROL_CONFIG				controlsToHide	[]			=
		{ ::dry::SCREEN_CONTROL_CONFIG_DIALOG
		};
	for(uint32_t iControl = 0; iControl < ::gpk::size(controlsToHide); ++iControl) {
		const ::gpk::cid_t								indexControl			= app.Screens.Config.ControlIds[controlsToHide[iControl]];
		if(indexControl == -1)
			continue;
		app.Framework.GUI->Controls.SetHidden(indexControl, true);
	}
	return ::dry::handleResizeScreenHome(app, app.Framework.RootWindow.Size.s1_t());
}

static	::gpk::error_t						setupScreens				(::dry::SApplication & app)	{
	::gpk::SGUI										& gui						= *app.Framework.GUI;
	app.Screens.RootControlIndex				= ::gpk::controlCreate(gui);
	info_printf("Created root control with index %i.", app.Screens.RootControlIndex);
	::gpk::SControlPlacement						& rootControl				= gui.Controls.Placement[app.Screens.RootControlIndex];
	::gpk::SControlDraw								& controlDraw				= gui.Controls.Draw		[app.Screens.RootControlIndex];
	rootControl.Area.Size						= app.Framework.RootWindow.Size.s1_t();
	controlDraw.NoBorder						= true;
	//controlMode.NoBackgroundRect				= true;
	rootControl.Border							= {};
	rootControl.Margin							= {};
	if_fail_e(::gpk::failed(::setupScreenHUD				(app)));
	if_fail_e(::gpk::failed(::setupScreenMenu				(app)));
	if_fail_e(::gpk::failed(::dry::setupScreenHome			(app)));
	if_fail_e(::gpk::failed(::dry::setupScreenLoadFilament	(app)));
	if_fail_e(::gpk::failed(::dry::setupScreenInfo			(app)));
	if_fail_e(::gpk::failed(::dry::setupScreenConfig			(app)));
	if_fail_e(::gpk::failed(::setupScreenModal				(app)));
	if_fail_e(::gpk::failed(::setupScreenSpoolInfo			(app)));
	return 0;
}


static	::gpk::error_t						setupGauge					(::dry::SGaugeFeeder	& feeder, const ::gpk::n2s1_t feederControlSize)	{
	const ::gpk::circlef32							gaugeMetrics				= {feederControlSize.x / 2.0f - 4, feederControlSize.f2_t() / 2.0f};
//#define GAUGE_LOW_RES
#if defined(GAUGE_LOW_RES)
	const uint32_t									resolution					= 12;
#else
	const uint32_t									resolution					= 64;
#endif
	::dry::gaugeBuildRadial(feeder, gaugeMetrics, resolution, 8);
	feeder.Image.resize(feederControlSize.u2_t());
	::dry::gaugeImageUpdate(feeder);
	return 0;
}

static	::gpk::error_t						setupGaugeControl			(::dry::SApplication & app, ::dry::SGaugeFeeder & feeder, int32_t feederControlId)	{
	::gpk::SGUI										& gui						= *app.Framework.GUI;
	const int32_t									indexControlFeeder			= app.Screens.Home.ControlIds[feederControlId];
	::gpk::SControlPlacement						& control					= gui.Controls.Placement[indexControlFeeder];
	const ::gpk::n2s1_t								feederControlSize			= control.Area.Size;
	::setupGauge(feeder, feederControlSize);
	gui.Controls.Images[indexControlFeeder].Image		= feeder.Image;
	return 0;
}

static	::gpk::error_t						setupGauges					(::dry::SApplication & app)	{
	for(uint32_t iFeeder = 0; iFeeder < ::gpk::size(app.Screens.Home.GaugeFeederSmall); ++iFeeder)
		::setupGaugeControl(app, app.Screens.Home.GaugeFeederSmall[iFeeder], ::dry::SCREEN_CONTROL_HOME_FEEDER_0_SMALL + iFeeder);

	::setupGaugeControl(app, app.Screens.Home.GaugeDryerSmall.Gauges[::dry::GAUGE_TEMPERATURE], ::dry::SCREEN_CONTROL_HOME_DRYER_SMALL);
	return 0;
}

::gpk::error_t								dry::setupGUI				(::dry::SApplication & app)	{
	auto											& framework					= app.Framework;
	::gpk::SGUI										& gui						= *framework.GUI;
	gui.ColorModeDefault						= ::gpk::GUI_COLOR_MODE_Flat;

	gui.ThemeDefault							= 7 * 16 + 11;
	//gui.ThemeDefault							= 18 * 16 + 10;

	gpk_necall(::setupScreens(app), "%s", "Failed to initialize screens.");
	gpk_necall(::dry::setupPalettes(gui), "%s", "Failed to set up palettes.");
	gpk_necall(::setupGauges(app), "%s", "Failed to initialize screens.");

	::gpk::pobj<::gpk::rt<::gpk::bgra, uint32_t>>
												& guiRenderTarget			= framework.RootWindow.BackBuffer;
	const auto								& guiRenderTargetMetrics	= guiRenderTarget->Color.View.metrics();
	gpk_necall(::gpk::guiUpdateMetrics(gui, guiRenderTargetMetrics.u1_t(), true), "Failed to update GUI metrics to {w: %i, h: %i}.", guiRenderTargetMetrics.x, guiRenderTargetMetrics.y);
	info_printf("Render target size: [%u x %u]", guiRenderTargetMetrics.x, guiRenderTargetMetrics.y);
	return 0;
}
