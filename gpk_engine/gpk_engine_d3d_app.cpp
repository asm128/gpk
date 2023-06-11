#include "gpk_engine_d3d_app.h"
#include "gpk_view_color.h"

::gpk::error_t			gpk::d3dGUIDraw		(::DX::D3DDeviceResources & d3dResources, ::gpk::SD3DGUIResources<::gpk::bgra> & guiStuff)	{
	if(!guiStuff.Texture2D) 
		return 1;
	
	D3D11_MAPPED_SUBRESOURCE	mappedTexture				= {};
	gpk_hrcall(d3dResources.GetD3DDeviceContext()->Map(guiStuff.Texture2D.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedTexture));
	if(mappedTexture.pData) {
		for(uint32_t y = 0; y < guiStuff.RenderTarget.metrics().y; ++y) {
			::gpk::vcbgra			scanLine					= guiStuff.RenderTarget[y];
			memcpy(&((char*)mappedTexture.pData)[y * mappedTexture.RowPitch], scanLine.begin(), scanLine.byte_count());
		}
	}

	d3dResources.GetD3DDeviceContext()->Unmap(guiStuff.Texture2D.get(), 0);
	// Each vertex is one instance of the VertexPositionColor struct.
	UINT									stride						= sizeof(::gpk::n3f32) + sizeof(::gpk::n2f32);
	UINT									offset						= 0;

	ID3D11DeviceContext						* context					= d3dResources.GetD3DDeviceContext();
	::gpk::pcom<ID3D11Buffer>				vb							= guiStuff.VertexBuffer;
	::gpk::pcom<ID3D11Buffer>				ib							= guiStuff.IndexBuffer ;
	context->IASetVertexBuffers		(0, 1, &vb, &stride, &offset);
	context->IASetIndexBuffer		(ib, DXGI_FORMAT_R16_UINT, 0);// Each index is one 16-bit unsigned integer (short).
	context->IASetPrimitiveTopology	(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout		(guiStuff.InputLayout);
	context->VSSetShader			(guiStuff.VertexShader, nullptr, 0);	// Attach our vertex shader.
	context->PSSetShader			(guiStuff.PixelShader, nullptr, 0);	// Attach our pixel shader.

	D3D11_BUFFER_DESC						desc						= {};
	ib->GetDesc(&desc);

	D3D11_RASTERIZER_DESC					rs							= {};
	rs.FillMode							= D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME; 
	rs.CullMode							= D3D11_CULL_BACK;
	rs.DepthClipEnable					= FALSE;

	::gpk::pcom<ID3D11RasterizerState>		prs;
	gpk_hrcall(d3dResources.GetD3DDevice()->CreateRasterizerState(&rs, &prs));
	context->RSSetState(prs);
	context->PSSetSamplers(0, 1, &guiStuff.SamplerStates);
	context->PSSetShaderResources(0, 1, &guiStuff.SRV);
	context->DrawIndexed(6, 0, 0);	// Draw the objects.
	return 0;
}


::gpk::error_t						gpk::d3dDrawEngineScene	(::gpk::Sample3DSceneRenderer & d3dScene, const ::gpk::SEngineScene & engineScene, const ::gpk::n2u16 & targetMetrics, const ::gpk::n3f & lightPos, const ::gpk::n3f & cameraPosition, const gpk::n3f & cameraTarget)	{
	::gpk::n3f								cameraFront						= (cameraTarget - cameraPosition).Normalize();
	::gpk::minmax<float>					nearFar 						= {.0001f, 10.0f}; 

	::gpk::SEngineSceneConstants			& constants						= d3dScene.ConstantBufferScene;
	constants.CameraPosition			= cameraPosition;
	constants.CameraFront				= cameraFront;
	constants.LightPosition				= lightPos;
	constants.LightDirection			= {0, -1, 0};

	constants.View.LookAt(cameraPosition, cameraTarget, {0, 1, 0});
	constants.Perspective.FieldOfView(.25 * ::gpk::math_pi, targetMetrics.x / (double)targetMetrics.y, nearFar.Min, nearFar.Max);
	constants.Screen.ViewportLH(targetMetrics);
	constants.VP						= constants.View * constants.Perspective;
	constants.VPS						= constants.VP * constants.Screen;

	for(uint32_t iNode = 0; iNode < engineScene.RenderNodes.RenderNodes.size(); ++iNode) {
		const ::gpk::SRenderNodeFlags			& flags							= engineScene.RenderNodes.Flags[iNode];
		const ::gpk::SRenderNode				& node							= engineScene.RenderNodes.RenderNodes[iNode];
		if(flags.NoDraw)
			continue;

		if(node.Mesh >= engineScene.Graphics->Meshes.size())
			continue;

		const ::gpk::SRenderNodeTransforms		& transforms					= engineScene.RenderNodes.Transforms[iNode];

		::gpk::SRenderNodeConstants				& nodeConstants					= d3dScene.ConstantBufferModel;
		nodeConstants.MVP					= (transforms.World * constants.View * constants.Perspective).GetTranspose();
		nodeConstants.Model					= transforms.World.GetTranspose();
		nodeConstants.ModelInverseTranspose	= transforms.WorldInverseTranspose.GetTranspose();

		const ::gpk::SGeometryMesh				& mesh							= *engineScene.Graphics->Meshes[node.Mesh];
		verbose_printf("Drawing node %i, mesh %i, slice %i, mesh name: %s", iNode, node.Mesh, node.Slice, engineScene.Graphics->Meshes.Names[node.Mesh].begin());

		uint32_t				indexCount			= 0;
		if(mesh.GeometryBuffers.size() > 0) {
			auto					pindices			= engineScene.Graphics->Buffers[mesh.GeometryBuffers[0]];
			indexCount			= pindices->Data.size() / pindices->Desc.Format.TotalBytes();
		}

		const ::gpk::SSkin						& skin							= *engineScene.Graphics->Skins.Elements[node.Skin];
		const ::gpk::SGeometrySlice				slice							= (node.Slice < mesh.GeometrySlices.size()) ? mesh.GeometrySlices[node.Slice] : ::gpk::SGeometrySlice{{0, indexCount / 3}};
		nodeConstants.Material				= skin.Material;

		d3dScene.Render(node.Mesh, slice.Slice, skin.Textures[0], node.Shader);
	}
	return 0;
}

::gpk::error_t							gpk::d3dAppDraw				(::gpk::SD3DApplication & d3dApp, const ::gpk::SEngineScene & engineScene, const ::gpk::rgbaf & clearColor, const ::gpk::n3f & lightPos, const ::gpk::n3f & cameraPosition, const gpk::n3f & cameraTarget) {
	{ // Set up render target for this frame
		ID3D11DeviceContext3						* context							= d3dApp.DeviceResources->GetD3DDeviceContext();
		const D3D11_VIEWPORT						viewport							= d3dApp.DeviceResources->GetScreenViewport();	// Reset the viewport to target the whole screen.
		context->RSSetViewports(1, &viewport);

		ID3D11RenderTargetView *					targets	[]							= { d3dApp.DeviceResources->GetBackBufferRenderTargetView(), };	// Reset render targets to the screen.
		context->OMSetRenderTargets(1, targets, d3dApp.DeviceResources->GetDepthStencilView());

		context->ClearRenderTargetView(d3dApp.DeviceResources->GetBackBufferRenderTargetView(), &clearColor.r);	// Clear the back buffer and depth stencil view.
		context->ClearDepthStencilView(d3dApp.DeviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);	
	}
	{ // Render 3d scene
		gpk_necs(::gpk::d3dDrawEngineScene(d3dApp.Scene, engineScene, d3dApp.DeviceResources->GetLogicalSize().u16(), lightPos, cameraPosition, cameraTarget));
	}
	{ // Render GUI
		gpk_necs(::gpk::d3dGUIDraw(*d3dApp.DeviceResources, d3dApp.GUIStuff)); 

		gpk_necs(d3dApp.Text.Render());
	}
	gpk_necs(d3dApp.DeviceResources->Present());
	return 0;
}
