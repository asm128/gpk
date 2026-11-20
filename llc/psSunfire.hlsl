#include "PixelShaderShared.hlsli"
#include "ShaderColors.hlsli"

float4			main					(PixelShaderInput input) : SV_TARGET {
	float2				scaledUV				= float2(input.world.uv.x * NodeSize.x, input.world.uv.y * NodeSize.z);
	const float			margin2					= .00025;
	//const bool			markCenter				= 
	//	 ( (input.world.uv.x > 0.0 && input.world.uv.x < margin2) 
	//	|| (input.world.uv.x > .25 - margin2 && input.world.uv.x < .25 + margin2)
	//	|| (input.world.uv.x > .50 - margin2 && input.world.uv.x < .50 + margin2)
	//	|| (input.world.uv.x > .75 - margin2 && input.world.uv.x < .75 + margin2)
	//	|| (input.world.uv.x > 1.0 - margin2 && input.world.uv.x < 1.0)
	//	);
	float				diffuseFactor			= max(0.0f, sin(distance(input.world.position.xz * 10, float2(0, 0)) * 1200.0 + CameraFront.w * 20)); 
	//diffuseFactor	*= max(0.0f, sin(distance(input.world.position.xy, float2(0, 0)) * 1200.0 + CameraFront.w * 50)); 
	//float				randFactor				= max(0.0f, sin(rand(input.world.uv + CameraFront.w) + CameraFront.w * 20)); 
	float				randFactor				= max(0.0f, sin((distance(input.world.position * 10, NodeSize.xyz * .5f)  * 1000 - CameraFront.w * (.005f / NodeSize.z)) * 3.14159f * 1000));
	float4				materialColor			= Diffuse * 2;
	materialColor	= saturate(float4(materialColor.r + max(0, Diffuse.b * diffuseFactor * 2) + max(0, Diffuse.g * randFactor), materialColor.g + max(0, Diffuse.r * diffuseFactor * 2) + max(0, Diffuse.b * randFactor), materialColor.b + max(0, Diffuse.g * diffuseFactor * .1), max(Diffuse.a * diffuseFactor * 2, .125f)));
		//markCenter
		//? saturate(float4(materialColor.r + max(0, Diffuse.b * diffuseFactor * 2) + max(0, Diffuse.g * randFactor), materialColor.g + max(0, Diffuse.r * diffuseFactor * 2) + max(0, Diffuse.b * randFactor), materialColor.b + max(0, Diffuse.g * diffuseFactor * 2) + max(0, Diffuse.r * randFactor), max(Diffuse.a * diffuseFactor * 8, .5f)))
		//: saturate(float4(materialColor.rgb, Diffuse.a * diffuseFactor * (1.0 - abs(input.world.uv.y - .5f) * 2)))
		//;
	materialColor	+= materialColor * randFactor;
	return float4(saturate(materialColor).xyz, materialColor.a);
}
