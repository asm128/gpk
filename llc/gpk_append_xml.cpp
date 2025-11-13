#include "gpk_append_xml.h"

gpk::err_t	gpk::appendXmlOpenTag		(::gpk::asc_t & output, ::gpk::vcst_t tagName) 							{ return gpk::append_strings(output, '<', tagName); }
gpk::err_t	gpk::appendXmlTagClosing	(::gpk::asc_t & output, ::gpk::vcst_t tagName) 							{ return gpk::append_strings(output, "</", tagName, '>'); }
gpk::err_t	gpk::appendXmlTagOpening	(::gpk::asc_t & output, ::gpk::vcst_t tagName, ::gpk::vcst_t tagAttributes)	{
	return tagAttributes.size()
		? ::gpk::append_strings(output, '<', tagName, ' ', tagAttributes, " >")
		: ::gpk::append_strings(output, '<', tagName, '>')
		;
}
gpk::err_t	gpk::appendXmlTagVoid		(::gpk::asc_t & output, ::gpk::vcst_t tagName, ::gpk::vcst_t tagAttributes) {
	return tagAttributes.size()
		? ::gpk::append_strings(output, '<', tagName, ' ', tagAttributes, " >")
		: ::gpk::append_strings(output, '<', tagName, " />")
		;
}
gpk::err_t	gpk::appendXmlTag			(::gpk::asc_t & output, ::gpk::vcst_t tagName, ::gpk::vcst_t tagAttributes, ::gpk::vcst_t innerHtml) {
	return tagAttributes.size()
		? ::gpk::append_strings(output, '<', tagName, ' ', tagAttributes, " >", innerHtml, "</", tagName, '>')
		: ::gpk::append_strings(output, '<', tagName, '>', innerHtml, "</", tagName, '>')
		;
}
gpk::err_t	gpk::appendXmlAttr			(::gpk::asc_t & output, ::gpk::vcst_t attrName, ::gpk::vcst_t attrValue)	{
	return attrValue.size()
		? ::gpk::append_strings(output, " ", attrName, "=\"", attrValue, '"')
		: ::gpk::append_strings(output, " ", attrName)
		;
}
gpk::err_t	gpk::appendXmlTag			(::gpk::asc_t & output, ::gpk::vcst_t tagName, ::gpk::vcst_t tagAttributes, const ::gpk::FAppend & funcAppend) {
	int32_t			result						= 0;

					{ int32_t _result; gpk_necs(_result = gpk::appendXmlTagOpening(output, tagName, tagAttributes)); 	result += _result; }
	if(funcAppend)	{ int32_t _result; gpk_necs(_result = funcAppend(output));											result += _result; }
					{ int32_t _result; gpk_necs(_result = gpk::appendXmlTagClosing(output, tagName)); 					result += _result; }
	return result;
}
gpk::err_t	gpk::appendXmlTag			(::gpk::asc_t & output, ::gpk::vcst_t tagName, ::gpk::vcst_t tagAttributes, const ::gpk::function<gpk::err_t()> & funcAppend) {
	int32_t			result						= 0;

					{ int32_t _result; gpk_necs(_result = gpk::appendXmlTagOpening(output, tagName, tagAttributes)); 	result += _result; }
	if(funcAppend)	{ int32_t _result; gpk_necs(_result = funcAppend());												result += _result; }
					{ int32_t _result; gpk_necs(_result = gpk::appendXmlTagClosing(output, tagName)); 					result += _result; }
	return result;
}
gpk::err_t	gpk::appendXmlTag			(::gpk::asc_t & output, ::gpk::vcst_t tagName, const ::gpk::function<gpk::err_t()> & funcAppendAttrs, const ::gpk::function<gpk::err_t()> & funcAppend) {
	int32_t			result						= 0;
	if(!funcAppendAttrs)
		return ::gpk::appendXmlTag(output, tagName, ::gpk::vcst_t{}, funcAppend);

					{ int32_t _result; gpk_necs(_result = gpk::append_strings(output, '<', tagName, ' '));	result += _result; }
					{ int32_t _result; gpk_necs(_result = funcAppendAttrs());								result += _result; }
					{ int32_t _result; gpk_necs(_result = output.append_string(" >"));						result += _result; }
	if(funcAppend)	{ int32_t _result; gpk_necs(_result = funcAppend());									result += _result; }
					{ int32_t _result; gpk_necs(_result = gpk::appendXmlTagClosing(output, tagName));		result += _result; }
	return result;
}
