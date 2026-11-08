#include "gpk_typeint.h"

#ifndef GPK_LINE_H_23627
#define GPK_LINE_H_23627

nmsp gpk
{
#pragma pack(push, 1)	// You can read about pragma pack() here: https://www.google.com/search?q=pragma+pack
	tplt<tpnm _tVertex>
	struct line { 
		_tVertex				A, B; 

		inxp					line		()											= default;
		inxp					line		(const line<_tVertex> & other)				= default;
		inxp					line		(const _tVertex & a, const _tVertex & b)	: A(a), B(b)	{}

		GPK_DEFAULT_OPERATOR(line<_tVertex>, A == other.A && B == other.B);
	};
#pragma pack(pop)
} // nmsp

#endif // GPK_LINE_H_23627
