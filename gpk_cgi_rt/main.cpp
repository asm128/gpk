#include "gpk_cgi.h"
#include "gpk_cgi_module.h"
#include "gpk_string_helper.h"

#include "gpk_parse.h"

#include <Windows.h>
//
//static ::gpk::error_t								metrics_split			(const ::gpk::view_const_string& input_string, ::gpk::n2<int32_t>& output_metrics)												{
//	uint32_t												iChar					= 0;
//	for(iChar = 0; iChar < input_string.size(); ++iChar) {
//		if('x' == input_string[iChar]) {
//			::gpk::apod<char>									sx						= {};
//			::gpk::apod<char>									sy						= {};
//			sy.append((char*)input_string.begin(), iChar);
//			sx.append((char*)&input_string[iChar + 1], input_string.size() - (iChar + 1));
//			try {
//				output_metrics.x									= ::std::stoi(::std::string{sy.begin(), sy.size()});
//				output_metrics.y									= ::std::stoi(::std::string{sy.begin(), sy.size()});
//			}
//			catch (...) {
//				output_metrics										= {};
//				return -1;
//			}
//			break;
//		}
//	}
//	return 0;
//}

::gpk::error_t										genHTMLModuleOutput				(::gpk::SCGIFramework & framework, ::gpk::apod<char> & output){
	const ::gpk::SCGIRuntimeValues							& runtimeValues					= framework.RuntimeValues;
	//char													buffer[4096]					= {};
	::gpk::SCGIModule										module;
	::gpk::apod<char>								moduleName						= gpk::toString({framework.ModuleName.begin(), ::gpk::min(framework.ModuleName.size(), 16U)});
	::gpk::loadCGIModule(module, moduleName);
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

stacxpr	const char						html_script	[]			=
	"\n<script>"
	"\nfunction bootstrap() {"
    "\nvar url = [self.location.protocol, '//', self.location.host, self.location.pathname].join('');"
    "\nself.location = url + \"?m=%s&bt=1\" + \"&w=\" + document.documentElement.clientWidth + \"&h=\" + document.documentElement.clientHeight;"
	"\n}"
	"\n</script>"
	;

::gpk::error_t										processKeyVal					(::gpk::SCGIFramework& framework, const ::gpk::TKeyValConstChar& keyVal)	{
	if(0 == keyVal.Key.size())
		return -1;

	framework.TargetSize								= {123, 456};

		 if(keyVal.Key == ::gpk::vcs("w")) ::gpk::parseIntegerDecimal(keyVal.Val, framework.TargetSize.x);
	else if(keyVal.Key == ::gpk::vcs("h")) ::gpk::parseIntegerDecimal(keyVal.Val, framework.TargetSize.y);

	if(keyVal.Val.size() && keyVal.Key.size() == (::gpk::size("bt") - 1) && 0 == memcmp("bt", keyVal.Key.begin(), ::gpk::size("bt") - 1)) {
		::gpk::parseIntegerDecimal(keyVal.Val, framework.Bootstrapped);
	}
	if(keyVal.Val.size() && keyVal.Key.size() == (::gpk::size("m") - 1) && 0 == memcmp("m", keyVal.Key.begin(), ::gpk::size("m") - 1))
		framework.ModuleName								= keyVal.Val;
	return 0;
}

int													cgiBootstrap			(::gpk::SCGIFramework & framework, ::gpk::apod<char> & output)										{
	const ::gpk::SCGIRuntimeValues							& runtimeValues			= framework.RuntimeValues;
	if(framework.Bootstrapped) {
		::genHTMLModuleOutput(framework, output);
	}
	else {
		char													buffer[4096]		= {};
		output.append(buffer, ::sprintf_s(buffer, "%s",
			"<html>"
			"\n<head>"
		));
		output.append(buffer, ::sprintf_s(buffer, html_script, framework.ModuleName.size() ? framework.ModuleName.begin() : "test_cgi_module"));
		output.append(buffer, ::sprintf_s(buffer, "%s",
			"\n<link rel=\"stylesheet\" href=\"./page.css\">"
			"\n</head>"
		));
		output.append(buffer, ::sprintf_s(buffer, "\n<body style=\"width:100%%; height:100%%; background-color:#4040C0; \" %s>", framework.Bootstrapped ? "" : "onload=\"bootstrap()\"" ));
		output.append(buffer, ::sprintf_s(buffer, "\n<h4>Booting %s...</h4>", framework.ModuleName.begin()));
		::gpk::view_const_string							querystring;
		::gpk::find(::gpk::vcs{"QUERY_STRING"}, framework.RuntimeValues.QueryStringKeyVals, querystring);
		if(querystring.size())
			output.append(buffer, sprintf_s(buffer, "\n<h4>QueryString (%u): %s</h4>", querystring.size(), querystring.begin()));
		const ::gpk::view<::gpk::vcc>		keyvalviews			= runtimeValues.QueryStringElements;
		for(uint32_t iChar = 0; iChar < keyvalviews.size(); ++iChar) {
			output.append(buffer, ::gpk::formatForSize(keyvalviews[iChar], buffer, "\n<h3>KeyVal: ", "</h3>"));

			const ::gpk::TKeyValConstChar						& keyval			= runtimeValues.QueryStringKeyVals[iChar];
			output.append(buffer, ::gpk::formatForSize(keyval.Key, buffer, "\n<h3>Key: ", "</h3>"));
			output.append(buffer, ::gpk::formatForSize(keyval.Val, buffer, "\n<h3>Val: ", "</h3>"));
		}

		::gpk::vcs			contentype	;
		::gpk::vcs			remoteIP	;
		::gpk::vcs			remotePORT	;
		::gpk::vcs			contentLength;
		::gpk::find(::gpk::vcs{"CONTENT_TYPE"	 }, runtimeValues.QueryStringKeyVals, contentype);
		::gpk::find(::gpk::vcs{"REMOTE_IP"	 }, runtimeValues.QueryStringKeyVals, remoteIP	);
		::gpk::find(::gpk::vcs{"REMOTE_PORT"	 }, runtimeValues.QueryStringKeyVals, remotePORT	);
		::gpk::find(::gpk::vcs{"CONTENT_LENGTH"}, runtimeValues.QueryStringKeyVals, contentLength);
		output.append(buffer, ::gpk::formatForSize({contentLength	.begin(), contentLength	.size()}, buffer, "\n<h2>CONTENT_LENGTH: "	, "</h2>"));
		output.append(buffer, ::gpk::formatForSize({contentype		.begin(), contentype	.size()}, buffer, "\n<h2>CONTENT_TYPE: "	, "</h2>"));
		output.append(buffer, ::sprintf_s(buffer, "\n<h2>Client area size: %u x %u</h2>", (uint32_t)framework.TargetSize.x, (uint32_t)framework.TargetSize.y));
		output.append(buffer, ::sprintf_s(buffer, "\n<h4>Bootstrapped: %s</h4>"			, framework.Bootstrapped ? "true" : "false"));
		output.append(buffer, ::sprintf_s(buffer, "\n<h4>IP: %u.%u.%u.%u:%u</h4>"		, GPK_IPV4_EXPAND(framework.RuntimeValues.RemoteIP)));
		output.append(buffer, ::sprintf_s(buffer, "\n<h4>String IP: %s</h4>"			, remoteIP	.begin()));
		output.append(buffer, ::sprintf_s(buffer, "\n<h4>String Port: %s</h4>"			, remotePORT.begin()));
		output.append(buffer, ::sprintf_s(buffer, "\n<h4>Bootstrapped: %s</h4>"			, framework.Bootstrapped ? "true" : "false"));

		//int														argc					= __argc;
		//char													** argv					= __argv;
		//for(int32_t iArg = 0; iArg < argc; ++iArg)
		//	output.append(buffer, sprintf_s(buffer, "\n<h1>argv[%u]: %s</h1>", iArg, argv[iArg]));
		{
			::gpk::apod<char>	content_body			= {};
			content_body.resize(runtimeValues.Content.Length);
			uint32_t			iChar					= 0;
			char				iArg					= 0;
			while(iChar < runtimeValues.Content.Length) {
				int					count					= 0;
				//memset(content_body.begin(), 0, content_body.size());
				content_body.resize(0);
				uint32_t			iOffset					= iChar;
				bool				value					= false;
				while(iChar < runtimeValues.Content.Length && (iArg = runtimeValues.Content.Body[iChar++])) {
					if(iArg == '\n') {
						++count;
						if(count >= 2) {
							value			= true;
							count			= 0;
							//break;
						}
					}
					else if(iArg != '\r')
						count			= 0;
					content_body.push_back(iArg);
				}
				if(0 == content_body.size())
					break;
				content_body.push_back(0);
				output.append(buffer, ::sprintf_s(buffer, "\n<h1>content_body (%u:%u/%u:%u): %s</h1>", iChar - iOffset, content_body.size(), runtimeValues.Content.Length, runtimeValues.Content.Body.size(), content_body.begin()));
			}
		}
		output.append(buffer, ::sprintf_s(buffer, "\n<h1>content_body (Raw:%u:%u): %s</h1>", runtimeValues.Content.Length, runtimeValues.Content.Body.size(), runtimeValues.Content.Body.begin()));
		output.append(buffer, ::sprintf_s(buffer, "\n%s\n", "</body>\n</html>"));
	}
	return 0;
}

int WINAPI											WinMain				(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, INT nCmdShow)	{
	hInstance, hPrevInstance, szCmdLine, nCmdShow;
	::gpk::SCGIFramework									framework;
	::gpk::cgiRuntimeValuesLoad(framework.RuntimeValues, {(const char**)__argv, (uint32_t)(__argc)});
	const ::gpk::array_obj<::gpk::vcc>			& keyvalviews			= framework.RuntimeValues.QueryStringElements;
	for(uint32_t iKeyVal = 0; iKeyVal < keyvalviews.size(); ++iKeyVal)
		::processKeyVal(framework, framework.RuntimeValues.QueryStringKeyVals[iKeyVal]);

	printf("%s\r\n\r\n",
		"Content-Type: text/html"
		"\nCache-Control: no-store"
	);
	::gpk::apod<char>									html;
	::cgiBootstrap(framework, html);
	html.push_back('\0');
	OutputDebugStringA(html.begin());
	printf("%s", html.begin());
	::std::putc(0, stdout);
	return 0;
}

// 17070035
// 0810-122-2666
