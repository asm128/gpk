#include "gpk_color.h"
#include "gpk_n2.h"
#include "gpk_linear_map_pobj.h"
#include "gpk_color_type.h"

#ifndef GPK_ENGINE_RENDERIMAGE_H_23627
#define GPK_ENGINE_RENDERIMAGE_H_23627

namespace gpk 
{
#pragma pack(push, 1)
	struct SSurfaceDescription {
		::gpk::COLOR_TYPE			ColorType			= {};
		int8_t						BitDepth			= {};
		int8_t						MethodCompression	= {};
		int8_t						MethodFilter		= {};
		int8_t						MethodInterlace		= {};
		::gpk::n2u16				Dimensions			= {};
	};
#pragma pack(pop)

	struct SSurface {
		::gpk::SSurfaceDescription	Desc			= {};
		::gpk::au8					Data			= {};

		::gpk::error_t				Save			(::gpk::au8 & output)	const	{ gpk_necs(::gpk::savePOD(output, Desc)); return ::gpk::saveView(output, Data); }
		::gpk::error_t				Load			(::gpk::vcu8 & input)			{ gpk_necs(::gpk::loadPOD(input, Desc)); return ::gpk::loadView(input, Data); }
	};

	typedef	::gpk::gpk_linear_map_pobj<::gpk::SSurface>	SSurfaceManager;
} // namespace

#endif // GPK_ENGINE_RENDERIMAGE_H_23627
