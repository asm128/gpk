#include "gpk_apod_n3.h"

#ifndef GPK_ATMEL		
#	include <cfloat>
#else
#	include <f2_t.h>
#endif
#ifndef GPK_PARTICLE_H_23627
#define GPK_PARTICLE_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm _tElement>
	struct SParticle2Forces {
		typedef	::gpk::n2<_tElement>	TCoord;

		stxp	const _tElement		VelocityEpsilon				= 0.0001f;

		TCoord						AccumulatedForce			= {};
		TCoord						Acceleration				= {};	// A vector representing the speed in a given direction
		TCoord						Velocity					= {};	// A vector representing the speed in a given direction

		inxp	bool				VelocityDepleted			()				const	noexcept	{ return (Velocity + Acceleration).LengthSquared() < (VelocityEpsilon * VelocityEpsilon); }
		// This basically does Acceleration += (Force * 1 / Mass) and Velocity += (Acceleration * Time).
		void						IntegrateAccumulatedForce	(const _tElement inverseMass, const _tElement damping, const f3_t timeElapsed)			noexcept	{
			Acceleration				+= AccumulatedForce * inverseMass;				// Calculate linear acceleration from force inputs.
			AccumulatedForce			= {};											// Clear this out now that we've used it already.

			Velocity					+= Acceleration * timeElapsed;					// Update linear velocity from both acceleration and impulse.
			Velocity					*= ::pow(damping, timeElapsed);					// Impose drag.
		}
	};

	tplt<tpnm _tElement>
	struct SParticle2 {
		// The member variables are organized such that matches the order in which they are used.
		::gpk::n2<_tElement>		Position					= {};
		SParticle2Forces<_tElement>	Forces						= {};
		_tElement					InverseMass					= 0;
		_tElement					Damping						= .99f;	// A vector representing the speed in a given direction

		inline	void				SetMass						(const f3_t mass)																			noexcept	{ InverseMass = mass ? ((_tElement)(1.0 / mass)) : 0;		}
		inxp	f3_t				GetMass						()				const	noexcept	{ return (InverseMass == 0) ? DBL_MAX : 1.0 / InverseMass;	}
		inxp	bool				HasFiniteMass				()				const	noexcept	{ return InverseMass >= 0.0f;								}
	};

	// This compact structure allows to define all the boolean states of the particle packed in a single byte.
	struct SParticle2State {
		bool					Unused						: 1;
		bool					Active						: 1;

		inxp	bool			RequiresProcessing			()				const	noexcept	{ return (false == Unused) && Active; }
	};
#pragma pack(pop)
	// This basically does FinalPosition = InitialPosition + Velocity * Time.
	tplt<tpnm _tElement>
	stin	void			particleIntegratePosition
		(	const ::gpk::n2<_tElement>	& velocity
		,	const f3_t				timeElapsed
		,	const f3_t				timeElapsedHalfSquared
		,	::gpk::n2<_tElement>		& position
		) {
		position				+= velocity * timeElapsed;
		position				+= velocity * timeElapsedHalfSquared;
	}

	tplt<tpnm _tElement>
	gpk::err_t 		integrate					(const ::gpk::view<::gpk::SParticle2<_tElement>>& particles, ::gpk::apod<::gpk::SParticle2State>& particleStates, ::gpk::view<::gpk::SParticle2<_tElement>>& particlesNext, f3_t timeElapsed, f3_t timeElapsedHalfSquared)			{
		for(u2_t iParticle = 0, particleCount = (u2_t)particleStates.size(); iParticle < particleCount; ++iParticle)
			if(particleStates[iParticle].RequiresProcessing()) {
				::gpk::SParticle2<_tElement>												& particleNext								= particlesNext[iParticle] = particles[iParticle];	// Copy the current particle state to the next
				::gpk::particleIntegratePosition(particleNext.Forces.Velocity, timeElapsed, timeElapsedHalfSquared, particleNext.Position);
				particleNext.Forces.IntegrateAccumulatedForce(particleNext.InverseMass, particleNext.Damping, timeElapsed);
				if(particleNext.Forces.VelocityDepleted())
					particleStates[iParticle].Active										= false;
			}
		return 0;
	}

	tplt<tpnm _tElement>
	struct SParticle2Integrator {
		typedef	::gpk::SParticle2	<_tElement>	TParticle;
		typedef	::gpk::n2			<_tElement>	TCoord;

		::gpk::apod<::gpk::SParticle2State>		ParticleState								= {};
		::gpk::apod<TParticle>					Particle									= {};
		::gpk::apod<TParticle>					ParticleNext								= {};
		// -----------------------------------------------------	---
		inline	gpk::err_t 				Integrate									(f3_t timeElapsed, f3_t timeElapsedHalfSquared)														{ return Integrate(ParticleNext, timeElapsed, timeElapsedHalfSquared);	}
		gpk::err_t 						Integrate									(::gpk::view<TParticle>& particleNext, f3_t timeElapsed, f3_t timeElapsedHalfSquared)			{ return ::gpk::integrate(Particle, ParticleState, particleNext, timeElapsed, timeElapsedHalfSquared);		}
		// --------------------------------------------------------
		gpk::err_t 						AddParticle					(const TParticle& particleData)																			{
			const u2_t								particleCount								= (u2_t)ParticleState.size();
			stxp	const ::gpk::SParticle2State		initialParticleState						= {false, true};

			for(u2_t iBody = 0; iBody < particleCount; ++iBody)	// Check if there is any unused particle that we can recycle.
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

	tplt<tpnm _tParticleType>
	struct SParticleBinding {
		typedef	_tParticleType	TParticleType;

		TParticleType			Binding										= {};
		s2_t					IndexParticlePhysics						= -1;
	};

	tplt<tpnm _tParticleType, tpnm _tCoord>
	gpk::err_t 		addParticle
		(	const _tParticleType										& particleType
		,	::gpk::apod<::gpk::SParticleBinding<_tParticleType>>	& particleInstances
		,	::gpk::SParticle2Integrator<_tCoord>						& particleIntegrator
		,	const ::gpk::SParticle2<_tCoord>							& particleDefinition
		)
	{
		::gpk::SParticleBinding<_tParticleType>	newInstance									= {};
		newInstance.Binding		= particleType;
		if_fail_fe(newInstance.IndexParticlePhysics = particleIntegrator.AddParticle(particleDefinition));
		return particleInstances.push_back(newInstance);
	}

	tplt<tpnm _tParticleType, tpnm _tCoord>
	struct SParticleSystem {
		typedef	::gpk::SParticleBinding<_tParticleType>	TParticleInstance;
		typedef	::gpk::SParticle2Integrator<_tCoord>	TIntegrator;

		::gpk::apod<TParticleInstance>	Instances									= {};
		TIntegrator						Integrator									= {};
	};

	tplt<tpnm _tParticleType, tpnm _tCoord>
	stin	gpk::err_t 		addParticle					(const _tParticleType& particleType, ::gpk::SParticleSystem<_tParticleType, _tCoord> & particleSystem,	const ::gpk::SParticle2<_tCoord> & particleDefinition)	{
		return addParticle(particleType, particleSystem.Instances, particleSystem.Integrator, particleDefinition);
	}

	// simple particle force integrator
	struct SParticles3 {
		::gpk::an3f2_t					OldPosition			= {};
		::gpk::an3f2_t					Position			= {};
		::gpk::an3f2_t					Direction			= {};
		::gpk::af2_t					Speed				= {};

		gpk::err_t 				IntegrateSpeed		(f3_t secondsLastFrame);

		gpk::err_t 				Create				(n3f2_c & position, n3f2_c & direction, f2_t speed);
		gpk::err_t 				Remove				(s2_t iParticle);

		gpk::err_t 				Load				(::gpk::vcu0_t & input);
		gpk::err_t 				Save				(::gpk::au0_t & output)	const;
	};
} // namespace

#endif // GPK_PARTICLE_H_23627
