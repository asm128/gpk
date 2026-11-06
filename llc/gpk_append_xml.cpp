#include "gpk_append_xml.h"

gpk::err_t	gpk::appendXmlOpenTag		(string & output, vcst_t tagName) 							{ return append_strings(output, '<', tagName); }
gpk::err_t	gpk::appendXmlTagClosing	(string & output, vcst_t tagName) 							{ return append_strings(output, "</", tagName, '>'); }
gpk::err_t	gpk::appendXmlTagOpening	(string & output, vcst_t tagName, vcst_t tagAttributes)	{
	return tagAttributes.size()
		? append_strings(output, '<', tagName, ' ', tagAttributes, " >")
		: append_strings(output, '<', tagName, '>')
		;
}
gpk::err_t	gpk::appendXmlTagVoid		(string & output, vcst_t tagName, vcst_t tagAttributes) {
	return tagAttributes.size()
		? append_strings(output, '<', tagName, ' ', tagAttributes, " >")
		: append_strings(output, '<', tagName, " />")
		;
}
gpk::err_t	gpk::appendXmlTag			(string & output, vcst_t tagName, vcst_t tagAttributes, vcst_t innerHtml) {
	return tagAttributes.size()
		? append_strings(output, '<', tagName, ' ', tagAttributes, " >", innerHtml, "</", tagName, '>')
		: append_strings(output, '<', tagName, '>', innerHtml, "</", tagName, '>')
		;
}
gpk::err_t	gpk::appendXmlAttr			(string & output, vcst_t attrName, vcst_t attrValue)	{
	return attrValue.size()
		? append_strings(output, " ", attrName, "=\"", attrValue, '"')
		: append_strings(output, " ", attrName)
		;
}
gpk::err_t	gpk::appendXmlTag			(string & output, vcst_t tagName, vcst_t tagAttributes, const FAppend & funcAppend) {
	s2_t			result						= 0;

					{ s2_t _result; if_fail_fe(_result = appendXmlTagOpening(output, tagName, tagAttributes)); 	result += _result; }
	if(funcAppend)	{ s2_t _result; if_fail_fe(_result = funcAppend(output));											result += _result; }
					{ s2_t _result; if_fail_fe(_result = appendXmlTagClosing(output, tagName)); 					result += _result; }
	return result;
}
gpk::err_t	gpk::appendXmlTag			(string & output, vcst_t tagName, vcst_t tagAttributes, const function<gpk::err_t()> & funcAppend) {
	s2_t			result						= 0;

					{ s2_t _result; if_fail_fe(_result = appendXmlTagOpening(output, tagName, tagAttributes)); 	result += _result; }
	if(funcAppend)	{ s2_t _result; if_fail_fe(_result = funcAppend());											result += _result; }
					{ s2_t _result; if_fail_fe(_result = appendXmlTagClosing(output, tagName)); 				result += _result; }
	return result;
}
gpk::err_t	gpk::appendXmlTag			(string & output, vcst_t tagName, const function<gpk::err_t()> & funcAppendAttrs, const function<gpk::err_t()> & funcAppend) {
	s2_t			result						= 0;
	if(!funcAppendAttrs)
		return appendXmlTag(output, tagName, vcst_t{}, funcAppend);

					{ s2_t _result; if_fail_fe(_result = append_strings(output, '<', tagName, ' '));	result += _result; }
					{ s2_t _result; if_fail_fe(_result = funcAppendAttrs());							result += _result; }
					{ s2_t _result; if_fail_fe(_result = append_strings(output, " >"));					result += _result; }
	if(funcAppend)	{ s2_t _result; if_fail_fe(_result = funcAppend());									result += _result; }
					{ s2_t _result; if_fail_fe(_result = appendXmlTagClosing(output, tagName));			result += _result; }
	return result;
}
