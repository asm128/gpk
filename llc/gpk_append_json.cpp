#include "gpk_append_json.h"

namespace gpk
{
    stxp vcs PREPENDED_QUOT = GPK_CXS(",\"");
    stxp vcs NOPREPDED_QUOT = GPK_CXS("\"");

    err_t   appendOpenKey   (string & output, vcs k       , bool prepSep)  { return append_strings(output, prepSep ? PREPENDED_QUOT : NOPREPDED_QUOT, k, "\":")              ; }
    err_t   appendKeyValue  (string & output, vcs k, vcs v, bool prepSep)  { return append_strings(output, prepSep ? PREPENDED_QUOT : NOPREPDED_QUOT, k, "\":", v)           ; }
    err_t   appendKeyString (string & output, vcs k, vcs v, bool prepSep)  { return append_strings(output, prepSep ? PREPENDED_QUOT : NOPREPDED_QUOT, k, "\":", '"',  v, '"'); }
    err_t   appendKeyList   (string & output, vcs k, vcs v, bool prepSep)  { return append_strings(output, prepSep ? PREPENDED_QUOT : NOPREPDED_QUOT, k, "\":", '[',  v, ']'); }
    err_t   appendKeyObject (string & output, vcs k, vcs v, bool prepSep)  { return append_strings(output, prepSep ? PREPENDED_QUOT : NOPREPDED_QUOT, k, "\":", '{',  v, '}'); }
}


