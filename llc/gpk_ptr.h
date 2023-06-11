#include "gpk_ptr_obj.h"
#include "gpk_ptr_pod.h"
#include "gpk_array.h"

#ifndef GPK_PTR_H_0297349827348923
#define GPK_PTR_H_0297349827348923

namespace gpk
{
	template <typename T> using apobj		= ::gpk::aobj<::gpk::pobj<T>>; 
	template <typename T> using appod		= ::gpk::aobj<::gpk::ppod<T>>;
	template <typename T> using paobj		= ::gpk::pobj<::gpk::aobj<T>>; 
	template <typename T> using papod		= ::gpk::pobj<::gpk::apod<T>>;

	template <typename T> using apo			= ::gpk::apobj<T>; 
	template <typename T> using app			= ::gpk::appod<T>;
	template <typename T> using pao			= ::gpk::paobj<T>; 
	template <typename T> using pap			= ::gpk::papod<T>;

	template <typename T> using vpobj		= ::gpk::view<::gpk::pobj<T>>; 
	template <typename T> using vppod		= ::gpk::view<::gpk::ppod<T>>;
	template <typename T> using vcpobj		= ::gpk::view<const ::gpk::pobj<T>>; 
	template <typename T> using vcppod		= ::gpk::view<const ::gpk::ppod<T>>;

	template <typename T> using apaobj		= ::gpk::aobj<::gpk::paobj<T>>; 
	template <typename T> using apapod		= ::gpk::aobj<::gpk::papod<T>>;

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

#endif // GPK_PTR_H_0297349827348923
