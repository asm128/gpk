#include "gpk_sync.h"
#include "gpk_log.h"
#include "gpk_memory.h"

#ifndef GPK_REF_H_23627
#define GPK_REF_H_23627

namespace gpk
{
	tplTstct gref {
		T					* Instance			= {};
		refcount_vl			References			= {};
	};

	tplT	gref<T>*	ref_acquire				(gref<T> * gpk_reference)	nxpt	{ if(gpk_reference) gpk_sync_increment(gpk_reference->References); return gpk_reference; }
	tplT	err_t		ref_release				(gref<T>* * gpk_reference)			{
		tydf	gref<T>			TRef;
		TRef					* oldRef				= *gpk_reference;
		*gpk_reference		= 0;
		if(oldRef)
			switch(refcount_c referenceCount = gpk_sync_decrement(oldRef->References)) {
			default: if_fail_fef(referenceCount, "Reference count error! (%i).", referenceCount); break;
			case  0:
				if_null_e(oldRef->Instance)
				else {
					oldRef->Instance->~T(); 
					gpk_free(oldRef->Instance);
				}
				gpk_free(oldRef);
				break;
			}
		return 0;
	}
	tplT	T*			ref_allocate			(gref<T>* * gpk_reference)	nxpt	{
		tydf	gref<T>			TRef;
		TRef					* newRef;
		if_null_ve(0, newRef = malloc(newRef));
		if_true_block_log(error_printf, 0 == (newRef->References = one_if(malloc(newRef->Instance))), gpk_free(newRef); return 0;);
		TRef					* oldRef				= *gpk_reference;
		*gpk_reference		= newRef;
		ref_release(&oldRef);
		return (*gpk_reference)->Instance;
	}
	tplT_vtArgs	T*		ref_create				(gref<T>* * gpk_reference, _tArgs&&... argsConstructor)	{
		tydf	gref<T>			TRef;
		TRef					* newRef				= 0;
		if_null_ve(0, ref_allocate(&newRef));
		new (newRef->Instance) T{argsConstructor...};
		TRef					* oldRef				= *gpk_reference;
		*gpk_reference		= newRef;
		ref_release(&oldRef);
		return (*gpk_reference)->Instance;
	}
} // namespace

#endif // GPK_REF_H_23627
