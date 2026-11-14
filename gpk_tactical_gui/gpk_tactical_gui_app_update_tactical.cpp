#include "gpk_tactical_gui_app.h"

static const ::dry::STacticalMenuItem<::dry::TACTICAL_SECTION> tacticalMenuItems[] =
	{ {::dry::TACTICAL_SECTION_START_MISSION	, ::gpk::vcst_t{"Start Mission"}}
	, {::dry::TACTICAL_SECTION_SET_UP_SQUAD		, ::gpk::vcst_t{"Set Up Squad"}}
	, {::dry::TACTICAL_SECTION_LABS				, ::gpk::vcst_t{"Visit Labs"}}
	, {::dry::TACTICAL_SECTION_UPGRADE_CENTER	, ::gpk::vcst_t{"Upgrade Center"}}
	, {::dry::TACTICAL_SECTION_FACTORY			, ::gpk::vcst_t{"Factory"}}
	, {::dry::TACTICAL_SECTION_BUY_AND_SELL		, ::gpk::vcst_t{"Buy / Sell"}}
	, {::dry::TACTICAL_SECTION_MEMORIAL			, ::gpk::vcst_t{"Memorial"}}
	};

static ::gpk::err_t tacticalDetails(::dry::TACTICAL_SECTION section, ::gpk::vcsc_t (&lines)[5]) {
	switch(section) {
	default:
	case ::dry::TACTICAL_SECTION_START_MISSION:
		lines[0] = "Review the squad and deploy to the tactical field.";
		lines[1] = "Available agents: 6";
		lines[2] = "Squad maintenance: 1,200 credits";
		lines[3] = "Mission reward: 4,000 credits";
		lines[4] = "Select an operation from the command list.";
		break;
	case ::dry::TACTICAL_SECTION_SET_UP_SQUAD:
		lines[0] = "Compose the active squad from available agents.";
		lines[1] = "Slots in use: 4 / 6";
		lines[2] = "Front line: assault and shield specialists";
		lines[3] = "Support: medic and field engineer";
		lines[4] = "Agent details remain bound to typed selections.";
		break;
	case ::dry::TACTICAL_SECTION_LABS:
		lines[0] = "Inspect research categories and active projects.";
		lines[1] = "Weapons: plasma containment";
		lines[2] = "Armor: reactive composite";
		lines[3] = "Facilities: field laboratory";
		lines[4] = "Research allocation is controlled below.";
		break;
	case ::dry::TACTICAL_SECTION_UPGRADE_CENTER:
		lines[0] = "Apply completed research to agents and equipment.";
		lines[1] = "Available upgrades: 3";
		lines[2] = "Pending equipment: 5";
		lines[3] = "Unassigned upgrade points: 12";
		lines[4] = "Choose an item to open its detail pane.";
		break;
	case ::dry::TACTICAL_SECTION_FACTORY:
		lines[0] = "Queue production from completed designs.";
		lines[1] = "Capacity: 4 simultaneous jobs";
		lines[2] = "Active jobs: 2";
		lines[3] = "Next completion: 00:42";
		lines[4] = "Production allocation is controlled below.";
		break;
	case ::dry::TACTICAL_SECTION_BUY_AND_SELL:
		lines[0] = "Browse typed equipment categories and inventory.";
		lines[1] = "Credits available: 24,800";
		lines[2] = "Market categories: 9";
		lines[3] = "Inventory slots used: 18 / 64";
		lines[4] = "Labels remain independent of semantic values.";
		break;
	case ::dry::TACTICAL_SECTION_MEMORIAL:
		lines[0] = "Remember agents lost in tactical operations.";
		lines[1] = "Recorded agents: 3";
		lines[2] = "Battles represented: 27";
		lines[3] = "Successful evacuations: 11";
		lines[4] = "Their service remains part of command history.";
		break;
	}
	return 0;
}

::gpk::err_t dry::updateScreenTactical(::dry::SApplication & app) {
	::gpk::SGUI & gui = *app.Framework.GUI;
	::dry::SScreenTactical & screen = app.Screens.Tactical;
	const ::dry::STacticalMenuItem<::dry::TACTICAL_SECTION> & selected = ::tacticalMenuItems[(u2_t)screen.SectionSelected];
	{ if_fail_fef(::gpk::controlTextSet(gui, screen.ControlIds[::dry::SCREEN_CONTROL_TACTICAL_DETAIL_TITLE], selected.Text), "%s", "Failed to set tactical detail title."); }
	::gpk::vcsc_t detailLines[5] = {};
	::tacticalDetails(screen.SectionSelected, detailLines);
	for(u2_t iLine = 0; iLine < ::gpk::size(detailLines); ++iLine)
		{ if_fail_fef(::gpk::controlTextSet(gui, screen.ControlIds[::dry::SCREEN_CONTROL_TACTICAL_DETAIL_LINE_0 + iLine], detailLines[iLine]), "line: %u", iLine); }

	const s2_t productionLength = snprintf(screen.ProductionText.begin(), screen.ProductionText.size(), "%i%%", screen.ProductionBudget);
	const s2_t researchLength = snprintf(screen.ResearchText.begin(), screen.ResearchText.size(), "%i%%", screen.ResearchBudget);
	{ if_fail_fef(::gpk::controlTextSet(gui, screen.ControlIds[::dry::SCREEN_CONTROL_TACTICAL_PRODUCTION_VALUE], {screen.ProductionText.begin(), (u2_t)productionLength}), "%s", "Failed to set production allocation."); }
	{ if_fail_fef(::gpk::controlTextSet(gui, screen.ControlIds[::dry::SCREEN_CONTROL_TACTICAL_RESEARCH_VALUE], {screen.ResearchText.begin(), (u2_t)researchLength}), "%s", "Failed to set research allocation."); }
	app.Framework.RootWindow.Repaint = true;
	return 0;
}

::gpk::err_t dry::handleControlTactical(::dry::SApplication & app, s2_t idControl) {
	::dry::SScreenTactical & screen = app.Screens.Tactical;
	for(u2_t iItem = 0; iItem < ::gpk::size(::tacticalMenuItems); ++iItem)
		if(screen.ControlIds[::dry::SCREEN_CONTROL_TACTICAL_MENU_START_MISSION + iItem] == idControl) {
			screen.SectionSelected = ::tacticalMenuItems[iItem].Value;
			for(u2_t iMenu = 0; iMenu < ::gpk::size(::tacticalMenuItems); ++iMenu)
				app.Framework.GUI->Controls.SetSelected(screen.ControlIds[::dry::SCREEN_CONTROL_TACTICAL_MENU_START_MISSION + iMenu], iMenu == iItem);
			return ::dry::updateScreenTactical(app);
		}

	switch((u2_t)idControl) {
	case ::dry::SCREEN_CONTROL_TACTICAL_PRODUCTION_DECREASE: screen.ProductionBudget = ::gpk::max(0, screen.ProductionBudget - 5); break;
	case ::dry::SCREEN_CONTROL_TACTICAL_PRODUCTION_INCREASE: screen.ProductionBudget = ::gpk::min(100, screen.ProductionBudget + 5); break;
	case ::dry::SCREEN_CONTROL_TACTICAL_RESEARCH_DECREASE: screen.ResearchBudget = ::gpk::max(0, screen.ResearchBudget - 5); break;
	case ::dry::SCREEN_CONTROL_TACTICAL_RESEARCH_INCREASE: screen.ResearchBudget = ::gpk::min(100, screen.ResearchBudget + 5); break;
	default: return 0;
	}
	return ::dry::updateScreenTactical(app);
}

::gpk::err_t dry::handleResizeScreenTactical(::dry::SApplication & app, ::gpk::n2s1_t newSize) {
	::gpk::SControlTable & controls = app.Framework.GUI->Controls;
	const ::dry::SScreenTactical & screen = app.Screens.Tactical;
	const s1_t heightMenu = (-1 == app.Screens.Menu.ControlIds[::dry::SCREEN_CONTROL_MENU_DIALOG]) ? 0 : controls.Placement[app.Screens.Menu.ControlIds[::dry::SCREEN_CONTROL_MENU_DIALOG]].Area.Size.y;
	const s1_t heightHUD = controls.Placement[app.Screens.HUD.ControlIds[::dry::SCREEN_CONTROL_HUD_DIALOG]].Area.Size.y;
	const s1_t finalHeight = s1_t(newSize.y - heightHUD - heightMenu);
	const s1_t menuWidth = 216;
	const s1_t contentX = s1_t(menuWidth + 8);
	const s1_t contentWidth = s1_t(newSize.x - contentX - 8);
	const s1_t balanceHeight = 128;

	controls.Placement[screen.ControlIds[::dry::SCREEN_CONTROL_TACTICAL_DIALOG]].Area = {{0, heightHUD}, {newSize.x, finalHeight}};
	controls.Placement[screen.ControlIds[::dry::SCREEN_CONTROL_TACTICAL_TITLE]].Area.Size.x = s1_t(newSize.x - 8);
	controls.Placement[screen.ControlIds[::dry::SCREEN_CONTROL_TACTICAL_MENU_GROUP]].Area.Size.y = s1_t(finalHeight - 64);
	controls.Placement[screen.ControlIds[::dry::SCREEN_CONTROL_TACTICAL_DETAIL_GROUP]].Area = {{contentX, 56}, {contentWidth, s1_t(finalHeight - 56 - balanceHeight - 8)}};
	controls.Placement[screen.ControlIds[::dry::SCREEN_CONTROL_TACTICAL_BALANCE_GROUP]].Area = {{contentX, s1_t(finalHeight - balanceHeight)}, {contentWidth, balanceHeight}};

	const ::dry::SCREEN_CONTROL_TACTICAL stretchControls[] =
		{ ::dry::SCREEN_CONTROL_TACTICAL_DETAIL_TITLE
		, ::dry::SCREEN_CONTROL_TACTICAL_DETAIL_LINE_0
		, ::dry::SCREEN_CONTROL_TACTICAL_DETAIL_LINE_1
		, ::dry::SCREEN_CONTROL_TACTICAL_DETAIL_LINE_2
		, ::dry::SCREEN_CONTROL_TACTICAL_DETAIL_LINE_3
		, ::dry::SCREEN_CONTROL_TACTICAL_DETAIL_LINE_4
		, ::dry::SCREEN_CONTROL_TACTICAL_BALANCE_TITLE
		};
	for(u2_t iControl = 0; iControl < ::gpk::size(stretchControls); ++iControl)
		controls.Placement[screen.ControlIds[stretchControls[iControl]]].Area.Size.x = s1_t(contentWidth - 16);
	for(u2_t iControl = 0; iControl < ::dry::SCREEN_CONTROL_TACTICAL_COUNT; ++iControl)
		if(screen.ControlIds[iControl] != -1)
			controls.SetUpdated(screen.ControlIds[iControl], false);
	return 0;
}
