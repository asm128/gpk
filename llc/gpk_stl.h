#include "gpk_array.h"
#include "gpk_tri3.h"

#ifndef GPK_STL_H
#define GPK_STL_H

namespace gpk {
#pragma pack(push, 1)
	struct SSTLTriangle {
		::gpk::n3f32				Normal			= {};
		::gpk::tri3f32				Triangle		= {};
		uint16_t					Attribute		= {};
	};
#pragma pack(pop)
	struct SSTLFile {
		::gpk::au8							Raw				= {};
		::gpk::vcu8							Header			= {};
		::gpk::view<::gpk::SSTLTriangle>	Triangles		= {};
	};

	::gpk::error_t						stlFileLoad		(::gpk::vcc filename, ::gpk::SSTLFile& file);
	::gpk::error_t						stlFileLoad		(::gpk::vcu8 fileInMemory, ::gpk::vcu8 & out_Header, ::gpk::view<::gpk::SSTLTriangle>	& out_Triangles);
}

#endif // GPK_STL_H
