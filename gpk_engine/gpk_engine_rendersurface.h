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
		s0_t						BitDepth			= {};
		s0_t						MethodCompression	= {};
		s0_t						MethodFilter		= {};
		s0_t						MethodInterlace		= {};
		::gpk::n2u1_t				Dimensions			= {};
	};
#pragma pack(pop)

	struct SSurface {
		::gpk::SSurfaceDescription	Desc			= {};
		::gpk::au0_t				Data			= {};

		gpk::err_t 			Save			(::gpk::au0_t & output)	const	{ if_fail_fe(gpk::savePOD(output, Desc)); return ::gpk::saveView(output, Data); }
		gpk::err_t 			Load			(::gpk::vcu0_t & input)			{ if_fail_fe(gpk::loadPOD(input, Desc)); return ::gpk::loadView(input, Data); }
	};

	typedef	::gpk::gpk_linear_map_pobj<::gpk::SSurface>	SSurfaceManager;
} // namespace

#endif // GPK_ENGINE_RENDERIMAGE_H_23627
