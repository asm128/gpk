#include "gpk_keyval.h"
#include "gpk_array_obj.h"
#include "gpk_string.h"

#ifndef GPK_PROCESS_H_23627
#define GPK_PROCESS_H_23627

namespace gpk
{
	error_t			environmentBlockViews		(vcs0_t environmentBlock, aobj<kv<vcst_t>> & out_Views);
	error_t			environmentBlockFromEnviron	(string & environmentBlock);
} // namespace

#endif // GPK_PROCESS_H_23627


