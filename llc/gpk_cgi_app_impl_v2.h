#include "gpk_cgi_runtime.h"

#ifndef GPK_CGI_APP_IMPL_V2_H_23627
#define GPK_CGI_APP_IMPL_V2_H_23627

#ifdef GPK_WINDOWS
#	ifndef GPK_CGI_CUSTOM_SYSTEM_ENTRY_POINT
#		define GPK_CGI_CUSTOM_SYSTEM_ENTRY_POINT()	\
int WINAPI	WinMain									\
	(	_In_		HINSTANCE	hInstance			\
	,	_In_opt_	HINSTANCE	hPrevInstance		\
	,	_In_		LPSTR		lpCmdLine			\
	,	_In_		int			nShowCmd			\
	) {												\
	(void)hInstance, (void)hPrevInstance, (void)lpCmdLine, (void)nShowCmd;		\
	return ::gpk::cgiMain(__argc, __argv, environ);	\
}
#	endif
#else
#	ifndef GPK_CGI_CUSTOM_SYSTEM_ENTRY_POINT
#		define GPK_CGI_CUSTOM_SYSTEM_ENTRY_POINT()
#	endif
#endif

#ifndef GPK_CGI_JSON_APP_IMPL
#	define GPK_CGI_JSON_APP_IMPL()							\
int			main	(int argc, sc_t** argv, sc_t**envv)	{	\
	return ::gpk::cgiMain(argc, argv, envv);				\
}															\
GPK_CGI_CUSTOM_SYSTEM_ENTRY_POINT();
#endif

#endif // GPK_CGI_APP_IMPL_V2_H_23627
