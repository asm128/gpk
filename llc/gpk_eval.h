#include "gpk_typeint.h"

#if !defined(GPK_ATMEL)
#include <memory.h>
#endif

#ifndef GPK_EVAL_H_23627
#define GPK_EVAL_H_23627

#if defined(GPK_WINDOWS)
#	if defined(min)
#		undef min
#	endif
#	if defined(max)
#		undef max
#	endif
#	if !defined(NOMINMAX)
#		define NOMINMAX
#	endif
#endif

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm T>	stincxp	T&			min			(T & a, T & b)							noexcept	{ return (a < b) ? a : b; }
	tplt<tpnm T>	stincxp	T&			max			(T & a, T & b)							noexcept	{ return (a > b) ? a : b; }
	tplt<tpnm T>	stincxp	const T&	min			(const T & a, const T & b)				noexcept	{ return (a < b) ? a : b; }
	tplt<tpnm T>	stincxp	const T&	max			(const T & a, const T & b)				noexcept	{ return (a > b) ? a : b; }
	tplt<tpnm T>	stincxp	T			clamped		(T _value, const T _min, const T _max)	noexcept	{ return ::gpk::min(_max, ::gpk::max(_min, _value)); }

	// excluding the stop value
	tplt<tpnm T>	inlcxpr	bool		in_range	(const T & valueToTest, const T & rangeStart, const T & rangeStop)	noexcept	{ 
		return (valueToTest >= rangeStart) && (valueToTest < rangeStop);
	}
	tplt<tpnm T>	bool				equal		(const T * other, const T * local, uint32_t count) {
		for(uint32_t iElement = 0; iElement < count; ++iElement)
			if(other[iElement] != local[iElement])
				return false;
		return true;
	}
#if !defined(GPK_ATMEL)
	tplt<>	inline	bool	equal	(const double	* other, const double	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(double	)*count); }
	tplt<>	inline	bool	equal	(const float	* other, const float	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(float		)*count); }
	tplt<>	inline	bool	equal	(const int32_t	* other, const int32_t	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(int32_t	)*count); }
	tplt<>	inline	bool	equal	(const uint32_t	* other, const uint32_t	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(uint32_t	)*count); }
	tplt<>	inline	bool	equal	(const int16_t	* other, const int16_t	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(int16_t	)*count); }
	tplt<>	inline	bool	equal	(const uint16_t	* other, const uint16_t	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(uint16_t	)*count); }
	tplt<>	inline	bool	equal	(const int8_t	* other, const int8_t	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(int8_t	)*count); }
	tplt<>	inline	bool	equal	(const uint8_t	* other, const uint8_t	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(uint8_t	)*count); }
#endif

#pragma pack(pop)
#ifndef true_if
#	define true_if(expression)				((expression) ? true : false)	// returns true  if the parameter evaluates to true
#	define false_if(expression)				((expression) ? false : true)	// returns false if the parameter evaluates to true
#	define one_if(expression)				((expression) ? 1 : 0)			// returns one   if the parameter evaluates to true
#	define zero_if(expression)				((expression) ? 0 : 1)			// returns zero  if the parameter evaluates to true
#	define is_one(your_int)					(1 == (your_int))				// returns true if the parameter is equal to one
#	define is_zero(your_int)				(0 == (your_int))				// returns true if the parameter is equal to zero
#	define one_if_false(expression)			one_if(false == (expression))
//
#	define round_up(count, denominator)		(((count) + (denominator) - 1) / (denominator))	// 
#endif

} // namespace

#endif // GPK_EVAL_H_23627



