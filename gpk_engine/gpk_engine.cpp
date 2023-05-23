#include "gpk_engine.h"
#include "gpk_voxel.h"
#include "gpk_ascii_color.h"
#include "gpk_geometry_lh.h"
#include "gpk_engine_shader.h"

int									gpk::updateEntityTransforms			
	( uint32_t								iEntity
	, const ::gpk::SVirtualEntity			& entity
	, const ::gpk::SVirtualEntityManager	& managedEntities	
	, ::gpk::SRigidBodyIntegrator			& integrator		
	, ::gpk::SRenderNodeManager				& renderNodes
	)
{
	const ::gpk::pobj<::gpk::apod<uint32_t>>	& children						= managedEntities.Children[iEntity];
	if(-1 != entity.RenderNode) {
		::gpk::SRenderNodeTransforms				& transforms					= renderNodes.Transforms[entity.RenderNode];
		::gpk::m4<float>							& worldTransform				= transforms.World;
		if(-1 == entity.RigidBody)
			worldTransform							= ::gpk::SMatrix4<float>::GetIdentity();
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
		::gpk::m4<float>							& worldTransform				= integrator.TransformsLocal[entity.RigidBody];
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

::gpk::error_t						gpk::SEngine::CreateLight			(::gpk::LIGHT_TYPE type) {
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

::gpk::error_t						gpk::SEngine::CreateCamera			() {
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

::gpk::error_t						gpk::SEngine::CreateBox				()	{
	int32_t									iEntity								= this->Entities.Create();
	Entities.Names[iEntity]		= ::gpk::vcs{"Box"};
	::gpk::SVirtualEntity					& entity							= Entities[iEntity];
	entity.RenderNode					= Scene->RenderNodes.Create();
	entity.RigidBody					= this->Integrator.Create();

	::gpk::pobj<::gpk::SRenderBuffer>		pIndicesVertex;
	::gpk::pobj<::gpk::SRenderBuffer>		pVertices;
	::gpk::pobj<::gpk::SRenderBuffer>		pNormals;
	::gpk::pobj<::gpk::SRenderBuffer>		pUV;
	pIndicesVertex	->Desc.Format		= ::gpk::DATA_TYPE_UINT16;
	pIndicesVertex	->Desc.Usage		= ::gpk::BUFFER_USAGE_Index;

	pVertices		->Desc.Format		= ::gpk::DATA_TYPE_FLOAT32_3;
	pVertices		->Desc.Usage		= ::gpk::BUFFER_USAGE_Position;

	pNormals		->Desc.Format		= ::gpk::DATA_TYPE_FLOAT32_3;
	pNormals		->Desc.Usage		= ::gpk::BUFFER_USAGE_Normal;

	pUV				->Desc.Format		= ::gpk::DATA_TYPE_FLOAT32_2;
	pUV				->Desc.Usage		= ::gpk::BUFFER_USAGE_UV;


	uint32_t								iVertices				= (uint32_t)Scene->Graphics->Buffers.push_back(pVertices);
	uint32_t								iNormals				= (uint32_t)Scene->Graphics->Buffers.push_back(pNormals);
	uint32_t								iUV						= (uint32_t)Scene->Graphics->Buffers.push_back(pUV);
	uint32_t								iIndicesVertex			= (uint32_t)Scene->Graphics->Buffers.push_back(pIndicesVertex);

	uint32_t								iMesh					= (uint32_t)Scene->Graphics->Meshes.Create();
	Scene->Graphics->Meshes.Names[iMesh]	= ::gpk::vcs{"Box"};
	::gpk::pobj<::gpk::SGeometryMesh>		& mesh					= Scene->Graphics->Meshes[iMesh];

	mesh->GeometryBuffers.append({iIndicesVertex, iVertices, iNormals, iUV});

	mesh->Desc.Mode						= ::gpk::MESH_MODE_List;
	mesh->Desc.Type						= ::gpk::GEOMETRY_TYPE_Triangle;
	mesh->Desc.NormalMode				= ::gpk::NORMAL_MODE_Point;
	mesh->GeometrySlices.resize(6);	// one per face

	pVertices		->Data.resize(24 * sizeof(::gpk::n3f));
	pNormals		->Data.resize(24 * sizeof(::gpk::n3f));
	pUV				->Data.resize(24 * sizeof(::gpk::n2f));
	pIndicesVertex	->Data.resize(12 * sizeof(::gpk::tri<uint16_t>));
	memcpy(&pVertices		->Data[0], ::gpk::VOXEL_FACE_VERTICES	, pVertices			->Data.size());
	memcpy(&pIndicesVertex	->Data[0], ::gpk::VOXEL_FACE_INDICES_16	, pIndicesVertex	->Data.size());
	memcpy(&pNormals		->Data[0], ::gpk::VOXEL_FACE_NORMALS	, pNormals			->Data.size());
	memcpy(&pUV				->Data[0], ::gpk::VOXEL_FACE_UV			, pUV				->Data.size());

	::gpk::view<::gpk::n3f>					viewPositions			= {(::gpk::n3f*)pVertices->Data.begin(), pVertices->Data.size() / sizeof(::gpk::n3f)};
	for(uint32_t index = 0; index < viewPositions.size(); ++index) {
		viewPositions[index]				-= {.5f, .5f, .5f};
	}

	uint32_t								offsetIndex				= 0;
	for(uint32_t iFace = 0; iFace < 6; ++iFace) {
		uint32_t								iSkin					= (uint32_t)Scene->Graphics->Skins.Create();
		uint32_t								iSurface				= (uint32_t)Scene->Graphics->Surfaces.Create();
		::gpk::pobj<::gpk::SSkin>				& skin					= Scene->Graphics->Skins.Elements[iSkin];
		skin->Textures.push_back(iSurface);
		skin->Material.Color.Ambient		= ::gpk::bgra(::gpk::ASCII_PALETTE[1 + iFace]);
		skin->Material.Color.Diffuse		= ::gpk::bgra(::gpk::ASCII_PALETTE[1 + iFace]);
		skin->Material.Color.Specular		= ::gpk::WHITE;
		skin->Material.SpecularPower		= 0.5f;

		skin->Material.Color.Ambient		*= .1f;

		::gpk::pobj<::gpk::SSurface>			& surface				= Scene->Graphics->Surfaces[iSurface];
		surface->Desc.ColorType				= ::gpk::COLOR_TYPE_BGRA;
		surface->Desc.MethodCompression		= 0;
		surface->Desc.MethodFilter			= 0;
		surface->Desc.MethodInterlace		= 0;
		surface->Desc.Dimensions			= {1, 1};
		surface->Data.resize(1 * sizeof(::gpk::bgra));
		*(::gpk::bgra*)&surface->Data[0]	= ::gpk::rgba{::gpk::VOXEL_PALETTE[iFace]};

		::gpk::SGeometrySlice					& slice					= mesh->GeometrySlices[iFace];
		slice.Slice							= {offsetIndex, 6};
		offsetIndex							+= slice.Slice.Count;

		int32_t									iFaceEntity				= this->Entities.Create();
		uint32_t								iFaceRenderNode			= Scene->RenderNodes.Create();
		::gpk::SVirtualEntity					& faceEntity			= this->Entities[iFaceEntity];
		::gpk::SRenderNode						& faceRenderNode		= this->Scene->RenderNodes.RenderNodes[iFaceRenderNode];

		faceRenderNode.Mesh					= iMesh;
		faceRenderNode.Slice				= iFace;
		faceRenderNode.Skin					= iSkin;
		Scene->Graphics->Shaders[faceRenderNode.Shader = Scene->Graphics->Shaders.push_back({})].create(::gpk::psSolid);

		//faceEntity.RigidBody				= Integrator.Create();
		faceEntity.Parent					= iEntity;
		faceEntity.RenderNode				= iFaceRenderNode;
		this->Entities.Names	[iFaceEntity]	= ::gpk::get_value_label((VOXEL_FACE)iFace);

		this->Entities.Children[iEntity]->push_back(iFaceEntity);
	}

	return iEntity;
}

::gpk::error_t						gpk::SEngine::CreateSphere			()	{ 
	::gpk::SGeometryIndexedTriangles		geometry;
	::gpk::geometryBuildSphere(geometry, 24, 24, .5f, {});

	int32_t									iEntity								= this->Entities.Create();
	Entities.Names[iEntity]		= ::gpk::vcs{"Sphere"};
	::gpk::SVirtualEntity					& entity							= Entities[iEntity];
	entity.RenderNode					= Scene->RenderNodes.Create();;
	Integrator.BoundingVolumes[entity.RigidBody = this->Integrator.Create()].HalfSizes = {0.5f, 0.5f, 0.5f};

	::gpk::pobj<::gpk::SRenderBuffer>		pIndicesVertex;
	::gpk::pobj<::gpk::SRenderBuffer>		pVertices;
	::gpk::pobj<::gpk::SRenderBuffer>		pNormals;
	::gpk::pobj<::gpk::SRenderBuffer>		pUV;

	pIndicesVertex	->Desc.Format		= ::gpk::DATA_TYPE_UINT16;
	pIndicesVertex	->Desc.Usage		= ::gpk::BUFFER_USAGE_Index;

	pVertices		->Desc.Format		= ::gpk::DATA_TYPE_FLOAT32_3;
	pVertices		->Desc.Usage		= ::gpk::BUFFER_USAGE_Position;

	pNormals		->Desc.Format		= ::gpk::DATA_TYPE_FLOAT32_3;
	pNormals		->Desc.Usage		= ::gpk::BUFFER_USAGE_Normal;

	pUV				->Desc.Format		= ::gpk::DATA_TYPE_FLOAT32_2;
	pUV				->Desc.Usage		= ::gpk::BUFFER_USAGE_UV;

	pIndicesVertex	->Data.resize(geometry.PositionIndices	.byte_count() / 2);
	::gpk::view<uint16_t>					viewIndices							= {(uint16_t*)pIndicesVertex->Data.begin(), geometry.PositionIndices.size()};
	for(uint32_t index = 0; index < geometry.PositionIndices.size(); ++index) {
		viewIndices[index] = (uint16_t)geometry.PositionIndices[index];
	}
	//memcpy(&pIndicesVertex	->Data[0], geometry.PositionIndices	.begin(), pIndicesVertex	->Data.size());

	pVertices	->Data.resize(geometry.Positions		.byte_count());
	pNormals	->Data.resize(geometry.Normals			.byte_count());
	pUV			->Data.resize(geometry.TextureCoords	.byte_count());
	memcpy(&pVertices	->Data[0], geometry.Positions		.begin(), pVertices	->Data.size());
	memcpy(&pNormals	->Data[0], geometry.Normals			.begin(), pNormals	->Data.size());
	memcpy(&pUV			->Data[0], geometry.TextureCoords	.begin(), pUV		->Data.size());

	uint32_t								iVertices				= (uint32_t)Scene->Graphics->Buffers.push_back(pVertices);
	uint32_t								iNormals				= (uint32_t)Scene->Graphics->Buffers.push_back(pNormals);
	uint32_t								iUV						= (uint32_t)Scene->Graphics->Buffers.push_back(pUV);
	uint32_t								iIndicesVertex			= (uint32_t)Scene->Graphics->Buffers.push_back(pIndicesVertex);

	uint32_t								iMesh					= (uint32_t)Scene->Graphics->Meshes.Create();
	::gpk::pobj<::gpk::SGeometryMesh>		& mesh					= Scene->Graphics->Meshes[iMesh];
	Scene->Graphics->Meshes.Names[iMesh]= ::gpk::vcs{"Sphere"};
	mesh->GeometryBuffers.append({iIndicesVertex, iVertices, iNormals, iUV});

	mesh->Desc.Mode						= ::gpk::MESH_MODE_List;
	mesh->Desc.Type						= ::gpk::GEOMETRY_TYPE_Triangle;
	mesh->Desc.NormalMode				= ::gpk::NORMAL_MODE_Point;

	uint32_t								iSkin					= (uint32_t)Scene->Graphics->Skins.Create();
	::gpk::pobj<::gpk::SSkin>			& skin					= Scene->Graphics->Skins.Elements[iSkin];
	skin->Material.Color.Ambient		= ::gpk::bgra(::gpk::ASCII_PALETTE[3]) * .1f;
	skin->Material.Color.Diffuse		= ::gpk::bgra(::gpk::ASCII_PALETTE[3]);
	skin->Material.Color.Specular		= ::gpk::WHITE;
	skin->Material.SpecularPower		= 0.5f;

	uint32_t								iSurface				= (uint32_t)Scene->Graphics->Surfaces.Create();
	skin->Textures.push_back(iSurface);

	skin->Material.Color.Ambient		*= .1f;

	::gpk::pobj<::gpk::SSurface>			& surface				= Scene->Graphics->Surfaces[iSurface];
	surface->Desc.ColorType				= ::gpk::COLOR_TYPE_BGRA;
	surface->Desc.MethodCompression		= 0;
	surface->Desc.MethodFilter			= 0;
	surface->Desc.MethodInterlace		= 0;
	surface->Desc.Dimensions			= {32, 32};
	surface->Data.resize(surface->Desc.Dimensions.Area() * sizeof(::gpk::bgra));
	memset(surface->Data.begin(), 0xFF, surface->Data.size());
	::gpk::view2d<::gpk::bgra>				view					= {(::gpk::bgra*)surface->Data.begin(), surface->Desc.Dimensions.Cast<uint32_t>()};
	::gpk::rgba								color					= {::gpk::ASCII_PALETTE[rand() % 16]};
	for(uint32_t y = surface->Desc.Dimensions.y / 3; y < surface->Desc.Dimensions.y / 3U * 2U; ++y)
	for(uint32_t x = 0; x < surface->Desc.Dimensions.x; ++x)
		view[y][x]	= color;

	mesh->GeometrySlices.resize(1);	// one per face
	::gpk::SGeometrySlice					& slice					= mesh->GeometrySlices[0];
	slice.Slice							= {0, geometry.PositionIndices.size()};

	::gpk::SRenderNode						& renderNode						= Scene->RenderNodes.RenderNodes[entity.RenderNode];
	renderNode.Skin						= iSkin;
	renderNode.Mesh						= iMesh;
	renderNode.Slice					= 0;
	Scene->Graphics->Shaders[renderNode.Shader = Scene->Graphics->Shaders.push_back({})].create(::gpk::psSolid);
	return iEntity;
}

::gpk::error_t						gpk::SEngine::CreateCylinder		(uint16_t slices, bool reverse, float diameterRatio)	{ 
	::gpk::SGeometryIndexedTriangles		geometry;
	if(reverse)
		::gpk::geometryBuildCylinder(geometry, 1, slices, .5f, .5f, {}, {1, 1, -1}, true, diameterRatio);
	else
		::gpk::geometryBuildCylinder(geometry, 1, slices, .5f, .5f, {}, {1, 1, 1}, false, diameterRatio);

	int32_t									iEntity								= this->Entities.Create();
	Entities.Names[iEntity]		= ::gpk::vcs{"Cylinder"};
	::gpk::SVirtualEntity					& entity							= Entities[iEntity];
	entity.RenderNode					= Scene->RenderNodes.Create();
	entity.RigidBody					= this->Integrator.Create();

	::gpk::pobj<::gpk::SRenderBuffer>		pIndicesVertex;
	::gpk::pobj<::gpk::SRenderBuffer>		pVertices;
	::gpk::pobj<::gpk::SRenderBuffer>		pNormals;
	::gpk::pobj<::gpk::SRenderBuffer>		pUV;

	pIndicesVertex	->Desc.Format		= ::gpk::DATA_TYPE_UINT16;
	pIndicesVertex	->Desc.Usage		= ::gpk::BUFFER_USAGE_Index;

	pVertices		->Desc.Format		= ::gpk::DATA_TYPE_FLOAT32_3;
	pVertices		->Desc.Usage		= ::gpk::BUFFER_USAGE_Position;

	pNormals		->Desc.Format		= ::gpk::DATA_TYPE_FLOAT32_3;
	pNormals		->Desc.Usage		= ::gpk::BUFFER_USAGE_Normal;

	pUV				->Desc.Format		= ::gpk::DATA_TYPE_FLOAT32_2;
	pUV				->Desc.Usage		= ::gpk::BUFFER_USAGE_UV;

	pIndicesVertex	->Data.resize(geometry.PositionIndices	.byte_count() / 2);
	::gpk::view<uint16_t>					viewIndices							= {(uint16_t*)pIndicesVertex->Data.begin(), geometry.PositionIndices.size()};
	for(uint32_t index = 0; index < geometry.PositionIndices.size(); ++index) {
		viewIndices[index]					= (uint16_t)geometry.PositionIndices[index];
	}
	//memcpy(&pIndicesVertex	->Data[0], geometry.PositionIndices	.begin(), pIndicesVertex	->Data.size());

	pVertices	->Data.resize(geometry.Positions		.byte_count());
	pNormals	->Data.resize(geometry.Normals			.byte_count());
	pUV			->Data.resize(geometry.TextureCoords	.byte_count());
	memcpy(&pVertices	->Data[0], geometry.Positions		.begin(), pVertices	->Data.size());
	memcpy(&pNormals	->Data[0], geometry.Normals			.begin(), pNormals	->Data.size());
	memcpy(&pUV			->Data[0], geometry.TextureCoords	.begin(), pUV		->Data.size());

	uint32_t								iVertices				= (uint32_t)Scene->Graphics->Buffers.push_back(pVertices);
	uint32_t								iNormals				= (uint32_t)Scene->Graphics->Buffers.push_back(pNormals);
	uint32_t								iUV						= (uint32_t)Scene->Graphics->Buffers.push_back(pUV);
	uint32_t								iIndicesVertex			= (uint32_t)Scene->Graphics->Buffers.push_back(pIndicesVertex);

	uint32_t								iMesh					= (uint32_t)Scene->Graphics->Meshes.Create();
	::gpk::pobj<::gpk::SGeometryMesh>		& mesh					= Scene->Graphics->Meshes[iMesh];
	Scene->Graphics->Meshes.Names[iMesh]= ::gpk::vcs{"Sphere"};
	mesh->GeometryBuffers.append({iIndicesVertex, iVertices, iNormals, iUV});

	mesh->Desc.Mode						= ::gpk::MESH_MODE_List;
	mesh->Desc.Type						= ::gpk::GEOMETRY_TYPE_Triangle;
	mesh->Desc.NormalMode				= ::gpk::NORMAL_MODE_Point;

	uint32_t								iSkin					= (uint32_t)Scene->Graphics->Skins.Create();
	::gpk::pobj<::gpk::SSkin>				& skin					= Scene->Graphics->Skins.Elements[iSkin];
	skin->Material.Color.Ambient		= ::gpk::bgra(::gpk::ASCII_PALETTE[3]);
	skin->Material.Color.Diffuse		= ::gpk::bgra(::gpk::ASCII_PALETTE[3]);
	skin->Material.Color.Specular		= ::gpk::WHITE;
	skin->Material.SpecularPower		= 0.5f;

	uint32_t								iSurface				= (uint32_t)Scene->Graphics->Surfaces.Create();
	skin->Textures.push_back(iSurface);

	skin->Material.Color.Ambient		*= .1f;

	::gpk::pobj<::gpk::SSurface>			& surface				= Scene->Graphics->Surfaces[iSurface];
	surface->Desc.ColorType				= ::gpk::COLOR_TYPE_BGRA;
	surface->Desc.MethodCompression		= 0;
	surface->Desc.MethodFilter			= 0;
	surface->Desc.MethodInterlace		= 0;
	surface->Desc.Dimensions			= {32, 16};
	surface->Data.resize(surface->Desc.Dimensions.Area() * sizeof(::gpk::bgra));
	memset(surface->Data.begin(), 0xFF, surface->Data.size());
	::gpk::view2d<::gpk::bgra>				view					= {(::gpk::bgra*)surface->Data.begin(), surface->Desc.Dimensions.Cast<uint32_t>()};
	::gpk::rgba								color					= {::gpk::ASCII_PALETTE[rand() % 16]};
	for(uint32_t y = surface->Desc.Dimensions.y / 3; y < surface->Desc.Dimensions.y / 3U * 2U; ++y)
	for(uint32_t x = 0; x < surface->Desc.Dimensions.x; ++x)
		view[y][x]							= color;

	mesh->GeometrySlices.resize(1);	// one per face
	::gpk::SGeometrySlice					& slice					= mesh->GeometrySlices[0];
	slice.Slice							= {0, geometry.PositionIndices.size()};

	::gpk::SRenderNode						& renderNode						= Scene->RenderNodes.RenderNodes[entity.RenderNode];
	renderNode.Skin						= iSkin;
	renderNode.Mesh						= iMesh;
	renderNode.Slice					= 0;
	Scene->Graphics->Shaders[renderNode.Shader = Scene->Graphics->Shaders.push_back({})].create(::gpk::psSolid);

	return iEntity;
}

::gpk::error_t			gpk::SEngine::CreateCircle			()	{ 	
	int32_t									iEntity								= this->Entities.Create();
	return iEntity;
 }
::gpk::error_t			gpk::SEngine::CreateRing			()	{ 	
	int32_t									iEntity								= this->Entities.Create();
	return iEntity;
 }
::gpk::error_t			gpk::SEngine::CreateSquare			()	{ 	
	int32_t									iEntity								= this->Entities.Create();
	return iEntity;
 }
::gpk::error_t			gpk::SEngine::CreateTriangle		()	{ 	
	int32_t									iEntity								= this->Entities.Create();
	return iEntity;
 }

