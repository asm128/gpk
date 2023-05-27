#include "gpk_platform.h"
#include "gpk_color.h"

#if defined(GPK_WINDOWS)

#include "gpk_complus.h"

#include <d3d11_3.h>

#ifndef GPK_D3D_H_982734
#define GPK_D3D_H_982734

namespace gpk
{
	::gpk::error_t				d3dDeviceValidate			(::gpk::ptr_com<::ID3D11Device3> & d3dDevice);
	::gpk::error_t				d3dCreateTextureDynamic		(ID3D11Device* pDevice, ::gpk::pcom<ID3D11Texture2D> & texture, ::gpk::pcom<ID3D11ShaderResourceView> & shaderResourceView, ::gpk::v2<const ::gpk::bgra> initData); 
} // namespace

#endif // GPK_D3D_H_982734

#endif // GPK_WINDOWS