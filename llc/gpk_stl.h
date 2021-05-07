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
#pragma pack(pop)
	struct SSTLFile {
		::gpk::array_pod<byte_t>				Raw			= {};
		::gpk::view_byte						Header		= {};
		::gpk::view_array<::gpk::SSTLTriangle>	Triangles	= {};
	};

	::gpk::error_t			stlFileLoad			(::gpk::view_const_char filename, ::gpk::SSTLFile& file);
	::gpk::error_t			stlFileLoad			(::gpk::view_byte fileInMemory, ::gpk::view_byte & out_Header, ::gpk::view_array<::gpk::SSTLTriangle>	& out_Triangles);
}

#endif // GPK_STL_H
