#include "gpk_keyval.h"
#include "gpk_array.h"

#ifndef GPK_PROCESS_H_23627
#define GPK_PROCESS_H_23627

namespace gpk
{
	gpk::err_t			environmentBlockViews		(::gpk::vcsc_c & environmentBlock, ::gpk::aobj<kv<vcsc_t, vcsc_t>> & out_Views);	
	gpk::err_t			environmentBlockFromEnviron	(::gpk::apod<sc_t> & environmentBlock);
} // namespace

#endif // GPK_PROCESS_H_23627
