#include "gpk_color.h"
#include "gpk_coord.h"
#include "gpk_engine_container.h"

#ifndef GPK_ENGINE_RENDERIMAGE_H
#define GPK_ENGINE_RENDERIMAGE_H

namespace gpk 
{
#pragma pack(push, 1)
	struct SSurfaceDescription {
		::gpk::COLOR_TYPE			ColorType			;
		int8_t						BitDepth			;
		int8_t						MethodCompression	;
		int8_t						MethodFilter		;
		int8_t						MethodInterlace		;
		::gpk::n2u16				Dimensions			;
	};
#pragma pack(pop)

	struct SSurface {
		::gpk::SSurfaceDescription	Desc;
		::gpk::au8					Data;

		::gpk::error_t				Save			(::gpk::apod<uint8_t> & output) const { 
			gpk_necs(::gpk::savePOD(output, Desc));
			gpk_necs(::gpk::saveView(output, Data));
			return 0;
		}
		::gpk::error_t				Load			(::gpk::vcu8 & input) {
			gpk_necs(::gpk::loadPOD(input, Desc));
			gpk_necs(::gpk::loadView(input, Data));
			return 0;
		}
	};

	typedef	::gpk::SLinearMap<::gpk::SSurface>	SSurfaceManager;
} // namespace

#endif
