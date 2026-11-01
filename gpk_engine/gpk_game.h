#include "gpk_game_stage.h"
#include "gpk_apod_serialize.h"

#ifndef GPK_GAME_H
#define GPK_GAME_H

namespace gpk
{
	GDEFINE_ENUM_TYPE(PLAYER_MODE, u0_t);
	GDEFINE_ENUM_VALUE(PLAYER_MODE, AI			, 0);
	GDEFINE_ENUM_VALUE(PLAYER_MODE, Human		, 1);

	struct SPlayerInventory {
		//::gpk::PLAYER_MODE	Mode				= (::gpk::PLAYER_MODE)::gpk::PLAYER_MODE_AI;
	};

	struct SPlayer {
		::gpk::view<sc_t>		Name				= "Player 1";
		u2_t			Color				= 0xFFFF0000;
		::gpk::PLAYER_MODE	Mode				= (::gpk::PLAYER_MODE)::gpk::PLAYER_MODE_AI;
	};

	GDEFINE_ENUM_TYPE(NETWORK_MODE, u0_t);
	GDEFINE_ENUM_VALUE(NETWORK_MODE, Offline, 0);
	GDEFINE_ENUM_VALUE(NETWORK_MODE, LAN	, 1);
	GDEFINE_ENUM_VALUE(NETWORK_MODE, Online	, 2);

#pragma pack(push, 1)
	struct SGameSetup {
		u2_t			OffsetStage			= 0;
		::gpk::NETWORK_MODE	NetworkMode			= ::gpk::NETWORK_MODE_Offline;
	};

	struct SGameState {
		u2_t			Stage				= 0;
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

		::gpk::err_t		Save				(::gpk::au0_t & output)	const	{ 
			if_fail_fe(gpk::savePOD(output, Setup)); 
			if_fail_fe(gpk::savePOD(output, State)); 
			if_fail_fe(gpk::saveView(output, Stages));
			return 0;
		}

		::gpk::err_t		Load				(::gpk::vcu0_t & input)	{ 
			if_fail_fe(gpk::loadPOD(input, Setup));
			if_fail_fe(gpk::loadPOD(input, State)); 
			if_fail_fe(gpk::loadView(input, Stages));
			return 0;
		}

	};
} // namespace

#endif // GPK_GAME_H
