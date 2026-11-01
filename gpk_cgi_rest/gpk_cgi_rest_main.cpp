#include "gpk_cgi.h"
#include "gpk_cgi_module.h"
#include "gpk_string_helper.h"
#include "gpk_process.h"
#include "gpk_json_expression.h"
#include "gpk_parse.h"
#include "gpk_file.h"

#include "gpk_udp_client.h"

sttc	gpk::err_t	initClient			(gpk::SUDPClient & bestClient)										{
	bestClient.AddressConnect	= {};
	::gpk::tcpipAddress(9998, 0, ::gpk::TRANSPORT_PROTOCOL_UDP, bestClient.AddressConnect);	// If loading the remote IP from the json fails, we fall back to the local address.
	::gpk::string				fileJSONConfig					= {};
	::gpk::SJSONReader			jsonConfig						= {};
	{	// Attempt to load config file.
		::gpk::vcst_t					fileNameJSONConfig				= "gpk_config.json";
		if_fail_fw(::gpk::fileToMemory(fileNameJSONConfig, fileJSONConfig));
		if_fail_w(::gpk::jsonParse(jsonConfig, fileJSONConfig));
	}
	{ // attempt to load address from config file.
		{ //
			::gpk::vcst_t					jsonIP							= {};
			if_fail_w(::gpk::jsonExpressionResolve(::gpk::vcst_t{"application.gpk_cgi_rest.remote_ip"}, jsonConfig, 0, jsonIP))
			else {
				info_printf("Remote IP: %s.", jsonIP.begin());
				if_fail_e(::gpk::tcpipAddress(jsonIP, {}, bestClient.AddressConnect));	// turn the string into a SIPv4Endpoint struct.
			}
		}
		{ // load port from config file
			bestClient.AddressConnect.Port	= 9998;
			::gpk::vcst_t					jsonPort							= {};
			if_fail_w(::gpk::jsonExpressionResolve("application.gpk_cgi_rest.remote_port", jsonConfig, 0, jsonPort))
			else {
				uint64_t					port								= 0;
				::gpk::parseIntegerDecimal(jsonPort, port);
				bestClient.AddressConnect.Port	= (uint16_t)port;
				info_printf("Remote port: %u.", (uint32_t)port);
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
			if_true_vef(-1, bestClient.State != ::gpk::UDP_CONNECTION_STATE_IDLE, "%s", "Failed to connect to server.");
			if_fail_fe(gpk::connectionPushData(bestClient, bestClient.Queue, {(const uint8_t*)environmentBlock.begin(), environmentBlock.size()}, true, true));	// Enqueue the packet
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

static	gpk::err_t		cgiMain				(gpk::SRuntimeValues & runtimeValues)	{
	if_fail_fef(gpk::tcpipInitialize(), "%s", "Failed to initialize network subsystem.");
	gpk::SCGIRuntimeValues		cgiRuntimeValues(runtimeValues);
	if_fail_fe(gpk::cgiRuntimeValuesLoad(cgiRuntimeValues));
	gpk::string					html;
	if(::gpk::failed(::cgiBootstrap(runtimeValues, html))) {
		fprintf(stdout, "%s\r\n", "Content-Type: text/html"
			"\r\nCache-Control: no-store"
			"\r\n\r\n"
			"<html><head><title>Internal server error</title></head><body>Failed to process request.</body></html>"
			"\r\n"
			"\r\n"
		);
	}
	else {
		fprintf(stdout, "%s\r\n", "Content-Type: application/json"
			"\r\nCache-Control: no-store"
		);
		html.push_back('\0');
		fprintf(stdout, "%.*s", GPK_VIEW_ARGS(html));
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
