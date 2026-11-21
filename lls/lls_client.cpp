#include "lls_client.h"
#include "lls_event_args.h"

#include "gpk_event_screen.h"
#include "gpk_gui_control_list.h"

using ::gpk::get_value_namep, ::gpk::get_enum_namep, ::gpk::failed;
GPK_USING_TYPEINT();

stxp	uint16_t		LLS_MAX_PLAYERS		= 4;
stxp	::gpk::n2u1_t	PLAYER_UI_SIZE		= {320, 40};
stxp	::gpk::astatic<::gpk::bgra, LLS_MAX_PLAYERS>	PLAYER_COLORS	=
	{ 0xFF00FF00U
	, ::gpk::PURPLE
	, 0xFF0000FFU
	, 0xFFFFFF00U
	};

sttc gpk::err_t	guiSetupCommon		(::gpk::SGUI & gui) {
	gui.ColorModeDefault	= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault		= ::gpk::ASCII_COLOR_Dark_red * 16 + 10;
	gui.SelectedFont		= 7;

	::gpk::pobj<::gpk::SControlColors>	colors;
	colors.create(*gui.Colors);
	gui.Colors				= colors;
	return 0;
}

sttc gpk::err_t	dialogCreateCommon	(::gpk::SDialog & dialog, cnst ::gpk::pobj<::gpk::SInput> & inputState, cnst ::gpk::n2f2_t & cursorPos) {
	dialog						= {};
	dialog.Input				= inputState;
	dialog.GUI->CursorPos		= cursorPos;
	dialog.GUI->Controls.Draw[0].NoClient	= true;
	dialog.GUI->Controls.Draw[0].NoBorder	= true;
	if_fail_fe(::guiSetupCommon(*dialog.GUI));
	return dialog.Update();
}

sttc gpk::err_t	uiPlayerSetupHome	(::lls::SUIPlayer & uiPlayer, cnst ::gpk::pobj<::gpk::SInput> & input, cnst ::gpk::n2f2_t & cursorPos, uint32_t iPlayer, cnst ::gpk::vcst_t & playerName) {
	::gpk::ALIGN				playerAlign		= {};
	::gpk::n2s1_t			playerPosition	= {};
	switch(iPlayer) {
	default:
	case 0: playerAlign = ::gpk::ALIGN_BOTTOM_LEFT;	break;
	case 1: playerAlign = ::gpk::ALIGN_TOP_RIGHT;		break;
	case 2: playerAlign = ::gpk::ALIGN_BOTTOM_RIGHT;	break;
	case 3: playerAlign = ::gpk::ALIGN_TOP_LEFT;		break;
	}

	::gpk::SDialog			& playerDialog		= uiPlayer.DialogHome;
	if_fail_fe(::dialogCreateCommon(playerDialog, input, cursorPos));
	::gpk::SGUI				& playerGUI			= *playerDialog.GUI;
	::gpk::pobj<::gpk::a8bgra>	palette;
	palette.create(*playerGUI.Colors->Palette);
	playerGUI.Colors->Palette	= palette;
	playerGUI.ThemeDefault		= ::gpk::ASCII_COLOR_Dark_grey * 16 + 12;
	if_fail_fe(::gpk::guiCreateControlList<::lls::UI_PROFILE>(playerGUI, playerDialog.Root, PLAYER_UI_SIZE, playerPosition, playerAlign, ::gpk::ALIGN_CENTER, uiPlayer.ControlsDialogHome));
	for(uint32_t iButton = 0, iStop = ::gpk::get_value_count<::lls::UI_PROFILE>() - 1, iOffset = playerDialog.Root + 1 + ::lls::UI_PROFILE_Name; iButton < iStop; ++iButton) {
		playerGUI.Controls.Placement[iOffset + iButton].Margin	= {};
		playerGUI.Controls.Draw[iOffset + iButton].NoClient		= true;
	}
	for(uint32_t iButton = 0, iStop = ::gpk::get_value_count<::lls::UI_PROFILE>() - 1, iOffset = playerDialog.Root + 1 + ::lls::UI_PROFILE_Score; iButton < iStop; ++iButton) {
		playerGUI.Controls.Placement[iOffset + iButton].Area.Offset.y	-= int16_t(16 * iButton);
		playerGUI.Controls.Placement[iOffset + iButton].Area.Size.y		= 24;
	}

	if_fail_fe(::gpk::inputBoxCreate(uiPlayer.InputBox, playerGUI, playerDialog.Root));
	playerGUI.Controls.SetHidden(uiPlayer.InputBox.IdRoot, true);
	playerGUI.Controls.Placement[uiPlayer.InputBox.IdRoot].Align			= playerAlign;
	playerGUI.Controls.Placement[uiPlayer.InputBox.IdRoot].Area.Offset.x	= 240;

	if_fail_fe(::gpk::controlFontSet(playerGUI.Controls, playerDialog.Root + 1 + ::lls::UI_PROFILE_Name, 10));
	if_fail_fe(::gpk::controlTextSet(playerGUI.Controls, playerDialog.Root + 1 + ::lls::UI_PROFILE_Name, playerName));
	return 0;
}

sttc gpk::err_t	clientUISetup		(::lls::LLSClient & app, cnst ::gpk::pobj<::gpk::SInput> & input) {
	while(app.Game.Players.size() < LLS_MAX_PLAYERS) {
		sc_t					playerName[64]	= {};
		sprintf_s(playerName, "Player %u", app.Game.Players.size() + 1);
		::gpk::SPlayer			player			= {};
		player.Name				= ::gpk::label(playerName);
		player.Color			= PLAYER_COLORS[app.Game.Players.size()];
		player.Mode				= ::gpk::PLAYER_MODE_Human;
		if_fail_fe(app.Game.Players.push_back(player));
	}

	if_fail_fe(app.UIPlay.PlayerUI.resize(LLS_MAX_PLAYERS));
	for(uint32_t iPlayer = 0; iPlayer < app.UIPlay.PlayerUI.size(); ++iPlayer)
		if_fail_fef(::uiPlayerSetupHome(app.UIPlay.PlayerUI[iPlayer], input, app.GUI->CursorPos, iPlayer, app.Game.Players[iPlayer].Name), "iPlayer: %u", iPlayer);
	return 0;
}

sttc gpk::err_t		uiPlayerUpdateHome	(::lls::SUIPlayer & uiPlayer, ::gpk::u1_t iPlayer, cnst ::gpk::vcst_t & playerName, cnst ::gpk::bgra & playerColor) {
	::gpk::SDialog			& playerDialog		= uiPlayer.DialogHome;
	::gpk::SGUI				& playerGUI			= *playerDialog.GUI;
	if_fail_fe(::gpk::controlTextSet(playerGUI.Controls, playerDialog.Root + 1 + ::lls::UI_PROFILE_Score, ""));
	if_fail_fe(::gpk::controlTextSet(playerGUI.Controls, playerDialog.Root + 1 + ::lls::UI_PROFILE_Name, playerName));
	{
		::gpk::SControlDraw		& controlDraw		= playerGUI.Controls.Draw[playerDialog.Root + 1 + ::lls::UI_PROFILE_Name];
		controlDraw.ColorTheme	= int16_t(3 + iPlayer);
		cnst ::gpk::SControlTheme	& theme			= (*playerGUI.Colors->ControlThemes)[(0 == controlDraw.ColorTheme) ? playerGUI.ThemeDefault : controlDraw.ColorTheme - 1];
		cnst ::gpk::astu2_t<::gpk::UI_CONTROL_AREA_COUNT>	& colorCombo	= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_NORMAL];
		(*playerGUI.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_TEXT_FACE]]	= playerColor;
		(*playerGUI.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_BACKGROUND]]	= playerColor;
		playerGUI.Controls.Draw[uiPlayer.InputBox.IdText].ColorTheme	= controlDraw.ColorTheme;
		playerGUI.Controls.Draw[uiPlayer.InputBox.IdText].NoClient		= true;
	}
	return 0;
}

sttc gpk::err_t	processClientScreenEvent	(::lls::LLSClient & app, cnst ::gpk::SEventView<::gpk::EVENT_SCREEN> & screenEvent) {
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_SCREEN_Create:
	case ::gpk::EVENT_SCREEN_Resize: {
		cnst ::gpk::n2u1_t		newMetrics		= *(cnst ::gpk::n2u1_t*)screenEvent.Data.begin();
		for(uint32_t iPlayer = 0; iPlayer < app.UIPlay.PlayerUI.size(); ++iPlayer)
			if_fail_fe(::gpk::guiUpdateMetrics(*app.UIPlay.PlayerUI[iPlayer].DialogHome.GUI, newMetrics, true));
		} break;
	}
	return 0;
}
sttc gpk::err_t	clientUIUpdate		(::lls::LLSClient & app, ::gpk::vpobj<::gpk::SEventSystem> systemEvents) {
	for(uint32_t iPlayer = 0; iPlayer < app.UIPlay.PlayerUI.size(); ++iPlayer) {
		::lls::SUIPlayer		& uiPlayer			= app.UIPlay.PlayerUI[iPlayer];
		::gpk::SDialog			& dialog				= uiPlayer.DialogHome;
		::gpk::SGUI				& gui					= *dialog.GUI;
		::gpk::guiProcessInput(gui, *dialog.Input, systemEvents);
		gui.Controls.SetHidden(dialog.Root, false);
		if_fail_fef(::uiPlayerUpdateHome(uiPlayer, (uint16_t)iPlayer, app.Game.Players[iPlayer].Name, app.Game.Players[iPlayer].Color), "iPlayer: %u", iPlayer);

		::gpk::acid				controlsToProcess	= {};
		if_fail_fe(::gpk::controlActions(gui.Controls, controlsToProcess));
		b8_c				wasEditing			= uiPlayer.InputBox.Editing;
		if(int32_t result = uiPlayer.InputBox.Update(gui, uiPlayer.InputBox.VirtualKeyboard, systemEvents, controlsToProcess)) {
			if(result == INT_MAX) {
				::gpk::vcst_t			trimmed				= {uiPlayer.InputBox.Text.begin(), ::gpk::min(uiPlayer.InputBox.Text.size(), 16U)};
				::gpk::trim(trimmed);
				if(trimmed.size())
					app.Game.Players[iPlayer].Name	= ::gpk::label(trimmed);
				if_fail_fe(uiPlayer.InputBox.Edit(gui, false));
			}
		}
		else {
			if_fail_fe(::gpk::controlHandle(gui.Controls, controlsToProcess, [&](uint32_t iControl) {
				cnst uint32_t			offsetControl		= dialog.Root + 1;
				switch(iControl - offsetControl) {
				case ::lls::UI_PROFILE_Name:
					if_fail_fe(app.UI.UserInput.Edit(*app.GUI, false));
					if_fail_fe(uiPlayer.InputBox.SetText(gui, gui.Controls.Text[offsetControl + ::lls::UI_PROFILE_Name].Text));
					if_fail_fe(uiPlayer.InputBox.Edit(gui, true));
					break;
				}
				return 0;
			}));
		}
		if(wasEditing && false == uiPlayer.InputBox.Editing) {
			if_fail_fe(app.UI.UserInput.SetText(*app.GUI, {}));
			if_fail_fe(app.UI.UserInput.Edit(*app.GUI, true));
		}
	}
	return 0;
}

::gpk::err_t		lls::llsClientUIDraw	(::lls::LLSClient & app, ::gpk::g8bgra targetPixels) {
	for(uint32_t iPlayer = 0; iPlayer < app.UIPlay.PlayerUI.size(); ++iPlayer)
		if_fail_fe(::gpk::guiDraw(*app.UIPlay.PlayerUI[iPlayer].DialogHome.GUI, targetPixels));
	return 0;
}

::gpk::err_t		lls::llsClientUpdate		(::lls::LLSClient & app, double lastTimeSeconds, cnst ::gpk::pobj<::gpk::SInput> & inputState, ::gpk::vpobj<::gpk::SEventSystem> systemEvents) { 
	if(0 == app.UIPlay.PlayerUI.size())
		if_fail_fe(::clientUISetup(app, inputState));
	if_fail_fe(systemEvents.for_each([&app](cnst ::gpk::pobj<::gpk::SEventSystem> & systemEvent) {
		if(systemEvent->Type == ::gpk::SYSTEM_EVENT_Screen)
			return ::gpk::eventExtractAndHandle<::gpk::EVENT_SCREEN>(*systemEvent, [&app](cnst ::gpk::SEventView<::gpk::EVENT_SCREEN> & screenEvent) { return ::processClientScreenEvent(app, screenEvent); });
		return 0;
	}));
	if_fail_fe(::lls::llsUpdate(app, lastTimeSeconds, inputState, systemEvents, [&app](::gpk::pobj<::lls::EventLLS> & _eventToProcess, ::gpk::apobj<::lls::EventLLS> & outputEvents) {
		if(!_eventToProcess)
			return false;

		cnst ::lls::EventLLS	& eventToProcess		= *_eventToProcess;
		info_printf("%s", ::gpk::get_value_namep(eventToProcess.Type));

		::gpk::err_t			result			= 0; 
		switch(eventToProcess.Type) {
		CASE_LLS_EVENT(result, app, WORLD_EVENT, outputEvents); 
		CASE_LLS_EVENT(result, app, WORLD_ADMIN, outputEvents); 
		CASE_LLS_EVENT(result, app, ACT_SAILING, outputEvents); 
		CASE_LLS_EVENT(result, app, ACTION_CHAR, outputEvents); 
		CASE_LLS_EVENT(result, app, ACT_ENGINES, outputEvents); 
		CASE_LLS_EVENT(result, app, ACT_AIRSHIP, outputEvents); 
		CASE_LLS_EVENT(result, app, ACT_WHEELED, outputEvents); 
		CASE_LLS_EVENT(result, app, CLIENT_ASKS, outputEvents); 
		CASE_LLS_EVENT(result, app, WORLD_SETUP, outputEvents); 
		CASE_LLS_EVENT(result, app, WORLD_VALUE, outputEvents); 
		default: 
			gpk_warning_unhandled_event(eventToProcess); 
			break;
		}
		return b8_t(result == 1);
	}));
	if_fail_fe(::clientUIUpdate(app, systemEvents));
	return 0; 
}

::gpk::err_t	lls::handleACTION_CHAR		(::lls::LLSClient & app, cnst ::lls::EViewMe & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)app; 
	cnst ::lls::SArgsEvent	& eventArgs				= *(cnst ::lls::SArgsEvent*)gameEvent.Data.begin();
	::lls::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::ACTION_CHAR_Walk	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Turn	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Rush	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Hide	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Dash	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Jump	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Duck	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Warp	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Pick	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Grab	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Wear	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Use		: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Spawn	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Shoot	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Throw	: gpk_warning_not_implemented(gameEvent); break;
	case ::lls::ACTION_CHAR_Sleep	: gpk_warning_not_implemented(gameEvent); break;
	} 
	return 0; 
}

::gpk::err_t	lls::handleWORLD_ADMIN		(::lls::LLSClient & app, cnst ::lls::EViewAdminWorld & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)app; 
	cnst ::lls::SArgsEvent	& eventArgs				= *(cnst ::lls::SArgsEvent*)gameEvent.Data.begin();
	::lls::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Create	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Rename	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Delete	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Locate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Rotate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Resize	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Reskin	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Relocate	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_ADMIN_Generate	: gpk_warning_not_implemented(gameEvent); break; 
	} 
	return 0; 
}

::gpk::err_t	lls::handleWORLD_EVENT		(::lls::LLSClient & app, cnst ::lls::EViewWorld & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)app; 
	cnst ::lls::SArgsEvent	& eventArgs				= *(cnst ::lls::SArgsEvent*)gameEvent.Data.begin();
	::lls::printArgsEvent(eventArgs);
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::WORLD_EVENT_Rain			: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Thunderstorm	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Earthquake	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Tornado		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Wave			: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Lightning		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Wildfire		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_EVENT_Flood			: gpk_warning_not_implemented(gameEvent); break; 
	} 
	return 0; 
}

::gpk::err_t	lls::handleCLIENT_ASKS		(::lls::LLSClient & app, cnst ::lls::EViewClient & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)app; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::CLIENT_ASKS_Join: 
	case ::lls::CLIENT_ASKS_Quit: 
		if_fail_fe(::gpk::eventEnqueueChild(app.EventsToSend, ::lls::LLS_EVENT_CLIENT_ASKS, gameEvent.Type, gameEvent.Data));
		return 1; 
	} 
	return 0; 
}

::gpk::err_t	lls::handleWORLD_SETUP		(::lls::LLSClient & world, cnst ::lls::EViewWorldSetup & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)world; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::WORLD_SETUP_Seed		: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_SETUP_MaxPlayers: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_SETUP_BlockSize	: gpk_warning_not_implemented(gameEvent); break; 
	}
	return 0;
}

::gpk::err_t	lls::handleWORLD_VALUE		(::lls::LLSClient & world, cnst ::lls::EViewWorldValue & gameEvent, ::gpk::apobj<::lls::EventLLS> & outputEvents) { 
	(void)outputEvents; (void)world; 
	switch(gameEvent.Type) { 
	default: gpk_warning_unhandled_event(gameEvent); break; 
	case ::lls::WORLD_VALUE_TimeCreated			: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_VALUE_TimeOffset			: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_VALUE_DaylightOffsetMinutes	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_VALUE_DaylightRatioExtra	: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_VALUE_TimeLastSaved			: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_VALUE_WorldSize				: gpk_warning_not_implemented(gameEvent); break; 
	case ::lls::WORLD_VALUE_TimeScale				: gpk_warning_not_implemented(gameEvent); break; 
	}
	return 0;
}
