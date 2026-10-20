#include "gpk_typeint.h"

#ifndef GPK_TRI_H_23627
#define GPK_TRI_H_23627

namespace gpk 
{
#pragma pack(push, 1)
	tplt<tpnm _tVertex>
	struct tri { 
		_tVertex	A, B, C; 

		cxpr		tri()															nxpt	= default;
		cxpr		tri(cnst tri & other)											nxpt	= default;
		cxpr		tri(cnst _tVertex & a, cnst _tVertex & b, cnst _tVertex & c)	nxpt	: A(a), B(b), C(c)	{}

		GPK_DEFAULT_OPERATOR(tri<_tVertex>, A == other.A && B == other.B && C == other.C);
	};
	tydf	tri<uc_t>	triuc_t;	tdcs triuc_t	triuc_c;
	tydf	tri<sc_t>	trisc_t;	tdcs trisc_t	trisc_c;
	tydf	tri<u0_t>	triu0_t;	tdcs triu0_t	triu0_c;
	tydf	tri<u1_t>	triu1_t;	tdcs triu1_t	triu1_c;
	tydf	tri<u2_t>	triu2_t;	tdcs triu2_t	triu2_c;
	tydf	triu2_t	triu32;
	tydf	tri<u3_t>	triu3_t;	tdcs triu3_t	triu3_c;
	tydf	tri<s0_t>	tris0_t;	tdcs tris0_t	tris0_c;
	tydf	tri<s1_t>	tris1_t;	tdcs tris1_t	tris1_c;
	tydf	tri<s2_t>	tris2_t;	tdcs tris2_t	tris2_c;
	tydf	tri<s3_t>	tris3_t;	tdcs tris3_t	tris3_c;
	tydf	tri<f2_t>	trif2_t;	tdcs trif2_t	trif2_c;
	tydf	trif2_t	trif32;
	tydf	tri<f3_t>	trif3_t;	tdcs trif3_t	trif3_c;

#pragma pack(pop)
} // namespace 

#endif // GPK_TRI_H_23627

