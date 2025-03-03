#include "gpk_typeint.h"

#ifndef GPK_RANGE_H
#define GPK_RANGE_H

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm T> 
	struct range {	
		T							Offset, Count;	
		
		GPK_DEFAULT_OPERATOR(range <T>, Offset == other.Offset && Count == other.Count); 
		tplt<tpnm _tOther> 
		inlcxpr	range<_tOther>		Cast		()		const	noexcept	{ return{(_tOther)Offset, (_tOther)Count}; } 

		inlcxpr	range<uint8_t>		u0_t			()		const	noexcept	{ return Cast<uint8_t	>(); }
		inlcxpr	range<uint16_t>		u1_t			()		const	noexcept	{ return Cast<uint16_t	>(); }
		inlcxpr	range<uint32_t>		u2_t			()		const	noexcept	{ return Cast<uint32_t	>(); }
		inlcxpr	range<uint64_t>		u3_t			()		const	noexcept	{ return Cast<uint64_t	>(); }
		inlcxpr	range<int8_t>		s0_t			()		const	noexcept	{ return Cast<int8_t	>(); }
		inlcxpr	range<int16_t>		s1_t			()		const	noexcept	{ return Cast<int16_t	>(); }
		inlcxpr	range<int32_t>		s2_t			()		const	noexcept	{ return Cast<int32_t	>(); }
		inlcxpr	range<int64_t>		s3_t			()		const	noexcept	{ return Cast<int64_t	>(); }
		inlcxpr	range<float>		f2_t			()		const	noexcept	{ return Cast<float		>(); }
		inlcxpr	range<double>		f3_t			()		const	noexcept	{ return Cast<double	>(); }
	};
#pragma pack(pop)

	typedef	range<sc_t		>	rangechar;
	typedef	range<uc_t	>	rangeuchar;
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
