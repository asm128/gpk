#include "gpk_array_pod.h"
#include "gpk_minmax.h"

#ifndef GPK_APOD_MINMAX_H
#define GPK_APOD_MINMAX_H

namespace gpk
{
	template<typename _tDimension>	
	using	aminmax	= ::gpk::apod<::gpk::minmax<_tDimension>>;

	typedef	::gpk::aminmax<char		>	aminmaxc	, aminmaxchar;
	typedef	::gpk::aminmax<uchar_t	>	aminmaxuc	, aminmaxuchar;
	typedef	::gpk::aminmax<float	>	aminmaxf32	;
	typedef	::gpk::aminmax<double	>	aminmaxf64	;
	typedef	::gpk::aminmax<uint8_t	>	aminmaxu8	;
	typedef	::gpk::aminmax<uint16_t	>	aminmaxu16	;
	typedef	::gpk::aminmax<uint32_t	>	aminmaxu32	;
	typedef	::gpk::aminmax<uint64_t	>	aminmaxu64	;
	typedef	::gpk::aminmax<int8_t	>	aminmaxi8	;
	typedef	::gpk::aminmax<int16_t	>	aminmaxi16	;
	typedef	::gpk::aminmax<int32_t	>	aminmaxi32	;
	typedef	::gpk::aminmax<int64_t	>	aminmaxi64	;
}

#endif // GPK_APOD_MINMAX_H