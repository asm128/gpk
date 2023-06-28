#include "gpk_aobj_pobj.h"
#include "gpk_aobj_ppod.h"
#include "gpk_array_pod.h"

#ifndef GPK_ARRAY_PTR_H_23627
#define GPK_ARRAY_PTR_H_23627

namespace gpk
{
	template<typename T> using papod	= ::gpk::pobj<::gpk::apod<T>>;
	template<typename T> using apapod	= ::gpk::aobj<::gpk::papod<T>>;

	typedef	::gpk::papod<uchar_t	>	pauchar;
	typedef	::gpk::papod<char		>	pachar;
	typedef	::gpk::papod<float		>	paf32;
	typedef	::gpk::papod<double		>	paf64;
	typedef	::gpk::papod<uint8_t	>	pau8;
	typedef	::gpk::papod<uint16_t	>	pau16;
	typedef	::gpk::papod<uint32_t	>	pau32;
	typedef	::gpk::papod<uint64_t	>	pau64;
	typedef	::gpk::papod<int8_t		>	pai8;
	typedef	::gpk::papod<int16_t	>	pai16;
	typedef	::gpk::papod<int32_t	>	pai32;
	typedef	::gpk::papod<int64_t	>	pai64;

	typedef	::gpk::aobj<::gpk::pauchar	>	apauchar;
	typedef	::gpk::aobj<::gpk::pachar	>	apachar;
	typedef	::gpk::aobj<::gpk::paf32	>	apaf32;
	typedef	::gpk::aobj<::gpk::paf64	>	apaf64;
	typedef	::gpk::aobj<::gpk::pau8		>	apau8;
	typedef	::gpk::aobj<::gpk::pau16	>	apau16;
	typedef	::gpk::aobj<::gpk::pau32	>	apau32;
	typedef	::gpk::aobj<::gpk::pau64	>	apau64;
	typedef	::gpk::aobj<::gpk::pai8		>	apai8;
	typedef	::gpk::aobj<::gpk::pai16	>	apai16;
	typedef	::gpk::aobj<::gpk::pai32	>	apai32;
	typedef	::gpk::aobj<::gpk::pai64	>	apai64;
} // namespace

#endif // GPK_ARRAY_PTR_H_23627
