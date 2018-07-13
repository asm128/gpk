#ifndef GPK_SAFE_H_209934782093
#define GPK_SAFE_H_209934782093

#ifndef safe_delete
#	define safe_delete(p) if(p) do { delete(p); p = 0; } while(0)
#endif

#ifndef safe_assign
#	define safe_assign(p, val) if(p) do { (*p) = (val); } while(0)
#endif

#ifndef safe_podcpy
#	define safe_podcpy(oldValue, val) if((oldValue) && (val)) do { ::gpk::podcpy((oldValue), (val)); } while(0)
#endif

#ifndef safe_closesocket
#	define safe_closesocket(p) if(p != 0 && p != INVALID_SOCKET) do { closesocket(p); p = INVALID_SOCKET; } while(0)
#endif

#endif // GPK_SAFE_H_209934782093