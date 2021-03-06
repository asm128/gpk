#include "gpk_cgi_runtime.h"

#ifndef GPK_CGI_APP_IMPL_H_92836492873648723
#define GPK_CGI_APP_IMPL_H_92836492873648723

#ifdef GPK_WINDOWS
	#define GPK_CGI_CUSTOM_SYSTEM_ENTRY_POINT()					\
		int WINAPI					WinMain												\
			(	_In_		HINSTANCE	hInstance										\
			,	_In_opt_	HINSTANCE	hPrevInstance									\
			,	_In_		LPSTR		lpCmdLine										\
			,	_In_		int			nShowCmd										\
			) {																			\
			(void)hInstance, (void)hPrevInstance, (void)lpCmdLine, (void)nShowCmd;		\
			return ::gpk::cgiMain(__argc, __argv, environ);	\
		}
#else
	#define GPK_CGI_CUSTOM_SYSTEM_ENTRY_POINT()
#endif

#define GPK_CGI_JSON_APP_IMPL()																																					\
	int								main				(int argc, char** argv, char**envv)		{ return ::gpk::cgiMain(argc, argv, envv); }	\
	GPK_CGI_CUSTOM_SYSTEM_ENTRY_POINT();

#endif // GPK_CGI_APP_IMPL_H_92836492873648723
