#include "gpk_append_xml.h"

#ifndef GPK_APPEND_HTML_H
#define GPK_APPEND_HTML_H

namespace gpk
{
	stin	err_t	appendHtmlHead		(string & output, vcst_t tagAttributes, vcst_t innerHtml)	{ return appendXmlTag(output, "head", tagAttributes, innerHtml); }
	stin	err_t	appendHtmlBody		(string & output, vcst_t tagAttributes, vcst_t innerHtml)	{ return appendXmlTag(output, "body", tagAttributes, innerHtml); }
	stin	err_t	appendHtmlScript	(string & output, vcst_t tagAttributes, vcst_t innerHtml)	{ return appendXmlTag(output, "script", tagAttributes, innerHtml); }
	stin	err_t	appendHtmlTable		(string & output, vcst_t tagAttributes, vcst_t innerHtml)	{ return appendXmlTag(output, "table", tagAttributes, innerHtml); }
	stin	err_t	appendHtmlTableRow	(string & output, vcst_t tagAttributes, vcst_t innerHtml)	{ return appendXmlTag(output, "tr", tagAttributes, innerHtml); }
	stin	err_t	appendHtmlTableCol	(string & output, vcst_t tagAttributes, vcst_t innerHtml)	{ return appendXmlTag(output, "td", tagAttributes, innerHtml); }
	
	err_t			appendHtmlStyles	(string & output, vcvcst_t filenames);
	err_t			appendHtmlScripts	(string & output, vcvcst_t filenames);
	err_t			appendHtmlHead		(string & output, vcst_t title, vcvcst_t filesCSS, vcvcst_t filesJS);
	err_t			appendHtmlPage		(string & output, const FAppend & funcAppendHead, const FAppend & funcAppendBody);
	err_t			appendHtmlPage		(string & output, const FAppend & funcAppendCSS, const FAppend & funcAppendJS, const FAppend & funcAppendBody);
	err_t			appendHtmlPage		(string & output, vcst_t title, vcvcst_t filesCSS, vcvcst_t filesJS, const FAppend & funcAppendBody, vcst_t postScript = {});
} // namespace 

#endif // GPK_APPEND_HTML_H
