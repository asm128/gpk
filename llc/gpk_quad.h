#include "gpk_typeint.h"

#ifndef GPK_QUAD_H_23627
#define GPK_QUAD_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm T>
	struct quad {
		T	A, B, C, D;

		cxpr	quad	()											= default;
		cxpr	quad	(cnst quad<T> & other)						= default;
		cxpr	quad	(cnst T & a, cnst T & b, cnst T & c, cnst T & d)
			: A(a), B(b), C(c), D(d) {}

		GPK_DEFAULT_OPERATOR(quad<T>, A == other.A && B == other.B && C == other.C && D == other.D);
	};
#pragma pack(pop)

	tydf quad<uc_t>	quaduc_t;	tdcs quaduc_t	quaduc_c;
	tydf quad<sc_t>	quadsc_t;	tdcs quadsc_t	quadsc_c;
	tydf quad<u0_t>	quadu0_t;	tdcs quadu0_t	quadu0_c;
	tydf quad<u1_t>	quadu1_t;	tdcs quadu1_t	quadu1_c;
	tydf quad<u2_t>	quadu2_t;	tdcs quadu2_t	quadu2_c;
	tydf quad<u3_t>	quadu3_t;	tdcs quadu3_t	quadu3_c;
	tydf quad<s0_t>	quads0_t;	tdcs quads0_t	quads0_c;
	tydf quad<s1_t>	quads1_t;	tdcs quads1_t	quads1_c;
	tydf quad<s2_t>	quads2_t;	tdcs quads2_t	quads2_c;
	tydf quad<s3_t>	quads3_t;	tdcs quads3_t	quads3_c;
	tydf quad<f2_t>	quadf2_t;	tdcs quadf2_t	quadf2_c;
	tydf quad<f3_t>	quadf3_t;	tdcs quadf3_t	quadf3_c;
}

#endif // GPK_QUAD_H_23627
