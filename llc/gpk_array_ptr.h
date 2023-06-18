#include "gpk_aobj_pobj.h"
#include "gpk_aobj_ppod.h"
#include "gpk_array_pod.h"

#ifndef GPK_ARRAY_PTR_H_0297349827348923611
#define GPK_ARRAY_PTR_H_0297349827348923611

namespace gpk
{
	template <typename T> using papod	= ::gpk::pobj<::gpk::apod<T>>;
	template <typename T> using apapod	= ::gpk::aobj<::gpk::papod<T>>;

	typedef	::gpk::apapod<uchar_t	>	apapoduc;
	typedef	::gpk::apapod<char		>	apapodc;
	typedef	::gpk::apapod<float		>	apapodf32, apapodf;
	typedef	::gpk::apapod<double	>	apapodf64, apapodd;
	typedef	::gpk::apapod<uint8_t	>	apapodu8;
	typedef	::gpk::apapod<uint16_t	>	apapodu16;
	typedef	::gpk::apapod<uint32_t	>	apapodu32;
	typedef	::gpk::apapod<uint64_t	>	apapodu64;
	typedef	::gpk::apapod<int8_t	>	apapodi8;
	typedef	::gpk::apapod<int16_t	>	apapodi16;
	typedef	::gpk::apapod<int32_t	>	apapodi32;
	typedef	::gpk::apapod<int64_t	>	apapodi64;
} // namespace

#endif // GPK_ARRAY_PTR_H_0297349827348923617
