#include "gpk_append_xml.h"

#ifndef GPK_APPEND_HTML_H
#define GPK_APPEND_HTML_H

namespace gpk
{
	stin	err_t	appendHtmlHead		(asc_t & output, vcst_t tagAttributes, vcst_t innerHtml)	{ return appendXmlTag(output, "head", tagAttributes, innerHtml); }
	stin	err_t	appendHtmlBody		(asc_t & output, vcst_t tagAttributes, vcst_t innerHtml)	{ return appendXmlTag(output, "body", tagAttributes, innerHtml); }
	stin	err_t	appendHtmlScript	(asc_t & output, vcst_t tagAttributes, vcst_t innerHtml)	{ return appendXmlTag(output, "script", tagAttributes, innerHtml); }
	stin	err_t	appendHtmlTable		(asc_t & output, vcst_t tagAttributes, vcst_t innerHtml)	{ return appendXmlTag(output, "table", tagAttributes, innerHtml); }
	stin	err_t	appendHtmlTableRow	(asc_t & output, vcst_t tagAttributes, vcst_t innerHtml)	{ return appendXmlTag(output, "tr", tagAttributes, innerHtml); }
	stin	err_t	appendHtmlTableCol	(asc_t & output, vcst_t tagAttributes, vcst_t innerHtml)	{ return appendXmlTag(output, "td", tagAttributes, innerHtml); }
	
	err_t			appendHtmlStyles	(asc_t & output, view<vcst_c> filenames);
	err_t			appendHtmlScripts	(asc_t & output, view<vcst_c> filenames);
	err_t			appendHtmlHead		(asc_t & output, vcst_t title, view<vcst_t> filesCSS, view<vcst_t> filesJS);
	err_t			appendHtmlPage		(asc_t & output, const FAppend & funcAppendHead, const FAppend & funcAppendBody);
	err_t			appendHtmlPage		(asc_t & output, const FAppend & funcAppendCSS, const FAppend & funcAppendJS, const FAppend & funcAppendBody);
	err_t			appendHtmlPage		(asc_t & output, vcst_t title, view<vcst_t> filesCSS, view<vcst_t> filesJS, const FAppend & funcAppendBody, vcst_t postScript = {});
} // namespace 

#endif // GPK_APPEND_HTML_H
