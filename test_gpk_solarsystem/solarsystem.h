#include "gpk_engine_planetary_system.h"

#include "gpk_engine.h"

#include "gpk_model.h"
#include "gpk_particle.h"

#include "gpk_camera.h"

#ifndef SOLARSYSTEM_H_23627
#define SOLARSYSTEM_H_23627

namespace ssg 
{
	enum PLANET
		{ PLANET_MERCURY
		, PLANET_VENUS
		, PLANET_EARTH
		, PLANET_MARS
		, PLANET_JUPITER
		, PLANET_SATURN
		, PLANET_URANUS
		, PLANET_NEPTUNE
		, PLANET_PLUTO

		, PLANET_COUNT
		};

	struct SModelPivot {
		::gpk::n3f2_t						Scale							= {1, 1, 1};
		::gpk::n3f2_t						Position						= {};
	};

	struct SScene {
		::gpk::apod<::ssg::SModelPivot>		Pivot							= {};
		::gpk::apod<::gpk::m4f2_t>			Transform						= {};

		::gpk::SCameraPoints				Camera							= {};
	};

	struct SEntity {
		int32_t								Parent						;
		int32_t								Geometry					;
		int32_t								Model						;
		int32_t								Images						;
		int32_t								Body						;
		::gpk::au2_t							Children					;
	};

	struct SDebris	{
		::gpk::bgra					Colors[4]			=
			{ {0x80, 0xAF, 0xFF, }
			, {0x40, 0xAF, 0xFF, }
			, {0x80, 0xCF, 0xFF, }
			//, {0x00, 0x00, 0xFF, }
			};
		::gpk::af2_t					Brightness			= {};
		::gpk::SParticles3			Particles			= {};

		int							Create				(const ::gpk::n3f2_t & position, const ::gpk::n3f2_t & direction, float speed, float brightness)	{
			Particles.Create(position, direction, speed);
			return Brightness.push_back(brightness);
		}
		int							SpawnSpherical		(uint32_t countDebris, const ::gpk::n3f2_t & position, float speedDebris, float brightness, float offset)	{
			for(uint32_t iDebris = 0; iDebris < countDebris; ++iDebris) {
				::gpk::n3f2_t										direction				= {0, 1 * offset, 0};
				direction.RotateX(rand() * (::gpk::math_2pi / RAND_MAX));
				direction.RotateY(rand() * (::gpk::math_2pi / RAND_MAX));
				direction.RotateZ(rand() * (::gpk::math_2pi / RAND_MAX));
				const ::gpk::n3f2_t	newPosition		= position + direction;
				direction.Normalize();
				Create(newPosition, direction, speedDebris, brightness);
			}
			return 0;
		}
		int							Update				(double secondsLastFrame)	{
			Particles.IntegrateSpeed(secondsLastFrame);
			const float											fLastFrame			= (float)secondsLastFrame;
			for(uint32_t iShot = 0; iShot < Particles.Position.size(); ++iShot) {
				float														& speed				= Particles.Speed		[iShot];
				float														& brightness 		= Brightness			[iShot];
				brightness												-= fLastFrame;
				speed													-= fLastFrame * (rand() % 16);
				if(brightness < 0) {
					Particles.Remove(iShot);
					Brightness.remove_unordered(iShot--);
				}
			}
			return 0;
		}
	};

	struct SSolarSystemGame : ::gpk::SPlanetarySystem {
		::gpk::SEngine							Engine;

		::ssg::SDebris							SunFire							= {};
		::ssg::SScene							Scene							= {};
		::gpk::aobj<::ssg::SEntity>				Entities						= {};
		::gpk::aobj<::gpk::gc8bgra>				Images							= {};
		::gpk::aobj<::gpk::SGeometryBuffers>	Geometries						= {};
	};

	::gpk::err_t		solarSystemSetup				(::ssg::SSolarSystemGame & sunsystem);
	::gpk::err_t		solarSystemUpdate				(::ssg::SSolarSystemGame & sunsystem, double secondsLastFrame, ::gpk::pobj<::gpk::rtbgra8d32> & target);

} // namespace

#endif // SOLARSYSTEM_H_23627

// 08003451058 galicia