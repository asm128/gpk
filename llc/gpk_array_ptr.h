#include "gpk_aobj_pobj.h"
#include "gpk_aobj_ppod.h"
#include "gpk_array_pod.h"

#ifndef GPK_ARRAY_PTR_H_0297349827348923611
#define GPK_ARRAY_PTR_H_0297349827348923611

namespace gpk
{
	template <typename T> using papod	= ::gpk::pobj<::gpk::apod<T>>;
	template <typename T> using apapod	= ::gpk::aobj<::gpk::papod<T>>;

	typedef	::gpk::apapod<uchar_t	>	apauchar;
	typedef	::gpk::apapod<char		>	apachar;
	typedef	::gpk::apapod<float		>	apaf32;
	typedef	::gpk::apapod<double	>	apaf64;
	typedef	::gpk::apapod<uint8_t	>	apau8;
	typedef	::gpk::apapod<uint16_t	>	apau16;
	typedef	::gpk::apapod<uint32_t	>	apau32;
	typedef	::gpk::apapod<uint64_t	>	apau64;
	typedef	::gpk::apapod<int8_t	>	apai8;
	typedef	::gpk::apapod<int16_t	>	apai16;
	typedef	::gpk::apapod<int32_t	>	apai32;
	typedef	::gpk::apapod<int64_t	>	apai64;
} // namespace

#endif // GPK_ARRAY_PTR_H_0297349827348923617
