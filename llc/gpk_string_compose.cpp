#include "gpk_string_compose.h"

::gpk::error_t	gpk::appendOpenKey			(::gpk::achar & output, ::gpk::vcs key) {
	gpk_necs(gpk::appendQuoted(output, key));
	gpk_necs(output.push_back(':'));
	return key.size() + 3;
}

::gpk::error_t	gpk::appendNclosd			(::gpk::achar & output, ::gpk::vcs textToQuote, ::gpk::vcs openChars, ::gpk::vcs closeChars) {
	gpk_necs(output.append(openChars));
	gpk_necs(output.append(textToQuote));
	gpk_necs(output.append(closeChars));
	return textToQuote.size() + openChars.size() + closeChars.size();
}

::gpk::error_t	gpk::appendNclosd			(::gpk::achar & output, ::gpk::vcs textToQuote, char openChar, char closeChar) {
	gpk_necs(output.push_back(openChar));
	gpk_necs(output.append(textToQuote));
	gpk_necs(output.push_back(closeChar));
	return textToQuote.size() + 2;
}
::gpk::error_t	gpk::appendNclosd			(::gpk::achar & output, ::gpk::vcs textToQuote, char encloserChar) {
	gpk_necs(output.push_back(encloserChar));
	gpk_necs(output.append(textToQuote));
	gpk_necs(output.push_back(encloserChar));
	return textToQuote.size() + 2;
}
::gpk::error_t gpk::appendNclosdPrefixed	(::gpk::achar & output, ::gpk::vcs textToQuote, char prefix, char encloser)				{ 
	if(0 == prefix) 
		return ::gpk::appendNclosd(output, textToQuote, encloser);        
	gpk_necs(output.push_back(prefix));
	return 1 + ::gpk::appendNclosd(output, textToQuote, encloser); 
}
::gpk::error_t gpk::appendNclosdPrefixed	(::gpk::achar & output, ::gpk::vcs textToQuote, char prefix, char openCh, char closeCh) { 
	if(0 == prefix) 
		return ::gpk::appendNclosd(output, textToQuote, openCh, closeCh); 
	gpk_necs(output.push_back(prefix));
	return 1 + ::gpk::appendNclosd(output, textToQuote, openCh, closeCh); 
}
::gpk::error_t gpk::appendKeyValue			(::gpk::achar & output, ::gpk::vcs key, ::gpk::vcs value) { 
	gpk_necs(gpk::appendOpenKey(output, key));
	gpk_necs(output.append(value));
	return key.size() + value.size() + 3;
}
::gpk::error_t gpk::appendKeyString			(::gpk::achar & output, ::gpk::vcs key, ::gpk::vcs value) { 
	gpk_necs(gpk::appendOpenKey(output, key));
	gpk_necs(gpk::appendBraced(output, value));
	return key.size() + value.size() + 5;
}
::gpk::error_t gpk::appendKeyList			(::gpk::achar & output, ::gpk::vcs key, ::gpk::vcs listItems) { 
	gpk_necs(gpk::appendOpenKey(output, key));
	gpk_necs(gpk::appendBraced(output, listItems));
	return key.size() + listItems.size() + 5;
}
::gpk::error_t gpk::appendKeyObject			(::gpk::achar & output, ::gpk::vcs key, ::gpk::vcs listItems) { 
	gpk_necs(gpk::appendQuoted(output, key));
	gpk_necs(output.push_back(':'));
	gpk_necs(gpk::appendNclosd(output, listItems));
	return key.size() + listItems.size() + 5;
}
::gpk::error_t gpk::appendHtmlTagOpening	(::gpk::achar & output, ::gpk::vcs tagName, ::gpk::vcs tagAttributes) { 
	if(0 == tagAttributes.size()) 
		return ::gpk::appendNclosd(output, tagName, '<', '>'); 

	gpk_necs(output.push_back('<'));
	gpk_necs(output.append(tagName));
	gpk_necs(output.push_back(' '));
	gpk_necs(output.append(tagAttributes));
	gpk_necs(output.push_back('>'));
	return tagName.size() + tagAttributes.size() + 3;
}
::gpk::error_t gpk::appendHtmlTagVoid		(::gpk::achar & output, ::gpk::vcs tagName, ::gpk::vcs tagAttributes) { 
	if(0 == tagAttributes.size()) 
		return ::gpk::appendNclosd(output, tagName, "<", " />"); 

	gpk_necs(output.push_back('<'));
	gpk_necs(output.append(tagName));
	gpk_necs(output.push_back(' '));
	gpk_necs(output.append(tagAttributes));
	gpk_necs(output.append_string(" />"));
	return tagName.size() + tagAttributes.size() + 5;
}
::gpk::error_t	gpk::appendHtmlTag			(::gpk::achar & output, ::gpk::vcs tagName, ::gpk::vcs tagAttributes, ::gpk::vcs innerHtml) {
	int32_t result = 0;
	{ int32_t _result; gpk_necs(_result = gpk::appendHtmlTagOpening(output, tagName, tagAttributes)); result += _result; }
	{ int32_t _result; gpk_necs(_result = output.append(innerHtml)); result += _result; }
	{ int32_t _result; gpk_necs(_result = gpk::appendHtmlTagClosing(output, tagName)); result += _result; }
	return result;
}