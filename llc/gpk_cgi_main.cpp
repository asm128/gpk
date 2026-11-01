#include "gpk_cgi_runtime.h"

GPK_USING_TYPEINT();
GPK_USING_VIEW();
GPK_USING_APOD();
GPK_USING_ASTATIC();
GPK_USING_N2();

::gpk::err_t			gpk_cgi_generate_output					(::gpk::SCGIRuntimeValues & runtimeValues, ::gpk::string & output);
::gpk::err_t			gpk::cgiMain							(::gpk::SRuntimeValues & runtimeValues)	{
		SCGIRuntimeValues		cgiRuntimeValues(runtimeValues);
		cgiRuntimeValuesLoad(cgiRuntimeValues);
		string					output;
		::gpk_cgi_generate_output(cgiRuntimeValues, output);
		fprintf(stdout, "%.*s", GPK_VIEW_ARGS(output));
		return 0;
}
