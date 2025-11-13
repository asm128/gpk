#include "gpk_typeint.h"

#ifndef GPK_MINMAX_H
#define GPK_MINMAX_H

namespace gpk
{
#pragma pack(push, 1)
	tpl_t struct minmax {
		tydf	_t				T;
		tydf	minmax<T>		TMinMax;

		T						Min, Max;

		GPK_DEFAULT_OPERATOR(minmax<T>, Max == other.Max && Min == other.Min);

		sinx	TMinMax			from		(T value)			nxpt	{ return {value, value}; }
		inxp	T				Length		()				csnx	{ return T(Max - Min); }
		inxp	T				Middle		()				csnx	{ return Min + Length() / 2; }
		inxp	T				Clamp		(T value)		csnx	{ return ::gpk::clamped(value, Min, Max); }
		T						Weighted	(f3_t weight)	csnx	{ return T(Min + Length() * weight); }
		f3_t					Weight		(T value)		csnx	{ return 1.0 / Length() * (value - Min); }
		f3_t					WeightClamp	(T value)		csnx	{ return Weight(Clamp(value)); }

		tpl_t2
		inxp	minmax<_t2> 	Cast		()				csnx	{ return{(_t2)Min, (_t2)Max}; } 

		inxp	minmax<uc_t> 	uc			()				csnx	{ return Cast<uc_t>(); }
		inxp	minmax<sc_t> 	sc			()				csnx	{ return Cast<sc_t>(); }
		inxp	minmax<u0_t> 	u0			()				csnx	{ return Cast<u0_t>(); }
		inxp	minmax<u1_t> 	u1			()				csnx	{ return Cast<u1_t>(); }
		inxp	minmax<u2_t> 	u2			()				csnx	{ return Cast<u2_t>(); }
		inxp	minmax<u3_t> 	u3			()				csnx	{ return Cast<u3_t>(); }
		inxp	minmax<s0_t> 	s0			()				csnx	{ return Cast<s0_t>(); }
		inxp	minmax<s1_t> 	s1			()				csnx	{ return Cast<s1_t>(); }
		inxp	minmax<s2_t> 	s2			()				csnx	{ return Cast<s2_t>(); }
		inxp	minmax<s3_t> 	s3			()				csnx	{ return Cast<s3_t>(); }
		inxp	minmax<f2_t> 	f2			()				csnx	{ return Cast<f2_t>(); }
		inxp	minmax<f3_t> 	f3			()				csnx	{ return Cast<f3_t>(); }

		inline	TMinMax&		Set			(T value)			nxpt	{ Min = Max = value; return *this; }
		inline	TMinMax&		From		(T value)			nxpt	{ Min = Max = value; return *this; }
	};
#pragma pack(pop)
	tydf	minmax<uc_t>	minmaxuc_t;	tdcs	minmaxuc_t	minmaxuc_c;
	tydf	minmax<sc_t>	minmaxsc_t;	tdcs	minmaxsc_t	minmaxsc_c;
	tydf	minmax<u0_t>	minmaxu0_t;	tdcs	minmaxu0_t	minmaxu0_c;
	tydf	minmax<u1_t>	minmaxu1_t;	tdcs	minmaxu1_t	minmaxu1_c;
	tydf	minmax<u2_t>	minmaxu2_t;	tdcs	minmaxu2_t	minmaxu2_c;
	tydf	minmax<u3_t>	minmaxu3_t;	tdcs	minmaxu3_t	minmaxu3_c;
	tydf	minmax<s0_t>	minmaxs0_t;	tdcs	minmaxs0_t	minmaxs0_c;
	tydf	minmax<s1_t>	minmaxs1_t;	tdcs	minmaxs1_t	minmaxs1_c;
	tydf	minmax<s2_t>	minmaxs2_t;	tdcs	minmaxs2_t	minmaxs2_c;
	tydf	minmax<s3_t>	minmaxs3_t;	tdcs	minmaxs3_t	minmaxs3_c;
	tydf	minmax<f2_t>	minmaxf2_t;	tdcs	minmaxf2_t	minmaxf2_c;
	tydf	minmax<f3_t>	minmaxf3_t;	tdcs	minmaxf3_t	minmaxf3_c;
}

#endif // GPK_MINMAX_H
