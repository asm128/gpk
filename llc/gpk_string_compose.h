#include "gpk_array_static.h"
#include "gpk_string.h"

#ifndef GPK_STRING_COMPOSE
#define GPK_STRING_COMPOSE

nmsp gpk
{
	stin		err_t	append_string			(asc_t & output, vcst_t arg)		{ rtrn output.append(vcst_t{arg}); }
	tplTstin	err_t	append_string			(asc_t & output, view<cnst T> arg)	{ rtrn output.append(vcst_t{arg}); }
	stin		err_t	append_string			(asc_t & output, sc_c arg)			{ rtrn output.push_back(arg); }
	tpl_vtArgs	err_t	append_strings			(asc_t & output, _tArgs&&... args)	{ 
		err_t					err			= 0;
		s2_c					results[]	= {err = (failed(err) ? -1 : append_string(output, args))..., 0}; 
		rtrn failed(err) ? err : sum(vcs2_t{results}); 
	}
	tplT_vtArgs	err_t	append_strings_separated	(asc_t & output, T separator, _tArgs&&... args)	{
		err_t					err			= 0;
		u2_t					len			= 0;
		s2_c					results[]	= {len = err = ((0 == len) ? append_string(output, args) : failed(err) ? -1 : append_string(output, separator, args))..., 0}; 
		rtrn failed(err) ? err : sum(vcs2_t{results}); 
	}
				err_t	appendNclosd			(string & output, vcst_t textToEnclose);
				err_t	appendBraced			(string & output, vcst_t textToEnclose);
				err_t	appendQuoted			(string & output, vcst_t textToEnclose);
				err_t	appendGtlted			(string & output, vcst_t textToEnclose);

				err_t	appendNclosd			(string & output, vcst_t textToEnclose, sc_t encloserChar);
				err_t	appendNclosd			(string & output, vcst_t textToEnclose, sc_t openChar, sc_t closeChar);
				err_t	appendNclosd			(string & output, vcst_t textToEnclose, vcst_t openChars, vcst_t closeChars);
				err_t	appendNclosdPrefixed	(string & output, vcst_t textToEnclose, sc_t prefix, sc_t encloserChar);
				err_t	appendNclosdPrefixed	(string & output, vcst_t textToEnclose, sc_t prefix, sc_t openChar, sc_t closeChar);

				err_t	appendBracedPrefixed	(string & output, vcst_t textToEnclose, b8_t prependSeparator = false, sc_t separator = ',');
				err_t	appendNclosdPrefixed	(string & output, vcst_t textToEnclose, b8_t prependSeparator = false, sc_t separator = ',');
				err_t	appendQuotedPrefixed	(string & output, vcst_t textToEnclose, b8_t prependSeparator = false, sc_t separator = ',');
				err_t	appendGtltedPrefixed	(string & output, vcst_t textToEnclose, b8_t prependSeparator = false, sc_t separator = ',');

	tydf		function<err_t(string&)>			FAppend;
} // nmsp

#endif // GPK_STRING_COMPOSE
