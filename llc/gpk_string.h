#include "gpk_array_pod.h"
#include "gpk_std_cstring.h"
#ifdef GPK_ARDUINO
#   include <IPAddress.h>
#endif // GPK_ARDUINO

#ifndef GPK_STRING_H
#define GPK_STRING_H

namespace gpk
{

    struct string : asc_t {
        using   asc_t  ::Data;
        using   asc_t  ::Count;
        using   asc_t  ::Size;
        using   asc_t  ::operator=;
        using   asc_t  ::asc_t;

        tplt<size_t buflen>
        inline  string      (sc_c (&other)[buflen])         : asc_t(vcst_t{other})                                  { initialize(); }
        inline  string      (vsc_c & other)                 : asc_t(other)                                          { initialize(); }
        inline  string      (asc_c & other)                 : asc_t(other)                                          { initialize(); }
        inline  string      (vcst_t other)                  : asc_t(other)                                          { initialize(); }
        inline  string      (sc_c * unsafe)                 : asc_t(tovcc(unsafe))                                  { initialize(); }
#ifdef GPK_ARDUINO
        inline  string      (const String & other)          : asc_t(vcst_t{other.begin(), (u2_t)other.length()})    { initialize(); }
        inline  string      (const IPAddress & other)       : string(other.toString())                              { initialize(); }

#   ifdef GPK_ESP8266
        ndin operator   String      ()  const   { return String(Data); }
#   else
        ndin operator   String      ()  const   { return String(Data, Count); }
#   endif
#endif
        ndin operator   vcst_t      ()  const   { return {Data, Count}; }
        ndin operator   const char* ()  const   { return vcst_t{Data, Count}; }

        const char*     begin       ()  const   { return vcst_t{Data, Count}.begin(); }
        const char*     end         ()  const   { return vcst_t{Data, Count}.end  (); }

        int32_t         initialize  ()          {
            if_true_block_logf(error_printf, (0 == Data) && (Count || Size), Count = Size = 0, "error|Count:%i, Size:%i.", Count, Size);
            return Count;
        }
    }; // struct

#ifdef GPK_ARDUINO
    ndsi string  str (const IPAddress & s)   { return s.toString(); }
#endif
} // namespace

#endif // GPK_STRING_H
