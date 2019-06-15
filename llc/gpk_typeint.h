#include "gpk_platform.h"

#include <cstdint>

#ifndef GPK_TYPEINT_H_29873490827342
#define GPK_TYPEINT_H_29873490827342

typedef					signed char						schar_t	, sbyte_t	;
typedef					char							char_t	, byte_t	;
typedef					unsigned char					uchar_t	, ubyte_t	;
namespace gpk
{
	typedef					double							float64_t;
	typedef					float							float32_t;
	static constexpr		const uint8_t					PLATFORM_BYTE_BIT_COUNT								= 8;
#if defined GPK_WINDOWS
#	if defined(_WIN64) || defined(WIN64)
#		define	REFCOUNT_T								long long
		typedef volatile REFCOUNT_T						refcount_t			;
#	else
#		define	REFCOUNT_T								long
		typedef volatile REFCOUNT_T						refcount_t			;
#	endif
#else
#	define	REFCOUNT_T								intptr_t
	typedef REFCOUNT_T								refcount_t			;
#endif

	template<class _tBase>	struct remove_cv																			{ using type = _tBase; };
	template<class _tBase>	struct remove_cv<const					_tBase>												{ using type = _tBase; };
	template<class _tBase>	struct remove_cv<volatile				_tBase>												{ using type = _tBase; };
	template<class _tBase>	struct remove_cv<const volatile			_tBase>												{ using type = _tBase; };
	template<class _tBase>	using									remove_cv_t											= typename remove_cv<_tBase>::type;

	template<class _tBase>	struct remove_volatile																		{ using type = _tBase; };
	template<class _tBase>	struct remove_volatile<const			_tBase>												{ using type = _tBase; };
	template<class _tBase>	struct remove_volatile<volatile			_tBase>												{ using type = _tBase; };
	template<class _tBase>	struct remove_volatile<const volatile	_tBase>												{ using type = _tBase; };
	template<class _tBase>	using									remove_volatile_t									= typename remove_volatile<_tBase>::type;

}

#endif // GPK_TYPEINT_H_29873490827342
