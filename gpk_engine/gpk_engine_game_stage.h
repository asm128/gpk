#include "gpk_enum.h"

#ifndef GPK_ENGINE_GAME_STAGE_H_23627
#define GPK_ENGINE_GAME_STAGE_H_23627

namespace gpk
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE(MATCH_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(MATCH_TYPE, Free			, 0);
	GDEFINE_ENUM_VALUE(MATCH_TYPE, Survival		, 1);
	GDEFINE_ENUM_VALUE(MATCH_TYPE, Campaign		, 2);
	GDEFINE_ENUM_VALUE(MATCH_TYPE, Deathmatch	, 3);
	GDEFINE_ENUM_VALUE(MATCH_TYPE, Tactical		, 4);

	//GDEFINE_ENUM_TYPE(SCENE_TYPE, uint8_t);Likely not needed
	//GDEFINE_ENUM_VALUE(SCENE_TYPE, OuterSpace	, 0);
	//GDEFINE_ENUM_VALUE(SCENE_TYPE, Planetary	, 1);
	//GDEFINE_ENUM_VALUE(SCENE_TYPE, Submarine	, 2);

	struct SStageSetup {
		uint16_t			CountPlayers			= {};
		MATCH_TYPE			MatchType				= {};
		MATCH_TYPE			MatchFlags				= {};
		uint32_t			Padding					= {};
		uint64_t			Seed					= {};
		//SCENE_TYPE			SceneType				= {};
		//SCENE_TYPE			SceneFlags				= {};
	};

	struct SStageTime {
		uint64_t			Started					= {};	// Timestamp
		uint64_t			Loaded					= {};	// Timestamp
		double				Played					= {};	// This one represents the real-world time played, incremented on every tick while not paused.
	};

	struct SRelativeSpeed {
		double				RelativeSpeedTarget		= 20;
		double				RelativeSpeedCurrent	= -50;
		double				AccelerationControl		= 0;
	};

	struct SRelativeTime {
		double				Scale					= 1.0f;
		double				Seconds					= 0;		// Relative seconds elapsed--could be the real-world time scaled by the `Scale` member. 
		double				Slowing					= .35f;
	};

	struct SStageState {
		SStageSetup			Constants				= {};	// This sets up the root/default game mode
		SStageTime			Time					= {};	// This is  
		SRelativeTime		SimulatedTime			= {};	// this one represents the elapsed virtual-reality time. It gets scaled by special effects.
		SRelativeSpeed		BackgroundSpeed			= {};	// this one represents the speed in which the background planets and stars move. 
	};
#pragma pack(pop)
} // namespace

#endif // GPK_ENGINE_GAME_STAGE_H_23627