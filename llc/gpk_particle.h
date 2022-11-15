// This work is loosely based on Ian Millington's "Game Physics Engine Development"
//
// A particle is the simplest object that can be simulated in the physics system.
// It has position data (no orientation data), along with velocity. It can be integrated forward through time, and have linear forces, and impulses applied to it.
// This system allows defining the floating-point precision of the elements. It obviously won't work for integer types so don't use it in that way.
#include "gpk_coord.h"			// for ::gpk::SCoord2<>
#include "gpk_error.h"			// for ::gpk::error_t
#include "gpk_array.h"		// for ::gpk::view_array<>
#include <cfloat>

#ifndef GPK_PARTICLE_H_29384923874
#define GPK_PARTICLE_H_29384923874

namespace gpk
{
#pragma pack(push, 1)
	template<typename _tElement>
	struct SParticle2Forces {
		typedef				::gpk::SCoord2<_tElement>							TCoord;
		static constexpr	const _tElement										VelocityEpsilon								= 0.0001f;

							TCoord												AccumulatedForce							= {};
							TCoord												Acceleration								= {};	// A vector representing the speed in a given direction
							TCoord												Velocity									= {};	// A vector representing the speed in a given direction

		inline constexpr	bool												VelocityDepleted							()																					const	noexcept	{ return (Velocity + Acceleration).LengthSquared() < (VelocityEpsilon * VelocityEpsilon); }
		// This basically does Acceleration += (Force * 1 / Mass) and Velocity += (Acceleration * Time).
							void												IntegrateAccumulatedForce					(const _tElement inverseMass, const _tElement damping, const double timeElapsed)			noexcept	{
			Acceleration															+= AccumulatedForce * inverseMass;				// Calculate linear acceleration from force inputs.
			AccumulatedForce														= {};											// Clear this out now that we've used it already.

			Velocity																+= Acceleration * timeElapsed;					// Update linear velocity from both acceleration and impulse.
			Velocity																*= ::pow(damping, timeElapsed);					// Impose drag.
		}
	};

	template<typename _tElement>
	struct SParticle2 {
		// The member variables are organized such that matches the order in which they are used.
							::gpk::SCoord2<_tElement>							Position									= {};
							SParticle2Forces<_tElement>							Forces										= {};
							_tElement											InverseMass									= 0;
							_tElement											Damping										= .99f;	// A vector representing the speed in a given direction

		inline				void												SetMass										(const double mass)																			noexcept	{ InverseMass = mass ? ((_tElement)(1.0 / mass)) : 0;		}
		inline constexpr	double												GetMass										()																					const	noexcept	{ return (InverseMass == 0) ? DBL_MAX : 1.0 / InverseMass;	}
		inline constexpr	bool												HasFiniteMass								()																					const	noexcept	{ return InverseMass >= 0.0f;								}
	};

	// This compact structure allows to define all the boolean states of the particle packed in a single byte.
	struct SParticle2State {
							bool												Unused										: 1;
							bool												Active										: 1;

		inline constexpr	bool												RequiresProcessing							()																					const	noexcept	{ return (false == Unused) && Active; }
	};
#pragma pack(pop)
	// This basically does FinalPosition = InitialPosition + Velocity * Time.
	template<typename _tElement>
	static inline		void												particleIntegratePosition
		(	const ::gpk::SCoord2<_tElement>	& velocity
		,	const double					timeElapsed
		,	const double					timeElapsedHalfSquared
		,	::gpk::SCoord2<_tElement>		& position
		)
	{
		position																+= velocity * timeElapsed;
		position																+= velocity * timeElapsedHalfSquared;
	}

	template<typename _tElement>
						::gpk::error_t										integrate									(const ::gpk::view_array<::gpk::SParticle2<_tElement>>& particles, ::gpk::array_pod<::gpk::SParticle2State>& particleStates, ::gpk::view_array<::gpk::SParticle2<_tElement>>& particlesNext, double timeElapsed, double timeElapsedHalfSquared)			{
		for(uint32_t iParticle = 0, particleCount = (uint32_t)particleStates.size(); iParticle < particleCount; ++iParticle)
			if(particleStates[iParticle].RequiresProcessing()) {
				::gpk::SParticle2<_tElement>												& particleNext								= particlesNext[iParticle] = particles[iParticle];	// Copy the current particle state to the next
				::gpk::particleIntegratePosition(particleNext.Forces.Velocity, timeElapsed, timeElapsedHalfSquared, particleNext.Position);
				particleNext.Forces.IntegrateAccumulatedForce(particleNext.InverseMass, particleNext.Damping, timeElapsed);
				if(particleNext.Forces.VelocityDepleted())
					particleStates[iParticle].Active										= false;
			}
		return 0;
	}

	template<typename _tElement>
	struct SParticle2Integrator {
		typedef				::gpk::SParticle2	<_tElement>						TParticle;
		typedef				::gpk::SCoord2		<_tElement>						TCoord;

							::gpk::array_pod<::gpk::SParticle2State>			ParticleState								= {};
							::gpk::array_pod<TParticle>							Particle									= {};
							::gpk::array_pod<TParticle>							ParticleNext								= {};
		// -----------------------------------------------------------------	---
		inline				::gpk::error_t										Integrate									(double timeElapsed, double timeElapsedHalfSquared)														{ return Integrate(ParticleNext, timeElapsed, timeElapsedHalfSquared);	}
							::gpk::error_t										Integrate									(::gpk::view_array<TParticle>& particleNext, double timeElapsed, double timeElapsedHalfSquared)			{ return ::gpk::integrate(Particle, ParticleState, particleNext, timeElapsed, timeElapsedHalfSquared);		}
		// --------------------------------------------------------------------
							::gpk::error_t										AddParticle									(const TParticle& particleData)																			{
								const uint32_t											particleCount								= (uint32_t)ParticleState.size();
			static constexpr	const ::gpk::SParticle2State							initialParticleState						= {false, true};

			for(uint32_t iBody = 0; iBody < particleCount; ++iBody)	// Check if there is any unused particle that we can recycle.
				if( ParticleState	[iBody].Unused ) {
					ParticleState	[iBody]													= initialParticleState;
					Particle		[iBody]													=
					ParticleNext	[iBody]													= particleData;
					return iBody;
				}
#if defined(GPK_DISABLE_CPP_EXCEPTIONS)
			ParticleState		.push_back(initialParticleState);
			Particle			.push_back(particleData);
			ParticleNext		.push_back(particleData);
#else
			try {	// Later on we're going to add a way to avoid using ::std::vector which require these ugly try/catch blocks.
				ParticleState		.push_back(initialParticleState);
				Particle			.push_back(particleData);
				ParticleNext		.push_back(particleData);
			}
			catch(...) { // Restore the previous size to all the containers and return an error.
				ParticleState		.resize(particleCount);
				Particle			.resize(particleCount);
				ParticleNext		.resize(particleCount);
				return -1;
			}
#endif
			return particleCount;
		}
	};

	template<typename _tParticleType>
	struct SParticleBinding {
		typedef					_tParticleType									TParticleType;

								TParticleType									Binding										= {};
								int32_t											IndexParticlePhysics						= -1;
	};

	template<typename _tParticleType, typename _tCoord>
							::gpk::error_t									addParticle
		(	const _tParticleType										& particleType
		,	::gpk::array_pod<::gpk::SParticleBinding<_tParticleType>>	& particleInstances
		,	::gpk::SParticle2Integrator<_tCoord>						& particleIntegrator
		,	const ::gpk::SParticle2<_tCoord>							& particleDefinition
		)
	{
		::gpk::SParticleBinding<_tParticleType>									newInstance									= {};
		newInstance.Binding													= particleType;
		gpk_necall(newInstance.IndexParticlePhysics = particleIntegrator.AddParticle(particleDefinition), "%s", "Failed to add particle definition instance to integrator.");
		return particleInstances.push_back(newInstance);
	}

	template<typename _tParticleType, typename _tCoord>
	struct SParticleSystem {
		typedef					::gpk::SParticleBinding<_tParticleType>			TParticleInstance;
		typedef					::gpk::SParticle2Integrator<_tCoord>			TIntegrator;

								::gpk::array_pod<TParticleInstance>				Instances									= {};
								TIntegrator										Integrator									= {};
	};

	template<typename _tParticleType, typename _tCoord>
	static inline			::gpk::error_t									addParticle									(const _tParticleType& particleType, ::gpk::SParticleSystem<_tParticleType, _tCoord> & particleSystem,	const ::gpk::SParticle2<_tCoord> & particleDefinition)	{
		return addParticle(particleType, particleSystem.Instances, particleSystem.Integrator, particleDefinition);
	}

	// simple particle force integrator
	struct SParticles3 {
		::gpk::array_pod<::gpk::SCoord3<float>>		Position			= {};
		::gpk::array_pod<::gpk::SCoord3<float>>		Direction			= {};
		::gpk::array_pod<float>						Speed				= {};

		int											IntegrateSpeed		(double secondsLastFrame)	{
			for(uint32_t iShot = 0; iShot < Position.size(); ++iShot) {
				::gpk::SCoord3<float>							& direction			= Direction	[iShot];
				::gpk::SCoord3<float>							& position			= Position	[iShot];
				float											& speed				= Speed		[iShot];
				position									+= direction * speed * secondsLastFrame;
			}
			return 0;
		}

		int											Remove				(int32_t iParticle)	{
			Position	.remove_unordered(iParticle);
			Direction	.remove_unordered(iParticle);
			return Speed.remove_unordered(iParticle);
		}

		int											Create				(const ::gpk::SCoord3<float> & position, const ::gpk::SCoord3<float> & direction, float speed)	{
			Position	.push_back(position);
			Direction	.push_back(direction);
			return Speed.push_back(speed);
		}

		::gpk::error_t										Save(::gpk::array_pod<byte_t> & output) const { 
			gpk_necs(::gpk::viewWrite(Position	, output)); 
			gpk_necs(::gpk::viewWrite(Direction	, output)); 
			gpk_necs(::gpk::viewWrite(Speed		, output)); 
			info_printf("Saved %s, %i", "Position	", Position		.size()); 
			info_printf("Saved %s, %i", "Direction	", Direction	.size()); 
			info_printf("Saved %s, %i", "Speed		", Speed		.size()); 
			return 0; 
		}

		::gpk::error_t										Load(::gpk::view_array<const byte_t> & input) { 
			gpk_necs(::gpk::loadView(input, Position	));
			gpk_necs(::gpk::loadView(input, Direction	));
			gpk_necs(::gpk::loadView(input, Speed		));
			return 0;
		}
	};
} // namespace

#endif // GPK_PARTICLE_H_29384923874
