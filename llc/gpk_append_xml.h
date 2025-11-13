#include "gpk_string_compose.h"

#ifndef GPK_APPEND_XML_H
#define GPK_APPEND_XML_H

namespace gpk
{
	gpk::err_t	appendXmlAttr		(::gpk::asc_t & output, ::gpk::vcst_t attrName, ::gpk::vcst_t attrValue = {});
	gpk::err_t	appendXmlOpenTag	(::gpk::asc_t & output, ::gpk::vcst_t tagName);
	gpk::err_t	appendXmlTagVoid	(::gpk::asc_t & output, ::gpk::vcst_t tagName, ::gpk::vcst_t tagAttributes = {});
	gpk::err_t	appendXmlTagOpening	(::gpk::asc_t & output, ::gpk::vcst_t tagName, ::gpk::vcst_t tagAttributes = {});
	gpk::err_t	appendXmlTagClosing	(::gpk::asc_t & output, ::gpk::vcst_t tagName);
	gpk::err_t	appendXmlTag		(::gpk::asc_t & output, ::gpk::vcst_t tagName, ::gpk::vcst_t tagAttributes = {}, ::gpk::vcst_t innerHtml = {});
	gpk::err_t	appendXmlTag		(::gpk::asc_t & output, ::gpk::vcst_t tagName, ::gpk::vcst_t tagAttributes, const FAppend & funcAppend);
	gpk::err_t	appendXmlTag		(::gpk::asc_t & output, ::gpk::vcst_t tagName, ::gpk::vcst_t tagAttributes, const ::gpk::function<gpk::err_t()> & funcAppend);
	gpk::err_t	appendXmlTag		(::gpk::asc_t & output, ::gpk::vcst_t tagName, const ::gpk::function<gpk::err_t()> & funcAppendAttributes, const ::gpk::function<gpk::err_t()> & funcAppend);

} // namespace 

#endif // GPK_APPEND_XML_H