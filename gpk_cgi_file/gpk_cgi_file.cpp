#include "gpk_cgi.h"
#include "gpk_cgi_module.h"
#include "gpk_string_helper.h"
#include "gpk_process.h"
#include "gpk_json_expression.h"
#include "gpk_parse.h"
#include "gpk_file.h"

#include "gpk_udp_client.h"

static	int					cgiBootstrap			(const ::gpk::SCGIRuntimeValues & runtimeValues, ::gpk::apod<char> & output)										{
	::gpk::apod<char>				environmentBlock		= {};
	{	// Prepare CGI environment and request content packet to save to disk.
		gpk_necs(gpk::environmentBlockFromEnviron(environmentBlock));
		char filename[1024] = {};
		sprintf_s(filename, "la_betty_criminal_at_%llu.jpg", ::gpk::timeCurrent());
		gpk_necs(::gpk::fileFromMemory(filename, runtimeValues.Content.Body));
	}
	gpk_necs(output.append_string("{\"code\":200, \"message\": \"OK\"}"));
	return 0;
}

static	int				cgiMain				(int argc, char** argv, char**envv)	{
	(void)(envv);
	::gpk::SCGIRuntimeValues	runtimeValues;
	gpk_necall(gpk::cgiRuntimeValuesLoad(runtimeValues, {(const char**)argv, (uint32_t)argc}), "%s", "Failed to load cgi runtime values.");
	::gpk::apod<char>			html;
	if errored(::cgiBootstrap(runtimeValues, html)) {
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

int				main				(int argc, char** argv, char**envv)	{
	return ::cgiMain(argc, argv, envv);
}

#ifdef GPK_WINDOWS
#include <Windows.h>

int WINAPI		WinMain
	(	_In_		HINSTANCE	hInstance
	,	_In_opt_	HINSTANCE	hPrevInstance
	,	_In_		LPSTR		lpCmdLine
	,	_In_		int			nShowCmd
	) {
	(void)hInstance, (void)hPrevInstance, (void)lpCmdLine, (void)nShowCmd;
	return ::cgiMain(__argc, __argv, environ);
}
#endif
// https://articulo.mercadolibre.com.ar/MLA-800947708-lora-sx1276-esp32-bt-wifi-kit-desarrollo-con-display-arduino-_JM
