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
		::gpk::SCoord3<float>							Velocity						= {};
		::gpk::SCoord3<float>							Acceleration					= {};
		::gpk::SCoord3<float>							Rotation						= {};
	};

	// The following structure stores mass information for a 3D rigid body. These almost never change during runtime
	struct SBodyMass {
		float											LinearDamping					= 1.0f;
		float											AngularDamping					= 1.0f;
		float											InverseMass						= 0;
		::gpk::SMatrix3<float>							InverseAngularMassTensor		= {1,0,0,0,1,0,0,0,1};
	};

	struct SBodyCenter {
		::gpk::SCoord3		<float>						Position						= {};
		::gpk::SQuaternion	<float>						Orientation						= {0, 0, 0, 1};
	};

	GDEFINE_ENUM_TYPE(BOUNDING_TYPE, uint8_t);
	GDEFINE_ENUM_VALUE(BOUNDING_TYPE, Sphere			, 0);
	GDEFINE_ENUM_VALUE(BOUNDING_TYPE, AABB				, 1);
	GDEFINE_ENUM_VALUE(BOUNDING_TYPE, Cylinder			, 2);

	GDEFINE_ENUM_VALUE(BOUNDING_TYPE, InwardSphere		, 4);
	GDEFINE_ENUM_VALUE(BOUNDING_TYPE, InwardAABB		, 5);
	GDEFINE_ENUM_VALUE(BOUNDING_TYPE, InwardCylinder	, 6);

	struct SRigidBodyFlags {
		uint8_t											UpdatedTransform				: 1;
		uint8_t											UpdatedTensorWorld				: 1;	// Tell the object that our matrices are up to date
		uint8_t											Active							: 1;
		uint8_t											Collides						: 1;
		uint8_t											Falling							: 1;
		uint8_t											BVType							: 3;
	};

	struct SRigidBodyFrame {
		::gpk::SMatrix3<float>							InverseInertiaTensorWorld		= {1, 0, 0, 0, 1, 0, 0, 0, 1};
		::gpk::SCoord3<float>							LastFrameAcceleration			= {0, 0, 0};
		::gpk::SCoord3<float>							AccumulatedForce				= {0, 0, 0};
		::gpk::SCoord3<float>							AccumulatedTorque				= {0, 0, 0};

		int32_t											ClearAccumulators				()	{ AccumulatedForce = AccumulatedTorque = {}; return 0; }
	};

	struct SBoundingVolume {
		::gpk::SCoord3<float>							HalfSizes						= {.5f, .5f, .5f};
	};
#pragma pack(pop)
	void											updateTransform					(::gpk::SBodyCenter & bodyTransform, ::gpk::SMatrix4<float> & transformLocal);
	int32_t											integrateForces					(double duration, ::gpk::SRigidBodyFrame& bodyFrame, ::gpk::SBodyForces & bodyForce, const ::gpk::SBodyMass & bodyMass);
	int32_t											integratePosition				(double duration, double durationHalfSquared, ::gpk::SRigidBodyFlags& bodyFlags, ::gpk::SBodyCenter & bodyTransform, const ::gpk::SBodyForces & bodyForces);
	void											transformInertiaTensor			(::gpk::SMatrix3<float> & iitWorld, const ::gpk::SMatrix3<float> &iitBody, const ::gpk::SMatrix4<float> &rotmat);


	struct SRigidBodyIntegrator {
		::gpk::apod<::gpk::SRigidBodyFrame	>		BodyFrames						= {};
		::gpk::apod<::gpk::SRigidBodyFlags	>		BodyFlags						= {};
		::gpk::apod<::gpk::SBodyForces		>		Forces							= {};
		::gpk::apod<::gpk::SBodyMass		>		Masses							= {};
		::gpk::apod<::gpk::SBodyCenter		>		Centers							= {};
		::gpk::apod<::gpk::SBoundingVolume	>		BoundingVolumes					= {};
		::gpk::apod<::gpk::SMatrix4<float>	>		TransformsLocal					= {};

		static constexpr const ::gpk::SMatrix4<float>	MatrixIdentity4					= {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
		static constexpr const ::gpk::SMatrix3<float>	MatrixIdentity3					= {1,0,0,0,1,0,0,0,1};

		int32_t 										ZeroForces						()	{
			for(uint32_t iForce = 0; iForce < Forces.size(); ++iForce)
				Forces[iForce] = {};
			return 0;
		}

		int32_t 										Clear							()	{
			::gpk::clear(BodyFrames, BodyFlags, Forces, Masses, Centers, BoundingVolumes, TransformsLocal);
			return 0;
		}

		int32_t 										Clone							(uint32_t iBody)	{
			BodyFrames				.push_back(::gpk::SRigidBodyFrame	{BodyFrames			[iBody]});
			BodyFlags				.push_back(::gpk::SRigidBodyFlags	{BodyFlags			[iBody]});
			Forces					.push_back(::gpk::SBodyForces		{Forces				[iBody]});
			Masses					.push_back(::gpk::SBodyMass			{Masses				[iBody]});
			Centers					.push_back(::gpk::SBodyCenter		{Centers			[iBody]});
			BoundingVolumes			.push_back(::gpk::SBoundingVolume	{BoundingVolumes	[iBody]});
			return TransformsLocal	.push_back(::gpk::SMatrix4<float>	{TransformsLocal	[iBody]});
		}

		int32_t 										Create							()	{
			BodyFrames				.push_back({});
			BodyFlags				.push_back({});
			Forces					.push_back({});
			Masses					.push_back({});
			Centers					.push_back({});
			BoundingVolumes			.push_back({});
			return TransformsLocal	.push_back(MatrixIdentity4);
		}

		int32_t 										Create							(uint32_t bodyCount)	{
			const uint32_t										indexFirstBody					= BodyFrames.size();
			BodyFrames		.resize(indexFirstBody + bodyCount, {});
			BodyFlags		.resize(indexFirstBody + bodyCount, {});
			Forces			.resize(indexFirstBody + bodyCount, {});
			Masses			.resize(indexFirstBody + bodyCount, {});
			Centers			.resize(indexFirstBody + bodyCount, {});
			TransformsLocal	.resize(indexFirstBody + bodyCount, MatrixIdentity4);
			return indexFirstBody;
		}

		int32_t 										GetTransform					(uint32_t iBody, ::gpk::SMatrix4<float>	& transform)	{
			::gpk::SRigidBodyFlags								& bodyFlags						= BodyFlags			[iBody];
			::gpk::SMatrix4<float>								& bodyTransformLocal			= TransformsLocal	[iBody];
			if(false == bodyFlags.UpdatedTransform || false == bodyFlags.UpdatedTensorWorld) {
				if(false == bodyFlags.UpdatedTransform) {
					::gpk::SBodyCenter									& bodyCenter					= Centers		[iBody];
					::gpk::updateTransform(bodyCenter, bodyTransformLocal);
					bodyFlags.UpdatedTransform						= true;
					bodyFlags.UpdatedTensorWorld					= false;
				}
				if(false == bodyFlags.UpdatedTensorWorld) {
					::gpk::SRigidBodyFrame								& bodyFrame						= BodyFrames[iBody];
					::gpk::SBodyMass									& bodyMass						= Masses	[iBody];
					::gpk::transformInertiaTensor(bodyFrame.InverseInertiaTensorWorld, bodyMass.InverseAngularMassTensor, bodyTransformLocal);
					bodyFlags.UpdatedTensorWorld					= true;
				}
			}
			transform										= bodyTransformLocal;
			return 0;
		}

		void											AddForceAtPoint					(uint32_t iBody, const ::gpk::SCoord3<float>& force, ::gpk::SCoord3<float> point)			{
			::gpk::SBodyCenter									& bodyCenter					= Centers[iBody];
			::gpk::SRigidBodyFlags								& bodyFlags						= BodyFlags	[iBody];
			::gpk::SRigidBodyFrame								& bodyFrame						= BodyFrames[iBody];
			point											-= bodyCenter.Position;	// Convert to coordinates relative to center of mass.
			bodyFrame.AccumulatedForce 						+= force;
			bodyFrame.AccumulatedTorque						+= point.Cross( force );
			bodyFlags.Active								= true;
		}

		int32_t											Integrate						(double duration)	{
			const double										durationHalfSquared				= duration * duration * 0.5;
			::gpk::SMatrix4<float>								dummy;
			for(uint32_t iBody = 0; iBody < BodyFlags.size(); ++iBody) {
				::gpk::SRigidBodyFlags								& bodyFlags						= BodyFlags[iBody];
				if(false == bodyFlags.Active)
					continue;

				GetTransform(iBody, dummy); // ensures tensor matrix is up to date

				::gpk::SRigidBodyFrame								& bodyFrame						= BodyFrames[iBody];
				::gpk::SBodyForces									& bodyForces					= Forces	[iBody];
				::gpk::SBodyMass									& bodyMass						= Masses	[iBody];
				::gpk::SBodyCenter									& bodyCenter					= Centers[iBody];
				::gpk::integrateForces	(duration, bodyFrame, bodyForces, bodyMass);
				::gpk::integratePosition(duration, durationHalfSquared, bodyFlags, bodyCenter, bodyForces);
				if(bodyForces.Acceleration.LengthSquared() < .01 && bodyForces.Velocity.LengthSquared() < .01 && bodyForces.Rotation.LengthSquared() < .01) {
					bodyFlags.Active								= false;
					bodyForces										= {};
				}
			}
			return 0;
		}

		bool											Active							(uint32_t iBody)													{ return BodyFlags[iBody].Active; }
		float											GetMass							(uint32_t iBody)													{ return 1.0f / Masses[iBody].InverseMass; }
		void											GetPosition						(uint32_t iBody, ::gpk::n3d<float>& position)						{ position		= Centers[iBody].Position; }
		void											GetOrientation					(uint32_t iBody, ::gpk::quat<float>& orientation)					{ orientation	= Centers[iBody].Orientation; }
		void											GetRotation						(uint32_t iBody, ::gpk::n3d<float>& rotation)						{ rotation		= Forces[iBody].Rotation;		}
		void											GetAcceleration					(uint32_t iBody, ::gpk::n3d<float>& acceleration)					{ acceleration	= Forces[iBody].Acceleration;	}
		void											GetVelocity						(uint32_t iBody, ::gpk::n3d<float>& velocity)						{ velocity		= Forces[iBody].Velocity;		}

		void											SetActive						(uint32_t iBody, bool active)										{ BodyFlags[iBody].Active = active; }
		void											SetMass							(uint32_t iBody, float mass)										{ Masses[iBody].InverseMass = 1.0f / mass; }
		void											SetPosition						(uint32_t iBody, const ::gpk::SCoord3<float>& newPosition )			{
			::gpk::SBodyCenter									& bodyCenter					= Centers[iBody];
			if(0 == memcmp(&newPosition.x, &bodyCenter.Position.x, sizeof(::gpk::SCoord3<float>)))
				return;
			bodyCenter.Position								= newPosition;
			::gpk::SRigidBodyFlags								& bodyFlags						= BodyFlags[iBody];
			bodyFlags.UpdatedTransform						= false;
			bodyFlags.UpdatedTensorWorld					= false;
		}
		void											SetOrientation					(uint32_t iBody, const ::gpk::SQuaternion<float>& newOrientation )	{
			::gpk::SBodyCenter									& bodyCenter					= Centers[iBody];
			if(0 == memcmp(&newOrientation.x, &bodyCenter.Orientation.x, sizeof(::gpk::SQuaternion<float>)))
				return;
			bodyCenter.Orientation							= newOrientation;
			::gpk::SRigidBodyFlags								& bodyFlags						= BodyFlags[iBody];
			bodyFlags.UpdatedTransform						= false;
			bodyFlags.UpdatedTensorWorld					= false;
		}
		void											SetVelocity						(uint32_t iBody, const ::gpk::SCoord3<float>& newVelocity)			{
			::gpk::SBodyForces									& bodyForces					= Forces[iBody];
			if(0 == memcmp(&newVelocity.x, &bodyForces.Velocity.x, sizeof(::gpk::SCoord3<float>)))
				return;
			bodyForces.Velocity								= newVelocity;
			::gpk::SRigidBodyFlags								& bodyFlags						= BodyFlags[iBody];
			bodyFlags.Active								= true;
			if(newVelocity.y) 
				bodyFlags.Falling								= true; 
		}
		void											SetAcceleration					(uint32_t iBody, const ::gpk::SCoord3<float>& acceleration)			{
			::gpk::SBodyForces									& bodyForces					= Forces[iBody];
			if(0 == memcmp(&acceleration.x, &bodyForces.Acceleration.x, sizeof(::gpk::SCoord3<float>)))
				return;
			bodyForces.Acceleration							= acceleration;
			::gpk::SRigidBodyFlags								& bodyFlags						= BodyFlags[iBody];
			bodyFlags.Active								= true;
		}
		void											SetRotation						(uint32_t iBody, const ::gpk::SCoord3<float>& newRotation)			{
			::gpk::SBodyForces									& bodyForces					= Forces[iBody];
			if(0 == memcmp(&newRotation.x, &bodyForces.Rotation.x, sizeof(::gpk::SCoord3<float>)))
				return;
			bodyForces.Rotation								= newRotation;
			::gpk::SRigidBodyFlags								& bodyFlags						= BodyFlags[iBody];
			bodyFlags.Active								= true;
			if(newRotation.z || newRotation.x) 
				bodyFlags.Falling								= true; 
		}

		::gpk::error_t									Save(::gpk::apod<byte_t> & output) const { 
			gpk_necs(::gpk::viewWrite(BodyFrames		, output));
			gpk_necs(::gpk::viewWrite(BodyFlags			, output));
			gpk_necs(::gpk::viewWrite(Forces			, output));
			gpk_necs(::gpk::viewWrite(Masses			, output));
			gpk_necs(::gpk::viewWrite(Centers			, output));
			gpk_necs(::gpk::viewWrite(TransformsLocal	, output));

			info_printf("Saved %s, %i", "BodyFrames"		, BodyFrames		.size());
			info_printf("Saved %s, %i", "BodyFlags"			, BodyFlags			.size());
			info_printf("Saved %s, %i", "Forces"			, Forces			.size());
			info_printf("Saved %s, %i", "Masses"			, Masses			.size());
			info_printf("Saved %s, %i", "Transforms"		, Centers			.size());
			info_printf("Saved %s, %i", "TransformsLocal"	, TransformsLocal	.size());
			return 0; 
		}
		::gpk::error_t									Load(::gpk::view_array<const byte_t> & input) { 
			gpk_necs(::gpk::loadView(input, BodyFrames		));
			gpk_necs(::gpk::loadView(input, BodyFlags		));
			gpk_necs(::gpk::loadView(input, Forces			));
			gpk_necs(::gpk::loadView(input, Masses			));
			gpk_necs(::gpk::loadView(input, Centers		));
			gpk_necs(::gpk::loadView(input, TransformsLocal	));
			return 0;																	  
		}
	};

	int												createOrbiter
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
