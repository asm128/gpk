#include "gpk_game_stage.h"

#ifndef GPK_SPACESHIP_H
#define GPK_SPACESHIP_H

namespace gpk
{
#pragma pack(push, 1)


#pragma pack(pop)

	struct SSpaceship {
		//SGameSetup			Setup					= {};
		//SGameState			State					= {};
		//astagestate			Stages					= {};

		::gpk::error_t			Save				(::gpk::au8 & output)	const	{ 
			//gpk_necs(::gpk::savePOD(output, Setup)); 
			//gpk_necs(::gpk::savePOD(output, State)); 
			//gpk_necs(::gpk::saveView(output, Stages));
			return 0;
		}

		::gpk::error_t			Load				(::gpk::vcu8 & input)	{ 
			//gpk_necs(::gpk::loadPOD(input, Setup));
			//gpk_necs(::gpk::loadPOD(input, State)); 
			//gpk_necs(::gpk::loadView(input, Stages));
			return 0;
		}

	};
} // namespace

#endif // GPK_SPACESHIP_H
