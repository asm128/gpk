#include "gpk_n3.h"

#ifndef GPK_SLICE_H
#define GPK_SLICE_H

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm T> 
	struct slice {	
		T							Begin, End;		
		
		GPK_DEFAULT_OPERATOR(slice <T>, Begin  == other.Begin  && End   == other.End  ); 

		tplt<tpnm _tOther> 
		inlcxpr	slice<_tOther>		Cast		()		const	noexcept	{ return{(_tOther)Begin , (_tOther)End  }; } 

		inlcxpr	slice<uint8_t>		u0_t			()		const	noexcept	{ return Cast<uint8_t	>(); }
		inlcxpr	slice<uint16_t>		u1_t			()		const	noexcept	{ return Cast<uint16_t	>(); }
		inlcxpr	slice<uint32_t>		u2_t			()		const	noexcept	{ return Cast<uint32_t	>(); }
		inlcxpr	slice<uint64_t>		u3_t			()		const	noexcept	{ return Cast<uint64_t	>(); }
		inlcxpr	slice<int8_t>		s0_t			()		const	noexcept	{ return Cast<int8_t	>(); }
		inlcxpr	slice<int16_t>		s1_t			()		const	noexcept	{ return Cast<int16_t	>(); }
		inlcxpr	slice<int32_t>		s2_t			()		const	noexcept	{ return Cast<int32_t	>(); }
		inlcxpr	slice<int64_t>		s3_t			()		const	noexcept	{ return Cast<int64_t	>(); }
		inlcxpr	slice<float>		f2_t			()		const	noexcept	{ return Cast<float		>(); }
		inlcxpr	slice<double>		f3_t			()		const	noexcept	{ return Cast<double	>(); }
	};
#pragma pack(pop)

	typedef	slice<sc_t		>	slicechar;
	typedef	slice<uc_t	>	sliceuchar;
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
