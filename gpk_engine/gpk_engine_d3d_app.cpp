#include "gpk_engine_d3d_app.h"
#include "gpk_view_color.h"

GPK_USING_TYPEINT();

gpk::err_t	gpk::d3dGUIDraw		(::DX::D3DDeviceResources & d3dResources, SD3DGUIResources<bgra> & guiStuff)	{
	if(!guiStuff.Texture2D) 
		return 1;
	
	D3D11_MAPPED_SUBRESOURCE	mappedTexture				= {};
	gpk_hrcall(d3dResources.GetD3DDeviceContext()->Map(guiStuff.Texture2D.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedTexture));
	if(mappedTexture.pData) {
		for(u2_t y = 0; y < guiStuff.RenderTarget.metrics().y; ++y) {
			vcbgra			scanLineSrc					= guiStuff.RenderTarget[y];
			vbgra			scanLineDst					= {(bgra*)(&((sc_t*)mappedTexture.pData)[y * mappedTexture.RowPitch]), scanLineSrc.size()};
			memcpy(scanLineDst.begin(), scanLineSrc.begin(), scanLineSrc.byte_count());
			//for(u2_t x = 0; x < scanLineDst.size(); ++x)
			//	 scanLineDst.begin()[x].a	= scanLineDst.begin()[x].a;
		}
	}

	d3dResources.GetD3DDeviceContext()->Unmap(guiStuff.Texture2D.get(), 0);
	// Each vertex is one instance of the VertexPositionColor struct.
	UINT									stride						= sizeof(n3f2_t) + sizeof(n2f2_t);
	UINT									offset						= 0;

	ID3D11DeviceContext						* context					= d3dResources.GetD3DDeviceContext();
	pcom<ID3D11Buffer>				vb							= guiStuff.VertexBuffer;
	pcom<ID3D11Buffer>				ib							= guiStuff.IndexBuffer ;
	context->IASetVertexBuffers		(0, 1, &vb, &stride, &offset);
	context->IASetIndexBuffer		(ib, DXGI_FORMAT_R16_UINT, 0);// Each index is one 16-bit unsigned integer (short).
	context->IASetPrimitiveTopology	(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout		(guiStuff.InputLayout);
	context->VSSetShader			(guiStuff.VertexShader, nullptr, 0);	// Attach our vertex shader.
	context->PSSetShader			(guiStuff.PixelShader, nullptr, 0);	// Attach our pixel shader.
	context->RSSetState				(guiStuff.RasterizerState);
	context->PSSetSamplers			(0, 1, &guiStuff.SamplerState);
	context->PSSetShaderResources	(0, 1, &guiStuff.ShaderResourceView);
	context->OMSetBlendState		(guiStuff.BlendState, 0, 0xFFFFFFFF);
	context->OMSetDepthStencilState	(guiStuff.DepthStencilState, 0);
	//
	context->DrawIndexed(6, 0, 0);	// Draw the objects.
	return 0;
}

sttc	gpk::err_t	getNodeTransform		(const gpk::SRenderNodeManager & nodeManager, u2_t iNode, const gpk::SEngineSceneConstants & constants, gpk::SRenderNodeConstants & nodeConstants) {
	nodeConstants			= nodeManager.Transforms[iNode];

	nodeConstants.Model					= nodeManager.BaseTransforms[iNode].Model * nodeConstants.Model;
	nodeConstants.ModelInverse			= nodeConstants.Model.GetInverse();
	nodeConstants.ModelInverseTranspose	= nodeConstants.ModelInverse.GetTranspose();
	nodeConstants.MVP					= nodeConstants.Model * constants.View * constants.Perspective;

	nodeConstants.Model					= nodeConstants.Model.GetTranspose();
	nodeConstants.ModelInverse			= nodeConstants.ModelInverse.GetTranspose();
	nodeConstants.ModelInverseTranspose	= nodeConstants.ModelInverseTranspose.GetTranspose();
	nodeConstants.MVP					= nodeConstants.MVP.GetTranspose();
	nodeConstants.NodeSize				= nodeManager.BaseTransforms[iNode].NodeSize; // Have to update this some other way.
	return 0;
}

gpk::err_t	gpk::d3dDrawEngineScene	(Sample3DSceneRenderer & d3dScene, const SEngineScene & engineScene, const n2u1_t & targetMetrics, const n3f2_t & lightPos, const n3f2_t & cameraPosition, const gpk::n3f2_t & cameraTarget, const gpk::minmaxf2_t & nearFar)	{
	n3f2_t				cameraFront				= (cameraTarget - cameraPosition).Normalize();

	SEngineSceneConstants& constants				= d3dScene.ConstantBufferScene;
	constants.CameraPosition= cameraPosition;
	constants.CameraFront	= cameraFront;
	constants.LightPosition	= lightPos;
	constants.LightDirection= {0, -1, 0};

	constants.View.LookAt(cameraPosition, cameraTarget, {0, 1, 0});
	constants.Perspective.FieldOfView(.25 * math_pi, targetMetrics.x / (f3_t)targetMetrics.y, nearFar.Min, nearFar.Max);
	constants.Screen.ViewportLH(targetMetrics);
	constants.VP			= constants.View * constants.Perspective;
	constants.VPS			= constants.VP * constants.Screen;

	au2_t								solid;
	au2_t								alpha;
	for(u2_t iNode = 0; iNode < engineScene.RenderNodes.RenderNodes.size(); ++iNode) {
		const SRenderNodeFlags			& flags							= engineScene.RenderNodes.Flags[iNode];
		if(flags.NoDraw)
			continue;

		const SRenderNode				& node							= engineScene.RenderNodes.RenderNodes[iNode];
		if(node.Mesh >= engineScene.Graphics->Meshes.size())
			continue;

		const SSkin						& skin							= *engineScene.Graphics->Skins.Elements[node.Skin];
		if(skin.Material.Color.Diffuse.a < 1)
			alpha.push_back(iNode);
		else
			solid.push_back(iNode);
	}
	for(u2_t iSolid = 0; iSolid < solid.size(); ++iSolid) {
		u2_t								iNode							= solid[iSolid];
		SRenderNodeConstants				nodeConstants;
		getNodeTransform(engineScene.RenderNodes, iNode, constants, nodeConstants);
		
		const SRenderNode				& node							= engineScene.RenderNodes.RenderNodes[iNode];
		const SGeometryMesh				& mesh							= *engineScene.Graphics->Meshes[node.Mesh];

		u2_t				indexCount			= 0;
		if(mesh.GeometryBuffers.size() > 0) {
			auto					pindices			= engineScene.Graphics->Buffers[mesh.GeometryBuffers[0]];
			indexCount			= pindices->Data.size() / pindices->Desc.Format.TotalBytes();
		}

		const SSkin						& skin							= *engineScene.Graphics->Skins.Elements[node.Skin];
		const SGeometrySlice				slice							= (node.Slice < mesh.GeometrySlices.size()) ? mesh.GeometrySlices[node.Slice] : SGeometrySlice{{0, indexCount / 3}};
		nodeConstants.Material				= skin.Material;

		verbose_printf("Drawing node %i, mesh %i, slice %i, mesh name: %s", iNode, node.Mesh, node.Slice, engineScene.Graphics->Meshes.Names[node.Mesh].begin());
		d3dScene.Render(node.Mesh, slice.Slice, skin.Textures[0], node.Shader, skin.Material.Color.Diffuse.a != 1.0f, nodeConstants);
	}
	for(u2_t iAlpha  = 0; iAlpha < alpha.size(); ++iAlpha) {
		u2_t								iNode							= alpha[iAlpha];
		SRenderNodeConstants				nodeConstants;
		getNodeTransform(engineScene.RenderNodes, iNode, constants, nodeConstants);
		
		const SRenderNode				& node							= engineScene.RenderNodes.RenderNodes[iNode];
		const SGeometryMesh				& mesh							= *engineScene.Graphics->Meshes[node.Mesh];
		u2_t				indexCount			= 0;
		if(mesh.GeometryBuffers.size() > 0) {
			auto					pindices			= engineScene.Graphics->Buffers[mesh.GeometryBuffers[0]];
			indexCount			= pindices->Data.size() / pindices->Desc.Format.TotalBytes();
		}

		const SSkin			& skin							= *engineScene.Graphics->Skins.Elements[node.Skin];
		const SGeometrySlice	slice							= (node.Slice < mesh.GeometrySlices.size()) ? mesh.GeometrySlices[node.Slice] : SGeometrySlice{{0, indexCount / 3}};
		nodeConstants.Material	= skin.Material;

		verbose_printf("Drawing node %i, mesh %i, slice %i, mesh name: %s", iNode, node.Mesh, node.Slice, engineScene.Graphics->Meshes.Names[node.Mesh].begin());
		d3dScene.Render(node.Mesh, slice.Slice, skin.Textures[0], node.Shader, skin.Material.Color.Diffuse.a != 1.0f, nodeConstants);
	}
	return 0;
}
gpk::err_t					gpk::d3dAppDraw				(SD3DApplication & d3dApp, const SEngineScene & engineScene, const rgbaf & clearColor, const n3f2_t & lightPos, const n3f2_t & cameraPosition, const gpk::n3f2_t & cameraTarget, const gpk::minmaxf2_t & nearFar) {
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
		//(void)engineScene; (void)lightPos; (void)cameraPosition; (void)cameraTarget; (void)nearFar;
		if_fail_fe(gpk::d3dDrawEngineScene(d3dApp.Scene, engineScene, d3dApp.DeviceResources->GetLogicalSize().u1(), lightPos, cameraPosition, cameraTarget, nearFar));
	}
	{ // Render GUI
		if_fail_fe(gpk::d3dGUIDraw(*d3dApp.DeviceResources, d3dApp.GUIStuff)); 

		if_fail_fe(d3dApp.Text.Render());
	}
	if_fail_fe(d3dApp.DeviceResources->Present());
	return 0;
}
