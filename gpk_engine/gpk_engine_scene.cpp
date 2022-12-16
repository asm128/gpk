#include "gpk_engine_scene.h"
#include "gpk_engine_shader.h"
#include "gpk_noise.h"
#include "gpk_ascii_color.h"

::gpk::error_t										gpk::drawScene									
	( ::gpk::view_grid<::gpk::SColorBGRA>	& backBufferColors
	, ::gpk::view_grid<uint32_t>			& backBufferDepth
	, ::gpk::SEngineRenderCache				& renderCache
	, const ::gpk::SEngineScene				& scene
	, const ::gpk::SEngineSceneConstants	& constants
) {	//

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

		if(renderNode.Shader >= scene.Graphics->Shaders.size()) 
			::gpk::shaderWireframe(backBufferColors, backBufferDepth, renderCache, scene, constants, iRenderNode); 
			//::the1::shaderBall(backBufferColors, backBufferDepth, renderCache, scene, constants, iRenderNode); 
		else {
			const ::std::function<TFuncEffect>				& fx					= *scene.Graphics->Shaders[renderNode.Shader];
			fx(backBufferColors, backBufferDepth, renderCache, scene, constants, iRenderNode);
		}
	}

	const ::gpk::SCoord2<uint16_t>					offscreenMetrics		= backBufferColors.metrics().Cast<uint16_t>();
	::gpk::array_pod<::gpk::SCoord2<int16_t>>		& wireframePixelCoords	= renderCache.CacheVertexShader.WireframePixelCoords;

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
		::gpk::drawLine(offscreenMetrics, ::gpk::SLine3<float>{{}, xyz[iVector]}, constants.Projection, wireframePixelCoords);
		for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord) {
			::gpk::SCoord2<int16_t>					coord		= wireframePixelCoords[iCoord];
			backBufferColors[coord.y][coord.x]	= colorXYZ[iVector];
		}
	}
	return 0;
}
