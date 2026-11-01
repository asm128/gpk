#include "dryer_app.h"
#include "gpk_noise.h"

::gpk::error_t									dry::modalClose				(::dry::SApplication & app) {
	::gpk::SGUI											& gui						= *app.Framework.GUI;
	const int32_t										indexDialog					= app.Screens.Modal.ControlIds[::dry::SCREEN_CONTROL_MODAL_DIALOG];
	gui.Controls.States[indexDialog].Hidden			= true;
	app.Screens.ModalEnabled						= false;
	gui.Controls.States[app.Screens.SpoolInfo.ControlIds[::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG]].Hidden	= true;
	return 0;
}

::gpk::error_t									dry::handleResize			(::dry::SApplication & app)		{
	auto												& framework					= app.Framework;
	const ::gpk::SCoord2<int16_t>						newSize						= framework.MainDisplay.Size.Cast<int16_t>();
	framework.GUI->Controls.Controls[app.Screens.RootControlIndex].Area.Size	= newSize;
	e_if(errored(::dry::handleResizeScreenHUD		(app, newSize)), "%s", "");
	e_if(errored(::dry::handleResizeScreenMenu		(app, newSize)), "%s", "");
	e_if(errored(::dry::handleResizeScreenModal		(app, newSize)), "%s", "");
	e_if(errored(::dry::handleResizeScreenHome		(app, newSize)), "%s", "");
	e_if(errored(::dry::handleResizeScreenSpoolInfo	(app, newSize)), "%s", "");
	return 0;
}

::gpk::error_t									dry::handleResizeScreenHUD	(::dry::SApplication & app, ::gpk::SCoord2<int16_t> newSize)	{
	::gpk::SGUI											& gui						= *app.Framework.GUI;
	gui.Controls.Controls	[app.Screens.HUD.ControlIds[::dry::SCREEN_CONTROL_HUD_DIALOG	]].Area.Size.x		= newSize.x;
	//gui.Controls.Controls	[app.Screens.HUD.ControlIds[::dry::SCREEN_CONTROL_HUD_CALENDAR	]].Area.Offset.x	= gui.Controls.Controls[app.Screens.HUD.ControlIds[::dry::SCREEN_CONTROL_HUD_DIALOG]].Area.Size.x / 2-
	//gui.Controls.Controls	[app.Screens.HUD.ControlIds[::dry::SCREEN_CONTROL_HUD_CALENDAR	]].Area.Size.x / 2;
	gui.Controls.States		[app.Screens.HUD.ControlIds[::dry::SCREEN_CONTROL_HUD_DIALOG	]].Updated			= false;
	return 0;
}

::gpk::error_t									dry::handleResizeScreenMenu	(::dry::SApplication & app, ::gpk::SCoord2<int16_t> newSize)	{
	::gpk::SGUI											& gui						= *app.Framework.GUI;
	const int32_t										indexDialog					= app.Screens.Menu.ControlIds[::dry::SCREEN_CONTROL_MENU_DIALOG];
	if(-1 == indexDialog)
		return 0;
	gui.Controls.Controls	[indexDialog].Area.Size.x	= newSize.x;
	gui.Controls.States		[indexDialog].Updated		= false;
	::gpk::vi32											dialogChildren				= gui.Controls.Children[indexDialog];
	int32_t												offsetX						= 0;
	for(uint32_t iMenuItem = 0, countMneuItems = dialogChildren.size(); iMenuItem < countMneuItems; ++iMenuItem) {
		const int32_t										indexMenuItem				= dialogChildren[iMenuItem];
		gui.Controls.Controls[indexMenuItem].Area.Offset.x	= (int16_t)offsetX;
		offsetX											+= gui.Controls.Controls[indexMenuItem].Area.Size.x;
	}
	return 0;
}

::gpk::error_t									dry::handleResizeScreenSpoolInfo	(::dry::SApplication & app, ::gpk::SCoord2<int16_t> newSize) {
	(void)newSize;
	::gpk::SGUI											& gui						= *app.Framework.GUI;
	const ::dry::SScreenSpoolInfo						& screen					= app.Screens.SpoolInfo;
	::gpk::SRectangle2<int16_t>							& dialogArea				= gui.Controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG]].Area;
	dialogArea.Size									= gui.Controls.Controls[app.Screens.Modal.ControlIds[::dry::SCREEN_CONTROL_MODAL_DIALOG]].Area.Size - ::gpk::SCoord2<int16_t>{4, 64};
	::dry::SCREEN_CONTROL_SPOOL_INFO					idControls	[]				=
	{	::dry::SCREEN_CONTROL_SPOOL_INFO_TITLE
	,	::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_BRAND
	,	::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_MATERIAL
	,	::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_COLOR
	,	::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_NOTES
	,	::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_DENSITY
	,	::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_DIAMETER
	,	::dry::SCREEN_CONTROL_SPOOL_INFO_LABEL_SPOOL_SIZE
	,	::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_BRAND
	,	::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_MATERIAL
	,	::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_COLOR
	,	::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_NOTES
	,	::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_DENSITY
	,	::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_DIAMETER
	,	::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_SPOOL_SIZE
	};
	for(uint32_t iControl = 0; iControl < ::gpk::size(idControls); ++iControl) {
		if(screen.ControlIds[idControls[iControl]] < screen.ControlIds[::dry::SCREEN_CONTROL_SPOOL_INFO_VALUE_BRAND])
			gui.Controls.Controls[screen.ControlIds[idControls[iControl]]].Area.Size.x		= dialogArea.Size.x - 2;
		else
			gui.Controls.Controls[screen.ControlIds[idControls[iControl]]].Area.Size.x		= int16_t(dialogArea.Size.x / 4.0 * 3);
		gui.Controls.States[screen.ControlIds[idControls[iControl]]].Updated = false;
	}
	gui.Controls.States		[app.Screens.Modal	.ControlIds[::dry::SCREEN_CONTROL_MODAL_DIALOG		]].Updated		= false;
	gui.Controls.States		[screen				.ControlIds[::dry::SCREEN_CONTROL_SPOOL_INFO_DIALOG	]].Updated		= false;
	return 0;
}

static	::gpk::error_t				screenSwitch			(::dry::SApplication & app, ::dry::SCREEN oldScreen, ::dry::SCREEN newScreen, int32_t idControl) {
	::gpk::SGUI								& gui					= *app.Framework.GUI;
	e_if(::gpk::guiSelect(*app.Framework.GUI, app.Screens.Menu.ControlIds[idControl]), "%s", "");
	gui.Controls.States[app.Screens.Config		.ControlIds[::dry::SCREEN_CONTROL_CONFIG_DIALOG			]].Hidden = idControl != dry::SCREEN_CONTROL_MENU_CONFIG		;
	gui.Controls.States[app.Screens.Home		.ControlIds[::dry::SCREEN_CONTROL_HOME_DIALOG			]].Hidden = idControl != dry::SCREEN_CONTROL_MENU_HOME			;
	gui.Controls.States[app.Screens.LoadFilament.ControlIds[::dry::SCREEN_CONTROL_LOAD_FILAMENT_DIALOG	]].Hidden = idControl != dry::SCREEN_CONTROL_MENU_LOAD_FILAMENT	;
	gui.Controls.States[app.Screens.Info		.ControlIds[::dry::SCREEN_CONTROL_INFO_DIALOG			]].Hidden = idControl != dry::SCREEN_CONTROL_MENU_INFO			;
	if(oldScreen != app.Screens.ScreenSelected) {
		info_printf("Switching to screen: %s. Previous screen: %s. Modal enabled: %s."
			, ::gpk::get_value_label(app.Screens.ScreenSelected).begin()
			, ::gpk::get_value_label(oldScreen).begin()
			, app.Screens.ModalEnabled ? "true" : "false"
			);
		app.Screens.ScreenSelected			= newScreen;
	}
	return 0;
}

static	::gpk::error_t				spoolEnqueue			(::dry::SApplication & app, const ::dry::SSpoolData & spool) {
	gpk_necall(app.Scanned.push_back(spool), "%s", "");
	return ::dry::modalDisplay(app, ::dry::MODAL_SCAN_SPOOL);
}

static	::gpk::error_t				handleControlSpoolInfo	(::dry::SApplication & app, int32_t idControl) {
	switch(idControl) {
	default: break;
	case ::dry::SCREEN_CONTROL_MODAL_CANCEL: app.Scanned.clear(); break;
	case ::dry::SCREEN_CONTROL_MODAL_UNLOAD:
		for(uint32_t iScanned = 0; iScanned < app.Scanned.size(); ++iScanned) {
			for(uint32_t iSpool = 0; iSpool < app.Catalog.FilamentLoaded.size(); ++iSpool)
				if(app.Scanned[iScanned].UniqueId == app.Catalog.FilamentLoaded[iSpool].UniqueId) {
					app.Catalog.FilamentLoaded.remove_unordered(iSpool--);
					break;
				}
		}
		::dry::updateCatalogDisplay(app);
		::dry::catalogSave(app.Catalog, ::gpk::vcs{"filaments_save_test.json"});
		app.Scanned.clear();
		break;
	case ::dry::SCREEN_CONTROL_MODAL_LOAD:
		for(uint32_t iScanned = 0; iScanned < app.Scanned.size(); ++iScanned)
			::dry::catalogAdd(app.Catalog, app.Scanned[iScanned], time(0), 0, 0, false);

		::dry::updateCatalogDisplay(app);
		::dry::catalogSave(app.Catalog, ::gpk::vcs{"filaments_save_test.json"});
		app.Scanned.clear();
		break;
	}
	return 0;
}

static	::gpk::error_t				stateSwitch				(::dry::SApplication & app, ::dry::SCREEN screen, int32_t idControl) {
	auto									& framework				= app.Framework;
	::gpk::SGUI								& gui					= *framework.GUI;
	::dry::SDryerState						& dryerState			= app.Dryer.State;
	::dry::SCommState						& commState				= app.Comms;
	const ::gpk::vcs						brands		[]			= {"Kodak", "BASF", "Zortrax"};
	const ::gpk::vcs						colors		[]			= {"red", "green", "blue", "black", "white"};
	const ::gpk::vcs						materials	[]			= {"ABS", "Nylon", "PLA"};
	switch(screen) {
	default: break;
	case ::dry::SCREEN_HOME			: ::dry::handleControlHome			(app, idControl); break;
	case ::dry::SCREEN_INFO			: ::dry::handleControlInfo			(app, idControl); break;
	case ::dry::SCREEN_LOAD_FILAMENT: ::dry::handleControlLoadFilament	(app, idControl); break;
	case ::dry::SCREEN_CONFIG		: ::dry::handleControlConfig		(app, idControl); break;
	case ::dry::SCREEN_MENU:
		switch((uint32_t)idControl) {
		default: break;
		case dry::SCREEN_CONTROL_MENU_CONFIG		: ::screenSwitch(app, app.Screens.ScreenSelected, ::dry::SCREEN_CONFIG			, idControl); break;
		case dry::SCREEN_CONTROL_MENU_HOME			: ::screenSwitch(app, app.Screens.ScreenSelected, ::dry::SCREEN_HOME			, idControl); break;
		case dry::SCREEN_CONTROL_MENU_LOAD_FILAMENT	: ::screenSwitch(app, app.Screens.ScreenSelected, ::dry::SCREEN_LOAD_FILAMENT	, idControl); break;
		case dry::SCREEN_CONTROL_MENU_INFO			: ::screenSwitch(app, app.Screens.ScreenSelected, ::dry::SCREEN_INFO			, idControl); break;
		}
		break;
	case ::dry::SCREEN_MODAL:
		switch(app.Screens.ModalSelected) {
		case ::dry::MODAL_DRYING_COMPLETE	:
		case ::dry::MODAL_CLOSE_DOOR		:
			app.Dryer.State.RunningPending					= false;
			::dry::modalClose(app);
			break;
		case ::dry::MODAL_DRYING_STOP		:
			if(idControl == ::dry::SCREEN_CONTROL_MODAL_YES)
				app.Dryer.State.StoppedPending	= true;
			break;
		case ::dry::MODAL_SCAN_SPOOL		:
			::handleControlSpoolInfo(app, idControl);
			break;
		}
		if(idControl != ::dry::SCREEN_CONTROL_MODAL_DIALOG)
			::dry::modalClose(app);
		break;
	case ::dry::SCREEN_HUD:
		switch((uint32_t)idControl) {
		default: break;
		case ::dry::SCREEN_CONTROL_HUD_DOOR		: dryerState.DoorClosed = !dryerState.DoorClosed; break;
		case ::dry::SCREEN_CONTROL_HUD_ETHERNET	: commState.Ethernet	= !commState.Ethernet	; break;
		case ::dry::SCREEN_CONTROL_HUD_WIFI		: commState.WiFi		= !commState.WiFi		; break;
		case ::dry::SCREEN_CONTROL_HUD_SERIAL	: break;
		case ::dry::SCREEN_CONTROL_HUD_RFID		: commState.RFID		= !commState.RFID		;
			{
				::dry::SSpoolData		spoolScanned	= {};
				const int32_t			material		= rand() % ::gpk::size(materials);
				spoolScanned.UniqueId				= 123456;//(uint64_t(rand()) << 32) | time(0);
				spoolScanned.Brand					= brands	[rand() % ::gpk::size(brands)];
				spoolScanned.Material				= materials	[material];
				spoolScanned.Color					= colors	[rand() % ::gpk::size(colors)];
				spoolScanned.Notes					= (rand() % 2) ? ::gpk::vcs{"Filament prototype"} : ::gpk::vcs{""};
				spoolScanned.Density				= 1.04;
				spoolScanned.Diameter				= (rand() % 2) ? 1.75 : 2.85;
				spoolScanned.SpoolSize				= (rand() % 2) ? 750 : 1000;
				spoolScanned.MaxDryingTemperature	= (material == 2) ? 45 : (material == 1) ? 85 : 65;
				::spoolEnqueue(app, spoolScanned);
			}
			break;
		}
		break;
	}
	e_if(::gpk::guiUpdateMetrics(gui, framework.MainDisplayOffscreen->Color.metrics(), false), "%s", "");
	return 0;
}

static	::gpk::error_t				handleGUI				(::dry::SApplication & app) {
	::gpk::SGUI								& gui					= *app.Framework.GUI;
	::gpk::SInput							& input					= *app.Framework.Input;
	bool									running					= true;
	::gpk::array_pod<uint32_t>				controlsToProcess		= {};
	::gpk::guiGetProcessableControls(gui, controlsToProcess);
	bool									executed				= false;
	for(uint32_t iProcessable = 0, countControls = controlsToProcess.size(); iProcessable < countControls; ++iProcessable) {
		uint32_t								iControl				= controlsToProcess[iProcessable];
		if(gui.Controls.States[iControl].Execute) {
			info_printf("Executed %u.", iControl);
			for(uint32_t iScreenControl = 0; iScreenControl < app.Screens.HUD.ControlIds.size(); ++iScreenControl)
				if(app.Screens.HUD.ControlIds[iScreenControl] == (int32_t)iControl) {
					info_printf("HUD Control executed: %s", ::gpk::get_value_label((::dry::SCREEN_CONTROL_HUD)iScreenControl).begin());
					::stateSwitch (app, ::dry::SCREEN_HUD, iScreenControl);
					executed				= true;
					break;
				}
			if(executed)
				break;
			if(app.Screens.ModalEnabled) { // Avoid processing other controls while modal is enabled.
				for(uint32_t iScreenControl = 0; iScreenControl < app.Screens.Modal.ControlIds.size(); ++iScreenControl)
					if(app.Screens.Modal.ControlIds[iScreenControl] == (int32_t)iControl) {
						info_printf("Modal Control executed: %s", ::gpk::get_value_label((::dry::SCREEN_CONTROL_MODAL)iScreenControl).begin());
						::stateSwitch (app, ::dry::SCREEN_MODAL, iScreenControl);
						executed				= true;
						break;
					}
				if(executed)
					break;
			}
			else {
				for(uint32_t iScreenControl = 0; iScreenControl < app.Screens.Menu.ControlIds.size(); ++iScreenControl)
					if(app.Screens.Menu.ControlIds[iScreenControl] == (int32_t)iControl) {
						info_printf("Menu Control executed: %s", ::gpk::get_value_label((::dry::SCREEN_CONTROL_MENU)iScreenControl).begin());
						::stateSwitch (app, ::dry::SCREEN_MENU, iScreenControl);
						executed				= true;
						break;
					}
				if(executed)
					break;
				for(uint32_t iScreenControl = 0; iScreenControl < app.Screens.Home.ControlIds.size(); ++iScreenControl)
					if(app.Screens.Home.ControlIds[iScreenControl] == (int32_t)iControl) {
						info_printf("Home Control executed: %s", ::gpk::get_value_label((::dry::SCREEN_CONTROL_HOME)iScreenControl).begin());
						::stateSwitch (app, ::dry::SCREEN_HOME, iScreenControl);
						executed				= true;
						break;
					}
				if(executed)
					break;
			}
		}
	}

	if(input.KeyboardCurrent.KeyState[9] & 1)
		running								= false;
	return running ? 0 : 1;
}

::gpk::error_t						dry::updateApp			(::dry::SApplication & app) {
	e_if(errored(::dry::dryerUpdate(app.Dryer, app.Framework.Timer.LastTimeMicroseconds / 1000)), "%s", "");
	for(uint32_t iDryerEvent = 0; iDryerEvent < app.Dryer.EventList.size(); ++iDryerEvent) {
		::dry::DRYER_EVENT						dryerEvent						= app.Dryer.EventList[iDryerEvent];
		always_printf("Processing dryer event: %s (%u).", ::gpk::get_value_label(dryerEvent).begin(), dryerEvent);
		switch(dryerEvent) {
		case DRYER_EVENT_STARTED				: break;
		case DRYER_EVENT_PRE_PREHEATING_STARTED	: break;
		case DRYER_EVENT_PREHEATING_STARTED		: break;
		case DRYER_EVENT_DRYING_STARTED			: break;
		case DRYER_EVENT_CYCLE_CHANGED			: break;
		case DRYER_EVENT_STOPPED				: break;
		case DRYER_EVENT_FINISHED				: break;
		case DRYER_EVENT_DOOR_OPEN				: break;
		case DRYER_EVENT_DOOR_CLOSED			: break;
		default:
			warning_printf("Dryer event not handled: %s (%u)", ::gpk::get_value_label(dryerEvent).begin(), dryerEvent);
			break;
		}
	}
	//for(uint32_t iDryerCommand = 0; iDryerCommand < app.Dryer.CommandList.size(); ++iDryerCommand) {
	//	const ::dry::SDryerCommand				dryerCommand					= app.Dryer.CommandList[iDryerCommand];
	//	::gpk::error_t							failed							= {};
	//	always_printf("Processing dryer command: %s (%u). Args: %i.", ::gpk::get_value_label(dryerCommand.Command).begin(), dryerCommand.Command, dryerCommand.Args);
	//	switch(dryerCommand.Command) {
	//	case ::dry::DRYER_COMMAND_PUMP: {
	//		const ::gpk::vcc						serialCommand					= dryerCommand.Args ? ::gpk::vcs{"<ON>"} : ::gpk::vcs{"<OFF>"};
	//		if(errored(::s3d::serialWrite(app.Serial, serialCommand))) {
	//			failed = true;
	//			error_printf("Failed to send serial command to control pump: %s.", serialCommand.begin());
	//		}
	//		break;
	//	}
	//	case ::dry::DRYER_COMMAND_TEMPERATURE: {
	//		const uint8_t							temperature						= (uint8_t)dryerCommand.Args;
	//		char									buffer[32]						= {};
	//		snprintf(buffer, ::gpk::size(buffer)-2, "<TEMP %u>", temperature);
	//		if(errored(::s3d::serialWrite(app.Serial, ::gpk::vcs{buffer}))) {
	//			error_printf("Failed to send serial command to set temperature to: %u.", temperature);
	//			failed								= true;
	//		}
	//		break;
	//	}
	//	}
	//	if(failed)
	//		break;
	//	app.Dryer.CommandList.remove(iDryerCommand);
	//}

	e_if(errored(::dry::updateScreenHUD(app)), "%s", "");
	switch(app.Screens.ScreenSelected) {
	case dry::SCREEN_HOME			:	e_if(errored(::dry::updateScreenHome		(app)), "%s", ""); break;
	case dry::SCREEN_LOAD_FILAMENT	:	e_if(errored(::dry::updateScreenLoadFilament(app)), "%s", ""); break;
	case dry::SCREEN_INFO			:	e_if(errored(::dry::updateScreenInfo		(app)), "%s", ""); break;
	case dry::SCREEN_CONFIG			:	e_if(errored(::dry::updateScreenConfig		(app)), "%s", ""); break;
	}
	bool									running					= ::handleGUI(app) ? false : true;
	if(false == running)
		return 1;

	if(app.Scanned.size() && (false == app.Screens.ModalEnabled || app.Screens.ModalSelected != ::dry::MODAL_SCAN_SPOOL)) {
		::dry::modalClose(app);
		::dry::modalDisplay(app, ::dry::MODAL_SCAN_SPOOL);
	}
	if(app.Framework.MainDisplay.Resized)
		e_if(errored(::dry::handleResize(app)), "%s", "");

	return 0;
}
