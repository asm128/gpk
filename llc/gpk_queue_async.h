#include "gpk_array_pod.h"
#include "gpk_array_obj.h"
#include "gpk_ptr_pod.h"
#include "gpk_ptr_obj.h"

#include "gpk_mutex.h"

#ifndef GPK_QUEUE_ASYNC_H
#define GPK_QUEUE_ASYNC_H

nmsp gpk
{
	tplt<tpnm _t, tpnm _tContainer>
	struct queue_async {
		tydf	_t	 		T;
		tydf	_tContainer	TContainer;

		TContainer				Queue	= {};
		::gpk::mutex			Lock	= {};

		inln	err_t 	Size	()						{ ::gpk::lock_guard locked(Lock); return Queue.size(); }
		inln	err_t	Clear	()						{ ::gpk::lock_guard	locked(Lock); return Queue.clear(); }
		inln	err_t	Post	(const T & item) 		{ ::gpk::lock_guard locked(Lock); return Queue.push_back(item); }
		// Appends to the `output` container all the messages from the queue
		inln	err_t	Read	(TContainer & output, bool clearQueue = true) {
			::gpk::lock_guard		locked(Lock);
			if_fail_fe(output.append(Queue));
			if(clearQueue) {
				Queue.clear();
			}
			return 0;
		}
		// Returns 1 + the amount of messages remaining in the queue
		err_t			Next	(T & output) {
			err_t	left;
			{
				lock_guard	locked(Lock);
				if_true_vi(0, 0 == Queue.size());
				output	= Queue[0];
				if_fail_fe(left = Queue.remove(0));
			}
			return 1 + left;
		}
	};

	tplt<tpnm _tObj> usng qobj			= ::gpk::queue_async<_tObj, ::gpk::aobj<_tObj>>;
	tplt<tpnm _tPOD> usng qpod			= ::gpk::queue_async<_tPOD, ::gpk::apod<_tPOD>>;
	tplt<tpnm _tObj> usng qpobj		= ::gpk::qobj<::gpk::pobj<_tObj>>;
	tplt<tpnm _tPOD> usng qppod		= ::gpk::qobj<::gpk::ppod<_tPOD>>;

	tplt<tpnm _tObj> usng queue_obj	= ::gpk::qobj <_tObj>;
	tplt<tpnm _tPOD> usng queue_pod	= ::gpk::qpod <_tPOD>;
	tplt<tpnm _tObj> usng queue_pobj	= ::gpk::qpobj<_tObj>;
	tplt<tpnm _tPOD> usng queue_ppod	= ::gpk::qppod<_tPOD>;

} // nmsp

#endif // GPK_QUEUE_ASYNC_H
