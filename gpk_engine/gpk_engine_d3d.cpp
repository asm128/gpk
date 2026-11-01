#include "gpk_engine_d3d.h"

#if defined(GPK_WINDOWS)

#include "gpk_file.h"

gpk::err_t 		gpk::d3dCreateBuffersFromEngineMeshes		(ID3D11Device * pDevice, const ::gpk::SMeshManager & engineMeshes, const ::gpk::SRenderBufferManager & engineBuffers, ::gpk::array_com<ID3D11Buffer> & indexBuffers, ::gpk::array_com<ID3D11Buffer> & vertexBuffers)	{
	for(u2_t iMesh = 0; iMesh < engineMeshes.size(); ++iMesh) {
		const ::gpk::pobj<::gpk::SGeometryMesh>		& mesh									= engineMeshes[iMesh];
		if(!mesh)
			continue;

		::gpk::pobj<::gpk::SRenderBuffer>			engineBufferIndices;
		::gpk::apobj<::gpk::SRenderBuffer>			engineBufferVertices;
		::gpk::au2_t						layoutOffsets;

		u2_t										vertexSize					= 0;
		for(u2_t iBuffer = 0; iBuffer < mesh->GeometryBuffers.size(); ++iBuffer) {
			const ::gpk::pobj<::gpk::SRenderBuffer>		& buffer								= engineBuffers[mesh->GeometryBuffers[iBuffer]];
			if(buffer->Desc.Usage == ::gpk::BUFFER_USAGE_Index) {
				engineBufferIndices							= buffer;
				continue;
			}
			if_fail_fe(engineBufferVertices.push_back(buffer));
			if_fail_fe(layoutOffsets.push_back(vertexSize));
			vertexSize									+= buffer->Desc.Format.TotalBytes();

		}
		::gpk::apod<u0_t>						packed;
		for(u2_t iValue = 0, valueCount = engineBufferVertices[0]->Data.size() / engineBufferVertices[0]->Desc.Format.TotalBytes(); iValue < valueCount; ++iValue) {
			for(u2_t iBuffer = 0; iBuffer < engineBufferVertices.size(); ++iBuffer) {
				const ::gpk::SRenderBuffer						& buffer					= *engineBufferVertices[iBuffer];
				::gpk::vcu0_t							value						= {&buffer.Data[iValue * buffer.Desc.Format.TotalBytes()], buffer.Desc.Format.TotalBytes()};
				packed.append(value);
			}
		}
		{
			D3D11_SUBRESOURCE_DATA							vertexBufferData			= {packed.begin()};
			D3D11_BUFFER_DESC								vertexBufferDesc			= {packed.size()};
			vertexBufferDesc.BindFlags					= D3D11_BIND_VERTEX_BUFFER;
			::gpk::ptr_com<ID3D11Buffer>					d3dBuffer;
			gpk_hrcall(pDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &d3dBuffer));
			vertexBuffers.push_back(d3dBuffer);
		}
		{
			D3D11_SUBRESOURCE_DATA			indexBufferData			= {engineBufferIndices->Data.begin()};
			D3D11_BUFFER_DESC				indexBufferDesc			= {engineBufferIndices->Data.size()};
			indexBufferDesc.BindFlags	= D3D11_BIND_INDEX_BUFFER;
			::gpk::au1_t						indices;
			const SDataTypeID				indexFormat				= engineBufferIndices->Desc.Format;
			const u2_t					indexSize				= engineBufferIndices->Desc.Format.TotalBytes();
			//if(engineBufferIndices->Desc.Format.TotalBytes() == 4) 
			//else 
			if(indexSize == 1) {
				if_fail_fe(indices.resize(engineBufferIndices->Data.size()));
				indices.enumerate([engineBufferIndices](u2_t & i, u1_t & value){ value = engineBufferIndices->Data[i]; return 0; }, 0);
				indexBufferData.pSysMem	= indices.begin();
				indexBufferDesc.ByteWidth = indices.byte_count();
			}

			::gpk::ptr_com<ID3D11Buffer>					d3dBuffer;
			gpk_hrcall(pDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, &d3dBuffer));
			indexBuffers.push_back(d3dBuffer);
		}
	}

	return 0;
}

gpk::err_t 		gpk::d3dCreateTexturesFromEngineSurfaces	(ID3D11Device* pDevice, const ::gpk::SSurfaceManager & engineSurfaces, ::gpk::array_com<ID3D11ShaderResourceView> & shaderResourceViews) {
	for(u2_t iSurface = 0; iSurface < engineSurfaces.size(); ++iSurface) {
		const ::gpk::SSurface						& surface			= *engineSurfaces[iSurface];
		D3D11_TEXTURE2D_DESC						desc				= {surface.Desc.Dimensions.x, surface.Desc.Dimensions.y, 1, 1, DXGI_FORMAT_B8G8R8A8_UNORM, {1, 0}, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE};
		D3D11_SUBRESOURCE_DATA						initData			= {};
		initData.pSysMem						= surface.Data.begin();
		initData.SysMemPitch					= surface.Desc.Dimensions.x * 4;

		::gpk::ptr_com<ID3D11Texture2D>				pTex2D				= {};
		gpk_hrcall(pDevice->CreateTexture2D(&desc, &initData, &pTex2D));
		if(!pTex2D) 
			continue;

		D3D11_SHADER_RESOURCE_VIEW_DESC				srvDesc				= {};
		srvDesc.Format							= desc.Format;
		srvDesc.ViewDimension					= D3D_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels				= desc.MipLevels;

		::gpk::ptr_com<ID3D11ShaderResourceView>	pSRV;
		gpk_hrcall(pDevice->CreateShaderResourceView(pTex2D, &srvDesc, &pSRV));
		shaderResourceViews.push_back(pSRV);
	}
	return 0;
}

//
gpk::err_t 		gpk::d3dCreatePixelShadersFromEngineShaders	(ID3D11Device* pDevice, const ::gpk::SShaderManager & engineShaders, ::gpk::array_com<ID3D11PixelShader> & pixelShaders) {
	for(u2_t iShader = 0; iShader < engineShaders.size(); ++iShader) {
		vcst_t		shaderName				= engineShaders.Names[iShader];

		// After the pixel shader file is loaded, create the shader.
		sc_t											shaderFileName	[1024]	= {};
		sprintf_s(shaderFileName, "%.*s.cso", GPK_VIEW_ARGS(shaderName));
		::gpk::apod<s0_t>						filePS;
		if_fail_fe(gpk::fileToMemory(::gpk::vcst_t{shaderFileName}, filePS));

		::gpk::ptr_com<ID3D11PixelShader>				pixelShader;
		gpk_hrcall(pDevice->CreatePixelShader(&filePS[0], filePS.size(), nullptr, &pixelShader));
		pixelShaders.push_back(pixelShader);
	}
	return 0;
}

#endif // GPK_WINDOWS
