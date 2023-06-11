#include "gpk_engine_shader.h"

::gpk::rgbaf			gpk::lightCalcSpecular	(::gpk::n3f32 gEyePosW, float specularPower, ::gpk::rgbaf specularLight, ::gpk::rgbaf specularMaterial, ::gpk::n3f32 posW, ::gpk::n3f32 normalW, ::gpk::n3f32 lightVecW) {
	const ::gpk::n3f32			pointToEye				= (gEyePosW - posW).Normalize();
	const ::gpk::n3f32			reflected				= normalW.Reflect(-lightVecW);
	const double				reflectedFactor			= ::gpk::max(reflected.Dot(pointToEye), 0.0);
	if(0 == reflectedFactor) 
		return ::gpk::BLACK;

	const double				factor					= pow(reflectedFactor, specularPower);
	::gpk::rgbaf				result					= specularMaterial * specularLight * factor;
	result.a					= specularMaterial.a;
	return result;
	
}

::gpk::rgbaf			gpk::lightCalcDiffuse	(::gpk::rgbaf diffuserMaterial, ::gpk::n3f32 normalW, ::gpk::n3f32 lightVecW) {
	double						lightFactor				= ::gpk::max(0.0, normalW.Dot(lightVecW));
	return lightFactor ? diffuserMaterial * (float)lightFactor : ::gpk::BLACK;
}

::gpk::error_t			gpk::psSolid
	( const ::gpk::SEngineSceneConstants	& constants
	, const ::gpk::SPSIn					& inPS
	, ::gpk::bgra							& outputPixel
	) {
	const ::gpk::n3f32			lightVecW				= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::rgbaf			diffuse					= ::gpk::lightCalcDiffuse(::gpk::DARKGREEN, inPS.WeightedNormal, lightVecW);
	outputPixel				= ::gpk::rgbaf(diffuse).Clamp();
	return 0;
}

::gpk::error_t			gpk::psHidden
	( const ::gpk::SEngineSceneConstants	& /*constants*/
	, const ::gpk::SPSIn					& /*inPS*/
	, ::gpk::bgra							& /*outputPixel*/
	) {
	return 0;
}

