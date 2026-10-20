/// Copyright 2010-2024 - ogarnd
#include "gpk_view_pobj.h"
#include "gpk_array_obj.h"

#ifndef GPK_APOBJ_H_23627
#define GPK_APOBJ_H_23627

namespace gpk
{
	tplT using apobj		= ::gpk::aobj<::gpk::pobj<T>>; 
	tplT using paobj		= ::gpk::pobj<::gpk::aobj<T>>; 

	tplT using apo			= ::gpk::apobj<T>; 

	tplT using apaobj		= ::gpk::aobj<::gpk::paobj<T>>; 
	tplT using aapobj		= ::gpk::aobj<::gpk::apobj<T>>; 
} // namespace

#endif // GPK_APOBJ_H_23627


