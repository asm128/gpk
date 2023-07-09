#include "PixelShaderShared.hlsli"

float			getSurfaceWeight		(float2 uv) {
	return abs(rand(uv) * .75) + abs(sin(CameraFront.w) * .25);
}

float4			getSurfaceColor			(float2 uv) {
	float				weight					= getSurfaceWeight(uv);
	float4				randColor				= {1.f, .5f * weight + .5f, .15f * weight, 1.f};
	return randColor;
}

float4			main					(PixelShaderInput input) : SV_TARGET {
	float2				relativeToCenter		= input.world.uv - float2(.5f, .5f);
	relativeToCenter.x	*= 2;
	
	float4				randColor				= getSurfaceColor(relativeToCenter);
	float				randWeight				= getSurfaceWeight(relativeToCenter);
	const float4		colorBody				= float4(randColor.rgb, Diffuse.a);

	randWeight		*= .25;
	const float4		colorMarkerX			= saturate(float4(1, randWeight, randWeight, Diffuse.a));
	const float4		colorMarkerY			= saturate(float4(randWeight, 1, randWeight, Diffuse.a));
	const float4		colorMarkerZ			= saturate(float4(randWeight, randWeight, 1, Diffuse.a));


	const float			markerRadius			= .05f;
	const bool			markY
		=  markerRadius >=  0.5f - relativeToCenter.y
		|| markerRadius <= -0.5f - relativeToCenter.y
		;
	const bool			markX
		=  markerRadius >= distance(float2( 0.0f, 0.0f), relativeToCenter) 
		|| markerRadius >= distance(float2( 1.0f, 0.0f), relativeToCenter) 
		|| markerRadius >= distance(float2(-1.0f, 0.0f), relativeToCenter) 
		;
	const bool			markZ
		=  markerRadius >= distance(float2( 0.5f, 0.0f), relativeToCenter) 
		|| markerRadius >= distance(float2(-0.5f, 0.0f), relativeToCenter);
		;

	const float4		surfaceColor
		= markX	? colorMarkerX
		: markY	? colorMarkerY
		: markZ	? colorMarkerZ
		: colorBody;

	return surfaceColor;
}
