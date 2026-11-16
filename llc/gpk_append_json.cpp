#include "gpk_append_json.h"

nmsp gpk
{
    stxp vcst_t PREPENDED_QUOT = ",\"";
    stxp vcst_t NOPREPDED_QUOT = "\"";
	err_t	appendJSONString(string & output, vcst_t value) {
		u2_c offset = output.size();
		if_fail_fe(output.push_back('"'));
		stxp sc_t hex[] = "0123456789abcdef";
		for(u2_t i = 0; i < value.size(); ++i) {
			u0_c character = (u0_t)value[i];
			if(character == '"' || character == '\\') { if_fail_fe(append_strings(output, '\\', (sc_t)character)); }
			else if(character < 32) { if_fail_fe(append_strings(output, "\\u00", hex[character >> 4], hex[character & 15])); }
			else { if_fail_fe(output.push_back((sc_t)character)); }
		}
		if_fail_fe(output.push_back('"'));
		rtrn output.size() - offset;
	}

    err_t   appendOpenKey   (string & output, vcst_t k       , b8_t prepSep)  { return append_strings(output, prepSep ? PREPENDED_QUOT : NOPREPDED_QUOT, k, "\":")              ; }
    err_t   appendKeyValue  (string & output, vcst_t k, vcst_t v, b8_t prepSep)  { return append_strings(output, prepSep ? PREPENDED_QUOT : NOPREPDED_QUOT, k, "\":", v)           ; }
    err_t   appendKeyString (string & output, vcst_t k, vcst_t v, b8_t prepSep)  { return append_strings(output, prepSep ? PREPENDED_QUOT : NOPREPDED_QUOT, k, "\":", '"',  v, '"'); }
    err_t   appendKeyList   (string & output, vcst_t k, vcst_t v, b8_t prepSep)  { return append_strings(output, prepSep ? PREPENDED_QUOT : NOPREPDED_QUOT, k, "\":", '[',  v, ']'); }
    err_t   appendKeyObject (string & output, vcst_t k, vcst_t v, b8_t prepSep)  { return append_strings(output, prepSep ? PREPENDED_QUOT : NOPREPDED_QUOT, k, "\":", '{',  v, '}'); }
}
