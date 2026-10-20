#include "gpk_string_compose.h"

#ifndef GPK_APPEND_JSON_H
#define GPK_APPEND_JSON_H

namespace gpk
{
				err_t	appendOpenKey	(string & output, vcst_t key							, b8_t prependComma = false);
				err_t	appendKeyValue	(string & output, vcst_t key, vcst_t value				, b8_t prependComma = false);
				err_t	appendKeyObject	(string & output, vcst_t key, vcst_t valuesNotEnclosed	, b8_t prependComma = false);
				err_t	appendKeyList	(string & output, vcst_t key, vcst_t valuesNotEnclosed	, b8_t prependComma = false);
				err_t	appendKeyString	(string & output, vcst_t key, vcst_t value				, b8_t prependComma = false);
	tplN2ustin	err_t	appendKeyString	(string & output, vcst_t key, sc_c (&value)[N]			, b8_t prependComma = false)	{ rtrn appendKeyString(output, key, vcst_t{value}, prependComma); }

} // namespace 

#endif // GPK_APPEND_JSON_H


