#include "gpk_typeint.h"

#ifdef GPK_ATMEL
#	include <string.h>
#else
#	include <memory.h>
#endif

#ifndef GPK_EVAL_H_23627
#define GPK_EVAL_H_23627

#if defined(GPK_WINDOWS) || defined(GPK_ARDUINO) || defined(GPK_ATMEL)
#	ifdef min
#		undef min
#	endif
#	ifdef max
#		undef max
#	endif
#	if !defined(NOMINMAX)
#		define NOMINMAX
#	endif
#endif

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm T>	nsix	T&			min			(T & a, T & b)							noexcept	{ return (a < b) ? a : b; }
	tplt<tpnm T>	nsix	T&			max			(T & a, T & b)							noexcept	{ return (a > b) ? a : b; }
	tplt<tpnm T>	nsix	const T&	min			(const T & a, const T & b)				noexcept	{ return (a < b) ? a : b; }
	tplt<tpnm T>	nsix	const T&	max			(const T & a, const T & b)				noexcept	{ return (a > b) ? a : b; }
	tplt<tpnm T>	nsix	T			clamped		(T _value, const T _min, const T _max)	noexcept	{ return ::gpk::min(_max, ::gpk::max(_min, _value)); }

	// excluding the stop value
	tplt<tpnm T>	ndsx	bool		in_range	(const T & valueToTest, const T & rangeStart, const T & rangeStop)	noexcept	{ 
		return (valueToTest >= rangeStart) && (valueToTest < rangeStop);
	}
	tplt<tpnm T>	ndsc	bool		equal		(const T * other, const T * local, ::gpk::u2_t count) {
		for(::gpk::u2_t iElement = 0; iElement < count; ++iElement)
			if(other[iElement] != local[iElement])
				return false;
		return true;
	}
//#if !defined(GPK_ATMEL)
	tplt<>	ndin	bool	equal	(const double	* other, const double	* local, ::gpk::u2_t count)	{ return 0 == memcmp(other, local, szof(double	)*count); }
	tplt<>	ndin	bool	equal	(const float	* other, const float	* local, ::gpk::u2_t count)	{ return 0 == memcmp(other, local, szof(float		)*count); }
	tplt<>	ndin	bool	equal	(const int32_t	* other, const int32_t	* local, ::gpk::u2_t count)	{ return 0 == memcmp(other, local, szof(int32_t	)*count); }
	tplt<>	ndin	bool	equal	(u2_c	* other, u2_c	* local, ::gpk::u2_t count)	{ return 0 == memcmp(other, local, szof(::gpk::u2_t	)*count); }
	tplt<>	ndin	bool	equal	(const int16_t	* other, const int16_t	* local, ::gpk::u2_t count)	{ return 0 == memcmp(other, local, szof(int16_t	)*count); }
	tplt<>	ndin	bool	equal	(const ::gpk::u1_t	* other, const ::gpk::u1_t	* local, ::gpk::u2_t count)	{ return 0 == memcmp(other, local, szof(::gpk::u1_t	)*count); }
	tplt<>	ndin	bool	equal	(const int8_t	* other, const int8_t	* local, ::gpk::u2_t count)	{ return 0 == memcmp(other, local, szof(int8_t	)*count); }
	tplt<>	ndin	bool	equal	(const u0_t	* other, const u0_t	* local, ::gpk::u2_t count)	{ return 0 == memcmp(other, local, szof(u0_t	)*count); }
//#endif

#pragma pack(pop)
#ifndef truif_true_e 
#	define truif_true_e(expression)				((expression) ? true : false)	// returns true  if the parameter evaluates to true
#	define falsif_true_e(expression)				((expression) ? false : true)	// returns false if the parameter evaluates to true
#	define one_if(expression)				((expression) ? 1 : 0)			// returns one   if the parameter evaluates to true
#	define zero_if(expression)				((expression) ? 0 : 1)			// returns zero  if the parameter evaluates to true
#	define is_one(your_int)					(1 == (your_int))				// returns true if the parameter is equal to one
#	define is_zero(your_int)				(0 == (your_int))				// returns true if the parameter is equal to zero
#	define onif_true_e_false(expression)			one_if(false == (expression))
//
#	define round_up(count, denominator)		(((count) + (denominator) - 1) / (denominator))	// 
#endif

} // namespace

#endif // GPK_EVAL_H_23627


