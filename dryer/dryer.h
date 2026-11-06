#include "gpk_array.h"
#include "gpk_array_static.h"
#include "gpk_json.h"
#include "gpk_string.h"


#ifndef DRYER_H_0927439821
#define DRYER_H_0927439821

namespace dry
{
	usns gpk;
#pragma pack(push, 1)
	// "<T:{},P:{},H:{},D:{},C:{},E:{}>"
    //def preassure_on(self, prea):
    //    data = '<ON {}>'.format(prea.decode('ascii', 'ignore'))
	//
    //def preassure_off(self):
    //    self.send('<OFF>')
	//
    //def status(self):
    //    self.send('<STATUS>')
	//
    //def set_temperature(self, temp):
    //    data = '<TEMP {}>'.format(temp.decode('ascii', 'ignore'))
	//
	GDEFINE_ENUM_TYPE(DRYER_EVENT, u0_t );
	GDEFINE_ENUM_VALUE(DRYER_EVENT, STARTED					, 0);
	GDEFINE_ENUM_VALUE(DRYER_EVENT, PRE_PREHEATING_STARTED	, 1);
	GDEFINE_ENUM_VALUE(DRYER_EVENT, PREHEATING_STARTED		, 2);
	GDEFINE_ENUM_VALUE(DRYER_EVENT, DRYING_STARTED			, 3);
	GDEFINE_ENUM_VALUE(DRYER_EVENT, CYCLE_CHANGED			, 4);
	GDEFINE_ENUM_VALUE(DRYER_EVENT, STOPPED					, 5);
	GDEFINE_ENUM_VALUE(DRYER_EVENT, FINISHED				, 6);
	GDEFINE_ENUM_VALUE(DRYER_EVENT, DOOR_OPEN				, 7);
	GDEFINE_ENUM_VALUE(DRYER_EVENT, DOOR_CLOSED				, 8);

	GDEFINE_ENUM_TYPE(DRYER_COMMAND, u0_t );
	GDEFINE_ENUM_VALUE(DRYER_COMMAND, NOP					, 0);
	GDEFINE_ENUM_VALUE(DRYER_COMMAND, PUMP					, 1);
	GDEFINE_ENUM_VALUE(DRYER_COMMAND, TEMPERATURE			, 2);

#pragma pack(push, 1)
	struct SDryerCommand {
		DRYER_COMMAND							Command							= {};
		s0_t 									Args							= {};
	};
#pragma pack(pop)

	struct SDryerMessage {
		float									Temperature						= {};
		float									Pressure						= {};
		float									Humidity						= {};
		u2_t 								Cycle							= {};
		::gpk::string					 		Error							= {};
		bool									DoorClosed						= {};
	};

	struct SDryerProgram {
		::gpk::vcst_t 							Name							= "undefined";
		u2_t 								Temperature						= 45;
		u2_t 								Pressure						= 0;
		u2_t 								PreHeatingTime					= 0;
		u2_t 								SubcycleDuration				= 0;
		u2_t 								Subcycles						= 1;
	};

	struct SDryerState {
		s3_t									TimeStarted						= 0;
		s3_t									TimeElapsedCyclePrePreheating	= 0;	// the entire
		s3_t									TimeElapsedCyclePreheating		= 0;
		s3_t									TimeElapsedCycleDrying			= 0;
		s3_t									TimeElapsedTotal				= 0;
		s3_t									TimeRemaining					= 0;
		s3_t									DryerTime						= 0;
		u2_t 								CurrentCycle					= 0;

		float									Temperature						= 0;
		float									Vacuum							= 0;
		float									Humidity						= 100;

		bool									DoorClosed						: 1;
		bool									RunningPending					: 1;
		bool									Running							: 1;
		bool									CyclePrePreheating				: 1;
		bool									CyclePreheating					: 1;
		bool									CycleDrying						: 1;
		bool									StoppedPending					: 1;
		bool									Dryer							: 1;
		bool									Pump							: 1;
	};
#pragma pack(pop)

	struct SDryer {
		::dry::SDryerState						State							= {};
		::gpk::aobj<SDryerMessage>				MessagesReceived				= {};
		::gpk::aobj<SDryerProgram>				ProgramList						= {};
		::gpk::apod<DRYER_EVENT>				EventList						= {};
		::gpk::apod<SDryerCommand>				CommandList						= {};
		::gpk::SJSONFile						ProgramFile						= {};
		::dry::SDryerProgram					CurrentProgram					= {};
		s2_t									ProgramSelected					= -1;

		::gpk::refcount_t						CloseSerialThread				= 0;

		volatile			s2_t				SerialLastTime					= 0;
		static constexpr	s2_t				SERIAL_TIMEOUT					= 10;
	};

	::gpk::err_t							dryerSetup						(::dry::SDryer & dryer);
	::gpk::err_t							dryerUpdate						(::dry::SDryer & dryer, u3_t  lastTimeMilliseconds);

	static constexpr u0_t 				DRYER_MIN_TEMPERATURE			= 45;

	static constexpr u2_t 				DRYER_MAX_TEMPERATURE			= 83;
	static constexpr u2_t 				DRYER_MAX_PRESSURE				= 800;
	static constexpr u2_t 				DRYER_MAX_TIME_PREHEATING		= 60;
	static constexpr u2_t 				DRYER_MAX_TIME_SUBCYCLE			= 120;
	static constexpr u2_t 				DRYER_MAX_SUBCYCLES				= 64;

	static constexpr u2_t 				DRYER_DEFAULT_TEMPERATURE		= 45;
	static constexpr u2_t 				DRYER_DEFAULT_PRESSURE			= 600;
	static constexpr u2_t 				DRYER_DEFAULT_TIME_PREHEATING	= 10;
	static constexpr u2_t 				DRYER_DEFAULT_TIME_SUBCYCLE		= 40;
	static constexpr u2_t 				DRYER_DEFAULT_SUBCYCLES			= 3;
} // namespace

#endif // DRYER_H_0927439821
