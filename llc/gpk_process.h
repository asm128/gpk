#include "gpk_keyval.h"
#include "gpk_array.h"

#ifndef GPK_PROCESS_H_23627
#define GPK_PROCESS_H_23627

namespace gpk
{
	::gpk::error_t			environmentBlockViews		(::gpk::vcsc_c & environmentBlock, ::gpk::aobj<TKeyValConstString> & out_Views);	
	::gpk::error_t			environmentBlockFromEnviron	(::gpk::apod<sc_t> & environmentBlock);
} // namespace

#endif // GPK_PROCESS_H_23627
