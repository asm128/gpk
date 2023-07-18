#include "gpk_enum.h"

#ifndef GPK_ENGINE_GAME_STAGE_H_23627
#define GPK_ENGINE_GAME_STAGE_H_23627

namespace gpk
{
	GDEFINE_ENUM_TYPE(MATCH_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(MATCH_TYPE, Free			, 0);
	GDEFINE_ENUM_VALUE(MATCH_TYPE, Survival		, 1);
	GDEFINE_ENUM_VALUE(MATCH_TYPE, Campaign		, 2);
	GDEFINE_ENUM_VALUE(MATCH_TYPE, Deathmatch	, 3);
	GDEFINE_ENUM_VALUE(MATCH_TYPE, Tactical		, 4);

#pragma pack(push, 1)
	struct SStageSetup {
		uint64_t		Seed					= {};
		uint16_t		Players					= {};
		MATCH_TYPE		MatchType				= {};
		uint8_t			Padding1				= {};
		uint32_t		Padding4				= {};
	};
	stacxpr	size_t SIZE_SSTAGESETUP = sizeof(SStageSetup);

	struct SStageTime {
		uint64_t		Started					= {};	// Timestamp
		uint64_t		Loaded					= {};	// Timestamp
		double			Played					= {};	// This one represents the real-world time played, incremented on every tick while not paused.
	};

	struct SRelativeSpeed {
		float			Current					= -50;
		float			Target					= 20;
		float			Step					= 20;
		float			AccelerationControl		= 0;

		float			ClampEpsilon			(double epsilon = 0.0001) {
			const float			diff					= Target - Current;
			if(fabsf(diff < epsilon))
				Current			= Target;
			return diff;
		}

		float			Update					(double seconds) {
			if(AccelerationControl)
				return Current += float(seconds * Step * AccelerationControl);

			const bool			slowdown				= Current > Target;
			if(slowdown || Current < Target) {
				Current			+= float(seconds * Step) * (slowdown ? -1 : 1);
				ClampEpsilon();
			}
			return Current;
		}
	};
	stacxpr	size_t SIZE_SRELATIVESPEED = sizeof(SRelativeSpeed);

	struct SRelativeTime {
		float			Scale					= 1.0f;
		float			Step					= -.35f;	// the amount in which the scale decreases or increases PER SECOND.
		double			Seconds					= 0;		// Relative seconds elapsed--could be the real-world time scaled by the `Scale` member. 

		double			Update					(double realSeconds, float fasting) {
			if(Step < 0) {
				Scale			+= float(realSeconds * Step);
				if(Scale < .1)
					Step			= fasting;
			}
			else if(Scale < .999999) {
				Scale			+= float(realSeconds * Step);
				if(Scale > 1.0f)
					Scale			= 1.0f;
			}
			Seconds			+= realSeconds * Scale;
			return realSeconds * Scale;
		}
	};
	stacxpr	size_t SIZE_SRELATIVETIME = sizeof(SRelativeTime);

	struct SStageState {
		SStageSetup		Constants				= {};	// This sets up the root/default game mode
		SStageTime		Time					= {};	// This is  
		SRelativeTime	SimulatedTime			= {};	// this one represents the elapsed virtual-reality time. It gets scaled by special effects.
		SRelativeSpeed	BackgroundSpeed			= {};	// this one represents the speed in which the background planets and stars move. 
	};
	stacxpr	size_t SIZE_SSTAGESTATE = sizeof(SStageState);
#pragma pack(pop)
} // namespace

#endif // GPK_ENGINE_GAME_STAGE_H_23627