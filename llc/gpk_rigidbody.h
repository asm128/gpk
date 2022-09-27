#include "gpk_matrix.h"
#include "gpk_array.h"

#ifndef GPK_RIGIDBODY_H_234872398472341
#define GPK_RIGIDBODY_H_234872398472341

namespace gpk
{
#pragma pack(push, 1)
	// The following structure stores forces information for a 3D rigid body. These change almost every frame during runtime
	struct SForce3 {
		::gpk::SCoord3<float>							Velocity						= {};
		::gpk::SCoord3<float>							Acceleration					= {};
		::gpk::SCoord3<float>							Rotation						= {};
	};

	// The following structure stores mass information for a 3D rigid body. These almost never change during runtime
	struct SMass3 {
		float											LinearDamping					= 1.0f;
		float											AngularDamping					= 1.0f;
		float											InverseMass						= 0;
		::gpk::SMatrix3<float>							InverseAngularMassTensor		= {1,0,0,0,1,0,0,0,1};
	};

	struct STransform3 {
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
	void											updateTransform					(::gpk::STransform3 & bodyTransform, ::gpk::SMatrix4<float> & transformLocal);
	int32_t											integrateForces					(double duration, ::gpk::SRigidBodyFrame& bodyFrame, ::gpk::SForce3 & bodyForce, const ::gpk::SMass3 & bodyMass);
	int32_t											integratePosition				(double duration, double durationHalfSquared, ::gpk::SRigidBodyFlags& bodyFlags, ::gpk::STransform3 & bodyTransform, const ::gpk::SForce3 & bodyForces);
	void											transformInertiaTensor			(::gpk::SMatrix3<float> & iitWorld, const ::gpk::SMatrix3<float> &iitBody, const ::gpk::SMatrix4<float> &rotmat);

	struct SIntegrator3 {
		::gpk::array_pod<::gpk::SRigidBodyFrame	>		BodyFrames						= {};
		::gpk::array_pod<::gpk::SRigidBodyFlags	>		BodyFlags						= {};
		::gpk::array_pod<::gpk::SForce3			>		Forces							= {};
		::gpk::array_pod<::gpk::SMass3			>		Masses							= {};
		::gpk::array_pod<::gpk::STransform3		>		Transforms						= {};
		::gpk::array_pod<::gpk::SMatrix4<float>	>		TransformsLocal					= {};

		static constexpr const ::gpk::SMatrix4<float>	MatrixIdentity4					= {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
		static constexpr const ::gpk::SMatrix3<float>	MatrixIdentity3					= {1,0,0,0,1,0,0,0,1};

		int32_t 										Clear							()	{
			BodyFrames				.clear();
			BodyFlags				.clear();
			Forces					.clear();
			Masses					.clear();
			Transforms				.clear();
			return TransformsLocal	.clear();
		}
		int32_t 										Create							()	{
			BodyFrames				.push_back({});
			BodyFlags				.push_back({});
			Forces					.push_back({});
			Masses					.push_back({});
			Transforms				.push_back({});
			return TransformsLocal	.push_back(MatrixIdentity4);
		}

		int32_t 										Create							(uint32_t bodyCount)	{
			const uint32_t										indexFirstBody					= BodyFrames.size();
			BodyFrames		.resize(indexFirstBody + bodyCount, {});
			BodyFlags		.resize(indexFirstBody + bodyCount, {});
			Forces			.resize(indexFirstBody + bodyCount, {});
			Masses			.resize(indexFirstBody + bodyCount, {});
			Transforms		.resize(indexFirstBody + bodyCount, {});
			TransformsLocal	.resize(indexFirstBody + bodyCount, MatrixIdentity4);
			return indexFirstBody;
		}

		int32_t 										GetTransform					(uint32_t iBody, ::gpk::SMatrix4<float>	& transform)	{
			::gpk::SRigidBodyFlags								& bodyFlags						= BodyFlags			[iBody];
			::gpk::SMatrix4<float>								& bodyTransformLocal			= TransformsLocal	[iBody];
			const bool											requiresUpdate					= bodyFlags.OutdatedTransform || bodyFlags.OutdatedTensorWorld;
			if(requiresUpdate) {
				if(bodyFlags.OutdatedTransform) {
					::gpk::STransform3									& bodyTransform					= Transforms[iBody];
					::gpk::updateTransform(bodyTransform, bodyTransformLocal);
					bodyFlags.OutdatedTransform						= false;
				}
				if(bodyFlags.OutdatedTensorWorld) {
					::gpk::SRigidBodyFrame								& bodyFrame						= BodyFrames[iBody];
					::gpk::SMass3										& bodyMass						= Masses	[iBody];
					::gpk::transformInertiaTensor(bodyFrame.InverseInertiaTensorWorld, bodyMass.InverseAngularMassTensor, bodyTransformLocal);
					bodyFlags.OutdatedTensorWorld					= false;
				}
			}
			transform										= bodyTransformLocal;
			return requiresUpdate ? 1 : 0;
		}
		void											AddForceAtPoint					(uint32_t iBody, const ::gpk::SCoord3<float>& force, ::gpk::SCoord3<float> point)			{
			::gpk::STransform3									& bodyTransform					= Transforms[iBody];
			::gpk::SRigidBodyFlags								& bodyFlags						= BodyFlags	[iBody];
			::gpk::SRigidBodyFrame								& bodyFrame						= BodyFrames[iBody];
			point											-= bodyTransform.Position;	// Convert to coordinates relative to center of mass.
			bodyFrame.AccumulatedForce 						+= force;
			bodyFrame.AccumulatedTorque						+= point.Cross( force );
			bodyFlags.Active								= true;
		}
		int32_t											Integrate						(double duration)	{
			const double										durationHalfSquared				= duration * duration * 0.5;
			for(uint32_t iBody = 0; iBody < BodyFlags.size(); ++iBody) {
				::gpk::SRigidBodyFlags								& bodyFlags						= BodyFlags[iBody];
				if(false == bodyFlags.Active)
					continue;
				::gpk::SRigidBodyFrame								& bodyFrame						= BodyFrames[iBody];
				::gpk::SForce3										& bodyForces					= Forces	[iBody];
				::gpk::SMass3										& bodyMass						= Masses	[iBody];
				::gpk::STransform3									& bodyTransform					= Transforms[iBody];
				::gpk::integrateForces	(duration, bodyFrame, bodyForces, bodyMass);
				::gpk::integratePosition(duration, durationHalfSquared, bodyFlags, bodyTransform, bodyForces);
			}
			return 0;
		}
		void											SetPosition						(uint32_t iBody, const ::gpk::SCoord3<float>& newPosition )			{
			::gpk::STransform3									& bodyTransform					= Transforms[iBody];
			if( 0 == memcmp( &newPosition.x, &bodyTransform.Position.x, sizeof( ::gpk::SCoord3<float> ) ) )
				return;
			bodyTransform.Position							= newPosition;
			::gpk::SRigidBodyFlags								& bodyFlags						= BodyFlags[iBody];
			bodyFlags.OutdatedTransform						=
			bodyFlags.OutdatedTensorWorld					= true;
		}
		void											SetOrientation					(uint32_t iBody, const ::gpk::SQuaternion<float>& newOrientation )	{
			::gpk::STransform3									& bodyTransform					= Transforms[iBody];
			if( 0 == memcmp( &newOrientation.x, &bodyTransform.Orientation.x, sizeof(::gpk::SQuaternion<float>) ) )
				return;
			bodyTransform.Orientation						= newOrientation;
			::gpk::SRigidBodyFlags								& bodyFlags						= BodyFlags[iBody];
			bodyFlags.OutdatedTransform						=
			bodyFlags.OutdatedTensorWorld					= true;
		}

		::gpk::error_t										Save(::gpk::array_pod<byte_t> & output) const { 
			gpk_necs(::gpk::viewWrite(BodyFrames		, output));
			gpk_necs(::gpk::viewWrite(BodyFlags			, output));
			gpk_necs(::gpk::viewWrite(Forces			, output));
			gpk_necs(::gpk::viewWrite(Masses			, output));
			gpk_necs(::gpk::viewWrite(Transforms		, output));
			gpk_necs(::gpk::viewWrite(TransformsLocal	, output));

			info_printf("Saved %s, %i", "BodyFrames"		, BodyFrames		.size());
			info_printf("Saved %s, %i", "BodyFlags"			, BodyFlags			.size());
			info_printf("Saved %s, %i", "Forces"			, Forces			.size());
			info_printf("Saved %s, %i", "Masses"			, Masses			.size());
			info_printf("Saved %s, %i", "Transforms"		, Transforms		.size());
			info_printf("Saved %s, %i", "TransformsLocal"	, TransformsLocal	.size());
			return 0; 
		}
		::gpk::error_t										Load(::gpk::view_array<const byte_t> & input) { 
			uint32_t												bytesRead				= 0;
			::gpk::view_array<const ::gpk::SRigidBodyFrame	> readBodyFrames		= {}; gpk_necs(bytesRead = ::gpk::viewRead(readBodyFrames		, input)); input = {input.begin() + bytesRead, input.size() - bytesRead}; BodyFrames		= readBodyFrames		;
			::gpk::view_array<const ::gpk::SRigidBodyFlags	> readBodyFlags			= {}; gpk_necs(bytesRead = ::gpk::viewRead(readBodyFlags		, input)); input = {input.begin() + bytesRead, input.size() - bytesRead}; BodyFlags			= readBodyFlags			;
			::gpk::view_array<const ::gpk::SForce3			> readForces			= {}; gpk_necs(bytesRead = ::gpk::viewRead(readForces			, input)); input = {input.begin() + bytesRead, input.size() - bytesRead}; Forces			= readForces			;
			::gpk::view_array<const ::gpk::SMass3			> readMasses			= {}; gpk_necs(bytesRead = ::gpk::viewRead(readMasses			, input)); input = {input.begin() + bytesRead, input.size() - bytesRead}; Masses			= readMasses			;
			::gpk::view_array<const ::gpk::STransform3		> readTransforms		= {}; gpk_necs(bytesRead = ::gpk::viewRead(readTransforms		, input)); input = {input.begin() + bytesRead, input.size() - bytesRead}; Transforms		= readTransforms		;
			::gpk::view_array<const ::gpk::SMatrix4<float>	> readTransformsLocal	= {}; gpk_necs(bytesRead = ::gpk::viewRead(readTransformsLocal	, input)); input = {input.begin() + bytesRead, input.size() - bytesRead}; TransformsLocal	= readTransformsLocal	;
			return 0;																	  
		}
	};

	int												createOrbiter
		( ::gpk::SIntegrator3	& bodies
		, double				mass
		, double				distance
		, double				axialTilt
		, double				rotation_period
		, double				rotation_unit
		, double				orbital_period
		, double				orbital_inclination
		, double				distance_scale
		);
} // namespace

#endif // CED_RIGIDBODY_H_234872398472341
