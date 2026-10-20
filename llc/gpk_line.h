#include "gpk_typeint.h"

#ifndef GPK_LINE_H_23627
#define GPK_LINE_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm T>
	struct line {
		T	A, B;

		cxpr	line	()							= default;
		cxpr	line	(cnst line<T> & other)		= default;
		cxpr	line	(cnst T & a, cnst T & b)	: A(a), B(b) {}

		GPK_DEFAULT_OPERATOR(line<T>, A == other.A && B == other.B);
	};
#pragma pack(pop)
}

#endif // GPK_LINE_H_23627


