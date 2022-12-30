#include "gpk_keyval.h"
#include "gpk_array.h"

#ifndef GPK_PROCESS_H_9823498236748932
#define GPK_PROCESS_H_9823498236748932

namespace gpk
{
	::gpk::error_t			environmentBlockViews						(const ::gpk::vcb & environmentBlock, ::gpk::aobj<TKeyValConstString> & out_Views);	
	::gpk::error_t			environmentBlockFromEnviron					(::gpk::apod<byte_t> & environmentBlock);
} // namespace

#endif // GPK_PROCESS_H_9823498236748932
