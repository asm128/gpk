#include "dryer_app.h"

static const ::dry::SControlParams	control_list_home[]				=
	{	{::gpk::GUI_COLOR_MODE_Flat, ::dry::SCREEN_CONTROL_HOME_DIALOG	, -1								,  true, true}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_GROUP_PROGRAM	, ::dry::SCREEN_CONTROL_HOME_DIALOG,  true, false, true, true	, ::gpk::ALIGN_TOP_LEFT			, {{}, {168, 222}}			, {1, 1, 1, 1}, {0, 0, 0, 2}}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_GROUP_GAUGE	, ::dry::SCREEN_CONTROL_HOME_DIALOG,  true, false, true, true	, ::gpk::ALIGN_TOP_RIGHT		, {{}, {272, 166}}			, {1, 1, 1, 1}, {2, 2, 2, 2}}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_GROUP_SESSION	, ::dry::SCREEN_CONTROL_HOME_DIALOG,  true, false, true, true	, ::gpk::ALIGN_BOTTOM_LEFT		, {{}, {272, 24 * 8 + 2}}	, {1, 1, 1, 1}, {}}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_GROUP_STATS	, ::dry::SCREEN_CONTROL_HOME_DIALOG,  true, false, true, true	, ::gpk::ALIGN_CENTER_BOTTOM	, {{}, {}}					, {1, 1, 1, 1}, {1, 1, 1, 1}}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_GROUP_SPOOL	, ::dry::SCREEN_CONTROL_HOME_DIALOG,  true, false, true, true	, ::gpk::ALIGN_BOTTOM_RIGHT		, {{}, {272, 120}}			, {1, 1, 1, 1}, {1, 1, 1, 1}}
	//,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_GROUP_ERROR	, ::dry::SCREEN_CONTROL_HOME_DIALOG,  true, false, true, false	, ::gpk::ALIGN_CENTER_BOTTOM	, {{}, {}}					, {1, 1, 1, 1}, {1, 1, 1, 1}}
	//
	,	{DRY_MENU_COLOR_MODE		, ::dry::SCREEN_CONTROL_HOME_PROGRAM_START			, ::dry::SCREEN_CONTROL_HOME_GROUP_PROGRAM	, false, BACKGROUND_3D_ONLY, false, false, ::gpk::ALIGN_CENTER_BOTTOM	, {{}, {176, 72}}, {3, 3, 3, 3}, {0, 8, 0, 8}, "Start Drying", ::gpk::ALIGN_CENTER_BOTTOM}
	,	{DRY_MENU_COLOR_MODE		, ::dry::SCREEN_CONTROL_HOME_PROGRAM_STOP			, ::dry::SCREEN_CONTROL_HOME_GROUP_PROGRAM	, false, BACKGROUND_3D_ONLY, false, false, ::gpk::ALIGN_CENTER_BOTTOM	, {{}, {176, 72}}, {3, 3, 3, 3}, {0, 8, 0, 8}, "Stop Drying", ::gpk::ALIGN_CENTER_BOTTOM}
	,	{::gpk::GUI_COLOR_MODE_Flat	, ::dry::SCREEN_CONTROL_HOME_PROGRAM_START_ICON		, ::dry::SCREEN_CONTROL_HOME_PROGRAM_START	,  true, true, false, false, ::gpk::ALIGN_CENTER_TOP					, {{}, { 32, 32}}, {}, {}, ""				, ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, "../smart-data/icons/icon_play.032.png"}
	,	{::gpk::GUI_COLOR_MODE_Flat	, ::dry::SCREEN_CONTROL_HOME_PROGRAM_STOP_ICON		, ::dry::SCREEN_CONTROL_HOME_PROGRAM_STOP	,  true, true, false, false, ::gpk::ALIGN_CENTER_TOP					, {{}, { 32, 32}}, {}, {}, ""				, ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER, "../smart-data/icons/icon_stop.032.png"}
	//
	,	{::gpk::GUI_COLOR_MODE_Flat	, ::dry::SCREEN_CONTROL_HOME_FEEDER_0_SMALL			, ::dry::SCREEN_CONTROL_HOME_GROUP_GAUGE	, false, true, false, false, ::gpk::ALIGN_TOP_LEFT		, {{0,  0}, {112,112}}, {}, {}, ""				, ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER}
	,	{::gpk::GUI_COLOR_MODE_Flat	, ::dry::SCREEN_CONTROL_HOME_FEEDER_1_SMALL			, ::dry::SCREEN_CONTROL_HOME_GROUP_GAUGE	, false, true, false, false, ::gpk::ALIGN_TOP_RIGHT		, {{0,  0}, {112,112}}, {}, {}, ""				, ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER}
	,	{::gpk::GUI_COLOR_MODE_Flat	, ::dry::SCREEN_CONTROL_HOME_DRYER_SMALL			, ::dry::SCREEN_CONTROL_HOME_GROUP_GAUGE	, false, true, false, false, ::gpk::ALIGN_CENTER_BOTTOM	, {{0,  0}, {144,144}}, {}, {}, ""				, ::gpk::ALIGN_CENTER, ::gpk::ALIGN_CENTER}
	,	{::gpk::GUI_COLOR_MODE_Flat	, ::dry::SCREEN_CONTROL_HOME_LABEL_DRYER_SMALL		, ::dry::SCREEN_CONTROL_HOME_DRYER_SMALL	,  true, true, false, false, ::gpk::ALIGN_CENTER		, {{0,-16}, {128, 32}}, {}, {}, "Temperature"	, ::gpk::ALIGN_CENTER}
	,	{::gpk::GUI_COLOR_MODE_Flat	, ::dry::SCREEN_CONTROL_HOME_LABEL_FEEDER_0_SMALL	, ::dry::SCREEN_CONTROL_HOME_FEEDER_0_SMALL	,  true, true, false, false, ::gpk::ALIGN_CENTER		, {{0,-16}, {128, 32}}, {}, {}, "Pressure"		, ::gpk::ALIGN_CENTER}
	,	{::gpk::GUI_COLOR_MODE_Flat	, ::dry::SCREEN_CONTROL_HOME_LABEL_FEEDER_1_SMALL	, ::dry::SCREEN_CONTROL_HOME_FEEDER_1_SMALL	,  true, true, false, false, ::gpk::ALIGN_CENTER		, {{0,-16}, {128, 32}}, {}, {}, "Humidity"		, ::gpk::ALIGN_CENTER}
	//
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_SESSION_STATUS_LABEL		, ::dry::SCREEN_CONTROL_HOME_GROUP_SESSION				,  true, true, false, false, ::gpk::ALIGN_TOP_LEFT		, {{0, 24 * 0}, {272, 48}}, {2, 2, 2, 2}, {24, 2, 0, 2}	, "STATUS:"						, ::gpk::ALIGN_CENTER_LEFT	}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_ELAPSED_TIME_LABEL			, ::dry::SCREEN_CONTROL_HOME_GROUP_SESSION				,  true, true, false, false, ::gpk::ALIGN_TOP_LEFT		, {{0, 24 * 2}, {272, 24}}, {1, 1, 1, 1}, {4, 2, 0, 2}	, "Elapsed time (total)"		, ::gpk::ALIGN_CENTER_LEFT	}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_PRE_PREHEATING_TIME_LABEL	, ::dry::SCREEN_CONTROL_HOME_GROUP_SESSION				,  true, true, false, false, ::gpk::ALIGN_TOP_LEFT		, {{0, 24 * 3}, {272, 24}}, {1, 1, 1, 1}, {4, 2, 0, 2}	, "Elapsed time (warming)"		, ::gpk::ALIGN_CENTER_LEFT	}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_PREHEATING_TIME_LABEL		, ::dry::SCREEN_CONTROL_HOME_GROUP_SESSION				,  true, true, false, false, ::gpk::ALIGN_TOP_LEFT		, {{0, 24 * 4}, {272, 24}}, {1, 1, 1, 1}, {4, 2, 0, 2}	, "Elapsed time (preheating)"	, ::gpk::ALIGN_CENTER_LEFT	}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_DRYING_TIME_LABEL			, ::dry::SCREEN_CONTROL_HOME_GROUP_SESSION				,  true, true, false, false, ::gpk::ALIGN_TOP_LEFT		, {{0, 24 * 5}, {272, 24}}, {1, 1, 1, 1}, {4, 2, 0, 2}	, "Elapsed time (pump)"			, ::gpk::ALIGN_CENTER_LEFT	}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_REMAINING_TIME_LABEL		, ::dry::SCREEN_CONTROL_HOME_GROUP_SESSION				,  true, true, false, false, ::gpk::ALIGN_TOP_LEFT		, {{0, 24 * 6}, {272, 24}}, {1, 1, 1, 1}, {4, 2, 0, 2}	, "Estimated remaining time"	, ::gpk::ALIGN_CENTER_LEFT	}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_CURRENT_CYCLE_LABEL		, ::dry::SCREEN_CONTROL_HOME_GROUP_SESSION				,  true, true, false, false, ::gpk::ALIGN_TOP_LEFT		, {{0, 24 * 7}, {272, 24}}, {1, 1, 1, 1}, {4, 2, 0, 2}	, "Current cycle"				, ::gpk::ALIGN_CENTER_LEFT	}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_SESSION_STATUS_VALUE		, ::dry::SCREEN_CONTROL_HOME_SESSION_STATUS_LABEL		,  true, true,  true, false, ::gpk::ALIGN_CENTER_RIGHT	, {{0,  0}, {168, 44}}, {2, 2, 2, 2}, {}, "READY"	, ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_ELAPSED_TIME_TIME			, ::dry::SCREEN_CONTROL_HOME_ELAPSED_TIME_LABEL			,  true, true,  true, false, ::gpk::ALIGN_CENTER_RIGHT	, {{0,  0}, { 64, 22}}, {2, 2, 2, 2}, {}, "00:00m"	, ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_PRE_PREHEATING_TIME_TIME	, ::dry::SCREEN_CONTROL_HOME_PRE_PREHEATING_TIME_LABEL	,  true, true,  true, false, ::gpk::ALIGN_CENTER_RIGHT	, {{0,  0}, { 64, 22}}, {2, 2, 2, 2}, {}, "00:00m"	, ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_PREHEATING_TIME_TIME		, ::dry::SCREEN_CONTROL_HOME_PREHEATING_TIME_LABEL		,  true, true,  true, false, ::gpk::ALIGN_CENTER_RIGHT	, {{0,  0}, { 64, 22}}, {2, 2, 2, 2}, {}, "00:00m"	, ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_DRYING_TIME_TIME			, ::dry::SCREEN_CONTROL_HOME_DRYING_TIME_LABEL			,  true, true,  true, false, ::gpk::ALIGN_CENTER_RIGHT	, {{0,  0}, { 64, 22}}, {2, 2, 2, 2}, {}, "00:00m"	, ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_REMAINING_TIME_TIME		, ::dry::SCREEN_CONTROL_HOME_REMAINING_TIME_LABEL		,  true, true,  true, false, ::gpk::ALIGN_CENTER_RIGHT	, {{0,  0}, { 64, 22}}, {2, 2, 2, 2}, {}, "00:00Hs"	, ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_CURRENT_CYCLE_NUMBER		, ::dry::SCREEN_CONTROL_HOME_CURRENT_CYCLE_LABEL		,  true, true,  true, false, ::gpk::ALIGN_CENTER_RIGHT	, {{0,  0}, { 64, 22}}, {2, 2, 2, 2}, {}, "1"		, ::gpk::ALIGN_CENTER}
	//
	// 10438147 ft
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_NAME						, ::dry::SCREEN_CONTROL_HOME_GROUP_PROGRAM	,  true, true, false, false, ::gpk::ALIGN_TOP_LEFT	, {{0,24 * 0}, {0, 24}}, {1, 1, 1, 1}, {4, }, "Program:"				, ::gpk::ALIGN_CENTER_LEFT}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_TEMPERATURE				, ::dry::SCREEN_CONTROL_HOME_GROUP_PROGRAM	,  true, true, false, false, ::gpk::ALIGN_TOP_LEFT	, {{0,24 * 1}, {0, 24}}, {1, 1, 1, 1}, {4, }, "Temperature"				, ::gpk::ALIGN_CENTER_LEFT}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_PRESSURE					, ::dry::SCREEN_CONTROL_HOME_GROUP_PROGRAM	,  true, true, false, false, ::gpk::ALIGN_TOP_LEFT	, {{0,24 * 2}, {0, 24}}, {1, 1, 1, 1}, {4, }, "Pressure"				, ::gpk::ALIGN_CENTER_LEFT}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_PRE_HEATING_TIME			, ::dry::SCREEN_CONTROL_HOME_GROUP_PROGRAM	,  true, true, false, false, ::gpk::ALIGN_TOP_LEFT	, {{0,24 * 3}, {0, 24}}, {1, 1, 1, 1}, {4, }, "Pre-heating time"		, ::gpk::ALIGN_CENTER_LEFT}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_SUBCYCLE_DURATION		, ::dry::SCREEN_CONTROL_HOME_GROUP_PROGRAM	,  true, true, false, false, ::gpk::ALIGN_TOP_LEFT	, {{0,24 * 4}, {0, 24}}, {1, 1, 1, 1}, {4, }, "Drying cycle time"		, ::gpk::ALIGN_CENTER_LEFT}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_SUBCYCLES				, ::dry::SCREEN_CONTROL_HOME_GROUP_PROGRAM	,  true, true, false, false, ::gpk::ALIGN_TOP_LEFT	, {{0,24 * 5}, {0, 24}}, {1, 1, 1, 1}, {4, }, "Cycles"					, ::gpk::ALIGN_CENTER_LEFT}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_VALUE_NAME				, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_NAME				,   true, true,  true, false, ::gpk::ALIGN_CENTER_RIGHT	, {{0,0}, {176, 22}}, {2, 2, 2, 2}, {}, "default"	, ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_VALUE_TEMPERATURE		, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_TEMPERATURE		,   true, true,  true, false, ::gpk::ALIGN_CENTER_RIGHT	, {{0,0}, { 80, 22}}, {2, 2, 2, 2}, {}, "45"		, ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_VALUE_PRESSURE			, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_PRESSURE			,   true, true,  true, false, ::gpk::ALIGN_CENTER_RIGHT	, {{0,0}, { 80, 22}}, {2, 2, 2, 2}, {}, "600"		, ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_VALUE_PRE_HEATING_TIME	, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_PRE_HEATING_TIME	,   true, true,  true, false, ::gpk::ALIGN_CENTER_RIGHT	, {{0,0}, { 80, 22}}, {2, 2, 2, 2}, {}, "10"		, ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_VALUE_SUBCYCLE_DURATION	, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_SUBCYCLE_DURATION,   true, true,  true, false, ::gpk::ALIGN_CENTER_RIGHT	, {{0,0}, { 80, 22}}, {2, 2, 2, 2}, {}, "40"		, ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_VALUE_SUBCYCLES			, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_SUBCYCLES		,   true, true,  true, false, ::gpk::ALIGN_CENTER_RIGHT	, {{0,0}, { 80, 22}}, {2, 2, 2, 2}, {}, "3"			, ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_PROGRAM_PREV							, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_VALUE_NAME		,  false, true, false, false, ::gpk::ALIGN_CENTER_LEFT	, {{0,0}, { 32, 24}}, {3, 3, 3, 3}, {}, "<"			, ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_PROGRAM_NEXT							, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_VALUE_NAME		,  false, true, false, false, ::gpk::ALIGN_CENTER_RIGHT	, {{0,0}, { 32, 24}}, {3, 3, 3, 3}, {}, ">"			, ::gpk::ALIGN_CENTER}
	//
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_SPOOL_LIST								, ::dry::SCREEN_CONTROL_HOME_GROUP_SPOOL	,  true,  true,  true, false, ::gpk::ALIGN_CENTER_BOTTOM, {{0, 0}, {272, 24}}, {2, 2, 2, 2}, }
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_SPOOL_LIST_TITLE						, ::dry::SCREEN_CONTROL_HOME_GROUP_SPOOL	,  true,  true, false, false, ::gpk::ALIGN_CENTER_TOP	, {{0, 0}, {272, 24}}, {2, 2, 2, 2}, {}, "Loaded Spools", ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_SPOOL_SCROLLABLE						, ::dry::SCREEN_CONTROL_HOME_SPOOL_LIST		,  true,  true,  true, false, ::gpk::ALIGN_TOP_LEFT		, {{0, 0}, {272, 24}}, }
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_SPOOL_SCROLLBAR						, ::dry::SCREEN_CONTROL_HOME_SPOOL_LIST		, false, false,  true, false, ::gpk::ALIGN_TOP_RIGHT	, {{0, 0}, {32, 24}}, {1, 1, 1, 1}, {1, 1, 1, 1}}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_SPOOL_LIST_PREV						, ::dry::SCREEN_CONTROL_HOME_SPOOL_SCROLLBAR, false, false, false, false, ::gpk::ALIGN_CENTER_TOP	, {{0, 0}, {24, 24}}, {2, 2, 2, 2}, {}, "\x1e", ::gpk::ALIGN_CENTER}
	,	{DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_HOME_SPOOL_LIST_NEXT						, ::dry::SCREEN_CONTROL_HOME_SPOOL_SCROLLBAR, false, false, false, false, ::gpk::ALIGN_CENTER_BOTTOM, {{0, 0}, {24, 24}}, {2, 2, 2, 2}, {}, "\x1f", ::gpk::ALIGN_CENTER}
};

::gpk::error_t						dry::setupScreenHome			(::dry::SApplication & app)	{
	::gpk::SGUI								& gui							= *app.Framework.GUI;
	::dry::setupScreenFromControlParams(*app.Framework.GUI, app.Screens.RootControlIndex, app.Screens.Home, ::control_list_home);
	const ::dry::SCREEN_CONTROL_HOME				controlsToHide	[]		=
		{ ::dry::SCREEN_CONTROL_HOME_PROGRAM_STOP
		//, ::dry::SCREEN_CONTROL_HOME_CURRENT_CYCLE_TIME_LABEL
		};
	for(uint32_t iControl = 0; iControl < ::gpk::size(controlsToHide); ++iControl) {
		const ::gpk::cid_t								indexControl			= app.Screens.Home.ControlIds[controlsToHide[iControl]];
		if(indexControl == -1)
			continue;
		gui.Controls.SetHidden(indexControl, true);
	}
	::dry::updateProgramDisplay(app);
	::dry::updateCatalogDisplay(app);
	return ::dry::handleResizeScreenHome(app, app.Framework.RootWindow.Size.s1_t());
}
