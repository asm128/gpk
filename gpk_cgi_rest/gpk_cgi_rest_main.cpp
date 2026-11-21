#include "gpk_cgi.h"
#include "gpk_cgi_module.h"
#include "gpk_string_helper.h"
#include "gpk_process.h"
#include "gpk_json_expression.h"
#include "gpk_parse.h"
#include "gpk_file.h"

#include "gpk_udp_client.h"

static	gpk::err_t	initClient						(::gpk::SUDPClient & bestClient)										{
	bestClient.AddressConnect	= {};
	::gpk::tcpipAddress(9998, 0, ::gpk::TRANSPORT_PROTOCOL_UDP, bestClient.AddressConnect);	// If loading the remote IP from the json fails, we fall back to the local address.
	::gpk::apod<char>			fileJSONConfig					= {};
	::gpk::SJSONReader			jsonConfig						= {};
	{	// Attempt to load config file.
		::gpk::vcst_t					fileNameJSONConfig				= "gpk_config.json";
		if_true_vwf(-1, ::gpk::failed(::gpk::fileToMemory(fileNameJSONConfig, fileJSONConfig)), "Failed to load config JSON file! File not found? File name: %s.", fileNameJSONConfig.begin());
		wf_if(::gpk::jsonParse(jsonConfig, {fileJSONConfig.begin(), fileJSONConfig.size()}), "Failed to read json! Not a valid json file? File name: %s.", fileNameJSONConfig.begin());
	}
	{ // attempt to load address from config file.
		{ //
			::gpk::vcst_t					jsonIP							= {};
			wf_if(::gpk::failed(::gpk::jsonExpressionResolve(::gpk::vcst_t{"application.gpk_cgi_rest.remote_ip"}, jsonConfig, 0, jsonIP)), "Failed to load config from json! Last contents found: %s.", jsonIP.begin())
			else {
				info_printf("Remote IP: %s.", jsonIP.begin());
				if_true_ef(::gpk::failed(::gpk::tcpipAddress(jsonIP, {}, bestClient.AddressConnect)), "Failed to read IP address from JSON config file: %s.", jsonIP.begin());	// turn the string into a SIPv4Endpoint struct.
			}
		}
		{ // load port from config file
			bestClient.AddressConnect.Port	= 9998;
			::gpk::vcst_t					jsonPort							= {};
			wf_if(::gpk::failed(::gpk::jsonExpressionResolve(::gpk::vcst_t{"application.gpk_cgi_rest.remote_port"}, jsonConfig, 0, jsonPort)), "Failed to load config from json! Last contents found: %s.", jsonPort.begin())
			else {
				::gpk::u3_t					port								= 0;
				::gpk::parseIntegerDecimal(jsonPort, port);
				bestClient.AddressConnect.Port	= (::gpk::u1_t)port;
				info_printf("Remote port: %u.", (::gpk::u2_t)port);
			}
		}
	}
	return 0;
}

static	int					cgiBootstrap			(const ::gpk::SCGIRuntimeValues & runtimeValues, ::gpk::apod<char> & output)										{
	::gpk::apod<char>				environmentBlock		= {};
	{	// Prepare CGI environment and request content packet to send to the service.
		if_fail_fe(gpk::environmentBlockFromEnviron(environmentBlock));
		environmentBlock.append(runtimeValues.Content.Body.begin(), runtimeValues.Content.Body.size());
		environmentBlock.push_back(0);
	}
	{	// Connect the client to the service.
		::gpk::SUDPClient				bestClient				= {};
		{	// setup and connect
			bestClient.AddressConnect	= {};
			if_fail_fe(::initClient(bestClient));
			if_fail_fe(gpk::clientConnect(bestClient));
		}
		::gpk::au0_t						responseRemote;
		{	// Send the request data to the connected service.
			if_true_fef(bestClient.State != ::gpk::UDP_CONNECTION_STATE_IDLE, "%s", "Failed to connect to server.");
			if_fail_fe(gpk::connectionPushData(bestClient, bestClient.Queue, {(const ::gpk::u0_t*)environmentBlock.begin(), environmentBlock.size()}, true, true));	// Enqueue the packet
			while(bestClient.State != ::gpk::UDP_CONNECTION_STATE_DISCONNECTED) {	// Loop until we ge the response or the client disconnects
				if_fail_fe(gpk::clientUpdate(bestClient));
				::gpk::apobj<::gpk::SUDPMessage>	received;
				{	// pick up messages for later processing
					::std::lock_guard				lockRecv					(bestClient.Queue.MutexReceive);
					received					= bestClient.Queue.Received;
					bestClient.Queue.Received.clear();
				}
				if(received.size()) {	// Response has been received. Break loop.
					responseRemote				= received[0]->Payload;
					break;
				}
			}
		}
		//info_printf("Remote CGI answer: %s.", responseRemote.begin());
		if_fail_fe(gpk::clientDisconnect(bestClient));
		output					= {(char*)responseRemote.begin(), responseRemote.size()};
	}
	return 0;
}

static	int				cgiMain				(int argc, char** argv, char**envv)	{
	(void)(envv);
	if_fail_fef(gpk::tcpipInitialize(), "%s", "Failed to initialize network subsystem.");
	::gpk::SCGIRuntimeValues	runtimeValues;
	if_fail_fef(gpk::cgiRuntimeValuesLoad(runtimeValues, {(const char**)argv, (::gpk::u2_t)argc}), "%s", "Failed to load cgi runtime values.");
	::gpk::apod<char>			html;
	if(::gpk::failed(::cgiBootstrap(runtimeValues, html))) {
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
		);
		html.push_back('\0');
		printf("%s", html.begin());
#ifdef GPK_WINDOWS
		OutputDebugStringA(html.begin());
#endif
	}
	if_fail_fef(gpk::tcpipShutdown(), "Failed to shut down network subsystem. %s", "Why!?");
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
