#include "gpk_ptr_obj.h"
#include "gpk_view.h"

#ifndef GPK_VIEW_POBJ_H_23627
#define GPK_VIEW_POBJ_H_23627

namespace gpk
{
	tplt <tpnm T>	using vpobj		= ::gpk::view<::gpk::pobj<T>>;
	tplt <tpnm T>	using vcpobj	= ::gpk::view<const ::gpk::pobj<T>>;
} // namespace

#endif // GPK_VIEW_POBJ_H_23627
