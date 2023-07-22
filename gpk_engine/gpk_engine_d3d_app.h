#include "Sample3DSceneRenderer.h"
#include "SampleFpsTextRenderer.h"
#include "DeviceResources.h"

#include "gpk_engine_d3d.h"

#include "gpk_image.h"
#include "gpk_file.h"

#ifndef GPK_ENGINE_D3D_APP_H_23627
#define GPK_ENGINE_D3D_APP_H_23627

namespace gpk
{
	tplt<tpnm _tColor>
	struct SD3DGUIResources {
		typedef	::gpk::img<_tColor>				TRenderTarget;

		TRenderTarget							RenderTarget		= {};

		// We need to render the GUI separately to compose DirectX target from a dynamic texture.
		::gpk::pcom<ID3D11BlendState>			BlendState			= {};
		::gpk::pcom<ID3D11RasterizerState>		RasterizerState		= {};
		::gpk::pcom<ID3D11SamplerState>			SamplerState		= {};
		::gpk::pcom<ID3D11DepthStencilState>	DepthStencilState	= {};	// - Size-dependent
		//															= {}
		::gpk::pcom<ID3D11Buffer>				IndexBuffer			= {};
		::gpk::pcom<ID3D11Buffer>				VertexBuffer		= {};
		::gpk::pcom<ID3D11Buffer>				ConstantBufferScene	= {};
		::gpk::pcom<ID3D11Buffer>				ConstantBufferNode	= {};
		::gpk::pcom<ID3D11InputLayout>			InputLayout			= {};
		::gpk::pcom<ID3D11VertexShader>			VertexShader		= {};
		::gpk::pcom<ID3D11PixelShader>			PixelShader			= {};
		::gpk::pcom<ID3D11ShaderResourceView>	ShaderResourceView	= {};
		::gpk::pcom<ID3D11Texture2D>			Texture2D			= {};	// - Size-dependent

		void					ReleaseDeviceResources		() {
			BlendState				= {};
			RasterizerState			= {};
			SamplerState			= {};
			DepthStencilState		= {};

			IndexBuffer				= {};
			VertexBuffer			= {};
			ConstantBufferScene		= {};
			ConstantBufferNode		= {};
			InputLayout				= {};
			VertexShader			= {};
			PixelShader				= {};
			ShaderResourceView		= {};
			Texture2D				= {};
		}

		::gpk::error_t			CreateSizeDependentResources(ID3D11Device3 * d3dDevice, ::gpk::n2u16 windowSize)	{
			gpk_necs(RenderTarget.resize(windowSize.u32(), {0, 0, 0, 0}));
			gpk_necs(::gpk::d3dCreateTextureDynamic(d3dDevice, Texture2D, ShaderResourceView, RenderTarget));
			return 0;
		}

		::gpk::error_t			CreateDeviceResources		(ID3D11Device3 * d3dDevice)	{
			{		
				::gpk::pcom<ID3D11Buffer>	constantBuffer;
				D3D11_BUFFER_DESC			constantBufferDesc			= {sizeof(::gpk::SRenderNodeConstants)};
				constantBufferDesc.BindFlags= D3D11_BIND_CONSTANT_BUFFER;
				gpk_hrcall(d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer));
				ConstantBufferNode		= constantBuffer;
			}
			{
				::gpk::pcom<ID3D11Buffer>	constantBuffer;
				D3D11_BUFFER_DESC			constantBufferDesc			= {sizeof(::gpk::SEngineSceneConstants)};
				constantBufferDesc.BindFlags= D3D11_BIND_CONSTANT_BUFFER;
				gpk_hrcall(d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer));
				ConstantBufferScene		= constantBuffer;
			}
			{
				// Load shaders asynchronously.
				::gpk::pcom<ID3D11InputLayout>	inputLayout	;
				::gpk::pcom<ID3D11VertexShader>	vertexShader;

				::gpk::vcs					shaderName					= "vsGUI";
				char						shaderFileName	[1024]		= {};
				cnstxpr	D3D11_INPUT_ELEMENT_DESC	vertexDesc []		=
					{ { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 00, D3D11_INPUT_PER_VERTEX_DATA, 0 }
					, { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
					};
				sprintf_s(shaderFileName, "%s.cso", shaderName.begin());

				::gpk::ai8					fileVS;
				gpk_necs(::gpk::fileToMemory(::gpk::vcs{shaderFileName}, fileVS));
				// After the vertex shader file is loaded, create the shader and input layout.
				gpk_hrcall(d3dDevice->CreateVertexShader(&fileVS[0], fileVS.size(), nullptr, &vertexShader));
				gpk_hrcall(d3dDevice->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileVS[0], fileVS.size(), &inputLayout)); 
				InputLayout				= inputLayout;
				VertexShader			= vertexShader;
			}
			{
				::gpk::vcs					shaderName							= "psGUI";
				char						shaderFileName	[1024]				= {};
				::gpk::pcom<ID3D11PixelShader>	pixelShader;
				sprintf_s(shaderFileName, "%s.cso", shaderName.begin());
				::gpk::ai8					filePS;
				gpk_necs(::gpk::fileToMemory(::gpk::vcs{shaderFileName}, filePS));

				gpk_hrcall(d3dDevice->CreatePixelShader(&filePS[0], filePS.size(), nullptr, &pixelShader));
				PixelShader				= pixelShader;
			}
			{
#pragma pack(push, 1)
				struct SPosUV  {
					::gpk::n3f32		Position;
					::gpk::n2f32		UV;
				};
#pragma pack(pop)
				constexpr SPosUV			vertices[4]				= {{{-1, 1}, {0, 0}},{{1, 1}, {1, 0}},{{-1, -1}, {0, 1}}, {{1, -1}, {1, 1}}};

				D3D11_SUBRESOURCE_DATA		vertexBufferData		= {vertices};
				D3D11_BUFFER_DESC			vertexBufferDesc		= {sizeof(SPosUV) * 4};
				vertexBufferDesc.BindFlags	= D3D11_BIND_VERTEX_BUFFER;
				::gpk::pcom<ID3D11Buffer>	d3dBuffer;
				gpk_hrcall(d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &d3dBuffer));
				VertexBuffer				= d3dBuffer;
			}
			{
				constexpr uint16_t			indices[]							= {0, 1, 2, 1, 3, 2};
				D3D11_SUBRESOURCE_DATA		vertexBufferData					= {indices};
				D3D11_BUFFER_DESC			vertexBufferDesc					= {sizeof(uint16_t) * 6};
				vertexBufferDesc.BindFlags	= D3D11_BIND_INDEX_BUFFER;
				::gpk::pcom<ID3D11Buffer>	d3dBuffer;
				gpk_hrcall(d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &d3dBuffer));
				IndexBuffer				= d3dBuffer;
			}
			// 3515504948
			{
				// Create a sampler state
				D3D11_SAMPLER_DESC			samDesc					= {};
				samDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_POINT;
				samDesc.AddressU		= D3D11_TEXTURE_ADDRESS_CLAMP;
				samDesc.AddressV		= D3D11_TEXTURE_ADDRESS_CLAMP;
				samDesc.AddressW		= D3D11_TEXTURE_ADDRESS_CLAMP;
				samDesc.MaxAnisotropy	= 1;
				samDesc.ComparisonFunc	= D3D11_COMPARISON_ALWAYS;
				samDesc.MaxLOD			= D3D11_FLOAT32_MAX;

				::gpk::pcom<ID3D11SamplerState>	samplerState;
				gpk_hrcall(d3dDevice->CreateSamplerState(&samDesc, &samplerState));
				SamplerState			= samplerState;
			}
			{ // straight
				D3D11_BLEND_DESC			blendDesc				= {};
				blendDesc.RenderTarget[0].BlendEnable			= TRUE;
				blendDesc.RenderTarget[0].SrcBlend				=
				blendDesc.RenderTarget[0].SrcBlendAlpha			= D3D11_BLEND_SRC_ALPHA;
				blendDesc.RenderTarget[0].DestBlend				=
				blendDesc.RenderTarget[0].DestBlendAlpha		= D3D11_BLEND_INV_SRC_ALPHA;
				blendDesc.RenderTarget[0].BlendOp				=
				blendDesc.RenderTarget[0].BlendOpAlpha			= D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;
				::gpk::pcom<ID3D11BlendState>	blendState;
				gpk_hrcall(d3dDevice->CreateBlendState(&blendDesc, &blendState));
				BlendState					= blendState;
			}
			{
				D3D11_RASTERIZER_DESC		rsDesc					= {};
				rsDesc.FillMode			= D3D11_FILL_SOLID; // D3D11_FILL_WIREFRAME; // 
				rsDesc.CullMode			= D3D11_CULL_BACK;
				rsDesc.DepthClipEnable	= false;
				::gpk::pcom<ID3D11RasterizerState>	rasterizerState;
				gpk_hrcall(d3dDevice->CreateRasterizerState(&rsDesc, &rasterizerState));
				RasterizerState			= rasterizerState;
			}
			{ // disabled
				D3D11_DEPTH_STENCIL_DESC	depthStencilDesc				= {};
				depthStencilDesc.DepthEnable		= FALSE;
				depthStencilDesc.DepthWriteMask		= D3D11_DEPTH_WRITE_MASK_ALL;
				depthStencilDesc.DepthFunc			= D3D11_COMPARISON_LESS;
				depthStencilDesc.StencilEnable		= 0;
				depthStencilDesc.StencilReadMask	= D3D11_DEFAULT_STENCIL_READ_MASK;
				depthStencilDesc.StencilWriteMask	= D3D11_DEFAULT_STENCIL_WRITE_MASK;

				depthStencilDesc.FrontFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;
				depthStencilDesc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;
				depthStencilDesc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
				::gpk::pcom<ID3D11DepthStencilState>	depthStencilState;
				gpk_hrcall(d3dDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilState));
				DepthStencilState				= depthStencilState;
			}
			return 0;
		}

	};

	::gpk::error_t			d3dGUIDraw				(::DX::D3DDeviceResources & d3dResources, ::gpk::SD3DGUIResources<::gpk::bgra> & guiStuff);

	struct SD3DApplication : DX::IDeviceNotify {
		::gpk::pobj<::gpk::SEngineGraphics>		EngineGraphics;

		::gpk::pobj<::DX::D3DDeviceResources>	DeviceResources			;
		::gpk::Sample3DSceneRenderer			Scene					;
		::gpk::SampleFpsTextRenderer			Text					;
		::gpk::SD3DGUIResources<::gpk::bgra>	GUIStuff				;
	
		virtual void			OnDeviceLost			() { return ReleaseDeviceResources(); }
		virtual ::gpk::error_t	OnDeviceRestored		() { return CreateDeviceResources(*EngineGraphics); }

		::gpk::error_t			Shutdown				()	{
			DeviceResources->m_swapChain->SetFullscreenState(FALSE, 0);
			ReleaseDeviceResources();
			Scene			= {};
			Text			= {};
			GUIStuff		= {};
			DeviceResources	= {};
			return 0;
		}
		::gpk::error_t			Initialize				(HWND hWnd, const ::gpk::pobj<::gpk::SEngineGraphics> & engineGraphics)	{
			EngineGraphics			= engineGraphics;

			gpk_necs(DeviceResources->Initialize());
			gpk_necs(DeviceResources->SetWindow(hWnd));
			DeviceResources->RegisterDeviceNotify(this);
			gpk_necs(Scene.Initialize(DeviceResources));
			gpk_necs(Text .Initialize(DeviceResources));
			return 0;
		}

		// Notifies renderers that device resources need to be released.
		void					ReleaseDeviceResources	()	{
			Scene		.ReleaseDeviceResources();
			Text		.ReleaseDeviceResources();
			GUIStuff	.ReleaseDeviceResources();
		}
		// Notifies renderers that device resources may now be recreated.

		::gpk::error_t			CreateDeviceDependentEngineResources(ID3D11Device3 * d3dDevice,  const ::gpk::SEngineGraphics & engineGraphics)	{
			gpk_necs(::gpk::d3dCreateBuffersFromEngineMeshes		(d3dDevice, engineGraphics.Meshes	, engineGraphics.Buffers, Scene.IndexBuffer, Scene.VertexBuffer));
			gpk_necs(::gpk::d3dCreateTexturesFromEngineSurfaces		(d3dDevice, engineGraphics.Surfaces	, Scene.ShaderResourceView));
			gpk_necs(::gpk::d3dCreatePixelShadersFromEngineShaders	(d3dDevice, engineGraphics.Shaders	, Scene.PixelShader));
			return 0;
		}
		::gpk::error_t			CreateDeviceResources	(const ::gpk::SEngineGraphics & engineGraphics)	{
			ID3D11Device3				* pd3dDevice			= DeviceResources->GetD3DDevice();
			gpk_necs(Scene		.CreateDeviceResources(pd3dDevice));
			gpk_necs(Text		.CreateDeviceResources(DeviceResources->GetD2DDeviceContext()));
			gpk_necs(GUIStuff	.CreateDeviceResources(pd3dDevice));
			gpk_necs(CreateDeviceDependentEngineResources(pd3dDevice, engineGraphics));
			return 0; 
		}

		::gpk::error_t			CreateSizeDependentResources(::gpk::n2u16 windowSize)	{
			gpk_necs(Scene		.CreateSizeDependentResources(DeviceResources->GetOutputSize(), DeviceResources->GetOrientationTransform3D()));
			gpk_necs(GUIStuff	.CreateSizeDependentResources(DeviceResources->GetD3DDevice(), windowSize));
			return 0;
		}

		::gpk::error_t			SetWindowSize			(::gpk::n2u16 windowSize)	{
			gpk_necs(DeviceResources->SetLogicalSize(windowSize.f32()));
			gpk_necs(CreateSizeDependentResources(windowSize));
			return 0;
		}
	};

	::gpk::error_t			d3dDrawEngineScene		(::gpk::Sample3DSceneRenderer & d3dScene, const ::gpk::SEngineScene & engineScene, const ::gpk::n2u16 & targetMetrics, const ::gpk::n3f32 & lightPos, const ::gpk::n3f32 & cameraPosition, const gpk::n3f32 & cameraTarget, const gpk::minmaxf32 & nearFar);
	::gpk::error_t			d3dAppDraw				(::gpk::SD3DApplication & d3dApp, const ::gpk::SEngineScene & engineScene, const ::gpk::rgbaf & clearColor, const ::gpk::n3f32 & lightPos, const ::gpk::n3f32 & cameraPosition, const gpk::n3f32 & cameraTarget, const gpk::minmaxf32 & nearFar);
};

#endif // GPK_ENGINE_D3D_APP_H_28903749823