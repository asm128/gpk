#include "gpk_engine_shader.h"

::gpk::SColorFloat								gpk::lightCalcSpecular		(::gpk::SCoord3<float> gEyePosW, float specularPower, ::gpk::SColorFloat specularLight, ::gpk::SColorFloat specularMaterial, ::gpk::SCoord3<float> posW, ::gpk::SCoord3<float> normalW, ::gpk::SCoord3<float> lightVecW) {
	const ::gpk::SCoord3<float>							pointToEye				= (gEyePosW - posW).Normalize();
	const ::gpk::SCoord3<float>							reflected				= normalW.Reflect(-lightVecW);
	const double										reflectedFactor			= ::gpk::max(reflected.Dot(pointToEye), 0.0);
	if(0 == reflectedFactor) 
		return ::gpk::BLACK;

	const double										factor					= pow(reflectedFactor, specularPower);
	::gpk::SColorFloat									result					= specularMaterial * specularLight * factor;
	result.a										= specularMaterial.a;
	return result;
	
}

::gpk::SColorFloat								gpk::lightCalcDiffuse		(::gpk::SColorFloat diffuserMaterial, ::gpk::SCoord3<float> normalW, ::gpk::SCoord3<float> lightVecW) {
	double												lightFactor				= ::gpk::max(0.0, normalW.Dot(lightVecW));
	return lightFactor ? diffuserMaterial * (float)lightFactor : ::gpk::BLACK;
}

static	::gpk::error_t							transformTrianglesWireframe
	( ::gpk::SVSOutput									& output
	, ::gpk::view_array<const uint16_t>					indices			
	, ::gpk::view_array<const ::gpk::SCoord3<float>>	positions	
	, ::gpk::view_array<const ::gpk::SCoord3<float>>	normals		
	, const ::gpk::SMatrix4<float>						& projection		
	, const ::gpk::SMatrix4<float>						& worldTransform	
	, const ::gpk::SCoord3<float>						& cameraFront
)	{ 
	::gpk::view_array<const ::gpk::STriangle<uint16_t>>		view_indices		= {(const ::gpk::STriangle<uint16_t>*)indices.begin(), indices.size() / 3};

	const ::gpk::SMatrix4<float>							mWVPS				= worldTransform * projection;
	for(uint32_t iTriangle = 0; iTriangle < view_indices.size(); ++iTriangle) {
		const ::gpk::STriangle<uint16_t>											vertexIndices								= view_indices[iTriangle];
		::gpk::STriangle3<float>													transformedNormals							= {normals[vertexIndices.A], normals[vertexIndices.B], normals[vertexIndices.C]};
		::gpk::transformDirection(transformedNormals, worldTransform);
		transformedNormals.A.Normalize();
		transformedNormals.B.Normalize();
		transformedNormals.C.Normalize();
		double																		directionFactorA							= transformedNormals.A.Dot(cameraFront);
		double																		directionFactorB							= transformedNormals.B.Dot(cameraFront);
		double																		directionFactorC							= transformedNormals.C.Dot(cameraFront);
		if(directionFactorA > .35 && directionFactorB > .35 && directionFactorC > .35)
			continue;

		output.Normals.push_back(transformedNormals);

		::gpk::STriangle3<float>													transformedPositions						= {positions[vertexIndices.A], positions[vertexIndices.B], positions[vertexIndices.C]};
		::gpk::transform(transformedPositions, worldTransform);
		output.PositionsWorld.push_back(transformedPositions);
		
		::gpk::transform(transformedPositions, projection);
		output.PositionsScreen.push_back(transformedPositions);
	
	}
	return 0; 
}

static	::gpk::error_t								drawBuffersWireframe
	( ::gpk::view_grid<::gpk::SColorBGRA>					& backBufferColors
	, ::gpk::view_grid<uint32_t>							& /*backBufferDepth*/
	, ::gpk::SVSOutput										& inVS
	, ::gpk::SVSCache										& cacheVS
	, ::gpk::view_array<const uint16_t>						indices		
	, ::gpk::view_array<const ::gpk::SCoord3<float>>		positions	
	, ::gpk::view_array<const ::gpk::SCoord3<float>>		normals		
	, const ::gpk::SRenderMaterial							& material
	, const ::gpk::SMatrix4<float>							& worldTransform	 
	, const ::gpk::SEngineSceneConstants					& constants
	) {	// 
	inVS												= {};
	::transformTrianglesWireframe(inVS, indices, positions, normals, constants.Projection, worldTransform, constants.CameraFront);
	const ::gpk::SCoord2<uint16_t>								offscreenMetrics			= backBufferColors.metrics().Cast<uint16_t>();
	for(uint32_t iTriangle = 0; iTriangle < inVS.PositionsScreen.size(); ++iTriangle) {
		const ::gpk::STriangle3<float>								& triPositions				= inVS.PositionsScreen	[iTriangle];
		if(triPositions.CulledZ({0, 0xFFFFFF}))
			continue;
		if(triPositions.CulledX({0, (float)offscreenMetrics.x}))
			continue;
		if(triPositions.CulledY({0, (float)offscreenMetrics.y}))
			continue;
		//const ::gpk::STriangle3<float>								& triNormals				= inVS.Normals			[iTriangle];
		cacheVS.WireframePixelCoords.clear();
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<int32_t>{triPositions.A.Cast<int32_t>(), triPositions.B.Cast<int32_t>()}, cacheVS.WireframePixelCoords);
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<int32_t>{triPositions.B.Cast<int32_t>(), triPositions.C.Cast<int32_t>()}, cacheVS.WireframePixelCoords);
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<int32_t>{triPositions.C.Cast<int32_t>(), triPositions.A.Cast<int32_t>()}, cacheVS.WireframePixelCoords);
		const ::gpk::SColorBGRA										wireColor					= material.Color.Diffuse;
		for(uint32_t iCoord = 0; iCoord < cacheVS.WireframePixelCoords.size(); ++iCoord) {
			::gpk::SCoord2<int16_t>								coord		= cacheVS.WireframePixelCoords[iCoord];
			backBufferColors[coord.y][coord.x]		= wireColor;
		}
	}
	return 0;
}

int32_t												gpk::shaderWireframe
	( ::gpk::view_grid<::gpk::SColorBGRA>	backBufferColors
	, ::gpk::view_grid<uint32_t>			backBufferDepth
	, ::gpk::SEngineRenderCache				& renderCache
	, const ::gpk::SEngineScene				& scene
	, const ::gpk::SEngineSceneConstants	& constants
	, int32_t								iRenderNode
	) {
	const ::gpk::SRenderNode								& renderNode			= scene.ManagedRenderNodes.RenderNodes[iRenderNode];
	const ::gpk::SRenderMesh								& mesh					= *scene.Graphics->Meshes[renderNode.Mesh];
	::gpk::vcc												meshName				= scene.Graphics->Meshes.Names[renderNode.Mesh];

	info_printf("Drawing node %i, mesh %i, slice %i, mesh name: %s", iRenderNode, renderNode.Mesh, renderNode.Slice, meshName.begin());

	const ::gpk::SRenderNodeTransforms						& transforms				= scene.ManagedRenderNodes.Transforms[iRenderNode];
	const ::gpk::SMatrix4<float>							& worldTransform			= transforms.World;
	const ::gpk::view_array<const uint16_t>					indices						= (mesh.GeometryBuffers.size() > 0) ? ::gpk::view_array<const uint16_t>					{(const uint16_t				*)scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.size() / sizeof(const uint16_t)}					: ::gpk::view_array<const uint16_t>					{};
	const ::gpk::view_array<const ::gpk::SCoord3<float>>	positions					= (mesh.GeometryBuffers.size() > 1) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)scene.Graphics->Buffers[mesh.GeometryBuffers[1]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[1]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
	const ::gpk::view_array<const ::gpk::SCoord3<float>>	normals						= (mesh.GeometryBuffers.size() > 2) ? ::gpk::view_array<const ::gpk::SCoord3<float>>	{(const ::gpk::SCoord3<float>	*)scene.Graphics->Buffers[mesh.GeometryBuffers[2]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[2]]->Data.size() / sizeof(const ::gpk::SCoord3<float>)}	: ::gpk::view_array<const ::gpk::SCoord3<float>>	{};
	const ::gpk::SSkin										& skin						= *scene.Graphics->Skins.Elements[renderNode.Skin];
	const ::gpk::SRenderMaterial							& material					= skin.Material;
	const ::gpk::SGeometrySlice								slice						= (renderNode.Slice < mesh.GeometrySlices.size()) ? mesh.GeometrySlices[renderNode.Slice] : ::gpk::SGeometrySlice{{0, indices.size() / 3}};

	return drawBuffersWireframe(backBufferColors, backBufferDepth, renderCache.OutputVertexShader, renderCache.CacheVertexShader, {&indices[slice.Slice.Offset], slice.Slice.Count}, positions, normals, material, worldTransform, constants);
}


int32_t												gpk::shaderHidden
	( ::gpk::view_grid<::gpk::SColorBGRA>	/*backBufferColors*/
	, ::gpk::view_grid<uint32_t>			/*backBufferDepth*/
	, ::gpk::SEngineRenderCache				& /*renderCache*/
	, const ::gpk::SEngineScene				& /*scene*/
	, const ::gpk::SEngineSceneConstants	& /*constants*/
	, int32_t								/*iRenderNode*/
	) {
	//info_printf("Drawing node %i, mesh %i, slice %i, mesh name: %s", iRenderNode, renderNode.Mesh, renderNode.Slice, meshName.begin());
	return 0;
}
