#include "gpk_cgi.h"
#include "gpk_cgi_module.h"
#include "gpk_string_helper.h"
#include "gpk_process.h"
#include "gpk_json_expression.h"
#include "gpk_parse.h"
#include "gpk_file.h"
#include "gpk_chrono.h"

#include "gpk_udp_client.h"

static	int					cgiBootstrap			(const ::gpk::SCGIRuntimeValues & runtimeValues, ::gpk::apod<char> & output)										{
	{	// Prepare CGI environment and request content packet to save to disk.
		char filename[1024] = {};
		sprintf_s(filename, "la_betty_criminal_at_%llu.jpg", ::gpk::timeCurrent());
		if_fail_fe(::gpk::fileFromMemory(filename, runtimeValues.Content.Body));
	}
	if_fail_fe(output.append_string("{\"code\":200, \"message\": \"OK\"}"));
	return 0;
}

static	int				cgiMain				(::gpk::SRuntimeValues & runtimeValues)	{
	::gpk::SCGIRuntimeValues	cgiRuntimeValues(runtimeValues);
	if_fail_fe(gpk::cgiRuntimeValuesLoad(cgiRuntimeValues));
	::gpk::apod<char>			html;
	if(::gpk::failed(::cgiBootstrap(cgiRuntimeValues, html))) {
		printf("%s\r\n", "Content-Type: text/html"
			"\r\nCache-Control: no-store"
			"\r\n\r\n"
			"<html><head><title>Internal server error</title></head><body>Failed to process request.</body></html>"
			"\r\n"
			"\r\n"
		);
	}
	else {
		printf("%s\r\n", "Content-Type: application/json"
			"\r\nCache-Control: no-store"
			"\r\n\r\n"
		);
		html.push_back('\0');
		printf("%s", html.begin());
#ifdef GPK_WINDOWS
		OutputDebugStringA(html.begin());
#endif
	}
	return 0;
}

GPK_OS_ENTRY_POINT(::cgiMain);
// https://articulo.mercadolibre.com.ar/MLA-800947708-lora-sx1276-esp32-bt-wifi-kit-desarrollo-con-display-arduino-_JM
