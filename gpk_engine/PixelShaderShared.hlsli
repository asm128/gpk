#include "ShaderShared.hlsli"

Texture2D				texDiffuse						: register( t0 );
SamplerState			samplerLinear					: register( s0 );

static const float		LIGHT_FACTOR_AMBIENT			= .025f;
static const float		LIGHT_FACTOR_SPECULAR_POWER		= 30.0f;

float4					lightCalcDiffuse				(float4 diffuseMaterial, float3 normalW, float3 lightVecW) {
	float						lightFactor						= max(dot(lightVecW, normalW), 0.0f);
	return lightFactor ? float4((diffuseMaterial * (float)lightFactor).rgb, diffuseMaterial.a) : float4(0, 0, 0, diffuseMaterial.a);
}

float4					lightCalcAmbient				(float4 ambientMaterial, float4 ambientLight) {
	return float4((ambientMaterial * ambientLight).rgb, ambientMaterial.a);
}

float					lightCalcAttenuation			(float3 attenuation012, float3 lightPosW, float3 posW) {
	float						d								= distance(lightPosW, posW);
	return attenuation012.x + attenuation012.y * d + attenuation012.z * d * d; // attenuation
}

float4					lightCalcSpecular				(float4 specularMaterial, float4 specularLight, float3 eyePosW, float specularPower, float3 posW, float3 normalW, float3 lightVecW) {
	const float3				pointToEye						= normalize(eyePosW - posW);
	const float3				reflected						= reflect(-lightVecW, normalW);
	const float					reflectedFactor					= max(dot(reflected, pointToEye), 0.0f);
	if(0 >= reflectedFactor) 
		return float4(0, 0, 0, specularMaterial.a);
	else {
		const float					factor							= pow(reflectedFactor, specularPower);
		return specularMaterial * specularLight * factor;
	}
}

float rand (float2 uv) {
	return frac(sin(dot(uv,float2(12.9898,78.233)))*43758.5453123);
}