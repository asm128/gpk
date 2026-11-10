#include "gpk_typeint.h"

#ifndef GPK_TRI_H_23627
#define GPK_TRI_H_23627

namespace gpk 
{
#pragma pack(push, 1)
	tplt<tpnm _tVertex>
	struct tri { 
		_tVertex	A, B, C; 

		cxpr		tri		()																= default;
		cxpr		tri		(const tri & other)												= default;
		cxpr		tri		(const _tVertex & a, const _tVertex & b, const _tVertex & c)	: A(a), B(b), C(c)	{}

		GPK_DEFAULT_OPERATOR(tri<_tVertex>, A == other.A && B == other.B && C == other.C);
	};
	tydf tri<sc_t> trisc_t; tdcs trisc_t trisc_c;
	tydf tri<uc_t> triuc_t; tdcs triuc_t triuc_c;
	tydf tri<u0_t> triu0_t; tdcs triu0_t triu0_c;
	tydf tri<u1_t> triu1_t; tdcs triu1_t triu1_c;
	tydf tri<u2_t> triu2_t; tdcs triu2_t triu2_c;
	tydf tri<u3_t> triu3_t; tdcs triu3_t triu3_c;
	tydf tri<s0_t> tris0_t; tdcs tris0_t tris0_c;
	tydf tri<s1_t> tris1_t; tdcs tris1_t tris1_c;
	tydf tri<s2_t> tris2_t; tdcs tris2_t tris2_c;
	tydf tri<s3_t> tris3_t; tdcs tris3_t tris3_c;
	tydf tri<f2_t> trif2_t; tdcs trif2_t trif2_c;
	tydf tri<f3_t> trif3_t; tdcs trif3_t trif3_c;

	//typedef	minmax<trisc_t>		minmaxtrichar;
	//typedef	minmax<triuc_t>		minmaxtriuchar;
	//typedef	minmax<trif2_t>		minmaxtrif32;
	//typedef	minmax<trif3_t>		minmaxtrif64;
	//typedef	minmax<triu0_t >	minmaxtriu8;
	//typedef	minmax<triu1_t>		minmaxtriu16;
	//typedef	minmax<triu2_t>		minmaxtriu32;
	//typedef	minmax<triu3_t>		minmaxtriu64;
	//typedef	minmax<tris0_t >	minmaxtrii8;
	//typedef	minmax<tris1_t>		minmaxtrii16;
	//typedef	minmax<tris2_t>		minmaxtrii32;
	//typedef	minmax<tris3_t>		minmaxtrii64;
#pragma pack(pop)
} // namespace 

#endif // GPK_TRI_H_23627