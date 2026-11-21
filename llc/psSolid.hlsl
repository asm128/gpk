#include "PixelShaderShared.hlsli"

float4 main(PixelShaderInput input) : SV_TARGET {
	float4 vDiffuse = texDiffuse.Sample( samplerLinear, input.world.uv );
	
	float fLighting = saturate(dot(normalize(LightDirectionAndSpotPower.xyz), normalize(input.world.normal)));
	fLighting = min( max( fLighting, .1f ), 0.5f);
	return float4((vDiffuse * fLighting).rgb, Diffuse.a * vDiffuse.a);
}
