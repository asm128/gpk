#include "gpk_engine_scene.h"

#ifndef GPK_ENGINE_D3D_SHADER_H
#define GPK_ENGINE_D3D_SHADER_H

namespace gpk
{
	::gpk::error_t										shaderD3DWireframe
		( ::gpk::view_grid<::gpk::SColorBGRA>	backBufferColors
		, ::gpk::view_grid<uint32_t>			backBufferDepth
		, ::gpk::SEngineRenderCache				& renderCache
		, const ::gpk::SEngineScene				& scene
		, const ::gpk::SEngineSceneConstants	& constants
		, int32_t								iRenderNode
		);
} // namespace

#endif // GPK_ENGINE_D3D_SHADER_H
