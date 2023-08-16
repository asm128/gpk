#include "gpk_typeint.h"

#ifndef GPK_MINMAX_H
#define GPK_MINMAX_H

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm T> 
	struct minmax {
		typedef minmax<T>		TMinMax;

		stincxp	TMinMax			from		(T value)				noexcept			{ return {value, value}; }

		T						Min, Max;

		GPK_DEFAULT_OPERATOR(minmax<T>, Max == other.Max && Min == other.Min);

		inlcxpr	T				Length		()				const	noexcept	{ return T(Max - Min); }
		inlcxpr	T				Middle		()				const	noexcept	{ return Min + Length() / 2; }
		inlcxpr	T				Clamp		(T value)		const	noexcept	{ return ::gpk::clamped(value, Min, Max); }
		T						Weighted	(double weight)	const	noexcept	{ return T(Min + Length() * weight); }
		double					Weight		(T value)		const	noexcept	{ return 1.0 / Length() * (value - Min); }
		double					WeightClamp	(T value)		const	noexcept	{ return Weight(Clamp(value)); }

		tplt<tpnm _tOther>
		inlcxpr	minmax<_tOther>	Cast		()				const	noexcept	{ return{(_tOther)Min, (_tOther)Max}; }

		inlcxpr	minmax<char		>	c		()				const	noexcept	{ return Cast<char		>(); }
		inlcxpr	minmax<uchar_t	>	uc		()				const	noexcept	{ return Cast<uchar_t	>(); }
		inlcxpr	minmax<uint8_t	>	u8		()				const	noexcept	{ return Cast<uint8_t	>(); }
		inlcxpr	minmax<uint16_t	>	u16		()				const	noexcept	{ return Cast<uint16_t	>(); }
		inlcxpr	minmax<uint32_t	>	u32		()				const	noexcept	{ return Cast<uint32_t	>(); }
		inlcxpr	minmax<uint64_t	>	u64		()				const	noexcept	{ return Cast<uint64_t	>(); }
		inlcxpr	minmax<int8_t	>	i8		()				const	noexcept	{ return Cast<int8_t	>(); }
		inlcxpr	minmax<int16_t	>	i16		()				const	noexcept	{ return Cast<int16_t	>(); }
		inlcxpr	minmax<int32_t	>	i32		()				const	noexcept	{ return Cast<int32_t	>(); }
		inlcxpr	minmax<int64_t	>	i64		()				const	noexcept	{ return Cast<int64_t	>(); }
		inlcxpr	minmax<float	>	f32		()				const	noexcept	{ return Cast<float		>(); }
		inlcxpr	minmax<double	>	f64		()				const	noexcept	{ return Cast<double	>(); }

		inline	TMinMax&		Set			(T value)				noexcept	{ Min = Max = value; return *this; }
		inline	TMinMax&		From		(T value)				noexcept	{ Min = Max = value; return *this; }
	};
#pragma pack(pop)
	typedef	minmax<char		>	minmaxchar, minmaxc;
	typedef	minmax<uchar_t	>	minmaxuchar, minmaxuc;
	typedef minmax<float	>	minmaxf32;
	typedef minmax<double	>	minmaxf64;
	typedef minmax<uint8_t	>	minmaxu8;
	typedef minmax<uint16_t	>	minmaxu16;
	typedef minmax<uint32_t	>	minmaxu32;
	typedef minmax<uint64_t	>	minmaxu64;
	typedef minmax<int8_t	>	minmaxi8;
	typedef minmax<int16_t	>	minmaxi16;
	typedef minmax<int32_t	>	minmaxi32;
	typedef minmax<int64_t	>	minmaxi64;
}

#endif // GPK_MINMAX_H
