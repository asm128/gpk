#include "gpk_append_xml.h"

gpk::err_t	gpk::appendXmlOpenTag		(::gpk::achar & output, ::gpk::vcs tagName) 							{ return gpk::append_strings(output, '<', tagName); }
gpk::err_t	gpk::appendXmlTagClosing	(::gpk::achar & output, ::gpk::vcs tagName) 							{ return gpk::append_strings(output, "</", tagName, '>'); }
gpk::err_t	gpk::appendXmlTagOpening	(::gpk::achar & output, ::gpk::vcs tagName, ::gpk::vcs tagAttributes)	{
	return tagAttributes.size()
		? ::gpk::append_strings(output, '<', tagName, ' ', tagAttributes, " >")
		: ::gpk::append_strings(output, '<', tagName, '>')
		;
}
gpk::err_t	gpk::appendXmlTagVoid		(::gpk::achar & output, ::gpk::vcs tagName, ::gpk::vcs tagAttributes) {
	return tagAttributes.size()
		? ::gpk::append_strings(output, '<', tagName, ' ', tagAttributes, " >")
		: ::gpk::append_strings(output, '<', tagName, " />")
		;
}
gpk::err_t	gpk::appendXmlTag			(::gpk::achar & output, ::gpk::vcs tagName, ::gpk::vcs tagAttributes, ::gpk::vcs innerHtml) {
	return tagAttributes.size()
		? ::gpk::append_strings(output, '<', tagName, ' ', tagAttributes, " >", innerHtml, "</", tagName, '>')
		: ::gpk::append_strings(output, '<', tagName, '>', innerHtml, "</", tagName, '>')
		;
}
gpk::err_t	gpk::appendXmlAttr			(::gpk::achar & output, ::gpk::vcs attrName, ::gpk::vcs attrValue)	{
	return attrValue.size()
		? ::gpk::append_strings(output, " ", attrName, "=\"", attrValue, '"')
		: ::gpk::append_strings(output, " ", attrName)
		;
}
gpk::err_t	gpk::appendXmlTag			(::gpk::achar & output, ::gpk::vcs tagName, ::gpk::vcs tagAttributes, const ::gpk::FAppend & funcAppend) {
	int32_t			result						= 0;

					{ int32_t _result; gpk_necs(_result = gpk::appendXmlTagOpening(output, tagName, tagAttributes)); 	result += _result; }
	if(funcAppend)	{ int32_t _result; gpk_necs(_result = funcAppend(output));											result += _result; }
					{ int32_t _result; gpk_necs(_result = gpk::appendXmlTagClosing(output, tagName)); 					result += _result; }
	return result;
}
gpk::err_t	gpk::appendXmlTag			(::gpk::achar & output, ::gpk::vcs tagName, ::gpk::vcs tagAttributes, const ::gpk::function<gpk::err_t()> & funcAppend) {
	int32_t			result						= 0;

					{ int32_t _result; gpk_necs(_result = gpk::appendXmlTagOpening(output, tagName, tagAttributes)); 	result += _result; }
	if(funcAppend)	{ int32_t _result; gpk_necs(_result = funcAppend());												result += _result; }
					{ int32_t _result; gpk_necs(_result = gpk::appendXmlTagClosing(output, tagName)); 					result += _result; }
	return result;
}
gpk::err_t	gpk::appendXmlTag			(::gpk::achar & output, ::gpk::vcs tagName, const ::gpk::function<gpk::err_t()> & funcAppendAttrs, const ::gpk::function<gpk::err_t()> & funcAppend) {
	int32_t			result						= 0;
	if(!funcAppendAttrs)
		return ::gpk::appendXmlTag(output, tagName, ::gpk::vcs{}, funcAppend);

					{ int32_t _result; gpk_necs(_result = gpk::append_strings(output, '<', tagName, ' '));	result += _result; }
					{ int32_t _result; gpk_necs(_result = funcAppendAttrs());								result += _result; }
					{ int32_t _result; gpk_necs(_result = output.append_string(" >"));						result += _result; }
	if(funcAppend)	{ int32_t _result; gpk_necs(_result = funcAppend());									result += _result; }
					{ int32_t _result; gpk_necs(_result = gpk::appendXmlTagClosing(output, tagName));		result += _result; }
	return result;
}
