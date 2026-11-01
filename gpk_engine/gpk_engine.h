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
	err_t					updateEntityTransforms			
		( u2_t								iEntity
		, const SEntity			& entity
		, const SVirtualEntityManager	& managedEntities	
		, SRigidBodyIntegrator			& integrator		
		, SRenderNodeManager				& renderNodes
		);

	struct SEngine {
		pobj<SEngineScene>	Scene			;
		SVirtualEntityManager		Entities		;
		SRigidBodyIntegrator			Integrator		;

		lmpod<SParamsBox			, u2_t>	ParamsBox			;
		lmpod<SParamsDisc		, u2_t>	ParamsDisc			;
		lmpod<SParamsCircle		, u2_t>	ParamsCircleSide	;
		lmpod<SParamsCylinderWall, u2_t>	ParamsCylinderWall	;
		lmpod<SParamsCylinderWall, u2_t>	ParamsCylinder		;
		lmpod<SParamsHelix		, u2_t>	ParamsFigure0		;
		lmpod<SParamsGrid		, u2_t>	ParamsGrid			;
		lmpod<SParamsHelix		, u2_t>	ParamsHelixHalf		;
		lmpod<SParamsHelix		, u2_t>	ParamsHelix			;
		lmpod<SParamsSphere		, u2_t>	ParamsSphere		;
		lmpod<SParamsTube		, u2_t>	ParamsTube			;
		lmpod<SParamsRingSide	, u2_t>	ParamsRingSide		;
		lmpod<SParamsRingSide	, u2_t>	ParamsRingFlat		;

		lmpod<view<sc_t>, u2_t>	Images			= {};

		SPNGData						PNGCache		= {};

		inline	err_t		GetRigidBody		(eid_t idEntity)					const	{ return Entities[idEntity].RigidBody; }
		inline	err_t		GetRenderNode		(eid_t idEntity)					const	{ return Entities[idEntity].RenderNode; }
		inline	err_t		GetParentIndex		(eid_t idEntity)					const	{ return Entities[idEntity].Parent; }

		inline	err_t		GetChildCount		(eid_t idEntity)					const	{ const papod<eid_t> & children = Entities.Children[idEntity]; return children ? children->size() : 0; }
		inline	vceid		GetChildren			(eid_t idEntity)					const	{ const papod<eid_t> & children = Entities.Children[idEntity]; return children ? vceid{*children} : vceid{}; }
		inline	veid			GetChildren			(eid_t idEntity)							{ papod<eid_t> & children = Entities.Children[idEntity]; return children ? *children : veid{}; }

		inline	const SBodyFlags&	GetBodyFlags		(eid_t idEntity)					const	{ return Integrator.GetBodyFlags (Entities[idEntity].RigidBody); }
		inline	const SBodyFrame&	GetBodyFrame		(eid_t idEntity)					const	{ return Integrator.GetBodyFrame (Entities[idEntity].RigidBody); }
		inline	const SBodyForces&	GetBodyForces		(eid_t idEntity)					const	{ return Integrator.GetBodyForces(Entities[idEntity].RigidBody); }
		inline	const SBodyMass&	GetBodyMass			(eid_t idEntity)					const	{ return Integrator.GetBodyMass  (Entities[idEntity].RigidBody); }
		inline	const SBodyCenter&	GetBodyCenter		(eid_t idEntity)					const	{ return Integrator.GetBodyCenter(Entities[idEntity].RigidBody); }
		inline	SBodyFlags&			GetBodyFlags		(eid_t idEntity)							{ return Integrator.GetBodyFlags (Entities[idEntity].RigidBody); }
		inline	SBodyFrame&			GetBodyFrame		(eid_t idEntity)							{ return Integrator.GetBodyFrame (Entities[idEntity].RigidBody); }
		inline	SBodyForces&		GetBodyForces		(eid_t idEntity)							{ return Integrator.GetBodyForces(Entities[idEntity].RigidBody); }
		inline	SBodyMass&			GetBodyMass			(eid_t idEntity)							{ return Integrator.GetBodyMass  (Entities[idEntity].RigidBody); }
		inline	SBodyCenter&		GetBodyCenter		(eid_t idEntity)							{ return Integrator.GetBodyCenter(Entities[idEntity].RigidBody); }

		inline	err_t		SetParent			(eid_t idEntity, eid_t idParent)	{ return Entities.SetParent(idEntity, idParent); }

		err_t				Load				(vcu0_t & input)			{
			if_fail_fe(Scene		->Load(input));
			if_fail_fe(Entities	.Load(input));
			if_fail_fe(Integrator	.Load(input));
			return 0;
		}

		err_t				Save				(au0_t & output)	const	{
			if(Scene)
				if_fail_fe(Scene->Save(output));
			else 
				if_fail_fe(gpk::SEngineScene{}.Save(output));

			if_fail_fe(Entities	.Save(output));
			if_fail_fe(Integrator	.Save(output));
			return 0;
		}

		err_t				Update				(double secondsLastFrame)		{
			Integrator.Integrate(secondsLastFrame);
			for(u2_t iEntity = 0; iEntity < Entities.size(); ++iEntity) {
				SEntity			& entity			= Entities[iEntity];
				if(entity.Parent != -1)
					continue;

				if_true_e(failed(updateEntityTransforms(iEntity, entity, Entities, Integrator, Scene->RenderNodes)));
			}
			return 0;
		}

		// This function messes up with the Entities array so be careful of not having any references to its contents or it may end up trying to access invalid memory
		err_t				Clone				(u2_t iEntitySource, bool cloneSkin, bool cloneSurfaces, bool cloneShaders) {
			const SEntity		entitySource		= Entities[iEntitySource];
			s2_t							iEntityNew			= Entities.Create();
			SEntity			& entityNew			= Entities[iEntityNew];
			entityNew.RenderNode		= Scene->Clone(entitySource.RenderNode, cloneSkin, cloneSurfaces, cloneShaders);
			entityNew.RigidBody			= ((u2_t)entitySource.RigidBody < Integrator.Flags.size()) ? Integrator.Clone(entitySource.RigidBody) : (u2_t)-1;
			entityNew.Parent			= entitySource.Parent;

			const pau32				childrenSource		= Entities.Children[iEntitySource];
			if(childrenSource && childrenSource->size()) {
				pau32					childrenNew			= Entities.Children[iEntityNew];
				for(u2_t iChild = 0; iChild < childrenSource->size(); ++iChild) {
					u2_t						entityChild			= Clone((*childrenSource)[iChild], cloneSkin, cloneSurfaces, cloneShaders);
					Entities[entityChild].Parent= iEntityNew;
					childrenNew->push_back(entityChild);
				}
				Entities.Children[iEntityNew]	= childrenNew;
			}
			return iEntityNew;
		}

		inline	err_t		SetNodeSize			(u2_t iEntity, const n3f2_t & size)					{ Scene->RenderNodes.BaseTransforms[GetRenderNode(iEntity)].NodeSize = size; return 0; }
		inline	err_t		SetMeshScale		(u2_t iEntity, const n3f2_t & scale, bool halfSizes)	{ Scene->RenderNodes.BaseTransforms[GetRenderNode(iEntity)].Model.Scale(scale, true); 
			if(halfSizes) {
				if_fail_fe(SetHalfSizes(iEntity, scale * .5f));
				if_fail_fe(SetNodeSize(iEntity, scale));
			}
			return 0; 
		}
		inline	err_t		SetMeshPosition		(u2_t iEntity, const n3f2_t & position)				{ Scene->RenderNodes.BaseTransforms[GetRenderNode(iEntity)].Model.SetTranslation(position, false); return 0; }
		err_t				SetColorDiffuse		(u2_t iEntity, const rgbaf & diffuse)				{ Scene->Graphics->Skins[Scene->RenderNodes[GetRenderNode(iEntity)].Skin]->Material.Color.Diffuse = diffuse; return 0; }
		err_t				SetShader			(u2_t iEntity, const ::std::function<TFuncPixelShader> & shader, vcst_t name) {
			const u2_t					iShader				= Scene->RenderNodes[GetRenderNode(iEntity)].Shader;
			Scene->Graphics->Shaders[iShader].create(shader);
			Scene->Graphics->Shaders.Names[iShader] = name;
			return 0;
		}
		inline	err_t		IsPhysicsActive		(u2_t iEntity)										const	{ return Integrator.Active	(Entities[iEntity].RigidBody) ? 1 : 0; }
		inline	float				GetMass				(u2_t iEntity)										const	{ return Integrator.GetMass	(Entities[iEntity].RigidBody);	}

		inline	err_t		InvalidateTransform	(u2_t iEntity)												{ Integrator.Flags[GetRigidBody(iEntity)].UpdatedTransform = false;		return 0; }

		inline	err_t		GetPosition			(u2_t iEntity, n3f2_t & position)				const	{ Integrator.GetPosition	(GetRigidBody(iEntity), position);			return 0; }
		inline	err_t		GetVelocity			(u2_t iEntity, n3f2_t & velocity)				const	{ Integrator.GetVelocity	(GetRigidBody(iEntity), velocity);			return 0; }
		inline	err_t		GetAcceleration		(u2_t iEntity, n3f2_t & acceleration)			const	{ Integrator.GetAcceleration(GetRigidBody(iEntity), acceleration);		return 0; }
		inline	err_t		GetRotation			(u2_t iEntity, n3f2_t & velocity)				const	{ Integrator.GetRotation	(GetRigidBody(iEntity), velocity);			return 0; }
		inline	err_t		GetOrientation		(u2_t iEntity, quatf2_t & orientation)		const	{ Integrator.GetOrientation	(GetRigidBody(iEntity), orientation);		return 0; }
		inline	err_t		SetPhysicsActive	(u2_t iEntity, bool active)									{ Integrator.SetActive		(GetRigidBody(iEntity), active);			return 0; }
		inline	err_t		SetCollides			(u2_t iEntity, bool collides)								{ Integrator.SetCollides	(GetRigidBody(iEntity), collides);			return 0; }
		inline	err_t		SetHalfSizes		(u2_t iEntity, const n3f2_t & halfSizes)				{ Integrator.SetHalfSizes	(GetRigidBody(iEntity), halfSizes);			return 0; }
		inline	err_t		SetMass				(u2_t iEntity, float mass)									{ Integrator.SetMass		(GetRigidBody(iEntity), mass);				return 0; }
		inline	err_t		SetMassInverse		(u2_t iEntity, float inverseMass)							{ Integrator.SetMassInverse	(GetRigidBody(iEntity), inverseMass);		return 0; }
		inline	err_t		SetPosition			(u2_t iEntity, const n3f2_t & position)				{ Integrator.SetPosition	(GetRigidBody(iEntity), position);			return 0; }
		inline	err_t		SetVelocity			(u2_t iEntity, const n3f2_t & velocity)				{ Integrator.SetVelocity	(GetRigidBody(iEntity), velocity);			return 0; }
		inline	err_t		SetAcceleration		(u2_t iEntity, const n3f2_t & acceleration)			{ Integrator.SetAcceleration(GetRigidBody(iEntity), acceleration);		return 0; }
		inline	err_t		SetRotation			(u2_t iEntity, const n3f2_t & velocity)				{ Integrator.SetRotation	(GetRigidBody(iEntity), velocity);			return 0; }
		inline	err_t		SetOrientation		(u2_t iEntity, const quatf2_t & orientation)			{ Integrator.SetOrientation	(GetRigidBody(iEntity), orientation);		return 0; }
		inline	err_t		SetDampingLinear	(u2_t iEntity, float damping)								{ Integrator.Masses[GetRigidBody(iEntity)].LinearDamping	= damping;	return 0; }
		inline	err_t		SetDampingAngular	(u2_t iEntity, float damping)								{ Integrator.Masses[GetRigidBody(iEntity)].AngularDamping	= damping;	return 0; }
		err_t				SetHidden			(u2_t iEntity, bool hidden)									{ Scene->RenderNodes.Flags[GetRenderNode(iEntity)].NoDraw	= hidden;	return 0; }	

		err_t				ToggleHidden		(u2_t iEntity) {
			SRenderNodeFlags			& flags				= Scene->RenderNodes.Flags[GetRenderNode(iEntity)];
			flags.NoDraw				= !flags.NoDraw;
			return 0;
		}

		err_t				CreateLight			(LIGHT_TYPE type);
		err_t				CreateCamera		();


		err_t				CreateBox			(const SParamsBox			& params, vcst_t entityName = view<sc_t>{ 3, "Box"});
		err_t				CreateSphere		(const SParamsSphere		& params, vcst_t entityName = view<sc_t>{ 6, "Sphere"});
		err_t				CreateCylinderWall	(const SParamsCylinderWall	& params, vcst_t entityName = view<sc_t>{13, "Cylinder Wall"});
		err_t				CreateCylinder		(const SParamsCylinderWall	& params, vcst_t entityName = view<sc_t>{ 8, "Cylinder"});		
		err_t				CreateCircleSide	(const SParamsCircle		& params, vcst_t entityName = view<sc_t>{11, "Circle Side"});
		err_t				CreateDisc			(const SParamsDisc			& params, vcst_t entityName = view<sc_t>{ 4, "Disc"});
		err_t				CreateTube			(const SParamsTube			& params, vcst_t entityName = view<sc_t>{ 4, "Tube"});
		err_t				CreateRingSide		(const SParamsRingSide		& params, vcst_t entityName = view<sc_t>{ 9, "Ring Side"});
		err_t				CreateRingFlat		(const SParamsRingSide		& params, vcst_t entityName = view<sc_t>{ 9, "Ring flat"});
		err_t				CreateGrid			(const SParamsGrid			& params, vcst_t entityName = view<sc_t>{ 4, "Grid"});
		err_t				CreateHelixHalf		(const SParamsHelix			& params, vcst_t entityName = view<sc_t>{10, "Helix half"});
		err_t				CreateHelix			(const SParamsHelix			& params, vcst_t entityName = view<sc_t>{ 5, "Helix"});
		err_t				CreateFigure0		(const SParamsHelix			& params, vcst_t entityName = "Figure0");

		err_t				CreateImageFromFile	(const vcst_t & fileFolder, const vcst_t & fileName);

		typedef std::function<err_t(SGeometryBuffers&)> TGeometryFunc;

		tplt<tpnm _tParams>
		err_t				CreateEntityFromGeometry
			( const view<sc_t>							name		
			, const n3f2_t						halfSizes
			, bool										createSkin
			, const _tParams							& params
			, linear_map_pod<_tParams, u2_t>	& recycleRenderNodeMap
			, const TGeometryFunc						& funcGeometry
			) {
			s2_t							iEntity				= Entities.Create();
			Entities.Names[iEntity]		= name;

			SEntity					& entity			= Entities[iEntity];
			Integrator.BoundingVolumes[entity.RigidBody = Integrator.Create()].HalfSizes = halfSizes;

			s2_t							reuse				= recycleRenderNodeMap.Keys.find([params](const _tParams & value) { return params == value; }, 0);
			if(recycleRenderNodeMap.size() > (u2_t)reuse)
				entity.RenderNode		= Scene->Clone(recycleRenderNodeMap.Values[reuse], true, true, true);
			else {
				SGeometryBuffers			geometry;
				if_fail_fe(funcGeometry(geometry));
				entity.RenderNode			= Scene->CreateRenderNode(geometry, name, createSkin);
				recycleRenderNodeMap.push_back(params, entity.RenderNode);
			}
			return iEntity;
		}
	};

#pragma pack(push, 1)
	struct SContactResult {
		n3f2_t			ContactPosition					= {};
		n3f2_t			DistanceDirection				= {};
		double					ForceTransferRatioA				= 0;
		double					ForceTransferRatioB				= 0;

		n3f2_t			InitialVelocityA				= {};
		n3f2_t			InitialVelocityB				= {};
		n3f2_t			FinalVelocityA					= {};
		n3f2_t			FinalVelocityB					= {};

		n3f2_t			InitialRotationA				= {};
		n3f2_t			InitialRotationB				= {};
		n3f2_t			FinalRotationA					= {};
		n3f2_t			FinalRotationB					= {};
	};

	struct SContact {
		double					Time							= {};
		u2_t				EntityA							= {};
		u2_t				EntityB							= {};
		n3f2_t			CenterDistance					= {};
		double					DistanceLength					= {};
		SContactResult	Result							= {};
	};
#pragma pack(pop)

	err_t			collisionDetect		(const SEngine & engine, double totalSeconds, apod<SContact> & contactsDetected);


} // namespace

#endif // GPK_ENGINE_H_23627
