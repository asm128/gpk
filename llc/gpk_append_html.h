#include "gpk_append_xml.h"

#ifndef GPK_APPEND_HTML_H
#define GPK_APPEND_HTML_H

namespace gpk
{
	stin	gpk::err_t	appendHtmlHead		(::gpk::asc_t & output, ::gpk::vcst_t tagAttributes, ::gpk::vcst_t innerHtml)	{ return ::gpk::appendXmlTag(output, "head", tagAttributes, innerHtml); }
	stin	gpk::err_t	appendHtmlBody		(::gpk::asc_t & output, ::gpk::vcst_t tagAttributes, ::gpk::vcst_t innerHtml)	{ return ::gpk::appendXmlTag(output, "body", tagAttributes, innerHtml); }
	stin	gpk::err_t	appendHtmlScript	(::gpk::asc_t & output, ::gpk::vcst_t tagAttributes, ::gpk::vcst_t innerHtml)	{ return ::gpk::appendXmlTag(output, "script", tagAttributes, innerHtml); }
	stin	gpk::err_t	appendHtmlTable		(::gpk::asc_t & output, ::gpk::vcst_t tagAttributes, ::gpk::vcst_t innerHtml)	{ return ::gpk::appendXmlTag(output, "table", tagAttributes, innerHtml); }
	stin	gpk::err_t	appendHtmlTableRow	(::gpk::asc_t & output, ::gpk::vcst_t tagAttributes, ::gpk::vcst_t innerHtml)	{ return ::gpk::appendXmlTag(output, "tr", tagAttributes, innerHtml); }
	stin	gpk::err_t	appendHtmlTableCol	(::gpk::asc_t & output, ::gpk::vcst_t tagAttributes, ::gpk::vcst_t innerHtml)	{ return ::gpk::appendXmlTag(output, "td", tagAttributes, innerHtml); }
	
	gpk::err_t			appendHtmlStyles	(::gpk::asc_t & output, gpk::vcvcs filenames);
	gpk::err_t			appendHtmlScripts	(::gpk::asc_t & output, gpk::vcvcs filenames);
	gpk::err_t			appendHtmlHead		(::gpk::asc_t & output, ::gpk::vcst_t title, ::gpk::vcvcs filesCSS, ::gpk::vcvcs filesJS);
	gpk::err_t			appendHtmlPage		(::gpk::asc_t & output, const ::gpk::FAppend & funcAppendHead, const ::gpk::FAppend & funcAppendBody);
	gpk::err_t			appendHtmlPage		(::gpk::asc_t & output, const ::gpk::FAppend & funcAppendCSS, const ::gpk::FAppend & funcAppendJS, const ::gpk::FAppend & funcAppendBody);
	gpk::err_t			appendHtmlPage		(::gpk::asc_t & output, ::gpk::vcst_t title, ::gpk::vcvcs filesCSS, ::gpk::vcvcs filesJS, const ::gpk::FAppend & funcAppendBody, ::gpk::vcst_t postScript = {});
} // namespace 

#endif // GPK_APPEND_HTML_H
