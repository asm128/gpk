#include "gpk_n3.h"

#ifndef GPK_COORD_H_23627
#define GPK_COORD_H_23627

namespace gpk
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	// ---- Geometric figures and other coord-related POD structs.
	tplt<tpnm T>
	struct SOrigin { 
		::gpk::n3<T>	Front, Up, Right; 
		GPK_DEFAULT_OPERATOR(SOrigin<T>, Front == other.Front && Up == other.Up && Right == other.Right); 
	};
#pragma pack(pop)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
}

#endif // GPK_COORD_H_23627
