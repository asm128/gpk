#include "gpk_grid_color.h"

#if defined(GPK_WINDOWS)

#include "gpk_complus.h"

#include <d3d11_3.h>

#ifndef GPK_D3D_H_23627
#define GPK_D3D_H_23627

namespace gpk
{
	::gpk::error_t	d3dDeviceValidate		(::gpk::pcom<::ID3D11Device3> & d3dDevice);
	::gpk::error_t	d3dCreateTextureDynamic	(ID3D11Device* pDevice, ::gpk::pcom<ID3D11Texture2D> & texture, ::gpk::pcom<ID3D11ShaderResourceView> & shaderResourceView, ::gpk::gc8bgra initData); 
} // namespace

#endif // GPK_D3D_H_23627

#endif // GPK_WINDOWS