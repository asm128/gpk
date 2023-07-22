#include "gpk_ptr_obj.h"
#include "gpk_array_obj.h"

#ifndef GPK_APOBJ_H_23627
#define GPK_APOBJ_H_23627

namespace gpk
{
	tplt <tpnm T> using apobj		= ::gpk::aobj<::gpk::pobj<T>>; 
	tplt <tpnm T> using paobj		= ::gpk::pobj<::gpk::aobj<T>>; 

	tplt <tpnm T> using vpobj		= ::gpk::view<::gpk::pobj<T>>; 
	tplt <tpnm T> using vcpobj		= ::gpk::view<const ::gpk::pobj<T>>; 

	tplt <tpnm T> using apo			= ::gpk::apobj<T>; 

	tplt <tpnm T> using apaobj		= ::gpk::aobj<::gpk::paobj<T>>; 
	tplt <tpnm T> using aapobj		= ::gpk::aobj<::gpk::apobj<T>>; 
} // namespace

#endif // GPK_APOBJ_H_23627
