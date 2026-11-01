#include "gpk_tactical_gui_app.h"
#include "gpk_gui_control_list.h"

static const ::dry::SControlParams control_list_tactical[] =
	{ {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_DIALOG			, -1												,  true, true , false, false, ::gpk::ALIGN_CENTER	, {{}, {640, 440}}, {1, 1, 1, 1}, {4, 4, 4, 4}}
	, {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_TITLE			, ::dry::SCREEN_CONTROL_TACTICAL_DIALOG				,  true, false,  true, false, ::gpk::ALIGN_CENTER_TOP	, {{0,  8}, {632, 40}}, {2, 2, 2, 2}, {6, 4, 6, 4}, "TACTICAL COMMAND CENTER", ::gpk::ALIGN_CENTER}
	, {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_MENU_GROUP		, ::dry::SCREEN_CONTROL_TACTICAL_DIALOG				,  true, false,  true, false, ::gpk::ALIGN_TOP_LEFT	, {{8, 56}, {208, 360}}, {2, 2, 2, 2}, {4, 4, 4, 4}}
	, {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_DETAIL_GROUP	, ::dry::SCREEN_CONTROL_TACTICAL_DIALOG				,  true, false,  true, false, ::gpk::ALIGN_TOP_LEFT	, {{224, 56}, {408, 224}}, {2, 2, 2, 2}, {8, 8, 8, 8}}
	, {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_DETAIL_TITLE	, ::dry::SCREEN_CONTROL_TACTICAL_DETAIL_GROUP		,  true, true ,  true, false, ::gpk::ALIGN_CENTER_TOP	, {{0,  0}, {392, 32}}, {1, 1, 1, 1}, {4, 2, 4, 2}, "Start Mission", ::gpk::ALIGN_CENTER_LEFT}
	, {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_DETAIL_LINE_0	, ::dry::SCREEN_CONTROL_TACTICAL_DETAIL_GROUP		,  true, true ,  true, false, ::gpk::ALIGN_TOP_LEFT	, {{0, 40}, {392, 24}}, {0, 0, 0, 0}, {4, 2, 4, 2}, "Review the squad and deploy to the tactical field.", ::gpk::ALIGN_CENTER_LEFT}
	, {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_DETAIL_LINE_1	, ::dry::SCREEN_CONTROL_TACTICAL_DETAIL_GROUP		,  true, true ,  true, false, ::gpk::ALIGN_TOP_LEFT	, {{0, 68}, {392, 24}}, {0, 0, 0, 0}, {4, 2, 4, 2}, "Available agents: 6", ::gpk::ALIGN_CENTER_LEFT}
	, {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_DETAIL_LINE_2	, ::dry::SCREEN_CONTROL_TACTICAL_DETAIL_GROUP		,  true, true ,  true, false, ::gpk::ALIGN_TOP_LEFT	, {{0, 96}, {392, 24}}, {0, 0, 0, 0}, {4, 2, 4, 2}, "Squad maintenance: 1,200 credits", ::gpk::ALIGN_CENTER_LEFT}
	, {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_DETAIL_LINE_3	, ::dry::SCREEN_CONTROL_TACTICAL_DETAIL_GROUP		,  true, true ,  true, false, ::gpk::ALIGN_TOP_LEFT	, {{0,124}, {392, 24}}, {0, 0, 0, 0}, {4, 2, 4, 2}, "Mission reward: 4,000 credits", ::gpk::ALIGN_CENTER_LEFT}
	, {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_DETAIL_LINE_4	, ::dry::SCREEN_CONTROL_TACTICAL_DETAIL_GROUP		,  true, true ,  true, false, ::gpk::ALIGN_TOP_LEFT	, {{0,152}, {392, 24}}, {0, 0, 0, 0}, {4, 2, 4, 2}, "Select an operation from the command list.", ::gpk::ALIGN_CENTER_LEFT}
	, {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_BALANCE_GROUP	, ::dry::SCREEN_CONTROL_TACTICAL_DIALOG				,  true, false,  true, false, ::gpk::ALIGN_TOP_LEFT	, {{224,288}, {408, 128}}, {2, 2, 2, 2}, {8, 8, 8, 8}}
	, {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_BALANCE_TITLE	, ::dry::SCREEN_CONTROL_TACTICAL_BALANCE_GROUP		,  true, true ,  true, false, ::gpk::ALIGN_CENTER_TOP	, {{0, 0}, {392, 28}}, {0, 0, 0, 0}, {4, 2, 4, 2}, "HEADQUARTERS BALANCE", ::gpk::ALIGN_CENTER_LEFT}
	, {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_PRODUCTION_LABEL	, ::dry::SCREEN_CONTROL_TACTICAL_BALANCE_GROUP		,  true, true ,  true, false, ::gpk::ALIGN_TOP_LEFT	, {{0,36}, {152, 28}}, {0, 0, 0, 0}, {4, 2, 4, 2}, "Production budget", ::gpk::ALIGN_CENTER_LEFT}
	, {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_PRODUCTION_DECREASE, ::dry::SCREEN_CONTROL_TACTICAL_BALANCE_GROUP		, false, false, false, false, ::gpk::ALIGN_TOP_LEFT	, {{160,36}, {32, 28}}, {2, 2, 2, 2}, {}, "-", ::gpk::ALIGN_CENTER}
	, {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_PRODUCTION_VALUE	, ::dry::SCREEN_CONTROL_TACTICAL_BALANCE_GROUP		,  true, false,  true, false, ::gpk::ALIGN_TOP_LEFT	, {{196,36}, {72, 28}}, {2, 2, 2, 2}, {}, "50%", ::gpk::ALIGN_CENTER}
	, {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_PRODUCTION_INCREASE, ::dry::SCREEN_CONTROL_TACTICAL_BALANCE_GROUP		, false, false, false, false, ::gpk::ALIGN_TOP_LEFT	, {{272,36}, {32, 28}}, {2, 2, 2, 2}, {}, "+", ::gpk::ALIGN_CENTER}
	, {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_RESEARCH_LABEL	, ::dry::SCREEN_CONTROL_TACTICAL_BALANCE_GROUP		,  true, true ,  true, false, ::gpk::ALIGN_TOP_LEFT	, {{0,72}, {152, 28}}, {0, 0, 0, 0}, {4, 2, 4, 2}, "Research budget", ::gpk::ALIGN_CENTER_LEFT}
	, {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_RESEARCH_DECREASE	, ::dry::SCREEN_CONTROL_TACTICAL_BALANCE_GROUP		, false, false, false, false, ::gpk::ALIGN_TOP_LEFT	, {{160,72}, {32, 28}}, {2, 2, 2, 2}, {}, "-", ::gpk::ALIGN_CENTER}
	, {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_RESEARCH_VALUE	, ::dry::SCREEN_CONTROL_TACTICAL_BALANCE_GROUP		,  true, false,  true, false, ::gpk::ALIGN_TOP_LEFT	, {{196,72}, {72, 28}}, {2, 2, 2, 2}, {}, "50%", ::gpk::ALIGN_CENTER}
	, {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_RESEARCH_INCREASE	, ::dry::SCREEN_CONTROL_TACTICAL_BALANCE_GROUP		, false, false, false, false, ::gpk::ALIGN_TOP_LEFT	, {{272,72}, {32, 28}}, {2, 2, 2, 2}, {}, "+", ::gpk::ALIGN_CENTER}
	, {DRY_MENU_COLOR_MODE, ::dry::SCREEN_CONTROL_TACTICAL_STATUS			, ::dry::SCREEN_CONTROL_TACTICAL_BALANCE_GROUP		,  true, true ,  true, false, ::gpk::ALIGN_BOTTOM_RIGHT, {{0, 4}, {80, 24}}, {0, 0, 0, 0}, {4, 2, 4, 2}, "READY", ::gpk::ALIGN_CENTER}
	};

::gpk::error_t dry::setupScreenTactical(::dry::SApplication & app) {
	::gpk::SGUI & gui = *app.Framework.GUI;
	::dry::SScreenTactical & screen = app.Screens.Tactical;
	::dry::setupScreenFromControlParams(gui, app.Screens.RootControlIndex, screen, ::control_list_tactical);

	static const ::gpk::vcsc_t menuLabels[] =
		{ ::gpk::vcs{"Start Mission"}
		, ::gpk::vcs{"Set Up Squad"}
		, ::gpk::vcs{"Visit Labs"}
		, ::gpk::vcs{"Upgrade Center"}
		, ::gpk::vcs{"Factory"}
		, ::gpk::vcs{"Buy / Sell"}
		, ::gpk::vcs{"Memorial"}
		};
	::gpk::SControlPlacement menuItemPlacement = {};
	menuItemPlacement.Align = ::gpk::ALIGN_TOP_LEFT;
	menuItemPlacement.Area = {{4, 4}, {192, 36}};
	menuItemPlacement.Border = {2, 2, 2, 2};
	menuItemPlacement.Margin = {4, 2, 4, 2};
	::gpk::vcid menuControlIds = {screen.ControlIds.begin() + ::dry::SCREEN_CONTROL_TACTICAL_MENU_START_MISSION, ::dry::TACTICAL_SECTION_COUNT};
	gpk_necall(::gpk::guiCreateControlList(gui, ::gpk::DIRECTION_DOWN, menuLabels, screen.ControlIds[::dry::SCREEN_CONTROL_TACTICAL_MENU_GROUP], menuItemPlacement, ::gpk::ALIGN_CENTER_LEFT, menuControlIds), "%s", "Failed to create tactical command list.");

	gui.Controls.SetHidden(app.Screens.Home.ControlIds[::dry::SCREEN_CONTROL_HOME_DIALOG], true);
	gui.Controls.SetSelected(screen.ControlIds[::dry::SCREEN_CONTROL_TACTICAL_MENU_START_MISSION], true);
	gpk_necall(::dry::updateScreenTactical(app), "%s", "Failed to initialize tactical screen text.");
	return ::dry::handleResizeScreenTactical(app, app.Framework.RootWindow.Size.s1_t());
}
