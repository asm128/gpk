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