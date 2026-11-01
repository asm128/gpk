#include "gpk_error.h"

#ifndef GPK_SAFE_H_23627
#define GPK_SAFE_H_23627

namespace gpk 
{
	tplT	err_t	gpk_safe_assign			(T* & handle, const T & value)	{ if(0			== handle) return -1; *handle = value; return 0; }
	tplT	err_t	gpk_safe_free			(T* & handle)					{ if(0			== handle) return -1; ::free		(handle); handle = {}; return 0; }
	tplT	err_t	gpk_safe_fclose			(T  & handle)					{ if(0			== handle) return -1; ::fclose		(handle); handle = {}; return 0; }
#ifdef GPK_WINDOWS
	tplT	err_t	gpk_safe_closehandle	(T  & handle)					{ if(T(-1LL)	== handle) return -1; ::CloseHandle	(handle); handle = T(-1LL); return 0; }
#endif
} // namespace 

#endif // GPK_SAFE_H_23627
