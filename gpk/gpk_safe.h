#include "gpk_platform.h"

#ifndef GPK_SAFE_H_209934782093
#define GPK_SAFE_H_209934782093

#ifndef gpk_safe_free
#	define gpk_safe_free(p) if(p) do { free(p); p = 0; } while(0)
#endif // gpk_safe_free

#ifndef gpk_safe_delete
#	define gpk_safe_delete(p) if(p) do { delete(p); p = 0; } while(0)
#endif // gpk_safe_delete

#ifndef gpk_safe_assign
#	define gpk_safe_assign(p, val) if(p) do { (*p) = (val); } while(0)
#endif // gpk_safe_assign

#ifndef gpk_safe_podcpy
#	define gpk_safe_podcpy(oldValue, val) if((oldValue) && (val)) do { ::gpk::podcpy((oldValue), (val)); } while(0)
#endif // gpk_safe_podcpy

#ifndef gpk_safe_fclose
#	define gpk_safe_fclose(p) if(p) do { fclose(p); p = 0; } while(0)
#endif // gpk_safe_fclose

#ifndef gpk_safe_closesocket
#	if defined(GPK_WINDOWS)
#		define gpk_safe_closesocket(p) if(p != 0 && p != INVALID_SOCKET) do { closesocket(p); p = INVALID_SOCKET; } while(0)
#	else
#		define gpk_safe_closesocket(p) if(p != 0 && p != INVALID_SOCKET) do { ::close(p); p = INVALID_SOCKET; } while(0)
#	endif
#endif

#endif // GPK_SAFE_H_209934782093