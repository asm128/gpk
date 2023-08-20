#include "gpk_queue_async.h"
#include "gpk_event.h"

#ifndef GPK_QUEUE_EVENT_H
#define GPK_QUEUE_EVENT_H

namespace gpk
{
	tplt<tpnm _tEventType> using qevt			= ::gpk::qobj <::gpk::SEvent<_tEventType>>;
    tplt<tpnm _tEventType> using qpevt			= ::gpk::qpobj<::gpk::SEvent<_tEventType>>;

	tplt<tpnm _tEventType> using queue_event	= ::gpk::qevt<_tEventType>;
    tplt<tpnm _tEventType> using queue_pevent	= ::gpk::qpevt<_tEventType>;
}

#endif // GPK_QUEUE_EVENT_H
