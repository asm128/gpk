#include "gpk_string_compose.h"

// -*- Generic string enclosing -*-
gpk::err_t	gpk::appendNclosd			(::gpk::string & output, ::gpk::vcst_t textToEnclose) 											{ return ::gpk::append_strings(output, '{', textToEnclose,'}'); }
gpk::err_t	gpk::appendBraced			(::gpk::string & output, ::gpk::vcst_t textToEnclose) 											{ return ::gpk::append_strings(output, '[', textToEnclose,']'); }
gpk::err_t	gpk::appendQuoted			(::gpk::string & output, ::gpk::vcst_t textToEnclose) 											{ return ::gpk::append_strings(output, '"', textToEnclose,'"'); }
gpk::err_t	gpk::appendGtlted			(::gpk::string & output, ::gpk::vcst_t textToEnclose) 											{ return ::gpk::append_strings(output, '<', textToEnclose,'>'); }
gpk::err_t	gpk::appendNclosd			(::gpk::string & output, ::gpk::vcst_t textToQuote, ::gpk::vcst_t openChars, ::gpk::vcst_t closeChars)	{ return ::gpk::append_strings(output, openChars, textToQuote, closeChars); }
gpk::err_t	gpk::appendNclosd			(::gpk::string & output, ::gpk::vcst_t textToQuote, sc_t openChar, sc_t closeChar)					{ return ::gpk::append_strings(output, openChar, textToQuote, closeChar); }
gpk::err_t	gpk::appendNclosd			(::gpk::string & output, ::gpk::vcst_t textToQuote, sc_t encloserChar)							{ return ::gpk::append_strings(output, encloserChar, textToQuote, encloserChar); }
gpk::err_t	gpk::appendNclosdPrefixed	(::gpk::string & output, ::gpk::vcst_t textToQuote, sc_t prefix, sc_t encloser) 					{ return ::gpk::append_strings(output, prefix, encloser, textToQuote, encloser); }
gpk::err_t	gpk::appendNclosdPrefixed	(::gpk::string & output, ::gpk::vcst_t textToQuote, sc_t prefix, sc_t openCh, sc_t closeCh) 		{ return ::gpk::append_strings(output, prefix, openCh, textToQuote, closeCh); }
gpk::err_t	gpk::appendNclosdPrefixed	(::gpk::string & output, ::gpk::vcst_t textToEnclose, b8_t separate, sc_t separator)			{ return separate ? ::gpk::appendNclosdPrefixed(output, textToEnclose, separator, '{', '}') : ::gpk::appendNclosd(output, textToEnclose); }
gpk::err_t	gpk::appendBracedPrefixed	(::gpk::string & output, ::gpk::vcst_t textToEnclose, b8_t separate, sc_t separator)			{ return separate ? ::gpk::appendNclosdPrefixed(output, textToEnclose, separator, '[', ']') : ::gpk::appendBraced(output, textToEnclose); }
gpk::err_t	gpk::appendQuotedPrefixed	(::gpk::string & output, ::gpk::vcst_t textToEnclose, b8_t separate, sc_t separator)			{ return separate ? ::gpk::appendNclosdPrefixed(output, textToEnclose, separator, '"')		: ::gpk::appendQuoted(output, textToEnclose); }
gpk::err_t	gpk::appendGtltedPrefixed	(::gpk::string & output, ::gpk::vcst_t textToEnclose, b8_t separate, sc_t separator)			{ return separate ? ::gpk::appendNclosdPrefixed(output, textToEnclose, separator, '<', '>') : ::gpk::appendGtlted(output, textToEnclose); }
