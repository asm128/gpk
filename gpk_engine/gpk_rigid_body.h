#include "gpk_matrix.h"
#include "gpk_enum.h"

#ifndef GPK_RIGIDBODY_H_23627
#define GPK_RIGIDBODY_H_23627

namespace gpk
{
#pragma pack(push, 1)
	// The following structure stores forces information for a 3D rigid body. These change almost every frame during runtime
	struct SBodyForces {
		::gpk::n3f2_t		Velocity						= {};
		::gpk::n3f2_t		Acceleration					= {};
		::gpk::n3f2_t		Rotation						= {};
	};

	// The following structure stores mass information for a 3D rigid body. These almost never change during runtime
	struct SBodyMass {
		float				LinearDamping					= 1.0f;
		float				AngularDamping					= 1.0f;
		float				InverseMass						= 0;
		::gpk::m3f32		InverseAngularMassTensor		= {1,0,0,0,1,0,0,0,1};

		inxp	double		GetMass							()	{
			return (InverseMass == 0) ? DBL_MAX : 1.0 / InverseMass; 
		}
	};

	struct SBodyCenter {
		::gpk::n3f2_t		Position						= {};
		::gpk::quatf32		Orientation						= {0, 0, 0, 1};
	};

	GDEFINE_ENUM_TYPE(BOUNDING_TYPE, ::gpk::u0_t);
	GDEFINE_ENUM_VALUE(BOUNDING_TYPE, Sphere	, 0);
	GDEFINE_ENUM_VALUE(BOUNDING_TYPE, AABB		, 1);
	GDEFINE_ENUM_VALUE(BOUNDING_TYPE, Ray		, 2);
	GDEFINE_ENUM_VALUE(BOUNDING_TYPE, Cylinder	, 3);

	GDEFINE_ENUM_VALUE(BOUNDING_TYPE, Inward	, 4);

	struct SBodyFlags {
		::gpk::u0_t				BVType							: 3;
		::gpk::u0_t				Collides						: 1;
		::gpk::u0_t				Active							: 1;
		::gpk::u0_t				Falling							: 1;
		::gpk::u0_t				UpdatedTransform				: 1;
		::gpk::u0_t				UpdatedTensorWorld				: 1;	// Tell the object that our matrices are up to date
	};

	struct SBodyFrame {
		::gpk::m3f32		InverseInertiaTensorWorld		= {1, 0, 0, 0, 1, 0, 0, 0, 1};
		::gpk::n3f2_t		LastFrameAcceleration			= {0, 0, 0};
		::gpk::n3f2_t		AccumulatedForce				= {0, 0, 0};
		::gpk::n3f2_t		AccumulatedTorque				= {0, 0, 0};

		::gpk::s2_t				ClearAccumulators				()	{ AccumulatedForce = AccumulatedTorque = {}; return 0; }
	};

	struct SBodyVolume {
		::gpk::n3f2_t		HalfSizes						= {.5f, .5f, .5f};
	};
#pragma pack(pop)
	void				updateTransform					(::gpk::SBodyCenter & bodyTransform, ::gpk::m4f32 & transformLocal);
	::gpk::s2_t				integrateForces					(double duration, ::gpk::SBodyFrame & bodyFrame, ::gpk::SBodyForces & bodyForce, const ::gpk::SBodyMass & bodyMass);
	::gpk::s2_t				integratePosition				(double duration, double durationHalfSquared, ::gpk::SBodyFlags & bodyFlags, ::gpk::SBodyCenter & bodyTransform, const ::gpk::SBodyForces & bodyForces);
	void				transformInertiaTensor			(::gpk::m3f32 & iitWorld, const ::gpk::m3f32 & iitBody, const ::gpk::m4f32 &rotmat);


	struct SRigidBodyIntegrator {
		::gpk::apod<::gpk::SBodyFrame	>	Frames							= {};
		::gpk::apod<::gpk::SBodyFlags	>	Flags							= {};
		::gpk::apod<::gpk::SBodyMass	>	Masses							= {};
		::gpk::apod<::gpk::SBodyForces	>	Forces							= {};
		::gpk::apod<::gpk::SBodyCenter	>	Centers							= {};
		::gpk::apod<::gpk::SBodyVolume	>	BoundingVolumes					= {};
		::gpk::apod<::gpk::m4f32		>	TransformsLocal					= {};

		stxp	::gpk::m4f32				MatrixIdentity4					= {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
		stxp	::gpk::m3f32				MatrixIdentity3					= {1,0,0,0,1,0,0,0,1};

		gpk::err_t						Load							(::gpk::vcu0_t & input);
		gpk::err_t						Save							(::gpk::au0_t & output)	const;
		inline	gpk::err_t				Load							(::gpk::vcsc_t & input)			{ return Load(*((::gpk::vcu0_t*)&input)); }
		inline	gpk::err_t				Save							(::gpk::as0_t & output)	const	{ return Save(*((::gpk::apod<::gpk::u0_t>*)&output)); }

		::gpk::s2_t 							ZeroCenters						()	{ return Centers.fill({}); }
		::gpk::s2_t 							ZeroForces						()	{ return Forces.fill({}); }
		::gpk::s2_t 							ZeroFrames						()	{ return Frames.fill({}); }

		::gpk::s2_t 							Reset							()	{
			ZeroForces();
			ZeroCenters();
			return ZeroFrames();
		}
		
		::gpk::s2_t 							Clear							()	{
			::gpk::clear(Frames, Flags, Forces, Masses, Centers, BoundingVolumes, TransformsLocal);
			return 0;
		}

		::gpk::s2_t 							Clone							(::gpk::u2_t iBody)	{
			Frames					.push_back(::gpk::SBodyFrame		{Frames				[iBody]});
			Flags					.push_back(::gpk::SBodyFlags		{Flags				[iBody]});
			Forces					.push_back(::gpk::SBodyForces		{Forces				[iBody]});
			Masses					.push_back(::gpk::SBodyMass			{Masses				[iBody]});
			Centers					.push_back(::gpk::SBodyCenter		{Centers			[iBody]});
			BoundingVolumes			.push_back(::gpk::SBodyVolume		{BoundingVolumes	[iBody]});
			return TransformsLocal	.push_back(::gpk::m4f32				{TransformsLocal	[iBody]});
		}

		::gpk::s2_t 							Create							()	{
			Frames					.push_back({});
			Flags					.push_back({});
			Forces					.push_back({});
			Masses					.push_back({});
			Centers					.push_back({});
			BoundingVolumes			.push_back({});
			return TransformsLocal	.push_back(MatrixIdentity4);
		}

		::gpk::s2_t 							Delete							(::gpk::u2_t iBody)	{
			Frames					.remove_unordered(iBody);
			Flags					.remove_unordered(iBody);
			Forces					.remove_unordered(iBody);
			Masses					.remove_unordered(iBody);
			Centers					.remove_unordered(iBody);
			BoundingVolumes			.remove_unordered(iBody);
			return TransformsLocal	.remove_unordered(iBody);
		}

		::gpk::s2_t 							Create							(::gpk::u2_t bodyCount)	{
			const ::gpk::u2_t							indexFirstBody					= Centers.size();
			const ::gpk::u2_t							newSize							= indexFirstBody + bodyCount;
			if_fail_fe(Frames				.resize(newSize, {}));				
			if_fail_fe(Flags				.resize(newSize, {}));
			if_fail_fe(Forces				.resize(newSize, {}));
			if_fail_fe(Masses				.resize(newSize, {}));
			if_fail_fe(Centers			.resize(newSize, {}));
			if_fail_fe(BoundingVolumes	.resize(newSize, {}));		
			if_fail_fe(TransformsLocal	.resize(newSize, MatrixIdentity4));
			return indexFirstBody;
		}

		::gpk::s2_t 							GetTransform					(::gpk::u2_t iBody, ::gpk::m4f32	& transform)	{
			::gpk::SBodyFlags						& bodyFlags						= Flags				[iBody];
			::gpk::m4f32							& bodyTransformLocal			= TransformsLocal	[iBody];
			if(false == bodyFlags.UpdatedTransform || false == bodyFlags.UpdatedTensorWorld) {
				if(false == bodyFlags.UpdatedTransform) {
					::gpk::SBodyCenter						& bodyCenter					= Centers[iBody];
					::gpk::updateTransform(bodyCenter, bodyTransformLocal);
					bodyFlags.UpdatedTransform			= true;
					bodyFlags.UpdatedTensorWorld		= false;
				}
				if(false == bodyFlags.UpdatedTensorWorld) {
					::gpk::SBodyFrame						& bodyFrame						= Frames[iBody];
					::gpk::SBodyMass						& bodyMass						= Masses[iBody];
					::gpk::transformInertiaTensor(bodyFrame.InverseInertiaTensorWorld, bodyMass.InverseAngularMassTensor, bodyTransformLocal);
					bodyFlags.UpdatedTensorWorld		= true;
				}
			}
			transform							= bodyTransformLocal;
			return 0;
		}

		void								AddForceAtPoint					(::gpk::u2_t iBody, const ::gpk::n3f2_t & force, const ::gpk::n3f2_t & point)	{
			::gpk::SBodyCenter						& bodyCenter					= Centers	[iBody];
			::gpk::SBodyFlags						& bodyFlags						= Flags	[iBody];
			::gpk::SBodyFrame						& bodyFrame						= Frames[iBody];
			bodyFrame.AccumulatedTorque			+= (point - bodyCenter.Position).Cross( force );// Convert to coordinates relative to center of mass.
			bodyFrame.AccumulatedForce 			+= force;
			bodyFlags.Active					= true;
		}

		::gpk::s2_t								Integrate						(double duration)	{
			const double							durationHalfSquared				= duration * duration * 0.5;
			::gpk::m4f32							dummy;
			for(::gpk::u2_t iBody = 0; iBody < Flags.size(); ++iBody) {
				::gpk::SBodyFlags						& bodyFlags						= Flags[iBody];
				if(false == bodyFlags.Active)
					continue;

				GetTransform(iBody, dummy); // ensures tensor matrix is up to date

				::gpk::SBodyFrame					& bodyFrame						= Frames	[iBody];
				::gpk::SBodyForces					& bodyForces					= Forces	[iBody];
				::gpk::SBodyMass					& bodyMass						= Masses	[iBody];
				::gpk::SBodyCenter					& bodyCenter					= Centers	[iBody];
				::gpk::integrateForces	(duration, bodyFrame, bodyForces, bodyMass);
				::gpk::integratePosition(duration, durationHalfSquared, bodyFlags, bodyCenter, bodyForces);
				if(bodyForces.Acceleration.LengthSquared() < .001 && bodyForces.Velocity.LengthSquared() < .001 && bodyForces.Rotation.LengthSquared() < .00000001) {
					bodyFlags.Active				= false;
					bodyForces						= {};
				}
			}
			return 0;
		}

		inline	const SBodyFlags&			GetBodyFlags					(::gpk::u2_t iBody)	const	{ return Flags  [iBody]; }
		inline	const SBodyFrame&			GetBodyFrame					(::gpk::u2_t iBody)	const	{ return Frames [iBody]; }
		inline	const SBodyForces&			GetBodyForces					(::gpk::u2_t iBody)	const	{ return Forces [iBody]; }
		inline	const SBodyMass&			GetBodyMass						(::gpk::u2_t iBody)	const	{ return Masses [iBody]; }
		inline	const SBodyCenter&			GetBodyCenter					(::gpk::u2_t iBody)	const	{ return Centers[iBody]; }
		inline	SBodyFlags&					GetBodyFlags					(::gpk::u2_t iBody)			{ return Flags  [iBody]; }
		inline	SBodyFrame&					GetBodyFrame					(::gpk::u2_t iBody)			{ return Frames [iBody]; }
		inline	SBodyForces&				GetBodyForces					(::gpk::u2_t iBody)			{ return Forces [iBody]; }
		inline	SBodyMass&					GetBodyMass						(::gpk::u2_t iBody)			{ return Masses [iBody]; }
		inline	SBodyCenter&				GetBodyCenter					(::gpk::u2_t iBody)			{ return Centers[iBody]; }


		inline	bool						Active							(::gpk::u2_t iBody)									const	{ return Flags[iBody].Active; }
		inline	bool						Collides						(::gpk::u2_t iBody)									const	{ return Flags[iBody].Collides; }
		inline	float						GetMass							(::gpk::u2_t iBody)									const	{ return 1.0f / Masses[iBody].InverseMass; }
		inline	void						GetPosition						(::gpk::u2_t iBody, ::gpk::n3f2_t & position)			const	{ position		= Centers[iBody].Position; }
		inline	void						GetOrientation					(::gpk::u2_t iBody, ::gpk::quatf32 & orientation)		const	{ orientation	= Centers[iBody].Orientation; }
		inline	void						GetRotation						(::gpk::u2_t iBody, ::gpk::n3f2_t & rotation)			const	{ rotation		= Forces[iBody].Rotation;		}
		inline	void						GetAcceleration					(::gpk::u2_t iBody, ::gpk::n3f2_t & acceleration)		const	{ acceleration	= Forces[iBody].Acceleration;	}
		inline	void						GetVelocity						(::gpk::u2_t iBody, ::gpk::n3f2_t & velocity)			const	{ velocity		= Forces[iBody].Velocity;		}

		inline	void						SetActive						(::gpk::u2_t iBody, bool active)								{ Flags[iBody].Active	= active; }
		inline	void						SetCollides						(::gpk::u2_t iBody, bool collides)								{ Flags[iBody].Collides	= collides; }
		inline	void						SetHalfSizes					(::gpk::u2_t iBody, const ::gpk::n3f2_t & halfSizes)			{ BoundingVolumes[iBody].HalfSizes	= halfSizes; }
		
		inline	void						SetMassInverse					(::gpk::u2_t iBody, float inverseMass)							{ Masses[iBody].InverseMass = inverseMass; }
		inline	void						SetMass							(::gpk::u2_t iBody, float mass)								{ Masses[iBody].InverseMass = 1.0f / mass; }
		inline	void						AddForce						(::gpk::u2_t iBody, const ::gpk::n3f2_t & force)				{ Frames[iBody].AccumulatedForce += force; }

		void								SetPosition						(::gpk::u2_t iBody, const ::gpk::n3f2_t & newPosition)			{
			::gpk::SBodyCenter						& bodyCenter					= Centers[iBody];
			if(0 == memcmp(&newPosition.x, &bodyCenter.Position.x, sizeof(::gpk::n3f2_t)))
				return;
			bodyCenter.Position					= newPosition;
			::gpk::SBodyFlags						& bodyFlags						= Flags[iBody];
			bodyFlags.UpdatedTransform			= 
			bodyFlags.UpdatedTensorWorld		= false;
		}
		void								SetOrientation					(::gpk::u2_t iBody, const ::gpk::quatf32 & newOrientation)		{
			::gpk::SBodyCenter						& bodyCenter					= Centers[iBody];
			if(0 == memcmp(&newOrientation.x, &bodyCenter.Orientation.x, sizeof(::gpk::quat<float>)))
				return;
			bodyCenter.Orientation				= newOrientation;
			::gpk::SBodyFlags						& bodyFlags						= Flags[iBody];
			bodyFlags.UpdatedTransform			= 
			bodyFlags.UpdatedTensorWorld		= false;
		}
		void								SetVelocity						(::gpk::u2_t iBody, const ::gpk::n3f2_t & newVelocity)			{
			::gpk::SBodyForces						& bodyForces					= Forces[iBody];
			if(0 == memcmp(&newVelocity.x, &bodyForces.Velocity.x, sizeof(::gpk::n3f2_t)))
				return;
			bodyForces.Velocity					= newVelocity;
			::gpk::SBodyFlags						& bodyFlags						= Flags[iBody];
			bodyFlags.Active					= true;
			if(newVelocity.y) 
				bodyFlags.Falling					= true; 
		}
		void								SetAcceleration					(::gpk::u2_t iBody, const ::gpk::n3f2_t & acceleration)			{
			::gpk::SBodyForces						& bodyForces					= Forces[iBody];
			if(0 == memcmp(&acceleration.x, &bodyForces.Acceleration.x, sizeof(::gpk::n3f2_t)))
				return;
			bodyForces.Acceleration				= acceleration;
			::gpk::SBodyFlags						& bodyFlags						= Flags[iBody];
			bodyFlags.Active					= true;
		}
		void								SetRotation						(::gpk::u2_t iBody, const ::gpk::n3f2_t & newRotation)			{
			::gpk::SBodyForces						& bodyForces					= Forces[iBody];
			if(0 == memcmp(&newRotation.x, &bodyForces.Rotation.x, sizeof(::gpk::n3f2_t)))
				return;
			bodyForces.Rotation					= newRotation;
			::gpk::SBodyFlags						& bodyFlags						= Flags[iBody];
			bodyFlags.Active					= true;
			if(newRotation.z || newRotation.x) 
				bodyFlags.Falling					= true; 
		}

	};

} // namespace

#endif // CED_RIGIDBODY_H_23627
