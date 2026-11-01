#include "gpk_color.h"
#include "gpk_engine_scene.h"
#include "gpk_raster_lh.h"

#ifndef GPK_ENGINE_SHADER_FUNC_H_23627
#define GPK_ENGINE_SHADER_FUNC_H_23627

namespace gpk
{
	::gpk::rgbaf			lightCalcSpecular	(::gpk::n3f2_t gEyePosW, float specularPower, ::gpk::rgbaf specularLight, ::gpk::rgbaf specularMaterial, ::gpk::n3f2_t posW, ::gpk::n3f2_t normalW, ::gpk::n3f2_t lightVecW);
	::gpk::rgbaf			lightCalcDiffuse	(::gpk::rgbaf diffuseMaterial, ::gpk::n3f2_t normalW, ::gpk::n3f2_t lightVecW);

	::gpk::err_t			psHidden			(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	::gpk::err_t			psSolid				(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);

	tplt<tpnm _tCoord>
	static	::gpk::err_t	drawLine			(const ::gpk::n2u1_t & targetMetrics, const ::gpk::line3<_tCoord> & line, const ::gpk::m4f2_t & mWVP, ::gpk::apod<::gpk::n3f2_t>& out_Points, ::gpk::gu32 depth) {
		::gpk::n3<_tCoord>			vA					= mWVP.Transform(line.A); 
		::gpk::n3<_tCoord>			vB					= mWVP.Transform(line.B);
		return ::gpk::drawLine(targetMetrics, ::gpk::line3<_tCoord>{vA, vB}, out_Points, depth);
	}

	::gpk::err_t			psGrid				(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	::gpk::err_t			psGridRuler			(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	::gpk::err_t			psCylinderIn		(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	::gpk::err_t			psCylinder			(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	::gpk::err_t			psSphereAxis		(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	::gpk::err_t			psSphereSolid		(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	::gpk::err_t			psSphereMeridian	(const ::gpk::SEngineSceneConstants & constants, const ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);

}  // namespace


#endif // GPK_ENGINE_SHADER_FUNC_H_23627