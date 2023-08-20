#include "gpk_array_pod.h"
#include "gpk_array_obj.h"
#include "gpk_ptr_pod.h"
#include "gpk_ptr_obj.h"

#include <mutex>

#ifndef GPK_QUEUE_ASYNC_H
#define GPK_QUEUE_ASYNC_H

namespace gpk
{
	tplt<tpnm _t, tpnm _tContainer>
	struct queue_async {
		typedef	_t	 		T;
		typedef	_tContainer	TContainer;

		TContainer				Queue	= {};
		::std::mutex			Lock	= {};

		inline	::gpk::error_t 	Size	()						{ ::std::lock_guard locked(Lock); return Queue.size(); }
		inline	::gpk::error_t	Clear	()						{ ::std::lock_guard	locked(Lock); return Queue.clear(); }
		inline	::gpk::error_t	Post	(const T & item) 		{ ::std::lock_guard locked(Lock); return Queue.push_back(item); }
		// Appends to the `output` container all the messages from the queue
		inline	::gpk::error_t	Read	(TContainer & output, bool clearQueue = true) {
			::std::lock_guard		locked(Lock);
			gpk_necs(output.append(Queue));
			if(clearQueue) {
				Queue.clear();
			}
			return 0;
		}
		// Returns 1 + the amount of messages remaining in the queue
		::gpk::error_t			Next	(T & output) {
			::gpk::error_t	left;
			{
				::std::lock_guard	locked(Lock);
				rnis_if(0 == Queue.size());
				output	= Queue[0];
				gpk_necs(left = Queue.remove(0));
			}
			return 1 + left;
		}
	};

	tplt<tpnm _tObj> using qobj			= ::gpk::queue_async<_tObj, ::gpk::aobj<_tObj>>;
	tplt<tpnm _tPOD> using qpod			= ::gpk::queue_async<_tPOD, ::gpk::apod<_tPOD>>;
	tplt<tpnm _tObj> using qpobj		= ::gpk::qobj<::gpk::pobj<_tObj>>;
	tplt<tpnm _tPOD> using qppod		= ::gpk::qobj<::gpk::ppod<_tPOD>>;

	tplt<tpnm _tObj> using queue_obj	= ::gpk::qobj <_tObj>;
	tplt<tpnm _tPOD> using queue_pod	= ::gpk::qpod <_tPOD>;
	tplt<tpnm _tObj> using queue_pobj	= ::gpk::qpobj<_tObj>;
	tplt<tpnm _tPOD> using queue_ppod	= ::gpk::qppod<_tPOD>;

} // namespace

#endif // GPK_QUEUE_ASYNC_H
