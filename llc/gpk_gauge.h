#include "gpk_minmax.h"

#ifndef GPK_GAUGE_H
#define GPK_GAUGE_H

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm T> 
	struct gaugemax { 
		T							Limit;	// Inclusive
		T							Value;	

		GPK_DEFAULT_OPERATOR(minmax<T>, Limit == other.Limit && Value == other.Value); 

		cxpr f3_t				Weight			()		const	noexcept	{ return 1.0 / Limit * Value; }
		inxp	f3_t				WeightClamp		()		const	noexcept	{ return 1.0 / Limit * ::gpk::clamped(Value, (T)0, Limit); }
		T							SetClamp		(T value)		noexcept	{ return Value = ::gpk::clamped(value, 0, Limit); }
		T							SetWeighted		(f3_t weight)	noexcept	{ return SetClamp(T(Limit * weight)); }

		tplt<tpnm _tOther>
		gaugemax<_tOther>			Cast		()		const	noexcept	{ return {(_tOther)Limit, (_tOther)Value}; }

		inxp	minmax<u0_t>		u0_t			()		const	noexcept	{ return Cast<u0_t	>(); }
		inxp	minmax<u1_t>	u1_t			()		const	noexcept	{ return Cast<u1_t	>(); }
		inxp	minmax<u2_t>	u2_t			()		const	noexcept	{ return Cast<u2_t	>(); }
		inxp	minmax<u3_t>	u3_t			()		const	noexcept	{ return Cast<u3_t	>(); }
		inxp	minmax<s0_t>		s0_t			()		const	noexcept	{ return Cast<s0_t	>(); }
		inxp	minmax<s1_t>		s1_t			()		const	noexcept	{ return Cast<s1_t	>(); }
		inxp	minmax<s2_t>		s2_t			()		const	noexcept	{ return Cast<s2_t	>(); }
		inxp	minmax<s3_t>		s3_t			()		const	noexcept	{ return Cast<s3_t	>(); }
		inxp	minmax<f2_t>		f2_t			()		const	noexcept	{ return Cast<f2_t		>(); }
		inxp	minmax<f3_t>		f3_t			()		const	noexcept	{ return Cast<f3_t	>(); }
	};

	tplt<tpnm T> 
	struct gaugeminmax { 
		minmax<T>					Limits;	// Inclusive
		T							Value;	

		GPK_DEFAULT_OPERATOR(minmax<T>, Limits == other.Limits && Value == other.Value); 

		inxp	T					Middle		()		const	noexcept	{ return Limits.Min + Limits.Length() / 2; }
		cxpr	f3_t				Weight		()		const	noexcept	{ return Limits.Weight(Value); }
		inxp	f3_t				WeightClamp	()		const	noexcept	{ return Limits.WeightClamp(Value); }
		T							SetClamp	(T value)		noexcept	{ return Value = Limits.Clamp(value); }
		T							SetWeighted	(f3_t weight)	noexcept	{ return Set(Limits.Weighted(weight)); }

		tplt<tpnm _tOther>
		gaugeminmax<_tOther>		Cast		()		const	noexcept	{ return {Limits.Cast<_tOther>(), (_tOther)Value}; }

		inxp	minmax<u0_t>		u0_t			()		const	noexcept	{ return Cast<u0_t	>(); }
		inxp	minmax<u1_t>	u1_t			()		const	noexcept	{ return Cast<u1_t	>(); }
		inxp	minmax<u2_t>	u2_t			()		const	noexcept	{ return Cast<u2_t	>(); }
		inxp	minmax<u3_t>	u3_t			()		const	noexcept	{ return Cast<u3_t	>(); }
		inxp	minmax<s0_t>		s0_t			()		const	noexcept	{ return Cast<s0_t	>(); }
		inxp	minmax<s1_t>		s1_t			()		const	noexcept	{ return Cast<s1_t	>(); }
		inxp	minmax<s2_t>		s2_t			()		const	noexcept	{ return Cast<s2_t	>(); }
		inxp	minmax<s3_t>		s3_t			()		const	noexcept	{ return Cast<s3_t	>(); }
		inxp	minmax<f2_t>		f2_t			()		const	noexcept	{ return Cast<f2_t		>(); }
		inxp	minmax<f3_t>		f3_t			()		const	noexcept	{ return Cast<f3_t	>(); }
	};
#pragma pack(pop)
	typedef	gaugemax<sc_t		>	gaugemaxchar;
	typedef	gaugemax<uc_t	>	gaugemaxuchar;
	typedef gaugemax<f2_t		>	gaugemaxf2_t;
	typedef gaugemax<f3_t		>	gaugemaxf64;
	typedef gaugemax<u0_t	>	gaugemaxu8;
	typedef gaugemax<u1_t	>	gaugemaxu1_t;
	typedef gaugemax<u2_t	>	gaugemaxu32;
	typedef gaugemax<u3_t	>	gaugemaxu64;
	typedef gaugemax<s0_t		>	gaugemaxi8;
	typedef gaugemax<s1_t	>	gaugemaxi16;
	typedef gaugemax<s2_t	>	gaugemaxi32;
	typedef gaugemax<s3_t	>	gaugemaxi64;

	typedef	gaugeminmax<sc_t	>	gaugeminmaxchar;
	typedef	gaugeminmax<uc_t	>	gaugeminmaxuchar;
	typedef gaugeminmax<f2_t	>	gaugeminmaxf2_t;
	typedef gaugeminmax<f3_t	>	gaugeminmaxf64;
	typedef gaugeminmax<u0_t	>	gaugeminmaxu8;
	typedef gaugeminmax<u1_t>	gaugeminmaxu1_t;
	typedef gaugeminmax<u2_t>	gaugeminmaxu32;
	typedef gaugeminmax<u3_t>	gaugeminmaxu64;
	typedef gaugeminmax<s0_t	>	gaugeminmaxi8;
	typedef gaugeminmax<s1_t	>	gaugeminmaxi16;
	typedef gaugeminmax<s2_t	>	gaugeminmaxi32;
	typedef gaugeminmax<s3_t	>	gaugeminmaxi64;
}

#endif // GPK_GAUGE_H
