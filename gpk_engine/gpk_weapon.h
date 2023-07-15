#include "gpk_particle.h"
#include "gpk_enum.h"
#include "gpk_array.h"
#include "gpk_apod_serialize.h"
#include "gpk_gauge.h"

#ifndef GPK_GALAXY_HELL_WEAPON_H
#define GPK_GALAXY_HELL_WEAPON_H

namespace gpk 
{
	struct SShots	{
		::gpk::aapod<::gpk::n3f32>	DistanceToTargets	= {};
		::gpk::af32					Lifetime			= {};
		::gpk::af32					Brightness			= {};
		::gpk::an3f32				PositionDraw		= {};
		::gpk::an3f32				PositionPrev		= {};
		::gpk::SParticles3			Particles			= {};

		int							Remove				(uint32_t iShot)			{
			Particles.Remove(iShot);
			DistanceToTargets	.remove_unordered(iShot);
			Lifetime			.remove_unordered(iShot);
			Brightness			.remove_unordered(iShot);
			PositionPrev		.remove_unordered(iShot);
			return PositionDraw	.remove_unordered(iShot);
		}

		int							SpawnForced			(const ::gpk::n3f32 & position, const ::gpk::n3f32 & direction, float speed, float brightness, float lifetime)	{
			PositionDraw.push_back(position);
			PositionPrev.push_back(position);
			Brightness	.push_back(brightness);
			Lifetime	.push_back(lifetime);
			DistanceToTargets[DistanceToTargets.push_back({})].push_back(direction);
			return Particles.Create(position, direction, speed);
		}

		int							SpawnForcedDirected	(double stabilityFactor, const ::gpk::n3f32 & position, const ::gpk::n3f32 & direction, float speedDebris, float brightness, float lifetime)	{
			stacxpr	double					randUnit			= ::gpk::math_2pi / RAND_MAX;
			::gpk::n3f32						finalDirection		= {0, 1, 0};
			finalDirection.RotateX(rand() * randUnit);
			finalDirection.RotateY(rand() * randUnit);
			finalDirection.Normalize();
			return SpawnForced(position, ::gpk::interpolate_linear(finalDirection, direction, stabilityFactor), speedDebris, brightness, lifetime);
		}

		::gpk::error_t				Update				(float secondsLastFrame)	{
			stacxpr	uint32_t				maxRange			= 200;
			stacxpr	uint32_t				maxRangeSquared		= maxRange * maxRange;
			memcpy(PositionPrev.begin(), Particles.Position.begin(), Particles.Position.size() * sizeof(::gpk::n3f32));
			Particles.IntegrateSpeed(secondsLastFrame);
			for(uint32_t iShot = 0; iShot < Particles.Position.size(); ++iShot) {
				Lifetime[iShot] -= secondsLastFrame;
				if(0 >= Lifetime[iShot])
					Remove(iShot--);
				//if (Particles.Position[iShot].LengthSquared() > maxRangeSquared)
				//	Remove(iShot--);
			}
			return 0;
		}

		::gpk::error_t				Save				(::gpk::au8 & output)	const	{ 
			gpk_necs(Particles.Save(output));
			gpk_necs(::gpk::saveView(output, PositionDraw	));
			gpk_necs(::gpk::saveView(output, PositionPrev	));
			gpk_necs(::gpk::saveView(output, Brightness		));
			gpk_necs(::gpk::saveView(output, Lifetime		));
			for(uint32_t iParticle = 0; iParticle < Particles.Position.size(); ++iParticle) 
				gpk_necs(::gpk::saveView(output, DistanceToTargets[iParticle]));
			
			return 0; 
		}

		::gpk::error_t				Load				(::gpk::vcu8 & input)			{ 
			gpk_necs(Particles.Load(input));
			gpk_necs(::gpk::loadView(input, PositionPrev	));
			gpk_necs(::gpk::loadView(input, PositionDraw	));
			gpk_necs(::gpk::loadView(input, Brightness		));
			gpk_necs(::gpk::loadView(input, Lifetime		));
			gpk_necs(DistanceToTargets.resize(Particles.Position.size()));
			for(uint32_t iParticle = 0; iParticle < DistanceToTargets.size(); ++iParticle)
				gpk_necs(::gpk::loadView(input, DistanceToTargets[iParticle]));

			return 0;
		}
	};

#pragma pack(push, 1)
	GDEFINE_FLAG_TYPE(AMMO_CLASS, int32_t);
	// these are the ammo classes that I can think of right now
	GDEFINE_FLAG_VALUE(AMMO_CLASS, None			,	0x00000UL);	
	GDEFINE_FLAG_VALUE(AMMO_CLASS, Arrow		,	0x00001UL);	
	GDEFINE_FLAG_VALUE(AMMO_CLASS, Bullet		,	0x00002UL);	
	GDEFINE_FLAG_VALUE(AMMO_CLASS, Grenade		,	0x00004UL);	
	GDEFINE_FLAG_VALUE(AMMO_CLASS, Rocket		,	0x00008UL);	
	//
	GDEFINE_FLAG_VALUE(AMMO_CLASS, Ray			,	0x00010UL);	
	GDEFINE_FLAG_VALUE(AMMO_CLASS, Dart			,	0x00020UL);	
	GDEFINE_FLAG_VALUE(AMMO_CLASS, Flame		,	0x00040UL);	
	GDEFINE_FLAG_VALUE(AMMO_CLASS, Fluid		,	0x00080UL);	
	//
	GDEFINE_FLAG_VALUE(AMMO_CLASS, Shot			,	0x00100UL);	
	GDEFINE_FLAG_VALUE(AMMO_CLASS, Knife		,	0x00200UL);	
	GDEFINE_FLAG_VALUE(AMMO_CLASS, Rock			,	0x00400UL);	
	GDEFINE_FLAG_VALUE(AMMO_CLASS, Shrapnel		,	0x00800UL);	
	//
	GDEFINE_FLAG_VALUE(AMMO_CLASS, Boomerang	,	0x01000UL);	
	GDEFINE_FLAG_VALUE(AMMO_CLASS, Lightning	,	0x02000UL);	
	GDEFINE_FLAG_VALUE(AMMO_CLASS, Hook			,	0x04000UL);	
	GDEFINE_FLAG_VALUE(AMMO_CLASS, Wave			,	0x08000UL);	
	//
	GDEFINE_FLAG_VALUE(AMMO_CLASS, Blast		,	0x010000UL);
	GDEFINE_FLAG_VALUE(AMMO_CLASS, Singularity	,	0x020000UL);
	GDEFINE_FLAG_VALUE(AMMO_CLASS, Temporized	,	0x100000UL);
	GDEFINE_FLAG_VALUE(AMMO_CLASS, Detonable	,	0x200000UL);

	GDEFINE_ENUM_TYPE(WEAPON_ACTION, uint16_t);
	GDEFINE_ENUM_VALUE(WEAPON_ACTION, Shoot		, 0x0);
	GDEFINE_ENUM_VALUE(WEAPON_ACTION, Cooldown	, 0x1);
	GDEFINE_ENUM_VALUE(WEAPON_ACTION, Target	, 0x2);

	GDEFINE_ENUM_TYPE (DAMAGE_TYPE, uint16_t);
	GDEFINE_ENUM_VALUE(DAMAGE_TYPE, Pierce		, 0x0);
	GDEFINE_ENUM_VALUE(DAMAGE_TYPE, Impact		, 0x1);
	GDEFINE_ENUM_VALUE(DAMAGE_TYPE, Wave		, 0x2);
	GDEFINE_ENUM_VALUE(DAMAGE_TYPE, Burn		, 0x4);
	GDEFINE_ENUM_VALUE(DAMAGE_TYPE, Shock		, 0x8);
	GDEFINE_ENUM_VALUE(DAMAGE_TYPE, Magnetic	, 0x10);
	GDEFINE_ENUM_VALUE(DAMAGE_TYPE, Radiation	, 0x20);
	GDEFINE_ENUM_VALUE(DAMAGE_TYPE, Poison		, 0x40);
	GDEFINE_ENUM_VALUE(DAMAGE_TYPE, Plasma		, 0x80);
	GDEFINE_ENUM_VALUE(DAMAGE_TYPE, Light		, 0x100);
	GDEFINE_ENUM_VALUE(DAMAGE_TYPE, Sound		, 0x200);
	GDEFINE_ENUM_VALUE(DAMAGE_TYPE, EMP			, 0x400);
	GDEFINE_ENUM_VALUE(DAMAGE_TYPE, Gravity		, 0x800);

	GDEFINE_ENUM_TYPE(WEAPON_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Gun			, 0);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Cannon		, 1);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Rocket		, 2);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Missile		, 3);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Torpedo		, 4);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Shotgun		, 5);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Mothership	, 6);
	GDEFINE_ENUM_VALUE(WEAPON_TYPE, Shield		, 7);

	GDEFINE_ENUM_TYPE (WEAPON_LOAD, uint8_t);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Bullet		, 0);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Shot		, 1);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Ray			, 2);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Cannonball	, 3);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Rocket		, 4);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Missile		, 5);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Wave		, 6);
	GDEFINE_ENUM_VALUE(WEAPON_LOAD, Flare		, 7);

	GDEFINE_ENUM_TYPE(WEAPON_EFFECT, uint8_t);
	GDEFINE_ENUM_VALUE(WEAPON_EFFECT, Pierce	, 0);

	// One per value combination
	struct SWeaponLoadDesc {
		WEAPON_LOAD		Type				= {};//= WEAPON_LOAD_Bullet;
		DAMAGE_TYPE		DamageType			= {};//= WEAPON_DAMAGE_Pierce;
		uint8_t			ParticleCount		= {};//= 1;
		float			Delay				= {};//= .1;
		int32_t			Damage				= {};//= 1;
		float			Lifetime			= {};//= 0;
		float			Speed				= {};//= 150;
		float			Weight				= {};//= 150;
		//uint32_t		Color				= {};
	};
	
	// One per value combinatiom
	struct SWeaponType {
		WEAPON_TYPE		Type				= {};//= WEAPON_TYPE_Gun;
		DAMAGE_TYPE		DamageType			= {};//= WEAPON_DAMAGE_Pierce;
		int32_t			DamageMultiplier	= {};//= 1;
		float			Cooldown			= {};//= 1;
		float			Stability			= {};//= 1.0;
		float			ShotLifetime		= {};//= 0;
		float			SpeedMultiplier		= {};//= 150;
	};

	struct SWeaponLoadState {
		gaugemaxf32		Delay				= {};	//= .1;
		gaugemaxf32		Overheat			= {};	//= 1; aka Reload
		bool			CoolingDown			= {};	//= false;
	};

	// One per orbiter
	struct SWeapon {
		WEAPON_TYPE		Type				= {};	//= WEAPON_TYPE_Gun;
		WEAPON_LOAD		Load				= {};	//= WEAPON_LOAD_Bullet;
		DAMAGE_TYPE		DamageType			= {};	//= WEAPON_DAMAGE_Pierce;
		uint8_t			ParticleCount		= {};	//= 1;
		gaugemaxf32		Delay				= {};	// Time in seconds between a shot and the next
		gaugemaxf32		Overheat			= {};	// Cooldown/Reload time in seconds
		float			OverheatPerShot		= {};	// 
		float			Stability			= {};	//= 1.0;
		float			Speed				= {};	//= 150;
		int32_t			Damage				= {};	//= 1;
		float			ShotLifetime		= {};	//= 0;
		bool			CoolingDown			= {};	//= false;

		int				Shoot				(::gpk::SShots & shots, const ::gpk::n3f32 & position, const ::gpk::n3f32 & direction, float speed, float brightness, float lifetime)	{
			if(Delay.Value < Delay.Limit)
				return 0;

			Delay.Value		= 0;
			Overheat.Value	+= OverheatPerShot;
			return shots.SpawnForced(position, direction, speed, brightness, lifetime);
		}

		int				ShootDirected		(::gpk::SShots & shots, double stabilityFactor, const ::gpk::n3f32 & position, const ::gpk::n3f32 & direction, float speedDebris, float brightness, float lifetime)	{
			if(Delay.Value < Delay.Limit)
				return 0;

			Delay.Value		= 0;
			Overheat.Value	+= OverheatPerShot;
			return shots.SpawnForcedDirected(Stability * stabilityFactor, position, direction, speedDebris, brightness, lifetime);
		}

		int				ShootDirected		(::gpk::SShots & shots, uint32_t countShots, double stabilityFactor, const ::gpk::n3f32 & position, const ::gpk::n3f32 & direction, float speedDebris, float brightness, float lifetime)	{
			if(Delay.Value < Delay.Limit || 0 == countShots)
				return 0;

			int32_t				indexFirst			= -1;
			if(countShots)
				indexFirst		= shots.SpawnForcedDirected(Stability * stabilityFactor, position, direction, speedDebris, brightness, lifetime);

			for(uint32_t iDebris = 0; iDebris < (countShots - 1); ++iDebris)
				shots.SpawnForcedDirected(Stability * stabilityFactor, position, direction, speedDebris, brightness, lifetime);

			Delay.Value		= 0;
			Overheat.Value	+= OverheatPerShot;
			return indexFirst;
		}
	};

	struct SWeaponScore {
		int64_t		ImpactTime		= 0;
		int64_t		BulletsHit		= 0;
		int64_t		BulletsUsed		= 0;
		int64_t		BulletsLoaded	= 0;
		int64_t		ShotsUsed		= 0;
		int64_t		ShotsLoaded		= 0;
		int64_t		ReloadCount		= 0;
		int64_t		ReloadTime		= 0;
		int64_t		OverheatCount	= 0;
		int64_t		OverheatTime	= 0;
	};
#pragma pack(pop)
} // namespace

#endif // GPK_GALAXY_HELL_WEAPON_H