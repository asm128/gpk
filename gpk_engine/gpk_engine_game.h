#include "gpk_engine_game_stage.h"

#ifndef GPK_ENGINE_GAME_H_23627
#define GPK_ENGINE_GAME_H_23627

namespace gpk
{
#pragma pack(push, 1)
	GDEFINE_ENUM_TYPE(PLAY_MODE, uint8_t);
	GDEFINE_ENUM_VALUE(PLAY_MODE, Offline		, 0);
	GDEFINE_ENUM_VALUE(PLAY_MODE, Online		, 1);

	struct SGameSetup {
		uint32_t			OffsetStage				= 0;
		::gpk::PLAY_MODE	PlayMode				= (::gpk::PLAY_MODE)::gpk::PLAY_MODE_Offline;
	};

	struct SGameState {
		uint64_t			Stage					= 0;
		SStageTime			UserTime				= {};
		bool				Paused					= false;
		double				CameraSwitchDelay		= 0;
	};

	typedef ::gpk::apod<SStageState>	astagestate;

	struct SGame {
		SGameSetup			Constants				= {};
		SGameState			State					= {};
		astagestate			Stages					= {};
	};
#pragma pack(pop)
} // namespace

#endif // GPK_ENGINE_GAMEPLAY_H_23627