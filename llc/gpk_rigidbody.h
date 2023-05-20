#include "gpk_matrix.h"
#include "gpk_array.h"
#include "gpk_enum.h"

#ifndef GPK_RIGIDBODY_H_234872398472341
#define GPK_RIGIDBODY_H_234872398472341

namespace gpk
{
#pragma pack(push, 1)
	// The following structure stores forces information for a 3D rigid body. These change almost every frame during runtime
	struct SBodyForces {
		::gpk::n3f									Velocity						= {};
		::gpk::n3f									Acceleration					= {};
		::gpk::n3f									Rotation						= {};
	};

	// The following structure stores mass information for a 3D rigid body. These almost never change during runtime
	struct SBodyMass {
		float										LinearDamping					= 1.0f;
		float										AngularDamping					= 1.0f;
		float										InverseMass						= 0;
		::gpk::m3f									InverseAngularMassTensor		= {1,0,0,0,1,0,0,0,1};

		inlcxpr	double								GetMass							()	{
			return (InverseMass == 0) ? DBL_MAX : 1.0 / InverseMass; 
		}
	};

	struct SBodyCenter {
		::gpk::n3f									Position						= {};
		::gpk::quatf								Orientation						= {0, 0, 0, 1};
	};

	GDEFINE_ENUM_TYPE(BOUNDING_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(BOUNDING_TYPE, Sphere	, 0);
	GDEFINE_ENUM_VALUE(BOUNDING_TYPE, AABB		, 1);
	GDEFINE_ENUM_VALUE(BOUNDING_TYPE, Ray		, 2);
	GDEFINE_ENUM_VALUE(BOUNDING_TYPE, Cylinder	, 3);

	GDEFINE_ENUM_VALUE(BOUNDING_TYPE, Inward	, 4);

	struct SBodyFlags {
		uint8_t										BVType							: 3;
		uint8_t										Collides						: 1;
		uint8_t										Active							: 1;
		uint8_t										Falling							: 1;
		uint8_t										UpdatedTransform				: 1;
		uint8_t										UpdatedTensorWorld				: 1;	// Tell the object that our matrices are up to date
	};

	struct SBodyFrame {
		::gpk::m3f									InverseInertiaTensorWorld		= {1, 0, 0, 0, 1, 0, 0, 0, 1};
		::gpk::n3f									LastFrameAcceleration			= {0, 0, 0};
		::gpk::n3f									AccumulatedForce				= {0, 0, 0};
		::gpk::n3f									AccumulatedTorque				= {0, 0, 0};

		int32_t										ClearAccumulators				()	{ AccumulatedForce = AccumulatedTorque = {}; return 0; }
	};

	struct SBodyVolume {
		::gpk::n3f									HalfSizes						= {.5f, .5f, .5f};
	};
#pragma pack(pop)
	void										updateTransform					(::gpk::SBodyCenter & bodyTransform, ::gpk::m4f & transformLocal);
	int32_t										integrateForces					(double duration, ::gpk::SBodyFrame & bodyFrame, ::gpk::SBodyForces & bodyForce, const ::gpk::SBodyMass & bodyMass);
	int32_t										integratePosition				(double duration, double durationHalfSquared, ::gpk::SBodyFlags & bodyFlags, ::gpk::SBodyCenter & bodyTransform, const ::gpk::SBodyForces & bodyForces);
	void										transformInertiaTensor			(::gpk::m3f & iitWorld, const ::gpk::m3f &iitBody, const ::gpk::m4f &rotmat);


	struct SRigidBodyIntegrator {
		::gpk::apod<::gpk::SBodyFrame	>			Frames							= {};
		::gpk::apod<::gpk::SBodyFlags	>			Flags							= {};
		::gpk::apod<::gpk::SBodyMass	>			Masses							= {};
		::gpk::apod<::gpk::SBodyForces	>			Forces							= {};
		::gpk::apod<::gpk::SBodyCenter	>			Centers							= {};
		::gpk::apod<::gpk::SBodyVolume	>			BoundingVolumes					= {};
		::gpk::apod<::gpk::m4f			>			TransformsLocal					= {};

		stacxpr	const ::gpk::m4f					MatrixIdentity4					= {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
		stacxpr	const ::gpk::m3f					MatrixIdentity3					= {1,0,0,0,1,0,0,0,1};

		int32_t 									ZeroCenters						()	{ return Centers.fill({}); }
		int32_t 									ZeroForces						()	{ return Forces.fill({}); }
		int32_t 									ZeroFrames						()	{ return Frames.fill({}); }

		int32_t 									Reset							()	{
			ZeroForces();
			ZeroCenters();
			return ZeroFrames();
		}
		
		int32_t 									Clear							()	{
			::gpk::clear(Frames, Flags, Forces, Masses, Centers, BoundingVolumes, TransformsLocal);
			return 0;
		}

		int32_t 									Clone							(uint32_t iBody)	{
			Frames					.push_back(::gpk::SBodyFrame		{Frames				[iBody]});
			Flags					.push_back(::gpk::SBodyFlags		{Flags				[iBody]});
			Forces					.push_back(::gpk::SBodyForces		{Forces				[iBody]});
			Masses					.push_back(::gpk::SBodyMass			{Masses				[iBody]});
			Centers					.push_back(::gpk::SBodyCenter		{Centers			[iBody]});
			BoundingVolumes			.push_back(::gpk::SBodyVolume		{BoundingVolumes	[iBody]});
			return TransformsLocal	.push_back(::gpk::m4f				{TransformsLocal	[iBody]});
		}

		int32_t 									Create							()	{
			Frames					.push_back({});
			Flags					.push_back({});
			Forces					.push_back({});
			Masses					.push_back({});
			Centers					.push_back({});
			BoundingVolumes			.push_back({});
			return TransformsLocal	.push_back(MatrixIdentity4);
		}

		int32_t 									Create							(uint32_t bodyCount)	{
			const uint32_t									indexFirstBody					= Centers.size();
			const uint32_t									newSize							= indexFirstBody + bodyCount;
			gpk_necs(Frames				.resize(newSize, {}));				
			gpk_necs(Flags				.resize(newSize, {}));
			gpk_necs(Forces				.resize(newSize, {}));
			gpk_necs(Masses				.resize(newSize, {}));
			gpk_necs(Centers			.resize(newSize, {}));
			gpk_necs(BoundingVolumes	.resize(newSize, {}));		
			gpk_necs(TransformsLocal	.resize(newSize, MatrixIdentity4));
			return indexFirstBody;
		}

		int32_t 									GetTransform					(uint32_t iBody, ::gpk::m4f	& transform)	{
			::gpk::SBodyFlags								& bodyFlags						= Flags				[iBody];
			::gpk::m4f										& bodyTransformLocal			= TransformsLocal	[iBody];
			if(false == bodyFlags.UpdatedTransform || false == bodyFlags.UpdatedTensorWorld) {
				if(false == bodyFlags.UpdatedTransform) {
					::gpk::SBodyCenter								& bodyCenter					= Centers[iBody];
					::gpk::updateTransform(bodyCenter, bodyTransformLocal);
					bodyFlags.UpdatedTransform					= true;
					bodyFlags.UpdatedTensorWorld				= false;
				}
				if(false == bodyFlags.UpdatedTensorWorld) {
					::gpk::SBodyFrame								& bodyFrame						= Frames[iBody];
					::gpk::SBodyMass								& bodyMass						= Masses[iBody];
					::gpk::transformInertiaTensor(bodyFrame.InverseInertiaTensorWorld, bodyMass.InverseAngularMassTensor, bodyTransformLocal);
					bodyFlags.UpdatedTensorWorld				= true;
				}
			}
			transform									= bodyTransformLocal;
			return 0;
		}

		void										AddForceAtPoint					(uint32_t iBody, const ::gpk::n3f32 & force, const ::gpk::n3f32 & point)	{
			::gpk::SBodyCenter								& bodyCenter					= Centers	[iBody];
			::gpk::SBodyFlags								& bodyFlags						= Flags	[iBody];
			::gpk::SBodyFrame								& bodyFrame						= Frames[iBody];
			bodyFrame.AccumulatedTorque					+= (point - bodyCenter.Position).Cross( force );// Convert to coordinates relative to center of mass.
			bodyFrame.AccumulatedForce 					+= force;
			bodyFlags.Active							= true;
		}

		int32_t										Integrate						(double duration)	{
			const double									durationHalfSquared				= duration * duration * 0.5;
			::gpk::m4f										dummy;
			for(uint32_t iBody = 0; iBody < Flags.size(); ++iBody) {
				::gpk::SBodyFlags							& bodyFlags						= Flags[iBody];
				if(false == bodyFlags.Active)
					continue;

				GetTransform(iBody, dummy); // ensures tensor matrix is up to date

				::gpk::SBodyFrame								& bodyFrame						= Frames[iBody];
				::gpk::SBodyForces								& bodyForces					= Forces	[iBody];
				::gpk::SBodyMass								& bodyMass						= Masses	[iBody];
				::gpk::SBodyCenter								& bodyCenter					= Centers	[iBody];
				::gpk::integrateForces	(duration, bodyFrame, bodyForces, bodyMass);
				::gpk::integratePosition(duration, durationHalfSquared, bodyFlags, bodyCenter, bodyForces);
				if(bodyForces.Acceleration.LengthSquared() < .01 && bodyForces.Velocity.LengthSquared() < .01 && bodyForces.Rotation.LengthSquared() < .01) {
					bodyFlags.Active							= false;
					bodyForces									= {};
				}
			}
			return 0;
		}

		inline	bool								Active							(uint32_t iBody)									const	{ return Flags[iBody].Active; }
		inline	float								GetMass							(uint32_t iBody)									const	{ return 1.0f / Masses[iBody].InverseMass; }
		inline	void								GetPosition						(uint32_t iBody, ::gpk::n3f32 & position)			const	{ position		= Centers[iBody].Position; }
		inline	void								GetOrientation					(uint32_t iBody, ::gpk::quatf32 & orientation)		const	{ orientation	= Centers[iBody].Orientation; }
		inline	void								GetRotation						(uint32_t iBody, ::gpk::n3f32 & rotation)			const	{ rotation		= Forces[iBody].Rotation;		}
		inline	void								GetAcceleration					(uint32_t iBody, ::gpk::n3f32 & acceleration)		const	{ acceleration	= Forces[iBody].Acceleration;	}
		inline	void								GetVelocity						(uint32_t iBody, ::gpk::n3f32 & velocity)			const	{ velocity		= Forces[iBody].Velocity;		}

		inline	void								SetActive						(uint32_t iBody, bool active)								{ Flags[iBody].Active = active; }
		inline	void								SetMassInverse					(uint32_t iBody, float inverseMass)							{ Masses[iBody].InverseMass = inverseMass; }
		inline	void								SetMass							(uint32_t iBody, float mass)								{ Masses[iBody].InverseMass = 1.0f / mass; }
		inline	void								AddForce						(uint32_t iBody, const ::gpk::n3f32 & force)				{ Frames[iBody].AccumulatedForce += force; }

		void										SetPosition						(uint32_t iBody, const ::gpk::n3f32 & newPosition)			{
			::gpk::SBodyCenter								& bodyCenter					= Centers[iBody];
			if(0 == memcmp(&newPosition.x, &bodyCenter.Position.x, sizeof(::gpk::n3<float>)))
				return;
			bodyCenter.Position							= newPosition;
			::gpk::SBodyFlags								& bodyFlags						= Flags[iBody];
			bodyFlags.UpdatedTransform					= 
			bodyFlags.UpdatedTensorWorld				= false;
		}
		void										SetOrientation					(uint32_t iBody, const ::gpk::quatf32 & newOrientation)		{
			::gpk::SBodyCenter								& bodyCenter					= Centers[iBody];
			if(0 == memcmp(&newOrientation.x, &bodyCenter.Orientation.x, sizeof(::gpk::quat<float>)))
				return;
			bodyCenter.Orientation						= newOrientation;
			::gpk::SBodyFlags								& bodyFlags						= Flags[iBody];
			bodyFlags.UpdatedTransform					= 
			bodyFlags.UpdatedTensorWorld				= false;
		}
		void										SetVelocity						(uint32_t iBody, const ::gpk::n3f32 & newVelocity)			{
			::gpk::SBodyForces								& bodyForces					= Forces[iBody];
			if(0 == memcmp(&newVelocity.x, &bodyForces.Velocity.x, sizeof(::gpk::n3<float>)))
				return;
			bodyForces.Velocity							= newVelocity;
			::gpk::SBodyFlags								& bodyFlags						= Flags[iBody];
			bodyFlags.Active							= true;
			if(newVelocity.y) 
				bodyFlags.Falling							= true; 
		}
		void										SetAcceleration					(uint32_t iBody, const ::gpk::n3f32 & acceleration)			{
			::gpk::SBodyForces								& bodyForces					= Forces[iBody];
			if(0 == memcmp(&acceleration.x, &bodyForces.Acceleration.x, sizeof(::gpk::n3<float>)))
				return;
			bodyForces.Acceleration						= acceleration;
			::gpk::SBodyFlags								& bodyFlags						= Flags[iBody];
			bodyFlags.Active							= true;
		}
		void										SetRotation						(uint32_t iBody, const ::gpk::n3f32 & newRotation)			{
			::gpk::SBodyForces								& bodyForces					= Forces[iBody];
			if(0 == memcmp(&newRotation.x, &bodyForces.Rotation.x, sizeof(::gpk::n3<float>)))
				return;
			bodyForces.Rotation							= newRotation;
			::gpk::SBodyFlags								& bodyFlags						= Flags[iBody];
			bodyFlags.Active							= true;
			if(newRotation.z || newRotation.x) 
				bodyFlags.Falling							= true; 
		}

		::gpk::error_t								Save							(::gpk::au8 & output) const { 
			gpk_necs(::gpk::saveView(output, Frames			));
			gpk_necs(::gpk::saveView(output, Flags			));
			gpk_necs(::gpk::saveView(output, Forces			));
			gpk_necs(::gpk::saveView(output, Masses			));
			gpk_necs(::gpk::saveView(output, Centers		));
			gpk_necs(::gpk::saveView(output, TransformsLocal));

			info_printf("Saved %s, %i", "Frames"			, Frames		.size());
			info_printf("Saved %s, %i", "Flags"				, Flags			.size());
			info_printf("Saved %s, %i", "Forces"			, Forces			.size());
			info_printf("Saved %s, %i", "Masses"			, Masses			.size());
			info_printf("Saved %s, %i", "Transforms"		, Centers			.size());
			info_printf("Saved %s, %i", "TransformsLocal"	, TransformsLocal	.size());
			return 0; 
		}
		::gpk::error_t								Load							(::gpk::vcu8 & input) { 
			gpk_necs(::gpk::loadView(input, Frames			));
			gpk_necs(::gpk::loadView(input, Flags			));
			gpk_necs(::gpk::loadView(input, Forces			));
			gpk_necs(::gpk::loadView(input, Masses			));
			gpk_necs(::gpk::loadView(input, Centers			));
			gpk_necs(::gpk::loadView(input, TransformsLocal	));
			return 0;																	  
		}
		inline	::gpk::error_t						Save							(::gpk::apod<int8_t> & output)	const	{ return Save(*((::gpk::apod<uint8_t>*)&output)); }
		inline	::gpk::error_t						Load							(::gpk::vcc & input)					{ return Load(*((::gpk::vcu8*)&input)); }
	};

	int											createOrbiter
		( ::gpk::SRigidBodyIntegrator	& bodies
		, double						mass
		, double						distance
		, double						axialTilt
		, double						rotation_period
		, double						rotation_unit
		, double						orbital_period
		, double						orbital_inclination
		, double						distance_scale
		);
} // namespace

#endif // CED_RIGIDBODY_H_234872398472341
