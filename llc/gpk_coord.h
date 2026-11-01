#include "gpk_n3.h"

#ifndef GPK_COORD_H_23627
#define GPK_COORD_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplT struct SOrigin {
		n3<T>	Front, Up, Right;

		GPK_DEFAULT_OPERATOR(SOrigin<T>, Front == other.Front && Up == other.Up && Right == other.Right);
	};
#pragma pack(pop)
}

#endif // GPK_COORD_H_23627
