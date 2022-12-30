#include "gpk_engine_scene.h"
#include "gpk_engine_shader.h"
#include "gpk_noise.h"
#include "gpk_ascii_color.h"
#include "gpk_raster_lh.h"

static	::gpk::error_t								transformTriangles					
	( ::gpk::SVSOutput				& output
	, ::gpk::vcu16					indices			
	, ::gpk::v1c3f					positions	
	, ::gpk::v1c3f					normals		
	, ::gpk::v1c2f					uv			
	, const ::gpk::SMatrix4<float>	& projection		
	, const ::gpk::SMatrix4<float>	& worldTransform	
	, const ::gpk::SCoord3<float>	& cameraFront
)	{ 
	::gpk::v1d<const ::gpk::STriangle<uint16_t>>		view_indices				= {(const ::gpk::STriangle<uint16_t>*)indices.begin(), indices.size() / 3};

	const ::gpk::SMatrix4<float>							mWVPS						= worldTransform * projection;
	for(uint32_t iTriangle = 0; iTriangle < view_indices.size(); ++iTriangle) {
		const ::gpk::STriangle<uint16_t>						vertexIndices				= view_indices[iTriangle];
		::gpk::STriangle3<float>								transformedNormals			= {normals[vertexIndices.A], normals[vertexIndices.B], normals[vertexIndices.C]};
		::gpk::transformDirection(transformedNormals, worldTransform);
		transformedNormals.A.Normalize();
		transformedNormals.B.Normalize();
		transformedNormals.C.Normalize();
		double													directionFactorA			= transformedNormals.A.Dot(cameraFront);
		double													directionFactorB			= transformedNormals.B.Dot(cameraFront);
		double													directionFactorC			= transformedNormals.C.Dot(cameraFront);
		if(directionFactorA > .35 && directionFactorB > .35 && directionFactorC > .35)
			continue;

		output.Normals.push_back(transformedNormals);

		::gpk::STriangle3<float>								transformedPositions		= {positions[vertexIndices.A], positions[vertexIndices.B], positions[vertexIndices.C]};
		::gpk::transform(transformedPositions, worldTransform);
		output.PositionsWorld.push_back(transformedPositions);
		
		::gpk::transform(transformedPositions, projection);
		output.PositionsScreen.push_back(transformedPositions);
		
		::gpk::STriangle2<float>								transformedUVs				= {uv[vertexIndices.A], uv[vertexIndices.B], uv[vertexIndices.C]};
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

static	::gpk::error_t								transformTriangles
	( ::gpk::SEngineRenderCache				& renderCache
	, const ::gpk::SEngineScene				& scene
	, const ::gpk::SEngineSceneConstants	& constants
	, int32_t								iRenderNode
	) {
	const ::gpk::SRenderNode								& renderNode			= scene.ManagedRenderNodes.RenderNodes[iRenderNode];
	const ::gpk::SGeometryMesh								& mesh					= *scene.Graphics->Meshes[renderNode.Mesh];
	verbose_printf("Drawing node %i, mesh %i, slice %i, mesh name: %s", iRenderNode, renderNode.Mesh, renderNode.Slice, scene.Graphics->Meshes.Names[renderNode.Mesh].begin());

	const ::gpk::SRenderNodeTransforms						& transforms				= scene.ManagedRenderNodes.Transforms[iRenderNode];
	const ::gpk::SMatrix4<float>							& worldTransform			= transforms.World;
	const ::gpk::vcu16										indices						= (mesh.GeometryBuffers.size() > 0) ? ::gpk::view_array<const uint16_t>					{(const uint16_t				*)scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.size() / sizeof(const uint16_t)}				: ::gpk::view_array<const uint16_t>					{};
	const ::gpk::v1c3f										positions					= (mesh.GeometryBuffers.size() > 1) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)scene.Graphics->Buffers[mesh.GeometryBuffers[1]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[1]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
	const ::gpk::v1c3f										normals						= (mesh.GeometryBuffers.size() > 2) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)scene.Graphics->Buffers[mesh.GeometryBuffers[2]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[2]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
	const ::gpk::v1c2f										uv							= (mesh.GeometryBuffers.size() > 3) ? ::gpk::view_array<const ::gpk::SCoord2<float>>	{(const ::gpk::SCoord2<float>	*)scene.Graphics->Buffers[mesh.GeometryBuffers[3]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[3]]->Data.size() / sizeof(const ::gpk::SCoord2<float>)}	: ::gpk::view_array<const ::gpk::SCoord2<float>>	{};

	const ::gpk::SGeometrySlice								slice						= (renderNode.Slice < mesh.GeometrySlices.size()) ? mesh.GeometrySlices[renderNode.Slice] : ::gpk::SGeometrySlice{{0, indices.size() / 3}};

	renderCache.VertexShaderOutput						= {};
	::transformTriangles(renderCache.VertexShaderOutput, indices, positions, normals, uv, constants.VPS, worldTransform, constants.CameraFront);
	return 0;
}


static	::gpk::error_t								drawBuffers
	( ::gpk::view2d<::gpk::SColorBGRA>					& backBufferColors
	, ::gpk::v2u32										backBufferDepth
	, ::gpk::SVSOutput									& outVS
	, ::gpk::SVSCache									& cacheVS
	, const ::gpk::SRenderMaterial						& material
	, ::gpk::view2d<const ::gpk::SColorBGRA>			surface
	, const ::gpk::SEngineSceneConstants				& constants
	, const ::std::function<::gpk::TFuncPixelShader>	& ps
	) {	// 
	::gpk::apod<::gpk::STriangle<float>>					& triangleWeights			= cacheVS.TriangleWeights		;
	::gpk::apod<::gpk::SCoord2<int16_t>>					& trianglePixelCoords		= cacheVS.SolidPixelCoords		;
	const ::gpk::SCoord2<uint16_t>							offscreenMetrics			= backBufferColors.metrics().Cast<uint16_t>();
	const ::gpk::SCoord3<float>								lightDirectionNormalized	= ::gpk::SCoord3<float>{constants.LightDirection}.Normalize();
	::gpk::SPSIn											inPS						= {};
	inPS.Surface										= surface;
	inPS.Material										= material;

	for(uint32_t iTriangle = 0; iTriangle < outVS.PositionsScreen.size(); ++iTriangle) {
		const ::gpk::STriangle3<float>							& triPositions				= outVS.PositionsScreen	[iTriangle];
		if( triPositions.CulledZ({0, 0xFFFFFF})
		 || triPositions.CulledX({0, (float)offscreenMetrics.x})
		 || triPositions.CulledY({0, (float)offscreenMetrics.y})
		)
			continue;

		const ::gpk::STriangle3<float>							& triPositionsWorld			= outVS.PositionsWorld	[iTriangle];
		const ::gpk::STriangle3<float>							& triNormals				= outVS.Normals			[iTriangle];
		const ::gpk::STriangle2<float>							& triUVs					= outVS.UVs				[iTriangle];

		trianglePixelCoords.clear();
		triangleWeights.clear();
		gerror_if(errored(::gpk::drawTriangle(offscreenMetrics.Cast<uint32_t>(), triPositions, trianglePixelCoords, triangleWeights, backBufferDepth)), "Not sure if these functions could ever fail");
		//const bool													stripped					= surface[0][0] == ::gpk::SColorBGRA{gpk::WHITE};
		for(uint32_t iCoord = 0; iCoord < trianglePixelCoords.size(); ++iCoord) {
			const ::gpk::STriangle<float>							& vertexWeights				= triangleWeights[iCoord];
			inPS.WeightedPosition								= triPositionsWorld.A * vertexWeights.A + triPositionsWorld.B * vertexWeights.B + triPositionsWorld.C * vertexWeights.C;
			inPS.WeightedNormal									= (triNormals.A * vertexWeights.A + triNormals.B * vertexWeights.B + triNormals.C * vertexWeights.C).Normalize();
			inPS.WeightedUV										= triUVs.A * vertexWeights.A + triUVs.B * vertexWeights.B + triUVs.C * vertexWeights.C;
			const ::gpk::SCoord2<uint16_t>							coord						= trianglePixelCoords[iCoord].Cast<uint16_t>();
			ps(constants, inPS, backBufferColors[coord.y][coord.x]);
		}
	}
	return 0;
}

::gpk::error_t										gpk::drawScene									
	( ::gpk::view2d<::gpk::SColorBGRA>	& backBufferColors
	, ::gpk::view2d<uint32_t>			& backBufferDepth
	, ::gpk::SEngineRenderCache				& renderCache
	, const ::gpk::SEngineScene				& scene
	, const ::gpk::SEngineSceneConstants	& constants
) {	//
	static ::std::function<::gpk::TFuncPixelShader>	defaultShader			= {::gpk::psSolid};
	for(uint32_t iRenderNode = 0, countNodes = scene.ManagedRenderNodes.RenderNodes.size(); iRenderNode < countNodes; ++iRenderNode) {
		const ::gpk::SRenderNodeFlags					& renderNodeFlags		= scene.ManagedRenderNodes.Flags[iRenderNode];
		if(renderNodeFlags.NoDraw)
			continue;

		const ::gpk::SRenderNode						& renderNode			= scene.ManagedRenderNodes.RenderNodes[iRenderNode];
		if(renderNode.Mesh >= scene.Graphics->Meshes.size())
			continue;
		
		const ::gpk::SRenderNodeTransforms				& transforms			= scene.ManagedRenderNodes.Transforms[iRenderNode];
		const ::gpk::SMatrix4<float>					& worldTransform		= transforms.World;
		if((worldTransform.GetTranslation() - constants.CameraPosition).Normalize().Dot(constants.CameraFront) <= 0)
			return 0;

		ce_if(errored(::transformTriangles(renderCache, scene, constants, iRenderNode)), "iRenderNode %i", iRenderNode);

		const ::gpk::SSkin										& skin						= *scene.Graphics->Skins.Elements[renderNode.Skin];
		const ::gpk::SRenderMaterial							& material					= skin.Material;
		const uint32_t											tex							= skin.Textures[0];
		const ::gpk::SSurface									& surface					= *scene.Graphics->Surfaces[tex];
		const ::std::function<::gpk::TFuncPixelShader>			& fx
			= (renderNode.Shader >= scene.Graphics->Shaders.size()) ? defaultShader : *scene.Graphics->Shaders[renderNode.Shader];

		drawBuffers(backBufferColors, backBufferDepth, renderCache.VertexShaderOutput, renderCache.VertexShaderCache, material, {(const ::gpk::SColorBGRA*)surface.Data.begin(), surface.Desc.Dimensions.Cast<uint32_t>()}, constants, fx);
	}

	const ::gpk::SCoord2<uint16_t>					offscreenMetrics		= backBufferColors.metrics().Cast<uint16_t>();
	::gpk::apod<::gpk::SCoord3<float>>			& wireframePixelCoords	= renderCache.VertexShaderCache.WireframePixelCoords;

	// ---- Draw axis vectors at the origin (0, 0, 0)
	constexpr ::gpk::SCoord3<float>					xyz	[3]					= 
		{ {1}
		, {0, 1}
		, {0, 0, 1}
		};
	constexpr ::gpk::SColorBGRA						colorXYZ[3]				= 
		{ ::gpk::RED
		, ::gpk::GREEN
		, ::gpk::BLUE
		};

	for(uint32_t iVector = 0; iVector < 3; ++iVector) {
		wireframePixelCoords.clear();
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{{}, xyz[iVector]}, constants.VPS, wireframePixelCoords, backBufferDepth);
		for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord) {
			::gpk::SCoord3<uint32_t>						coord					= wireframePixelCoords[iCoord].Cast<uint32_t>();
			backBufferColors[coord.y][coord.x]	= colorXYZ[iVector];
		}
	}
	return 0;
}
