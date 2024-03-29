#include "gpk_engine_entity.h"
#include "gpk_engine_scene.h"
#include "gpk_rigid_body.h"
#include "gpk_linear_map_pod.h"
#include "gpk_geometry.h"
#include "gpk_png.h"

#ifndef GPK_ENGINE_H_23627
#define GPK_ENGINE_H_23627

namespace gpk 
{
	::gpk::error_t					updateEntityTransforms			
		( uint32_t								iEntity
		, const ::gpk::SEntity			& entity
		, const ::gpk::SVirtualEntityManager	& managedEntities	
		, ::gpk::SRigidBodyIntegrator			& integrator		
		, ::gpk::SRenderNodeManager				& renderNodes
		);

	struct SEngine {
		::gpk::pobj<::gpk::SEngineScene>	Scene			;
		::gpk::SVirtualEntityManager		Entities		;
		::gpk::SRigidBodyIntegrator			Integrator		;

		::gpk::lmpod<SParamsBox			, uint32_t>	ParamsBox			;
		::gpk::lmpod<SParamsDisc		, uint32_t>	ParamsDisc			;
		::gpk::lmpod<SParamsCircle		, uint32_t>	ParamsCircleSide	;
		::gpk::lmpod<SParamsCylinderWall, uint32_t>	ParamsCylinderWall	;
		::gpk::lmpod<SParamsCylinderWall, uint32_t>	ParamsCylinder		;
		::gpk::lmpod<SParamsHelix		, uint32_t>	ParamsFigure0		;
		::gpk::lmpod<SParamsGrid		, uint32_t>	ParamsGrid			;
		::gpk::lmpod<SParamsHelix		, uint32_t>	ParamsHelixHalf		;
		::gpk::lmpod<SParamsHelix		, uint32_t>	ParamsHelix			;
		::gpk::lmpod<SParamsSphere		, uint32_t>	ParamsSphere		;
		::gpk::lmpod<SParamsTube		, uint32_t>	ParamsTube			;
		::gpk::lmpod<SParamsRingSide	, uint32_t>	ParamsRingSide		;
		::gpk::lmpod<SParamsRingSide	, uint32_t>	ParamsRingFlat		;

		::gpk::lmpod<::gpk::vcc, uint32_t>	Images			= {};

		::gpk::SPNGData						PNGCache		= {};

		inline	::gpk::error_t		GetRigidBody		(::gpk::eid_t idEntity)					const	{ return Entities[idEntity].RigidBody; }
		inline	::gpk::error_t		GetRenderNode		(::gpk::eid_t idEntity)					const	{ return Entities[idEntity].RenderNode; }
		inline	::gpk::error_t		GetParentIndex		(::gpk::eid_t idEntity)					const	{ return Entities[idEntity].Parent; }

		inline	::gpk::error_t		GetChildCount		(::gpk::eid_t idEntity)					const	{ const ::gpk::papod<::gpk::eid_t> & children = Entities.Children[idEntity]; return children ? children->size() : 0; }
		inline	::gpk::vceid		GetChildren			(::gpk::eid_t idEntity)					const	{ const ::gpk::papod<::gpk::eid_t> & children = Entities.Children[idEntity]; return children ? ::gpk::vceid{*children} : ::gpk::vceid{}; }
		inline	::gpk::veid			GetChildren			(::gpk::eid_t idEntity)							{ ::gpk::papod<::gpk::eid_t> & children = Entities.Children[idEntity]; return children ? *children : ::gpk::veid{}; }

		inline	const SBodyFlags&	GetBodyFlags		(::gpk::eid_t idEntity)					const	{ return Integrator.GetBodyFlags (Entities[idEntity].RigidBody); }
		inline	const SBodyFrame&	GetBodyFrame		(::gpk::eid_t idEntity)					const	{ return Integrator.GetBodyFrame (Entities[idEntity].RigidBody); }
		inline	const SBodyForces&	GetBodyForces		(::gpk::eid_t idEntity)					const	{ return Integrator.GetBodyForces(Entities[idEntity].RigidBody); }
		inline	const SBodyMass&	GetBodyMass			(::gpk::eid_t idEntity)					const	{ return Integrator.GetBodyMass  (Entities[idEntity].RigidBody); }
		inline	const SBodyCenter&	GetBodyCenter		(::gpk::eid_t idEntity)					const	{ return Integrator.GetBodyCenter(Entities[idEntity].RigidBody); }
		inline	SBodyFlags&			GetBodyFlags		(::gpk::eid_t idEntity)							{ return Integrator.GetBodyFlags (Entities[idEntity].RigidBody); }
		inline	SBodyFrame&			GetBodyFrame		(::gpk::eid_t idEntity)							{ return Integrator.GetBodyFrame (Entities[idEntity].RigidBody); }
		inline	SBodyForces&		GetBodyForces		(::gpk::eid_t idEntity)							{ return Integrator.GetBodyForces(Entities[idEntity].RigidBody); }
		inline	SBodyMass&			GetBodyMass			(::gpk::eid_t idEntity)							{ return Integrator.GetBodyMass  (Entities[idEntity].RigidBody); }
		inline	SBodyCenter&		GetBodyCenter		(::gpk::eid_t idEntity)							{ return Integrator.GetBodyCenter(Entities[idEntity].RigidBody); }

		inline	::gpk::error_t		SetParent			(::gpk::eid_t idEntity, ::gpk::eid_t idParent)	{ return Entities.SetParent(idEntity, idParent); }

		::gpk::error_t				Load				(::gpk::vcu8 & input)			{
			gpk_necs(Scene		->Load(input));
			gpk_necs(Entities	.Load(input));
			gpk_necs(Integrator	.Load(input));
			return 0;
		}

		::gpk::error_t				Save				(::gpk::au8 & output)	const	{
			if(Scene)
				gpk_necs(Scene->Save(output));
			else 
				gpk_necs(gpk::SEngineScene{}.Save(output));

			gpk_necs(Entities	.Save(output));
			gpk_necs(Integrator	.Save(output));
			return 0;
		}

		::gpk::error_t				Update				(double secondsLastFrame)		{
			Integrator.Integrate(secondsLastFrame);
			for(uint32_t iEntity = 0; iEntity < Entities.size(); ++iEntity) {
				::gpk::SEntity			& entity			= Entities[iEntity];
				if(entity.Parent != -1)
					continue;

				e_if(errored(::gpk::updateEntityTransforms(iEntity, entity, Entities, Integrator, Scene->RenderNodes)));
			}
			return 0;
		}

		// This function messes up with the Entities array so be careful of not having any references to its contents or it may end up trying to access invalid memory
		::gpk::error_t				Clone				(uint32_t iEntitySource, bool cloneSkin, bool cloneSurfaces, bool cloneShaders) {
			const ::gpk::SEntity		entitySource		= Entities[iEntitySource];
			int32_t							iEntityNew			= Entities.Create();
			::gpk::SEntity			& entityNew			= Entities[iEntityNew];
			entityNew.RenderNode		= Scene->Clone(entitySource.RenderNode, cloneSkin, cloneSurfaces, cloneShaders);
			entityNew.RigidBody			= ((uint32_t)entitySource.RigidBody < Integrator.Flags.size()) ? Integrator.Clone(entitySource.RigidBody) : (uint32_t)-1;
			entityNew.Parent			= entitySource.Parent;

			const ::gpk::pau32				childrenSource		= Entities.Children[iEntitySource];
			if(childrenSource && childrenSource->size()) {
				::gpk::pau32					childrenNew			= Entities.Children[iEntityNew];
				for(uint32_t iChild = 0; iChild < childrenSource->size(); ++iChild) {
					uint32_t						entityChild			= Clone((*childrenSource)[iChild], cloneSkin, cloneSurfaces, cloneShaders);
					Entities[entityChild].Parent= iEntityNew;
					childrenNew->push_back(entityChild);
				}
				Entities.Children[iEntityNew]	= childrenNew;
			}
			return iEntityNew;
		}

		inline	::gpk::error_t		SetNodeSize			(uint32_t iEntity, const ::gpk::n3f32 & size)					{ Scene->RenderNodes.BaseTransforms[GetRenderNode(iEntity)].NodeSize = size; return 0; }
		inline	::gpk::error_t		SetMeshScale		(uint32_t iEntity, const ::gpk::n3f32 & scale, bool halfSizes)	{ Scene->RenderNodes.BaseTransforms[GetRenderNode(iEntity)].Model.Scale(scale, true); 
			if(halfSizes) {
				gpk_necs(SetHalfSizes(iEntity, scale * .5f));
				gpk_necs(SetNodeSize(iEntity, scale));
			}
			return 0; 
		}
		inline	::gpk::error_t		SetMeshPosition		(uint32_t iEntity, const ::gpk::n3f32 & position)				{ Scene->RenderNodes.BaseTransforms[GetRenderNode(iEntity)].Model.SetTranslation(position, false); return 0; }
		::gpk::error_t				SetColorDiffuse		(uint32_t iEntity, const ::gpk::rgbaf & diffuse)				{ Scene->Graphics->Skins[Scene->RenderNodes[GetRenderNode(iEntity)].Skin]->Material.Color.Diffuse = diffuse; return 0; }
		::gpk::error_t				SetShader			(uint32_t iEntity, const ::std::function<::gpk::TFuncPixelShader> & shader, ::gpk::vcs name) {
			const uint32_t					iShader				= Scene->RenderNodes[GetRenderNode(iEntity)].Shader;
			Scene->Graphics->Shaders[iShader].create(shader);
			Scene->Graphics->Shaders.Names[iShader] = name;
			return 0;
		}
		inline	::gpk::error_t		IsPhysicsActive		(uint32_t iEntity)										const	{ return Integrator.Active	(Entities[iEntity].RigidBody) ? 1 : 0; }
		inline	float				GetMass				(uint32_t iEntity)										const	{ return Integrator.GetMass	(Entities[iEntity].RigidBody);	}

		inline	::gpk::error_t		InvalidateTransform	(uint32_t iEntity)												{ Integrator.Flags[GetRigidBody(iEntity)].UpdatedTransform = false;		return 0; }

		inline	::gpk::error_t		GetPosition			(uint32_t iEntity, ::gpk::n3f32 & position)				const	{ Integrator.GetPosition	(GetRigidBody(iEntity), position);			return 0; }
		inline	::gpk::error_t		GetVelocity			(uint32_t iEntity, ::gpk::n3f32 & velocity)				const	{ Integrator.GetVelocity	(GetRigidBody(iEntity), velocity);			return 0; }
		inline	::gpk::error_t		GetAcceleration		(uint32_t iEntity, ::gpk::n3f32 & acceleration)			const	{ Integrator.GetAcceleration(GetRigidBody(iEntity), acceleration);		return 0; }
		inline	::gpk::error_t		GetRotation			(uint32_t iEntity, ::gpk::n3f32 & velocity)				const	{ Integrator.GetRotation	(GetRigidBody(iEntity), velocity);			return 0; }
		inline	::gpk::error_t		GetOrientation		(uint32_t iEntity, ::gpk::quatf32 & orientation)		const	{ Integrator.GetOrientation	(GetRigidBody(iEntity), orientation);		return 0; }
		inline	::gpk::error_t		SetPhysicsActive	(uint32_t iEntity, bool active)									{ Integrator.SetActive		(GetRigidBody(iEntity), active);			return 0; }
		inline	::gpk::error_t		SetCollides			(uint32_t iEntity, bool collides)								{ Integrator.SetCollides	(GetRigidBody(iEntity), collides);			return 0; }
		inline	::gpk::error_t		SetHalfSizes		(uint32_t iEntity, const ::gpk::n3f32 & halfSizes)				{ Integrator.SetHalfSizes	(GetRigidBody(iEntity), halfSizes);			return 0; }
		inline	::gpk::error_t		SetMass				(uint32_t iEntity, float mass)									{ Integrator.SetMass		(GetRigidBody(iEntity), mass);				return 0; }
		inline	::gpk::error_t		SetMassInverse		(uint32_t iEntity, float inverseMass)							{ Integrator.SetMassInverse	(GetRigidBody(iEntity), inverseMass);		return 0; }
		inline	::gpk::error_t		SetPosition			(uint32_t iEntity, const ::gpk::n3f32 & position)				{ Integrator.SetPosition	(GetRigidBody(iEntity), position);			return 0; }
		inline	::gpk::error_t		SetVelocity			(uint32_t iEntity, const ::gpk::n3f32 & velocity)				{ Integrator.SetVelocity	(GetRigidBody(iEntity), velocity);			return 0; }
		inline	::gpk::error_t		SetAcceleration		(uint32_t iEntity, const ::gpk::n3f32 & acceleration)			{ Integrator.SetAcceleration(GetRigidBody(iEntity), acceleration);		return 0; }
		inline	::gpk::error_t		SetRotation			(uint32_t iEntity, const ::gpk::n3f32 & velocity)				{ Integrator.SetRotation	(GetRigidBody(iEntity), velocity);			return 0; }
		inline	::gpk::error_t		SetOrientation		(uint32_t iEntity, const ::gpk::quatf32 & orientation)			{ Integrator.SetOrientation	(GetRigidBody(iEntity), orientation);		return 0; }
		inline	::gpk::error_t		SetDampingLinear	(uint32_t iEntity, float damping)								{ Integrator.Masses[GetRigidBody(iEntity)].LinearDamping	= damping;	return 0; }
		inline	::gpk::error_t		SetDampingAngular	(uint32_t iEntity, float damping)								{ Integrator.Masses[GetRigidBody(iEntity)].AngularDamping	= damping;	return 0; }
		::gpk::error_t				SetHidden			(uint32_t iEntity, bool hidden)									{ Scene->RenderNodes.Flags[GetRenderNode(iEntity)].NoDraw	= hidden;	return 0; }	

		::gpk::error_t				ToggleHidden		(uint32_t iEntity) {
			::gpk::SRenderNodeFlags			& flags				= Scene->RenderNodes.Flags[GetRenderNode(iEntity)];
			flags.NoDraw				= !flags.NoDraw;
			return 0;
		}

		::gpk::error_t				CreateLight			(::gpk::LIGHT_TYPE type);
		::gpk::error_t				CreateCamera		();


		::gpk::error_t				CreateBox			(const ::gpk::SParamsBox			& params, ::gpk::vcs entityName = ::gpk::vcc{ 3, "Box"});
		::gpk::error_t				CreateSphere		(const ::gpk::SParamsSphere			& params, ::gpk::vcs entityName = ::gpk::vcc{ 6, "Sphere"});
		::gpk::error_t				CreateCylinderWall	(const ::gpk::SParamsCylinderWall	& params, ::gpk::vcs entityName = ::gpk::vcc{13, "Cylinder Wall"});
		::gpk::error_t				CreateCylinder		(const ::gpk::SParamsCylinderWall	& params, ::gpk::vcs entityName = ::gpk::vcc{ 8, "Cylinder"});		
		::gpk::error_t				CreateCircleSide	(const ::gpk::SParamsCircle			& params, ::gpk::vcs entityName = ::gpk::vcc{11, "Circle Side"});
		::gpk::error_t				CreateDisc			(const ::gpk::SParamsDisc			& params, ::gpk::vcs entityName = ::gpk::vcc{ 4, "Disc"});
		::gpk::error_t				CreateTube			(const ::gpk::SParamsTube			& params, ::gpk::vcs entityName = ::gpk::vcc{ 4, "Tube"});
		::gpk::error_t				CreateRingSide		(const ::gpk::SParamsRingSide		& params, ::gpk::vcs entityName = ::gpk::vcc{ 9, "Ring Side"});
		::gpk::error_t				CreateRingFlat		(const ::gpk::SParamsRingSide		& params, ::gpk::vcs entityName = ::gpk::vcc{ 9, "Ring flat"});
		::gpk::error_t				CreateGrid			(const ::gpk::SParamsGrid			& params, ::gpk::vcs entityName = ::gpk::vcc{ 4, "Grid"});
		::gpk::error_t				CreateHelixHalf		(const ::gpk::SParamsHelix			& params, ::gpk::vcs entityName = ::gpk::vcc{10, "Helix half"});
		::gpk::error_t				CreateHelix			(const ::gpk::SParamsHelix			& params, ::gpk::vcs entityName = ::gpk::vcc{ 5, "Helix"});
		::gpk::error_t				CreateFigure0		(const ::gpk::SParamsHelix			& params, ::gpk::vcs entityName = ::gpk::vcs{"Figure0"});

		::gpk::error_t				CreateImageFromFile	(const ::gpk::vcs & fileFolder, const ::gpk::vcs & fileName);

		typedef std::function<::gpk::error_t(::gpk::SGeometryBuffers&)> TGeometryFunc;

		tplt<tpnm _tParams>
		::gpk::error_t				CreateEntityFromGeometry
			( const ::gpk::vcc							name		
			, const ::gpk::n3f32						halfSizes
			, bool										createSkin
			, const _tParams							& params
			, ::gpk::linear_map_pod<_tParams, uint32_t>	& recycleRenderNodeMap
			, const TGeometryFunc						& funcGeometry
			) {
			int32_t							iEntity				= Entities.Create();
			Entities.Names[iEntity]		= name;

			::gpk::SEntity					& entity			= Entities[iEntity];
			Integrator.BoundingVolumes[entity.RigidBody = Integrator.Create()].HalfSizes = halfSizes;

			int32_t							reuse				= recycleRenderNodeMap.Keys.find([params](const _tParams & value) { return params == value; }, 0);
			if(recycleRenderNodeMap.size() > (uint32_t)reuse)
				entity.RenderNode		= Scene->Clone(recycleRenderNodeMap.Values[reuse], true, true, true);
			else {
				::gpk::SGeometryBuffers			geometry;
				gpk_necs(funcGeometry(geometry));
				entity.RenderNode			= Scene->CreateRenderNode(geometry, name, createSkin);
				recycleRenderNodeMap.push_back(params, entity.RenderNode);
			}
			return iEntity;
		}
	};

#pragma pack(push, 1)
	struct SContactResult {
		::gpk::n3f32			ContactPosition					= {};
		::gpk::n3f32			DistanceDirection				= {};
		double					ForceTransferRatioA				= 0;
		double					ForceTransferRatioB				= 0;

		::gpk::n3f32			InitialVelocityA				= {};
		::gpk::n3f32			InitialVelocityB				= {};
		::gpk::n3f32			FinalVelocityA					= {};
		::gpk::n3f32			FinalVelocityB					= {};

		::gpk::n3f32			InitialRotationA				= {};
		::gpk::n3f32			InitialRotationB				= {};
		::gpk::n3f32			FinalRotationA					= {};
		::gpk::n3f32			FinalRotationB					= {};
	};

	struct SContact {
		double					Time							= {};
		uint32_t				EntityA							= {};
		uint32_t				EntityB							= {};
		::gpk::n3f32			CenterDistance					= {};
		double					DistanceLength					= {};
		::gpk::SContactResult	Result							= {};
	};
#pragma pack(pop)

	::gpk::error_t			collisionDetect		(const ::gpk::SEngine & engine, double totalSeconds, ::gpk::apod<::gpk::SContact> & contactsDetected);


} // namespace

#endif // GPK_ENGINE_H_23627
