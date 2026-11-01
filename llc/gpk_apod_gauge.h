#include "gpk_array_pod.h"
#include "gpk_gauge.h"

#ifndef GPK_APOD_GAUGE_H
#define GPK_APOD_GAUGE_H

namespace gpk
{
	tplt<tpnm _tDimension>	
	using	agauge	= ::gpk::apod<::gpk::gauge<_tDimension>>;

	typedef	::gpk::agauge<sc_t		>	agaugec		, agaugechar;
	typedef	::gpk::agauge<uc_t	>	agaugeuc	, agaugeuchar;
	typedef	::gpk::agauge<f2_t		>	agaugef2_t	;
	typedef	::gpk::agauge<f3_t	>	agaugef64	;
	typedef	::gpk::agauge<u0_t	>	agaugeu8	;
	typedef	::gpk::agauge<u1_t	>	agaugeu1_t	;
	typedef	::gpk::agauge<u2_t	>	agaugeu32	;
	typedef	::gpk::agauge<u3_t	>	agaugeu64	;
	typedef	::gpk::agauge<s0_t	>	agaugei8	;
	typedef	::gpk::agauge<s1_t	>	agaugei16	;
	typedef	::gpk::agauge<s2_t	>	agaugei32	;
	typedef	::gpk::agauge<s3_t	>	agaugei64	;
}

#endif // GPK_APOD_GAUGE_H