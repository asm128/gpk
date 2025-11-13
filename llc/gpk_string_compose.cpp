#include "gpk_string_compose.h"

// -*- Generic string enclosing -*-
gpk::err_t	gpk::appendNclosd			(::gpk::asc_t & output, ::gpk::vcst_t textToEnclose) 												{ return ::gpk::append_strings(output, '{', textToEnclose,'}'); }
gpk::err_t	gpk::appendBraced			(::gpk::asc_t & output, ::gpk::vcst_t textToEnclose) 												{ return ::gpk::append_strings(output, '[', textToEnclose,']'); }
gpk::err_t	gpk::appendQuoted			(::gpk::asc_t & output, ::gpk::vcst_t textToEnclose) 												{ return ::gpk::append_strings(output, '"', textToEnclose,'"'); }
gpk::err_t	gpk::appendGtlted			(::gpk::asc_t & output, ::gpk::vcst_t textToEnclose) 												{ return ::gpk::append_strings(output, '<', textToEnclose,'>'); }
gpk::err_t	gpk::appendNclosd			(::gpk::asc_t & output, ::gpk::vcst_t textToQuote, ::gpk::vcst_t openChars, ::gpk::vcst_t closeChars)	{ return ::gpk::append_strings(output, openChars, textToQuote, closeChars); }
gpk::err_t	gpk::appendNclosd			(::gpk::asc_t & output, ::gpk::vcst_t textToQuote, char openChar, char closeChar)					{ return ::gpk::append_strings(output, openChar, textToQuote, closeChar); }
gpk::err_t	gpk::appendNclosd			(::gpk::asc_t & output, ::gpk::vcst_t textToQuote, char encloserChar)								{ return ::gpk::append_strings(output, encloserChar, textToQuote, encloserChar); }
gpk::err_t	gpk::appendNclosdPrefixed	(::gpk::asc_t & output, ::gpk::vcst_t textToQuote, char prefix, char encloser) 					{ return ::gpk::append_strings(output, prefix, encloser, textToQuote, encloser); }
gpk::err_t	gpk::appendNclosdPrefixed	(::gpk::asc_t & output, ::gpk::vcst_t textToQuote, char prefix, char openCh, char closeCh) 		{ return ::gpk::append_strings(output, prefix, openCh, textToQuote, closeCh); }
gpk::err_t	gpk::appendNclosdPrefixed	(::gpk::asc_t & output, ::gpk::vcst_t textToEnclose, bool separate, char separator)				{ return separate ? ::gpk::appendNclosdPrefixed(output, textToEnclose, separator, '{', '}') : ::gpk::appendNclosd(output, textToEnclose); }
gpk::err_t	gpk::appendBracedPrefixed	(::gpk::asc_t & output, ::gpk::vcst_t textToEnclose, bool separate, char separator)				{ return separate ? ::gpk::appendNclosdPrefixed(output, textToEnclose, separator, '[', ']') : ::gpk::appendBraced(output, textToEnclose); }
gpk::err_t	gpk::appendQuotedPrefixed	(::gpk::asc_t & output, ::gpk::vcst_t textToEnclose, bool separate, char separator)				{ return separate ? ::gpk::appendNclosdPrefixed(output, textToEnclose, separator, '"')		: ::gpk::appendQuoted(output, textToEnclose); }
gpk::err_t	gpk::appendGtltedPrefixed	(::gpk::asc_t & output, ::gpk::vcst_t textToEnclose, bool separate, char separator)				{ return separate ? ::gpk::appendNclosdPrefixed(output, textToEnclose, separator, '<', '>') : ::gpk::appendGtlted(output, textToEnclose); }
