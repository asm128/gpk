#include "PixelShaderShared.hlsli"
#include "ShaderColors.hlsli"

float4					main					(PixelShaderInput input) : SV_TARGET {
	float3						lightVecW				= normalize(LightPosition.xyz - input.world.position);
	float						diffuseFactor			= max(dot(lightVecW, normalize(input.world.normal)), 0.0f);
	float4						materialColor			
		= (input.world.uv.x > .2485 && input.world.uv.x < .2515) ? float4(GPK_WHITE.xyz, Diffuse.a)
		: Diffuse
		;

	float4						diffuse					= diffuseFactor ? float4((materialColor * diffuseFactor).rgb * max(.65f, rand(input.world.uv)), materialColor.a) : float4(0, 0, 0, 1);
	float2						scaledUV				= float2(input.world.uv.x * NodeSize.x, input.world.uv.y * NodeSize.z);
	if( .05 >= distance(scaledUV, float2(0, 0))
	 || .05 >= distance(scaledUV, float2(NodeSize.x, 0))
	 || .05 >= distance(scaledUV, float2(0, NodeSize.z))
	 || .05 >= distance(scaledUV, float2(NodeSize.x, NodeSize.z))
	 || .05 >= distance(scaledUV, float2(NodeSize.x * .5, NodeSize.z))
	 || .05 >= distance(scaledUV, float2(NodeSize.x * .5, 0))
	 || .05 >= distance(scaledUV, float2(NodeSize.x, NodeSize.z * .5))
	 || .05 >= distance(scaledUV, float2(0, NodeSize.z * .5))
	) {
		discard;
		return float4(0,0,0,0);
	}
	else {
		return float4(saturate(diffuse).xyz, diffuse.a);
	}
}
