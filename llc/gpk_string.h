#include "gpk_array_pod.h"
#ifdef GPK_ARDUINO
#   include <IPAddress.h>
#endif // GPK_ARDUINO

#ifndef GPK_STRING_H
#define GPK_STRING_H

namespace gpk
{

    struct string : asc_t {
        usng    asc_t       ::Data;
        usng    asc_t       ::Count;

        inln                string      ()                      nxpt    = dflt;
        inln                string      (cnst vcst_t & other)           : asc_t(other)                                          {}
        tplN2uinln          string      (sc_c (&other)[N])              : asc_t(vcst_t(other))                                  {}
#ifdef GPK_ARDUINO
        inln                string      (cnst String & other)           : asc_t(vcst_t{other.begin(), (u2_t)other.length()})    {}
        inln                string      (cnst IPAddress & other)        : string(other.GPK_VIEW_ARGS())                              {}
#   ifdef GPK_ESP8266
        ndin        oper    String      ()                      cnst    { return String(Data); }
#   else
        ndin        oper    String      ()                      cnst    { return String(Data, Count); }
#   endif
#endif
        inln        b8_t    oper==      (cnst vcst_t & other)   cnst    { rtrn asc_t::oper==(other); }
        inln        b8_t    oper!=      (cnst vcst_t & other)   cnst    { rtrn asc_t::oper!=(other); }
        tplN2uinln  b8_t    oper==      (sc_c (&other)[N])      cnst    { rtrn oper==(vcst_t{other}); }
        tplN2uinln  b8_t    oper!=      (sc_c (&other)[N])      cnst    { rtrn oper!=(vcst_t{other}); }

        ndin        oper    vcst_t      ()                      cnst    { return {Data, Count}; }
        ndin        oper    vstr_t      ()                              { return {Data, Count}; }
        ndin        oper    sc_c*       ()                      cnst    { return vcst_t{Data, Count}; }
        ndin        oper    sc_t*       ()                              { return vstr_t{Data, Count}; }

        ndin        sc_c*   begin       ()                      cnst    { return vcst_t{Data, Count}.begin(); }
        ndin        sc_c*   end         ()                      cnst    { return vcst_t{Data, Count}.end  (); }
        ndin        sc_t*   begin       ()                              { return vstr_t{Data, Count}.begin(); }
        ndin        sc_t*   end         ()                              { return vstr_t{Data, Count}.end  (); }

    }; // struct

#ifdef GPK_ARDUINO
    ndsi        string  str         (cnst IPAddress & s)            { return s.GPK_VIEW_ARGS(); }
#endif
                err_t   camelCase   (vcst_t input, asc_t & camelCased);

} // namespace

#endif // GPK_STRING_H
