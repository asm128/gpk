#include "gpk_append_html.h"

#ifndef GPK_ATMEL
gpk::err_t	gpk::appendHtmlScripts	(::gpk::string & output, gpk::vcvcst_t filenames) {
	::gpk::err_t 		result 				= 0;
	for(::gpk::u2_t iFile = 0; iFile < filenames.size(); ++iFile) {
		::gpk::string		cooked			= {};
		if_fail_fe(::gpk::append_strings(cooked, "type=\"text/javascript\" src=\"/", filenames[iFile], ".js\""));
		result 		+= ::gpk::appendXmlTag(output, "script", cooked);
	}
	return result;
}
gpk::err_t	gpk::appendHtmlStyles	(::gpk::string & output, gpk::vcvcst_t filenames) {
	::gpk::err_t 		result 				= 0;
	for(::gpk::u2_t iFile = 0; iFile < filenames.size(); ++iFile) {
		::gpk::string		cooked			= {};
		if_fail_fe(::gpk::append_strings(cooked, "rel=\"stylesheet\" href=\"/", filenames[iFile], ".css\""));
		result 		+= ::gpk::appendXmlTag(output, "link", cooked);
	}
	return result;
}
gpk::err_t	gpk::appendHtmlHead	(::gpk::string & output, ::gpk::vcst_t title, ::gpk::vcvcst_t filesCSS, ::gpk::vcvcst_t filesJS) {
	return ::gpk::appendXmlTag(output, "head", vcst_t{}, [&output, title, filesCSS, filesJS]() { 
		return ::gpk::appendXmlTag		(output, "title", vcst_t{}, title)
			+  ::gpk::appendHtmlStyles	(output, filesCSS)
			+  ::gpk::appendHtmlScripts	(output, filesJS)
			; 
	});
}
gpk::err_t	gpk::appendHtmlPage	(::gpk::string & output, const ::gpk::FAppend & funcAppendHead, const ::gpk::FAppend & funcAppendBody) {
	return ::gpk::appendXmlTag(output, "html", vcst_t{}, [&output, funcAppendHead, funcAppendBody]() {
		return ::gpk::appendXmlTag(output, "head", vcst_t{}, [&output, funcAppendHead]() { return funcAppendHead(output); })
			+  ::gpk::appendXmlTag(output, "body", vcst_t{}, [&output, funcAppendBody]() { return funcAppendBody(output); })
			;
	});
}
gpk::err_t	gpk::appendHtmlPage	(::gpk::string & output, ::gpk::vcst_t title, ::gpk::vcvcst_t filesCSS, ::gpk::vcvcst_t filesJS, const ::gpk::FAppend & funcAppendBody, ::gpk::vcst_t postScript) {
	return ::gpk::appendXmlTag(output, "html", vcst_t{}, [&output, &title, &filesCSS, &filesJS, &funcAppendBody, &postScript]() {
		return ::gpk::appendHtmlHead(output, title, filesCSS, filesJS)
			+  ::gpk::appendXmlTag(output, "body", vcst_t{}, [&output, funcAppendBody]() { return funcAppendBody(output); })
			+  ((0 == postScript.size()) ? 0 : ::gpk::appendXmlTag(output, "script", vcst_t{}, postScript))
			;
	});
}
gpk::err_t	gpk::appendHtmlPage	(::gpk::string & output, const ::gpk::FAppend & funcAppendCSS, const ::gpk::FAppend & funcAppendJS, const ::gpk::FAppend & funcAppendBody) {
	return ::gpk::appendXmlTag(output, "html", vcst_t{}, [&output, funcAppendBody, funcAppendCSS, funcAppendJS]() {
		return ::gpk::appendXmlTag(output, "head", vcst_t{}, [&output, funcAppendCSS, funcAppendJS]() {
			return ::gpk::appendXmlTag(output, "style" , vcst_t{}, [&output, funcAppendCSS]() { return funcAppendCSS ? funcAppendCSS(output) : 0; })
				+  ::gpk::appendXmlTag(output, "script", vcst_t{}, [&output, funcAppendJS ]() { return funcAppendJS  ? funcAppendJS (output) : 0; })
				;
		}) + ::gpk::appendXmlTag(output, "body", vcst_t{}, [&output, funcAppendBody]() { return funcAppendBody(output); });
	});
}
#endif
