#include "gpk_array_pod.h"
#include "gpk_range.h"

#ifndef GPK_APOD_RANGE_H
#define GPK_APOD_RANGE_H

namespace gpk
{
	template<typename _tDimension>	
	using	arange	= ::gpk::apod<::gpk::range<_tDimension>>;

	typedef	::gpk::arange<char		>	arangec		, arangechar;
	typedef	::gpk::arange<uchar_t	>	arangeuc	, arangeuchar;
	typedef	::gpk::arange<float		>	arangef32	;
	typedef	::gpk::arange<double	>	arangef64	;
	typedef	::gpk::arange<uint8_t	>	arangeu8	;
	typedef	::gpk::arange<uint16_t	>	arangeu16	;
	typedef	::gpk::arange<uint32_t	>	arangeu32	;
	typedef	::gpk::arange<uint64_t	>	arangeu64	;
	typedef	::gpk::arange<int8_t	>	arangei8	;
	typedef	::gpk::arange<int16_t	>	arangei16	;
	typedef	::gpk::arange<int32_t	>	arangei32	;
	typedef	::gpk::arange<int64_t	>	arangei64	;
}

#endif // GPK_APOD_RANGE_H