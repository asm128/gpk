#include "gpk_game_stage.h"
#include "gpk_apod_serialize.h"

#ifndef GPK_GAME_H
#define GPK_GAME_H

namespace gpk
{
	GDEFINE_ENUM_TYPE(PLAYER_MODE, uint8_t);
	GDEFINE_ENUM_VALUE(PLAYER_MODE, AI			, 0);
	GDEFINE_ENUM_VALUE(PLAYER_MODE, Human		, 1);

	struct SPlayerInventory {
		//::gpk::PLAYER_MODE	Mode				= (::gpk::PLAYER_MODE)::gpk::PLAYER_MODE_AI;
	};

	struct SGamePlayer {
		::gpk::vcc			Name				= "Player 1";
		::gpk::PLAYER_MODE	Mode				= (::gpk::PLAYER_MODE)::gpk::PLAYER_MODE_AI;
		uint32_t			Color				= 0xFFFF0000;
	};

	GDEFINE_ENUM_TYPE(PLAY_MODE, uint8_t);
	GDEFINE_ENUM_VALUE(PLAY_MODE, Offline		, 0);
	GDEFINE_ENUM_VALUE(PLAY_MODE, Online		, 1);

#pragma pack(push, 1)
	struct SGameSetup {
		uint32_t			OffsetStage			= 0;
		::gpk::PLAY_MODE	PlayMode			= (::gpk::PLAY_MODE)::gpk::PLAY_MODE_Offline;
	};

	struct SGameState {
		uint64_t			Stage				= 0;
		SStageTime			UserTime			= {};
		bool				Paused				= false;
		double				CameraSwitchDelay	= 0;
	};
#pragma pack(pop)

	typedef ::gpk::apod<SStageState>	astagestate;

	struct SGame {
		SGameSetup			Setup				= {};
		SGameState			State				= {};
		astagestate			Stages				= {};

		::gpk::error_t		Save				(::gpk::au8 & output)	const	{ 
			gpk_necs(::gpk::savePOD(output, Setup)); 
			gpk_necs(::gpk::savePOD(output, State)); 
			gpk_necs(::gpk::saveView(output, Stages));
			return 0;
		}

		::gpk::error_t		Load				(::gpk::vcu8 & input)	{ 
			gpk_necs(::gpk::loadPOD(input, Setup));
			gpk_necs(::gpk::loadPOD(input, State)); 
			gpk_necs(::gpk::loadView(input, Stages));
			return 0;
		}

	};
} // namespace

#endif // GPK_GAME_H
