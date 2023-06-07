#include "gpk_sync.h"
#include "gpk_log.h"
#include "gpk_memory.h"

#ifndef GPK_REF_H_0297349827220523
#define GPK_REF_H_0297349827220523

namespace gpk
{
	template<typename _tInstance>
	struct gref {
		typedef	_tInstance		TInstance;

		_tInstance				* Instance;
		refcount_t				References;
	};

	template<typename _tNCO>
	::gpk::gref<_tNCO>*		ref_acquire				(::gpk::gref<_tNCO> * gpk_reference)	noexcept	{
		if(gpk_reference)
			gpk_sync_increment(gpk_reference->References);
		return gpk_reference;
	};

	template<typename _tNCO>
	::gpk::error_t			ref_release				(::gpk::gref<_tNCO>* * gpk_reference) {
		typedef	::gpk::gref<_tNCO>	TRef;
		TRef						* oldRef				= *gpk_reference;
		*gpk_reference			= 0;
		if(oldRef)
			switch(gpk_sync_decrement(oldRef->References)) {
			case -1: error_printf("%s", "Reference count error!"); return -1;
			case  0:
				if(oldRef->Instance)
					oldRef->Instance->~_tNCO();
				else
					error_printf("%s", "Instance is NULL! At the point of writing this code it wouldn't make any sense.");
				::gpk::gpk_free(oldRef->Instance);
				::gpk::gpk_free(oldRef);
				break;
			}
		return 0;
	};

	template<typename _tOBJ>
	_tOBJ*					ref_allocate			(::gpk::gref<_tOBJ>* * gpk_reference)	noexcept	{
		typedef	::gpk::gref<_tOBJ>	TRef;
		TRef						* newRef				= (TRef *)::gpk::gpk_malloc(sizeof(TRef));
		retnul_gerror_if(0 == newRef, "%s", "Failed to allocate reference! Out of memory?");
		if(0 == (newRef->Instance = (_tOBJ*)::gpk::gpk_malloc(sizeof(_tOBJ)))) {
			::gpk::gpk_free(newRef);
			error_printf("%s", "Failed to allocate instance! Out of memory?");
			return 0;
		}
		newRef->References		= 1;
		TRef						* oldRef				= *gpk_reference;
		*gpk_reference			= newRef;
		::gpk::ref_release(&oldRef);
		return (*gpk_reference)->Instance;
	};

	template<typename _tOBJ, typename... _tArgs>
	_tOBJ*					ref_create				(::gpk::gref<_tOBJ>* * gpk_reference, _tArgs&&... argsConstructor)	{
		typedef	::gpk::gref<_tOBJ>	TRef;
		TRef						* newRef				= 0;
		retnul_gerror_if(0 == ::gpk::ref_allocate(&newRef), "%s", "Failed to allocate reference");
		new (newRef->Instance) _tOBJ{argsConstructor...};

		TRef						* oldRef				= *gpk_reference;
		*gpk_reference			= newRef;
		::gpk::ref_release(&oldRef);
		return (*gpk_reference)->Instance;
	};
} // namespace

#endif // GPK_REF_H_0297349827220523
