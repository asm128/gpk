#include "gpk_cgi.h"
#include "gpk_cgi_module.h"
#include <string>

#include <Windows.h>

::gpk::error_t										metrics_split			(const ::gpk::view_const_string & input_string, ::gpk::SCoord2<int32_t>& output_metrics)												{
	uint32_t												iChar					= 0;
	for(iChar = 0; iChar < input_string.size(); ++iChar) {
		if('x' == input_string[iChar]) {
			::gpk::array_pod<char>									sx						= {};
			::gpk::array_pod<char>									sy						= {};
			sy.append((char*)input_string.begin(), iChar);
			sx.append((char*)&input_string[iChar + 1], input_string.size() - (iChar + 1));
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

::gpk::error_t										formatForSize					(::gpk::view_const_string text, char* output, uint32_t maxlen, const char* pre = "", const char* post = "")	{
	::gpk::array_pod<char>									format							= {};
	format.resize((uint32_t)(strlen(pre) + strlen(post) + text.size() + 32));
	memset(format.begin(), 0, format.size());
	sprintf_s(format.begin(), format.size(), "%s" "%%" ".%u" "s" "%s", pre, text.size(), post);
	return sprintf_s(output, maxlen, format.begin(), text.begin());
	
}

template<size_t _sizeOutput>
::gpk::error_t										formatForSize					(::gpk::view_const_string text, char (&output)[_sizeOutput], const char* pre = "", const char* post = "")	{
	return ::formatForSize(text, output, (uint32_t)_sizeOutput, pre, post);
}

::gpk::error_t										genHTMLModuleOutput				(const ::gpk::SCGIFramework & framework, ::gpk::array_pod<char> & output){
	const ::gpk::SCGIRuntimeValues							& runtimeValues					= framework.RuntimeValues;
	//char													buffer[4096]					= {};
	::gpk::SCGIModule										module;
	::std::string											moduleName						= {framework.ModuleName.begin(), framework.ModuleName.size()};
	::gpk::loadCGIModule(module, {moduleName.data(), (uint32_t)moduleName.size()});
	module.Render(module.Application, output);
	//output.append(buffer, formatForSize(framework.ModuleName, buffer, ::gpk::size(buffer), "<html>\n<body>\n<h1>bootstrapped module: ", "</h1>\n</body>\n</html>"));
	runtimeValues;
	return 0;
}

static constexpr	const char						html_script	[]			= 
	"\n<script>"
	//"\nfunction mover(nam, col) {"
	//"\n    document.getElementById(\"Title\" + nam).style.setProperty(\"color\", col);"
	//"\n    document.getElementById(\"Button\"+ nam).style.setProperty(\"border-color\", col);"
	//"\n}"
	"\nfunction bootstrap(name) {"
	"\n    self.location = name + \"?m=%s&bt=1\" + \"&width=\" + document.documentElement.clientWidth + \"&height=\" + document.documentElement.clientHeight;"
	"\n}"
	"\n</script>"
	;

int													htmlBootstrap			(const ::gpk::SCGIFramework & framework, ::gpk::array_pod<char> & output)										{
	const ::gpk::SCGIRuntimeValues& runtimeValues = framework.RuntimeValues;
	if(false == framework.Bootstrapped) {
		char													buffer[4096]		= {};
		output.append(buffer, sprintf_s(buffer, "%s", 
			"<html>"
			"\n<head>"
		));
		output.append(buffer, sprintf_s(buffer, html_script, "test_cgi_module"));
		output.append(buffer, sprintf_s(buffer, "%s", 
			"\n<link rel=\"stylesheet\" href=\"./page.css\">"
			"\n</head>"
		));
		output.append(buffer, sprintf_s(buffer, "\n<body style=\"width:95%%; height:95%%; background-color:#FFCCAA; \" %s>", framework.Bootstrapped ? "" : "onload=\"bootstrap('./gpk_cgi_rt.exe')\"" ));
		const ::gpk::array_obj<::gpk::view_const_string>		& keyvalviews		= runtimeValues.QueryStringElements;
		output.append(buffer, sprintf_s(buffer, "\n<h4>QueryString (%u): %s</h4>", runtimeValues.QueryString.size(), runtimeValues.QueryString.begin()));
		for(uint32_t iChar = 0; iChar < keyvalviews.size(); ++iChar) {
			output.append(buffer, ::formatForSize(keyvalviews[iChar], buffer, "\n<h3>KeyVal: ", "</h3>"));

			const ::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>	& keyval		= runtimeValues.QueryStringKeyVals[iChar];
			output.append(buffer, ::formatForSize(keyval.Key, buffer, "\n<h3>Key: ", "</h3>"));
			output.append(buffer, ::formatForSize(keyval.Val, buffer, "\n<h3>Val: ", "</h3>"));
		}
		output.append(buffer, ::formatForSize({runtimeValues.ContentLength	.begin(), runtimeValues.ContentLength	.size()}, buffer, "\n<h2>CONTENT_LENGTH: "	, "</h2>"));
		output.append(buffer, ::formatForSize({runtimeValues.ContentType	.begin(), runtimeValues.ContentType		.size()}, buffer, "\n<h2>CONTENT_TYPE: "	, "</h2>"));
		output.append(buffer, sprintf_s(buffer, "\n<h2>Client area size: %u x %u</h2>"	, (uint32_t)framework.TargetSize.x, (uint32_t)framework.TargetSize.y));
		output.append(buffer, sprintf_s(buffer, "\n<h4>Bootstrapped: %s</h4>"			, framework.Bootstrapped ? "true" : "false"));

		int														argc					= __argc; 
		char													** argv					= __argv;
		for(int32_t iArg = 0; iArg < argc; ++iArg) 
			output.append(buffer, sprintf_s(buffer, "\n<h1>argv[%u]: %s</h1>", iArg, argv[iArg]));

		//for(uint32_t i)
		uint32_t												content_length			= runtimeValues.ContentLength.size() ? (uint32_t)::std::stoi(runtimeValues.ContentLength.begin()) : 0;
		uint32_t												content_count			= 0;
		int														iArg					= 0;
		while(content_count < content_length && iArg != -1)
		{
			iArg												= 0;
			int														count					= 0;
			char													content_body[1024*32]	= {};
			int32_t charCount = 0;
			while(iArg = getc(stdin)) {
				if(iArg == -1)
					break;
				if(iArg == '\n') {
					++count;
					if(count >= 2)
						break;
				}
				else if(iArg != '\r')
					count												= 0;
				content_body[charCount++]							= (char)iArg;
			}
			content_count += charCount ? charCount : 1;
			//output.append(buffer, sprintf_s(buffer, "<h1>getch(%u): %c</h1>\n", iArg, ((int)(char)iArg)));
			output.append(buffer, sprintf_s(buffer, "\n<h1>content_body: %s</h1>", content_body));
		}
		//output.append(buffer, sprintf_s(buffer, "%s", "<iframe width=\"100%\" height=\"100%\" src=\"http://localhost/home.html\"></iframe>\n"));
		output.append(buffer, sprintf_s(buffer, "\n%s\n", "</body>\n</html>"));
	}
	else {
		::genHTMLModuleOutput(framework, output);
	}
	return 0;
}

int WINAPI											WinMain				(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, INT nCmdShow)	{
	hInstance, hPrevInstance, szCmdLine, nCmdShow;
	::gpk::SCGIFramework									framework;
	::gpk::cgiRuntimeValuesLoad(framework);
	printf("%s\n\n", "Content-Type: text/html"
		"\nCache-Control: no-store\n"
	);
	
	::gpk::array_pod<char>									html;
	::htmlBootstrap(framework, html);
	html.push_back('\0');
	OutputDebugStringA(html.begin());
	printf("%s", html.begin());
	return 0;
}
