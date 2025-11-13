#include "gpk_typeint.h"

#ifndef GPK_ERROR_H_23627
#define GPK_ERROR_H_23627

namespace gpk
{
	tydf		s2_t	err_t;
	tdcs		err_t	err_c;

	tydf		s2_t	error_t;
	tdcs		error_t	error_c;

	tplTnsix	T		failed		(T errorCode)	nxpt	{ rtrn T((errorCode < 0) ? errorCode : 0);	}
	tplTnsix	bool	not_failed	(T errorCode)	nxpt	{ rtrn false == failed(errorCode); }
	tplTnsix	bool	succeeded	(T errorCode)	nxpt	{ rtrn false == failed(errorCode); }

	stxp		err_c	OS_OK                = 0;
	stxp		err_c	OS_ERROR             = ::gpk::OS_OK                - 1;
	stxp		err_c	OS_BUSY              = ::gpk::OS_ERROR             - 1;
	stxp		err_c	OS_TIMEOUT           = ::gpk::OS_BUSY              - 1;
	stxp		err_c	OS_FULL              = ::gpk::OS_TIMEOUT           - 1;
	stxp		err_c	OS_EMPTY             = ::gpk::OS_FULL              - 1;
	stxp		err_c	OS_OVERRUN           = ::gpk::OS_EMPTY             - 1;
	stxp		err_c	OS_NOT_AVAILABLE     = ::gpk::OS_OVERRUN           - 1;
	stxp		err_c	OS_NOT_FOUND         = ::gpk::OS_NOT_AVAILABLE     - 1;
	stxp		err_c	OS_INVALID_PARAMETER = ::gpk::OS_NOT_FOUND         - 1;
	stxp		err_c	OS_FORBIDDEN         = ::gpk::OS_INVALID_PARAMETER - 1;
	stxp		err_c	OS_RESTART           = ::gpk::OS_FORBIDDEN         - 1;
	stxp		err_c	OS_WAKE_UP           = ::gpk::OS_RESTART           - 1;
	stxp		err_c	OS_SLEEP             = ::gpk::OS_WAKE_UP           - 1;
	stxp		err_c	OS_OFFLINE           = ::gpk::OS_SLEEP             - 1;
	stxp		err_c	OS_DISCONNECTED      = ::gpk::OS_OFFLINE           - 1;
	stxp		err_c	OS_CONNECTING        = ::gpk::OS_DISCONNECTED      - 1;
	stxp		err_c	OS_CONNECTED         = ::gpk::OS_CONNECTING        - 1;
	stxp		err_c	OS_MISSING_DATA      = ::gpk::OS_CONNECTED         - 1;
	stxp		err_c	OS_NO_MEMORY         = ::gpk::OS_MISSING_DATA      - 1;
	stxp		err_c	OS_READ              = ::gpk::OS_NO_MEMORY         - 1;
	stxp		err_c	OS_WRITE             = ::gpk::OS_READ              - 1;

	stxp		err_c	ESP_NO_MEM                     = -0x101 ;
	stxp		err_c	ESP_INVALID_ARG                = -0x102 ;
	stxp		err_c	ESP_INVALID_STATE              = -0x103 ;
	stxp		err_c	ESP_INVALID_SIZE               = -0x104 ;
	stxp		err_c	ESP_NOT_FOUND                  = -0x105 ;
	stxp		err_c	ESP_NOT_SUPPORTED              = -0x106 ;
	stxp		err_c	ESP_TIMEOUT                    = -0x107 ;
	stxp		err_c	ESP_INVALID_RESPONSE           = -0x108 ;
	stxp		err_c	ESP_INVALID_CRC                = -0x109 ;
	stxp		err_c	ESP_INVALID_VERSION            = -0x10A ;
	stxp		err_c	ESP_INVALID_MAC                = -0x10B ;
	stxp		err_c	ESP_NOT_FINISHED               = -0x10C ;
	stxp		err_c	ESP_OTA_BASE                   = -0x1500;
	stxp		err_c	ESP_OTA_PARTITION_CONFLICT     = ESP_OTA_BASE                - 1;
	stxp		err_c	ESP_OTA_SELECT_INFO_INVALID    = ESP_OTA_PARTITION_CONFLICT  - 1;
	stxp		err_c	ESP_OTA_VALIDATE_FAILED        = ESP_OTA_SELECT_INFO_INVALID - 1;
	stxp		err_c	ESP_OTA_SMALL_SEC_VER          = ESP_OTA_VALIDATE_FAILED     - 1;
	stxp		err_c	ESP_OTA_ROLLBACK_FAILED        = ESP_OTA_SMALL_SEC_VER       - 1;
	stxp		err_c	ESP_OTA_ROLLBACK_INVALID_STATE = ESP_OTA_ROLLBACK_FAILED     - 1;
	stxp		err_c	ESP_WIFI_BASE                  = -0x3000;
	stxp		err_c	ESP_MESH_BASE                  = -0x4000;
	stxp		err_c	ESP_FLASH_BASE                 = -0x6000;
	stxp		err_c	ESP_FLASH_OP_FAIL              = ESP_FLASH_BASE    - 1;
	stxp		err_c	ESP_FLASH_OP_TIMEOUT           = ESP_FLASH_OP_FAIL - 1;
	stxp		err_c	ESP_HW_CRYPTO_BASE             = -0xc000;
	stxp		err_c	ESP_MEMPROT_BASE               = -0xd000;
} // nameespace

#ifndef if_fail
#	define	if_fail(errVal)		if(::gpk::failed(errVal))
#endif
#ifndef if_not_fail
#	define	if_not_fail(errVal)	if_not(::gpk::failed(errVal))
#endif
#ifndef if_zero
#	define	if_zero(valueToTest)		if(0 == (valueToTest))
#endif
#ifndef if_nonzero
#	define	if_nonzero(valueToTest)		if(0 != (valueToTest))
#endif
#ifndef if_failed
#	define	if_failed			if_fail
#endif
#ifndef if_not_failed
#	define	if_not_failed		if_not_fail
#endif
#ifndef if_sccs
#	define	if_sccs(errVal)	if_not_failed(errVal)
#endif

#ifdef GPK_ESP32
#	define GPK_CRASH()	do { ::gpk::u3_t * _tasdas = 0; for(::gpk::u2_t i = 0; i < 0xFFFFFFFF; ++i) { for(::gpk::u2_t j = 0; j < 1000; ++j) delay(1); base_log_print("I had to do something with this.\n"); } } while(0)	// No throw? Just crash.
#elif defined(GPK_ARDUINO) 
#	define GPK_CRASH()	do { ::gpk::u3_t * _tasdas = 0; for(::gpk::u2_t i = 0; i < 0xFFFFFFFF; ++i) { for(::gpk::u2_t j = 0; j < 1000; ++j) delay(1); base_log_print_F("I had to do something with this.\n"); } } while(0)	// No throw? Just crash.
#else
#	define GPK_CRASH()	do { ::gpk::u3_t * _tasdas = 0; for(::gpk::u2_t i = 0; i < 0xFFFFFFFF; ++i) _tasdas[i] = 0xFFFFFFFF00000000ULL; } while(0)	// No throw? Just crash.
#endif

#endif // GPK_ERROR_H_23627
