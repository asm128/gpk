#include "gpk_cgi_runtime.h"

::gpk::error_t											gpk_cgi_generate_output					(::gpk::SCGIRuntimeValues & runtimeValues, ::gpk::array_pod<char_t> & output);
::gpk::error_t											gpk::cgiMain							(int argc, char** argv, char**envv)	{
		(void)(envv);
		::gpk::SCGIRuntimeValues								runtimeValues;
		::gpk::cgiRuntimeValuesLoad(runtimeValues, {(const char**)argv, (uint32_t)argc});
		::gpk::array_pod<char_t>								output;
		::gpk_cgi_generate_output(runtimeValues, output);
		output.push_back('\0');
		printf("%s", output.begin());
		return 0;
}
