#include "llc_enum.h"

#ifndef LLC_WIFI_H
#define LLC_WIFI_H

namespace llc
{
	GDEFINE_ENUM_TYPE(WIFI_AUTH, u0_t);
    GDEFINE_ENUM_VALUE(WIFI_AUTH, OPEN                    , 0);  //  
    GDEFINE_ENUM_VALUE(WIFI_AUTH, WEP                     , 1);  //  
    GDEFINE_ENUM_VALUE(WIFI_AUTH, WPA_PSK                 , 2);  //  
    GDEFINE_ENUM_VALUE(WIFI_AUTH, WPA2_PSK                , 3);  //  
    GDEFINE_ENUM_VALUE(WIFI_AUTH, WPA_WPA2_PSK            , 4);  //  
    GDEFINE_ENUM_VALUE(WIFI_AUTH, ENTERPRISE              , 5);  // Same as WPA2_ENTERPRISE
    GDEFINE_ENUM_VALUE(WIFI_AUTH, WPA2_ENTERPRISE         , WIFI_AUTH_ENTERPRISE); // Same as Enterprise security
    GDEFINE_ENUM_VALUE(WIFI_AUTH, WPA3_PSK                , 6);  //  
    GDEFINE_ENUM_VALUE(WIFI_AUTH, WPA2_WPA3_PSK           , 7);  //  
    GDEFINE_ENUM_VALUE(WIFI_AUTH, WAPI_PSK                , 8);  //  
    GDEFINE_ENUM_VALUE(WIFI_AUTH, OWE                     , 9);  //  
    GDEFINE_ENUM_VALUE(WIFI_AUTH, WPA3_ENT_SUITE_B_192_BIT, 10); //  
    GDEFINE_ENUM_VALUE(WIFI_AUTH, WPA3_EXT_PSK            , 11); // Deprecated. Use WIFI_AUTH_WPA3_PSK instead.
    GDEFINE_ENUM_VALUE(WIFI_AUTH, WPA3_EXT_PSK_MIXED_MODE , 12); // Deprecated. Use WIFI_AUTH_WPA3_PSK instead.
    GDEFINE_ENUM_VALUE(WIFI_AUTH, DPP                     , 13); //  
    GDEFINE_ENUM_VALUE(WIFI_AUTH, WPA3_ENTERPRISE         , 14); //  
    GDEFINE_ENUM_VALUE(WIFI_AUTH, WPA2_WPA3_ENTERPRISE    , 15); //  
    GDEFINE_ENUM_VALUE(WIFI_AUTH, WPA_ENTERPRISE          , 16); //  
    GDEFINE_ENUM_VALUE(WIFI_AUTH, MAX                     , 17); //
} // namespace 

#endif // LLC_WIFI_H
