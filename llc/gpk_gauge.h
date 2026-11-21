#include "gpk_minmax.h"

#ifndef GPK_GAUGE_H
#define GPK_GAUGE_H

nmsp gpk
{
#pragma pack(push, 1)
	tplt<tpnm T> 
	struct gaugemax { 
		T							Limit;	// Inclusive
		T							Value;	

		GPK_DEFAULT_OPERATOR(minmax<T>, Limit == other.Limit && Value == other.Value); 

		cxpr f3_t					Weight			()		csnx	{ return 1.0 / Limit * Value; }
		inxp	f3_t				WeightClamp		()		csnx	{ return 1.0 / Limit * ::gpk::clamped(Value, (T)0, Limit); }
		T							SetClamp		(T value)		nxpt	{ return Value = ::gpk::clamped(value, 0, Limit); }
		T							SetWeighted		(f3_t weight)	nxpt	{ return SetClamp(T(Limit * weight)); }

		tplt<tpnm _tOther>
		gaugemax<_tOther>			Cast		()		csnx	{ return {(_tOther)Limit, (_tOther)Value}; }

		inxp	minmax<u0_t>		u0			()		csnx	{ return Cast<u0_t	>(); }
		inxp	minmax<u1_t>		u1			()		csnx	{ return Cast<u1_t	>(); }
		inxp	minmax<u2_t>		u2			()		csnx	{ return Cast<u2_t	>(); }
		inxp	minmax<u3_t>		u3			()		csnx	{ return Cast<u3_t	>(); }
		inxp	minmax<s0_t>		s0			()		csnx	{ return Cast<s0_t	>(); }
		inxp	minmax<s1_t>		s1			()		csnx	{ return Cast<s1_t	>(); }
		inxp	minmax<s2_t>		s2			()		csnx	{ return Cast<s2_t	>(); }
		inxp	minmax<s3_t>		s3			()		csnx	{ return Cast<s3_t	>(); }
		inxp	minmax<f2_t>		f2			()		csnx	{ return Cast<f2_t		>(); }
		inxp	minmax<f3_t>		f3			()		csnx	{ return Cast<f3_t	>(); }
	};

	tplt<tpnm T> 
	struct gaugeminmax { 
		minmax<T>					Limits;	// Inclusive
		T							Value;	

		GPK_DEFAULT_OPERATOR(minmax<T>, Limits == other.Limits && Value == other.Value); 

		inxp	T					Middle		()		csnx	{ return Limits.Min + Limits.Length() / 2; }
		cxpr	f3_t				Weight		()		csnx	{ return Limits.Weight(Value); }
		inxp	f3_t				WeightClamp	()		csnx	{ return Limits.WeightClamp(Value); }
		T							SetClamp	(T value)		nxpt	{ return Value = Limits.Clamp(value); }
		T							SetWeighted	(f3_t weight)	nxpt	{ return Set(Limits.Weighted(weight)); }

		tplt<tpnm _tOther>
		gaugeminmax<_tOther>		Cast		()		csnx	{ return {Limits.Cast<_tOther>(), (_tOther)Value}; }

		inxp	minmax<u0_t>		u0			()		csnx	{ return Cast<u0_t	>(); }
		inxp	minmax<u1_t>		u1			()		csnx	{ return Cast<u1_t	>(); }
		inxp	minmax<u2_t>		u2			()		csnx	{ return Cast<u2_t	>(); }
		inxp	minmax<u3_t>		u3			()		csnx	{ return Cast<u3_t	>(); }
		inxp	minmax<s0_t>		s0			()		csnx	{ return Cast<s0_t	>(); }
		inxp	minmax<s1_t>		s1			()		csnx	{ return Cast<s1_t	>(); }
		inxp	minmax<s2_t>		s2			()		csnx	{ return Cast<s2_t	>(); }
		inxp	minmax<s3_t>		s3			()		csnx	{ return Cast<s3_t	>(); }
		inxp	minmax<f2_t>		f2			()		csnx	{ return Cast<f2_t		>(); }
		inxp	minmax<f3_t>		f3			()		csnx	{ return Cast<f3_t	>(); }
	};
#pragma pack(pop)
	tydf gaugemax<sc_t>	gaugemaxsc_t; tdcs gaugemaxsc_t gaugemaxsc_c;
	tydf gaugemax<uc_t>	gaugemaxuc_t; tdcs gaugemaxuc_t gaugemaxuc_c;
	tydf gaugemax<f2_t>	gaugemaxf2_t; tdcs gaugemaxf2_t gaugemaxf2_c;
	tydf gaugemax<f3_t>	gaugemaxf3_t; tdcs gaugemaxf3_t gaugemaxf3_c;
	tydf gaugemax<u0_t>	gaugemaxu0_t; tdcs gaugemaxu0_t gaugemaxu0_c;
	tydf gaugemax<u1_t>	gaugemaxu1_t; tdcs gaugemaxu1_t gaugemaxu1_c;
	tydf gaugemax<u2_t>	gaugemaxu2_t; tdcs gaugemaxu2_t gaugemaxu2_c;
	tydf gaugemax<u3_t>	gaugemaxu3_t; tdcs gaugemaxu3_t gaugemaxu3_c;
	tydf gaugemax<s0_t>	gaugemaxs0_t; tdcs gaugemaxs0_t gaugemaxs0_c;
	tydf gaugemax<s1_t>	gaugemaxs1_t; tdcs gaugemaxs1_t gaugemaxs1_c;
	tydf gaugemax<s2_t>	gaugemaxs2_t; tdcs gaugemaxs2_t gaugemaxs2_c;
	tydf gaugemax<s3_t>	gaugemaxs3_t; tdcs gaugemaxs3_t gaugemaxs3_c;

	tydf gaugeminmax<sc_t>	gaugeminmaxsc_t; tdcs gaugeminmaxsc_t gaugeminmaxsc_c;
	tydf gaugeminmax<uc_t>	gaugeminmaxuc_t; tdcs gaugeminmaxuc_t gaugeminmaxuc_c;
	tydf gaugeminmax<f2_t>	gaugeminmaxf2_t; tdcs gaugeminmaxf2_t gaugeminmaxf2_c;
	tydf gaugeminmax<f3_t>	gaugeminmaxf3_t; tdcs gaugeminmaxf3_t gaugeminmaxf3_c;
	tydf gaugeminmax<u0_t>	gaugeminmaxu0_t; tdcs gaugeminmaxu0_t gaugeminmaxu0_c;
	tydf gaugeminmax<u1_t>	gaugeminmaxu1_t; tdcs gaugeminmaxu1_t gaugeminmaxu1_c;
	tydf gaugeminmax<u2_t>	gaugeminmaxu2_t; tdcs gaugeminmaxu2_t gaugeminmaxu2_c;
	tydf gaugeminmax<u3_t>	gaugeminmaxu3_t; tdcs gaugeminmaxu3_t gaugeminmaxu3_c;
	tydf gaugeminmax<s0_t>	gaugeminmaxs0_t; tdcs gaugeminmaxs0_t gaugeminmaxs0_c;
	tydf gaugeminmax<s1_t>	gaugeminmaxs1_t; tdcs gaugeminmaxs1_t gaugeminmaxs1_c;
	tydf gaugeminmax<s2_t>	gaugeminmaxs2_t; tdcs gaugeminmaxs2_t gaugeminmaxs2_c;
	tydf gaugeminmax<s3_t>	gaugeminmaxs3_t; tdcs gaugeminmaxs3_t gaugeminmaxs3_c;
}

#endif // GPK_GAUGE_H
