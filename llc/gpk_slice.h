#include "gpk_n3.h"

#ifndef GPK_SLICE_H
#define GPK_SLICE_H

namespace gpk
{
#pragma pack(push, 1)
	template<typename T> 
	struct slice {	
		T							Begin, End;		
		
		GPK_DEFAULT_OPERATOR(slice <T>, Begin  == other.Begin  && End   == other.End  ); 

		template<typename _tOther> 
		inlcxpr	slice<_tOther>		Cast		()		const	noexcept	{ return{(_tOther)Begin , (_tOther)End  }; } 

		inlcxpr	slice<uint8_t>		u8			()		const	noexcept	{ return Cast<uint8_t	>(); }
		inlcxpr	slice<uint16_t>		u16			()		const	noexcept	{ return Cast<uint16_t	>(); }
		inlcxpr	slice<uint32_t>		u32			()		const	noexcept	{ return Cast<uint32_t	>(); }
		inlcxpr	slice<uint64_t>		u64			()		const	noexcept	{ return Cast<uint64_t	>(); }
		inlcxpr	slice<int8_t>		i8			()		const	noexcept	{ return Cast<int8_t	>(); }
		inlcxpr	slice<int16_t>		i16			()		const	noexcept	{ return Cast<int16_t	>(); }
		inlcxpr	slice<int32_t>		i32			()		const	noexcept	{ return Cast<int32_t	>(); }
		inlcxpr	slice<int64_t>		i64			()		const	noexcept	{ return Cast<int64_t	>(); }
		inlcxpr	slice<float>		f32			()		const	noexcept	{ return Cast<float		>(); }
		inlcxpr	slice<double>		f64			()		const	noexcept	{ return Cast<double	>(); }
	};
#pragma pack(pop)

	typedef	slice<char		>	slicechar;
	typedef	slice<uchar_t	>	sliceuchar;
	typedef slice<float		>	slicef32;
	typedef slice<double	>	slicef64;
	typedef slice<uint8_t	>	sliceu8;
	typedef slice<uint16_t	>	sliceu16;
	typedef slice<uint32_t	>	sliceu32;
	typedef slice<uint64_t	>	sliceu64;
	typedef slice<int8_t	>	slicei8;
	typedef slice<int16_t	>	slicei16;
	typedef slice<int32_t	>	slicei32;
	typedef slice<int64_t	>	slicei64;

}

#endif // GPK_SLICE_H
