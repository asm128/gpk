#include "gpk_append_html.h"

#ifndef GPK_ATMEL
gpk::err_t	gpk::appendHtmlScripts	(::gpk::asc_t & output, gpk::vcvcs filenames) { 
	::gpk::err_t 		result 				= 0;
	stxp	const char	FMT_ATTR_SRC_JS	[]	= "type=\"text/javascript\" src=\"/%s.js\"";
	for(uint32_t iFile = 0; iFile < filenames.size(); ++iFile) {
		char 				cooked	[128] 		= {};
		if_fail_e(::gpk::sprintf_s(cooked, FMT_ATTR_SRC_JS, filenames[iFile].begin()));
		result 		+= ::gpk::appendXmlTag(output, "script", cooked);
	}
	return result;
}
gpk::err_t	gpk::appendHtmlStyles	(::gpk::asc_t & output, gpk::vcvcs filenames) { 
	::gpk::err_t 		result 				= 0;
	stxp	const char	FMT_ATTR_HREF_CSS	[]	= "rel=\"stylesheet\" href=\"/%s.css\"";
	for(uint32_t iFile = 0; iFile < filenames.size(); ++iFile) {
		char 				cooked	[128] 		= {};
		if_fail_e(::gpk::sprintf_s(cooked, FMT_ATTR_HREF_CSS, filenames[iFile].begin()));
		result 		+= ::gpk::appendXmlTag(output, "link", cooked);
	}
	return result;
}
gpk::err_t	gpk::appendHtmlHead	(::gpk::asc_t & output, ::gpk::vcst_t title, ::gpk::vcvcs filesCSS, ::gpk::vcvcs filesJS) {
	return ::gpk::appendXmlTag(output, "head", vcs{}, [&output, title, filesCSS, filesJS]() { 
		return ::gpk::appendXmlTag		(output, "title", vcs{}, title)
			+  ::gpk::appendHtmlStyles	(output, filesCSS)
			+  ::gpk::appendHtmlScripts	(output, filesJS)
			; 
	});
}
gpk::err_t	gpk::appendHtmlPage	(::gpk::asc_t & output, const ::gpk::FAppend & funcAppendHead, const ::gpk::FAppend & funcAppendBody) {
	return ::gpk::appendXmlTag(output, "html", vcs{}, [&output, funcAppendHead, funcAppendBody]() {
		return ::gpk::appendXmlTag(output, "head", vcs{}, [&output, funcAppendHead]() { return funcAppendHead(output); })
			+  ::gpk::appendXmlTag(output, "body", vcs{}, [&output, funcAppendBody]() { return funcAppendBody(output); })
			;
	});
}
gpk::err_t	gpk::appendHtmlPage	(::gpk::asc_t & output, ::gpk::vcst_t title, ::gpk::vcvcs filesCSS, ::gpk::vcvcs filesJS, const ::gpk::FAppend & funcAppendBody, ::gpk::vcst_t postScript) {
	return ::gpk::appendXmlTag(output, "html", vcs{}, [&output, &title, &filesCSS, &filesJS, &funcAppendBody, &postScript]() {
		return ::gpk::appendHtmlHead(output, title, filesCSS, filesJS)
			+  ::gpk::appendXmlTag(output, "body", vcs{}, [&output, funcAppendBody]() { return funcAppendBody(output); })
			+  ((0 == postScript.size()) ? 0 : ::gpk::appendXmlTag(output, "script", vcs{}, postScript))
			;
	});
}
gpk::err_t	gpk::appendHtmlPage	(::gpk::asc_t & output, const ::gpk::FAppend & funcAppendCSS, const ::gpk::FAppend & funcAppendJS, const ::gpk::FAppend & funcAppendBody) {
	return ::gpk::appendXmlTag(output, "html", vcs{}, [&output, funcAppendBody, funcAppendCSS, funcAppendJS]() {
		return ::gpk::appendXmlTag(output, "head", vcs{}, [&output, funcAppendCSS, funcAppendJS]() {
			return ::gpk::appendXmlTag(output, "style" , vcs{}, [&output, funcAppendCSS]() { return funcAppendCSS ? funcAppendCSS(output) : 0; })
				+  ::gpk::appendXmlTag(output, "script", vcs{}, [&output, funcAppendJS ]() { return funcAppendJS  ? funcAppendJS (output) : 0; })
				;
		}) + ::gpk::appendXmlTag(output, "body", vcs{}, [&output, funcAppendBody]() { return funcAppendBody(output); });
	});
}
#endif
