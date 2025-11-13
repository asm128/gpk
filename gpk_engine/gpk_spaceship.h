#include "gpk_enum.h"
#include "gpk_gauge.h"
#include "gpk_array_ptr.h"
#include "gpk_view_n3.h"
#include "gpk_apod_serialize.h"

#ifndef GPK_SPACESHIP_H
#define GPK_SPACESHIP_H

namespace gpk
{
	GDEFINE_ENUM_TYPE (SHIP_GEOMETRY, uint8_t);
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

	GDEFINE_ENUM_TYPE(SHIP_PART_TYPE, uint8_t);
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

	GDEFINE_ENUM_TYPE (SHIP_ACTION, uint8_t);
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
		uint32_t			Weapon			= (uint32_t)-1;
		uint32_t			ActiveAction	= ~0U;
	};

	struct SSpaceshipScore {
		uint64_t			Score			= 0;
		uint64_t			Hits			= 0;
		uint64_t			Shots			= 0;
		uint64_t			Bullets			= 0;
		uint64_t			DamageDone		= 0;
		uint64_t			DamageReceived	= 0;
		uint64_t			HitsSurvived	= 0;
		uint32_t			OrbitersLost	= 0;
		uint32_t			KilledShips		= 0;
		uint32_t			KilledOrbiters	= 0;
	};

	struct SSpaceshipCore {
		// Generally constants, little or no variation during level gameplay
		int32_t				Team			= {};
		::gpk::gaugemaxf32	Nitro			= {};

		// Variable, may change at any time during 
		int32_t				Health			= {};
		uint32_t			AvailableNitros	= {};
		double				TimePlayed		= {};
	};
#pragma pack(pop)
	tplt<tpnm _tPOD> 
	::gpk::error_t			loadView			(::gpk::vcu0_t & input, ::gpk::apapod<_tPOD> & output, const uint32_t index) { 
		::gpk::view<const _tPOD>	viewToLoad;
		gpk_necall(gpk::loadView(input, viewToLoad), "index: %i", index);
		if(0 == viewToLoad.size()) {
			if(index >= output.size()) 
				gpk_necall(output.resize(index + 1), "index: %i", index);
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
				gpk_necall(output.push_back(block), "index: %i", index);
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

		int32_t						Clear				()	{
			::gpk::clear(ShipScores, ShipCores, ShipParts, Orbiters, ShipOrbiterActionQueue);
			return 0;
		}

		int32_t						GetShipHealth		(uint32_t iShipCore)				{ 
			int32_t							totalHealth			= 0;
			::gpk::vcu1_t					shipCoreParts		= ShipParts[iShipCore];
			for(uint32_t iShipCorePart = 0, countParts = shipCoreParts.size(); iShipCorePart < countParts; ++iShipCorePart)
				totalHealth += Orbiters[shipCoreParts[iShipCorePart]].Health.Value;

			return totalHealth;
		}

		int32_t						GetTeamHealth		(int32_t teamId) {
			int32_t							totalHealth			= 0;
			for(uint32_t iShipCore = 0, countShips = ShipCores.size(); iShipCore < countShips; ++iShipCore) {
				if(ShipCores[iShipCore].Team != teamId) 
					continue;

				::gpk::vcu1_t					shipCoreParts		= ShipParts[iShipCore];
				for(uint32_t iShipCorePart = 0, countParts = shipCoreParts.size(); iShipCorePart < countParts; ++iShipCorePart) 
					totalHealth += Orbiters[shipCoreParts[iShipCorePart]].Health.Value;
			}
			return totalHealth;
		}


		::gpk::error_t				Save				(::gpk::au0_t & output)	const	{ 
			gpk_necs(gpk::saveView(output, ShipScores	));
			gpk_necs(gpk::saveView(output, ShipCores	));
			uint32_t						totalEntityChildren	= 0;
			for(uint32_t iShipCore = 0; iShipCore < ShipCores.size(); ++iShipCore) {
				::gpk::vcu1_t					v		{ShipParts[iShipCore]};
				gpk_necall(gpk::saveView(output, v), "iPart: %i", iShipCore);
				totalEntityChildren += v.size();
			}

			info_printf("Saved %s, %i", "ShipScores	", ShipScores	.size());
			info_printf("Saved %s, %i", "ShipCores	", ShipCores	.size());
	
			// Orbiters
			gpk_necs(gpk::saveView(output, Orbiters));
			info_printf("Saved %s, %i", "Orbiters", Orbiters.size());
			for(uint32_t iShipOrbiter = 0; iShipOrbiter < Orbiters.size(); ++iShipOrbiter) {
				if(ShipOrbiterActionQueue[iShipOrbiter] && ShipOrbiterActionQueue[iShipOrbiter]->size())
					gpk_necall(gpk::saveView(output, *ShipOrbiterActionQueue[iShipOrbiter]), "iShipOrbiter: %i", iShipOrbiter);
				else
					gpk_necall(gpk::saveView(output, ::gpk::vc3f32{}), "iShipOrbiter: %i", iShipOrbiter);
			}
			return 0; 
		}
		::gpk::error_t				Load				(::gpk::vcu0_t & input) { 
			ShipScores	.clear();
			ShipCores	.clear();
			gpk_necs(gpk::loadView(input, ShipScores	));
			gpk_necs(gpk::loadView(input, ShipCores	));
			gpk_necs(ShipParts.resize(ShipCores.size()));
			for(uint32_t iPart = 0; iPart < ShipParts.size(); ++iPart) 
				gpk_necall(gpk::loadView(input, ShipParts[iPart]), "iPart: %i", iPart);

			Orbiters.clear();
			gpk_necs(gpk::loadView(input, Orbiters));
			gpk_necs(ShipOrbiterActionQueue.reserve(Orbiters.size()));
			for(uint32_t iShipOrbiter = 0; iShipOrbiter < Orbiters.size(); ++iShipOrbiter) {
				gpk_necs(gpk::loadView(input, ShipOrbiterActionQueue, iShipOrbiter));
			}
			return 0; 
		}
	};
} // namespace

#endif // GPK_SPACESHIP_H
