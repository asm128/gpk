#include "gpk_ptr_obj.h"
#include "gpk_view.h"

#ifndef GPK_VIEW_POBJ_H_0297349827348923611
#define GPK_VIEW_POBJ_H_0297349827348923611

namespace gpk
{
	template <typename T>	using vpobj		= ::gpk::view<::gpk::pobj<T>>;
	template <typename T>	using vcpobj	= ::gpk::view<const ::gpk::pobj<T>>;
} // namespace

#endif // GPK_VIEW_POBJ_H_0297349827348923611
