#include "gpk_platform.h"

#ifndef GPK_SAFE_H_209934782093
#define GPK_SAFE_H_209934782093

#ifndef gpk_safe_free
#	define gpk_safe_free(p) do { if(p) { free(p); p = 0; } } while(0)
#endif // gpk_safe_free

#ifndef gpk_safe_delete
#	define gpk_safe_delete(p) do { if(p) { delete(p); p = 0; } } while(0)
#endif // gpk_safe_delete

#ifndef gpk_safe_assign
#	define gpk_safe_assign(p, val) do { if(p) (*p) = (val); } while(0)
#endif // gpk_safe_assign

#ifndef gpk_safe_podcpy
#	define gpk_safe_podcpy(oldValue, val) do { if((oldValue) && (val)) ::gpk::podcpy((oldValue), (val)); } while(0)
#endif // gpk_safe_podcpy

#ifndef gpk_safe_fclose
#	define gpk_safe_fclose(p) do { if(p) { fclose(p); p = 0; } } while(0)
#endif // gpk_safe_fclose

#ifndef gpk_safe_closesocket
#	if defined(GPK_WINDOWS)
#		define gpk_safe_closesocket(p) do { if((p) != 0 && (p) != INVALID_SOCKET) { closesocket(p); p = INVALID_SOCKET; } } while(0)
#	else									 
#		define gpk_safe_closesocket(p) do { if((p) != 0 && (p) != INVALID_SOCKET) { ::close(p); p = INVALID_SOCKET; } } while(0)
#	endif
#endif


#if defined(GPK_WINDOWS)
#	ifndef gpk_safe_closehandle
#		define gpk_safe_closehandle(p) do { if(INVALID_HANDLE_VALUE == (p)) { CloseHandle(p); p = INVALID_HANDLE_VALUE; } } while(0)
#	endif
#endif

#endif // GPK_SAFE_H_209934782093