#include "gpk_color.h"
#include "gpk_engine_scene.h"
#include "gpk_raster_lh.h"

#ifndef GPK_ENGINE_SHADER_FUNC_H
#define GPK_ENGINE_SHADER_FUNC_H

namespace gpk
{
	::gpk::rgbaf			lightCalcSpecular	(::gpk::n3f32 gEyePosW, float specularPower, ::gpk::rgbaf specularLight, ::gpk::rgbaf specularMaterial, ::gpk::n3f32 posW, ::gpk::n3f32 normalW, ::gpk::n3f32 lightVecW);
	::gpk::rgbaf			lightCalcDiffuse	(::gpk::rgbaf diffuseMaterial, ::gpk::n3f32 normalW, ::gpk::n3f32 lightVecW);

	::gpk::error_t			psHidden			(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	::gpk::error_t			psSolid				(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);

	template<typename _tCoord>
	static	::gpk::error_t	drawLine			(const ::gpk::n2<uint16_t>& targetMetrics, const ::gpk::line3<_tCoord>& line, const ::gpk::m4f32 & mWVP, ::gpk::apod<::gpk::n3f32>& out_Points, ::gpk::v2u32 depth) {
		::gpk::n3<_tCoord>			vA					= mWVP.Transform(line.A); 
		::gpk::n3<_tCoord>			vB					= mWVP.Transform(line.B);
		return ::gpk::drawLine(targetMetrics, ::gpk::line3<_tCoord>{vA, vB}, out_Points, depth);
	}

}  // namespace


#endif // GPK_ENGINE_SHADER_FUNC_H