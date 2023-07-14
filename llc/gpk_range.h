#include "gpk_typeint.h"

#ifndef GPK_RANGE_H
#define GPK_RANGE_H

namespace gpk
{
#pragma pack(push, 1)
	template<typename T> 
	struct range {	
		T							Offset, Count;	
		
		GPK_DEFAULT_OPERATOR(range <T>, Offset == other.Offset && Count == other.Count); 
		template<typename _tOther> 
		inlcxpr	range<_tOther>		Cast		()		const	noexcept	{ return{(_tOther)Offset, (_tOther)Count}; } 

		inlcxpr	range<uint8_t>		u8			()		const	noexcept	{ return Cast<uint8_t	>(); }
		inlcxpr	range<uint16_t>		u16			()		const	noexcept	{ return Cast<uint16_t	>(); }
		inlcxpr	range<uint32_t>		u32			()		const	noexcept	{ return Cast<uint32_t	>(); }
		inlcxpr	range<uint64_t>		u64			()		const	noexcept	{ return Cast<uint64_t	>(); }
		inlcxpr	range<int8_t>		i8			()		const	noexcept	{ return Cast<int8_t	>(); }
		inlcxpr	range<int16_t>		i16			()		const	noexcept	{ return Cast<int16_t	>(); }
		inlcxpr	range<int32_t>		i32			()		const	noexcept	{ return Cast<int32_t	>(); }
		inlcxpr	range<int64_t>		i64			()		const	noexcept	{ return Cast<int64_t	>(); }
		inlcxpr	range<float>		f32			()		const	noexcept	{ return Cast<float		>(); }
		inlcxpr	range<double>		f64			()		const	noexcept	{ return Cast<double	>(); }
	};
#pragma pack(pop)

	typedef	range<char		>	rangechar;
	typedef	range<uchar_t	>	rangeuchar;
	typedef range<float		>	rangef32;
	typedef range<double	>	rangef64;
	typedef range<uint8_t	>	rangeu8;
	typedef range<uint16_t	>	rangeu16;
	typedef range<uint32_t	>	rangeu32;
	typedef range<uint64_t	>	rangeu64;
	typedef range<int8_t	>	rangei8;
	typedef range<int16_t	>	rangei16;
	typedef range<int32_t	>	rangei32;
	typedef range<int64_t	>	rangei64;
}

#endif // GPK_RANGE_H
