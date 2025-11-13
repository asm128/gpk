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

	struct SPlayer {
		::gpk::vcsc_t			Name				= "Player 1";
		uint32_t			Color				= 0xFFFF0000;
		::gpk::PLAYER_MODE	Mode				= (::gpk::PLAYER_MODE)::gpk::PLAYER_MODE_AI;
	};

	GDEFINE_ENUM_TYPE(NETWORK_MODE, uint8_t);
	GDEFINE_ENUM_VALUE(NETWORK_MODE, Offline, 0);
	GDEFINE_ENUM_VALUE(NETWORK_MODE, LAN	, 1);
	GDEFINE_ENUM_VALUE(NETWORK_MODE, Online	, 2);

#pragma pack(push, 1)
	struct SGameSetup {
		uint32_t			OffsetStage			= 0;
		::gpk::NETWORK_MODE	NetworkMode			= ::gpk::NETWORK_MODE_Offline;
	};

	struct SGameState {
		uint32_t			Stage				= 0;
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

		::gpk::error_t		Save				(::gpk::au0_t & output)	const	{ 
			gpk_necs(gpk::savePOD(output, Setup)); 
			gpk_necs(gpk::savePOD(output, State)); 
			gpk_necs(gpk::saveView(output, Stages));
			return 0;
		}

		::gpk::error_t		Load				(::gpk::vcu0_t & input)	{ 
			gpk_necs(gpk::loadPOD(input, Setup));
			gpk_necs(gpk::loadPOD(input, State)); 
			gpk_necs(gpk::loadView(input, Stages));
			return 0;
		}

	};
} // namespace

#endif // GPK_GAME_H
