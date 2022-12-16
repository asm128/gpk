#include "gpk_color.h"
#include "gpk_engine_scene.h"

#ifndef GPK_ENGINE_SHADER_H
#define GPK_ENGINE_SHADER_H

namespace gpk
{
	::gpk::SColorFloat			lightCalcSpecular		(::gpk::SCoord3<float> gEyePosW, float specularPower, ::gpk::SColorFloat specularLight, ::gpk::SColorFloat specularMaterial, ::gpk::SCoord3<float> posW, ::gpk::SCoord3<float> normalW, ::gpk::SCoord3<float> lightVecW);
	::gpk::SColorFloat			lightCalcDiffuse		(::gpk::SColorFloat diffuserMaterial, ::gpk::SCoord3<float> normalW, ::gpk::SCoord3<float> lightVecW);

	::gpk::error_t				shaderWireframe
		( ::gpk::view_grid<::gpk::SColorBGRA>	backBufferColors
		, ::gpk::view_grid<uint32_t>			backBufferDepth
		, ::gpk::SEngineRenderCache				& renderCache
		, const ::gpk::SEngineScene				& scene
		, const ::gpk::SEngineSceneConstants	& constants
		, int32_t								iRenderNode
		);
	::gpk::error_t				shaderHidden
		( ::gpk::view_grid<::gpk::SColorBGRA>	backBufferColors
		, ::gpk::view_grid<uint32_t>			backBufferDepth
		, ::gpk::SEngineRenderCache				& renderCache
		, const ::gpk::SEngineScene				& scene
		, const ::gpk::SEngineSceneConstants	& constants
		, int32_t								iRenderNode
		);
}  // namespace


#endif // GPK_ENGINE_SHADER_H