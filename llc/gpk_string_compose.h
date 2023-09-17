#include "gpk_array_pod.h"

#ifndef GPK_STRING_COMPOSE
#define GPK_STRING_COMPOSE

namespace gpk
{
	::gpk::error_t			appendEnclosed			(::gpk::achar & output, ::gpk::vcs textToQuote, char encloserChar);
	::gpk::error_t			appendEnclosed			(::gpk::achar & output, ::gpk::vcs textToQuote, char openChar, char closeChar);
	::gpk::error_t			appendEnclosedPrefixed	(::gpk::achar & output, ::gpk::vcs textToQuote, char prefix, char encloserChar);
	::gpk::error_t			appendEnclosedPrefixed	(::gpk::achar & output, ::gpk::vcs textToQuote, char prefix, char openChar, char closeChar);

	stainli	::gpk::error_t	appendQuoted			(::gpk::achar & output, ::gpk::vcs textToQuote) { return appendEnclosed(output, textToQuote, '"'); }
	stainli	::gpk::error_t	appendBraced			(::gpk::achar & output, ::gpk::vcs textToQuote) { return appendEnclosed(output, textToQuote, '[', ']'); }
	stainli	::gpk::error_t	appendQuotedPrefixed	(::gpk::achar & output, ::gpk::vcs textToQuote, bool prependSeparator = false, char separator = ',') { return prependSeparator ? appendEnclosedPrefixed(output, textToQuote, separator, '"') : appendQuoted(output, textToQuote); }
	stainli	::gpk::error_t	appendBracedPrefixed	(::gpk::achar & output, ::gpk::vcs textToQuote, bool prependSeparator = false, char separator = ',') { return prependSeparator ? appendEnclosedPrefixed(output, textToQuote, separator, '[', ']') : appendBraced(output, textToQuote); }

	::gpk::error_t			appendKeyValue			(::gpk::achar & output, ::gpk::vcs key, ::gpk::vcs value);
	::gpk::error_t			appendKeyString			(::gpk::achar & output, ::gpk::vcs key, ::gpk::vcs value);
	::gpk::error_t			appendKeyObject			(::gpk::achar & output, ::gpk::vcs key, ::gpk::vcs itemsNotEnclosed);
	::gpk::error_t			appendKeyList			(::gpk::achar & output, ::gpk::vcs key, ::gpk::vcs itemsNotEnclosed);
} // namespace

#endif // GPK_STRING_COMPOSE
