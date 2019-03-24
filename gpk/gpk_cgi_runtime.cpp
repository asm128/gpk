#include "gpk_cgi_runtime.h"
#include <string>

static	::gpk::error_t								load_env	(const ::gpk::view_const_string& key, ::gpk::array_pod<char> & queryString)	{
	char													* qs				= {};
	size_t													ql					= 0;
	_dupenv_s(&qs, &ql, key.begin());
	if(0 == qs) {
		queryString.resize(0);
		return -1;
	}
	queryString.resize((uint32_t)ql + 1);
	memset(queryString.begin(), 0, queryString.size());
	memcpy(queryString.begin(), qs, ql);
	//queryString.resize(ql);
	queryString.resize((uint32_t)ql - ((0 == qs[ql - 1]) ? 1 : 0));
	free(qs);
	return 0;
}

::gpk::error_t										processKeyVal					(::gpk::SCGIFramework& webRuntimeValues, const ::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>& keyVal)	{ 
	if(0 == keyVal.Key.size())
		return -1;
	try {
			 if(keyVal.Key.size() == (::gpk::size("width"	) - 1) && 0 == memcmp("width"	, keyVal.Key.begin(), ::gpk::size("width"	) - 1)) webRuntimeValues.TargetSize.x = (uint16_t)::std::stoi(::std::string{keyVal.Val.begin(), keyVal.Val.size()});
		else if(keyVal.Key.size() == (::gpk::size("height"	) - 1) && 0 == memcmp("height"	, keyVal.Key.begin(), ::gpk::size("height"	) - 1)) webRuntimeValues.TargetSize.y = (uint16_t)::std::stoi(::std::string{keyVal.Val.begin(), keyVal.Val.size()});
	}
	catch(...){
		webRuntimeValues.TargetSize							= {123, 456};
		return -1;
	}
	try {
		if(keyVal.Val.size() && keyVal.Key.size() == (::gpk::size("bt") - 1) && 0 == memcmp("bt", keyVal.Key.begin(), ::gpk::size("bt") - 1)) if(1 == (uint16_t)::std::stoi(::std::string{keyVal.Val.begin(), keyVal.Val.size()})) webRuntimeValues.Bootstrapped = true;
	}
	catch(...){
		webRuntimeValues.Bootstrapped						= false;
		return -1;
	}
	if(keyVal.Val.size() && keyVal.Key.size() == (::gpk::size("m") - 1) && 0 == memcmp("m", keyVal.Key.begin(), ::gpk::size("m") - 1)) 
		webRuntimeValues.ModuleName							= keyVal.Val;
	return 0;
}

::gpk::error_t										gpk::cgiRuntimeValuesLoad		(::gpk::SCGIFramework & framework)	{ 
	::gpk::SCGIRuntimeValues								& webRuntimeValues				= framework.RuntimeValues;
	::load_env("QUERY_STRING"	, webRuntimeValues.QueryString		); 
	//webRuntimeValues.QueryString = "bt=1&width=100&height=200&m=test_cgi_module";
	::gpk::querystring_split({webRuntimeValues.QueryString.begin(), webRuntimeValues.QueryString.size()}, webRuntimeValues.QueryStringElements);
	webRuntimeValues.QueryStringKeyVals.resize(webRuntimeValues.QueryStringElements.size());
	for(uint32_t iKeyVal = 0; iKeyVal < webRuntimeValues.QueryStringKeyVals.size(); ++iKeyVal) {
		::gpk::SKeyVal<::gpk::view_const_string, ::gpk::view_const_string>	& keyValDst				= webRuntimeValues.QueryStringKeyVals[iKeyVal];
		::gpk::keyval_split(webRuntimeValues.QueryStringElements[iKeyVal], keyValDst);
		::processKeyVal(framework, webRuntimeValues.QueryStringKeyVals[iKeyVal]);
	}
	::load_env("CONTENT_TYPE"	, webRuntimeValues.ContentType		); 
	::load_env("CONTENT_LENGTH"	, webRuntimeValues.ContentLength	); 
	return 0;
}
