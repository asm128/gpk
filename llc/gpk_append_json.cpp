#include "gpk_append_json.h"

namespace gpk
{
    stxp vcst_t PREPENDED_QUOT = ",\"";
    stxp vcst_t NOPREPDED_QUOT = "\"";

    err_t   appendOpenKey   (string & output, vcst_t k       , bool prepSep)  { return append_strings(output, prepSep ? PREPENDED_QUOT : NOPREPDED_QUOT, k, "\":")              ; }
    err_t   appendKeyValue  (string & output, vcst_t k, vcst_t v, bool prepSep)  { return append_strings(output, prepSep ? PREPENDED_QUOT : NOPREPDED_QUOT, k, "\":", v)           ; }
    err_t   appendKeyString (string & output, vcst_t k, vcst_t v, bool prepSep)  { return append_strings(output, prepSep ? PREPENDED_QUOT : NOPREPDED_QUOT, k, "\":", '"',  v, '"'); }
    err_t   appendKeyList   (string & output, vcst_t k, vcst_t v, bool prepSep)  { return append_strings(output, prepSep ? PREPENDED_QUOT : NOPREPDED_QUOT, k, "\":", '[',  v, ']'); }
    err_t   appendKeyObject (string & output, vcst_t k, vcst_t v, bool prepSep)  { return append_strings(output, prepSep ? PREPENDED_QUOT : NOPREPDED_QUOT, k, "\":", '{',  v, '}'); }
}
