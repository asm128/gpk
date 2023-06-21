#include "gpk_engine.h"
#include "gpk_voxel.h"
#include "gpk_ascii_color.h"
#include "gpk_geometry_lh.h"
#include "gpk_engine_shader.h"

::gpk::error_t			gpk::updateEntityTransforms			
	( uint32_t								iEntity
	, const ::gpk::SVirtualEntity			& entity
	, const ::gpk::SVirtualEntityManager	& managedEntities	
	, ::gpk::SRigidBodyIntegrator			& integrator		
	, ::gpk::SRenderNodeManager				& renderNodes
	)
{
	const ::gpk::pobj<::gpk::au32>	& children						= managedEntities.Children[iEntity];
	if(-1 != entity.RenderNode) {
		::gpk::SRenderNodeTransforms				& transforms					= renderNodes.Transforms[entity.RenderNode];
		::gpk::m4f32								& worldTransform				= transforms.World;
		if(-1 == entity.RigidBody)
			worldTransform							= ::gpk::m4f32::GetIdentity();
		else {
			integrator.GetTransform(entity.RigidBody, worldTransform);
		}

		worldTransform = renderNodes.BaseTransforms[entity.RenderNode].World * worldTransform;

		if(-1 != entity.Parent) {
			const ::gpk::SVirtualEntity					& entityParent					= managedEntities.Entities[entity.Parent];
			if(-1 != entityParent.RenderNode)
				worldTransform							= worldTransform * renderNodes.Transforms[entityParent.RenderNode].World;
			else if(-1 != entityParent.RigidBody) {
				worldTransform							= worldTransform * integrator.TransformsLocal[entityParent.RigidBody];
			}
		}
		transforms.WorldInverse					= worldTransform.GetInverse();
		transforms.WorldInverseTranspose		= transforms.WorldInverse.GetTranspose();
	}
	else if(-1 != entity.RigidBody) {
		::gpk::m4f32							& worldTransform				= integrator.TransformsLocal[entity.RigidBody];
		integrator.GetTransform(entity.RigidBody, worldTransform);
		if(-1 != entity.Parent) {
			const ::gpk::SVirtualEntity					& entityParent					= managedEntities.Entities[entity.Parent];
			if(-1 != entityParent.RenderNode)
				worldTransform							= worldTransform * renderNodes.Transforms[entityParent.RenderNode].World;
			else if(-1 != entityParent.RigidBody) {
				worldTransform							= worldTransform * integrator.TransformsLocal[entityParent.RigidBody];
			}
		}
	}
	if(children) {
		for(uint32_t iChild = 0; iChild < children->size(); ++iChild) {
			const uint32_t								iChildEntity					= (*children)[iChild];
			::gpk::updateEntityTransforms(iChildEntity, managedEntities.Entities[iChildEntity], managedEntities, integrator, renderNodes);
		}
	}

	return 0;
}

::gpk::error_t			gpk::SEngine::CreateLight			(::gpk::LIGHT_TYPE type) {
	int32_t									iEntity								= this->Entities.Create();
	::gpk::SVirtualEntity					& entity							= Entities[iEntity];
	entity.RenderNode					= Scene->RenderNodes.Create();

	uint16_t								indexLight							= (uint16_t)-1;
	switch(type) {
	case ::gpk::LIGHT_TYPE_Directional	: indexLight = (uint16_t)Scene->RenderNodes.LightsDirectional	.push_back({}); break;
	case ::gpk::LIGHT_TYPE_Point		: indexLight = (uint16_t)Scene->RenderNodes.LightsPoint			.push_back({}); break;
	case ::gpk::LIGHT_TYPE_Spot			: indexLight = (uint16_t)Scene->RenderNodes.LightsSpot			.push_back({}); break;
	default:
		error_printf("Invalid light type: %i", (int)type);
		return -1;
	}
	Scene->RenderNodes.Lights		[entity.RenderNode]->push_back({type, indexLight});
	Scene->RenderNodes.Transforms	[entity.RenderNode] = {};
	return iEntity;
}

::gpk::error_t			gpk::SEngine::CreateCamera			() {
	int32_t									iEntity								= this->Entities.Create();
	::gpk::SVirtualEntity					& entity							= Entities[iEntity];
	entity.RenderNode					= Scene->RenderNodes.Create();
	entity.RigidBody					= this->Integrator.Create();

	::gpk::SCamera							camera;
	camera.Angle						= .25 * ::gpk::math_pi;
	camera.Front						= {1, 0, 0};
	camera.Up							= {0, 1, 0};
	camera.Right						= {0, 0, 1};

	Scene->RenderNodes.Cameras		[entity.RenderNode]->push_back(camera);
	Scene->RenderNodes.Transforms	[entity.RenderNode]					= {};
	return iEntity;
}

static ::gpk::error_t	createBuffers	
	( uint32_t							vertexCount
	, ::gpk::pobj<::gpk::SRenderBuffer>	& pIndicesVertex
	, ::gpk::pobj<::gpk::SRenderBuffer>	& pVertices
	, ::gpk::pobj<::gpk::SRenderBuffer>	& pNormals
	, ::gpk::pobj<::gpk::SRenderBuffer>	& pUV
	) {
	::gpk::clear(pIndicesVertex, pVertices, pNormals, pUV);
	pVertices	->Desc.Format	= ::gpk::DATA_TYPE_FLOAT32_3;
	pVertices	->Desc.Usage	= ::gpk::BUFFER_USAGE_Position;

	pNormals	->Desc.Format	= ::gpk::DATA_TYPE_FLOAT32_3;
	pNormals	->Desc.Usage	= ::gpk::BUFFER_USAGE_Normal;

	pUV			->Desc.Format	= ::gpk::DATA_TYPE_FLOAT32_2;
	pUV			->Desc.Usage	= ::gpk::BUFFER_USAGE_UV;

	pIndicesVertex->Desc.Usage	= ::gpk::BUFFER_USAGE_Index;
	pIndicesVertex->Desc.Format	
		= (vertexCount > 0xFFFF) ? ::gpk::DATA_TYPE_UINT32
		: (vertexCount > 0x00FF) ? ::gpk::DATA_TYPE_UINT16
		: ::gpk::DATA_TYPE_UINT8
		;
	return 0;
}

static ::gpk::error_t	createBuffers	
	( const ::gpk::STrianglesIndexed	& geometry
	, ::gpk::pobj<::gpk::SRenderBuffer>	& pIndicesVertex
	, ::gpk::pobj<::gpk::SRenderBuffer>	& pVertices
	, ::gpk::pobj<::gpk::SRenderBuffer>	& pNormals
	, ::gpk::pobj<::gpk::SRenderBuffer>	& pUV
	) {
	gpk_necs(::createBuffers(geometry.Positions.size(), pIndicesVertex, pVertices, pNormals, pUV));

	pVertices	->Data.resize(geometry.Positions	.byte_count());
	pNormals	->Data.resize(geometry.Normals		.byte_count());
	pUV			->Data.resize(geometry.TextureCoords.byte_count());
	memcpy(&pVertices	->Data[0], geometry.Positions		.begin(), pVertices	->Data.size());
	memcpy(&pNormals	->Data[0], geometry.Normals			.begin(), pNormals	->Data.size());
	memcpy(&pUV			->Data[0], geometry.TextureCoords	.begin(), pUV		->Data.size());
	if(geometry.Positions.size() > 0xFFFF) {
		pIndicesVertex->Data.resize(geometry.PositionIndices.byte_count());
		memcpy(pIndicesVertex->Data.begin(), geometry.PositionIndices.begin(), pIndicesVertex->Data.size());
	}
	else if(geometry.Positions.size() > 0xFF) {
		pIndicesVertex->Data.resize(geometry.PositionIndices.byte_count() >> 1);
		::gpk::vu16					viewIndices					= ::gpk::vu16{(uint16_t*)pIndicesVertex->Data.begin(), geometry.PositionIndices.size()};
		viewIndices.enumerate([&geometry](uint32_t index, uint16_t & value){ value = (uint16_t)geometry.PositionIndices[index]; }, 0);
	}
	else {
		pIndicesVertex->Data.resize(geometry.PositionIndices.byte_count() >> 2);
		::gpk::vu8					viewIndices					= ::gpk::vu8{(uint8_t*)pIndicesVertex->Data.begin(), geometry.PositionIndices.size()};
		viewIndices.enumerate([&geometry](uint32_t index, uint8_t & value){ value = (uint8_t)geometry.PositionIndices[index]; }, 0);
	}

	return 0;
}

static ::gpk::error_t	createBuffers
	( ::gpk::SRenderBufferManager		& bufferManager 
	, const ::gpk::STrianglesIndexed	& geometry
	, uint32_t							& iIndicesVertex
	, uint32_t							& iVertices
	, uint32_t							& iNormals
	, uint32_t							& iUV
	) {
	::gpk::pobj<::gpk::SRenderBuffer>	pIndicesVertex, pVertices, pNormals, pUV;
	gpk_necs(::createBuffers(geometry, pIndicesVertex, pVertices, pNormals, pUV));
	gpk_necs(iIndicesVertex			= (uint32_t)bufferManager.push_back(pIndicesVertex));
	gpk_necs(iVertices				= (uint32_t)bufferManager.push_back(pVertices));
	gpk_necs(iNormals				= (uint32_t)bufferManager.push_back(pNormals));
	gpk_necs(iUV					= (uint32_t)bufferManager.push_back(pUV));
	return 0;
}

static ::gpk::error_t	createRenderNode
	( ::gpk::SEngineScene				& scene
	, const ::gpk::STrianglesIndexed	& geometry
	, const ::gpk::vcc					name 
	, bool								createSkin
	) {
	uint32_t					iIndicesVertex			= 0;
	uint32_t					iVertices				= 0;
	uint32_t					iNormals				= 0;
	uint32_t					iUV						= 0;
	::createBuffers(scene.Graphics->Buffers, geometry, iIndicesVertex, iVertices, iNormals, iUV);

	uint32_t					iMesh					= (uint32_t)scene.Graphics->Meshes.Create();
	::gpk::pobj<::gpk::SGeometryMesh>	& mesh			= scene.Graphics->Meshes[iMesh];
	scene.Graphics->Meshes.Names[iMesh] = name;
	mesh->GeometryBuffers.append({iIndicesVertex, iVertices, iNormals, iUV});

	mesh->Desc.Mode			= ::gpk::MESH_MODE_List;
	mesh->Desc.Type			= ::gpk::GEOMETRY_TYPE_Triangle;
	mesh->Desc.NormalMode	= ::gpk::NORMAL_MODE_Point;


	uint32_t					iRenderNode				= scene.RenderNodes.Create();
	::gpk::SRenderNode			& renderNode			= scene.RenderNodes.RenderNodes[iRenderNode];
	renderNode.Mesh			= iMesh;
	if(createSkin) {
		uint32_t					iSkin					= (uint32_t)scene.Graphics->Skins.Create();
		::gpk::pobj<::gpk::SSkin>	& skin					= scene.Graphics->Skins.Elements[iSkin];
		skin->Material.Color.Ambient	= ::gpk::bgra(::gpk::ASCII_PALETTE[3]) * .1f;
		skin->Material.Color.Diffuse	= ::gpk::bgra(::gpk::ASCII_PALETTE[3]);
		skin->Material.Color.Specular	= ::gpk::WHITE;
		skin->Material.SpecularPower	= 0.5f;

		uint32_t					iSurface				= (uint32_t)scene.Graphics->Surfaces.Create();
		skin->Textures.push_back(iSurface);

		skin->Material.Color.Ambient	*= .1f;

		::gpk::pobj<::gpk::SSurface>	& surface			= scene.Graphics->Surfaces[iSurface];
		surface->Desc.ColorType			= ::gpk::COLOR_TYPE_BGRA;
		surface->Desc.MethodCompression	= 0;
		surface->Desc.MethodFilter		= 0;
		surface->Desc.MethodInterlace	= 0;
		surface->Desc.Dimensions		= {32, 32};
		surface->Data.resize(surface->Desc.Dimensions.Area() * sizeof(::gpk::bgra));
		memset(surface->Data.begin(), 0xFF, surface->Data.size());
		::gpk::g8bgra	view					= {(::gpk::bgra*)surface->Data.begin(), surface->Desc.Dimensions.u32()};
		::gpk::rgba					color					= {::gpk::ASCII_PALETTE[rand() % 16]};
		for(uint32_t y = surface->Desc.Dimensions.y / 3; y < surface->Desc.Dimensions.y / 3U * 2U; ++y)
		for(uint32_t x = 0; x < surface->Desc.Dimensions.x; ++x)
			view[y][x]	= color;


		mesh->GeometrySlices.resize(1);	// 

		::gpk::SGeometrySlice		& slice					= mesh->GeometrySlices[0];
		slice.Slice				= {0, geometry.PositionIndices.size()};

		renderNode.Skin			= iSkin;
		renderNode.Slice		= 0;
		scene.Graphics->Shaders[renderNode.Shader = scene.Graphics->Shaders.push_back({})].create(::gpk::psSolid);
		scene.Graphics->Shaders.Names[renderNode.Shader] = "psSolid";
	}
	return iRenderNode;
}

typedef std::function<::gpk::error_t(::gpk::STrianglesIndexed&)> TGeometryFunc;

template<typename _tParams>
static ::gpk::error_t	createEntityFromGeometry
	( ::gpk::SEngine							& engine
	, const ::gpk::vcc							name		
	, const ::gpk::n3f32							halfSizes
	, bool										createSkin
	, const _tParams							& params
	, ::gpk::SLinearPODMap<_tParams, uint32_t>	& recycleRenderNodeMap
	, const TGeometryFunc						& funcGeometry
	) {
	int32_t						iEntity					= engine.Entities.Create();
	engine.Entities.Names[iEntity]	= name;

	::gpk::SVirtualEntity		& entity				= engine.Entities[iEntity];
	engine.Integrator.BoundingVolumes[entity.RigidBody = engine.Integrator.Create()].HalfSizes = halfSizes;

	int32_t						reuse					= recycleRenderNodeMap.Keys.find([params](const _tParams & value) { return params == value; }, 0);
	if(recycleRenderNodeMap.size() > (uint32_t)reuse)
		entity.RenderNode		= engine.Scene->Clone(recycleRenderNodeMap.Values[reuse], false, false, false);
	else {
		::gpk::STrianglesIndexed	geometry;
		gpk_necs(funcGeometry(geometry));
		if(!engine.Scene)
			engine.Scene.create();
		entity.RenderNode		= ::createRenderNode(*engine.Scene, geometry, name, createSkin);
		recycleRenderNodeMap.push_back(params, entity.RenderNode);
	}

	return iEntity;
};

::gpk::error_t			gpk::SEngine::CreateBox	(const ::gpk::SParamsBox & params)	{
	int32_t						iEntity					= ::createEntityFromGeometry(*this, ::gpk::vcs{"Box"}, params.HalfSizes.f32(), false, params, ParamsBox
		, [params](::gpk::STrianglesIndexed & geometry) { 
			return ::gpk::geometryBuildBox(geometry, params); 
		});
	::gpk::SVirtualEntity		& entity				= Entities[iEntity];
	::gpk::SRenderNode			& renderNode			= Scene->RenderNodes[entity.RenderNode];

	uint32_t					iMesh					= renderNode.Mesh;
	::gpk::pobj<::gpk::SGeometryMesh>	& mesh			= Scene->Graphics->Meshes[iMesh];
	renderNode.Mesh			= (uint32_t)-1;
	mesh->GeometrySlices.resize(6);	// one per face

	uint32_t								offsetIndex				= 0;
	for(uint32_t iFace = 0; iFace < 6; ++iFace) {
		uint32_t								iSkin					= (uint32_t)Scene->Graphics->Skins.Create();
		::gpk::pobj<::gpk::SSkin>				& skin					= Scene->Graphics->Skins.Elements[iSkin];

		uint32_t								iSurface				= (uint32_t)Scene->Graphics->Surfaces.Create();
		::gpk::pobj<::gpk::SSurface>			& surface				= Scene->Graphics->Surfaces[iSurface];
		surface->Desc.ColorType				= ::gpk::COLOR_TYPE_BGRA;
		surface->Desc.MethodCompression		= 0;
		surface->Desc.MethodFilter			= 0;
		surface->Desc.MethodInterlace		= 0;
		surface->Desc.Dimensions			= {1, 1};
		surface->Data.resize(1 * sizeof(::gpk::bgra));
		*(::gpk::bgra*)&surface->Data[0]	= ::gpk::rgba{::gpk::VOXEL_PALETTE[iFace]};
		skin->Textures.push_back(iSurface);

		skin->Material.Color.Ambient		= ::gpk::bgra(::gpk::ASCII_PALETTE[1 + iFace]);
		skin->Material.Color.Diffuse		= ::gpk::bgra(::gpk::ASCII_PALETTE[1 + iFace]);
		skin->Material.Color.Specular		= ::gpk::WHITE;
		skin->Material.SpecularPower		= 0.5f;

		skin->Material.Color.Ambient		*= .1f;

		::gpk::SGeometrySlice					& slice					= mesh->GeometrySlices[iFace];
		slice.Slice							= {offsetIndex, 6};
		offsetIndex							+= slice.Slice.Count;

		int32_t									iFaceEntity				= Entities.Create();
		uint32_t								iFaceRenderNode			= Scene->RenderNodes.Create();
		::gpk::SVirtualEntity					& faceEntity			= Entities[iFaceEntity];
		::gpk::SRenderNode						& faceRenderNode		= Scene->RenderNodes.RenderNodes[iFaceRenderNode];

		faceRenderNode.Mesh					= iMesh;
		faceRenderNode.Slice				= iFace;
		faceRenderNode.Skin					= iSkin;
		Scene->Graphics->Shaders[faceRenderNode.Shader = Scene->Graphics->Shaders.push_back({})].create(::gpk::psSolid);
		Scene->Graphics->Shaders.Names[faceRenderNode.Shader] = "psSolid";

		//faceEntity.RigidBody				= Integrator.Create();
		faceEntity.Parent					= iEntity;
		faceEntity.RenderNode				= iFaceRenderNode;
		this->Entities.Names	[iFaceEntity]	= ::gpk::get_value_label((VOXEL_FACE)iFace);

		this->Entities.Children[iEntity]->push_back(iFaceEntity);
	}

	return iEntity;
}


::gpk::error_t			gpk::SEngine::CreateSphere(const SParamsSphere & params)	{ 
	return ::createEntityFromGeometry(*this, ::gpk::vcs{"Sphere"}, ::gpk::n3f32{params.Radius, params.Radius, params.Radius}, true, params, ParamsSphere
		, [params](::gpk::STrianglesIndexed & geometry) { 
			return ::gpk::geometryBuildSphere(geometry, params); 
		});
}

::gpk::error_t			gpk::SEngine::CreateGrid(const SParamsGrid & params)	{ 
	stacxpr	float				radius					= .5f;
	return ::createEntityFromGeometry(*this, ::gpk::vcs{"Grid"}, ::gpk::n3f32{radius, radius, radius}, true, params, ParamsGrid
		, [params](::gpk::STrianglesIndexed & geometry) {
			gpk_necs(::gpk::geometryBuildGrid(geometry, params));
			return 0;
		});
}

::gpk::error_t			gpk::SEngine::CreateCylinder(const SParamsCylinder & params)	{ 
	const	float				radius					= ::gpk::max(params.Radius.Min, params.Radius.Max);
	return ::createEntityFromGeometry(*this, ::gpk::vcs{"Cylinder"}, ::gpk::n3f32{radius, radius, radius}, true, params, ParamsCylinder
		, [params](::gpk::STrianglesIndexed & geometry) { 
			gpk_necs(::gpk::geometryBuildCylinder(geometry, params));
			return 0;
		});
}


::gpk::error_t			gpk::SEngine::CreateCircle			()	{ 	
	int32_t						iEntity								= this->Entities.Create();
	return iEntity;
 }
::gpk::error_t			gpk::SEngine::CreateRing			()	{ 	
	int32_t						iEntity								= this->Entities.Create();
	return iEntity;
 }
::gpk::error_t			gpk::SEngine::CreateTriangle		()	{ 	
	int32_t						iEntity								= this->Entities.Create();
	return iEntity;
 }

 