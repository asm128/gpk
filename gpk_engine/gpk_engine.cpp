#include "gpk_engine.h"
#include "gpk_voxel.h"
#include "gpk_ascii_color.h"
#include "gpk_geometry_lh.h"
#include "gpk_engine_shader.h"

::gpk::err_t			gpk::updateEntityTransforms			
	( u2_t								iEntity
	, const ::gpk::SEntity			& entity
	, const ::gpk::SVirtualEntityManager	& managedEntities	
	, ::gpk::SRigidBodyIntegrator			& integrator		
	, ::gpk::SRenderNodeManager				& renderNodes
	)
{
	const ::gpk::pau32			& children				= managedEntities.Children[iEntity];
	if(-1 != entity.RenderNode) {
		::gpk::SRenderNodeConstants	& transforms			= renderNodes.Transforms[entity.RenderNode];
		::gpk::m4f2_t				& worldTransform		= transforms.Model;
		if(-1 == entity.RigidBody)
			worldTransform			= ::gpk::m4f2_t::GetIdentity();
		else {
			integrator.GetTransform(entity.RigidBody, worldTransform);
		}

		if(-1 != entity.Parent) {
			const ::gpk::SEntity	& entityParent			= managedEntities.Entities[entity.Parent];
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
		::gpk::m4f2_t							& worldTransform				= integrator.TransformsLocal[entity.RigidBody];
		integrator.GetTransform(entity.RigidBody, worldTransform);
		if(-1 != entity.Parent) {
			const ::gpk::SEntity				& entityParent					= managedEntities.Entities[entity.Parent];
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
			::gpk::updateEntityTransforms(iChildEntity, managedEntities.Entities[iChildEntity], managedEntities, integrator, renderNodes);
		}
	}

	return 0;
}

::gpk::err_t			gpk::SEngine::CreateLight(::gpk::LIGHT_TYPE type) {
	s2_t							iEntity				= this->Entities.Create();
	::gpk::SEntity			& entity			= Entities[iEntity];
	entity.RenderNode			= Scene->RenderNodes.Create();

	u1_t						indexLight			= (u1_t)-1;
	switch(type) {
	case ::gpk::LIGHT_TYPE_Directional	: indexLight = (u1_t)Scene->RenderNodes.LightsDirectional	.push_back({}); break;
	case ::gpk::LIGHT_TYPE_Point		: indexLight = (u1_t)Scene->RenderNodes.LightsPoint			.push_back({}); break;
	case ::gpk::LIGHT_TYPE_Spot			: indexLight = (u1_t)Scene->RenderNodes.LightsSpot			.push_back({}); break;
	default:
		error_printf("Invalid light type: %i", (int)type);
		return -1;
	}
	Scene->RenderNodes.Lights		[entity.RenderNode]->push_back({type, indexLight});
	Scene->RenderNodes.Transforms	[entity.RenderNode] = {};
	return iEntity;
}

::gpk::err_t			gpk::SEngine::CreateCamera() {
	s2_t						iEntity						= this->Entities.Create();
	::gpk::SEntity				& entity					= Entities[iEntity];
	entity.RenderNode		= Scene->RenderNodes.Create();
	entity.RigidBody		= this->Integrator.Create();

	::gpk::SCamera				camera;
	camera.Front			= {1, 0, 0};
	camera.Up				= {0, 1, 0};
	camera.Right			= {0, 0, 1};
	camera.Angle			= .25 * ::gpk::math_pi;

	Scene->RenderNodes.Cameras		[entity.RenderNode]->push_back(camera);
	Scene->RenderNodes.Transforms	[entity.RenderNode]		= {};

	s2_t						iSphere;
	if_fail_fe(iSphere = CreateSphere({}, "Camera reference"));
	if_fail_fe(Entities.SetParent(iSphere, iEntity));
	return iEntity;
}

::gpk::err_t			gpk::SEngine::CreateBox			(const ::gpk::SParamsBox & params, ::gpk::vcst_t entityName)	{
	return CreateEntityFromGeometry(entityName, params.HalfSizes, true, params, ParamsBox
		, [params](::gpk::SGeometryBuffers & geometry) { 
			return ::gpk::geometryBuildBox(geometry, params); 
		});
}

::gpk::err_t			gpk::SEngine::CreateCylinder		(const ::gpk::SParamsCylinderWall & params, ::gpk::vcst_t entityName)	{
	const float					radius							= ::gpk::max(params.Radius.Min, params.Radius.Max);
	return CreateEntityFromGeometry(entityName, ::gpk::n3f2_t{radius, params.Height * .5f, radius}, true, params, ParamsCylinder
		, [params](::gpk::SGeometryBuffers & geometry) { 
			if_fail_fe(gpk::geometryBuildCylinder(geometry, params));
			return 0;
		});
}

::gpk::err_t			gpk::SEngine::CreateSphere		(const ::gpk::SParamsSphere & params, ::gpk::vcst_t entityName)	{ 
	return CreateEntityFromGeometry(entityName, ::gpk::n3f2_t{params.Radius, params.Radius, params.Radius}, true, params, ParamsSphere
		, [params](::gpk::SGeometryBuffers & geometry) { 
			return ::gpk::geometryBuildSphere(geometry, params); 
		});
}

::gpk::err_t			gpk::SEngine::CreateGrid		(const ::gpk::SParamsGrid & params, ::gpk::vcst_t entityName)	{ 
	return CreateEntityFromGeometry(entityName, ::gpk::n3f2_t{params.Size.x * .5f, 0, params.Size.y * .5f}, true, params, ParamsGrid
		, [params](::gpk::SGeometryBuffers & geometry) {
			if_fail_fe(gpk::geometryBuildGrid(geometry, params));
			return 0;
		});
}

::gpk::err_t			gpk::SEngine::CreateHelixHalf	(const ::gpk::SParamsHelix & params, ::gpk::vcst_t entityName)	{ 
	const float					radius							= ::gpk::max(params.Radius.Min, params.Radius.Max);
	return CreateEntityFromGeometry(entityName, ::gpk::n3f2_t{radius, params.Length * .5f, radius}, true, params, ParamsHelix
		, [params](::gpk::SGeometryBuffers & geometry) {
			if_fail_fe(gpk::geometryBuildHelixHalf(geometry, params));
			return 0;
		});
}

::gpk::err_t			gpk::SEngine::CreateHelix		(const ::gpk::SParamsHelix & params, ::gpk::vcst_t entityName)	{ 
	const float					radius							= ::gpk::max(params.Radius.Min, params.Radius.Max);
	return CreateEntityFromGeometry(entityName, ::gpk::n3f2_t{radius, params.Length * .5f, radius}, true, params, ParamsHelix
		, [params](::gpk::SGeometryBuffers & geometry) {
			if_fail_fe(gpk::geometryBuildHelix(geometry, params));
			return 0;
		});
}

::gpk::err_t			gpk::SEngine::CreateFigure0		(const ::gpk::SParamsHelix & params, ::gpk::vcst_t entityName)	{ 
	const float					radius							= ::gpk::max(params.Radius.Min, params.Radius.Max);
	return CreateEntityFromGeometry(entityName, ::gpk::n3f2_t{radius, params.Length * .5f, radius}, true, params, ParamsHelix
		, [params](::gpk::SGeometryBuffers & geometry) {
			if_fail_fe(gpk::geometryBuildHelix(geometry, params));
			return 0;
		});
}

::gpk::err_t			gpk::SEngine::CreateCylinderWall(const ::gpk::SParamsCylinderWall & params, ::gpk::vcst_t entityName)	{ 
	const float					radius							= ::gpk::max(params.Radius.Min, params.Radius.Max);
	return CreateEntityFromGeometry(entityName, ::gpk::n3f2_t{radius, params.Height * .5f, radius}, true, params, ParamsCylinderWall
		, [params](::gpk::SGeometryBuffers & geometry) { 
			if_fail_fe(gpk::geometryBuildCylinderWall(geometry, params));
			return 0;
		});
}

::gpk::err_t			gpk::SEngine::CreateCircleSide		(const ::gpk::SParamsCircle & params, ::gpk::vcst_t entityName)	{ 
	const float					radius							= params.Radius;
	return CreateEntityFromGeometry(entityName, ::gpk::n3f2_t{radius, 0, radius}, true, params, ParamsCircleSide
		, [params](::gpk::SGeometryBuffers & geometry) { 
			if_fail_fe(gpk::geometryBuildCircleSide(geometry, params));
			return 0;
		});
}

::gpk::err_t			gpk::SEngine::CreateDisc		(const ::gpk::SParamsDisc & params, ::gpk::vcst_t entityName)	{ 
	const float					radius							= params.Radius;
	return CreateEntityFromGeometry(entityName, ::gpk::n3f2_t{radius, 0, radius}, true, params, ParamsDisc
		, [params](::gpk::SGeometryBuffers & geometry) { 
			if_fail_fe(gpk::geometryBuildDisc(geometry, params));
			return 0;
		});
}

::gpk::err_t			gpk::SEngine::CreateRingSide	(const ::gpk::SParamsRingSide & params, ::gpk::vcst_t entityName)	{ 
	const float					radius							= params.Radius.Max;
	return CreateEntityFromGeometry(entityName, ::gpk::n3f2_t{radius, radius, radius}, true, params, ParamsRingSide
		, [params](::gpk::SGeometryBuffers & geometry) { 
			if_fail_fe(gpk::geometryBuildRingSide(geometry, params));
			return 0;
		});
}
::gpk::err_t			gpk::SEngine::CreateRingFlat	(const ::gpk::SParamsRingSide & params, ::gpk::vcst_t entityName)	{ 
	const float					radius							= params.Radius.Max;
	return CreateEntityFromGeometry(entityName, ::gpk::n3f2_t{radius, radius, radius}, true, params, ParamsRingFlat
		, [params](::gpk::SGeometryBuffers & geometry) { 
			if_fail_fe(gpk::geometryBuildRingFlat(geometry, params));
			return 0;
		});
}

::gpk::err_t			gpk::SEngine::CreateTube		(const ::gpk::SParamsTube & params, ::gpk::vcst_t entityName)	{ 
	const float					radius							= ::gpk::max(params.RadiusYMax.Max, params.RadiusYMin.Max);
	return CreateEntityFromGeometry(entityName, ::gpk::n3f2_t{radius, radius, radius}, true, params, ParamsTube
		, [params](::gpk::SGeometryBuffers & geometry) { 
			if_fail_fe(gpk::geometryBuildRing(geometry, params));
			return 0;
		});
}

::gpk::err_t			gpk::SEngine::CreateImageFromFile(const ::gpk::vcst_t & fileFolder, const ::gpk::vcst_t & fileName) {
	sc_t						filePath	[2048]				= {};
	sprintf_s(filePath, "%s/%s", GPK_VIEW_ARGS(fileFolder).begin(), GPK_VIEW_ARGS(fileName).begin());
	s2_t						index							= Images.Keys.find(filePath);
	if(index != -1)
		return Images.Values[index];

	index					= Scene->Graphics->Surfaces.Create(::gpk::label(filePath));
	::gpk::pobj<::gpk::SSurface>	& surface					= Scene->Graphics->Surfaces[index];
	if_fail_fe(gpk::pngFileLoad(PNGCache, filePath, surface->Data));
	surface->Desc.Dimensions	= PNGCache.Header.Size.u1();
	surface->Desc.BitDepth		= PNGCache.Header.BitDepth;
	surface->Desc.ColorType		= PNGCache.Header.ColorType;
	return index; 
}

//
//	s2_t						iEntity					= CreateBox();
//	::gpk::SEntity		& entity				= Entities[iEntity];
//	::gpk::SRenderNode			& renderNode			= Scene->RenderNodes[entity.RenderNode];
//
//	u2_t					iMesh					= renderNode.Mesh;
//	::gpk::pobj<::gpk::SGeometryMesh>	& mesh			= Scene->Graphics->Meshes[iMesh];
//	Scene->Graphics->Skins[renderNode.Skin]->Material.Color.Diffuse.a = .25f;
//	mesh->GeometrySlices.resize(6);	// one per face
//
//	u2_t					offsetIndex				= 0;
//	for(u2_t iFace = 0; iFace < 6; ++iFace) {
//		u2_t					iSkin					= (u2_t)Scene->Graphics->Skins.Create();
//		::gpk::pobj<::gpk::SSkin>	& skin					= Scene->Graphics->Skins.Elements[iSkin];
//
//		u2_t					iSurface				= (u2_t)Scene->Graphics->Surfaces.Create();
//		::gpk::pobj<::gpk::SSurface>& surface				= Scene->Graphics->Surfaces[iSurface];
//		surface->Desc.ColorType				= ::gpk::COLOR_TYPE_BGRA;
//		surface->Desc.MethodCompression		= 0;
//		surface->Desc.MethodFilter			= 0;
//		surface->Desc.MethodInterlace		= 0;
//		surface->Desc.Dimensions			= {1, 1};
//		surface->Data.resize(1 * sizeof(::gpk::bgra));
//		*(::gpk::bgra*)&surface->Data[0]	= ::gpk::rgba{::gpk::VOXEL_PALETTE[iFace]};
//		skin->Textures.push_back(iSurface);
//
//		skin->Material.Color.Ambient		= ::gpk::bgra(::gpk::ASCII_PALETTE[1 + iFace]);
//		skin->Material.Color.Diffuse		= ::gpk::bgra(::gpk::ASCII_PALETTE[1 + iFace]);
//		skin->Material.Color.Specular		= ::gpk::WHITE;
//		skin->Material.SpecularPower		= 0.5f;
//
//		skin->Material.Color.Ambient		*= .1f;
//
//		::gpk::SGeometrySlice		& slice					= mesh->GeometrySlices[iFace];
//		slice.Slice				= {offsetIndex, 6};
//		offsetIndex				+= slice.Slice.Count;
//
//		s2_t						iFaceEntity				= Entities.Create();
//		u2_t					iFaceRenderNode			= Scene->RenderNodes.Create();
//		::gpk::SEntity		& faceEntity			= Entities[iFaceEntity];
//		::gpk::SRenderNode			& faceRenderNode		= Scene->RenderNodes.RenderNodes[iFaceRenderNode];
//
//		faceRenderNode.Mesh		= iMesh;
//		faceRenderNode.Slice	= iFace;
//		faceRenderNode.Skin		= iSkin;
//		Scene->Graphics->Shaders[faceRenderNode.Shader = Scene->Graphics->Shaders.push_back({})].create(::gpk::psSolid);
//		Scene->Graphics->Shaders.Names[faceRenderNode.Shader] = "psSolid";
//
//		//faceEntity.RigidBody				= Integrator.Create();
//		faceEntity.Parent					= iEntity;
//		faceEntity.RenderNode				= iFaceRenderNode;
//		this->Entities.Names	[iFaceEntity]	= ::gpk::get_value_label((VOXEL_FACE)iFace);
//
//		this->Entities.Children[iEntity]->push_back(iFaceEntity);
//	}
//
//	return iEntity;
//}
