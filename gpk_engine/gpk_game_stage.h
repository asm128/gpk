#include "gpk_enum.h"

#ifndef GPK_ENGINE_GAME_STAGE_H_23627
#define GPK_ENGINE_GAME_STAGE_H_23627

namespace gpk
{
	GDEFINE_ENUM_TYPE(MATCH_TYPE, u0_t);
	GDEFINE_ENUM_VALUE(MATCH_TYPE, Free			, 0);
	GDEFINE_ENUM_VALUE(MATCH_TYPE, Survival		, 1);
	GDEFINE_ENUM_VALUE(MATCH_TYPE, Campaign		, 2);
	GDEFINE_ENUM_VALUE(MATCH_TYPE, Deathmatch	, 3);
	GDEFINE_ENUM_VALUE(MATCH_TYPE, Tactical		, 4);

#pragma pack(push, 1)
	struct SStageSetup {
		u3_t		Seed					= {};
		u1_t		Players					= {};
		MATCH_TYPE		MatchType				= {};
		u0_t			Padding1				= {};
		u2_t		Padding4				= {};
	};
	stxp	size_t SIZE_SSTAGESETUP = sizeof(SStageSetup);

	struct SStageTime {
		u3_t		Started					= {};	// Timestamp
		u3_t		Loaded					= {};	// Timestamp
		f3_t			Played					= {};	// This one represents the real-world time played, incremented on every tick while not paused.
	};

	struct SRelativeSpeed {
		f2_t			Current					= -50;
		f2_t			Target					= 20;
		f2_t			Step					= 20;
		f2_t			AccelerationControl		= 0;

		f2_t			ClampEpsilon			(f3_t epsilon = 0.0001) {
			const f2_t			diff					= Target - Current;
			if(fabsf(diff < epsilon))
				Current			= Target;
			return diff;
		}

		f2_t			Update					(f3_t seconds) {
			if(AccelerationControl)
				return Current += f2_t(seconds * Step * AccelerationControl);

			const bool			slowdown				= Current > Target;
			if(slowdown || Current < Target) {
				Current			+= f2_t(seconds * Step) * (slowdown ? -1 : 1);
				ClampEpsilon();
			}
			return Current;
		}
	};
	stxp	size_t SIZE_SRELATIVESPEED = sizeof(SRelativeSpeed);

	struct SRelativeTime {
		f2_t			Scale					= 1.0f;
		f2_t			Step					= -.35f;	// the amount in which the scale decreases or increases PER SECOND.
		f3_t			Seconds					= 0;		// Relative seconds elapsed--could be the real-world time scaled by the `Scale` member. 

		f3_t			Update					(f3_t realSeconds, f2_t fasting) {
			if(Step < 0) {
				Scale			+= f2_t(realSeconds * Step);
				if(Scale < .1)
					Step			= fasting;
			}
			else if(Scale < .999999) {
				Scale			+= f2_t(realSeconds * Step);
				if(Scale > 1.0f)
					Scale			= 1.0f;
			}
			Seconds			+= realSeconds * Scale;
			return realSeconds * Scale;
		}
	};
	stxp	size_t SIZE_SRELATIVETIME = sizeof(SRelativeTime);

	struct SStageState {
		SStageSetup		Constants				= {};	// This sets up the root/default game mode
		SStageTime		Time					= {};	// This is  
		SRelativeTime	SimulatedTime			= {};	// this one represents the elapsed virtual-reality time. It gets scaled by special effects.
		SRelativeSpeed	BackgroundSpeed			= {};	// this one represents the speed in which the background planets and stars move. 
	};
	stxp	size_t SIZE_SSTAGESTATE = sizeof(SStageState);
#pragma pack(pop)
} // namespace

#endif // GPK_ENGINE_GAME_STAGE_H_23627