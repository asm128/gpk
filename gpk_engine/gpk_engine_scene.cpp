#include "gpk_engine_scene.h"
#include "gpk_engine_shader.h"
#include "gpk_noise.h"
#include "gpk_ascii_color.h"
#include "gpk_raster_lh.h"
#include "gpk_view_n3.h"
#include "gpk_view_n2.h"
#include "gpk_view_tri.h"

template <typename _tIndex>
static	::gpk::error_t	transformTriangles					
	( ::gpk::SVSOutput		& output
	, ::gpk::view<const _tIndex>	indices			
	, ::gpk::vc3f32			positions	
	, ::gpk::vc3f32			normals		
	, ::gpk::vc2f32			uv			
	, const ::gpk::m4f32	& projection		
	, const ::gpk::m4f32	& worldTransform	
	, const ::gpk::n3f32	& cameraFront
)	{ 
	::gpk::vctri<_tIndex>				view_indices			= {(const ::gpk::tri<_tIndex>*)indices.begin(), indices.size() / 3};
	const ::gpk::m4f32			mWVPS					= worldTransform * projection;

	for(uint32_t iTriangle = 0; iTriangle < view_indices.size(); ++iTriangle) {
		const ::gpk::tri<_tIndex>	vertexIndices			= view_indices[iTriangle];
		::gpk::tri3f32				transformedNormals		= {normals[vertexIndices.A], normals[vertexIndices.B], normals[vertexIndices.C]};
		::gpk::transformDirection(transformedNormals, worldTransform);
		transformedNormals.A.Normalize();
		transformedNormals.B.Normalize();
		transformedNormals.C.Normalize();
		double						directionFactorA		= transformedNormals.A.Dot(cameraFront);
		double						directionFactorB		= transformedNormals.B.Dot(cameraFront);
		double						directionFactorC		= transformedNormals.C.Dot(cameraFront);
		if(directionFactorA > .35 && directionFactorB > .35 && directionFactorC > .35)
			continue;

		output.Normals.push_back(transformedNormals);

		::gpk::tri3f32				transformedPositions	= {positions[vertexIndices.A], positions[vertexIndices.B], positions[vertexIndices.C]};
		::gpk::transform(transformedPositions, worldTransform);
		output.PositionsWorld.push_back(transformedPositions);
		
		::gpk::transform(transformedPositions, projection);
		output.PositionsScreen.push_back(transformedPositions);
		
		::gpk::tri2f32				transformedUVs			= {uv[vertexIndices.A], uv[vertexIndices.B], uv[vertexIndices.C]};
		if( transformedUVs.A.x > 1.0f
		 || transformedUVs.A.y > 1.0f
		 || transformedUVs.B.x > 1.0f
		 || transformedUVs.B.y > 1.0f
		 || transformedUVs.C.x > 1.0f
		 || transformedUVs.C.y > 1.0f
		) 
			_CrtDbgBreak();
		output.UVs.push_back(transformedUVs);
	}
	return 0; 
}

static	::gpk::error_t	transformTriangles
	( ::gpk::SEngineRenderCache				& renderCache
	, const ::gpk::SEngineScene				& scene
	, const ::gpk::SEngineSceneConstants	& constants
	, int32_t								iRenderNode
	) {
	const ::gpk::SRenderNode	& renderNode			= scene.RenderNodes.RenderNodes[iRenderNode];
	const ::gpk::SGeometryMesh	& mesh					= *scene.Graphics->Meshes[renderNode.Mesh];
	verbose_printf("Drawing node %i, mesh %i, slice %i, mesh name: %s", iRenderNode, renderNode.Mesh, renderNode.Slice, scene.Graphics->Meshes.Names[renderNode.Mesh].begin());

	const ::gpk::SRenderNodeTransforms	& transforms	= scene.RenderNodes.Transforms[iRenderNode];
	const ::gpk::m4f32			& worldTransform		= transforms.World;

	const ::gpk::vc3f32			positions				= (mesh.GeometryBuffers.size() > 1) ? ::gpk::vc3f32	{(const ::gpk::n3f32*)scene.Graphics->Buffers[mesh.GeometryBuffers[1]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[1]]->Data.size() / sizeof(const ::gpk::n3f32)}	: ::gpk::vc3f32{};
	const ::gpk::vc3f32			normals					= (mesh.GeometryBuffers.size() > 2) ? ::gpk::vc3f32	{(const ::gpk::n3f32*)scene.Graphics->Buffers[mesh.GeometryBuffers[2]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[2]]->Data.size() / sizeof(const ::gpk::n3f32)}	: ::gpk::vc3f32{};
	const ::gpk::vc2f32			uv						= (mesh.GeometryBuffers.size() > 3) ? ::gpk::vc2f32	{(const ::gpk::n2f32*)scene.Graphics->Buffers[mesh.GeometryBuffers[3]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[3]]->Data.size() / sizeof(const ::gpk::n2f32)}	: ::gpk::vc2f32{};


	renderCache.VertexShaderOutput	= {};

	if(mesh.GeometryBuffers.size()) {
		::gpk::pobj<::gpk::SRenderBuffer>	indexBuffer		= scene.Graphics->Buffers[mesh.GeometryBuffers[0]];
		if(indexBuffer->Desc.Format.TotalBytes() == 1) {
			const ::gpk::vcu8			indices					= ::gpk::vcu8{(const uint8_t*)indexBuffer->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.size() / sizeof(const uint8_t)};
			const ::gpk::SGeometrySlice	slice					= (renderNode.Slice < mesh.GeometrySlices.size()) ? mesh.GeometrySlices[renderNode.Slice] : ::gpk::SGeometrySlice{{0, indices.size() / 3}};
			::transformTriangles(renderCache.VertexShaderOutput, indices, positions, normals, uv, constants.VPS, worldTransform, constants.CameraFront);
		}
		else if(indexBuffer->Desc.Format.TotalBytes() == 2) {
			const ::gpk::vcu16			indices					= ::gpk::vcu16{(const uint16_t*)indexBuffer->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.size() / sizeof(const uint16_t)};
			const ::gpk::SGeometrySlice	slice					= (renderNode.Slice < mesh.GeometrySlices.size()) ? mesh.GeometrySlices[renderNode.Slice] : ::gpk::SGeometrySlice{{0, indices.size() / 3}};
			::transformTriangles(renderCache.VertexShaderOutput, indices, positions, normals, uv, constants.VPS, worldTransform, constants.CameraFront);
		}
		else if(indexBuffer->Desc.Format.TotalBytes() == 4) {
			const ::gpk::vcu32			indices					= ::gpk::vcu32{(const uint32_t*)indexBuffer->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.size() / sizeof(const uint16_t)};
			const ::gpk::SGeometrySlice	slice					= (renderNode.Slice < mesh.GeometrySlices.size()) ? mesh.GeometrySlices[renderNode.Slice] : ::gpk::SGeometrySlice{{0, indices.size() / 3}};
			::transformTriangles(renderCache.VertexShaderOutput, indices, positions, normals, uv, constants.VPS, worldTransform, constants.CameraFront);
		}
	}
	return 0;
}


static	::gpk::error_t	drawBuffers
	( ::gpk::g8bgra										& backBufferColors
	, ::gpk::gu32										backBufferDepth
	, ::gpk::SVSOutput									& outVS
	, ::gpk::SVSCache									& cacheVS
	, const ::gpk::SRenderMaterial						& material
	, ::gpk::gc8bgra									surface
	, const ::gpk::SEngineSceneConstants				& constants
	, const ::std::function<::gpk::TFuncPixelShader>	& ps
	) {	// 
	::gpk::atrif32				& triangleWeights			= cacheVS.TriangleWeights		;
	::gpk::an2i16				& trianglePixelCoords		= cacheVS.SolidPixelCoords		;
	const ::gpk::n2u16			offscreenMetrics			= backBufferColors.metrics().u16();
	const ::gpk::n3f32			lightDirectionNormalized	= ::gpk::n3f32{constants.LightDirection}.Normalize();
	::gpk::SPSIn				inPS						= {};
	inPS.Surface			= surface;
	inPS.Material			= material;

	for(uint32_t iTriangle = 0; iTriangle < outVS.PositionsScreen.size(); ++iTriangle) {
		const ::gpk::tri3f32		& triPositions				= outVS.PositionsScreen	[iTriangle];
		if( triPositions.CulledZ({0, 0xFFFFFF})
		 || triPositions.CulledX({0, (float)offscreenMetrics.x})
		 || triPositions.CulledY({0, (float)offscreenMetrics.y})
		)
			continue;

		const ::gpk::tri3f32		& triPositionsWorld			= outVS.PositionsWorld	[iTriangle];
		const ::gpk::tri3f32		& triNormals				= outVS.Normals			[iTriangle];
		const ::gpk::tri2f32		& triUVs					= outVS.UVs				[iTriangle];

		trianglePixelCoords.clear();
		triangleWeights.clear();
		es_if_failed(::gpk::drawTriangle(offscreenMetrics.u32(), triPositions, trianglePixelCoords, triangleWeights, backBufferDepth));
		//const bool													stripped					= surface[0][0] == ::gpk::bgra{gpk::WHITE};
		for(uint32_t iCoord = 0; iCoord < trianglePixelCoords.size(); ++iCoord) {
			const ::gpk::trif32							& vertexWeights				= triangleWeights[iCoord];
			inPS.WeightedPosition								= triPositionsWorld.A * vertexWeights.A + triPositionsWorld.B * vertexWeights.B + triPositionsWorld.C * vertexWeights.C;
			inPS.WeightedNormal									= (triNormals.A * vertexWeights.A + triNormals.B * vertexWeights.B + triNormals.C * vertexWeights.C).Normalize();
			inPS.WeightedUV										= triUVs.A * vertexWeights.A + triUVs.B * vertexWeights.B + triUVs.C * vertexWeights.C;
			const ::gpk::n2u16										coord						= trianglePixelCoords[iCoord].u16();
			ps(constants, inPS, backBufferColors[coord.y][coord.x]);
		}
	}
	return 0;
}

::gpk::error_t			gpk::drawScene									
	( ::gpk::g8bgra							backBufferColors
	, ::gpk::gu32							backBufferDepth
	, ::gpk::SEngineRenderCache				& renderCache
	, const ::gpk::SEngineScene				& scene
	, const ::gpk::SEngineSceneConstants	& constants
) {	//
	static ::std::function<::gpk::TFuncPixelShader>	defaultShader			= {::gpk::psSolid};
	for(uint32_t iRenderNode = 0, countNodes = scene.RenderNodes.RenderNodes.size(); iRenderNode < countNodes; ++iRenderNode) {
		const ::gpk::SRenderNodeFlags					& renderNodeFlags		= scene.RenderNodes.Flags[iRenderNode];
		if(renderNodeFlags.NoDraw)
			continue;

		const ::gpk::SRenderNode						& renderNode			= scene.RenderNodes.RenderNodes[iRenderNode];
		if(renderNode.Mesh >= scene.Graphics->Meshes.size())
			continue;

		info_printf("Mesh name: %s", scene.Graphics->Meshes.Names[renderNode.Mesh].begin());
		
		const ::gpk::SRenderNodeTransforms				& transforms			= scene.RenderNodes.Transforms[iRenderNode];
		const ::gpk::m4f32								& worldTransform		= transforms.World;
		if((worldTransform.GetTranslation() - constants.CameraPosition).Normalize().Dot(constants.CameraFront) <= 0)
			return 0;

		ce_if(errored(::transformTriangles(renderCache, scene, constants, iRenderNode)), "iRenderNode %i", iRenderNode);

		const ::gpk::SSkin										& skin						= *scene.Graphics->Skins.Elements[renderNode.Skin];
		const ::gpk::SRenderMaterial							& material					= skin.Material;
		const uint32_t											tex							= skin.Textures[0];
		const ::gpk::SSurface									& surface					= *scene.Graphics->Surfaces[tex];
		const ::std::function<::gpk::TFuncPixelShader>			& fx
			= (renderNode.Shader >= scene.Graphics->Shaders.size()) ? defaultShader : *scene.Graphics->Shaders[renderNode.Shader];

		drawBuffers(backBufferColors, backBufferDepth, renderCache.VertexShaderOutput, renderCache.VertexShaderCache, material, {(const ::gpk::bgra*)surface.Data.begin(), surface.Desc.Dimensions.u32()}, constants, fx);
	}

	const ::gpk::n2<uint16_t>					offscreenMetrics		= backBufferColors.metrics().u16();
	::gpk::apod<::gpk::n3f32>					& wireframePixelCoords	= renderCache.VertexShaderCache.WireframePixelCoords;

	// ---- Draw axis vectors at the origin (0, 0, 0)
	constexpr ::gpk::n3f32						xyz	[3]					= 
		{ {1}
		, {0, 1}
		, {0, 0, 1}
		};
	constexpr ::gpk::bgra						colorXYZ[3]				= 
		{ ::gpk::RED
		, ::gpk::GREEN
		, ::gpk::BLUE
		};

	for(uint32_t iVector = 0; iVector < 3; ++iVector) {
		wireframePixelCoords.clear();
		::gpk::drawLine(offscreenMetrics, ::gpk::line3f32{{}, xyz[iVector]}, constants.VPS, wireframePixelCoords, backBufferDepth);
		for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord) {
			::gpk::n3u16									coord					= wireframePixelCoords[iCoord].u16();
			backBufferColors[coord.y][coord.x]	= colorXYZ[iVector];
		}
	}
	return 0;
}

::gpk::error_t			gpk::SEngineScene::CreateRenderNode
	( const ::gpk::SGeometryBuffers		& geometry
	, const ::gpk::vcc					name 
	, bool								createSkin
	) {
	uint32_t					iIndicesVertex			= 0;
	uint32_t					iVertices				= 0;
	uint32_t					iNormals				= 0;
	uint32_t					iUV						= 0;
	::gpk::createBuffers(Graphics->Buffers, geometry, iIndicesVertex, iVertices, iNormals, iUV);

	uint32_t					iMesh					= (uint32_t)Graphics->Meshes.Create();
	::gpk::pobj<::gpk::SGeometryMesh>	& mesh			= Graphics->Meshes[iMesh];
	Graphics->Meshes.Names[iMesh] = name;
	mesh->GeometryBuffers.append({iIndicesVertex, iVertices, iNormals, iUV});

	mesh->Desc.Mode			= ::gpk::MESH_MODE_List;
	mesh->Desc.Type			= ::gpk::GEOMETRY_TYPE_Triangle;
	mesh->Desc.NormalMode	= ::gpk::NORMAL_MODE_Point;


	uint32_t					iRenderNode				= RenderNodes.Create();
	::gpk::SRenderNode			& renderNode			= RenderNodes.RenderNodes[iRenderNode];
	renderNode.Mesh			= iMesh;
	if(createSkin) {
		uint32_t					iSkin					= (uint32_t)Graphics->Skins.Create();
		::gpk::pobj<::gpk::SSkin>	& skin					= Graphics->Skins.Elements[iSkin];
		skin->Material.Color.Ambient	= ::gpk::bgra(::gpk::ASCII_PALETTE[3]) * .1f;
		skin->Material.Color.Diffuse	= ::gpk::bgra(::gpk::ASCII_PALETTE[3]);
		skin->Material.Color.Specular	= ::gpk::WHITE;
		skin->Material.SpecularPower	= 0.5f;

		uint32_t					iSurface				= (uint32_t)Graphics->Surfaces.Create();
		skin->Textures.push_back(iSurface);

		skin->Material.Color.Ambient	*= .1f;

		::gpk::pobj<::gpk::SSurface>	& surface			= Graphics->Surfaces[iSurface];
		surface->Desc.ColorType	= ::gpk::COLOR_TYPE_BGRA;
		surface->Desc.MethodCompression	= 0;
		surface->Desc.MethodFilter		= 0;
		surface->Desc.MethodInterlace	= 0;
		surface->Desc.Dimensions		= {32, 32};
		surface->Data.resize(surface->Desc.Dimensions.Area() * sizeof(::gpk::bgra));
		memset(surface->Data.begin(), 0xFF, surface->Data.size());
		::gpk::g8bgra				view					= {(::gpk::bgra*)surface->Data.begin(), surface->Desc.Dimensions.u32()};
		::gpk::rgba					color					= {::gpk::ASCII_PALETTE[rand() % 16]};
		for(uint32_t y = surface->Desc.Dimensions.y / 3; y < surface->Desc.Dimensions.y / 3U * 2U; ++y)
		for(uint32_t x = 0; x < surface->Desc.Dimensions.x; ++x)
			view[y][x]				= color;


		mesh->GeometrySlices.resize(1);	// 

		::gpk::SGeometrySlice		& slice					= mesh->GeometrySlices[0];
		slice.Slice				= {0, geometry.PositionIndices.size()};

		renderNode.Skin			= iSkin;
		renderNode.Slice		= 0;
		Graphics->Shaders[renderNode.Shader = Graphics->Shaders.push_back({})].create(::gpk::psSolid);
		Graphics->Shaders.Names[renderNode.Shader] = "psSolid";
	}
	return iRenderNode;
}