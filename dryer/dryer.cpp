#include "dryer.h"

static	::gpk::error_t				addDefaultProgram					(::dry::SDryer & dryer) {
	::dry::SDryerProgram					loadedProgram						= {};
	loadedProgram.Name					= "default";
	loadedProgram.Pressure				= ::dry::DRYER_DEFAULT_PRESSURE			;
	loadedProgram.Temperature			= ::dry::DRYER_DEFAULT_TEMPERATURE		;
	loadedProgram.PreHeatingTime		= ::dry::DRYER_DEFAULT_TIME_PREHEATING	;
	loadedProgram.SubcycleDuration		= ::dry::DRYER_DEFAULT_TIME_SUBCYCLE	;
	loadedProgram.Subcycles				= ::dry::DRYER_DEFAULT_SUBCYCLES		;
	if_fail_fe(dryer.ProgramList.push_back(loadedProgram));
	return 0;
}

static	::gpk::error_t				dryerProgramListLoad				(::dry::SDryer & dryer, ::gpk::vcst_t programsFilename) {
	if_fail_ef(::gpk::jsonFileRead(dryer.ProgramFile, programsFilename), "Failed to load programs file: '%.*s'", GPK_VIEW_ARGS(programsFilename))
	else {
		if_zero_ef(dryer.ProgramFile.Reader.Token.size(), "%.*s", GPK_VIEW_ARGS(programsFilename))
		else {
			::gpk::pnco<::gpk::SJSONNode>		rootArray							= dryer.ProgramFile.Reader[0];
			::gpk::err_t						programCount						;
			if_fail_ef(programCount = ::gpk::jsonArraySize(*rootArray), "Invalid root json type: '%s'", ::gpk::get_value_namep(rootArray->Token->Type))
			for(gpk::s2_t iProgram = 0; iProgram < programCount; ++iProgram) {
				::dry::SDryerProgram					loadedProgram						= {};
				const int32_t							jsonIndexProgram					= ::gpk::jsonArrayValueGet(*rootArray, iProgram);
				const int32_t							jsonIndexProgramName				= ::gpk::jsonObjectValueGet(dryer.ProgramFile.Reader, jsonIndexProgram, "name"				);
				const int32_t							jsonIndexProgramPressure			= ::gpk::jsonObjectValueGet(dryer.ProgramFile.Reader, jsonIndexProgram, "pressure"			);
				const int32_t							jsonIndexProgramTemperature			= ::gpk::jsonObjectValueGet(dryer.ProgramFile.Reader, jsonIndexProgram, "temperature"		);
				const int32_t							jsonIndexProgramPreHeatingTime		= ::gpk::jsonObjectValueGet(dryer.ProgramFile.Reader, jsonIndexProgram, "pre_heating_time"	);
				const int32_t							jsonIndexProgramSubcycleDuration	= ::gpk::jsonObjectValueGet(dryer.ProgramFile.Reader, jsonIndexProgram, "subcycle_duration"	);
				const int32_t							jsonIndexProgramSubcycles			= ::gpk::jsonObjectValueGet(dryer.ProgramFile.Reader, jsonIndexProgram, "subcycles"			);
				if_fail_ce(jsonIndexProgramName				, "Failed to retrieve program key: '%s'. Skipping program load.", "name"				);
				if_fail_ce(jsonIndexProgramPressure			, "Failed to retrieve program key: '%s'. Skipping program load.", "pressure"			);
				if_fail_ce(jsonIndexProgramTemperature		, "Failed to retrieve program key: '%s'. Skipping program load.", "temperature"			);
				if_fail_ce(jsonIndexProgramPreHeatingTime	, "Failed to retrieve program key: '%s'. Skipping program load.", "pre_heating_time"	);
				if_fail_ce(jsonIndexProgramSubcycleDuration	, "Failed to retrieve program key: '%s'. Skipping program load.", "subcycle_duration"	);
				if_fail_ce(jsonIndexProgramSubcycles		, "Failed to retrieve program key: '%s'. Skipping program load.", "subcycles"			);
				loadedProgram.Name					= dryer.ProgramFile.Reader.View				[jsonIndexProgramName				];
				loadedProgram.Pressure				= (int32_t)dryer.ProgramFile.Reader.Token	[jsonIndexProgramPressure			].Value;
				loadedProgram.Temperature			= (int32_t)dryer.ProgramFile.Reader.Token	[jsonIndexProgramTemperature		].Value;
				loadedProgram.PreHeatingTime		= (int32_t)dryer.ProgramFile.Reader.Token	[jsonIndexProgramPreHeatingTime		].Value;
				loadedProgram.SubcycleDuration		= (int32_t)dryer.ProgramFile.Reader.Token	[jsonIndexProgramSubcycleDuration	].Value;
				loadedProgram.Subcycles				= (int32_t)dryer.ProgramFile.Reader.Token	[jsonIndexProgramSubcycles			].Value;
				info_printf("Program Name             : %s."			, ::gpk::string(loadedProgram.Name).begin());
				info_printf("Program Pressure         : %i mbar."		, loadedProgram.Pressure		);
				info_printf("Program Temperature      : %i \xf8\x43."	, loadedProgram.Temperature		);
				info_printf("Program PreHeatingTime   : %i min."		, loadedProgram.PreHeatingTime	);
				info_printf("Program SubcycleDuration : %i min."		, loadedProgram.SubcycleDuration);
				info_printf("Program Subcycles        : %i."			, loadedProgram.Subcycles		);
				if_true_ce(false == ::gpk::in_range(loadedProgram.Pressure			, 0U, ::dry::DRYER_MAX_PRESSURE			+ 1U), "Pressure         value '%i' out of range for program '%s'", loadedProgram.Pressure			, ::gpk::string(loadedProgram.Name).begin());
				if_true_ce(false == ::gpk::in_range(loadedProgram.Temperature		, 0U, ::dry::DRYER_MAX_TEMPERATURE		+ 1U), "Temperature      value '%i' out of range for program '%s'", loadedProgram.Temperature		, ::gpk::string(loadedProgram.Name).begin());
				if_true_ce(false == ::gpk::in_range(loadedProgram.PreHeatingTime	, 0U, ::dry::DRYER_MAX_TIME_PREHEATING	+ 1U), "PreHeatingTime   value '%i' out of range for program '%s'", loadedProgram.PreHeatingTime	, ::gpk::string(loadedProgram.Name).begin());
				if_true_ce(false == ::gpk::in_range(loadedProgram.SubcycleDuration	, 0U, ::dry::DRYER_MAX_TIME_SUBCYCLE	+ 1U), "SubcycleDuration value '%i' out of range for program '%s'", loadedProgram.SubcycleDuration	, ::gpk::string(loadedProgram.Name).begin());
				if_true_ce(false == ::gpk::in_range(loadedProgram.Subcycles			, 0U, ::dry::DRYER_MAX_SUBCYCLES		+ 1U), "Subcycles        value '%i' out of range for program '%s'", loadedProgram.Subcycles			, ::gpk::string(loadedProgram.Name).begin());
				if(loadedProgram.Name == ::gpk::vcst_t{"default"}) {
					dryer.ProgramSelected				= dryer.ProgramList.size();
				}
				if_fail_ce(dryer.ProgramList.push_back(loadedProgram));
			}
		}
	}
	if(0 == dryer.ProgramList.size())
		gpk_necall(::addDefaultProgram(dryer), "%s", "");

	if((dryer.ProgramSelected == -1 && dryer.ProgramList.size())	// Ensure a valid program selected.
	 || dryer.ProgramSelected >= (int32_t)dryer.ProgramList.size()
	)
		dryer.ProgramSelected				= 0;
	return 0;
}

::gpk::error_t						dry::dryerSetup				(::dry::SDryer & dryer) {
	::gpk::vcs								programsFilename			= "./program.json";
	gpk_necall(::dryerProgramListLoad(dryer, programsFilename), "%s", "");
	return 0;
}

static	constexpr	int64_t			minutesToMillisec			(int64_t minutes)							{ return 60 * 1000 * minutes; }
static	constexpr	int64_t			totalCycleTimeInMillisec	(const ::dry::SDryerProgram	& dryerProgram) { return ::minutesToMillisec(dryerProgram.PreHeatingTime + dryerProgram.SubcycleDuration); }
static	constexpr	int64_t			preheatingTimeInMillisec	(const ::dry::SDryerProgram	& dryerProgram) { return ::minutesToMillisec(dryerProgram.PreHeatingTime); }
static	constexpr	int64_t			cycleTimeInMillisec			(const ::dry::SDryerProgram	& dryerProgram) { return ::minutesToMillisec(dryerProgram.SubcycleDuration); }

::gpk::error_t						dry::dryerUpdate			(::dry::SDryer & dryer, uint64_t lastTimeMilliseconds) {
	dryer.EventList.clear();
	::dry::SDryerState						& dryerState				= dryer.State;
	if(dryerState.Running) {
		if(dryerState.StoppedPending) { // Stop dryer
			dryerState.RunningPending			= false;
			dryerState.Running					= false;
			dryerState.CyclePrePreheating		= false;
			dryerState.CyclePreheating			= false;
			dryerState.CycleDrying				= false;
			dryerState.StoppedPending			= false;
			dryer.CommandList.push_back({::dry::DRYER_COMMAND_PUMP, 0});
			dryer.CommandList.push_back({::dry::DRYER_COMMAND_TEMPERATURE, (int8_t)::dry::DRYER_MIN_TEMPERATURE});
			dryer.EventList.push_back(::dry::DRYER_EVENT_STOPPED);
		}
		else if(dryerState.DoorClosed) {
			dryerState.TimeElapsedTotal			+= lastTimeMilliseconds;
			dryerState.TimeRemaining			-= lastTimeMilliseconds;
			if(dryerState.CyclePrePreheating) {
				dryerState.TimeElapsedCyclePrePreheating	+= lastTimeMilliseconds;
				dryerState.TimeRemaining					+= lastTimeMilliseconds;	// compensate for unknown warming-up time
				if(dryerState.Temperature >= dryer.CurrentProgram.Temperature) {
					dryerState.CyclePrePreheating			= false;
					dryerState.CyclePreheating				= true;
					dryerState.TimeElapsedCyclePreheating	= 0;
					dryer.EventList.push_back(::dry::DRYER_EVENT_PREHEATING_STARTED);
				}
			}
			else if(dryerState.CyclePreheating) {
				dryerState.TimeElapsedCyclePreheating	+= lastTimeMilliseconds;
				if(dryerState.TimeElapsedCyclePreheating >= ::preheatingTimeInMillisec(dryer.CurrentProgram)) {
					dryerState.CyclePreheating				= false;
					dryerState.CycleDrying					= true;
					dryerState.TimeElapsedCycleDrying		= 0;
					dryerState.Pump							= true;
					dryer.CommandList.push_back({::dry::DRYER_COMMAND_PUMP, 1});
					dryer.EventList.push_back(::dry::DRYER_EVENT_DRYING_STARTED);
				}
			}
			else {
				dryerState.TimeElapsedCycleDrying		+= lastTimeMilliseconds;
				if(dryerState.TimeElapsedCycleDrying >= ::cycleTimeInMillisec(dryer.CurrentProgram)) {
					++dryerState.CurrentCycle;
					if(dryerState.CurrentCycle < dryer.CurrentProgram.Subcycles) {
						dryerState.TimeElapsedCyclePreheating	= 0;
						dryerState.CyclePreheating				= true;
						dryerState.Pump							= false;
						dryer.EventList.push_back(::dry::DRYER_EVENT_CYCLE_CHANGED);
						dryer.EventList.push_back(::dry::DRYER_EVENT_PREHEATING_STARTED);
						dryer.CommandList.push_back({::dry::DRYER_COMMAND_PUMP, 0});
					}
					else {
						dryerState.Running					= false;
						dryerState.Pump 					= false;
						dryer.EventList.push_back(::dry::DRYER_EVENT_FINISHED);
					}
				}
			}
		}
	}
	else {
		if(dryerState.RunningPending && dryerState.DoorClosed) {
			if_fail_ef(dryer.ProgramSelected == -1, "%s", "No program selected")
			else {
				::dry::SDryerProgram							& programSelected		= dryer.ProgramList[dryer.ProgramSelected];
				dryer.CurrentProgram						= programSelected;
				// - reset state flags
				dryerState.Running							= true;
				dryerState.CyclePrePreheating				= true;
				dryerState.RunningPending					= false;
				dryerState.StoppedPending					= false;
				dryerState.CurrentCycle						= 0;
				// - reset timers
				dryerState.TimeElapsedCycleDrying			=
				dryerState.TimeElapsedCyclePrePreheating	=
				dryerState.TimeElapsedCyclePreheating		=
				dryerState.TimeElapsedTotal					= 0;
				// - reset gauges
				dryerState.Temperature						= 0;
				dryerState.Vacuum							= 0;
				dryerState.Humidity							= 100;
				dryerState.TimeRemaining					= ::totalCycleTimeInMillisec(dryer.CurrentProgram) * dryer.CurrentProgram.Subcycles;
				// -
				dryerState.Pump								= false;

				dryer.EventList.push_back(::dry::DRYER_EVENT_STARTED);
				dryer.EventList.push_back(::dry::DRYER_EVENT_PRE_PREHEATING_STARTED);
				dryer.CommandList.push_back({::dry::DRYER_COMMAND_PUMP, 0});
				dryer.CommandList.push_back({::dry::DRYER_COMMAND_TEMPERATURE, (int8_t)dryer.CurrentProgram.Temperature});
			}
		}
	}

	if(false == dryerState.Dryer) {
		static bool								invert0					= false;
		invert0
			= (dryerState.Temperature > ((int8_t)::dry::DRYER_MIN_TEMPERATURE - 5)) ? true
			: (dryerState.Temperature <= 0) ? false
			: invert0;
		if(invert0) {
			dryerState.Temperature				-= 1;
			dryerState.Vacuum					-= 10;
			dryerState.Humidity					+= 1;
		}
		else {
			dryerState.Temperature				+= 1;
			dryerState.Vacuum					+= 10;
			dryerState.Humidity					-= 1;
		}
	}
	return 0;
}
