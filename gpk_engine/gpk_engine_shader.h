#include "gpk_color.h"
#include "gpk_engine_scene.h"
#include "gpk_raster_lh.h"

#ifndef GPK_ENGINE_SHADER_FUNC_H_23627
#define GPK_ENGINE_SHADER_FUNC_H_23627

nmsp gpk
{
	::gpk::rgbaf			lightCalcSpecular	(::gpk::n3f2_t gEyePosW, f2_t specularPower, ::gpk::rgbaf specularLight, ::gpk::rgbaf specularMaterial, ::gpk::n3f2_t posW, ::gpk::n3f2_t normalW, ::gpk::n3f2_t lightVecW);
	::gpk::rgbaf			lightCalcDiffuse	(::gpk::rgbaf diffuseMaterial, ::gpk::n3f2_t normalW, ::gpk::n3f2_t lightVecW);

	gpk::err_t 		psHidden			(cnst ::gpk::SEngineSceneConstants & constants, cnst ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	gpk::err_t 		psSolid				(cnst ::gpk::SEngineSceneConstants & constants, cnst ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);

	tplt<tpnm _tCoord>
	sttc	gpk::err_t drawLine			(cnst ::gpk::n2u1_t & targetMetrics, cnst ::gpk::line3<_tCoord> & line, cnst ::gpk::m4f2_t & mWVP, ::gpk::apod<::gpk::n3f2_t>& out_Points, ::gpk::gu2_t depth) {
		::gpk::n3<_tCoord>			vA					= mWVP.Transform(line.A); 
		::gpk::n3<_tCoord>			vB					= mWVP.Transform(line.B);
		return ::gpk::drawLine(targetMetrics, ::gpk::line3<_tCoord>{vA, vB}, out_Points, depth);
	}

	gpk::err_t 		psGrid				(cnst ::gpk::SEngineSceneConstants & constants, cnst ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	gpk::err_t 		psGridRuler			(cnst ::gpk::SEngineSceneConstants & constants, cnst ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	gpk::err_t 		psCylinderIn		(cnst ::gpk::SEngineSceneConstants & constants, cnst ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	gpk::err_t 		psCylinder			(cnst ::gpk::SEngineSceneConstants & constants, cnst ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	gpk::err_t 		psSphereAxis		(cnst ::gpk::SEngineSceneConstants & constants, cnst ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	gpk::err_t 		psSphereSolid		(cnst ::gpk::SEngineSceneConstants & constants, cnst ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);
	gpk::err_t 		psSphereMeridian	(cnst ::gpk::SEngineSceneConstants & constants, cnst ::gpk::SPSIn & inPS, ::gpk::bgra & outputPixel);

}  // nmsp


#endif // GPK_ENGINE_SHADER_FUNC_H_23627