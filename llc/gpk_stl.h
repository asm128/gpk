#include "gpk_coord.h"
#include "gpk_array.h"

#ifndef GPK_STL_H
#define GPK_STL_H

namespace gpk {
#pragma pack(push, 1)
	struct SSTLTriangle {
		::gpk::SCoord3<float>				Normal			= {};
		::gpk::STriangle3<float>			Triangle		= {};
		uint16_t							Attribute		= {};
	};
#pragma pack(pop)
	struct SSTLFile {
		::gpk::apod<uint8_t>				Raw				= {};
		::gpk::vcu8							Header			= {};
		::gpk::view<::gpk::SSTLTriangle>	Triangles		= {};
	};

	::gpk::error_t						stlFileLoad		(::gpk::vcc filename, ::gpk::SSTLFile& file);
	::gpk::error_t						stlFileLoad		(::gpk::vcu8 fileInMemory, ::gpk::vcu8 & out_Header, ::gpk::view<::gpk::SSTLTriangle>	& out_Triangles);
}

#endif // GPK_STL_H
