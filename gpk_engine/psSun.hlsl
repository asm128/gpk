#include "PixelShaderShared.hlsli"

float4			main					(PixelShaderInput input) : SV_TARGET {
	float2				relativeToCenter		= input.world.uv - float2(.5f, .5f);
	relativeToCenter.x	*= 2;
	
	const float			randWeight				= rand(input.world.position.xz *  CameraFront.w);

	const float4		colorMarker				= float4(1, randWeight * .5f, randWeight * .5f, Diffuse.a);
	const float4		colorBody				= float4(1, randWeight, 0, Diffuse.a);

	bool				markY					
		=  ( 0.5f - relativeToCenter.y) <  .05f
		|| (-0.5f - relativeToCenter.y) > -.05f
		;
	const float4		surfaceColor				
		= (distance(float2( 0.0f, 0.0f), relativeToCenter) < .05f)	? colorMarker
		: (distance(float2( 1.0f, 0.0f), relativeToCenter) < .05f)	? colorMarker
		: (distance(float2(-1.0f, 0.0f), relativeToCenter) < .05f)	? colorMarker
		: (distance(float2( 0.5f, 0.0f), relativeToCenter) < .05f)	? colorMarker
		: (distance(float2(-0.5f, 0.0f), relativeToCenter) < .05f)	? colorMarker
		: markY														? colorMarker
		: colorBody;

	return surfaceColor;
}
