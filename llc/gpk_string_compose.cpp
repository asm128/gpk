#include "gpk_string_compose.h"

// -*- Generic string enclosing -*-
gpk::err_t	gpk::appendNclosd			(::gpk::achar & output, ::gpk::vcs textToEnclose) 												{ return ::gpk::append_strings(output, '{', textToEnclose,'}'); }
gpk::err_t	gpk::appendBraced			(::gpk::achar & output, ::gpk::vcs textToEnclose) 												{ return ::gpk::append_strings(output, '[', textToEnclose,']'); }
gpk::err_t	gpk::appendQuoted			(::gpk::achar & output, ::gpk::vcs textToEnclose) 												{ return ::gpk::append_strings(output, '"', textToEnclose,'"'); }
gpk::err_t	gpk::appendGtlted			(::gpk::achar & output, ::gpk::vcs textToEnclose) 												{ return ::gpk::append_strings(output, '<', textToEnclose,'>'); }
gpk::err_t	gpk::appendNclosd			(::gpk::achar & output, ::gpk::vcs textToQuote, ::gpk::vcs openChars, ::gpk::vcs closeChars)	{ return ::gpk::append_strings(output, openChars, textToQuote, closeChars); }
gpk::err_t	gpk::appendNclosd			(::gpk::achar & output, ::gpk::vcs textToQuote, char openChar, char closeChar)					{ return ::gpk::append_strings(output, openChar, textToQuote, closeChar); }
gpk::err_t	gpk::appendNclosd			(::gpk::achar & output, ::gpk::vcs textToQuote, char encloserChar)								{ return ::gpk::append_strings(output, encloserChar, textToQuote, encloserChar); }
gpk::err_t	gpk::appendNclosdPrefixed	(::gpk::achar & output, ::gpk::vcs textToQuote, char prefix, char encloser) 					{ return ::gpk::append_strings(output, prefix, encloser, textToQuote, encloser); }
gpk::err_t	gpk::appendNclosdPrefixed	(::gpk::achar & output, ::gpk::vcs textToQuote, char prefix, char openCh, char closeCh) 		{ return ::gpk::append_strings(output, prefix, openCh, textToQuote, closeCh); }
gpk::err_t	gpk::appendNclosdPrefixed	(::gpk::achar & output, ::gpk::vcs textToEnclose, bool separate, char separator)				{ return separate ? ::gpk::appendNclosdPrefixed(output, textToEnclose, separator, '{', '}') : ::gpk::appendNclosd(output, textToEnclose); }
gpk::err_t	gpk::appendBracedPrefixed	(::gpk::achar & output, ::gpk::vcs textToEnclose, bool separate, char separator)				{ return separate ? ::gpk::appendNclosdPrefixed(output, textToEnclose, separator, '[', ']') : ::gpk::appendBraced(output, textToEnclose); }
gpk::err_t	gpk::appendQuotedPrefixed	(::gpk::achar & output, ::gpk::vcs textToEnclose, bool separate, char separator)				{ return separate ? ::gpk::appendNclosdPrefixed(output, textToEnclose, separator, '"')		: ::gpk::appendQuoted(output, textToEnclose); }
gpk::err_t	gpk::appendGtltedPrefixed	(::gpk::achar & output, ::gpk::vcs textToEnclose, bool separate, char separator)				{ return separate ? ::gpk::appendNclosdPrefixed(output, textToEnclose, separator, '<', '>') : ::gpk::appendGtlted(output, textToEnclose); }
