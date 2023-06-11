#include "gpk_ptr_obj.h"
#include "gpk_array_obj.h"

#ifndef GPK_APOBJ_H_0297349827348923611
#define GPK_APOBJ_H_0297349827348923611

namespace gpk
{
	template <typename T> using apobj		= ::gpk::aobj<::gpk::pobj<T>>; 
	template <typename T> using paobj		= ::gpk::pobj<::gpk::aobj<T>>; 

	template <typename T> using vpobj		= ::gpk::view<::gpk::pobj<T>>; 
	template <typename T> using vcpobj		= ::gpk::view<const ::gpk::pobj<T>>; 

	template <typename T> using apo			= ::gpk::apobj<T>; 

	template <typename T> using apaobj		= ::gpk::aobj<::gpk::paobj<T>>; 
} // namespace

#endif // GPK_APOBJ_H_0297349827348923611
