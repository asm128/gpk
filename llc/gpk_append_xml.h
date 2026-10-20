#include "gpk_string_compose.h"

#ifndef GPK_APPEND_XML_H
#define GPK_APPEND_XML_H

namespace gpk
{
	err_t	appendXmlAttr		(string & output, vcst_t attrName, vcst_t attrValue = {});
	err_t	appendXmlOpenTag	(string & output, vcst_t tagName);
	err_t	appendXmlTagVoid	(string & output, vcst_t tagName, vcst_t tagAttributes = {});
	err_t	appendXmlTagOpening	(string & output, vcst_t tagName, vcst_t tagAttributes = {});
	err_t	appendXmlTagClosing	(string & output, vcst_t tagName);
	err_t	appendXmlTag		(string & output, vcst_t tagName, vcst_t tagAttributes = {}, vcst_t innerHtml = {});
	err_t	appendXmlTag		(string & output, vcst_t tagName, vcst_t tagAttributes, cnst FAppend & funcAppend);
	err_t	appendXmlTag		(string & output, vcst_t tagName, vcst_t tagAttributes, cnst function<err_t()> & funcAppend);
	err_t	appendXmlTag		(string & output, vcst_t tagName, cnst function<err_t()> & funcAppendAttributes, const function<err_t()> & funcAppend);

} // namespace 

#endif // GPK_APPEND_XML_H


