#include "llc_enum.h"

#ifndef LLC_MICRO_H
#define LLC_MICRO_H

namespace llc
{
    GDEFINE_ENUM_TYPE (ESP_ERROR, s2_t);                 
    GDEFINE_ENUM_VALUE(ESP_ERROR, OK                            , OS_OK                                 );
    GDEFINE_ENUM_VALUE(ESP_ERROR, NO_MEM                        , ESP_NO_MEM                            );
    GDEFINE_ENUM_VALUE(ESP_ERROR, INVALID_ARG                   , ESP_INVALID_ARG                       );
    GDEFINE_ENUM_VALUE(ESP_ERROR, INVALID_STATE                 , ESP_INVALID_STATE                     );
    GDEFINE_ENUM_VALUE(ESP_ERROR, INVALID_SIZE                  , ESP_INVALID_SIZE                      );
    GDEFINE_ENUM_VALUE(ESP_ERROR, NOT_FOUND                     , ESP_NOT_FOUND                         );
    GDEFINE_ENUM_VALUE(ESP_ERROR, NOT_SUPPORTED                 , ESP_NOT_SUPPORTED                     );
    GDEFINE_ENUM_VALUE(ESP_ERROR, TIMEOUT                       , ESP_TIMEOUT                           );
    GDEFINE_ENUM_VALUE(ESP_ERROR, INVALID_RESPONSE              , ESP_INVALID_RESPONSE                  );
    GDEFINE_ENUM_VALUE(ESP_ERROR, INVALID_CRC                   , ESP_INVALID_CRC                       );
    GDEFINE_ENUM_VALUE(ESP_ERROR, INVALID_VERSION               , ESP_INVALID_VERSION                   );
    GDEFINE_ENUM_VALUE(ESP_ERROR, INVALID_MAC                   , ESP_INVALID_MAC                       );
    GDEFINE_ENUM_VALUE(ESP_ERROR, NOT_FINISHED                  , ESP_NOT_FINISHED                      );
    GDEFINE_ENUM_VALUE(ESP_ERROR, WIFI_BASE                     , ESP_WIFI_BASE                         );
    GDEFINE_ENUM_VALUE(ESP_ERROR, MESH_BASE                     , ESP_MESH_BASE                         );
    GDEFINE_ENUM_VALUE(ESP_ERROR, FLASH_BASE                    , ESP_FLASH_BASE                        );
    GDEFINE_ENUM_VALUE(ESP_ERROR, HW_CRYPTO_BASE                , ESP_HW_CRYPTO_BASE                    );
    GDEFINE_ENUM_VALUE(ESP_ERROR, MEMPROT_BASE                  , ESP_MEMPROT_BASE                      );
    GDEFINE_ENUM_VALUE(ESP_ERROR, FLASH_OP_TIMEOUT              , ESP_FLASH_OP_TIMEOUT                  );
    GDEFINE_ENUM_VALUE(ESP_ERROR, FLASH_OP_FAIL                 , ESP_FLASH_OP_FAIL                     );
    GDEFINE_ENUM_VALUE(ESP_ERROR, OTA_BASE                      , ESP_OTA_BASE                          );
    GDEFINE_ENUM_VALUE(ESP_ERROR, OTA_PARTITION_CONFLICT        , ESP_OTA_PARTITION_CONFLICT            );
    GDEFINE_ENUM_VALUE(ESP_ERROR, OTA_SELECT_INFO_INVALID       , ESP_OTA_SELECT_INFO_INVALID           );
    GDEFINE_ENUM_VALUE(ESP_ERROR, OTA_VALIDATE_FAILED           , ESP_OTA_VALIDATE_FAILED               ); // Partition contained invalid app image. Also returned if secure boot is enabled and signature validation failed.
    GDEFINE_ENUM_VALUE(ESP_ERROR, OTA_SMALL_SEC_VER             , ESP_OTA_SMALL_SEC_VER                 );
    GDEFINE_ENUM_VALUE(ESP_ERROR, OTA_ROLLBACK_FAILED           , ESP_OTA_ROLLBACK_FAILED               );
    GDEFINE_ENUM_VALUE(ESP_ERROR, OTA_ROLLBACK_INVALID_STATE    , ESP_OTA_ROLLBACK_INVALID_STATE        );
    GDEFINE_ENUM_VALUE(ESP_ERROR, FAIL                          , OS_ERROR                              );

    GDEFINE_ENUM_TYPE (ESP_TCP, s2_t);
    GDEFINE_ENUM_VALUE(ESP_TCP, OK                    ,  0);
    GDEFINE_ENUM_VALUE(ESP_TCP, NO_MEMORY             , -1);
    GDEFINE_ENUM_VALUE(ESP_TCP, BUFFER                , -2);
    GDEFINE_ENUM_VALUE(ESP_TCP, TIMEOUT               , -3);
    GDEFINE_ENUM_VALUE(ESP_TCP, ROUTING               , -4);
    GDEFINE_ENUM_VALUE(ESP_TCP, IN_PROGRESS           , -5);
    GDEFINE_ENUM_VALUE(ESP_TCP, ILLEGAL_VALUE         , -6);
    GDEFINE_ENUM_VALUE(ESP_TCP, WOULDBLOCK            , -7);
    GDEFINE_ENUM_VALUE(ESP_TCP, ADDRESS_IN_USE        , -8);
    GDEFINE_ENUM_VALUE(ESP_TCP, IS_CONNECTING         , -9);
    GDEFINE_ENUM_VALUE(ESP_TCP, ALREADY_CONNECTED     , -10);
    GDEFINE_ENUM_VALUE(ESP_TCP, NOT_CONNECTED         , -11);
    GDEFINE_ENUM_VALUE(ESP_TCP, LOW_LEVEL_NETIF_ERROR , -12);
    GDEFINE_ENUM_VALUE(ESP_TCP, CONNECTION_ABORTED    , -13);
    GDEFINE_ENUM_VALUE(ESP_TCP, CONNECTION_RESET      , -14);
    GDEFINE_ENUM_VALUE(ESP_TCP, CLOSED                , -15);
    GDEFINE_ENUM_VALUE(ESP_TCP, ILLEGAL_ARGUMENT      , -16);

	GDEFINE_ENUM_TYPE  (ESP_RESET, s2_t); 
#ifdef LLC_ESP8266
	GDEFINE_ENUM_VALUED(ESP_RESET, POWERON   ,  0, "Reset due to power-on event"                            );
	GDEFINE_ENUM_VALUED(ESP_RESET, WDT       ,  1, "Reset due to other watchdogs"                           );
	GDEFINE_ENUM_VALUED(ESP_RESET, PANIC     ,  2, "Software reset due to exception/panic"                  );
	GDEFINE_ENUM_VALUED(ESP_RESET, TASK_WDT  ,  3, "Reset due to task watchdog"                             );
	GDEFINE_ENUM_VALUED(ESP_RESET, SW        ,  4, "Software reset via esp_restart"                         );
	GDEFINE_ENUM_VALUED(ESP_RESET, DEEPSLEEP ,  5, "Reset after exiting deep sleep mode"                    );
	GDEFINE_ENUM_VALUED(ESP_RESET, SDIO      ,  6, "Reset over SDIO"                                        );
#else // !LLC_ESP8266
	GDEFINE_ENUM_VALUED(ESP_RESET, UNKNOWN   ,  0, "Reset reason can not be determined"                     );
	GDEFINE_ENUM_VALUED(ESP_RESET, POWERON   ,  1, "Reset due to power-on event"                            );
	GDEFINE_ENUM_VALUED(ESP_RESET, EXT       ,  2, "Reset by external pin (not applicable for ESP32)"       );
	GDEFINE_ENUM_VALUED(ESP_RESET, SW        ,  3, "Software reset via esp_restart"                         );
	GDEFINE_ENUM_VALUED(ESP_RESET, PANIC     ,  4, "Software reset due to exception/panic"                  );
	GDEFINE_ENUM_VALUED(ESP_RESET, INT_WDT   ,  5, "Reset (software or hardware) due to interrupt watchdog" );
	GDEFINE_ENUM_VALUED(ESP_RESET, TASK_WDT  ,  6, "Reset due to task watchdog"                             );
	GDEFINE_ENUM_VALUED(ESP_RESET, WDT       ,  7, "Reset due to other watchdogs"                           );
	GDEFINE_ENUM_VALUED(ESP_RESET, DEEPSLEEP ,  8, "Reset after exiting deep sleep mode"                    );
	GDEFINE_ENUM_VALUED(ESP_RESET, BROWNOUT  ,  9, "Brownout reset (software or hardware)"                  );
	GDEFINE_ENUM_VALUED(ESP_RESET, SDIO      , 10, "Reset over SDIO"                                        );
#endif // LLC_ESP8266
	GDEFINE_ENUM_TYPE  (ESP_AWAKE, s2_t);
    GDEFINE_ENUM_VALUE (ESP_AWAKE, UNDEFINED         ,   0); 
    GDEFINE_ENUM_VALUE (ESP_AWAKE, ALL               ,   1); 
    GDEFINE_ENUM_VALUED(ESP_AWAKE, EXT0              ,   2, "Wakeup by external signal using RTC_IO"  ); 
    GDEFINE_ENUM_VALUED(ESP_AWAKE, EXT1              ,   3, "Wakeup by external signal using RTC_CNTL"); 
    GDEFINE_ENUM_VALUED(ESP_AWAKE, TIMER             ,   4, "Wakeup by timer"                         ); 
    GDEFINE_ENUM_VALUED(ESP_AWAKE, TOUCHPAD          ,   5, "Wakeup by touchpad"                      ); 
    GDEFINE_ENUM_VALUED(ESP_AWAKE, ULP               ,   6, "Wakeup by ULP program"                   ); 
    GDEFINE_ENUM_VALUE (ESP_AWAKE, GPIO              ,   7); 
    GDEFINE_ENUM_VALUE (ESP_AWAKE, UART              ,   8); 
    GDEFINE_ENUM_VALUE (ESP_AWAKE, WIFI              ,   9); 
    GDEFINE_ENUM_VALUE (ESP_AWAKE, COCPU             ,  10); 
    GDEFINE_ENUM_VALUE (ESP_AWAKE, COCPU_TRAP_TRIG   ,  11); 
    GDEFINE_ENUM_VALUE (ESP_AWAKE, BT                ,  12); 
} // namespace

#endif // LLC_MICRO_H
