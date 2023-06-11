#include "PixelShaderShared.hlsli"

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 vDiffuse = texDiffuse.Sample( samplerLinear, input.world.uv );
	
	float fLighting = saturate( dot( normalize(float3(-.5f, -1, 0)), normalize(input.world.normal)) );
	fLighting = min( max( fLighting, .1f ), 0.5f);
	
	return vDiffuse * fLighting;
}
