#include "gpk_n3.h"

#ifndef GPK_RECTANGLE3_H_23627
#define GPK_RECTANGLE3_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm T>
	struct rect3 { 
		::gpk::n3<T>			Offset, Size; 

		GPK_DEFAULT_OPERATOR(rect3<T>, Offset == other.Offset && Size == other.Size); 
	};
#pragma pack(pop)
} // namespace

#endif // GPK_RECTANGLE3_H_23627
