#include "gpk_cgi_runtime.h"

using gpk::sc_t;

::gpk::error_t			gpk_cgi_generate_output					(::gpk::SCGIRuntimeValues & runtimeValues, ::gpk::apod<sc_t> & output);
::gpk::error_t			gpk::cgiMain							(int argc, sc_t** argv, sc_t**envv)	{
		(void)(envv);
		::gpk::SCGIRuntimeValues								runtimeValues;
		::gpk::cgiRuntimeValuesLoad(runtimeValues, {(const sc_t**)argv, (uint32_t)argc});
		::gpk::apod<sc_t>								output;
		::gpk_cgi_generate_output(runtimeValues, output);
		output.push_back('\0');
		printf("%s", output.begin());
		return 0;
}
