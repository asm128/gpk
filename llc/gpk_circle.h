#include "gpk_n2.h"

#ifndef GPK_CIRCLE_H_23627
#define GPK_CIRCLE_H_23627

namespace gpk
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	tplt<tpnm T> 
	struct circle { 
		f3_t			Radius; 
		::gpk::n2<T>	Center; 

		GPK_DEFAULT_OPERATOR(circle<T>, Center == other.Center && Radius == other.Radius); 
	};
	typedef circle<f2_t>		circlef2_t, circlef;
	typedef circle<f3_t>		circlef64, circled;
	typedef circle<u0_t>		circleu8;
	typedef circle<u1_t>	circleu1_t;
	typedef circle<u2_t>	circleu32;
	typedef circle<u3_t>	circleu64;
	typedef circle<s0_t>		circlei8;
	typedef circle<s1_t>		circlei16;
	typedef circle<s2_t>		circlei32;
	typedef circle<s3_t>		circlei64;
#pragma pack(pop)
} // namespace

#endif // GPK_CIRCLE_H_23627
