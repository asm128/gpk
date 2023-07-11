#include "PixelShaderShared.hlsli"
#include "ShaderColors.hlsli"

float4			main					(PixelShaderInput input) : SV_TARGET {
	float4				sampleColor				= texDiffuse.Sample(samplerLinear, input.world.uv);
	float4				materialColor			= sampleColor;
	float				randWeight				= rand(input.world.uv);
	float				randWaveR				= rand(input.world.uv * sin(CameraFront.w));
	float				randWaveG				= rand(input.world.uv * sin(CameraFront.w) * 2);
	float				randWaveB				= rand(input.world.uv * sin(CameraFront.w) * 3);
	float4				randWaveColor			=
		{ max(0, randWaveR * .95)
		, max(0, randWaveG * .50)
		, max(0, randWaveB * .125)
		, 1
		};
	float				colorFactor				= abs(randWaveR - randWeight);
	if(colorFactor > .2)
		discard;

	float3				cubicColor				= materialColor.rgb * materialColor.rgb * materialColor.rgb;
	float4				finalColor				= float4(cubicColor * cubicColor * randWaveColor.rgb + cubicColor.rgb * colorFactor, Diffuse.a);
	return finalColor;
}
