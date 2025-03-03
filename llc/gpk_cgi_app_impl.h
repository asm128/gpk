#include "gpk_cgi_runtime.h"

#ifndef GPK_CGI_APP_IMPL_H_23627
#define GPK_CGI_APP_IMPL_H_23627

#ifdef GPK_WINDOWS
	#define GPK_CGI_CUSTOM_SYSTEM_ENTRY_POINT()				\
		int WINAPI			WinMain								\
			(	_In_			HINSTANCE	/*hInstance		*/	\
			,	_In_opt_		HINSTANCE	/*hPrevInstance	*/	\
			,	_In_			LPSTR		/*lpCmdLine		*/	\
			,	_In_			int			/*nShowCmd		*/	\
			) {												\
			return ::cgiMain(__argc, __argv, environ);		\
		}
#else
	#define GPK_CGI_CUSTOM_SYSTEM_ENTRY_POINT()
#endif

#define GPK_CGI_JSON_APP_IMPL()																\
	::gpk::error_t			generate_output		(::gpk::SCGIRuntimeValues & runtimeValues, ::gpk::apod<sc_t> & output);			\
	::gpk::error_t			cgiMain				(int argc, sc_t** argv, sc_t**envv)		{										\
		(void)(envv);																		\
		::gpk::SCGIRuntimeValues	runtimeValues;				\
		::gpk::cgiRuntimeValuesLoad(runtimeValues, {(const sc_t**)argv, (uint32_t)argc});			\
		::gpk::apod<sc_t>			output;						\
		output.append_string("Content-type: application/json\r\n");							\
		::generate_output(runtimeValues, output);											\
		output.push_back('\0');																\
		printf("%s", output.begin());														\
		return 0;																			\
	}					\
						\
	int						main				(int argc, sc_t** argv, sc_t**envv)		{ return ::cgiMain(argc, argv, envv); }	\
	GPK_CGI_CUSTOM_SYSTEM_ENTRY_POINT();

#endif // GPK_CGI_APP_IMPL_H_23627
