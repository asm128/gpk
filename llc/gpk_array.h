#include "gpk_array_pod.h"
#include "gpk_array_obj.h"

#ifndef GPK_ARRAY_H_23610
#define GPK_ARRAY_H_23610

namespace gpk
{
	template <typename T>	using aaobj		= ::gpk::aobj<::gpk::aobj<T>>; 
	template <typename T>	using aapod		= ::gpk::aobj<::gpk::apod<T>>; 

	typedef	::gpk::aapod<uchar_t	>	aauc;
	typedef	::gpk::aapod<char		>	aac;
	typedef	::gpk::aapod<float		>	aaf32, aaf;
	typedef	::gpk::aapod<double		>	aaf64, aad;
	typedef	::gpk::aapod<uint8_t	>	aau8;
	typedef	::gpk::aapod<uint16_t	>	aau16;
	typedef	::gpk::aapod<uint32_t	>	aau32;
	typedef	::gpk::aapod<uint64_t	>	aau64;
	typedef	::gpk::aapod<int8_t		>	aai8;
	typedef	::gpk::aapod<int16_t	>	aai16;
	typedef	::gpk::aapod<int32_t	>	aai32;
	typedef	::gpk::aapod<int64_t	>	aai64;
} // namespace 

#endif // GPK_ARRAY_H_23610
