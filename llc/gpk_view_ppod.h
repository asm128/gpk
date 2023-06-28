#include "gpk_ptr_pod.h"
#include "gpk_view.h"

#ifndef GPK_VIEW_PPOD_H_23627
#define GPK_VIEW_PPOD_H_23627

namespace gpk
{
	template <typename T>	using vppod		= ::gpk::view<::gpk::ppod<T>>;
	template <typename T>	using vcppod	= ::gpk::view<const ::gpk::ppod<T>>;
} // namespace

#endif // GPK_VIEW_PPOD_H_23627
