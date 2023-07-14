#include "gpk_minmax.h"

#ifndef GPK_GAUGE_H
#define GPK_GAUGE_H

namespace gpk
{
#pragma pack(push, 1)
	template<typename T> 
	struct gauge { 
		minmax<T>					Limits;	// Inclusive
		T							Current;	

		GPK_DEFAULT_OPERATOR(minmax<T>, Limits == other.Limits && Current == other.Current); 

		template<typename _tOther>
		gauge<_tOther>				Cast		()		const	noexcept	{ return {(_tOther)Limits.Cast<_tOther>(), (_tOther)Current}; }

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
	typedef	gauge<char		>	gaugechar;
	typedef	gauge<uchar_t	>	gaugeuchar;
	typedef gauge<float		>	gaugef32;
	typedef gauge<double	>	gaugef64;
	typedef gauge<uint8_t	>	gaugeu8;
	typedef gauge<uint16_t	>	gaugeu16;
	typedef gauge<uint32_t	>	gaugeu32;
	typedef gauge<uint64_t	>	gaugeu64;
	typedef gauge<int8_t	>	gaugei8;
	typedef gauge<int16_t	>	gaugei16;
	typedef gauge<int32_t	>	gaugei32;
	typedef gauge<int64_t	>	gaugei64;
}

#endif // GPK_GAUGE_H
