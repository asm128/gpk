#include "gpk_apod_n3.h"

#ifndef GPK_ATMEL		
#	include <cfloat>
#else
#	include <f2_t.h>
#endif
#ifndef GPK_PARTICLE_H_23627
#define GPK_PARTICLE_H_23627

nmsp gpk
{
#pragma pack(push, 1)
	tplt<tpnm _tElement>
	struct SParticle2Forces {
		tydf	::gpk::n2<_tElement>	TCoord;

		stxp	cnst _tElement		VelocityEpsilon				= 0.0001f;

		TCoord						AccumulatedForce			= {};
		TCoord						Acceleration				= {};	// A vector representing the speed in a given direction
		TCoord						Velocity					= {};	// A vector representing the speed in a given direction

		inxp	b8_t				VelocityDepleted			()				csnx	{ return (Velocity + Acceleration).LengthSquared() < (VelocityEpsilon * VelocityEpsilon); }
		// This basically does Acceleration += (Force * 1 / Mass) and Velocity += (Acceleration * Time).
		void						IntegrateAccumulatedForce	(cnst _tElement inverseMass, cnst _tElement damping, cnst f3_t timeElapsed)			nxpt	{
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

		inln	void				SetMass						(cnst f3_t mass)																			nxpt	{ InverseMass = mass ? ((_tElement)(1.0 / mass)) : 0;		}
		inxp	f3_t				GetMass						()				csnx	{ return (InverseMass == 0) ? DBL_MAX : 1.0 / InverseMass;	}
		inxp	b8_t				HasFiniteMass				()				csnx	{ return InverseMass >= 0.0f;								}
	};

	// This compact structure allows to define all the boolean states of the particle packed in a single byte.
	struct SParticle2State {
		b8_t					Unused						: 1;
		b8_t					Active						: 1;

		inxp	b8_t			RequiresProcessing			()				csnx	{ return (false == Unused) && Active; }
	};
#pragma pack(pop)
	// This basically does FinalPosition = InitialPosition + Velocity * Time.
	tplt<tpnm _tElement>
	stin	void			particleIntegratePosition
		(	cnst ::gpk::n2<_tElement>	& velocity
		,	cnst f3_t				timeElapsed
		,	cnst f3_t				timeElapsedHalfSquared
		,	::gpk::n2<_tElement>		& position
		) {
		position				+= velocity * timeElapsed;
		position				+= velocity * timeElapsedHalfSquared;
	}

	tplt<tpnm _tElement>
	gpk::err_t 		integrate					(cnst ::gpk::view<::gpk::SParticle2<_tElement>>& particles, ::gpk::apod<::gpk::SParticle2State>& particleStates, ::gpk::view<::gpk::SParticle2<_tElement>>& particlesNext, f3_t timeElapsed, f3_t timeElapsedHalfSquared)			{
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
		tydf	::gpk::SParticle2	<_tElement>	TParticle;
		tydf	::gpk::n2			<_tElement>	TCoord;

		::gpk::apod<::gpk::SParticle2State>		ParticleState								= {};
		::gpk::apod<TParticle>					Particle									= {};
		::gpk::apod<TParticle>					ParticleNext								= {};
		// -----------------------------------------------------	---
		inln	gpk::err_t 				Integrate									(f3_t timeElapsed, f3_t timeElapsedHalfSquared)														{ return Integrate(ParticleNext, timeElapsed, timeElapsedHalfSquared);	}
		gpk::err_t 						Integrate									(::gpk::view<TParticle>& particleNext, f3_t timeElapsed, f3_t timeElapsedHalfSquared)			{ return ::gpk::integrate(Particle, ParticleState, particleNext, timeElapsed, timeElapsedHalfSquared);		}
		// --------------------------------------------------------
		gpk::err_t 						AddParticle					(cnst TParticle& particleData)																			{
			u2_c								particleCount								= (u2_t)ParticleState.size();
			stxp	cnst ::gpk::SParticle2State		initialParticleState						= {false, true};

			for(u2_t iBody = 0; iBody < particleCount; ++iBody)	// Check if there is any unused particle that we can recycle.
				if( ParticleState	[iBody].Unused ) {
					ParticleState	[iBody]													= initialParticleState;
					Particle		[iBody]													=
					ParticleNext	[iBody]													= particleData;
					return iBody;
				}
			if_fail_fe(gpk::resize(particleCount + 1, ParticleState, Particle, ParticleNext));
			ParticleState	[particleCount]										= initialParticleState;
			Particle		[particleCount]										=
			ParticleNext	[particleCount]										= particleData;
			return particleCount;
		}
	};

	tplt<tpnm _tParticleType>
	struct SParticleBinding {
		tydf	_tParticleType	TParticleType;

		TParticleType			Binding										= {};
		s2_t					IndexParticlePhysics						= -1;
	};

	tplt<tpnm _tParticleType, tpnm _tCoord>
	gpk::err_t 		addParticle
		(	cnst _tParticleType										& particleType
		,	::gpk::apod<::gpk::SParticleBinding<_tParticleType>>	& particleInstances
		,	::gpk::SParticle2Integrator<_tCoord>						& particleIntegrator
		,	cnst ::gpk::SParticle2<_tCoord>							& particleDefinition
		)
	{
		::gpk::SParticleBinding<_tParticleType>	newInstance									= {};
		newInstance.Binding		= particleType;
		if_fail_fe(newInstance.IndexParticlePhysics = particleIntegrator.AddParticle(particleDefinition));
		return particleInstances.push_back(newInstance);
	}

	tplt<tpnm _tParticleType, tpnm _tCoord>
	struct SParticleSystem {
		tydf	::gpk::SParticleBinding<_tParticleType>	TParticleInstance;
		tydf	::gpk::SParticle2Integrator<_tCoord>	TIntegrator;

		::gpk::apod<TParticleInstance>	Instances									= {};
		TIntegrator						Integrator									= {};
	};

	tplt<tpnm _tParticleType, tpnm _tCoord>
	stin	gpk::err_t 		addParticle					(cnst _tParticleType& particleType, ::gpk::SParticleSystem<_tParticleType, _tCoord> & particleSystem,	cnst ::gpk::SParticle2<_tCoord> & particleDefinition)	{
		return addParticle(particleType, particleSystem.Instances, particleSystem.Integrator, particleDefinition);
	}

	// simple particle force integrator
	struct SParticles3 {
		::gpk::a3f2_t					OldPosition			= {};
		::gpk::a3f2_t					Position			= {};
		::gpk::a3f2_t					Direction			= {};
		::gpk::af2_t					Speed				= {};

		gpk::err_t 				IntegrateSpeed		(f3_t secondsLastFrame);

		gpk::err_t 				Create				(n3f2_c & position, n3f2_c & direction, f2_t speed);
		gpk::err_t 				Remove				(s2_t iParticle);

		gpk::err_t 				Load				(::gpk::vcu0_t & input);
		gpk::err_t 				Save				(::gpk::au0_t & output)	cnst;
	};
} // nmsp

#endif // GPK_PARTICLE_H_23627
