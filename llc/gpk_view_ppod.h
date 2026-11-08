#include "gpk_ptr_pod.h"
#include "gpk_view.h"

#ifndef GPK_VIEW_PPOD_H_23627
#define GPK_VIEW_PPOD_H_23627

nmsp gpk
{
	tplT usng	vppod	= ::gpk::view<::gpk::ppod<T>>;
	tplT usng	vcppod	= ::gpk::view<cnst ::gpk::ppod<T>>;
} // nmsp

#endif // GPK_VIEW_PPOD_H_23627
