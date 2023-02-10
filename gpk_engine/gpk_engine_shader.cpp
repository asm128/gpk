#include "gpk_engine_shader.h"

::gpk::SColorFloat								gpk::lightCalcSpecular		(::gpk::n3<float> gEyePosW, float specularPower, ::gpk::SColorFloat specularLight, ::gpk::SColorFloat specularMaterial, ::gpk::n3<float> posW, ::gpk::n3<float> normalW, ::gpk::n3<float> lightVecW) {
	const ::gpk::n3<float>							pointToEye				= (gEyePosW - posW).Normalize();
	const ::gpk::n3<float>							reflected				= normalW.Reflect(-lightVecW);
	const double										reflectedFactor			= ::gpk::max(reflected.Dot(pointToEye), 0.0);
	if(0 == reflectedFactor) 
		return ::gpk::BLACK;

	const double										factor					= pow(reflectedFactor, specularPower);
	::gpk::SColorFloat									result					= specularMaterial * specularLight * factor;
	result.a										= specularMaterial.a;
	return result;
	
}

::gpk::SColorFloat								gpk::lightCalcDiffuse		(::gpk::SColorFloat diffuserMaterial, ::gpk::n3<float> normalW, ::gpk::n3<float> lightVecW) {
	double												lightFactor				= ::gpk::max(0.0, normalW.Dot(lightVecW));
	return lightFactor ? diffuserMaterial * (float)lightFactor : ::gpk::BLACK;
}

int32_t												gpk::psSolid
	( const ::gpk::SEngineSceneConstants	& constants
	, const ::gpk::SPSIn					& inPS
	, ::gpk::SColorBGRA						& outputPixel
	) {
	const ::gpk::n3<float>							lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::SColorFloat							diffuse						= ::gpk::lightCalcDiffuse(::gpk::DARKGREEN, inPS.WeightedNormal, lightVecW);
	outputPixel										= ::gpk::SColorFloat(diffuse).Clamp();
	return 0;
}

int32_t												gpk::psHidden
	( const ::gpk::SEngineSceneConstants	& /*constants*/
	, const ::gpk::SPSIn					& /*inPS*/
	, ::gpk::SColorBGRA						& /*outputPixel*/
	) {
	return 0;
}

