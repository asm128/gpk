#include "blitter.h"

#include "gpk_cgi_runtime.h"

GPK_USING_TYPEINT();

GPK_OS_ENTRY_POINT(::gpk::cgiMain);

::gpk::err_t			jsonFail						(::gpk::string & output, ::gpk::vcst_t textToLog, ::gpk::vcst_t status)	{
	error_printf("%.*s", GPK_VIEW_ARGS(textToLog));
	output											= ::gpk::vcst_t{"Status: "};
	if_fail_fef(output.append(status)						, "%s", "Failed to generate output message.");
	if_fail_fef(output.append(::gpk::vcst_t{"\r\nContent-Type: application/json\r\n\r\n"}), "%s", "Failed to generate output message.");
	if_fail_fef(output.append(::gpk::vcst_t{"{ \"status\":"})		, "%s", "Failed to generate output message.");
	if_fail_fef(output.append(status)						, "%s", "Failed to generate output message.");
	if_fail_fef(output.append(::gpk::vcst_t{", \"description\" :\""}), "%s", "Failed to generate output message.");
	if_fail_fef(output.append(textToLog)						, "%s", "Failed to generate output message.");
	if_fail_fef(output.append(::gpk::vcst_t{"\" }\r\n"})				, "%s", "Failed to generate output message.");
	return 0;
}

::gpk::err_t			gpk_cgi_generate_output			(::gpk::SCGIRuntimeValues & runtimeValues, ::gpk::string & output)	{
	::gpk::SHTTPAPIRequest								requestReceived					= {};
	bool												isCGIEnviron					= ::gpk::httpRequestInit(requestReceived, runtimeValues, true);
	::gpk::array_obj<::gpk::keyval<::gpk::vcst_t>>			environViews;
	if_fail_fef(environViews.append(runtimeValues.RuntimeValues.EntryPointArgs.Environment), "%s", "If this breaks, we better know ASAP.");
	if (isCGIEnviron) {
		if_fail_fe(output.append(::gpk::vcst_t{"Content-type: application/json\r\n"}));
		if_fail_fe(output.append(::gpk::vcst_t{"\r\n"}));
	}

	const ::gpk::vcst_t						configFileName					= "itwapi.json";
	::blt::SBlitter										app								= {};
	if_fail_fef(::blt::requestProcess(app.ExpressionReader, app.Query, requestReceived, app.ExpansionKeyStorage), "%s", "Failed to process request.");
	if_fail_fef(::blt::loadConfig(app, "./blitter.json"), "%s", "Failed to load blitter configuration.");
	const s3_t										result							= ::blt::queryProcess(app.LoadCache, app.Databases, app.ExpressionReader, app.Query, app.Folder, output);
	if_true_fef(0 > result, "%s", "Failed to load blitter databases.");
	if(output.size()) {
		OutputDebugStringA(output.begin());
		OutputDebugStringA("\n");
	}
	return 0;
}
