#include "gpk_array_pod.h"
#include "gpk_slice.h"

#ifndef GPK_APOD_SLICE_H
#define GPK_APOD_SLICE_H

namespace gpk
{
	template<typename _tDimension>	
	using	aslice	= ::gpk::apod<::gpk::slice<_tDimension>>;

	typedef	::gpk::aslice<char		>	aslicec		, aslicechar;
	typedef	::gpk::aslice<uchar_t	>	asliceuc	, asliceuchar;
	typedef	::gpk::aslice<float		>	aslicef32	;
	typedef	::gpk::aslice<double	>	aslicef64	;
	typedef	::gpk::aslice<uint8_t	>	asliceu8	;
	typedef	::gpk::aslice<uint16_t	>	asliceu16	;
	typedef	::gpk::aslice<uint32_t	>	asliceu32	;
	typedef	::gpk::aslice<uint64_t	>	asliceu64	;
	typedef	::gpk::aslice<int8_t	>	aslicei8	;
	typedef	::gpk::aslice<int16_t	>	aslicei16	;
	typedef	::gpk::aslice<int32_t	>	aslicei32	;
	typedef	::gpk::aslice<int64_t	>	aslicei64	;
}

#endif // GPK_APOD_SLICE_H