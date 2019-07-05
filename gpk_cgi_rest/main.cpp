#include "gpk_cgi.h"
#include "gpk_cgi_module.h"
#include "gpk_string_helper.h"
#include "gpk_process.h"
#include "gpk_json_expression.h"
#include "gpk_parse.h"
#include "gpk_storage.h"

#include "gpk_udp_client.h"

#include <string>

#include <Windows.h>

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

::gpk::error_t										genHTMLModuleOutput				(::gpk::SCGIFramework & framework, ::gpk::array_pod<char> & output){
	const ::gpk::SCGIRuntimeValues							& runtimeValues					= framework.RuntimeValues;
	//char													buffer[4096]					= {};
	::gpk::SCGIModule										module;
	::std::string											moduleName						= {framework.ModuleName.begin(), ::gpk::min(framework.ModuleName.size(), 16U)};
	::gpk::loadCGIModule(module, {moduleName.data(), (uint32_t)moduleName.size()});
	module.Create	(&module.Application, &framework);
	module.Setup	(module.Application);
	module.Update	(module.Application, false);
	module.Render	(module.Application, output);
	module.Cleanup	(module.Application);
	module.Delete	(&module.Application);
	//output.append(buffer, formatForSize(framework.ModuleName, buffer, ::gpk::size(buffer), "<html>\n<body>\n<h1>bootstrapped module: ", "</h1>\n</body>\n</html>"));
	runtimeValues;
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
		const int32_t											indexObjectConfig				= ::gpk::jsonArrayValueGet(*jsonConfig.Tree[0], 0);	// Get the first JSON {object} found in the [document]
		{ // 
			::gpk::view_const_string								jsonIP							= {};
			gwarn_if(errored(::gpk::jsonExpressionResolve("application.gpk_cgi_rest.remote_ip", jsonConfig, indexObjectConfig, jsonIP)), "Failed to load config from json! Last contents found: %s.", jsonIP.begin())
			else {
				info_printf("Remote IP: %s.", jsonIP.begin());
				gerror_if(errored(::gpk::tcpipAddress(jsonIP, {}, bestClient.AddressConnect)), "Failed to read IP address from JSON config file: %s.", jsonIP.begin());	// turn the string into a SIPv4 struct.
			}
		}
		{ // load port from config file
			bestClient.AddressConnect.Port						= 9998;
			::gpk::view_const_string								jsonPort							= {};
			gwarn_if(errored(::gpk::jsonExpressionResolve("application.gpk_cgi_rest.remote_port"	, jsonConfig, indexObjectConfig, jsonPort)), "Failed to load config from json! Last contents found: %s.", jsonPort.begin()) 
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

int													cgiBootstrap			(::gpk::array_pod<char> & output)										{
	::gpk::array_pod<char_t>								environmentBlock		= {};
	ree_if(errored(::gpk::getEnvironmentBlock(environmentBlock)), "%s", "Failed");
	{ 
		::gpk::SUDPClient										bestClient				= {};
		{	// setup and connect
			bestClient.AddressConnect							= {};
			gpk_necall(::initClient(bestClient), "%s", "error");
			gpk_necall(::gpk::clientConnect(bestClient), "%s", "error");
		}
		::gpk::array_pod<char_t>								responseRemote;
		{
			ree_if(bestClient.State != ::gpk::UDP_CONNECTION_STATE_IDLE, "%s", "Failed to connect to server.");
			gpk_necall(::gpk::connectionPushData(bestClient, bestClient.Queue, environmentBlock), "%s", "error");
			while(bestClient.State != ::gpk::UDP_CONNECTION_STATE_DISCONNECTED) {
				gpk_necall(::gpk::clientUpdate(bestClient), "%s", "error");
				::gpk::array_obj<::gpk::ptr_obj<::gpk::SUDPConnectionMessage>>	received;
				{	// pick up messages for later processing
					::gpk::mutex_guard														lockRecv					(bestClient.Queue.MutexReceive);
					received															= bestClient.Queue.Received;
					bestClient.Queue.Received.clear();
				}
				if(received.size()) {
					responseRemote	= received[0]->Payload;
					break;
				}
			}
		}
		info_printf("Remote CGI answer: %s.", responseRemote.begin());
		gpk_necall(::gpk::clientDisconnect(bestClient), "%s", "error");
		output									= responseRemote;
	}
	return 0;
}

static int											cgiMain				()	{
	gpk_necall(::gpk::tcpipInitialize(), "%s", "Failed to initialize network subsystem.");
	::gpk::SCGIRuntimeValues								runtimeValues;
	gpk_necall(::gpk::cgiRuntimeValuesLoad(runtimeValues), "%s", "Failed to load cgi runtime values.");
	::gpk::array_pod<char>									html;
	if errored(::cgiBootstrap(html)) {
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
		OutputDebugStringA(html.begin());
		printf("%s", html.begin());
	}
	gpk_necall(::gpk::tcpipShutdown(), "Failed to shut down network subsystem. %s", "Why??!?");
	return 0;
}

int													main				(int argc, char** argv, char**envv)	{
	(void)argc, (void)argv, (void)envv;
	return ::cgiMain();
}

int WINAPI											WinMain				
	(	_In_		HINSTANCE	hInstance
	,	_In_opt_	HINSTANCE	hPrevInstance
	,	_In_		LPSTR		lpCmdLine
	,	_In_		int			nShowCmd
	) {
	(void)hInstance, (void)hPrevInstance, (void)lpCmdLine, (void)nShowCmd;
	return ::cgiMain();
}

// 17070035
// 0810-122-2666
