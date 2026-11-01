#include "dryer_app.h"

::gpk::error_t						dry::handleResizeScreenHome		(::dry::SApplication & app, ::gpk::SCoord2<int16_t> newSize)	{
	::gpk::SGUIControlTable					& controls						= app.Framework.GUI->Controls;
	const ::dry::SScreenHome				& screen						= app.Screens.Home;
	::gpk::guiUpdateMetrics(*app.Framework.GUI, newSize.Cast<uint32_t>(), false);
	const uint32_t							heightMenu						= (-1 == app.Screens.Menu.ControlIds[::dry::SCREEN_CONTROL_MENU_DIALOG])
		? 0
		: controls.Controls[app.Screens.Menu.ControlIds[::dry::SCREEN_CONTROL_MENU_DIALOG]].Area.Size.y;
	const uint32_t							heightHUD						= controls.Controls[app.Screens.HUD	.ControlIds[::dry::SCREEN_CONTROL_HUD_DIALOG]].Area.Size.y;
	const int16_t							finalHeight						= int16_t(newSize.y - (heightMenu + heightHUD));

	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_DIALOG		]].Area.Offset.y	= (int16_t)heightHUD;
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_DIALOG		]].Area.Size		= {newSize.x, finalHeight};
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_GROUP_PROGRAM]].Area.Size.x		=  int16_t(newSize.x / 7.0 * 3);//, finalHeight};
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_GROUP_SESSION]].Area.Size.x		=  int16_t(newSize.x / 7.0 * 3);
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_GROUP_GAUGE	]].Area.Size.x		=  int16_t(newSize.x / 7.0 * 4);
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_GROUP_SPOOL	]].Area.Size		= {int16_t(newSize.x / 7.0 * 4), int16_t(finalHeight - controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_GROUP_GAUGE]].Area.Size.y)};

	const int16_t							sessionGroupWidth					= controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_GROUP_SESSION]].Area.Size.x;
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_SESSION_STATUS_LABEL		]].Area.Size.x	= sessionGroupWidth - 2;
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_ELAPSED_TIME_LABEL		]].Area.Size.x	= sessionGroupWidth - 2;
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_PRE_PREHEATING_TIME_LABEL]].Area.Size.x	= sessionGroupWidth - 2;
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_PREHEATING_TIME_LABEL	]].Area.Size.x	= sessionGroupWidth - 2;
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_DRYING_TIME_LABEL		]].Area.Size.x	= sessionGroupWidth - 2;
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_REMAINING_TIME_LABEL		]].Area.Size.x	= sessionGroupWidth - 2;
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_CURRENT_CYCLE_LABEL		]].Area.Size.x	= sessionGroupWidth - 2;

	const int16_t							programGroupWidth					= controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_GROUP_PROGRAM]].Area.Size.x;
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_PROGRAM_START					]].Area.Size.x	= programGroupWidth - 8;
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_PROGRAM_STOP						]].Area.Size.x	= programGroupWidth - 8;
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_NAME				]].Area.Size.x	= programGroupWidth - 2;
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_TEMPERATURE		]].Area.Size.x	= programGroupWidth - 2;
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_PRESSURE			]].Area.Size.x	= programGroupWidth - 2;
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_PRE_HEATING_TIME	]].Area.Size.x	= programGroupWidth - 2;
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_SUBCYCLE_DURATION	]].Area.Size.x	= programGroupWidth - 2;
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_SUBCYCLES			]].Area.Size.x	= programGroupWidth - 2;

	const ::gpk::SCoord2<int16_t>			spoolGroupSize						= controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_GROUP_SPOOL]].Area.Size;
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_SPOOL_LIST_TITLE	]].Area.Size.x		= spoolGroupSize.x - 4;
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_SPOOL_LIST		]].Area.Size		= {int16_t(spoolGroupSize.x -  4), int16_t(spoolGroupSize.y - controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_SPOOL_LIST_TITLE]].Area.Size.y - 4)};
	::gpk::SCoord2<int16_t>					& scrollableSize					= controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_SPOOL_SCROLLABLE]].Area.Size;
	scrollableSize						= {int16_t(spoolGroupSize.x - 36), int16_t(spoolGroupSize.y - controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_SPOOL_LIST_TITLE]].Area.Size.y - 6)};
	controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_SPOOL_SCROLLBAR	]].Area.Size.y		= int16_t(spoolGroupSize.y - controls.Controls[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_SPOOL_LIST_TITLE]].Area.Size.y - 8);
	const ::gpk::vci32						listChildren						= controls.Children[screen.ControlIds[::dry::SCREEN_CONTROL_HOME_SPOOL_SCROLLABLE]];
	for(uint32_t iSpool = 0, countSpools = listChildren.size(); iSpool < countSpools; ++iSpool) {
		controls.Controls[listChildren[iSpool]].Area.Size.x		= spoolGroupSize.x - 40;
		controls.States[listChildren[iSpool]].Updated			= false;
	}
	return 0;
}

::gpk::error_t						updateGaugesFromState			(::dry::SApplication & app, bool forceUpdate) {
	::dry::SDryerState						& dryerState					= app.Dryer.State;
	::dry::SScreenHome						& screenHome					= app.Screens.Home;
	screenHome.GaugeDryerSmall.Gauges[::dry::GAUGE_TEMPERATURE	].SetValue(dryerState.Temperature	, 100);	//::dry::DRYER_MAX_TEMPERATURE);
	screenHome.GaugeDryerSmall.Gauges[::dry::GAUGE_VACUUM		].SetValue(dryerState.Vacuum		, 1000);//::dry::DRYER_MAX_PRESSURE);
	screenHome.GaugeDryerSmall.Gauges[::dry::GAUGE_HUMIDITY		].SetValue(dryerState.Humidity		, 100);	//app.MAX_HUMIDITY);
	for(uint32_t iFeeder = 0; iFeeder < ::gpk::size(screenHome.GaugeFeederSmall); ++iFeeder) {
		screenHome.GaugeFeederSmall[iFeeder].SetValue(iFeeder ? dryerState.Humidity : dryerState.Vacuum, iFeeder ? 100 : 1000);//app.MAX_HUMIDITY : ::dry::DRYER_MAX_PRESSURE);
	}
	// The following string comparisons are to refresh the gauge images only if the values changed
	::gpk::SGUI								& gui						= *app.Framework.GUI;
	char_t									bufferTemp	[32]			= {};
	snprintf(bufferTemp, ::gpk::size(bufferTemp) - 2, "%i\xf8\x43", (int32_t)dryerState.Temperature);
	if(forceUpdate || (::gpk::vcs{bufferTemp} != ::gpk::vcs{screenHome.Strings.DryerTemperature.Storage})) {
		::gpk::memcpy_s(screenHome.Strings.DryerTemperature.Storage, bufferTemp);
		::dry::gaugeImageUpdate(screenHome.GaugeDryerSmall.Gauges[::dry::GAUGE_TEMPERATURE]);
		app.Framework.MainDisplay.Repaint	= true;
		e_if(errored(::gpk::controlTextSet(gui, screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_DRYER_SMALL], screenHome.Strings.DryerTemperature.Storage)), "%s", "");
	}
	for(uint32_t iFeeder = 0; iFeeder < ::gpk::size(screenHome.GaugeFeederSmall); ++iFeeder) {
		snprintf(bufferTemp, ::gpk::size(bufferTemp) - 2, iFeeder ? "%i%%" : "%i mbar", int32_t(iFeeder ? dryerState.Humidity : dryerState.Vacuum));
		if(forceUpdate || (::gpk::vcs{bufferTemp} != ::gpk::vcs{screenHome.Strings.FeederTemperature[iFeeder].Storage})) {
			::gpk::memcpy_s(screenHome.Strings.FeederTemperature[iFeeder].Storage, bufferTemp);
			::dry::gaugeImageUpdate(screenHome.GaugeFeederSmall[iFeeder]
				, iFeeder ? ::gpk::CYAN	: ::gpk::DARKGRAY
				, iFeeder ? ::gpk::BLUE	: ::gpk::LIGHTGRAY
			);
			e_if(errored(::gpk::controlTextSet(gui, screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_FEEDER_0_SMALL + iFeeder], screenHome.Strings.FeederTemperature[iFeeder].Storage)), "%s", "");
			app.Framework.MainDisplay.Repaint	= true;
		}
	}
	return 0;
}

template<size_t _nSize>
static	::gpk::error_t				updateControlIfNeeded			(::dry::SApplication & app, char_t (&bufferTemp)[_nSize], ::gpk::array_static<char_t, _nSize> & storage, ::dry::SCREEN_CONTROL_HOME idControl) {
	::dry::SScreenHome						& screenHome					= app.Screens.Home;
	::gpk::SGUI								& gui							= *app.Framework.GUI;
	if(::gpk::vcs{bufferTemp} != ::gpk::vcs{storage.Storage}) {
		::gpk::memcpy_s(storage.Storage, bufferTemp);
		e_if(errored(::gpk::controlTextSet(gui, screenHome.ControlIds[idControl], storage.Storage)), "%s", "");
		app.Framework.MainDisplay.Repaint	= true;
	}
	return 0;
}

::gpk::error_t						dry::updateProgramDisplay		(::dry::SApplication & app) {
	::gpk::SGUI								& gui							= *app.Framework.GUI;
	::dry::SScreenHome						& screenHome					= app.Screens.Home;
	const ::dry::SDryerProgram				& dryerProgram					= app.Dryer.ProgramList[app.Dryer.ProgramSelected];
	char_t									bufferTemp	[32]				= {};
	e_if(errored(::gpk::controlTextSet(gui, screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_VALUE_NAME], dryerProgram.Name)), "%s", "");
	snprintf(bufferTemp, ::gpk::size(bufferTemp) - 2, "%u\xf8\x43"	, dryerProgram.Temperature		); ::updateControlIfNeeded(app, bufferTemp, screenHome.Strings.ProgramTemperature		, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_VALUE_TEMPERATURE		);
	snprintf(bufferTemp, ::gpk::size(bufferTemp) - 2, "%i mbar"		, dryerProgram.Pressure			); ::updateControlIfNeeded(app, bufferTemp, screenHome.Strings.ProgramPressure			, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_VALUE_PRESSURE			);
	snprintf(bufferTemp, ::gpk::size(bufferTemp) - 2, "%u min"		, dryerProgram.PreHeatingTime	); ::updateControlIfNeeded(app, bufferTemp, screenHome.Strings.ProgramPreHeatingTime	, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_VALUE_PRE_HEATING_TIME	);
	snprintf(bufferTemp, ::gpk::size(bufferTemp) - 2, "%u min"		, dryerProgram.SubcycleDuration	); ::updateControlIfNeeded(app, bufferTemp, screenHome.Strings.ProgramSubcycleDuration	, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_VALUE_SUBCYCLE_DURATION	);
	snprintf(bufferTemp, ::gpk::size(bufferTemp) - 2, "%u"			, dryerProgram.Subcycles		); ::updateControlIfNeeded(app, bufferTemp, screenHome.Strings.ProgramSubcycles			, ::dry::SCREEN_CONTROL_HOME_PROGRAM_LABEL_VALUE_SUBCYCLES			);

	if(app.Dryer.State.RunningPending || app.Dryer.State.Running) {
		gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_PROGRAM_PREV]].Disabled = true;
		gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_PROGRAM_NEXT]].Disabled = true;
	}
	else {
		gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_PROGRAM_PREV]].Disabled = false;
		gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_PROGRAM_NEXT]].Disabled = false;
	}
	app.Framework.MainDisplay.Repaint	= true;
	return 0;
}

::gpk::error_t						dry::updateCatalogDisplay		(::dry::SApplication & app) {
	::dry::SScreenHome						& screenHome					= app.Screens.Home;
	::gpk::error_t							indexSpoolList					= screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_SPOOL_SCROLLABLE];
	::gpk::SGUI								& gui							= *app.Framework.GUI;
	::gpk::vci32							menuItemControls				= gui.Controls.Children[indexSpoolList];
	for(uint32_t iSpool = 0, countSpools = app.Catalog.FilamentLoaded.size(); iSpool < countSpools; ++iSpool) {
		::gpk::error_t							indexControl					= -1;
		if(iSpool < menuItemControls.size())
			indexControl						= menuItemControls[iSpool];
		else {
			indexControl						= ::gpk::controlCreate(gui);
			::gpk::controlSetParent(gui, indexControl, indexSpoolList);
		}
		::gpk::SControl							& controlFilament				= gui.Controls.Controls[indexControl];
		::gpk::SControlText						& controlText					= gui.Controls.Text[indexControl];
		controlFilament.Align				= ::gpk::ALIGN_TOP_LEFT;
		controlFilament.Area.Offset.y		= int16_t(24 * iSpool);
		controlFilament.Area.Size.y			= 24;
		controlFilament.Border				= {1, 1, 1, 1};
		char_t									bufferFormat[4096]				= {};
		snprintf(bufferFormat, ::gpk::size(bufferFormat), "%s - %s - %s - %.2fmm"
			, ::gpk::toString(app.Catalog.Material	[app.Catalog.FilamentLoaded[iSpool].Material]).begin()
			, ::gpk::toString(app.Catalog.Brand		[app.Catalog.FilamentLoaded[iSpool].Brand	]).begin()
			, ::gpk::toString(app.Catalog.Color		[app.Catalog.FilamentLoaded[iSpool].Color	]).begin()
			, app.Catalog.Diameter[app.Catalog.FilamentLoaded[iSpool].Diameter]
			);
		controlText.Text					= ::gpk::label(bufferFormat);
	}
	for(uint32_t iSpool = app.Catalog.FilamentLoaded.size(), countSpools = menuItemControls.size(); iSpool < countSpools; ++iSpool) {
		const ::gpk::error_t					indexControl					= menuItemControls[iSpool];
		::gpk::SControl							& controlFilament				= gui.Controls.Controls[indexControl];
		::gpk::SControlText						& controlText					= gui.Controls.Text[indexControl];
		controlFilament.Border				= {};
		controlText.Text					= "";
	}

	::dry::handleResizeScreenHome(app, gui.LastSize.Cast<int16_t>());
	return 0;
}

static	::gpk::error_t				formatTimerString				(int64_t timeInMilliseconds, ::gpk::vc target) {
	const int64_t							timeInSeconds					= timeInMilliseconds / 1000;
	const int64_t							timeInMinutes					= timeInSeconds / 60;
	const uint32_t							hours							= (uint32_t)(timeInMinutes / 60);
	const uint32_t							mins							= (uint32_t)(timeInMinutes) % 60;
	const uint32_t							secs							= (uint32_t)(timeInSeconds) % 60;
	if(1 == hours)
		snprintf(target.begin(), target.size() - 2, "%u:%2.2uHr", hours, mins);
	else if (hours)
		snprintf(target.begin(), target.size() - 2, "%u:%2.2uHs", hours, mins);
	else
		snprintf(target.begin(), target.size() - 2, "%u:%2.2um", mins, secs);
	return 0;
}

static	::gpk::error_t				updateSessionDisplay			(::dry::SApplication & app)	{
	::dry::SScreenHome						& screenHome					= app.Screens.Home;
	::dry::SDryerState						& dryerState					= app.Dryer.State;
	char_t									bufferTemp	[32]				= {};
	::formatTimerString(dryerState.TimeElapsedTotal				, bufferTemp);	::updateControlIfNeeded(app, bufferTemp, screenHome.Strings.ElapsedTimeTotal		, ::dry::SCREEN_CONTROL_HOME_ELAPSED_TIME_TIME);
	::formatTimerString(dryerState.TimeElapsedCycleDrying		, bufferTemp);	::updateControlIfNeeded(app, bufferTemp, screenHome.Strings.ElapsedTimeDrying		, ::dry::SCREEN_CONTROL_HOME_DRYING_TIME_TIME);
	::formatTimerString(dryerState.TimeElapsedCyclePrePreheating, bufferTemp);	::updateControlIfNeeded(app, bufferTemp, screenHome.Strings.ElapsedTimePrePreheating, ::dry::SCREEN_CONTROL_HOME_PRE_PREHEATING_TIME_TIME);
	::formatTimerString(dryerState.TimeElapsedCyclePreheating	, bufferTemp);	::updateControlIfNeeded(app, bufferTemp, screenHome.Strings.ElapsedTimePreheating	, ::dry::SCREEN_CONTROL_HOME_PREHEATING_TIME_TIME);
	::formatTimerString(dryerState.TimeRemaining				, bufferTemp);	::updateControlIfNeeded(app, bufferTemp, screenHome.Strings.RemainingTime			, ::dry::SCREEN_CONTROL_HOME_REMAINING_TIME_TIME);
	snprintf(bufferTemp, ::gpk::size(bufferTemp) - 2, "%u", dryerState.CurrentCycle + 1);
	::updateControlIfNeeded(app, bufferTemp, screenHome.Strings.CurrentCycle, ::dry::SCREEN_CONTROL_HOME_CURRENT_CYCLE_NUMBER);
	return 0;
}

::gpk::error_t						dry::updateScreenHome			(::dry::SApplication & app) {
	for(uint32_t iDryerEvent = 0; iDryerEvent < app.Dryer.EventList.size(); ++iDryerEvent) {
		::dry::DRYER_EVENT						dryerEvent						= app.Dryer.EventList[iDryerEvent];
		::dry::SScreenHome						& screenHome					= app.Screens.Home;
		::gpk::SGUI								& gui							= *app.Framework.GUI;
		switch(dryerEvent) {
		case ::dry::DRYER_EVENT_FINISHED:
		case ::dry::DRYER_EVENT_STOPPED	:
			gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_PROGRAM_STOP				]].Hidden	= true;
			gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_PROGRAM_START				]].Hidden	= false;
			//gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_CURRENT_CYCLE_LABEL		]].Disabled	=
			gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_DRYING_TIME_LABEL			]].Disabled	=
			gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_PRE_PREHEATING_TIME_LABEL	]].Disabled	=
			gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_PREHEATING_TIME_LABEL		]].Disabled	= true;
			::gpk::controlTextSet(gui, screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_SESSION_STATUS_VALUE], "READY");
			::dry::updateProgramDisplay(app);
			app.Framework.MainDisplay.Repaint	= true;
			break;
		case ::dry::DRYER_EVENT_STARTED	:
			::dry::modalClose(app);
			gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_CURRENT_CYCLE_LABEL]].Disabled	=
			gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_PROGRAM_STOP		]].Hidden	= false;
			gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_PROGRAM_START		]].Hidden	= true;
			break;
		case ::dry::DRYER_EVENT_PRE_PREHEATING_STARTED	:
			gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_PRE_PREHEATING_TIME_LABEL	]].Disabled	= false;
			gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_DRYING_TIME_LABEL			]].Disabled	=
			gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_PREHEATING_TIME_LABEL		]].Disabled	= true;
			::gpk::controlTextSet(gui, screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_SESSION_STATUS_VALUE], "RAISING TEMPERATURE");
			::dry::updateProgramDisplay(app);
			app.Framework.MainDisplay.Repaint	= true;
			break;
		case ::dry::DRYER_EVENT_PREHEATING_STARTED	:
			gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_PREHEATING_TIME_LABEL		]].Disabled	= false;
			gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_PRE_PREHEATING_TIME_LABEL	]].Disabled	=
			gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_DRYING_TIME_LABEL			]].Disabled	= true;
			::gpk::controlTextSet(gui, screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_SESSION_STATUS_VALUE], "PREHEATING");
			::dry::updateProgramDisplay(app);
			app.Framework.MainDisplay.Repaint	= true;
			break;
		case ::dry::DRYER_EVENT_DRYING_STARTED	:
			gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_DRYING_TIME_LABEL			]].Disabled	= gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_PROGRAM_STOP ]].Hidden = false;
			gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_PREHEATING_TIME_LABEL		]].Disabled	=
			gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_PRE_PREHEATING_TIME_LABEL	]].Disabled	=
			gui.Controls.States[screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_PROGRAM_START				]].Hidden = true;
			::gpk::controlTextSet(gui, screenHome.ControlIds[::dry::SCREEN_CONTROL_HOME_SESSION_STATUS_VALUE], "DRYING");
			::dry::updateProgramDisplay(app);
			app.Framework.MainDisplay.Repaint	= true;
			break;
		}

		switch(dryerEvent) {
		case ::dry::DRYER_EVENT_FINISHED:
			::dry::modalDisplay(app, ::dry::MODAL_DRYING_COMPLETE);
			break;
		}
	}
	e_if(::updateGaugesFromState(app, false), "%s", "Failed to update gauges.");
	if(app.Dryer.State.Running) { // Update timers text
		e_if(::updateSessionDisplay(app), "%s", "Failed to update session gui.");
	}
	else {

	}
	return 0;
}

::gpk::error_t						dry::handleControlHome			(::dry::SApplication & app, int32_t idControl)	{
	switch((uint32_t)idControl) {
	case dry::SCREEN_CONTROL_HOME_SPOOL_LIST_PREV:
		if (app.Catalog.FilamentLoaded.size()) {
			--app.Screens.Home.IndexSpool;
			if (app.Screens.Home.IndexSpool < 0)
				app.Screens.Home.IndexSpool			= int32_t(app.Catalog.FilamentLoaded.size()) - 1;
			::dry::updateProgramDisplay(app);
		}
		break;
	case dry::SCREEN_CONTROL_HOME_SPOOL_LIST_NEXT:
		if (app.Catalog.FilamentLoaded.size()) {
			app.Screens.Home.IndexSpool					= int32_t(app.Screens.Home.IndexSpool + 1) % app.Catalog.FilamentLoaded.size();
			::dry::updateProgramDisplay(app);
		}
		break;
	case dry::SCREEN_CONTROL_HOME_PROGRAM_PREV:
		if (app.Dryer.ProgramList.size()) {
			--app.Dryer.ProgramSelected;
			if(app.Dryer.ProgramSelected < 0)
				app.Dryer.ProgramSelected			= int32_t(app.Dryer.ProgramList.size()) - 1;
			::dry::updateProgramDisplay(app);
		}
		break;
	case dry::SCREEN_CONTROL_HOME_PROGRAM_NEXT:
		if (app.Dryer.ProgramList.size()) {
			app.Dryer.ProgramSelected			= int32_t(app.Dryer.ProgramSelected + 1) % app.Dryer.ProgramList.size();
			::dry::updateProgramDisplay(app);
		}
		break;
	case dry::SCREEN_CONTROL_HOME_PROGRAM_START:
		if (false == app.Dryer.State.Running) {
			app.Dryer.State.RunningPending		= true;
			if(app.Dryer.State.DoorClosed) {
			}
			else {
				::dry::modalDisplay(app, ::dry::MODAL_CLOSE_DOOR);
			}
		}
		break;
	case dry::SCREEN_CONTROL_HOME_PROGRAM_STOP:
		if (app.Dryer.State.Running) {
			::dry::modalDisplay(app, dry::MODAL_DRYING_STOP);
			//app.Dryer.State.DryingStopped		= true;
			::updateGaugesFromState(app, true);
		}
		break;
	}
	return 0;
}
