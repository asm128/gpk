#include "gpk_cgi.h"
#include "gpk_cgi_module.h"
#include "gpk_string_helper.h"
#include "gpk_process.h"

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

int													cgiBootstrap			(::gpk::array_pod<char> & output)										{
	::gpk::array_pod<char_t>								environmentBlock		= {};
	ree_if(errored(::gpk::getEnvironmentBlock(environmentBlock)), "%s", "Failed");
	{ 
		::gpk::SUDPClient										bestClient				= {};
		{	// setup and connect
			bestClient.AddressConnect							= {};
			gpk_necall(::gpk::tcpipAddress(9998, 0, ::gpk::TRANSPORT_PROTOCOL_UDP, bestClient.AddressConnect), "%s", "error");
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
		//{
		//	char													buffer[8192]		= {};
		//	const ::gpk::array_obj<::gpk::view_const_string>		& keyvalviews		= runtimeValues.QueryStringElements;
		//	if(runtimeValues.QueryString.size())
		//		output.append(buffer, sprintf_s(buffer, "\r\n[{ \"queryString\" : { \"length\" : %u, \"data\" : \"%s\", \"values\" : [", runtimeValues.QueryString.size(), runtimeValues.QueryString.begin()));

		//	for(uint32_t iKeyVal = 0; iKeyVal < keyvalviews.size(); ++iKeyVal) {
		//		const ::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>	& keyval		= runtimeValues.QueryStringKeyVals[iKeyVal];
		//		output.append(buffer, ::gpk::formatForSize(keyval.Key, buffer, "\n { \"Key\": \"", "\""));
		//		if(keyvalviews.size() - 1 == iKeyVal)
		//			output.append(buffer, ::gpk::formatForSize(keyval.Val, buffer, "\n , \"Val\": \"", "\"\n }"));// without the trailing , comma character
		//		else
		//			output.append(buffer, ::gpk::formatForSize(keyval.Val, buffer, "\n , \"Val\": \"", "\"\n }, "));
		//	}
		//	output.append(buffer, sprintf_s(buffer, "\n] }, "));
		//	output.append(buffer, sprintf_s(buffer, "\n \"cgi_environment\" : "));

		//	output.append(buffer, sprintf_s(buffer, "\n { \"AUTH_PASSWORD\"			: \"%s\"", runtimeValues.Environment.AUTH_PASSWORD			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"AUTH_TYPE\"				: \"%s\"", runtimeValues.Environment.AUTH_TYPE				.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"AUTH_USER\"				: \"%s\"", runtimeValues.Environment.AUTH_USER				.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"CERT_COOKIE\"			: \"%s\"", runtimeValues.Environment.CERT_COOKIE			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"CERT_FLAGS\"			: \"%s\"", runtimeValues.Environment.CERT_FLAGS				.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"CERT_ISSUER\"			: \"%s\"", runtimeValues.Environment.CERT_ISSUER			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"CERT_KEYSIZE\"			: \"%s\"", runtimeValues.Environment.CERT_KEYSIZE			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"CERT_SECRETKEYSIZE\"	: \"%s\"", runtimeValues.Environment.CERT_SECRETKEYSIZE		.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"CERT_SERIALNUMBER\"		: \"%s\"", runtimeValues.Environment.CERT_SERIALNUMBER		.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"CERT_SERVER_ISSUER\"	: \"%s\"", runtimeValues.Environment.CERT_SERVER_ISSUER		.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"CERT_SERVER_SUBJECT\"	: \"%s\"", runtimeValues.Environment.CERT_SERVER_SUBJECT	.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"CERT_SUBJECT\"			: \"%s\"", runtimeValues.Environment.CERT_SUBJECT			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"CF_TEMPLATE_PATH\"		: \"%s\"", runtimeValues.Environment.CF_TEMPLATE_PATH		.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"CONTENT_LENGTH\"		: \"%s\"", runtimeValues.Environment.CONTENT_LENGTH			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"CONTENT_TYPE\"			: \"%s\"", runtimeValues.Environment.CONTENT_TYPE			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"CONTEXT_PATH\"			: \"%s\"", runtimeValues.Environment.CONTEXT_PATH			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"GATEWAY_INTERFACE\"		: \"%s\"", runtimeValues.Environment.GATEWAY_INTERFACE		.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"HTTPS\"					: \"%s\"", runtimeValues.Environment.HTTPS					.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"HTTPS_KEYSIZE\"			: \"%s\"", runtimeValues.Environment.HTTPS_KEYSIZE			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"HTTPS_SECRETKEYSIZE\"	: \"%s\"", runtimeValues.Environment.HTTPS_SECRETKEYSIZE	.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"HTTPS_SERVER_ISSUER\"	: \"%s\"", runtimeValues.Environment.HTTPS_SERVER_ISSUER	.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"HTTPS_SERVER_SUBJECT\"	: \"%s\"", runtimeValues.Environment.HTTPS_SERVER_SUBJECT	.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"HTTP_ACCEPT\"			: \"%s\"", runtimeValues.Environment.HTTP_ACCEPT			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"HTTP_ACCEPT_ENCODING\"	: \"%s\"", runtimeValues.Environment.HTTP_ACCEPT_ENCODING	.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"HTTP_ACCEPT_LANGUAGE\"	: \"%s\"", runtimeValues.Environment.HTTP_ACCEPT_LANGUAGE	.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"HTTP_CONNECTION\"		: \"%s\"", runtimeValues.Environment.HTTP_CONNECTION		.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"HTTP_COOKIE\"			: \"%s\"", runtimeValues.Environment.HTTP_COOKIE			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"HTTP_HOST\"				: \"%s\"", runtimeValues.Environment.HTTP_HOST				.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"HTTP_REFERER\"			: \"%s\"", runtimeValues.Environment.HTTP_REFERER			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"HTTP_USER_AGENT\"		: \"%s\"", runtimeValues.Environment.HTTP_USER_AGENT		.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"QUERY_STRING\"			: \"%s\"", runtimeValues.Environment.QUERY_STRING			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"REMOTE_ADDR\"			: \"%s\"", runtimeValues.Environment.REMOTE_ADDR			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"REMOTE_HOST\"			: \"%s\"", runtimeValues.Environment.REMOTE_HOST			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"REMOTE_USER\"			: \"%s\"", runtimeValues.Environment.REMOTE_USER			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"REQUEST_METHOD\"		: \"%s\"", runtimeValues.Environment.REQUEST_METHOD			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"SCRIPT_NAME\"			: \"%s\"", runtimeValues.Environment.SCRIPT_NAME			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"SERVER_NAME\"			: \"%s\"", runtimeValues.Environment.SERVER_NAME			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"SERVER_PORT\"			: \"%s\"", runtimeValues.Environment.SERVER_PORT			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"SERVER_PORT_SECURE\"	: \"%s\"", runtimeValues.Environment.SERVER_PORT_SECURE		.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"SERVER_PROTOCOL\"		: \"%s\"", runtimeValues.Environment.SERVER_PROTOCOL		.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"SERVER_SOFTWARE\"		: \"%s\"", runtimeValues.Environment.SERVER_SOFTWARE		.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n , \"WEB_SERVER_API\"		: \"%s\"", runtimeValues.Environment.WEB_SERVER_API			.begin()));
		//	output.append(buffer, sprintf_s(buffer, "\n },"));
		//																			 
		//	//int														argc					= __argc; 
		//	//char													** argv					= __argv;
		//	//for(int32_t iArg = 0; iArg < argc; ++iArg) 
		//	//	output.append(buffer, sprintf_s(buffer, "\n<h1>argv[%u]: %s</h1>", iArg, argv[iArg]));
		//	{
		//		::gpk::array_pod<char>									content_body			= {};
		//		content_body.resize(runtimeValues.Content.Length);
		//		uint32_t												iChar					= 0;
		//		char													iArg					= 0;
		//		while(iChar < runtimeValues.Content.Length) {
		//			int														count					= 0;
		//			//memset(content_body.begin(), 0, content_body.size());
		//			content_body.resize(0);
		//			uint32_t												iOffset					= iChar;
		//			bool													value					= false;
		//			while(iChar < runtimeValues.Content.Length && (iArg = runtimeValues.Content.Body[iChar++])) {
		//				if(iArg == '\n') {
		//					++count;
		//					if(count >= 2) {
		//						value												= true;
		//						count												= 0;
		//						//break;
		//					}
		//				}
		//				else if(iArg != '\r')
		//					count												= 0;
		//				content_body.push_back(iArg);
		//			}
		//			if(0 == content_body.size())
		//				break;
		//			content_body.push_back(0);
		//			output.append(buffer, sprintf_s(buffer, "\n \"content_body\" : {\"u0\" : %u, \"u1\" : %u, \"u2\" : %u, \"u3\" : %u, \"value\": \n\"%s\" }, "
		//				, iChar - iOffset, content_body.size(), runtimeValues.Content.Length, runtimeValues.Content.Body.size(), content_body.begin()));
		//		}
		//	}
		//	output.append(buffer, sprintf_s(buffer, "\n \"content_body_\" : {\"u0\" : %u, \"u1\" : %u, \"text\" : \"%s\" }", runtimeValues.Content.Length, runtimeValues.Content.Body.size(), runtimeValues.Content.Body.begin()));

		//	//output.append(buffer, sprintf_s(buffer, "%s", "<iframe width=\"100%%\" height=\"100%%\" src=\"http://localhost/home.html\"></iframe>\n"));
		//	output.append(buffer, sprintf_s(buffer, "\n%s\n", "}]"));
		//}
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
