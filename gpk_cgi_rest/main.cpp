#include "gpk_cgi.h"
#include "gpk_cgi_module.h"
#include "gpk_string_helper.h"
#include "gpk_process.h"
#include "gpk_json_expression.h"
#include "gpk_parse.h"
#include "gpk_storage.h"

#include "gpk_udp_client.h"

#include <string>		// for ::std::stoi()

::gpk::error_t										metrics_split			(const ::gpk::view_const_string& input_string, ::gpk::SCoord2<int32_t>& output_metrics)												{
	uint32_t												iChar					= 0;
	for(iChar = 0; iChar < input_string.size(); ++iChar) {
		if('x' == input_string[iChar]) {
			::gpk::array_pod<char>									sx						= {};
			::gpk::array_pod<char>									sy						= {};
			gpk_necall(sy.append((char*)input_string.begin(), iChar), "%s", "");
			gpk_necall(sx.append((char*)&input_string[iChar + 1], input_string.size() - (iChar + 1)), "%s", "");
			try {
				output_metrics.x									= ::std::stoi(::std::string{sy.begin(), sy.size()});
				output_metrics.y									= ::std::stoi(::std::string{sy.begin(), sy.size()});
			}
			catch (...) {
				output_metrics										= {};
				return -1;
			}
			break;
		}
	}
	return 0;
}

static	::gpk::error_t								initClient						(::gpk::SUDPClient & bestClient)										{
	bestClient.AddressConnect							= {};
	::gpk::tcpipAddress(9998, 0, ::gpk::TRANSPORT_PROTOCOL_UDP, bestClient.AddressConnect);	// If loading the remote IP from the json fails, we fall back to the local address.
	::gpk::array_pod<char_t>								fileJSONConfig					= {};
	::gpk::SJSONReader										jsonConfig						= {};
	{	// Attempt to load config file.
		::gpk::view_const_string								fileNameJSONConfig			= "gpk_config.json";
		rew_if(errored(::gpk::fileToMemory(fileNameJSONConfig, fileJSONConfig)), "Failed to load config JSON file! File not found? File name: %s.", fileNameJSONConfig.begin());
		gwarn_if(::gpk::jsonParse(jsonConfig, {fileJSONConfig.begin(), fileJSONConfig.size()}), "Failed to read json! Not a valid json file? File name: %s.", fileNameJSONConfig.begin());
	}
	{ // attempt to load address from config file.
		{ //
			::gpk::view_const_string								jsonIP							= {};
			gwarn_if(errored(::gpk::jsonExpressionResolve("application.gpk_cgi_rest.remote_ip", jsonConfig, 0, jsonIP)), "Failed to load config from json! Last contents found: %s.", jsonIP.begin())
			else {
				info_printf("Remote IP: %s.", jsonIP.begin());
				gerror_if(errored(::gpk::tcpipAddress(jsonIP, {}, bestClient.AddressConnect)), "Failed to read IP address from JSON config file: %s.", jsonIP.begin());	// turn the string into a SIPv4 struct.
			}
		}
		{ // load port from config file
			bestClient.AddressConnect.Port						= 9998;
			::gpk::view_const_string								jsonPort							= {};
			gwarn_if(errored(::gpk::jsonExpressionResolve("application.gpk_cgi_rest.remote_port"	, jsonConfig, 0, jsonPort)), "Failed to load config from json! Last contents found: %s.", jsonPort.begin())
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
		ree_if(errored(::gpk::environmentBlockFromEnviron(environmentBlock)), "%s", "Failed");
		environmentBlock.append(runtimeValues.Content.Body.begin(), runtimeValues.Content.Body.size());
		environmentBlock.push_back(0);
	}
	{	// Connect the client to the service.
		::gpk::SUDPClient										bestClient				= {};
		{	// setup and connect
			bestClient.AddressConnect							= {};
			gpk_necall(::initClient(bestClient), "%s", "error");
			gpk_necall(::gpk::clientConnect(bestClient), "%s", "error");
		}
		::gpk::array_pod<char_t>								responseRemote;
		{	// Send the request data to the connected service.
			ree_if(bestClient.State != ::gpk::UDP_CONNECTION_STATE_IDLE, "%s", "Failed to connect to server.");
			gpk_necall(::gpk::connectionPushData(bestClient, bestClient.Queue, environmentBlock, true, true), "%s", "error");	// Enqueue the packet
			while(bestClient.State != ::gpk::UDP_CONNECTION_STATE_DISCONNECTED) {	// Loop until we ge the response or the client disconnects
				gpk_necall(::gpk::clientUpdate(bestClient), "%s", "error");
				::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>	received;
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
		gpk_necall(::gpk::clientDisconnect(bestClient), "%s", "error");
		output									= responseRemote;
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
	gpk_necall(::gpk::tcpipShutdown(), "Failed to shut down network subsystem. %s", "Why??!?");
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
