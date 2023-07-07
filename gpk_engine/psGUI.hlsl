#include "PixelShaderShared2D.hlsli"

float4 main(GUIPixelShaderInput input) : SV_TARGET
{
	float4 vDiffuse = texDiffuse.Sample( samplerLinear, input.uv );
	return vDiffuse;
}
