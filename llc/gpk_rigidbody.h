#include "gpk_matrix.h"
#include "gpk_array.h"

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

	struct SRigidBodyFlags {
		bool											OutdatedTransform				: 1;
		bool											OutdatedTensorWorld				: 1;	// Tell the object that our matrices are up to date
		bool											Active							: 1;

		constexpr										SRigidBodyFlags					()
			: OutdatedTransform		(true)
			, OutdatedTensorWorld	(true)
			, Active				(true)
		{}
	};

	struct SRigidBodyFrame {
		::gpk::SMatrix3<float>							InverseInertiaTensorWorld		= {1, 0, 0, 0, 1, 0, 0, 0, 1};
		::gpk::SCoord3<float>							LastFrameAcceleration			= {0, 0, 0};
		::gpk::SCoord3<float>							AccumulatedForce				= {0, 0, 0};
		::gpk::SCoord3<float>							AccumulatedTorque				= {0, 0, 0};

		int32_t											ClearAccumulators				()	{ AccumulatedForce = AccumulatedTorque = {}; return 0; }
	};
#pragma pack(pop)
	void											updateTransform					(::gpk::SBodyCenter & bodyTransform, ::gpk::SMatrix4<float> & transformLocal);
	int32_t											integrateForces					(double duration, ::gpk::SRigidBodyFrame& bodyFrame, ::gpk::SBodyForces & bodyForce, const ::gpk::SBodyMass & bodyMass);
	int32_t											integratePosition				(double duration, double durationHalfSquared, ::gpk::SRigidBodyFlags& bodyFlags, ::gpk::SBodyCenter & bodyTransform, const ::gpk::SBodyForces & bodyForces);
	void											transformInertiaTensor			(::gpk::SMatrix3<float> & iitWorld, const ::gpk::SMatrix3<float> &iitBody, const ::gpk::SMatrix4<float> &rotmat);

	struct SRigidBodyIntegrator {
		::gpk::array_pod<::gpk::SRigidBodyFrame	>		BodyFrames						= {};
		::gpk::array_pod<::gpk::SRigidBodyFlags	>		BodyFlags						= {};
		::gpk::array_pod<::gpk::SBodyForces		>		Forces							= {};
		::gpk::array_pod<::gpk::SBodyMass		>		Masses							= {};
		::gpk::array_pod<::gpk::SBodyCenter		>		Centers							= {};
		::gpk::array_pod<::gpk::SMatrix4<float>	>		TransformsLocal					= {};

		static constexpr const ::gpk::SMatrix4<float>	MatrixIdentity4					= {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
		static constexpr const ::gpk::SMatrix3<float>	MatrixIdentity3					= {1,0,0,0,1,0,0,0,1};

		int32_t 										ZeroForces						()	{
			for(uint32_t iForce = 0; iForce < Forces.size(); ++iForce)
				Forces[iForce] = {};
			return 0;
		}

		int32_t 										Clear							()	{
			BodyFrames				.clear();
			BodyFlags				.clear();
			Forces					.clear();
			Masses					.clear();
			Centers					.clear();
			return TransformsLocal	.clear();
		}

		int32_t 										Clone							(uint32_t iBody)	{
			BodyFrames				.push_back(::gpk::SRigidBodyFrame	{BodyFrames			[iBody]});
			BodyFlags				.push_back(::gpk::SRigidBodyFlags	{BodyFlags			[iBody]});
			Forces					.push_back(::gpk::SBodyForces		{Forces				[iBody]});
			Masses					.push_back(::gpk::SBodyMass			{Masses				[iBody]});
			Centers					.push_back(::gpk::SBodyCenter		{Centers			[iBody]});
			return TransformsLocal	.push_back(::gpk::SMatrix4<float>	{TransformsLocal	[iBody]});
		}

		int32_t 										Create							()	{
			BodyFrames				.push_back({});
			BodyFlags				.push_back({});
			Forces					.push_back({});
			Masses					.push_back({});
			Centers				.push_back({});
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
			if(bodyFlags.OutdatedTransform || bodyFlags.OutdatedTensorWorld) {
				if(bodyFlags.OutdatedTransform) {
					::gpk::SBodyCenter									& bodyCenter					= Centers		[iBody];
					::gpk::updateTransform(bodyCenter, bodyTransformLocal);
					bodyFlags.OutdatedTransform						= false;
				}
				if(bodyFlags.OutdatedTensorWorld) {
					::gpk::SRigidBodyFrame								& bodyFrame						= BodyFrames[iBody];
					::gpk::SBodyMass									& bodyMass						= Masses	[iBody];
					::gpk::transformInertiaTensor(bodyFrame.InverseInertiaTensorWorld, bodyMass.InverseAngularMassTensor, bodyTransformLocal);
					bodyFlags.OutdatedTensorWorld					= false;
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
				if(bodyForces.Acceleration.LengthSquared() < .01 && bodyForces.Velocity.LengthSquared() < .01 && bodyForces.Rotation.LengthSquared() < .01)
					bodyFlags.Active								= false;
			}
			return 0;
		}
		void											SetPosition						(uint32_t iBody, const ::gpk::SCoord3<float>& newPosition )			{
			::gpk::SBodyCenter									& bodyCenter					= Centers[iBody];
			if( 0 == memcmp( &newPosition.x, &bodyCenter.Position.x, sizeof( ::gpk::SCoord3<float> ) ) )
				return;
			bodyCenter.Position								= newPosition;
			::gpk::SRigidBodyFlags								& bodyFlags						= BodyFlags[iBody];
			bodyFlags.OutdatedTransform						=
			bodyFlags.OutdatedTensorWorld					= true;
		}
		void											SetOrientation					(uint32_t iBody, const ::gpk::SQuaternion<float>& newOrientation )	{
			::gpk::SBodyCenter									& bodyCenter					= Centers[iBody];
			if( 0 == memcmp( &newOrientation.x, &bodyCenter.Orientation.x, sizeof(::gpk::SQuaternion<float>) ) )
				return;
			bodyCenter.Orientation							= newOrientation;
			::gpk::SRigidBodyFlags								& bodyFlags						= BodyFlags[iBody];
			bodyFlags.OutdatedTransform						=
			bodyFlags.OutdatedTensorWorld					= true;
		}
		void											SetVelocity						(uint32_t iBody, const ::gpk::SCoord3<float>& newVelocity)			{
			::gpk::SBodyForces									& bodyCenter					= Forces[iBody];
			if( 0 == memcmp( &newVelocity.x, &bodyCenter.Velocity.x, sizeof( ::gpk::SCoord3<float> ) ) )
				return;
			bodyCenter.Velocity								= newVelocity;
			::gpk::SRigidBodyFlags								& bodyFlags						= BodyFlags[iBody];
			bodyFlags.Active								= true;
		}

		void											SetRotation						(uint32_t iBody, const ::gpk::SCoord3<float>& newRotation)			{
			::gpk::SBodyForces									& bodyCenter					= Forces[iBody];
			if( 0 == memcmp( &newRotation.x, &bodyCenter.Rotation.x, sizeof( ::gpk::SCoord3<float> ) ) )
				return;
			bodyCenter.Rotation								= newRotation;
			::gpk::SRigidBodyFlags								& bodyFlags						= BodyFlags[iBody];
			bodyFlags.Active								= true;
		}

		::gpk::error_t									Save(::gpk::array_pod<byte_t> & output) const { 
			gpk_necs(::gpk::viewWrite(BodyFrames		, output));
			gpk_necs(::gpk::viewWrite(BodyFlags			, output));
			gpk_necs(::gpk::viewWrite(Forces			, output));
			gpk_necs(::gpk::viewWrite(Masses			, output));
			gpk_necs(::gpk::viewWrite(Centers		, output));
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
