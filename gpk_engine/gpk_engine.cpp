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
	const ::gpk::pau32			& children				= managedEntities.Children[iEntity];
	if(-1 != entity.RenderNode) {
		::gpk::SRenderNodeConstants	& transforms			= renderNodes.Transforms[entity.RenderNode];
		::gpk::m4f32				& worldTransform		= transforms.Model;
		if(-1 == entity.RigidBody)
			worldTransform			= ::gpk::m4f32::GetIdentity();
		else {
			integrator.GetTransform(entity.RigidBody, worldTransform);
		}

		if(-1 != entity.Parent) {
			const ::gpk::SVirtualEntity	& entityParent			= managedEntities.Entities[entity.Parent];
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
		::gpk::m4f32							& worldTransform				= integrator.TransformsLocal[entity.RigidBody];
		integrator.GetTransform(entity.RigidBody, worldTransform);
		if(-1 != entity.Parent) {
			const ::gpk::SVirtualEntity				& entityParent					= managedEntities.Entities[entity.Parent];
			if(-1 != entityParent.RenderNode)
				worldTransform						= worldTransform * renderNodes.Transforms[entityParent.RenderNode].Model;
			else if(-1 != entityParent.RigidBody) {
				worldTransform						= worldTransform * integrator.TransformsLocal[entityParent.RigidBody];
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

::gpk::error_t			gpk::SEngine::CreateRing		()	{ 	
	int32_t						iEntity							= this->Entities.Create();
	return iEntity;
 }
::gpk::error_t			gpk::SEngine::CreateTriangle	()	{ 	
	int32_t						iEntity							= this->Entities.Create();
	return iEntity;
 }

::gpk::error_t			gpk::SEngine::CreateLight(::gpk::LIGHT_TYPE type) {
	int32_t							iEntity				= this->Entities.Create();
	::gpk::SVirtualEntity			& entity			= Entities[iEntity];
	entity.RenderNode			= Scene->RenderNodes.Create();

	uint16_t						indexLight			= (uint16_t)-1;
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

::gpk::error_t			gpk::SEngine::CreateCamera() {
	int32_t						iEntity						= this->Entities.Create();
	::gpk::SVirtualEntity		& entity					= Entities[iEntity];
	entity.RenderNode		= Scene->RenderNodes.Create();
	entity.RigidBody		= this->Integrator.Create();

	::gpk::SCamera				camera;
	camera.Front			= {1, 0, 0};
	camera.Up				= {0, 1, 0};
	camera.Right			= {0, 0, 1};
	camera.Angle			= .25 * ::gpk::math_pi;

	Scene->RenderNodes.Cameras		[entity.RenderNode]->push_back(camera);
	Scene->RenderNodes.Transforms	[entity.RenderNode]		= {};
	return iEntity;
}

::gpk::error_t			gpk::createBuffers	
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

::gpk::error_t			gpk::createBuffers	
	( const ::gpk::SGeometryBuffers		& geometry
	, ::gpk::pobj<::gpk::SRenderBuffer>	& pIndicesVertex
	, ::gpk::pobj<::gpk::SRenderBuffer>	& pVertices
	, ::gpk::pobj<::gpk::SRenderBuffer>	& pNormals
	, ::gpk::pobj<::gpk::SRenderBuffer>	& pUV
	) {
	gpk_necs(::gpk::createBuffers(geometry.Positions.size(), pIndicesVertex, pVertices, pNormals, pUV));

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

::gpk::error_t			gpk::createBuffers
	( ::gpk::SRenderBufferManager	& bufferManager 
	, const ::gpk::SGeometryBuffers	& geometry
	, uint32_t						& iIndicesVertex
	, uint32_t						& iVertices
	, uint32_t						& iNormals
	, uint32_t						& iUV
	) {
	::gpk::pobj<::gpk::SRenderBuffer>	pIndicesVertex, pVertices, pNormals, pUV;
	gpk_necs(::gpk::createBuffers(geometry, pIndicesVertex, pVertices, pNormals, pUV));
	gpk_necs(iIndicesVertex			= (uint32_t)bufferManager.push_back(pIndicesVertex));
	gpk_necs(iVertices				= (uint32_t)bufferManager.push_back(pVertices));
	gpk_necs(iNormals				= (uint32_t)bufferManager.push_back(pNormals));
	gpk_necs(iUV					= (uint32_t)bufferManager.push_back(pUV));
	return 0;
}

::gpk::error_t			gpk::SEngine::CreateBox	(const ::gpk::SParamsBox & params)	{
	int32_t						iEntity					= CreateEntityFromGeometry(::gpk::vcs{"Box"}, params.HalfSizes.f32(), false, params, ParamsBox
		, [params](::gpk::SGeometryBuffers & geometry) { 
			return ::gpk::geometryBuildBox(geometry, params); 
		});
	::gpk::SVirtualEntity		& entity				= Entities[iEntity];
	::gpk::SRenderNode			& renderNode			= Scene->RenderNodes[entity.RenderNode];

	uint32_t					iMesh					= renderNode.Mesh;
	::gpk::pobj<::gpk::SGeometryMesh>	& mesh			= Scene->Graphics->Meshes[iMesh];
	renderNode.Mesh			= (uint32_t)-1;
	mesh->GeometrySlices.resize(6);	// one per face

	uint32_t					offsetIndex				= 0;
	for(uint32_t iFace = 0; iFace < 6; ++iFace) {
		uint32_t					iSkin					= (uint32_t)Scene->Graphics->Skins.Create();
		::gpk::pobj<::gpk::SSkin>	& skin					= Scene->Graphics->Skins.Elements[iSkin];

		uint32_t					iSurface				= (uint32_t)Scene->Graphics->Surfaces.Create();
		::gpk::pobj<::gpk::SSurface>& surface				= Scene->Graphics->Surfaces[iSurface];
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

		::gpk::SGeometrySlice		& slice					= mesh->GeometrySlices[iFace];
		slice.Slice				= {offsetIndex, 6};
		offsetIndex				+= slice.Slice.Count;

		int32_t						iFaceEntity				= Entities.Create();
		uint32_t					iFaceRenderNode			= Scene->RenderNodes.Create();
		::gpk::SVirtualEntity		& faceEntity			= Entities[iFaceEntity];
		::gpk::SRenderNode			& faceRenderNode		= Scene->RenderNodes.RenderNodes[iFaceRenderNode];

		faceRenderNode.Mesh		= iMesh;
		faceRenderNode.Slice	= iFace;
		faceRenderNode.Skin		= iSkin;
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

::gpk::error_t			gpk::SEngine::CreateSphere		(const ::gpk::SParamsSphere & params)	{ 
	return CreateEntityFromGeometry(::gpk::vcs{"Sphere"}, ::gpk::n3f32{params.Radius, params.Radius, params.Radius}, true, params, ParamsSphere
		, [params](::gpk::SGeometryBuffers & geometry) { 
			return ::gpk::geometryBuildSphere(geometry, params); 
		});
}

::gpk::error_t			gpk::SEngine::CreateGrid		(const ::gpk::SParamsGrid & params)	{ 
	stacxpr	float				radius							= .5f;
	return CreateEntityFromGeometry(::gpk::vcs{"Grid"}, ::gpk::n3f32{radius, radius, radius}, true, params, ParamsGrid
		, [params](::gpk::SGeometryBuffers & geometry) {
			gpk_necs(::gpk::geometryBuildGrid(geometry, params));
			return 0;
		});
}

::gpk::error_t			gpk::SEngine::CreateHelixHalf	(const ::gpk::SParamsHelix & params)	{ 
	stacxpr	float				radius							= .5f;
	return CreateEntityFromGeometry(::gpk::vcs{"Helix half"}, ::gpk::n3f32{radius, radius, radius}, true, params, ParamsHelix
		, [params](::gpk::SGeometryBuffers & geometry) {
			gpk_necs(::gpk::geometryBuildHelixHalf(geometry, params));
			return 0;
		});
}

::gpk::error_t			gpk::SEngine::CreateHelix		(const ::gpk::SParamsHelix & params)	{ 
	const float					radius							= ::gpk::max(params.Radius.Min, params.Radius.Max);
	return CreateEntityFromGeometry(::gpk::vcs{"Helix"}, ::gpk::n3f32{radius, radius, radius}, true, params, ParamsHelix
		, [params](::gpk::SGeometryBuffers & geometry) {
			gpk_necs(::gpk::geometryBuildHelix(geometry, params));
			return 0;
		});
}

::gpk::error_t			gpk::SEngine::CreateFigure0		(const ::gpk::SParamsHelix & params)	{ 
	const float					radius							= ::gpk::max(params.Radius.Min, params.Radius.Max);
	return CreateEntityFromGeometry(::gpk::vcs{"Figure0"}, ::gpk::n3f32{radius, radius, radius}, true, params, ParamsHelix
		, [params](::gpk::SGeometryBuffers & geometry) {
			gpk_necs(::gpk::geometryBuildHelix(geometry, params));
			return 0;
		});
}

::gpk::error_t			gpk::SEngine::CreateCylinder	(const ::gpk::SParamsCylinder & params)	{ 
	const float					radius							= ::gpk::max(params.Radius.Min, params.Radius.Max);
	return CreateEntityFromGeometry(::gpk::vcs{"Cylinder"}, ::gpk::n3f32{radius, radius, radius}, true, params, ParamsCylinder
		, [params](::gpk::SGeometryBuffers & geometry) { 
			gpk_necs(::gpk::geometryBuildCylinder(geometry, params));
			return 0;
		});
}

::gpk::error_t			gpk::SEngine::CreateCircle		(const ::gpk::SParamsCircle & params)	{ 
	const float					radius							= params.Radius;
	return CreateEntityFromGeometry(::gpk::vcs{"Circle"}, ::gpk::n3f32{radius, radius, radius}, true, params, ParamsCircle
		, [params](::gpk::SGeometryBuffers & geometry) { 
			gpk_necs(::gpk::geometryBuildCircle(geometry, params));
			return 0;
		});
}
::gpk::error_t			gpk::SEngine::CreateRingFlat	(const ::gpk::SParamsRing & params)	{ 
	const float					radius							= params.Radius.Max;
	return CreateEntityFromGeometry(::gpk::vcs{"Ring"}, ::gpk::n3f32{radius, radius, radius}, true, params, ParamsRing
		, [params](::gpk::SGeometryBuffers & geometry) { 
			gpk_necs(::gpk::geometryBuildRingFlat(geometry, params));
			return 0;
		});
}

::gpk::error_t			gpk::SEngine::CreateImageFromFile(const ::gpk::vcs & fileFolder, const ::gpk::vcs & fileName) {
	char						filePath	[2048]	= {};
	sprintf_s(filePath, "%s/%s", ::gpk::toString(fileFolder).begin(), ::gpk::toString(fileName).begin());
	int32_t						index				= Images.Keys.find(filePath);
	if(index != -1)
		return Images.Values[index];

	index					= Scene->Graphics->Surfaces.Create(::gpk::label(filePath));
	::gpk::pobj<::gpk::SSurface>	& surface			= Scene->Graphics->Surfaces[index];
	gpk_necs(::gpk::pngFileLoad(PNGCache, filePath, surface->Data));
	surface->Desc.Dimensions	= PNGCache.Header.Size.u16();
	surface->Desc.BitDepth		= PNGCache.Header.BitDepth;
	surface->Desc.ColorType		= PNGCache.Header.ColorType;
	return index; 
}