#include "gpk_array_pod.h"

#ifndef GPK_STRING_COMPOSE
#define GPK_STRING_COMPOSE

namespace gpk
{
	::gpk::error_t			appendNclosd			(::gpk::achar & output, ::gpk::vcs textToQuote, char encloserChar);
	::gpk::error_t			appendNclosd			(::gpk::achar & output, ::gpk::vcs textToQuote, char openChar, char closeChar);
	::gpk::error_t			appendNclosd			(::gpk::achar & output, ::gpk::vcs textToQuote, ::gpk::vcs openChars, ::gpk::vcs closeChars);
	::gpk::error_t			appendNclosdPrefixed	(::gpk::achar & output, ::gpk::vcs textToQuote, char prefix, char encloserChar);
	::gpk::error_t			appendNclosdPrefixed	(::gpk::achar & output, ::gpk::vcs textToQuote, char prefix, char openChar, char closeChar);

	stainli	::gpk::error_t	appendNclosd			(::gpk::achar & output, ::gpk::vcs textToEnclose) { return appendNclosd(output, textToEnclose, '{', '}'); }
	stainli	::gpk::error_t	appendBraced			(::gpk::achar & output, ::gpk::vcs textToEnclose) { return appendNclosd(output, textToEnclose, '[', ']'); }
	stainli	::gpk::error_t	appendQuoted			(::gpk::achar & output, ::gpk::vcs textToEnclose) { return appendNclosd(output, textToEnclose, '"'); }
	stainli	::gpk::error_t	appendBracedPrefixed	(::gpk::achar & output, ::gpk::vcs textToEnclose, bool prependSeparator = false, char separator = ',') { return prependSeparator ? appendNclosdPrefixed(output, textToEnclose, separator, '[', ']')	: appendNclosd(output, textToEnclose); }
	stainli	::gpk::error_t	appendNclosdPrefixed	(::gpk::achar & output, ::gpk::vcs textToEnclose, bool prependSeparator = false, char separator = ',') { return prependSeparator ? appendNclosdPrefixed(output, textToEnclose, separator, '{', '}')	: appendBraced(output, textToEnclose); }
	stainli	::gpk::error_t	appendQuotedPrefixed	(::gpk::achar & output, ::gpk::vcs textToEnclose, bool prependSeparator = false, char separator = ',') { return prependSeparator ? appendNclosdPrefixed(output, textToEnclose, separator, '"')		: appendQuoted(output, textToEnclose); }

	::gpk::error_t			appendKeyValue			(::gpk::achar & output, ::gpk::vcs key, ::gpk::vcs value);
	::gpk::error_t			appendKeyString			(::gpk::achar & output, ::gpk::vcs key, ::gpk::vcs value);
	template<size_t nSize>
	stainli	::gpk::error_t	appendKeyString			(::gpk::achar & output, ::gpk::vcs key, const char(&value)[nSize]) { return appendKeyString(output, key, ::gpk::vcs{value}); }
	stainli	::gpk::error_t	appendKeyString			(::gpk::achar & output, ::gpk::vcs key, const ::gpk::vcc & value) { return appendKeyString(output, key, ::gpk::vcs{value}); }
	::gpk::error_t			appendKeyObject			(::gpk::achar & output, ::gpk::vcs key, ::gpk::vcs itemsNotEnclosed);
	::gpk::error_t			appendKeyList			(::gpk::achar & output, ::gpk::vcs key, ::gpk::vcs itemsNotEnclosed);
	::gpk::error_t			appendOpenKey			(::gpk::achar & output, ::gpk::vcs key);
} // namespace

#endif // GPK_STRING_COMPOSE
