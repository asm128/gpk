#include "gpk_engine.h"
#include "gpk_voxel.h"
#include "gpk_ascii_color.h"
#include "gpk_geometry_lh.h"
#include "gpk_engine_shader.h"

gpk::err_t			gpk::updateEntityTransforms			
	( u2_t								iEntity
	, const SEntity			& entity
	, const SVirtualEntityManager	& managedEntities	
	, SRigidBodyIntegrator			& integrator		
	, SRenderNodeManager				& renderNodes
	)
{
	const pau2_t			& children				= managedEntities.Children[iEntity];
	if(-1 != entity.RenderNode) {
		SRenderNodeConstants	& transforms			= renderNodes.Transforms[entity.RenderNode];
		m4f2_t				& worldTransform		= transforms.Model;
		if(-1 == entity.RigidBody)
			worldTransform			= m4f2_t::GetIdentity();
		else {
			integrator.GetTransform(entity.RigidBody, worldTransform);
		}

		if(-1 != entity.Parent) {
			const SEntity	& entityParent			= managedEntities.Entities[entity.Parent];
			if(-1 != entityParent.RenderNode)
				worldTransform			= worldTransform * renderNodes.Transforms[entityParent.RenderNode].Model;
			else if(-1 != entityParent.RigidBody) {
				worldTransform			= worldTransform * integrator.TransformsLocal[entityParent.RigidBody];
			}
		}

		transforms.ModelInverse				= worldTransform.GetInverse();
		transforms.ModelInverseTranspose	= transforms.ModelInverse.GetTranspose();
	}
	else if(-1 != entity.RigidBody) {
		m4f2_t							& worldTransform				= integrator.TransformsLocal[entity.RigidBody];
		integrator.GetTransform(entity.RigidBody, worldTransform);
		if(-1 != entity.Parent) {
			const SEntity				& entityParent					= managedEntities.Entities[entity.Parent];
			if(-1 != entityParent.RenderNode)
				worldTransform						= worldTransform * renderNodes.Transforms[entityParent.RenderNode].Model;
			else if(-1 != entityParent.RigidBody) {
				worldTransform						= worldTransform * integrator.TransformsLocal[entityParent.RigidBody];
			}
		}
	}
	if(children) {
		for(u2_t iChild = 0; iChild < children->size(); ++iChild) {
			const u2_t								iChildEntity					= (*children)[iChild];
			updateEntityTransforms(iChildEntity, managedEntities.Entities[iChildEntity], managedEntities, integrator, renderNodes);
		}
	}

	return 0;
}

gpk::err_t			gpk::SEngine::CreateLight(LIGHT_TYPE type) {
	s2_t							iEntity				= this->Entities.Create();
	SEntity			& entity			= Entities[iEntity];
	entity.RenderNode			= Scene->RenderNodes.Create();

	u1_t						indexLight			= (u1_t)-1;
	switch(type) {
	case LIGHT_TYPE_Directional	: indexLight = (u1_t)Scene->RenderNodes.LightsDirectional	.push_back({}); break;
	case LIGHT_TYPE_Point		: indexLight = (u1_t)Scene->RenderNodes.LightsPoint			.push_back({}); break;
	case LIGHT_TYPE_Spot			: indexLight = (u1_t)Scene->RenderNodes.LightsSpot			.push_back({}); break;
	default:
		error_printf("Invalid light type: %i", (int)type);
		return -1;
	}
	Scene->RenderNodes.Lights		[entity.RenderNode]->push_back({type, indexLight});
	Scene->RenderNodes.Transforms	[entity.RenderNode] = {};
	return iEntity;
}

gpk::err_t			gpk::SEngine::CreateCamera() {
	s2_t						iEntity						= this->Entities.Create();
	SEntity				& entity					= Entities[iEntity];
	entity.RenderNode		= Scene->RenderNodes.Create();
	entity.RigidBody		= this->Integrator.Create();

	SCamera				camera;
	camera.Front			= {1, 0, 0};
	camera.Up				= {0, 1, 0};
	camera.Right			= {0, 0, 1};
	camera.Angle			= .25 * math_pi;

	Scene->RenderNodes.Cameras		[entity.RenderNode]->push_back(camera);
	Scene->RenderNodes.Transforms	[entity.RenderNode]		= {};

	s2_t						iSphere;
	if_fail_fe(iSphere = CreateSphere({}, "Camera reference"));
	if_fail_fe(Entities.SetParent(iSphere, iEntity));
	return iEntity;
}

gpk::err_t			gpk::SEngine::CreateBox			(const SParamsBox & params, vcst_t entityName)	{
	return CreateEntityFromGeometry(entityName, params.HalfSizes, true, params, ParamsBox
		, [params](SGeometryBuffers & geometry) { 
			return geometryBuildBox(geometry, params); 
		});
}

gpk::err_t			gpk::SEngine::CreateCylinder		(const SParamsCylinderWall & params, vcst_t entityName)	{
	const f2_t					radius							= max(params.Radius.Min, params.Radius.Max);
	return CreateEntityFromGeometry(entityName, n3f2_t{radius, params.Height * .5f, radius}, true, params, ParamsCylinder
		, [params](SGeometryBuffers & geometry) { 
			if_fail_fe(gpk::geometryBuildCylinder(geometry, params));
			return 0;
		});
}

gpk::err_t			gpk::SEngine::CreateSphere		(const SParamsSphere & params, vcst_t entityName)	{ 
	return CreateEntityFromGeometry(entityName, n3f2_t{params.Radius, params.Radius, params.Radius}, true, params, ParamsSphere
		, [params](SGeometryBuffers & geometry) { 
			return geometryBuildSphere(geometry, params); 
		});
}

gpk::err_t			gpk::SEngine::CreateGrid		(const SParamsGrid & params, vcst_t entityName)	{ 
	return CreateEntityFromGeometry(entityName, n3f2_t{params.Size.x * .5f, 0, params.Size.y * .5f}, true, params, ParamsGrid
		, [params](SGeometryBuffers & geometry) {
			if_fail_fe(gpk::geometryBuildGrid(geometry, params));
			return 0;
		});
}

gpk::err_t			gpk::SEngine::CreateHelixHalf	(const SParamsHelix & params, vcst_t entityName)	{ 
	const f2_t					radius							= max(params.Radius.Min, params.Radius.Max);
	return CreateEntityFromGeometry(entityName, n3f2_t{radius, params.Length * .5f, radius}, true, params, ParamsHelix
		, [params](SGeometryBuffers & geometry) {
			if_fail_fe(gpk::geometryBuildHelixHalf(geometry, params));
			return 0;
		});
}

gpk::err_t			gpk::SEngine::CreateHelix		(const SParamsHelix & params, vcst_t entityName)	{ 
	const f2_t					radius							= max(params.Radius.Min, params.Radius.Max);
	return CreateEntityFromGeometry(entityName, n3f2_t{radius, params.Length * .5f, radius}, true, params, ParamsHelix
		, [params](SGeometryBuffers & geometry) {
			if_fail_fe(gpk::geometryBuildHelix(geometry, params));
			return 0;
		});
}

gpk::err_t			gpk::SEngine::CreateFigure0		(const SParamsHelix & params, vcst_t entityName)	{ 
	const f2_t					radius							= max(params.Radius.Min, params.Radius.Max);
	return CreateEntityFromGeometry(entityName, n3f2_t{radius, params.Length * .5f, radius}, true, params, ParamsHelix
		, [params](SGeometryBuffers & geometry) {
			if_fail_fe(gpk::geometryBuildHelix(geometry, params));
			return 0;
		});
}

gpk::err_t			gpk::SEngine::CreateCylinderWall(const SParamsCylinderWall & params, vcst_t entityName)	{ 
	const f2_t					radius							= max(params.Radius.Min, params.Radius.Max);
	return CreateEntityFromGeometry(entityName, n3f2_t{radius, params.Height * .5f, radius}, true, params, ParamsCylinderWall
		, [params](SGeometryBuffers & geometry) { 
			if_fail_fe(gpk::geometryBuildCylinderWall(geometry, params));
			return 0;
		});
}

gpk::err_t			gpk::SEngine::CreateCircleSide		(const SParamsCircle & params, vcst_t entityName)	{ 
	const f2_t					radius							= params.Radius;
	return CreateEntityFromGeometry(entityName, n3f2_t{radius, 0, radius}, true, params, ParamsCircleSide
		, [params](SGeometryBuffers & geometry) { 
			if_fail_fe(gpk::geometryBuildCircleSide(geometry, params));
			return 0;
		});
}

gpk::err_t			gpk::SEngine::CreateDisc		(const SParamsDisc & params, vcst_t entityName)	{ 
	const f2_t					radius							= params.Radius;
	return CreateEntityFromGeometry(entityName, n3f2_t{radius, 0, radius}, true, params, ParamsDisc
		, [params](SGeometryBuffers & geometry) { 
			if_fail_fe(gpk::geometryBuildDisc(geometry, params));
			return 0;
		});
}

gpk::err_t			gpk::SEngine::CreateRingSide	(const SParamsRingSide & params, vcst_t entityName)	{ 
	const f2_t					radius							= params.Radius.Max;
	return CreateEntityFromGeometry(entityName, n3f2_t{radius, radius, radius}, true, params, ParamsRingSide
		, [params](SGeometryBuffers & geometry) { 
			if_fail_fe(gpk::geometryBuildRingSide(geometry, params));
			return 0;
		});
}
gpk::err_t			gpk::SEngine::CreateRingFlat	(const SParamsRingSide & params, vcst_t entityName)	{ 
	const f2_t					radius							= params.Radius.Max;
	return CreateEntityFromGeometry(entityName, n3f2_t{radius, radius, radius}, true, params, ParamsRingFlat
		, [params](SGeometryBuffers & geometry) { 
			if_fail_fe(gpk::geometryBuildRingFlat(geometry, params));
			return 0;
		});
}

gpk::err_t			gpk::SEngine::CreateTube		(const SParamsTube & params, vcst_t entityName)	{ 
	const f2_t					radius							= max(params.RadiusYMax.Max, params.RadiusYMin.Max);
	return CreateEntityFromGeometry(entityName, n3f2_t{radius, radius, radius}, true, params, ParamsTube
		, [params](SGeometryBuffers & geometry) { 
			if_fail_fe(gpk::geometryBuildRing(geometry, params));
			return 0;
		});
}

gpk::err_t			gpk::SEngine::CreateImageFromFile(vcst_t fileFolder, vcst_t fileName) {
	sc_t						filePath	[2048]				= {};
	sprintf_s(filePath, "%.*s/%.*s", GPK_VIEW_ARGS(fileFolder), GPK_VIEW_ARGS(fileName));
	s2_t						index							= Images.Keys.find(filePath);
	if(index != -1)
		return Images.Values[index];

	index					= Scene->Graphics->Surfaces.Create(label(filePath));
	pobj<SSurface>	& surface					= Scene->Graphics->Surfaces[index];
	if_fail_fe(gpk::pngFileLoad(PNGCache, filePath, surface->Data));
	surface->Desc.Dimensions	= PNGCache.Header.Size.u1();
	surface->Desc.BitDepth		= PNGCache.Header.BitDepth;
	surface->Desc.ColorType		= PNGCache.Header.ColorType;
	return index; 
}

//
//	s2_t						iEntity					= CreateBox();
//	SEntity		& entity				= Entities[iEntity];
//	SRenderNode			& renderNode			= Scene->RenderNodes[entity.RenderNode];
//
//	u2_t					iMesh					= renderNode.Mesh;
//	pobj<SGeometryMesh>	& mesh			= Scene->Graphics->Meshes[iMesh];
//	Scene->Graphics->Skins[renderNode.Skin]->Material.Color.Diffuse.a = .25f;
//	mesh->GeometrySlices.resize(6);	// one per face
//
//	u2_t					offsetIndex				= 0;
//	for(u2_t iFace = 0; iFace < 6; ++iFace) {
//		u2_t					iSkin					= (u2_t)Scene->Graphics->Skins.Create();
//		pobj<SSkin>	& skin					= Scene->Graphics->Skins.Elements[iSkin];
//
//		u2_t					iSurface				= (u2_t)Scene->Graphics->Surfaces.Create();
//		pobj<SSurface>& surface				= Scene->Graphics->Surfaces[iSurface];
//		surface->Desc.ColorType				= COLOR_TYPE_BGRA;
//		surface->Desc.MethodCompression		= 0;
//		surface->Desc.MethodFilter			= 0;
//		surface->Desc.MethodInterlace		= 0;
//		surface->Desc.Dimensions			= {1, 1};
//		surface->Data.resize(1 * sizeof(bgra));
//		*(bgra*)&surface->Data[0]	= rgba{VOXEL_PALETTE[iFace]};
//		skin->Textures.push_back(iSurface);
//
//		skin->Material.Color.Ambient		= bgra(ASCII_PALETTE[1 + iFace]);
//		skin->Material.Color.Diffuse		= bgra(ASCII_PALETTE[1 + iFace]);
//		skin->Material.Color.Specular		= WHITE;
//		skin->Material.SpecularPower		= 0.5f;
//
//		skin->Material.Color.Ambient		*= .1f;
//
//		SGeometrySlice		& slice					= mesh->GeometrySlices[iFace];
//		slice.Slice				= {offsetIndex, 6};
//		offsetIndex				+= slice.Slice.Count;
//
//		s2_t						iFaceEntity				= Entities.Create();
//		u2_t					iFaceRenderNode			= Scene->RenderNodes.Create();
//		SEntity		& faceEntity			= Entities[iFaceEntity];
//		SRenderNode			& faceRenderNode		= Scene->RenderNodes.RenderNodes[iFaceRenderNode];
//
//		faceRenderNode.Mesh		= iMesh;
//		faceRenderNode.Slice	= iFace;
//		faceRenderNode.Skin		= iSkin;
//		Scene->Graphics->Shaders[faceRenderNode.Shader = Scene->Graphics->Shaders.push_back({})].create(psSolid);
//		Scene->Graphics->Shaders.Names[faceRenderNode.Shader] = "psSolid";
//
//		//faceEntity.RigidBody				= Integrator.Create();
//		faceEntity.Parent					= iEntity;
//		faceEntity.RenderNode				= iFaceRenderNode;
//		this->Entities.Names	[iFaceEntity]	= get_value_label((VOXEL_FACE)iFace);
//
//		this->Entities.Children[iEntity]->push_back(iFaceEntity);
//	}
//
//	return iEntity;
//}
