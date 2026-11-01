#include "gpk_engine_geometry.h"
#include "gpk_engine_scene.h"
#include "gpk_engine_shader.h"
#include "gpk_noise.h"
#include "gpk_ascii_color.h"
#include "gpk_raster_lh.h"
#include "gpk_view_n3.h"
#include "gpk_view_n2.h"
#include "gpk_view_tri.h"

tplt <tpnm _tIndex>
static	::gpk::err_t	transformTriangles					
	( ::gpk::SVSOutput		& output
	, ::gpk::view<const _tIndex>	indices			
	, ::gpk::vc3f2_t			positions	
	, ::gpk::vc3f2_t			normals		
	, ::gpk::vc2f2_t			uv			
	, const ::gpk::m4f2_t	& projection		
	, const ::gpk::m4f2_t	& worldTransform	
	, const ::gpk::n3f2_t	& cameraFront
)	{ 
	::gpk::vctri<_tIndex>				view_indices			= {(const ::gpk::tri<_tIndex>*)indices.begin(), indices.size() / 3};
	const ::gpk::m4f2_t			mWVPS					= worldTransform * projection;

	for(u2_t iTriangle = 0; iTriangle < view_indices.size(); ++iTriangle) {
		const ::gpk::tri<_tIndex>	vertexIndices			= view_indices[iTriangle];
		::gpk::tri3f2_t				transformedNormals		= {normals[vertexIndices.A], normals[vertexIndices.B], normals[vertexIndices.C]};
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

		::gpk::tri3f2_t				transformedPositions	= {positions[vertexIndices.A], positions[vertexIndices.B], positions[vertexIndices.C]};
		::gpk::transform(transformedPositions, worldTransform);
		output.PositionsWorld.push_back(transformedPositions);
		
		::gpk::transform(transformedPositions, projection);
		output.PositionsScreen.push_back(transformedPositions);
		
		::gpk::tri2f2_t				transformedUVs			= {uv[vertexIndices.A], uv[vertexIndices.B], uv[vertexIndices.C]};
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

static	::gpk::err_t	transformTriangles
	( ::gpk::SEngineRenderCache				& renderCache
	, const ::gpk::SEngineScene				& scene
	, const ::gpk::SEngineSceneConstants	& constants
	, s2_t								iRenderNode
	) {
	const ::gpk::SRenderNode	& renderNode			= scene.RenderNodes.RenderNodes[iRenderNode];
	const ::gpk::SGeometryMesh	& mesh					= *scene.Graphics->Meshes[renderNode.Mesh];
	verbose_printf("Drawing node %i, mesh %i, slice %i, mesh name: %s", iRenderNode, renderNode.Mesh, renderNode.Slice, scene.Graphics->Meshes.Names[renderNode.Mesh].begin());


	renderCache.VertexShaderOutput	= {};

	if(mesh.GeometryBuffers.size()) {
		::gpk::SRenderNodeConstants	transforms				= scene.RenderNodes.Transforms[iRenderNode];
		const ::gpk::m4f2_t			& worldTransform		= transforms.Model;

		const ::gpk::vc3f2_t			positions				= (mesh.GeometryBuffers.size() > 1) ? ::gpk::vc3f2_t	{(const ::gpk::n3f2_t*)scene.Graphics->Buffers[mesh.GeometryBuffers[1]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[1]]->Data.size() / (u0_t)sizeof(const ::gpk::n3f2_t)}	: ::gpk::vc3f2_t{};
		const ::gpk::vc3f2_t			normals					= (mesh.GeometryBuffers.size() > 2) ? ::gpk::vc3f2_t	{(const ::gpk::n3f2_t*)scene.Graphics->Buffers[mesh.GeometryBuffers[2]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[2]]->Data.size() / (u0_t)sizeof(const ::gpk::n3f2_t)}	: ::gpk::vc3f2_t{};
		const ::gpk::vc2f2_t			uv						= (mesh.GeometryBuffers.size() > 3) ? ::gpk::vc2f2_t	{(const ::gpk::n2f2_t*)scene.Graphics->Buffers[mesh.GeometryBuffers[3]]->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[3]]->Data.size() / (u0_t)sizeof(const ::gpk::n2f2_t)}	: ::gpk::vc2f2_t{};

		transforms.Model					= scene.RenderNodes.BaseTransforms[iRenderNode].Model * transforms.Model;
		::gpk::pobj<::gpk::SRenderBuffer>	indexBuffer		= scene.Graphics->Buffers[mesh.GeometryBuffers[0]];
		if(indexBuffer->Desc.Format.TotalBytes() == 1) {
			const ::gpk::vcu0_t			indices					= ::gpk::vcu0_t{(const u0_t*)indexBuffer->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.size() / (u0_t)sizeof(const u0_t)};
			const ::gpk::SGeometrySlice	slice					= (renderNode.Slice < mesh.GeometrySlices.size()) ? mesh.GeometrySlices[renderNode.Slice] : ::gpk::SGeometrySlice{{0, indices.size() / 3}};
			::transformTriangles(renderCache.VertexShaderOutput, indices, positions, normals, uv, constants.VPS, worldTransform, constants.CameraFront);
		}
		else if(indexBuffer->Desc.Format.TotalBytes() == 2) {
			const ::gpk::vcu1_t			indices					= ::gpk::vcu1_t{(const u1_t*)indexBuffer->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.size() / (u0_t)sizeof(const u1_t)};
			const ::gpk::SGeometrySlice	slice					= (renderNode.Slice < mesh.GeometrySlices.size()) ? mesh.GeometrySlices[renderNode.Slice] : ::gpk::SGeometrySlice{{0, indices.size() / 3}};
			::transformTriangles(renderCache.VertexShaderOutput, indices, positions, normals, uv, constants.VPS, worldTransform, constants.CameraFront);
		}
		else if(indexBuffer->Desc.Format.TotalBytes() == 4) {
			const ::gpk::vcu2_t			indices					= ::gpk::vcu2_t{(const u2_t*)indexBuffer->Data.begin(), scene.Graphics->Buffers[mesh.GeometryBuffers[0]]->Data.size() / (u0_t)sizeof(const u1_t)};
			const ::gpk::SGeometrySlice	slice					= (renderNode.Slice < mesh.GeometrySlices.size()) ? mesh.GeometrySlices[renderNode.Slice] : ::gpk::SGeometrySlice{{0, indices.size() / 3}};
			::transformTriangles(renderCache.VertexShaderOutput, indices, positions, normals, uv, constants.VPS, worldTransform, constants.CameraFront);
		}
	}
	return 0;
}


static	::gpk::err_t	drawBuffers
	( ::gpk::g8bgra										& backBufferColors
	, ::gpk::gu32										backBufferDepth
	, ::gpk::SVSOutput									& outVS
	, ::gpk::SVSCache									& cacheVS
	, const ::gpk::SRenderMaterial						& material
	, ::gpk::gc8bgra									surface
	, const ::gpk::SEngineSceneConstants				& constants
	, const ::gpk::SRenderNodeConstants					& nodeConstants
	, const ::std::function<::gpk::TFuncPixelShader>	& ps
	) {	// 
	::gpk::atrif2_t				& triangleWeights			= cacheVS.TriangleWeights		;
	::gpk::an2s1_t				& trianglePixelCoords		= cacheVS.SolidPixelCoords		;
	const ::gpk::n2u1_t			offscreenMetrics			= backBufferColors.metrics().u1();
	const ::gpk::n3f2_t			lightDirectionNormalized	= ::gpk::n3f2_t{constants.LightDirection}.Normalize();
	::gpk::SPSIn				inPS						= {nodeConstants};
	inPS.Surface			= surface;
	inPS.Material			= material;

	for(u2_t iTriangle = 0; iTriangle < outVS.PositionsScreen.size(); ++iTriangle) {
		const ::gpk::tri3f2_t		& triPositions				= outVS.PositionsScreen	[iTriangle];
		if( triPositions.CulledZ({0, 0xFFFFFF})
		 || triPositions.CulledX({0, (float)offscreenMetrics.x})
		 || triPositions.CulledY({0, (float)offscreenMetrics.y})
		)
			continue;

		const ::gpk::tri3f2_t		& triPositionsWorld			= outVS.PositionsWorld	[iTriangle];
		const ::gpk::tri3f2_t		& triNormals				= outVS.Normals			[iTriangle];
		const ::gpk::tri2f2_t		& triUVs					= outVS.UVs				[iTriangle];

		trianglePixelCoords.clear();
		triangleWeights.clear();
		if_fail_e(::gpk::drawTriangle(offscreenMetrics.u2(), triPositions, trianglePixelCoords, triangleWeights, backBufferDepth));
		//const bool													stripped					= surface[0][0] == ::gpk::bgra{gpk::WHITE};
		for(u2_t iCoord = 0; iCoord < trianglePixelCoords.size(); ++iCoord) {
			const ::gpk::trif2_t							& vertexWeights				= triangleWeights[iCoord];
			inPS.WeightedPosition								= triPositionsWorld.A * vertexWeights.A + triPositionsWorld.B * vertexWeights.B + triPositionsWorld.C * vertexWeights.C;
			inPS.WeightedNormal									= (triNormals.A * vertexWeights.A + triNormals.B * vertexWeights.B + triNormals.C * vertexWeights.C).Normalize();
			inPS.WeightedUV										= triUVs.A * vertexWeights.A + triUVs.B * vertexWeights.B + triUVs.C * vertexWeights.C;
			const ::gpk::n2u1_t										coord						= trianglePixelCoords[iCoord].u1();
			ps(constants, inPS, backBufferColors[coord.y][coord.x]);
		}
	}
	return 0;
}

::gpk::err_t			gpk::drawScene									
	( ::gpk::g8bgra				backBufferColors
	, ::gpk::gu32				backBufferDepth
	, ::gpk::SEngineRenderCache	& renderCache
	, const ::gpk::SEngineScene	& scene
	, const ::gpk::n3f2_t		& cameraPosition
	, const ::gpk::n3f2_t		& cameraTarget
	, const ::gpk::n3f2_t		& cameraUp
	, const ::gpk::minmaxf2_t	& nearFar
) {	//
	const ::gpk::n3f2_t			cameraFront			= (cameraTarget - cameraPosition).Normalize();
	const ::gpk::n2u1_t			offscreenMetrics	= backBufferColors.metrics16();

	::gpk::SEngineSceneConstants	constants		= {};
	constants.CameraPosition	= cameraPosition;
	constants.CameraFront		= cameraFront;

	constants.View.LookAt(cameraPosition, cameraTarget, cameraUp);
	constants.Perspective.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar.Min, nearFar.Max);
	constants.Screen.ViewportLH(offscreenMetrics);
	constants.VP			= constants.View * constants.Perspective;
	constants.VPS			= constants.VP * constants.Screen;

	static ::std::function<::gpk::TFuncPixelShader>	defaultShader			= {::gpk::psSolid};
	for(u2_t iRenderNode = 0, countNodes = scene.RenderNodes.RenderNodes.size(); iRenderNode < countNodes; ++iRenderNode) {
		const ::gpk::SRenderNodeFlags	& renderNodeFlags		= scene.RenderNodes.Flags[iRenderNode];
		if(renderNodeFlags.NoDraw)
			continue;

		const ::gpk::SRenderNode		& renderNode		= scene.RenderNodes.RenderNodes[iRenderNode];
		if(renderNode.Mesh >= scene.Graphics->Meshes.size())
			continue;

		info_printf("Mesh name: %s", scene.Graphics->Meshes.Names[renderNode.Mesh].begin());
		
		::gpk::SRenderNodeConstants		transforms			= scene.RenderNodes.Transforms[iRenderNode];
		transforms.NodeSize	= scene.RenderNodes.BaseTransforms[iRenderNode].NodeSize;
		const ::gpk::m4f2_t				& worldTransform	= transforms.Model;
		if((worldTransform.GetTranslation() - constants.CameraPosition).Normalize().Dot(constants.CameraFront) <= 0)
			return 0;

		if_true_cef(failed(::transformTriangles(renderCache, scene, constants, iRenderNode)), "iRenderNode %i", iRenderNode);

		const ::gpk::SSkin				& skin				= *scene.Graphics->Skins.Elements[renderNode.Skin];
		const ::gpk::SRenderMaterial	& material			= skin.Material;
		const u2_t					tex					= skin.Textures[0];
		const ::gpk::SSurface			& surface			= *scene.Graphics->Surfaces[tex];
		const ::std::function<::gpk::TFuncPixelShader>	& fx
			= (renderNode.Shader >= scene.Graphics->Shaders.size()) ? defaultShader : *scene.Graphics->Shaders[renderNode.Shader];

		drawBuffers(backBufferColors, backBufferDepth, renderCache.VertexShaderOutput, renderCache.VertexShaderCache, material, {(const ::gpk::bgra*)surface.Data.begin(), surface.Desc.Dimensions.u2()}, constants, transforms, fx);
	}

	::gpk::apod<::gpk::n3f2_t>					& wireframePixelCoords	= renderCache.VertexShaderCache.WireframePixelCoords;

	// ---- Draw axis vectors at the origin (0, 0, 0)
	constexpr ::gpk::n3f2_t						xyz	[3]					= 
		{ {1}
		, {0, 1}
		, {0, 0, 1}
		};
	constexpr ::gpk::bgra						colorXYZ[3]				= 
		{ ::gpk::RED
		, ::gpk::GREEN
		, ::gpk::BLUE
		};

	for(u2_t iVector = 0; iVector < 3; ++iVector) {
		wireframePixelCoords.clear();
		::gpk::drawLine(offscreenMetrics, ::gpk::line3f2_t{{}, xyz[iVector]}, constants.VPS, wireframePixelCoords, backBufferDepth);
		for(u2_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord) {
			::gpk::n3u1_t									coord					= wireframePixelCoords[iCoord].u1();
			backBufferColors[coord.y][coord.x]	= colorXYZ[iVector];
		}
	}
	return 0;
}

::gpk::err_t			gpk::createBuffers	
	( const ::gpk::SGeometryBuffers		& geometry
	, ::gpk::pobj<::gpk::SRenderBuffer>	& pIndicesVertex
	, ::gpk::pobj<::gpk::SRenderBuffer>	& pVertices
	, ::gpk::pobj<::gpk::SRenderBuffer>	& pNormals
	, ::gpk::pobj<::gpk::SRenderBuffer>	& pUV
	) {
	if_fail_fe(gpk::createBuffers(geometry.Positions.size(), pIndicesVertex, pVertices, pNormals, pUV));

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
		::gpk::vu1_t					viewIndices					= ::gpk::vu1_t{(u1_t*)pIndicesVertex->Data.begin(), geometry.PositionIndices.size()};
		viewIndices.enumerate([&geometry](u2_t index, u1_t & value){ value = (u1_t)geometry.PositionIndices[index]; return 0; }, 0);
	}
	else {
		pIndicesVertex->Data.resize(geometry.PositionIndices.byte_count() >> 2);
		::gpk::vu0_t					viewIndices					= ::gpk::vu0_t{(u0_t*)pIndicesVertex->Data.begin(), geometry.PositionIndices.size()};
		viewIndices.enumerate([&geometry](u2_t index, u0_t & value){ value = (u0_t)geometry.PositionIndices[index]; return 0; }, 0);
	}

	return 0;
}

::gpk::err_t			gpk::createBuffers
	( ::gpk::SRenderBufferManager	& bufferManager 
	, const ::gpk::SGeometryBuffers	& geometry
	, u2_t						& iIndicesVertex
	, u2_t						& iVertices
	, u2_t						& iNormals
	, u2_t						& iUV
	) {
	::gpk::pobj<::gpk::SRenderBuffer>	pIndicesVertex, pVertices, pNormals, pUV;
	if_fail_fe(gpk::createBuffers(geometry, pIndicesVertex, pVertices, pNormals, pUV));
	if_fail_fe(iIndicesVertex			= (u2_t)bufferManager.push_back(pIndicesVertex));
	if_fail_fe(iVertices				= (u2_t)bufferManager.push_back(pVertices));
	if_fail_fe(iNormals				= (u2_t)bufferManager.push_back(pNormals));
	if_fail_fe(iUV					= (u2_t)bufferManager.push_back(pUV));
	return 0;
}

::gpk::err_t			gpk::SEngineScene::CreateRenderNode
	( const ::gpk::SGeometryBuffers		& geometry
	, const ::gpk::view<sc_t>				name 
	, bool								createSkin
	) {
	u2_t					iIndicesVertex			= 0;
	u2_t					iVertices				= 0;
	u2_t					iNormals				= 0;
	u2_t					iUV						= 0;
	::gpk::createBuffers(Graphics->Buffers, geometry, iIndicesVertex, iVertices, iNormals, iUV);

	u2_t					iMesh					= (u2_t)Graphics->Meshes.Create();
	::gpk::pobj<::gpk::SGeometryMesh>	& mesh			= Graphics->Meshes[iMesh];
	Graphics->Meshes.Names[iMesh] = name;
	mesh->GeometryBuffers.append({iIndicesVertex, iVertices, iNormals, iUV});

	mesh->Desc.Mode			= ::gpk::MESH_MODE_List;
	mesh->Desc.Type			= ::gpk::GEOMETRY_TYPE_Triangle;
	mesh->Desc.NormalMode	= ::gpk::NORMAL_MODE_Point;


	u2_t					iRenderNode				= RenderNodes.Create();
	::gpk::SRenderNode			& renderNode			= RenderNodes.RenderNodes[iRenderNode];
	renderNode.Mesh			= iMesh;
	if(createSkin) {
		u2_t					iSkin					= (u2_t)Graphics->Skins.Create();
		::gpk::pobj<::gpk::SSkin>	& skin					= Graphics->Skins.Elements[iSkin];
		skin->Material.Color.Ambient	= ::gpk::bgra(::gpk::ASCII_PALETTE[3]) * .1f;
		skin->Material.Color.Diffuse	= ::gpk::bgra(::gpk::ASCII_PALETTE[3]);
		skin->Material.Color.Specular	= ::gpk::WHITE;
		skin->Material.SpecularPower	= 0.5f;

		u2_t					iSurface				= (u2_t)Graphics->Surfaces.Create();
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
		::gpk::g8bgra				view					= {(::gpk::bgra*)surface->Data.begin(), surface->Desc.Dimensions.u2()};
		::gpk::rgba					color					= {::gpk::ASCII_PALETTE[rand() % 16]};
		for(u2_t y = surface->Desc.Dimensions.y / 3; y < surface->Desc.Dimensions.y / 3U * 2U; ++y)
		for(u2_t x = 0; x < surface->Desc.Dimensions.x; ++x)
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
