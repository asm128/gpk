#include "gpk_ptr_obj.h"
#include "gpk_view.h"

#ifndef GPK_VIEW_POBJ_H_23627
#define GPK_VIEW_POBJ_H_23627

namespace gpk
{
	tplT using	vpobj	= ::gpk::view<::gpk::pobj<T>>;
	tplT using	vcpobj	= ::gpk::view<cnst ::gpk::pobj<T>>;
} // namespace

#endif // GPK_VIEW_POBJ_H_23627
