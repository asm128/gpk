cbuffer SRenderNodeConstants : register(b0) {
	float4				Diffuse;
	float4				Ambient;
	float4				Specular;
	float4				EmissionaAndSpecularPower;	// .w or .a for the specular power
	matrix				MVP;
	matrix				Model;
	matrix				ModelInverse;
	matrix				ModelInverseTranspose;
	float4				NodeSize;	// .w or .a for the specular power
};

cbuffer SEngineSceneConstants : register(b1) {
	matrix				View;
	matrix				Perspective;
	matrix				Screen;
	matrix				VP;
	matrix				VPS;
    float4				CameraPositionAndAngle; // use .w for the angle
    float4				CameraFront;
    float4				LightDirectionAndSpotPower;
    float4				LightPosition; // use .w for the spot power
};

struct VertexShaderInput {
	float3				position	: POSITION;
	float3				normal		: NORMAL;
	float2				uv			: TEXCOORD;
};

struct PixelShaderInput {
	float4				position	: SV_POSITION;
	VertexShaderInput	world;
};

