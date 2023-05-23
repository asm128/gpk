#include "gpk_n3.h"

#ifndef GPK_QUAD_H_230523
#define GPK_QUAD_H_230523

namespace gpk 
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	template<typename _tDim>	struct SQuad	{ _tDim				A, B, C, D; GPK_DEFAULT_OPERATOR_NE(SQuad <_tDim>, A == other.A && B == other.B && C == other.C && D == other.D); };
	template<typename _tDim>	struct SQuad2	{ ::gpk::n2<_tDim>	A, B, C, D; GPK_DEFAULT_OPERATOR_NE(SQuad2<_tDim>, A == other.A && B == other.B && C == other.C && D == other.D); };
	template<typename _tDim>	struct SQuad3	{ ::gpk::n3<_tDim>	A, B, C, D; GPK_DEFAULT_OPERATOR_NE(SQuad3<_tDim>, A == other.A && B == other.B && C == other.C && D == other.D); };
#pragma pack(pop)
}
#endif // GPK_QUAD_H_230523