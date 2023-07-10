#include "PixelShaderShared.hlsli"
#include "ShaderColors.hlsli"

float4			main					(PixelShaderInput input) : SV_TARGET {
	float2				scaledUV				= float2(input.world.uv.x * NodeSize.x, input.world.uv.y * NodeSize.z);
	
	bool				markBorder				= 
	  ( .00125 >= distance(scaledUV, float2(0, 0))
	 || .00125 >= distance(scaledUV, float2(NodeSize.x, 0))
	 || .00125 >= distance(scaledUV, float2(0, NodeSize.z))
	 || .00125 >= distance(scaledUV, float2(NodeSize.x, NodeSize.z))
	 || .00125 >= distance(scaledUV, float2(NodeSize.x * .5, NodeSize.z))
	 || .00125 >= distance(scaledUV, float2(NodeSize.x * .5, 0))
	 || .00125 >= distance(scaledUV, float2(NodeSize.x, NodeSize.z * .5))
	 || .00125 >= distance(scaledUV, float2(0, NodeSize.z * .5))
	);
	
	if(markBorder) {
		discard;
		return float4(0,0,0,0);
	}
	bool				markCenter
		= ((scaledUV.x > NodeSize.x * .5 - .00010 && scaledUV.x < NodeSize.x * .5 + .00010) 
		|| (scaledUV.y > NodeSize.z * .5 - .00010 && scaledUV.y < NodeSize.z * .5 + .00010)
		);
	float				diffuseFactor			= max(sin((distance(scaledUV, NodeSize.xz * .5f) - CameraFront.w * (.005f / NodeSize.z)) * 3.14159f * 100) -.5f, 0.0f);
	float4				materialColor			= markCenter
		? float4(saturate(Diffuse * 2).xyz, max(Diffuse.a * diffuseFactor * 2, .1f))
		: float4(Diffuse.xyz, Diffuse.a * diffuseFactor)
		;

	return float4(saturate(materialColor).xyz, materialColor.a);
}
