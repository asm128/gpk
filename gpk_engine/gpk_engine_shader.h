#include "gpk_color.h"
#include "gpk_engine_scene.h"
#include "gpk_raster_lh.h"

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

	template<typename _tCoord>
	static	::gpk::error_t						drawLine		(const ::gpk::SCoord2<uint16_t>& targetMetrics, const ::gpk::SLine3<_tCoord>& line, const ::gpk::SMatrix4<float> & mWVP, ::gpk::array_pod<::gpk::SCoord3<float>>& out_Points, ::gpk::view2d_uint32 depth) {
		::gpk::SCoord3<_tCoord>							vA				= mWVP.Transform(line.A); 
		::gpk::SCoord3<_tCoord>							vB				= mWVP.Transform(line.B);
		return ::gpk::drawLine(targetMetrics, ::gpk::SLine3<_tCoord>{vA, vB}, out_Points, depth);
	}

}  // namespace


#endif // GPK_ENGINE_SHADER_H