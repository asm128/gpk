#include "gpk_coord.h"
#include "gpk_array.h"

#ifndef GPK_STL_H
#define GPK_STL_H

namespace gpk {
#pragma pack(push, 1)
	struct SSTLTriangle {
		::gpk::SCoord3<float>					Normal		= {};
		::gpk::STriangle3<float>				Triangle	= {};
		uint16_t								Attribute	= {};
	};
	struct SSTLFile {
		::gpk::array_pod<byte_t>				Raw			= {};
		::gpk::view_array<uint8_t>				Header		= {};
		::gpk::view_array<::gpk::SSTLTriangle>	Triangles	= {};
	};
#pragma pack(pop)

	::gpk::error_t							stlFileLoad						(::gpk::view_const_char filename, ::gpk::SSTLFile& file);
}

#endif // GPK_STL_H
