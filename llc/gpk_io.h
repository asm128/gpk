#include "gpk_log.h"
#include "gpk_string.h"

#include <cstdio>

#ifndef GPK_IO_H_56847984984__
#define GPK_IO_H_56847984984__

namespace gpk
{
	stainli ::gpk::error_t	fopen_s		(FILE** out_fp, const char* pFilename, const char* mode) {
		ree_if(0 == out_fp, "File pointer cannot be a null pointer. File name: %s.", pFilename);
#if defined(GPK_WINDOWS)
		return ::fopen_s(out_fp, pFilename, mode);
#else
		FILE						* fp		= fopen(pFilename, mode);
		return (0 == (*out_fp = fp)) ? -1:  0;
#endif
	}
} // namespace

#endif // GPK_IO_H_56847984984__
