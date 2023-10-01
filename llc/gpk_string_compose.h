#include "gpk_apod_n2.h"

#ifndef GPK_STRING_COMPOSE
#define GPK_STRING_COMPOSE

namespace gpk
{
	tplt<tpnm... _tArgs>	
	gpk::err_t			append_strings			(::gpk::achar & output, _tArgs&&... args)	{ 
		const int32_t results[] = {output.append_string(args)..., 0}; 
		return ::gpk::sum(::gpk::vci32{results}); 
	}

	tplt<tpnm TSeparator, tpnm... _tArgs>	
	gpk::err_t			append_strings_separated(::gpk::achar & output, TSeparator separator, _tArgs&&... args)	{
		uint32_t			len					= 0;
		const int32_t		results[]			= {len = ((0 == len) ? output.append_string(args) : output.append_string(separator) + output.append_string(args))..., 0}; 
		return ::gpk::sum(::gpk::vci32{results}); 
	}

	typedef ::gpk::function<gpk::err_t(::gpk::achar & )> FAppend;

	gpk::err_t			appendNclosd			(::gpk::achar & output, ::gpk::vcs textToEnclose);
	gpk::err_t			appendBraced			(::gpk::achar & output, ::gpk::vcs textToEnclose);
	gpk::err_t			appendQuoted			(::gpk::achar & output, ::gpk::vcs textToEnclose);
	gpk::err_t			appendGtlted			(::gpk::achar & output, ::gpk::vcs textToEnclose);

	gpk::err_t			appendNclosd			(::gpk::achar & output, ::gpk::vcs textToEnclose, char encloserChar);
	gpk::err_t			appendNclosd			(::gpk::achar & output, ::gpk::vcs textToEnclose, char openChar, char closeChar);
	gpk::err_t			appendNclosd			(::gpk::achar & output, ::gpk::vcs textToEnclose, ::gpk::vcs openChars, ::gpk::vcs closeChars);
	gpk::err_t			appendNclosdPrefixed	(::gpk::achar & output, ::gpk::vcs textToEnclose, char prefix, char encloserChar);
	gpk::err_t			appendNclosdPrefixed	(::gpk::achar & output, ::gpk::vcs textToEnclose, char prefix, char openChar, char closeChar);

	gpk::err_t			appendBracedPrefixed	(::gpk::achar & output, ::gpk::vcs textToEnclose, bool prependSeparator = false, char separator = ',');
	gpk::err_t			appendNclosdPrefixed	(::gpk::achar & output, ::gpk::vcs textToEnclose, bool prependSeparator = false, char separator = ',');
	gpk::err_t			appendQuotedPrefixed	(::gpk::achar & output, ::gpk::vcs textToEnclose, bool prependSeparator = false, char separator = ',');
	gpk::err_t			appendGtltedPrefixed	(::gpk::achar & output, ::gpk::vcs textToEnclose, bool prependSeparator = false, char separator = ',');
} // namespace

#endif // GPK_STRING_COMPOSE
