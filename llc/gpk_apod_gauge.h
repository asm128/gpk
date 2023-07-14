#include "gpk_array_pod.h"
#include "gpk_gauge.h"

#ifndef GPK_APOD_GAUGE_H
#define GPK_APOD_GAUGE_H

namespace gpk
{
	template<typename _tDimension>	
	using	agauge	= ::gpk::apod<::gpk::gauge<_tDimension>>;

	typedef	::gpk::agauge<char		>	agaugec		, agaugechar;
	typedef	::gpk::agauge<uchar_t	>	agaugeuc	, agaugeuchar;
	typedef	::gpk::agauge<float		>	agaugef32	;
	typedef	::gpk::agauge<double	>	agaugef64	;
	typedef	::gpk::agauge<uint8_t	>	agaugeu8	;
	typedef	::gpk::agauge<uint16_t	>	agaugeu16	;
	typedef	::gpk::agauge<uint32_t	>	agaugeu32	;
	typedef	::gpk::agauge<uint64_t	>	agaugeu64	;
	typedef	::gpk::agauge<int8_t	>	agaugei8	;
	typedef	::gpk::agauge<int16_t	>	agaugei16	;
	typedef	::gpk::agauge<int32_t	>	agaugei32	;
	typedef	::gpk::agauge<int64_t	>	agaugei64	;
}

#endif // GPK_APOD_GAUGE_H