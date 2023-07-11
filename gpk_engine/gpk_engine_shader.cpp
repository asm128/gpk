#include "gpk_engine_shader.h"
#include "gpk_noise.h"

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

::gpk::error_t			gpk::psGrid
	( const ::gpk::SEngineSceneConstants	& constants
	, const ::gpk::SPSIn					& inPS
	, ::gpk::bgra							& outputPixel
	) { 
	const ::gpk::n3f32			lightVecW				= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::rgbaf			diffuse					= ::gpk::lightCalcDiffuse(::gpk::DARKGREEN, inPS.WeightedNormal, lightVecW);
	outputPixel				= ::gpk::rgbaf(diffuse).Clamp();
	return 0; 
}

::gpk::error_t			gpk::psGridRuler
	( const ::gpk::SEngineSceneConstants	& constants
	, const ::gpk::SPSIn					& inPS
	, ::gpk::bgra							& outputPixel
	) { 
	::gpk::n2f32				scaledUV				= {inPS.WeightedUV.x * inPS.NodeConstants.NodeSize.x, inPS.WeightedUV.y * inPS.NodeConstants.NodeSize.z};
	if( 0.0025f > (scaledUV - ::gpk::n2f32{0, 0}).LengthSquared()
	 || 0.0025f > (scaledUV - ::gpk::n2f32{inPS.NodeConstants.NodeSize.x, 0}).LengthSquared()
	 || 0.0025f > (scaledUV - ::gpk::n2f32{0, inPS.NodeConstants.NodeSize.z}).LengthSquared()
	 || 0.0025f > (scaledUV - ::gpk::n2f32{inPS.NodeConstants.NodeSize.x, inPS.NodeConstants.NodeSize.z}).LengthSquared()
	 || 0.0025f > (scaledUV - ::gpk::n2f32{inPS.NodeConstants.NodeSize.x * .5f, inPS.NodeConstants.NodeSize.z}).LengthSquared()
	 || 0.0025f > (scaledUV - ::gpk::n2f32{inPS.NodeConstants.NodeSize.x * .5f, 0}).LengthSquared()
	 || 0.0025f > (scaledUV - ::gpk::n2f32{inPS.NodeConstants.NodeSize.x, inPS.NodeConstants.NodeSize.z * .5f}).LengthSquared()
	 || 0.0025f > (scaledUV - ::gpk::n2f32{0, inPS.NodeConstants.NodeSize.z * .5f}).LengthSquared()
	) {
		return 0;
	}
	else {
		::gpk::n3f32				lightVecW				= (constants.LightPosition - inPS.WeightedPosition).Normalize();
		double						diffuseFactor			= ::gpk::max(lightVecW.Dot(inPS.WeightedNormal.Normalized()), 0.0);
		::gpk::rgbaf				materialColor			= 
			 ( (scaledUV.x > inPS.NodeConstants.NodeSize.x * .5 - .005 && scaledUV.x < inPS.NodeConstants.NodeSize.x * .5 + .005) 
			|| (scaledUV.y > inPS.NodeConstants.NodeSize.z * .5 - .005 && scaledUV.y < inPS.NodeConstants.NodeSize.z * .5 + .005)
			)	? ::gpk::rgbaf(::gpk::WHITE.rgb(), inPS.Material.Color.Diffuse.a)
				: inPS.Material.Color.Diffuse
				;
		::gpk::rgbaf				diffuse					= diffuseFactor ? ::gpk::rgbaf(((materialColor * (double)diffuseFactor) * ::gpk::max(.65, ::gpk::noise2D(uint32_t(inPS.WeightedUV.x * 10000), uint32_t(inPS.WeightedUV.y * 10000), 10000))).rgb(), materialColor.a) : ::gpk::BLACK;
		return outputPixel		= diffuse.Clamp();
	}
}

::gpk::error_t			gpk::psCylinderIn
	( const ::gpk::SEngineSceneConstants	& constants
	, const ::gpk::SPSIn					& inPS
	, ::gpk::bgra							& outputPixel
	) { 
	const ::gpk::n3f32			lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::rgbaf			diffuse						= ::gpk::lightCalcDiffuse(::gpk::DARKGRAY, inPS.WeightedNormal, lightVecW);
	outputPixel				= ::gpk::rgbaf(diffuse).Clamp();
	return 0; 
}

stacxpr	float			LIGHT_FACTOR_AMBIENT		= .025f;
stacxpr	float			LIGHT_FACTOR_SPECULAR_POWER	= 30.0f;
stacxpr	::gpk::bgra8	PIXEL_BLACK_NUMBER			= ::gpk::bgra{0, 0, 0, 255};

::gpk::error_t			gpk::psCylinder
	( const ::gpk::SEngineSceneConstants	& constants
	, const ::gpk::SPSIn					& inPS
	, ::gpk::bgra							& outputPixel
	) { 
	::gpk::rgbaf				materialcolor				= ::gpk::BROWN + (::gpk::ORANGE * .5f);
	const ::gpk::n3f32			lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::rgbaf			diffuse						= ::gpk::lightCalcDiffuse(materialcolor, inPS.WeightedNormal, lightVecW);
	const ::gpk::rgbaf			ambient						= materialcolor * ::LIGHT_FACTOR_AMBIENT;

	outputPixel				= ::gpk::rgbaf(ambient + diffuse).Clamp();
	return 0; 
}

::gpk::error_t			gpk::psSphereAxis				
	( const ::gpk::SEngineSceneConstants	& constants
	, const ::gpk::SPSIn					& inPS
	, ::gpk::bgra							& outputPixel
	) { 
	::gpk::n2f32				relativeToCenter			= ::gpk::n2f32{inPS.WeightedUV.x, inPS.WeightedUV.y} - ::gpk::n2f32{.5f, .5f};
	relativeToCenter.x		*= 2;

	const ::gpk::bgra			surfacecolor				
		= ((::gpk::n2f32{ 0.0f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
		: ((::gpk::n2f32{ 1.0f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
		: ((::gpk::n2f32{-1.0f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
		: ((::gpk::n2f32{ 0.5f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
		: ((::gpk::n2f32{-0.5f, 0.0f} - relativeToCenter).LengthSquared() < .0025f) ? ::gpk::RED 
		: (( 0.5f - relativeToCenter.y) <  .05f) ? ::gpk::RED 
		: ((-0.5f - relativeToCenter.y) > -.05f) ? ::gpk::RED 
		: ::gpk::WHITE;
	::gpk::rgbaf				materialcolor				= surfacecolor;		
	const ::gpk::n3f32			lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
	const ::gpk::rgbaf			specular					= ::gpk::lightCalcSpecular(constants.CameraPosition, ::LIGHT_FACTOR_SPECULAR_POWER, gpk::WHITE, ::gpk::WHITE, inPS.WeightedPosition, inPS.WeightedNormal, lightVecW);
	const ::gpk::rgbaf			diffuse						= ::gpk::lightCalcDiffuse(materialcolor, inPS.WeightedNormal, lightVecW);
	const ::gpk::rgbaf			ambient						= materialcolor * ::LIGHT_FACTOR_AMBIENT;

	outputPixel				= (0 == surfacecolor.g) ? ::gpk::RED : ::gpk::rgbaf(ambient + diffuse + specular).Clamp();
	return 0; 
}

::gpk::error_t			gpk::psSphereSolid				
	( const ::gpk::SEngineSceneConstants	& constants
	, const ::gpk::SPSIn					& inPS
	, ::gpk::bgra							& outputPixel
	) { 
	const ::gpk::n2u32			& surfaceSize				= inPS.Surface.metrics();
	::gpk::n2f32				relativeToCenter			= ::gpk::n2f32{inPS.WeightedUV.x, inPS.WeightedUV.y} - ::gpk::n2f32{.5f, .5f};
	relativeToCenter.x		*= 2;

	::gpk::rgbaf				materialColor;
	bool						shade						= false;
	float						ambientFactor				= ::LIGHT_FACTOR_AMBIENT;
	if((::gpk::n2f32{0.0f, 0.0f} - relativeToCenter).LengthSquared() >= .0225f) {
		materialColor			= inPS.Material.Color.Diffuse;
		shade					= true;
	}
	else {
		const ::gpk::n2u32			fetchCoord					= 
			{ (uint32_t)(relativeToCenter.x * 2.f * surfaceSize.x + surfaceSize.x / 2)
			, (uint32_t)(relativeToCenter.y * 4.f * surfaceSize.y + surfaceSize.y / 2)
			};
		const ::gpk::bgra			surfacecolor				= inPS.Surface
			[fetchCoord.y % surfaceSize.y]
			[fetchCoord.x % surfaceSize.x]
			;
		if(surfacecolor != PIXEL_BLACK_NUMBER) {
			materialColor			= ::gpk::WHITE;
			shade					= rand() % 2;
			ambientFactor			= .65f;
		}
		else {
			materialColor			= gpk::BLACK;
			shade					= true;
		}
	}

	if(shade) {
		const ::gpk::n3f32			lightVecW					= (constants.LightPosition - inPS.WeightedPosition).Normalize();
		double						diffuseFactor				= inPS.WeightedNormal.Dot(lightVecW);
		if(diffuseFactor < 0) {
			ambientFactor			+= (rand() % 256) / 255.0f * float(diffuseFactor) * -.25f;
			materialColor			= (materialColor * ambientFactor).Clamp();
		}
		else {
			const ::gpk::rgbaf			specular					= ::gpk::lightCalcSpecular(constants.CameraPosition, ::LIGHT_FACTOR_SPECULAR_POWER, gpk::WHITE, ::gpk::WHITE, inPS.WeightedPosition, inPS.WeightedNormal, lightVecW);
			const ::gpk::rgbaf			diffuse						= materialColor * ::gpk::max(0.0, diffuseFactor);
			const ::gpk::rgbaf			ambient						= materialColor * ambientFactor;
			materialColor			= ::gpk::rgbaf(ambient + diffuse + specular).Clamp();
		}
	}
	outputPixel				= materialColor;
	return 0; 
}

::gpk::error_t			gpk::psSphereMeridian			
	( const ::gpk::SEngineSceneConstants	& constants
	, const ::gpk::SPSIn					& inPS
	, ::gpk::bgra							& outputPixel
	) { 
	const ::gpk::n2u32			& surfaceSize			= inPS.Surface.metrics();
	::gpk::n2f32				relativeToCenter		= ::gpk::n2f32{inPS.WeightedUV.x, inPS.WeightedUV.y} - ::gpk::n2f32{.5f, .5f};
	relativeToCenter.x		*= 2;

	::gpk::rgbaf				materialColor;
	bool						shade					= false;
	float						ambientFactor			= ::LIGHT_FACTOR_AMBIENT;
	if((::gpk::n2f32{0.0f, 0.0f} - relativeToCenter).LengthSquared() >= .0225f) {
		materialColor							
			= (relativeToCenter.y >  .20f) ? ::gpk::WHITE
			: (relativeToCenter.y < -.20f) ? ::gpk::WHITE
			: inPS.Material.Color.Diffuse
			;
		shade				= true;
	}
	else {
		const ::gpk::n2u32			fetchCoord				= 
			{ (uint32_t)(relativeToCenter.x * 2.f * surfaceSize.x + surfaceSize.x / 2)
			, (uint32_t)(relativeToCenter.y * 4.f * surfaceSize.y + surfaceSize.y / 2)
			};
		const ::gpk::bgra			surfacecolor			= inPS.Surface
			[fetchCoord.y % surfaceSize.y]
			[fetchCoord.x % surfaceSize.x]
			;

		if(surfacecolor != PIXEL_BLACK_NUMBER) {
			materialColor		= ::gpk::WHITE;
			shade				= rand() % 2;
			ambientFactor		= .65f;
		}
		else {
			materialColor		= ::gpk::BLACK;
			shade				= true;
		} 
	}
	if(shade) {
		const ::gpk::n3f32			lightVecW				= (constants.LightPosition - inPS.WeightedPosition).Normalize();
		double						diffuseFactor			= inPS.WeightedNormal.Dot(lightVecW);
		if(diffuseFactor < 0) {
			ambientFactor			+= (rand() % 256) * (1.0f / 255) * float(diffuseFactor) * -.25f;
			materialColor			= (materialColor * ambientFactor).Clamp();
		}
		else {
			const ::gpk::rgbaf			specular				= ::gpk::lightCalcSpecular(constants.CameraPosition, ::LIGHT_FACTOR_SPECULAR_POWER, gpk::WHITE, ::gpk::WHITE, inPS.WeightedPosition, inPS.WeightedNormal, lightVecW);
			const ::gpk::rgbaf			diffuse					= materialColor * ::gpk::max(0.0, diffuseFactor);
			const ::gpk::rgbaf			ambient					= materialColor * ambientFactor;
			materialColor			= ::gpk::rgbaf(ambient + diffuse + specular).Clamp();
		}
	}
	outputPixel				= materialColor;
	return 0; 
}
