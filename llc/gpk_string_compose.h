#include "gpk_array_static.h"
#include "gpk_array_pod.h"

#ifndef GPK_STRING_COMPOSE
#define GPK_STRING_COMPOSE

namespace gpk
{
	tplTstin	err_t	append_string			(asc_t & output, cnst T & arg)			{ rtrn output.append_string(arg); }
	tplN2ustin	err_t	append_string			(asc_t & output, sc_c (&arg)[N])		{ rtrn output.append_string(arg); }
	tplN2ustin	err_t	append_string			(asc_t & output, cnst astchar<N> & arg)	{ rtrn output.append_string(arg.Storage); }
	stin		err_t	append_string			(asc_t & output, asc_c & arg)			{ rtrn output.append_string(arg.cc()); }
	tpl_vtArgs	err_t	append_strings			(asc_t & output, _tArgs&&... args)	{ 
		err_t					err			= 0;
		s2_c					results[]	= {err = (failed(err) ? -1 : append_string(output, args))..., 0}; 
		rtrn failed(err) ? err : ::gpk::sum(vcs2_t{results}); 
	}
	tplT_vtArgs	err_t	append_strings_separated	(asc_t & output, T separator, _tArgs&&... args)	{
		err_t					err			= 0;
		u2_t					len			= 0;
		s2_c					results[]	= {len = err = ((0 == len) ? append_string(output, args) : failed(err) ? -1 : append_string(output, separator) + append_string(output, args))..., 0}; 
		rtrn failed(err) ? err : ::gpk::sum(vcs2_t{results}); 
	}
				err_t	appendNclosd			(asc_t & output, vcst_t textToEnclose);
				err_t	appendBraced			(asc_t & output, vcst_t textToEnclose);
				err_t	appendQuoted			(asc_t & output, vcst_t textToEnclose);
				err_t	appendGtlted			(asc_t & output, vcst_t textToEnclose);

				err_t	appendNclosd			(asc_t & output, vcst_t textToEnclose, sc_t encloserChar);
				err_t	appendNclosd			(asc_t & output, vcst_t textToEnclose, sc_t openChar, sc_t closeChar);
				err_t	appendNclosd			(asc_t & output, vcst_t textToEnclose, vcst_t openChars, vcst_t closeChars);
				err_t	appendNclosdPrefixed	(asc_t & output, vcst_t textToEnclose, sc_t prefix, sc_t encloserChar);
				err_t	appendNclosdPrefixed	(asc_t & output, vcst_t textToEnclose, sc_t prefix, sc_t openChar, sc_t closeChar);

				err_t	appendBracedPrefixed	(asc_t & output, vcst_t textToEnclose, b8_t prependSeparator = false, sc_t separator = ',');
				err_t	appendNclosdPrefixed	(asc_t & output, vcst_t textToEnclose, b8_t prependSeparator = false, sc_t separator = ',');
				err_t	appendQuotedPrefixed	(asc_t & output, vcst_t textToEnclose, b8_t prependSeparator = false, sc_t separator = ',');
				err_t	appendGtltedPrefixed	(asc_t & output, vcst_t textToEnclose, b8_t prependSeparator = false, sc_t separator = ',');

	tydf		function<err_t(asc_t&)>			FAppend;
} // namespace

#endif // GPK_STRING_COMPOSE
