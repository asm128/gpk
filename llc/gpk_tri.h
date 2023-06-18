#include "gpk_typeint.h"

#ifndef GPK_TRI_H_230516
#define GPK_TRI_H_230516

namespace gpk 
{
#pragma pack(push, 1)
	template<typename _tVertex>
	struct tri { 
		_tVertex	A, B, C; 

		cnstxpr		tri()															= default;
		cnstxpr		tri(const tri & other)											= default;
		cnstxpr		tri(const _tVertex & a, const _tVertex & b, const _tVertex & c)	: A(a), B(b), C(c)	{}

		GPK_DEFAULT_OPERATOR(tri<_tVertex>, A == other.A && B == other.B && C == other.C);
	};
	typedef	tri<char>		trichar;
	typedef	tri<uchar_t>	triuchar;
	typedef	tri<float>		trif32;
	typedef	tri<double>		trif64;
	typedef	tri<uint8_t>	triu8;
	typedef	tri<uint16_t>	triu16;
	typedef	tri<uint32_t>	triu32;
	typedef	tri<uint64_t>	triu64;
	typedef	tri<int8_t>		trii8;
	typedef	tri<int16_t>	trii16;
	typedef	tri<int32_t>	trii32;
	typedef	tri<int64_t>	trii64;

	typedef	minmax<trichar>		minmaxtrichar;
	typedef	minmax<triuchar>	minmaxtriuchar;
	typedef	minmax<trif32>		minmaxtrif32;
	typedef	minmax<trif64>		minmaxtrif64;
	typedef	minmax<triu8 >		minmaxtriu8;
	typedef	minmax<triu16>		minmaxtriu16;
	typedef	minmax<triu32>		minmaxtriu32;
	typedef	minmax<triu64>		minmaxtriu64;
	typedef	minmax<trii8 >		minmaxtrii8;
	typedef	minmax<trii16>		minmaxtrii16;
	typedef	minmax<trii32>		minmaxtrii32;
	typedef	minmax<trii64>		minmaxtrii64;
#pragma pack(pop)
} // namespace 

#endif // GPK_TRI_H_230516