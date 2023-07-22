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

		cnstxpr double				Weight			()		const	noexcept	{ return 1.0 / Limit * Value; }
		inlcxpr	double				WeightClamp		()		const	noexcept	{ return 1.0 / Limit * ::gpk::clamped(Value, (T)0, Limit); }
		T							SetClamp		(T value)		noexcept	{ return Value = ::gpk::clamped(value, 0, Limit); }
		T							SetWeighted		(double weight)	noexcept	{ return SetClamp(T(Limit * weight)); }

		tplt<tpnm _tOther>
		gaugemax<_tOther>			Cast		()		const	noexcept	{ return {(_tOther)Limit, (_tOther)Value}; }

		inlcxpr	minmax<uint8_t>		u8			()		const	noexcept	{ return Cast<uint8_t	>(); }
		inlcxpr	minmax<uint16_t>	u16			()		const	noexcept	{ return Cast<uint16_t	>(); }
		inlcxpr	minmax<uint32_t>	u32			()		const	noexcept	{ return Cast<uint32_t	>(); }
		inlcxpr	minmax<uint64_t>	u64			()		const	noexcept	{ return Cast<uint64_t	>(); }
		inlcxpr	minmax<int8_t>		i8			()		const	noexcept	{ return Cast<int8_t	>(); }
		inlcxpr	minmax<int16_t>		i16			()		const	noexcept	{ return Cast<int16_t	>(); }
		inlcxpr	minmax<int32_t>		i32			()		const	noexcept	{ return Cast<int32_t	>(); }
		inlcxpr	minmax<int64_t>		i64			()		const	noexcept	{ return Cast<int64_t	>(); }
		inlcxpr	minmax<float>		f32			()		const	noexcept	{ return Cast<float		>(); }
		inlcxpr	minmax<double>		f64			()		const	noexcept	{ return Cast<double	>(); }
	};

	tplt<tpnm T> 
	struct gaugeminmax { 
		minmax<T>					Limits;	// Inclusive
		T							Value;	

		GPK_DEFAULT_OPERATOR(minmax<T>, Limits == other.Limits && Value == other.Value); 

		inlcxpr	T					Middle		()		const	noexcept	{ return Limits.Min + Limits.Length() / 2; }
		cnstxpr	double				Weight		()		const	noexcept	{ return Limits.Weight(Value); }
		inlcxpr	double				WeightClamp	()		const	noexcept	{ return Limits.WeightClamp(Value); }
		T							SetClamp	(T value)		noexcept	{ return Value = Limits.Clamp(value); }
		T							SetWeighted	(double weight)	noexcept	{ return Set(Limits.Weighted(weight)); }

		tplt<tpnm _tOther>
		gaugeminmax<_tOther>		Cast		()		const	noexcept	{ return {Limits.Cast<_tOther>(), (_tOther)Value}; }

		inlcxpr	minmax<uint8_t>		u8			()		const	noexcept	{ return Cast<uint8_t	>(); }
		inlcxpr	minmax<uint16_t>	u16			()		const	noexcept	{ return Cast<uint16_t	>(); }
		inlcxpr	minmax<uint32_t>	u32			()		const	noexcept	{ return Cast<uint32_t	>(); }
		inlcxpr	minmax<uint64_t>	u64			()		const	noexcept	{ return Cast<uint64_t	>(); }
		inlcxpr	minmax<int8_t>		i8			()		const	noexcept	{ return Cast<int8_t	>(); }
		inlcxpr	minmax<int16_t>		i16			()		const	noexcept	{ return Cast<int16_t	>(); }
		inlcxpr	minmax<int32_t>		i32			()		const	noexcept	{ return Cast<int32_t	>(); }
		inlcxpr	minmax<int64_t>		i64			()		const	noexcept	{ return Cast<int64_t	>(); }
		inlcxpr	minmax<float>		f32			()		const	noexcept	{ return Cast<float		>(); }
		inlcxpr	minmax<double>		f64			()		const	noexcept	{ return Cast<double	>(); }
	};
#pragma pack(pop)
	typedef	gaugemax<char		>	gaugemaxchar;
	typedef	gaugemax<uchar_t	>	gaugemaxuchar;
	typedef gaugemax<float		>	gaugemaxf32;
	typedef gaugemax<double		>	gaugemaxf64;
	typedef gaugemax<uint8_t	>	gaugemaxu8;
	typedef gaugemax<uint16_t	>	gaugemaxu16;
	typedef gaugemax<uint32_t	>	gaugemaxu32;
	typedef gaugemax<uint64_t	>	gaugemaxu64;
	typedef gaugemax<int8_t		>	gaugemaxi8;
	typedef gaugemax<int16_t	>	gaugemaxi16;
	typedef gaugemax<int32_t	>	gaugemaxi32;
	typedef gaugemax<int64_t	>	gaugemaxi64;

	typedef	gaugeminmax<char	>	gaugeminmaxchar;
	typedef	gaugeminmax<uchar_t	>	gaugeminmaxuchar;
	typedef gaugeminmax<float	>	gaugeminmaxf32;
	typedef gaugeminmax<double	>	gaugeminmaxf64;
	typedef gaugeminmax<uint8_t	>	gaugeminmaxu8;
	typedef gaugeminmax<uint16_t>	gaugeminmaxu16;
	typedef gaugeminmax<uint32_t>	gaugeminmaxu32;
	typedef gaugeminmax<uint64_t>	gaugeminmaxu64;
	typedef gaugeminmax<int8_t	>	gaugeminmaxi8;
	typedef gaugeminmax<int16_t	>	gaugeminmaxi16;
	typedef gaugeminmax<int32_t	>	gaugeminmaxi32;
	typedef gaugeminmax<int64_t	>	gaugeminmaxi64;
}

#endif // GPK_GAUGE_H
