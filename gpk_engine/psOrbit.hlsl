#include "PixelShaderShared.hlsli"
#include "ShaderColors.hlsli"

float4			main					(PixelShaderInput input) : SV_TARGET {
	float2				scaledUV				= float2(input.world.uv.x * NodeSize.x, input.world.uv.y * NodeSize.z);
	const float			margin2					= .00025;
	const bool			markCenter				= 
		 ( (input.world.uv.x > 0.0 && input.world.uv.x < margin2) 
		|| (input.world.uv.x > .25 - margin2 && input.world.uv.x < .25 + margin2)
		|| (input.world.uv.x > .50 - margin2 && input.world.uv.x < .50 + margin2)
		|| (input.world.uv.x > .75 - margin2 && input.world.uv.x < .75 + margin2)
		|| (input.world.uv.x > 1.0 - margin2 && input.world.uv.x < 1.0)
		);
	float				diffuseFactor			= max(0.0f, sin(distance(input.world.position.xz, float2(0, 0)) * 1200.0 + CameraFront.w * 10)); 
	//float				diffuseFactor			= max(0.0f, sin((distance(scaledUV, NodeSize.xz * .5f) - CameraFront.w * (.005f / NodeSize.z)) * 3.14159f * 100));
	float4				materialColor			= markCenter
		? float4(saturate(Diffuse * 2).xyz, max(Diffuse.a * diffuseFactor * 2, .125f))
		: float4(GPK_YELLOW.rgb, Diffuse.a * diffuseFactor * (1.0 - abs(input.world.uv.y - .5f) * 2))
		;

	return float4(saturate(materialColor).xyz, materialColor.a);
}
