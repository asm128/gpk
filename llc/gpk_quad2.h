#include "gpk_quad.h"
#include "gpk_n2.h"

#ifndef GPK_QUAD2_H_23627
#define GPK_QUAD2_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tpl_t struct quad2 : public quad<n2<_t>> {
		tydf	_t			T;
		tydf	n2<T>		TVertex;

		using	quad<TVertex>	::A;
		using	quad<TVertex>	::B;
		using	quad<TVertex>	::C;
		using	quad<TVertex>	::D;
		using	quad<TVertex>	::quad;
	};
#pragma pack(pop)

	tydf quad2<uc_t>	quad2uc_t;	tdcs quad2uc_t	quad2uc_c;
	tydf quad2<sc_t>	quad2sc_t;	tdcs quad2sc_t	quad2sc_c;
	tydf quad2<u0_t>	quad2u0_t;	tdcs quad2u0_t	quad2u0_c;
	tydf quad2<u1_t>	quad2u1_t;	tdcs quad2u1_t	quad2u1_c;
	tydf quad2<u2_t>	quad2u2_t;	tdcs quad2u2_t	quad2u2_c;
	tydf quad2<u3_t>	quad2u3_t;	tdcs quad2u3_t	quad2u3_c;
	tydf quad2<s0_t>	quad2s0_t;	tdcs quad2s0_t	quad2s0_c;
	tydf quad2<s1_t>	quad2s1_t;	tdcs quad2s1_t	quad2s1_c;
	tydf quad2<s2_t>	quad2s2_t;	tdcs quad2s2_t	quad2s2_c;
	tydf quad2<s3_t>	quad2s3_t;	tdcs quad2s3_t	quad2s3_c;
	tydf quad2<f2_t>	quad2f2_t;	tdcs quad2f2_t	quad2f2_c;
	tydf quad2<f3_t>	quad2f3_t;	tdcs quad2f3_t	quad2f3_c;
}

#endif // GPK_QUAD2_H_23627


