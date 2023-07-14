#include "gpk_game_stage.h"

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

#pragma pack(pop)

	struct SGame {
		SGameSetup			Setup					= {};
		SGameState			State					= {};
		astagestate			Stages					= {};

		::gpk::error_t			Save				(::gpk::au8 & output)	const	{ 
			gpk_necs(::gpk::savePOD(output, Setup)); 
			gpk_necs(::gpk::savePOD(output, State)); 
			gpk_necs(::gpk::saveView(output, Stages));
			return 0;
		}

		::gpk::error_t			Load				(::gpk::vcu8 & input)	{ 
			gpk_necs(::gpk::loadPOD(input, Setup));
			gpk_necs(::gpk::loadPOD(input, State)); 
			gpk_necs(::gpk::loadView(input, Stages));
			return 0;
		}

	};
} // namespace

#endif // GPK_ENGINE_GAMEPLAY_H_23627