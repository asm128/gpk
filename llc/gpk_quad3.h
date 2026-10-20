#include "gpk_quad.h"
#include "gpk_n3.h"

#ifndef GPK_QUAD3_H_23627
#define GPK_QUAD3_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tpl_t struct quad3 : public quad<n3<_t>> {
		tydf	_t			T;
		tydf	n3<T>		TVertex;

		using	quad<TVertex>	::A;
		using	quad<TVertex>	::B;
		using	quad<TVertex>	::C;
		using	quad<TVertex>	::D;
		using	quad<TVertex>	::quad;
	};
#pragma pack(pop)

	tydf quad3<uc_t>	quad3uc_t;	tdcs quad3uc_t	quad3uc_c;
	tydf quad3<sc_t>	quad3sc_t;	tdcs quad3sc_t	quad3sc_c;
	tydf quad3<u0_t>	quad3u0_t;	tdcs quad3u0_t	quad3u0_c;
	tydf quad3<u1_t>	quad3u1_t;	tdcs quad3u1_t	quad3u1_c;
	tydf quad3<u2_t>	quad3u2_t;	tdcs quad3u2_t	quad3u2_c;
	tydf quad3<u3_t>	quad3u3_t;	tdcs quad3u3_t	quad3u3_c;
	tydf quad3<s0_t>	quad3s0_t;	tdcs quad3s0_t	quad3s0_c;
	tydf quad3<s1_t>	quad3s1_t;	tdcs quad3s1_t	quad3s1_c;
	tydf quad3<s2_t>	quad3s2_t;	tdcs quad3s2_t	quad3s2_c;
	tydf quad3<s3_t>	quad3s3_t;	tdcs quad3s3_t	quad3s3_c;
	tydf quad3<f2_t>	quad3f2_t;	tdcs quad3f2_t	quad3f2_c;
	tydf quad3<f3_t>	quad3f3_t;	tdcs quad3f3_t	quad3f3_c;
}

#endif // GPK_QUAD3_H_23627


