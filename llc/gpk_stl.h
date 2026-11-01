#include "gpk_array.h"
#include "gpk_tri3.h"

#ifndef GPK_STL_H_23627
#define GPK_STL_H_23627

namespace gpk {
#pragma pack(push, 1)
	struct SSTLTriangle {
		n3f2_t				Normal			= {};
		tri3f2_t			Triangle		= {};
		u1_t				Attribute		= {};
	};
#pragma pack(pop)
	struct SSTLFile {
		au0_t				Raw				= {};
		vcu0_t				Header			= {};
		view<SSTLTriangle>	Triangles		= {};
	};

	err_t						stlFileLoad		(vcst_t filename, SSTLFile & file);
	err_t						stlFileLoad		(vu0_t fileInMemory, vcu0_t & out_Header, view<SSTLTriangle> & out_Triangles);
	err_t						stlFileLoad		(vcu0_t fileInMemory, vcu0_t & out_Header, view<cnst SSTLTriangle> & out_Triangles);
}

#endif // GPK_STL_H_23627
