/// Copyright 2010-2023 - asm128
#ifndef GPK_PLATFORM_GLOBALS_H_23627
#define GPK_PLATFORM_GLOBALS_H_23627

#if (!defined(GPK_ANDROID)) && (defined(ANDROID) || defined(__ANDROID__))
#	define GPK_ANDROID
#elif (!defined(GPK_LINUX)) && defined(__linux__)
#	define GPK_LINUX
#elif (!defined(GPK_ESP32)) && (defined(ESP32) || defined(ARDUINO_ARCH_ESP32))
#	define GPK_ESP32
#elif (!defined(GPK_ARDUINO)) && defined(__ARDUINO__)
#	define GPK_ARDUINO
#elif (!defined(GPK_WINDOWS)) && (defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64))
#	define GPK_WINDOWS
#elif !defined(GPK_ST) && !defined(GPK_CMSIS) && !defined(GPK_ESP32) && !defined(GPK_ATMEL) && !defined(GPK_ARDUINO)
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

#if defined(GPK_ATMEL)
//#	include "initializer_list.h"
#else
#	include <initializer_list>
#endif

// These aliases make keyword width to be consistent and makes the code less verbose.
#define vltl	volatile
#define tpnm	typename
#define tplt	template

#define cnstxpr	constexpr
#define inlcxpr	inline cnstxpr
#define stacxpr	static cnstxpr
#define stainli	static inline
#define stincxp	stainli	cnstxpr

#define nodscrd [[nodiscard]]
#define nodinli nodscrd inline
#define nodcxpr nodscrd cnstxpr
#define ndincxp nodscrd inlcxpr
#define nodstin nodscrd stainli
#define nodstxp nodscrd stacxpr
#define ndstinx nodstin cnstxpr

#define	GPK_STRINGIFY(x) #x
#define	GPK_TOSTRING(x)  GPK_STRINGIFY(x)

#ifndef GPK_ATMEL
#	define GPK_FMT_F32 "f"
#	define GPK_FMT_F64 "f"
#	define GPK_FMT_I32 "i"
#	define GPK_FMT_U32 "u"
#	define GPK_FMT_I64 "lli"
#	define GPK_FMT_U64 "llu"
#else
#	define GPK_FMT_F32 "f"
#	define GPK_FMT_F64 "f"
#	define GPK_FMT_I32 "li"
#	define GPK_FMT_U32 "lu"
#	define GPK_FMT_I64 "lli"
#	define GPK_FMT_U64 "llu"
#endif

#define GPK_FMT_F32_GT_F32 "%" GPK_FMT_F32 " > %" GPK_FMT_F32
#define GPK_FMT_F64_GT_F64 "%" GPK_FMT_F64 " > %" GPK_FMT_F64
#define GPK_FMT_I32_GT_I32 "%" GPK_FMT_I32 " > %" GPK_FMT_I32
#define GPK_FMT_U32_GT_U32 "%" GPK_FMT_U32 " > %" GPK_FMT_U32
#define GPK_FMT_I64_GT_I64 "%" GPK_FMT_I64 " > %" GPK_FMT_I64
#define GPK_FMT_U64_GT_U64 "%" GPK_FMT_U64 " > %" GPK_FMT_U64

#define GPK_FMT_F32_LT_F32 "%" GPK_FMT_F32 " < %" GPK_FMT_F32
#define GPK_FMT_F64_LT_F64 "%" GPK_FMT_F64 " < %" GPK_FMT_F64
#define GPK_FMT_I32_LT_I32 "%" GPK_FMT_I32 " < %" GPK_FMT_I32
#define GPK_FMT_U32_LT_U32 "%" GPK_FMT_U32 " < %" GPK_FMT_U32
#define GPK_FMT_I64_LT_I64 "%" GPK_FMT_I64 " < %" GPK_FMT_I64
#define GPK_FMT_U64_LT_U64 "%" GPK_FMT_U64 " < %" GPK_FMT_U64

#define GPK_FMT_F32_EQ_F32 "%" GPK_FMT_F32 " == %" GPK_FMT_F32
#define GPK_FMT_F64_EQ_F64 "%" GPK_FMT_F64 " == %" GPK_FMT_F64
#define GPK_FMT_I32_EQ_I32 "%" GPK_FMT_I32 " == %" GPK_FMT_I32
#define GPK_FMT_U32_EQ_U32 "%" GPK_FMT_U32 " == %" GPK_FMT_U32
#define GPK_FMT_I64_EQ_I64 "%" GPK_FMT_I64 " == %" GPK_FMT_I64
#define GPK_FMT_U64_EQ_U64 "%" GPK_FMT_U64 " == %" GPK_FMT_U64

#define GPK_FMT_F32_GE_F32 "%" GPK_FMT_F32 " >= %" GPK_FMT_F32
#define GPK_FMT_F64_GE_F64 "%" GPK_FMT_F64 " >= %" GPK_FMT_F64
#define GPK_FMT_I32_GE_I32 "%" GPK_FMT_I32 " >= %" GPK_FMT_I32
#define GPK_FMT_U32_GE_U32 "%" GPK_FMT_U32 " >= %" GPK_FMT_U32
#define GPK_FMT_I64_GE_I64 "%" GPK_FMT_I64 " >= %" GPK_FMT_I64
#define GPK_FMT_U64_GE_U64 "%" GPK_FMT_U64 " >= %" GPK_FMT_U64

#define GPK_FMT_F32_LE_F32 "%" GPK_FMT_F32 " <= %" GPK_FMT_F32
#define GPK_FMT_F64_LE_F64 "%" GPK_FMT_F64 " <= %" GPK_FMT_F64
#define GPK_FMT_I32_LE_I32 "%" GPK_FMT_I32 " <= %" GPK_FMT_I32
#define GPK_FMT_U32_LE_U32 "%" GPK_FMT_U32 " <= %" GPK_FMT_U32
#define GPK_FMT_I64_LE_I64 "%" GPK_FMT_I64 " <= %" GPK_FMT_I64
#define GPK_FMT_U64_LE_U64 "%" GPK_FMT_U64 " <= %" GPK_FMT_U64

namespace gpk
{
#if defined(GPK_ANDROID) || defined(GPK_CLANG)
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wtautological-constant-out-of-range-compare"
#endif
	// -- Returns 0 on little-endian machines

	nodstin int		test_endianness		()	noexcept	{ stacxpr uint16_t test = 0xFF00U; return (((const  char*)&test)[0] == 0xFFU) ? 1 : 0; }

#if defined(GPK_ANDROID) || defined(GPK_CLANG)
#	pragma clang diagnostic pop
#endif
}

#endif // GPK_PLATFORM_GLOBALS_H_23627
