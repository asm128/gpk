#include "PixelShaderShared.hlsli"
#include "ShaderColors.hlsli"

float4			main					(PixelShaderInput input) : SV_TARGET {
	const float			margin2					= .00025;
	const bool			markCenter				= 
		 ( (input.world.uv.x > 0.0 && input.world.uv.x < margin2) 
		|| (input.world.uv.x > .25 - margin2 && input.world.uv.x < .25 + margin2)
		|| (input.world.uv.x > .50 - margin2 && input.world.uv.x < .50 + margin2)
		|| (input.world.uv.x > .75 - margin2 && input.world.uv.x < .75 + margin2)
		|| (input.world.uv.x > 1.0 - margin2 && input.world.uv.x < 1.0)
		);
	float				diffuseFactor			= wave3(input.world.position.xyz, float3(0, 0, 0), 1200, 10);
	float2				scaledUV				= float2(input.world.uv.x * NodeSize.x, input.world.uv.y * NodeSize.z);
	float				randFactor				= wave2(scaledUV, NodeSize.xyz, 1, 100); // 
	float4				materialColor			= Diffuse * 2;
	materialColor	= markCenter
		? saturate(float4(materialColor.r + max(0, Diffuse.b * diffuseFactor * 2) + max(0, Diffuse.g * randFactor), materialColor.g + max(0, Diffuse.r * diffuseFactor * 2) + max(0, Diffuse.b * randFactor), materialColor.b + max(0, Diffuse.g * diffuseFactor * 2) + max(0, Diffuse.r * randFactor), max(Diffuse.a * diffuseFactor * 4, .25f)))
		: saturate(float4(materialColor.rgb, Diffuse.a * diffuseFactor * (1.0 - abs(input.world.uv.y - .5f) * 2)))
		;
	materialColor	+= materialColor * randFactor;
	return float4(saturate(materialColor).xyz, materialColor.a);
}
