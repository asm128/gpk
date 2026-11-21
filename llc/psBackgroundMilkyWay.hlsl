#include "PixelShaderShared.hlsli"
#include "ShaderColors.hlsli"

float4			main					(PixelShaderInput input) : SV_TARGET {
	float4				sampleColor				= texDiffuse.Sample(samplerLinear, input.world.uv);
	float4				materialColor			= sampleColor;
	float3				cubicColor				= materialColor.rgb * materialColor.rgb * materialColor.rgb * materialColor.rgb;

	float				randWeight				= .5f + .5f * rand(input.world.uv);
	float				randWaveR				= .5f + .5f * rand(input.world.uv * sin(CameraFront.w));
	float				randWaveG				= .5f + .5f * rand(input.world.uv * sin(CameraFront.w) * 2);
	float				randWaveB				= .5f + .5f * rand(input.world.uv * sin(CameraFront.w) * 3);
	float4				randWaveColor			=
		{ max(0, randWaveR * .95)
		, max(0, randWaveG * .50)
		, max(0, randWaveB * .125)
		, 1
		};
	float				colorFactor				= abs(randWaveR - randWeight);
	if(colorFactor > .2)
		discard;

	float2				scaledUV				= float2(input.world.uv.x * NodeSize.x, input.world.uv.y * NodeSize.z);
	float				randFactor				= .5f * .5f + wave2(scaledUV, NodeSize.xyz, 1, 100); // 

	float4				finalColor				= float4(cubicColor * cubicColor * randWaveColor.rgb * randFactor * 4 + cubicColor.rgb * colorFactor, Diffuse.a);
	return finalColor;
}
