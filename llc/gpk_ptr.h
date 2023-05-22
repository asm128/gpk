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

	template <typename T> using array_pobj	= ::gpk::apobj<T>; 
	template <typename T> using array_ppod	= ::gpk::appod<T>;

	template <typename T> using vpobj		= ::gpk::view<::gpk::pobj<T>>; 
	template <typename T> using vppod		= ::gpk::view<::gpk::ppod<T>>;
	template <typename T> using vcpobj		= ::gpk::view<const ::gpk::pobj<T>>; 
	template <typename T> using vcppod		= ::gpk::view<const ::gpk::ppod<T>>;
} // namespace

#endif // GPK_PTR_H_0297349827348923
