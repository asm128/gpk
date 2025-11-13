#include "gpk_array.h"
#include "gpk_tri3.h"

#ifndef GPK_STL_H_23627
#define GPK_STL_H_23627

namespace gpk {
#pragma pack(push, 1)
	struct SSTLTriangle {
		::gpk::n3f2_t				Normal			= {};
		::gpk::tri3f32				Triangle		= {};
		uint16_t					Attribute		= {};
	};
#pragma pack(pop)
	struct SSTLFile {
		::gpk::au0_t						Raw				= {};
		::gpk::vcu0_t						Header			= {};
		::gpk::view<::gpk::SSTLTriangle>	Triangles		= {};
	};

	::gpk::error_t						stlFileLoad		(::gpk::vcsc_t filename, ::gpk::SSTLFile & file);
	::gpk::error_t						stlFileLoad		(::gpk::vcu0_t fileInMemory, ::gpk::vcu0_t & out_Header, ::gpk::view<::gpk::SSTLTriangle> & out_Triangles);
}

#endif // GPK_STL_H_23627
