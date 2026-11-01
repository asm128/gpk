#include "gpk_enum.h"
#include "gpk_gauge.h"
#include "gpk_array_ptr.h"
#include "gpk_view_n3.h"
#include "gpk_apod_serialize.h"

#ifndef GPK_SPACESHIP_H
#define GPK_SPACESHIP_H

namespace gpk
{
	GDEFINE_ENUM_TYPE (SHIP_GEOMETRY, u0_t);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, Line			, 0);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, Triangle		, 1);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, Rectangle		, 2);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, Circle		, 3);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, Frame			, 4);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, Ring			, 5);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, Cube			, 6);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, Sphere		, 7);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, Cylinder		, 8);
	//
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, Engine		, 100);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, Gun			, 101);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, Wafer			, 102);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, Cannon		, 103);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, Shotgun		, 104);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, WaferShotgun	, 105);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, Cannonball	, 106);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, Rocket		, 107);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, Missile		, 108);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, Bullet		, 109);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, Shred			, 110);
	GDEFINE_ENUM_VALUE(SHIP_GEOMETRY, Nitro			, 111);

	GDEFINE_ENUM_TYPE(SHIP_PART_TYPE, u0_t);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Cargo		, 0);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Cannon		, 1);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Wafer		, 2);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Gun			, 3);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, ShotgunWafer	, 4);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Shotgun		, 5);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Coil			, 6);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Shield		, 7);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Silo			, 8);
	GDEFINE_ENUM_VALUE(SHIP_PART_TYPE, Tractor		, 9);

	GDEFINE_ENUM_TYPE (SHIP_ACTION, u0_t);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, Spawn		, 0);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, MoveLeft	, 1);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, MoveRight	, 2);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, MoveUp		, 3);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, MoveDown	, 4);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, RotateXUp	, 5);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, RotateXDown	, 6);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, RotateZFront, 7);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, RotateZBack	, 8);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, RotateYLeft	, 9);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, RotateYRight, 10);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, Turbo		, 11);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, Bomb		, 12);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, Lock		, 13);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, Unlock		, 14);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, Dash		, 15);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, Vanish		, 16);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, Explode		, 17);
	GDEFINE_ENUM_VALUE(SHIP_ACTION, Hit			, 18);

#pragma pack(push, 1)
	struct SSpaceshipOrbiter {
		SHIP_PART_TYPE		Type			= SHIP_PART_TYPE_Cargo;
		::gpk::gaugemaxi32	Health			= {};
		u2_t			Weapon			= (u2_t)-1;
		u2_t			ActiveAction	= ~0U;
	};

	struct SSpaceshipScore {
		u3_t			Score			= 0;
		u3_t			Hits			= 0;
		u3_t			Shots			= 0;
		u3_t			Bullets			= 0;
		u3_t			DamageDone		= 0;
		u3_t			DamageReceived	= 0;
		u3_t			HitsSurvived	= 0;
		u2_t			OrbitersLost	= 0;
		u2_t			KilledShips		= 0;
		u2_t			KilledOrbiters	= 0;
	};

	struct SSpaceshipCore {
		// Generally constants, little or no variation during level gameplay
		s2_t				Team			= {};
		::gpk::gaugemaxf2_t	Nitro			= {};

		// Variable, may change at any time during 
		s2_t				Health			= {};
		u2_t			AvailableNitros	= {};
		f3_t				TimePlayed		= {};
	};
#pragma pack(pop)
	tplt<tpnm _tPOD> 
	gpk::err_t 		loadView			(::gpk::vcu0_t & input, ::gpk::apapod<_tPOD> & output, const u2_t index) { 
		::gpk::view<const _tPOD>	viewToLoad;
		if_fail_fef(gpk::loadView(input, viewToLoad), "index: %i", index);
		if(0 == viewToLoad.size()) {
			if(index >= output.size()) 
				if_fail_fef(output.resize(index + 1), "index: %i", index);
			else {
				::gpk::papod<_tPOD>			& block				= output[index];
				if(block)
					block->clear();
			}
		}
		else {
			if(index >= output.size()) {
				::gpk::papod<_tPOD>			block;
				*block.create()			= viewToLoad;
				if_fail_fef(output.push_back(block), "index: %i", index);
			}
			else {
				::gpk::papod<_tPOD>			& block				= output[index];
				if(!block)
					block.create();
				*block					= viewToLoad;
			}
		}
		return 0;
	}

	struct SSpaceshipManager {
		::gpk::apod<::gpk::SSpaceshipScore	>	ShipScores					= {}; // One per core
		::gpk::apod<::gpk::SSpaceshipCore	>	ShipCores					= {}; // One per core
		::gpk::aobj<::gpk::au1_t				>	ShipParts					= {}; // One per core, each one mapping to a list of orbiters.

		::gpk::apod<::gpk::SSpaceshipOrbiter>	Orbiters					= {};	// One per orbiter	
		::gpk::apapod<::gpk::SHIP_ACTION	>	ShipOrbiterActionQueue		= {};

		s2_t						Clear				()	{
			::gpk::clear(ShipScores, ShipCores, ShipParts, Orbiters, ShipOrbiterActionQueue);
			return 0;
		}

		s2_t						GetShipHealth		(u2_t iShipCore)				{ 
			s2_t							totalHealth			= 0;
			::gpk::vcu1_t					shipCoreParts		= ShipParts[iShipCore];
			for(u2_t iShipCorePart = 0, countParts = shipCoreParts.size(); iShipCorePart < countParts; ++iShipCorePart)
				totalHealth += Orbiters[shipCoreParts[iShipCorePart]].Health.Value;

			return totalHealth;
		}

		s2_t						GetTeamHealth		(s2_t teamId) {
			s2_t							totalHealth			= 0;
			for(u2_t iShipCore = 0, countShips = ShipCores.size(); iShipCore < countShips; ++iShipCore) {
				if(ShipCores[iShipCore].Team != teamId) 
					continue;

				::gpk::vcu1_t					shipCoreParts		= ShipParts[iShipCore];
				for(u2_t iShipCorePart = 0, countParts = shipCoreParts.size(); iShipCorePart < countParts; ++iShipCorePart) 
					totalHealth += Orbiters[shipCoreParts[iShipCorePart]].Health.Value;
			}
			return totalHealth;
		}


		gpk::err_t 			Save				(::gpk::au0_t & output)	const	{ 
			if_fail_fe(gpk::saveView(output, ShipScores	));
			if_fail_fe(gpk::saveView(output, ShipCores	));
			u2_t						totalEntityChildren	= 0;
			for(u2_t iShipCore = 0; iShipCore < ShipCores.size(); ++iShipCore) {
				::gpk::vcu1_t					v		{ShipParts[iShipCore]};
				if_fail_fef(gpk::saveView(output, v), "iPart: %i", iShipCore);
				totalEntityChildren += v.size();
			}

			info_printf("Saved %s, %i", "ShipScores	", ShipScores	.size());
			info_printf("Saved %s, %i", "ShipCores	", ShipCores	.size());
	
			// Orbiters
			if_fail_fe(gpk::saveView(output, Orbiters));
			info_printf("Saved %s, %i", "Orbiters", Orbiters.size());
			for(u2_t iShipOrbiter = 0; iShipOrbiter < Orbiters.size(); ++iShipOrbiter) {
				if(ShipOrbiterActionQueue[iShipOrbiter] && ShipOrbiterActionQueue[iShipOrbiter]->size())
					if_fail_fef(gpk::saveView(output, *ShipOrbiterActionQueue[iShipOrbiter]), "iShipOrbiter: %i", iShipOrbiter);
				else
					if_fail_fef(gpk::saveView(output, ::gpk::vc3f2_t{}), "iShipOrbiter: %i", iShipOrbiter);
			}
			return 0; 
		}
		gpk::err_t 			Load				(::gpk::vcu0_t & input) { 
			ShipScores	.clear();
			ShipCores	.clear();
			if_fail_fe(gpk::loadView(input, ShipScores	));
			if_fail_fe(gpk::loadView(input, ShipCores	));
			if_fail_fe(ShipParts.resize(ShipCores.size()));
			for(u2_t iPart = 0; iPart < ShipParts.size(); ++iPart) 
				if_fail_fef(gpk::loadView(input, ShipParts[iPart]), "iPart: %i", iPart);

			Orbiters.clear();
			if_fail_fe(gpk::loadView(input, Orbiters));
			if_fail_fe(ShipOrbiterActionQueue.reserve(Orbiters.size()));
			for(u2_t iShipOrbiter = 0; iShipOrbiter < Orbiters.size(); ++iShipOrbiter) {
				if_fail_fe(gpk::loadView(input, ShipOrbiterActionQueue, iShipOrbiter));
			}
			return 0; 
		}
	};
} // namespace

#endif // GPK_SPACESHIP_H
