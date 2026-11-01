#include "gpk_engine_entity.h"
#include "gpk_engine_scene.h"
#include "gpk_rigid_body.h"
#include "gpk_linear_map_pod.h"
#include "gpk_linear_map_pobj.h"
#include "gpk_geometry.h"
#include "gpk_png.h"

#ifndef GPK_ENGINE_H_23627
#define GPK_ENGINE_H_23627

namespace gpk 
{
	err_t					updateEntityTransforms			
		( u2_t							iEntity
		, cnst SEntity					& entity
		, cnst SVirtualEntityManager	& managedEntities	
		, SRigidBodyIntegrator			& integrator		
		, SRenderNodeManager			& renderNodes
		);

	struct SEngine {
		pobj<SEngineScene>			Scene			;
		SVirtualEntityManager		Entities		;
		SRigidBodyIntegrator		Integrator		;

		lmpod<SParamsBox			, u2_t>	ParamsBox			;
		lmpod<SParamsDisc			, u2_t>	ParamsDisc			;
		lmpod<SParamsCircle			, u2_t>	ParamsCircleSide	;
		lmpod<SParamsCylinderWall	, u2_t>	ParamsCylinderWall	;
		lmpod<SParamsCylinderWall	, u2_t>	ParamsCylinder		;
		lmpod<SParamsHelix			, u2_t>	ParamsFigure0		;
		lmpod<SParamsGrid			, u2_t>	ParamsGrid			;
		lmpod<SParamsHelix			, u2_t>	ParamsHelixHalf		;
		lmpod<SParamsHelix			, u2_t>	ParamsHelix			;
		lmpod<SParamsSphere			, u2_t>	ParamsSphere		;
		lmpod<SParamsTube			, u2_t>	ParamsTube			;
		lmpod<SParamsRingSide		, u2_t>	ParamsRingSide		;
		lmpod<SParamsRingSide		, u2_t>	ParamsRingFlat		;

		lmpod<vcst_t, u2_t>			Images			= {};
		SPNGData					PNGCache		= {};

		inln	err_t				GetRigidBody		(eid_t idEntity)					cnst	{ return Entities[idEntity].RigidBody; }
		inln	err_t				GetRenderNode		(eid_t idEntity)					cnst	{ return Entities[idEntity].RenderNode; }
		inln	err_t				GetParentIndex		(eid_t idEntity)					cnst	{ return Entities[idEntity].Parent; }

		inln	err_t				GetChildCount		(eid_t idEntity)					cnst	{ cnst papod<eid_t> & children = Entities.Children[idEntity]; return children ? children->size() : 0; }
		inln	vceid				GetChildren			(eid_t idEntity)					cnst	{ cnst papod<eid_t> & children = Entities.Children[idEntity]; return children ? vceid{*children} : vceid{}; }
		inln	veid				GetChildren			(eid_t idEntity)							{ papod<eid_t> & children = Entities.Children[idEntity]; return children ? *children : veid{}; }

		inln	cnst SBodyFlags&	GetBodyFlags		(eid_t idEntity)					cnst	{ return Integrator.GetBodyFlags (Entities[idEntity].RigidBody); }
		inln	cnst SBodyFrame&	GetBodyFrame		(eid_t idEntity)					cnst	{ return Integrator.GetBodyFrame (Entities[idEntity].RigidBody); }
		inln	cnst SBodyForces&	GetBodyForces		(eid_t idEntity)					cnst	{ return Integrator.GetBodyForces(Entities[idEntity].RigidBody); }
		inln	cnst SBodyMass&		GetBodyMass			(eid_t idEntity)					cnst	{ return Integrator.GetBodyMass  (Entities[idEntity].RigidBody); }
		inln	cnst SBodyCenter&	GetBodyCenter		(eid_t idEntity)					cnst	{ return Integrator.GetBodyCenter(Entities[idEntity].RigidBody); }
		inln	SBodyFlags&			GetBodyFlags		(eid_t idEntity)							{ return Integrator.GetBodyFlags (Entities[idEntity].RigidBody); }
		inln	SBodyFrame&			GetBodyFrame		(eid_t idEntity)							{ return Integrator.GetBodyFrame (Entities[idEntity].RigidBody); }
		inln	SBodyForces&		GetBodyForces		(eid_t idEntity)							{ return Integrator.GetBodyForces(Entities[idEntity].RigidBody); }
		inln	SBodyMass&			GetBodyMass			(eid_t idEntity)							{ return Integrator.GetBodyMass  (Entities[idEntity].RigidBody); }
		inln	SBodyCenter&		GetBodyCenter		(eid_t idEntity)							{ return Integrator.GetBodyCenter(Entities[idEntity].RigidBody); }

		inln	err_t				SetParent			(eid_t idEntity, eid_t idParent)			{ return Entities.SetParent(idEntity, idParent); }

		err_t						Load				(vcu0_t & input)			{
			if_fail_fe(Scene		->Load(input));
			if_fail_fe(Entities		.Load(input));
			if_fail_fe(Integrator	.Load(input));
			return 0;
		}

		err_t				Save				(au0_t & output)	cnst	{
			if(Scene) {
				if_fail_fe(Scene->Save(output));
			}
			else 
				if_fail_fe(gpk::SEngineScene{}.Save(output));

			if_fail_fe(Entities	.Save(output));
			if_fail_fe(Integrator	.Save(output));
			return 0;
		}

		err_t				Update				(f3_t secondsLastFrame)		{
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
			cnst SEntity		entitySource		= Entities[iEntitySource];
			s2_t				iEntityNew			= Entities.Create();
			SEntity				& entityNew			= Entities[iEntityNew];
			entityNew.RenderNode		= Scene->Clone(entitySource.RenderNode, cloneSkin, cloneSurfaces, cloneShaders);
			entityNew.RigidBody			= ((u2_t)entitySource.RigidBody < Integrator.Flags.size()) ? Integrator.Clone(entitySource.RigidBody) : (u2_t)-1;
			entityNew.Parent			= entitySource.Parent;

			cnst pau2_t		childrenSource		= Entities.Children[iEntitySource];
			if(childrenSource && childrenSource->size()) {
				pau2_t				childrenNew			= Entities.Children[iEntityNew];
				for(u2_t iChild = 0; iChild < childrenSource->size(); ++iChild) {
					u2_t				entityChild			= Clone((*childrenSource)[iChild], cloneSkin, cloneSurfaces, cloneShaders);
					Entities[entityChild].Parent= iEntityNew;
					childrenNew->push_back(entityChild);
				}
				Entities.Children[iEntityNew]	= childrenNew;
			}
			return iEntityNew;
		}

		inln	err_t		SetNodeSize			(u2_t iEntity, cnst n3f2_t & size)					{ Scene->RenderNodes.BaseTransforms[GetRenderNode(iEntity)].NodeSize = size; return 0; }
		inln	err_t		SetMeshScale		(u2_t iEntity, cnst n3f2_t & scale, bool halfSizes)	{ Scene->RenderNodes.BaseTransforms[GetRenderNode(iEntity)].Model.Scale(scale, true); 
			if(halfSizes) {
				if_fail_fe(SetHalfSizes(iEntity, scale * .5f));
				if_fail_fe(SetNodeSize(iEntity, scale));
			}
			return 0; 
		}
		inln	err_t		SetMeshPosition		(u2_t iEntity, cnst n3f2_t & position)				{ Scene->RenderNodes.BaseTransforms[GetRenderNode(iEntity)].Model.SetTranslation(position, false); return 0; }
		err_t				SetColorDiffuse		(u2_t iEntity, cnst rgbaf & diffuse)				{ Scene->Graphics->Skins[Scene->RenderNodes[GetRenderNode(iEntity)].Skin]->Material.Color.Diffuse = diffuse; return 0; }
		err_t				SetShader			(u2_t iEntity, cnst function<TFuncPixelShader> & shader, vcst_t name) {
			cnst u2_t					iShader				= Scene->RenderNodes[GetRenderNode(iEntity)].Shader;
			Scene->Graphics->Shaders[iShader].create(shader);
			Scene->Graphics->Shaders.Names[iShader] = name;
			return 0;
		}
		inln	err_t		IsPhysicsActive		(u2_t iEntity)							cnst	{ return Integrator.Active	(Entities[iEntity].RigidBody) ? 1 : 0; }
		inln	f2_t		GetMass				(u2_t iEntity)							cnst	{ return Integrator.GetMass	(Entities[iEntity].RigidBody);	}

		inln	err_t		InvalidateTransform	(u2_t iEntity)									{ Integrator.Flags[GetRigidBody(iEntity)].UpdatedTransform = false;		return 0; }

		inln	err_t		GetPosition			(u2_t iEntity, n3f2_t & position)		cnst	{ Integrator.GetPosition	(GetRigidBody(iEntity), position);			return 0; }
		inln	err_t		GetVelocity			(u2_t iEntity, n3f2_t & velocity)		cnst	{ Integrator.GetVelocity	(GetRigidBody(iEntity), velocity);			return 0; }
		inln	err_t		GetAcceleration		(u2_t iEntity, n3f2_t & acceleration)	cnst	{ Integrator.GetAcceleration(GetRigidBody(iEntity), acceleration);		return 0; }
		inln	err_t		GetRotation			(u2_t iEntity, n3f2_t & velocity)		cnst	{ Integrator.GetRotation	(GetRigidBody(iEntity), velocity);			return 0; }
		inln	err_t		GetOrientation		(u2_t iEntity, quatf2_t & orientation)	cnst	{ Integrator.GetOrientation	(GetRigidBody(iEntity), orientation);		return 0; }
		inln	err_t		SetPhysicsActive	(u2_t iEntity, bool active)						{ Integrator.SetActive		(GetRigidBody(iEntity), active);			return 0; }
		inln	err_t		SetCollides			(u2_t iEntity, bool collides)					{ Integrator.SetCollides	(GetRigidBody(iEntity), collides);			return 0; }
		inln	err_t		SetHalfSizes		(u2_t iEntity, cnst n3f2_t & halfSizes)			{ Integrator.SetHalfSizes	(GetRigidBody(iEntity), halfSizes);			return 0; }
		inln	err_t		SetMass				(u2_t iEntity, f2_t mass)						{ Integrator.SetMass		(GetRigidBody(iEntity), mass);				return 0; }
		inln	err_t		SetMassInverse		(u2_t iEntity, f2_t inverseMass)				{ Integrator.SetMassInverse	(GetRigidBody(iEntity), inverseMass);		return 0; }
		inln	err_t		SetPosition			(u2_t iEntity, cnst n3f2_t & position)			{ Integrator.SetPosition	(GetRigidBody(iEntity), position);			return 0; }
		inln	err_t		SetVelocity			(u2_t iEntity, cnst n3f2_t & velocity)			{ Integrator.SetVelocity	(GetRigidBody(iEntity), velocity);			return 0; }
		inln	err_t		SetAcceleration		(u2_t iEntity, cnst n3f2_t & acceleration)		{ Integrator.SetAcceleration(GetRigidBody(iEntity), acceleration);		return 0; }
		inln	err_t		SetRotation			(u2_t iEntity, cnst n3f2_t & velocity)			{ Integrator.SetRotation	(GetRigidBody(iEntity), velocity);			return 0; }
		inln	err_t		SetOrientation		(u2_t iEntity, cnst quatf2_t & orientation)		{ Integrator.SetOrientation	(GetRigidBody(iEntity), orientation);		return 0; }
		inln	err_t		SetDampingLinear	(u2_t iEntity, f2_t damping)					{ Integrator.Masses[GetRigidBody(iEntity)].LinearDamping	= damping;	return 0; }
		inln	err_t		SetDampingAngular	(u2_t iEntity, f2_t damping)					{ Integrator.Masses[GetRigidBody(iEntity)].AngularDamping	= damping;	return 0; }
		err_t				SetHidden			(u2_t iEntity, bool hidden)						{ Scene->RenderNodes.Flags[GetRenderNode(iEntity)].NoDraw	= hidden;	return 0; }	

		err_t				ToggleHidden		(u2_t iEntity) {
			SRenderNodeFlags			& flags				= Scene->RenderNodes.Flags[GetRenderNode(iEntity)];
			flags.NoDraw				= !flags.NoDraw;
			return 0;
		}

		err_t				CreateLight			(LIGHT_TYPE type);
		err_t				CreateCamera		();


		err_t				CreateBox			(cnst SParamsBox			& params, vcst_t entityName = "Box");
		err_t				CreateSphere		(cnst SParamsSphere			& params, vcst_t entityName = "Sphere");
		err_t				CreateCylinderWall	(cnst SParamsCylinderWall	& params, vcst_t entityName = "Cylinder Wall");
		err_t				CreateCylinder		(cnst SParamsCylinderWall	& params, vcst_t entityName = "Cylinder");
		err_t				CreateCircleSide	(cnst SParamsCircle			& params, vcst_t entityName = "Circle Side");
		err_t				CreateDisc			(cnst SParamsDisc			& params, vcst_t entityName = "Disc");
		err_t				CreateTube			(cnst SParamsTube			& params, vcst_t entityName = "Tube");
		err_t				CreateRingSide		(cnst SParamsRingSide		& params, vcst_t entityName = "Ring Side");
		err_t				CreateRingFlat		(cnst SParamsRingSide		& params, vcst_t entityName = "Ring flat");
		err_t				CreateGrid			(cnst SParamsGrid			& params, vcst_t entityName = "Grid");
		err_t				CreateHelixHalf		(cnst SParamsHelix			& params, vcst_t entityName = "Helix half");
		err_t				CreateHelix			(cnst SParamsHelix			& params, vcst_t entityName = "Helix");
		err_t				CreateFigure0		(cnst SParamsHelix			& params, vcst_t entityName = "Figure0");

		err_t				CreateImageFromFile	(vcst_t fileFolder, vcst_t fileName);

		typedef function<err_t(SGeometryBuffers&)> TGeometryFunc;

		tplt<tpnm _tParams>
		err_t				CreateEntityFromGeometry
			( vcst_t							name		
			, cnst n3f2_t						halfSizes
			, bool								createSkin
			, cnst _tParams						& params
			, linear_map_pod<_tParams, u2_t>	& recycleRenderNodeMap
			, cnst TGeometryFunc				& funcGeometry
			) {
			s2_t				iEntity				= Entities.Create();
			Entities.Names[iEntity]	= name;

			SEntity				& entity			= Entities[iEntity];
			Integrator.BoundingVolumes[entity.RigidBody = Integrator.Create()].HalfSizes = halfSizes;

			s2_t				reuse				= recycleRenderNodeMap.Keys.find([params](cnst _tParams & value) { return params == value; }, 0);
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
		n3f2_t			ContactPosition		= {};
		n3f2_t			DistanceDirection	= {};
		f3_t			ForceTransferRatioA	= 0;
		f3_t			ForceTransferRatioB	= 0;

		n3f2_t			InitialVelocityA	= {};
		n3f2_t			InitialVelocityB	= {};
		n3f2_t			FinalVelocityA		= {};
		n3f2_t			FinalVelocityB		= {};

		n3f2_t			InitialRotationA	= {};
		n3f2_t			InitialRotationB	= {};
		n3f2_t			FinalRotationA		= {};
		n3f2_t			FinalRotationB		= {};
	};

	struct SContact {
		f3_t			Time				= {};
		u2_t			EntityA				= {};
		u2_t			EntityB				= {};
		n3f2_t			CenterDistance		= {};
		f3_t			DistanceLength		= {};
		SContactResult	Result				= {};
	};
#pragma pack(pop)

	err_t			collisionDetect		(cnst SEngine & engine, f3_t totalSeconds, apod<SContact> & contactsDetected);


} // namespace

#endif // GPK_ENGINE_H_23627
