#include "gpk_string_compose.h"

#ifndef GPK_APPEND_XML_H
#define GPK_APPEND_XML_H

namespace gpk
{
	gpk::err_t	appendXmlAttr		(::gpk::achar & output, ::gpk::vcs attrName, ::gpk::vcs attrValue = {});
	gpk::err_t	appendXmlOpenTag	(::gpk::achar & output, ::gpk::vcs tagName);
	gpk::err_t	appendXmlTagVoid	(::gpk::achar & output, ::gpk::vcs tagName, ::gpk::vcs tagAttributes = {});
	gpk::err_t	appendXmlTagOpening	(::gpk::achar & output, ::gpk::vcs tagName, ::gpk::vcs tagAttributes = {});
	gpk::err_t	appendXmlTagClosing	(::gpk::achar & output, ::gpk::vcs tagName);
	gpk::err_t	appendXmlTag		(::gpk::achar & output, ::gpk::vcs tagName, ::gpk::vcs tagAttributes = {}, ::gpk::vcs innerHtml = {});
	gpk::err_t	appendXmlTag		(::gpk::achar & output, ::gpk::vcs tagName, ::gpk::vcs tagAttributes, const FAppend & funcAppend);
	gpk::err_t	appendXmlTag		(::gpk::achar & output, ::gpk::vcs tagName, ::gpk::vcs tagAttributes, const ::gpk::function<gpk::err_t()> & funcAppend);
	gpk::err_t	appendXmlTag		(::gpk::achar & output, ::gpk::vcs tagName, const ::gpk::function<gpk::err_t()> & funcAppendAttributes, const ::gpk::function<gpk::err_t()> & funcAppend);

} // namespace 

#endif // GPK_APPEND_XML_H