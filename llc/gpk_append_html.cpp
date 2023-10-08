#include "gpk_append_html.h"
#include "gpk_append_xml.h"

gpk::err_t	gpk::appendHtmlPage	(::gpk::achar & output, const ::gpk::FAppend & funcAppendCSS, const ::gpk::FAppend & funcAppendJS, const ::gpk::FAppend & funcAppendBody) {
	return ::gpk::appendXmlTag(output, "html", vcs{}, [&output, funcAppendBody, funcAppendCSS, funcAppendJS]() {
		return ::gpk::appendXmlTag(output, "head", vcs{}, [&output, funcAppendCSS, funcAppendJS]() {
			return ::gpk::appendXmlTag(output, "style" , vcs{}, [&output, funcAppendCSS]() { return funcAppendCSS ? funcAppendCSS(output) : 0; })
				+  ::gpk::appendXmlTag(output, "script", vcs{}, [&output, funcAppendJS ]() { return funcAppendJS  ? funcAppendJS (output) : 0; })
				;
		}) + ::gpk::appendXmlTag(output, "body", vcs{}, [&output, funcAppendBody]() { return funcAppendBody(output); });
	});
}

gpk::err_t	gpk::appendHtmlPage	(::gpk::achar & output, const ::gpk::FAppend & funcAppendBody) {
	return ::gpk::appendXmlTag(output, "html", vcs{}, [&output, funcAppendBody]() {
		return ::gpk::appendXmlTag(output, "head", vcs{}, [&output]() {
			return ::gpk::appendXmlTag(output, "link" , "rel=\"stylesheet\" href=\"/style/main.css\"")
				+  ::gpk::appendXmlTag(output, "script", "src=\"/script/main.js\"")
				;
		}) + ::gpk::appendXmlTag(output, "body", vcs{}, [&output, funcAppendBody]() { return funcAppendBody(output); });
	});
}

gpk::err_t	gpk::appendHtmlHead		(::gpk::achar & output, ::gpk::vcs tagAttributes, ::gpk::vcs innerHtml)	{ return ::gpk::appendXmlTag(output, "head", tagAttributes, innerHtml); }
gpk::err_t	gpk::appendHtmlBody		(::gpk::achar & output, ::gpk::vcs tagAttributes, ::gpk::vcs innerHtml)	{ return ::gpk::appendXmlTag(output, "body", tagAttributes, innerHtml); }
gpk::err_t	gpk::appendHtmlScript	(::gpk::achar & output, ::gpk::vcs tagAttributes, ::gpk::vcs innerHtml)	{ return ::gpk::appendXmlTag(output, "script", tagAttributes, innerHtml); }
gpk::err_t	gpk::appendHtmlTable	(::gpk::achar & output, ::gpk::vcs tagAttributes, ::gpk::vcs innerHtml)	{ return ::gpk::appendXmlTag(output, "table", tagAttributes, innerHtml); }
gpk::err_t	gpk::appendHtmlTableRow	(::gpk::achar & output, ::gpk::vcs tagAttributes, ::gpk::vcs innerHtml)	{ return ::gpk::appendXmlTag(output, "tr", tagAttributes, innerHtml); }
gpk::err_t	gpk::appendHtmlTableCol	(::gpk::achar & output, ::gpk::vcs tagAttributes, ::gpk::vcs innerHtml)	{ return ::gpk::appendXmlTag(output, "td", tagAttributes, innerHtml); }
