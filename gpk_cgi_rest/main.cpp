#include "gpk_cgi.h"
#include "gpk_cgi_module.h"
#include "gpk_string_helper.h"
#include "gpk_process.h"
#include "gpk_json_expression.h"
#include "gpk_parse.h"
#include "gpk_storage.h"

#include "gpk_udp_client.h"

::gpk::error_t										metrics_split			(const ::gpk::view_const_string& input_string, ::gpk::SCoord2<int32_t>& output_metrics)												{
	uint32_t												iChar					= 0;
	for(iChar = 0; iChar < input_string.size(); ++iChar) {
		if('x' == input_string[iChar]) {
			::gpk::apod<char>										sx						= {};
			::gpk::apod<char>										sy						= {};
			output_metrics										= {};
			gpk_necs(sy.append((char*)input_string.begin(), iChar));
			gpk_necs(sx.append((char*)&input_string[iChar + 1], input_string.size() - (iChar + 1)));
			::gpk::parseIntegerDecimal(sx, &output_metrics.x);
			::gpk::parseIntegerDecimal(sy, &output_metrics.y);
			break;
		}
	}
	return 0;
}

static	::gpk::error_t								initClient						(::gpk::SUDPClient & bestClient)										{
	bestClient.AddressConnect							= {};
	::gpk::tcpipAddress(9998, 0, ::gpk::TRANSPORT_PROTOCOL_UDP, bestClient.AddressConnect);	// If loading the remote IP from the json fails, we fall back to the local address.
	::gpk::apod<char_t>										fileJSONConfig					= {};
	::gpk::SJSONReader										jsonConfig						= {};
	{	// Attempt to load config file.
		::gpk::vcs												fileNameJSONConfig				= "gpk_config.json";
		rew_if(errored(::gpk::fileToMemory(fileNameJSONConfig, fileJSONConfig)), "Failed to load config JSON file! File not found? File name: %s.", fileNameJSONConfig.begin());
		gwarn_if(::gpk::jsonParse(jsonConfig, {fileJSONConfig.begin(), fileJSONConfig.size()}), "Failed to read json! Not a valid json file? File name: %s.", fileNameJSONConfig.begin());
	}
	{ // attempt to load address from config file.
		{ //
			::gpk::vcs												jsonIP							= {};
			gwarn_if(errored(::gpk::jsonExpressionResolve(::gpk::vcs{"application.gpk_cgi_rest.remote_ip"}, jsonConfig, 0, jsonIP)), "Failed to load config from json! Last contents found: %s.", jsonIP.begin())
			else {
				info_printf("Remote IP: %s.", jsonIP.begin());
				gerror_if(errored(::gpk::tcpipAddress(jsonIP, {}, bestClient.AddressConnect)), "Failed to read IP address from JSON config file: %s.", jsonIP.begin());	// turn the string into a SIPv4 struct.
			}
		}
		{ // load port from config file
			bestClient.AddressConnect.Port						= 9998;
			::gpk::vcs												jsonPort							= {};
			gwarn_if(errored(::gpk::jsonExpressionResolve(::gpk::vcs{"application.gpk_cgi_rest.remote_port"}, jsonConfig, 0, jsonPort)), "Failed to load config from json! Last contents found: %s.", jsonPort.begin())
			else {
				uint64_t												port								= 0;
				::gpk::parseIntegerDecimal(jsonPort, &port);
				bestClient.AddressConnect.Port										= (uint16_t)port;
				info_printf("Remote port: %u.", (uint32_t)port);
			}
		}
	}
	return 0;
}

static	int											cgiBootstrap			(const ::gpk::SCGIRuntimeValues & runtimeValues, ::gpk::array_pod<char> & output)										{
	::gpk::array_pod<char_t>								environmentBlock		= {};
	{	// Prepare CGI environment and request content packet to send to the service.
		gpk_necs(::gpk::environmentBlockFromEnviron(environmentBlock));
		environmentBlock.append(runtimeValues.Content.Body.begin(), runtimeValues.Content.Body.size());
		environmentBlock.push_back(0);
	}
	{	// Connect the client to the service.
		::gpk::SUDPClient										bestClient				= {};
		{	// setup and connect
			bestClient.AddressConnect							= {};
			gpk_necs(::initClient(bestClient));
			gpk_necs(::gpk::clientConnect(bestClient));
		}
		::gpk::apod<char_t>										responseRemote;
		{	// Send the request data to the connected service.
			ree_if(bestClient.State != ::gpk::UDP_CONNECTION_STATE_IDLE, "%s", "Failed to connect to server.");
			gpk_necs(::gpk::connectionPushData(bestClient, bestClient.Queue, environmentBlock, true, true));	// Enqueue the packet
			while(bestClient.State != ::gpk::UDP_CONNECTION_STATE_DISCONNECTED) {	// Loop until we ge the response or the client disconnects
				gpk_necs(::gpk::clientUpdate(bestClient));
				::gpk::apobj<::gpk::SUDPMessage>	received;
				{	// pick up messages for later processing
					::gpk::mutex_guard												lockRecv					(bestClient.Queue.MutexReceive);
					received													= bestClient.Queue.Received;
					bestClient.Queue.Received.clear();
				}
				if(received.size()) {	// Response has been received. Break loop.
					responseRemote												= received[0]->Payload;
					break;
				}
			}
		}
		//info_printf("Remote CGI answer: %s.", responseRemote.begin());
		gpk_necs(::gpk::clientDisconnect(bestClient));
		output												= responseRemote;
	}
	return 0;
}

static int											cgiMain				(int argc, char** argv, char**envv)	{
	(void)(envv);
	gpk_necall(::gpk::tcpipInitialize(), "%s", "Failed to initialize network subsystem.");
	::gpk::SCGIRuntimeValues								runtimeValues;
	gpk_necall(::gpk::cgiRuntimeValuesLoad(runtimeValues, {(const char**)argv, (uint32_t)argc}), "%s", "Failed to load cgi runtime values.");
	::gpk::array_pod<char>									html;
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
		);
		html.push_back('\0');
		printf("%s", html.begin());
#ifdef GPK_WINDOWS
		OutputDebugStringA(html.begin());
#endif
	}
	gpk_necall(::gpk::tcpipShutdown(), "Failed to shut down network subsystem. %s", "Why!?");
	return 0;
}

int													main				(int argc, char** argv, char**envv)	{
	return ::cgiMain(argc, argv, envv);
}

#ifdef GPK_WINDOWS
#include <Windows.h>

int WINAPI											WinMain
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
