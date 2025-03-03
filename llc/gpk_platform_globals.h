/// Copyright 2010-2023 - asm128
#ifndef GPK_PLATFORM_GLOBALS_H_23627
#define GPK_PLATFORM_GLOBALS_H_23627

#if(!defined(GPK_ESPIDF)) && (defined(ESP32) || defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_ESP32))
#	define GPK_ESPIDF
#   define GPK_FREERTOS
#   define GPK_ESP32
#endif

#if (!defined(GPK_ARDUINO))	&&	(defined(ARDUINO) || defined(__ARDUINO__) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266))
#	define GPK_ARDUINO
#endif

#if (!defined(GPK_ATMEL))	&&	defined(__ATMEGA__)
#	define GPK_ATMEL
#elif (!defined(GPK_ESP32))		&&	(defined(ESP32)		||	defined(ARDUINO_ARCH_ESP32))
#	define GPK_ESP32
#elif (!defined(GPK_ESP8266))	&&	(defined(ESP8266)	||	defined(ARDUINO_ARCH_ESP8266))
#	define GPK_ESP8266
#elif (!defined(GPK_ANDROID))	&&	(defined(ANDROID)	||	defined(__ANDROID__))
#	define GPK_ANDROID
#elif (!defined(GPK_LINUX))		&&	defined(__linux__)
#	define GPK_LINUX
#elif (!defined(GPK_WINDOWS))	&&	(defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64))
#	define GPK_WINDOWS
#endif

#if (!defined(GPK_ST)) && !defined(GPK_CMSIS) && !defined(GPK_ESP32) && !defined(GPK_ATMEL) && !defined(GPK_ARDUINO) && !defined(GPK_WINDOWS) && !defined(GPK_LINUX) && !defined(GPK_ANDROID)
#	error "Unsupported platform."
#	define GPK_UNKNOWN
#endif

#if (defined(DEBUG) || defined(_DEBUG)) && (!defined(GPK_DISABLE_DEBUG)) && !defined(GPK_DEBUG_ENABLED)
#	define GPK_DEBUG_ENABLED
#	if !defined(USE_DEBUG_BREAK_ON_ERROR_LOG)
#		define USE_DEBUG_BREAK_ON_ERROR_LOG
#	endif
#endif	// _DEBUG

#if (!defined(GPK_MTSUPPORT)) && !defined(GPK_DISABLE_MTSUPPORT)
#	define GPK_MTSUPPORT
#endif


#ifdef __GNUC__
#	define UNUSED __attribute__((__unused__))
#elif defined(GPK_WINDOWS)
#	ifdef _DEBUG
#		define UNUSED
#	else
#		define UNUSED
#	endif
#else
#	define UNUSED
#endif

#if defined(GPK_WINDOWS)
//#	pragma warning(disable : 4592)		// We needed this because of a bug in the compiler causing our static const variables to throw a warning as if its constructor was defined as constexpr.
//#	pragma warning(disable : 6326)		// Bogus potential comparison of a constant with another constant for static tplt functionality.
#	pragma warning(disable : 4706)		// Enable assignment within conditional expression. We do this constantly inside our macros in a completely valid way.
#endif

#if defined(GPK_WINDOWS)
#	if !defined(NOMINMAX)
#		define NOMINMAX
#	endif
#	if !defined(WIN32_LEAN_AND_MEAN)
#		define WIN32_LEAN_AND_MEAN
#	endif
#	define GPK_DYNAMIC_LIBRARY_EXTENSION "dll"
#else
#	define GPK_DYNAMIC_LIBRARY_EXTENSION "os"
#endif

#if defined GPK_ANDROID
#	define GPK_DISABLE_CPP_EXCEPTIONS
#endif

#ifdef GPK_WINDOWS
#	include <cstdint>
#else
#	if defined(GPK_ATMEL)
#		include <stddef.h>
#		include <stdint.h>
#	else
#		include <cstddef>
#		include <cstdint>
#	endif
#endif

// These aliases make keyword width to be consistent and makes the code less verbose.
#define dnwn			do{}while(0)
#define tydf			typedef
#define cnst			const
#define tdcs			tydf cnst
#define vltl			volatile
#define tdvl			tydf vltl
#define csvl			cnst vltl
#define tdcv			tydf csvl
#define nxpt			noexcept
#define csnx			cnst nxpt
#define sttc			static
#define stcs			sttc cnst
#define stvl			sttc vltl
#define inln			inline
#define stin			sttc inln
#define cxpr			constexpr
#define stxp			sttc cxpr
#define inxp			inln cxpr
#define sinx			stin cxpr
#define ndsc			[[nodiscard]]
#define ndcs			ndsc cnst
#define ndst			ndsc sttc
#define ndin			ndsc inln
#define ndsi			ndsc stin
#define ndxp			ndsc cxpr
#define ndsx			ndsc stxp
#define ndix			ndsc inxp
#define nsix			ndsc sinx
#define rtrn			return
#define dflt			default
#define oper			operator
#define tplt			template
#define tpnm			typename
#define sgnd			signed
#define unsd			unsigned
#define usng			using
#define stct			struct
#define clss			class
#define pblc			public
#define prtc			protected
#define prvt			private
#define lt4(lt4type)	gpk::lt4type
#define usl4(type)		usng lt4(type)

#define tdfT(type)		tydf	type	T

#define tpn(type)		tpnm	type
#define tnv(type)		tpnm...	type
#define tpnT			tpn(T)
#define tnv_tArgs  	    tnv(_tArgs)
#define tpl(type)		tplt<tpn(type)>
#define tplT			tpl(T)
#define tplT2			tpl(T2)
#define tpl_t			tpl(_t)
#define tpl_t2			tpl(_t2)
#define tpl_vtArgs		tplt<tnv_tArgs>
#define tplT_vtArgs		tplt<tpnT, tnv_tArgs>
#define tplTstct		tplT	stct
#define tplTsttc		tplT	sttc
#define tplTinln		tplT	inln
#define tplTstin		tplT	stin
#define tplTusng		tplT	usng
#define tplTndsx		tplT	ndsx
#define tplTsinx		tplT	sinx
#define tplTstxp		tplT	stxp
#define tplTnsix		tplT	nsix
#define tpl_tstct		tpl_t	stct
#define tpl_tsttc		tpl_t	sttc
#define tpl_tinln		tpl_t	inln
#define tpl_tusng		tpl_t	usng
#define tpl_tndsx		tpl_t	ndsx
#define tpl_tsinx		tpl_t	sinx
#define tpl_tstxp		tpl_t	stxp
#define tpl_tnsix		tpl_t	nsix 
#define tpl_tstin		tpl_t	stin

#define tplTInTOut		tplt<tpnm TIn, tpnm TOut>
#define tplTOutTIn		tplt<tpnm TOut, tpnm TIn>

#define cxpr_if(condition)		if cxpr(condition)
#define cxpr_if_not(condition)	if cxpr(not(condition))
#define if_not(condition)		if(not(condition))

#define GPK_STRINGIFY(x)	#x
#define GPK_TOSTRING(x)		GPK_STRINGIFY(x)

#define GPKREP0(param)	(param)
#define GPKREP1(param)	GPKREP0(param), (param)
#define GPKREP2(param)	GPKREP1(param), (param)
#define GPKREP3(param)	GPKREP2(param), (param)
#define GPKREP4(param)	GPKREP3(param), (param)
#define GPKREP5(param)	GPKREP4(param), (param)
#define GPKREP6(param)	GPKREP5(param), (param)
#define GPKREP7(param)	GPKREP6(param), (param)

#ifndef	GPK_ATMEL
#	define GPK_FMT_F32	"f"
#	define GPK_FMT_F64	"f"
#	define GPK_FMT_I32	"i"
#	define GPK_FMT_U32	"u"
#	define GPK_FMT_I64	"lli"
#	define GPK_FMT_U64	"llu"
#else
#	define GPK_FMT_F32	"f"
#	define GPK_FMT_F64	"f"
#	define GPK_FMT_I32	"li"
#	define GPK_FMT_U32	"lu"
#	define GPK_FMT_I64	"lli"
#	define GPK_FMT_U64	"llu"
#endif
#define GPK_FMT_I8			"i"
#define GPK_FMT_U8			"u"
#define GPK_FMT_I16			"i"
#define GPK_FMT_U16			"u"
#define GPK_FMT_F32_GT_F32	"%"	GPK_FMT_F32	" > %"	GPK_FMT_F32
#define GPK_FMT_F64_GT_F64	"%"	GPK_FMT_F64	" > %"	GPK_FMT_F64
#define GPK_FMT_I32_GT_I32	"%"	GPK_FMT_I32	" > %"	GPK_FMT_I32
#define GPK_FMT_U32_GT_U32	"%"	GPK_FMT_U32	" > %"	GPK_FMT_U32
#define GPK_FMT_I64_GT_I64	"%"	GPK_FMT_I64	" > %"	GPK_FMT_I64
#define GPK_FMT_U64_GT_U64	"%"	GPK_FMT_U64	" > %"	GPK_FMT_U64
#define GPK_FMT_F32_LT_F32	"%"	GPK_FMT_F32	" < %"	GPK_FMT_F32
#define GPK_FMT_F64_LT_F64	"%"	GPK_FMT_F64	" < %"	GPK_FMT_F64
#define GPK_FMT_I32_LT_I32	"%"	GPK_FMT_I32	" < %"	GPK_FMT_I32
#define GPK_FMT_U32_LT_U32	"%"	GPK_FMT_U32	" < %"	GPK_FMT_U32
#define GPK_FMT_I64_LT_I64	"%"	GPK_FMT_I64	" < %"	GPK_FMT_I64
#define GPK_FMT_U64_LT_U64	"%"	GPK_FMT_U64	" < %"	GPK_FMT_U64
#define GPK_FMT_F32_EQ_F32	"%"	GPK_FMT_F32	" == %"	GPK_FMT_F32
#define GPK_FMT_F64_EQ_F64	"%"	GPK_FMT_F64	" == %"	GPK_FMT_F64
#define GPK_FMT_I32_EQ_I32	"%"	GPK_FMT_I32	" == %"	GPK_FMT_I32
#define GPK_FMT_U32_EQ_U32	"%"	GPK_FMT_U32	" == %"	GPK_FMT_U32
#define GPK_FMT_I64_EQ_I64	"%"	GPK_FMT_I64	" == %"	GPK_FMT_I64
#define GPK_FMT_U64_EQ_U64	"%"	GPK_FMT_U64	" == %"	GPK_FMT_U64
#define GPK_FMT_F32_GE_F32	"%"	GPK_FMT_F32	" >= %"	GPK_FMT_F32
#define GPK_FMT_F64_GE_F64	"%"	GPK_FMT_F64	" >= %"	GPK_FMT_F64
#define GPK_FMT_I32_GE_I32	"%"	GPK_FMT_I32	" >= %"	GPK_FMT_I32
#define GPK_FMT_U32_GE_U32	"%"	GPK_FMT_U32	" >= %"	GPK_FMT_U32
#define GPK_FMT_I64_GE_I64	"%"	GPK_FMT_I64	" >= %"	GPK_FMT_I64
#define GPK_FMT_U64_GE_U64	"%"	GPK_FMT_U64	" >= %"	GPK_FMT_U64
#define GPK_FMT_F32_LE_F32	"%"	GPK_FMT_F32	" <= %"	GPK_FMT_F32
#define GPK_FMT_F64_LE_F64	"%"	GPK_FMT_F64	" <= %"	GPK_FMT_F64
#define GPK_FMT_I32_LE_I32	"%"	GPK_FMT_I32	" <= %"	GPK_FMT_I32
#define GPK_FMT_U32_LE_U32	"%"	GPK_FMT_U32	" <= %"	GPK_FMT_U32
#define GPK_FMT_I64_LE_I64	"%"	GPK_FMT_I64	" <= %"	GPK_FMT_I64
#define GPK_FMT_U64_LE_U64	"%"	GPK_FMT_U64	" <= %"	GPK_FMT_U64
#define GPK_FMT_F32_NE_F32	"%"	GPK_FMT_F32	" != %"	GPK_FMT_F32
#define GPK_FMT_F64_NE_F64	"%"	GPK_FMT_F64	" != %"	GPK_FMT_F64
#define GPK_FMT_I32_NE_I32	"%"	GPK_FMT_I32	" != %"	GPK_FMT_I32
#define GPK_FMT_U32_NE_U32	"%"	GPK_FMT_U32	" != %"	GPK_FMT_U32
#define GPK_FMT_I64_NE_I64	"%"	GPK_FMT_I64	" != %"	GPK_FMT_I64
#define GPK_FMT_U64_NE_U64	"%"	GPK_FMT_U64	" != %"	GPK_FMT_U64

#define GPK_FMT_EQ_F32	GPK_FMT_F32_EQ_F32
#define GPK_FMT_EQ_F64	GPK_FMT_F64_EQ_F64
#define GPK_FMT_EQ_I32	GPK_FMT_I32_EQ_I32
#define GPK_FMT_EQ_U32	GPK_FMT_U32_EQ_U32
#define GPK_FMT_EQ_I64	GPK_FMT_I64_EQ_I64
#define GPK_FMT_EQ_U64	GPK_FMT_U64_EQ_U64
#define GPK_FMT_NE_F32	GPK_FMT_F32_NE_F32
#define GPK_FMT_NE_F64	GPK_FMT_F64_NE_F64
#define GPK_FMT_NE_I32	GPK_FMT_I32_NE_I32
#define GPK_FMT_NE_U32	GPK_FMT_U32_NE_U32
#define GPK_FMT_NE_I64	GPK_FMT_I64_NE_I64
#define GPK_FMT_NE_U64	GPK_FMT_U64_NE_U64
#define GPK_FMT_GT_F32	GPK_FMT_F32_GT_F32
#define GPK_FMT_GT_F64	GPK_FMT_F64_GT_F64
#define GPK_FMT_GT_I32	GPK_FMT_I32_GT_I32
#define GPK_FMT_GT_U32	GPK_FMT_U32_GT_U32
#define GPK_FMT_GT_I64	GPK_FMT_I64_GT_I64
#define GPK_FMT_GT_U64	GPK_FMT_U64_GT_U64
#define GPK_FMT_GE_F32	GPK_FMT_F32_GE_F32
#define GPK_FMT_GE_F64	GPK_FMT_F64_GE_F64
#define GPK_FMT_GE_I32	GPK_FMT_I32_GE_I32
#define GPK_FMT_GE_U32	GPK_FMT_U32_GE_U32
#define GPK_FMT_GE_I64	GPK_FMT_I64_GE_I64
#define GPK_FMT_GE_U64	GPK_FMT_U64_GE_U64
#define GPK_FMT_LE_F32	GPK_FMT_F32_LE_F32
#define GPK_FMT_LE_F64	GPK_FMT_F64_LE_F64
#define GPK_FMT_LE_I32	GPK_FMT_I32_LE_I32
#define GPK_FMT_LE_U32	GPK_FMT_U32_LE_U32
#define GPK_FMT_LE_I64	GPK_FMT_I64_LE_I64
#define GPK_FMT_LE_U64	GPK_FMT_U64_LE_U64
#define GPK_FMT_LT_F32	GPK_FMT_F32_LT_F32
#define GPK_FMT_LT_F64	GPK_FMT_F64_LT_F64
#define GPK_FMT_LT_I32	GPK_FMT_I32_LT_I32
#define GPK_FMT_LT_U32	GPK_FMT_U32_LT_U32
#define GPK_FMT_LT_I64	GPK_FMT_I64_LT_I64
#define GPK_FMT_LT_U64	GPK_FMT_U64_LT_U64

// Legacy -- Compat
#define cnstnxpt	csnx
#define cnstvltl	csvl
#define unsgnd		unsd
#define operatr		oper
#define cnstxpr		cxpr
#define inlcxpr		inxp
#define stacxpr		stxp
#define stainli		stin
#define stincxp		sinx
#define prtctd		prtc
#define privte		prvt
#define nodscrd		ndsc
#define ndsttic		ndst
#define ndinlne		ndin
#define ndcnxpr		ndxp
#define ndstain		ndsi
#define ndstcxp		ndsx
#define ndincxp		ndix
#define ndstinx		nsix
#define tplTstruct	tplTstct
#define tplTndstcxp	tplTndsx
#define tplTstincxp	tplTsinx
#define tplTstacxpr	tplTstxp
#define tplTndstinx	tplTnsix


#endif // GPK_PLATFORM_GLOBALS_H_23627
