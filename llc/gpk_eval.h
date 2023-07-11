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
	template<typename T>	inlcxpr	const T&	min			(const T & a, const T & b)	noexcept	{ return (a < b) ? a : b; }
	template<typename T>	inlcxpr	const T&	max			(const T & a, const T & b)	noexcept	{ return (a > b) ? a : b; }

	template<typename T>	stincxp	T			clamp		(const T _value, const T _min, const T _max)	noexcept	{ return ::gpk::min(_max, ::gpk::max(_min, _value)); }

	// excluding the stop value
	template<typename T>	inlcxpr	bool		in_range	(const T & valueToTest, const T & rangeStart, const T & rangeStop)	noexcept	{ return (valueToTest >= rangeStart) && (valueToTest < rangeStop);	}
	template<typename T>	bool				equal		(const T * other, const T * local, uint32_t count) {
		for(uint32_t iElement = 0; iElement < count; ++iElement)
			if(other[iElement] != local[iElement])
				return false;
		return true;
	}
#if !defined(GPK_ATMEL)
	template<>	inline	bool	equal	(const double	* other, const double	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(double	)*count); }
	template<>	inline	bool	equal	(const float	* other, const float	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(float		)*count); }
	template<>	inline	bool	equal	(const int32_t	* other, const int32_t	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(int32_t	)*count); }
	template<>	inline	bool	equal	(const uint32_t	* other, const uint32_t	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(uint32_t	)*count); }
	template<>	inline	bool	equal	(const int16_t	* other, const int16_t	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(int16_t	)*count); }
	template<>	inline	bool	equal	(const uint16_t	* other, const uint16_t	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(uint16_t	)*count); }
	template<>	inline	bool	equal	(const int8_t	* other, const int8_t	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(int8_t	)*count); }
	template<>	inline	bool	equal	(const uint8_t	* other, const uint8_t	* local, uint32_t count)	{ return 0 == memcmp(other, local, sizeof(uint8_t	)*count); }
#endif
	template<typename T> struct minmax	{ 
		T							Min, Max;	

		GPK_DEFAULT_OPERATOR(minmax<T>, Max == other.Max && Min == other.Min); 

		inlcxpr	minmax<uint8_t>		u8			()		const	noexcept	{ return Cast<uint8_t	>(); }
		inlcxpr	minmax<uint16_t>	u16			()		const	noexcept	{ return Cast<uint16_t	>(); }
		inlcxpr	minmax<uint32_t>	u32			()		const	noexcept	{ return Cast<uint32_t	>(); }
		inlcxpr	minmax<uint64_t>	u64			()		const	noexcept	{ return Cast<uint64_t	>(); }
		inlcxpr	minmax<int8_t>		i8			()		const	noexcept	{ return Cast<int8_t	>(); }
		inlcxpr	minmax<int16_t>		i16			()		const	noexcept	{ return Cast<int16_t	>(); }
		inlcxpr	minmax<int32_t>		i32			()		const	noexcept	{ return Cast<int32_t	>(); }
		inlcxpr	minmax<int64_t>		i64			()		const	noexcept	{ return Cast<int64_t	>(); }
		inlcxpr	minmax<float>		f32			()		const	noexcept	{ return Cast<float		>(); }
		inlcxpr	minmax<double>		f64			()		const	noexcept	{ return Cast<double	>(); }

		template<typename _tOther>
		minmax<_tOther>				Cast		()		const	noexcept	{
			return
				{ (_tOther)Min
				, (_tOther)Max
				};
		}
	};
	template<typename T> struct range	{ T Offset, Count;	GPK_DEFAULT_OPERATOR(range <T>, Offset == other.Offset && Count == other.Count); };
	template<typename T> struct slice	{ T Begin, End;		GPK_DEFAULT_OPERATOR(slice <T>, Begin  == other.Begin  && End   == other.End  ); };

	typedef	minmax<char		>	minmaxchar;
	typedef	minmax<uchar_t	>	minmaxuchar;
	typedef minmax<float	>	minmaxf32;
	typedef minmax<double	>	minmaxf64;
	typedef minmax<uint8_t	>	minmaxu8;
	typedef minmax<uint16_t	>	minmaxu16;
	typedef minmax<uint32_t	>	minmaxu32;
	typedef minmax<uint64_t	>	minmaxu64;
	typedef minmax<int8_t	>	minmaxi8;
	typedef minmax<int16_t	>	minmaxi16;
	typedef minmax<int32_t	>	minmaxi32;
	typedef minmax<int64_t	>	minmaxi64;

	typedef	range<char		>	rangechar;
	typedef	range<uchar_t	>	rangeuchar;
	typedef range<float		>	rangef32;
	typedef range<double	>	rangef64;
	typedef range<uint8_t	>	rangeu8;
	typedef range<uint16_t	>	rangeu16;
	typedef range<uint32_t	>	rangeu32;
	typedef range<uint64_t	>	rangeu64;
	typedef range<int8_t	>	rangei8;
	typedef range<int16_t	>	rangei16;
	typedef range<int32_t	>	rangei32;
	typedef range<int64_t	>	rangei64;

	typedef	range<char		>	slicechar;
	typedef	range<uchar_t	>	sliceuchar;
	typedef slice<float		>	slicef32;
	typedef slice<double	>	slicef64;
	typedef slice<uint8_t	>	sliceu8;
	typedef slice<uint16_t	>	sliceu16;
	typedef slice<uint32_t	>	sliceu32;
	typedef slice<uint64_t	>	sliceu64;
	typedef slice<int8_t	>	slicei8;
	typedef slice<int16_t	>	slicei16;
	typedef slice<int32_t	>	slicei32;
	typedef slice<int64_t	>	slicei64;

#pragma pack(pop)
#ifndef true_if
#	define true_if(expression)		((expression) ? true : false)	// returns true  if the parameter evaluates to true
#	define false_if(expression)		((expression) ? false : true)	// returns false if the parameter evaluates to true
#	define one_if(expression)		((expression) ? 1 : 0)			// returns one   if the parameter evaluates to true
#	define zero_if(expression)		((expression) ? 0 : 1)			// returns zero  if the parameter evaluates to true
#	define is_one(your_int)			(1 == (your_int))				// returns true if the parameter is equal to one
#	define is_zero(your_int)		(0 == (your_int))				// returns true if the parameter is equal to zero
#	define one_if_false(expression)	one_if(false == (expression));
//
#	define round_up(count, denominator)				(((count) + (denominator) - 1) / (denominator))	// 
#endif

} // namespace

#endif // GPK_EVAL_H_23627



