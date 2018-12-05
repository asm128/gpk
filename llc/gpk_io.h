#include <cstdio>

#ifndef GPK_IO_H_56847984984__
#define GPK_IO_H_56847984984__

#if defined(GPK_ANDROID) || defined(GPK_LINUX)
#include "gpk_log.h"
#include "gpk_string.h"

	static inline int						fopen_s					(FILE** out_fp, const char* pFilename, const char* mode)				{
		ree_if(0 == out_fp, "File pointer cannot be a null pointer. File name: %s.", pFilename);
		FILE										* fp					= fopen(pFilename, mode);
		return (0 == (*out_fp = fp)) ? -1:  0;
	}
#endif

#endif // NWOL_IO_H_56847984984__
