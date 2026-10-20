#include "gpk_cerrno.h"
#include "gpk_cstring.h"

#ifdef GPK_ATMEL
#	include <stdio.h>
#else
#	include <cstdio>
#endif

#ifndef GPK_CSTDIO_H
#define GPK_CSTDIO_H

namespace gpk
{
#ifdef GPK_WINDOWS
	ndsi int64_t	ftell		(FILE * fp) { return ::_ftelli64(fp); }
#else
	ndsi int64_t	ftell		(FILE * fp) { return ::ftell(fp); }
#endif
	// fseek
	GDEFINE_ENUM_TYPE(FSEEK, u0_t);
	GDEFINE_ENUM_VALUE(FSEEK, SET, SEEK_SET);
	GDEFINE_ENUM_VALUE(FSEEK, CUR, SEEK_CUR);
	GDEFINE_ENUM_VALUE(FSEEK, END, SEEK_END);
#ifdef GPK_WINDOWS
	ndsi err_t		fseek		(FILE * fp, int64_t offset, FSEEK origin) { if_true_vef(-1, ::_fseeki64(fp, offset, origin), "%p, %" GPK_FMT_S3 ", %X'%s'.", fp, offset, origin, get_value_namep(origin)); return 0; }
#else
	ndsi err_t		fseek		(FILE * fp, int64_t offset, FSEEK origin) { if_true_vef(-1, ::fseek(fp, offset, origin), "%p, %" GPK_FMT_S3 ", %X'%s'.", fp, offset, origin, get_value_namep(origin)); return 0; }
#endif
	ndsi err_t		fopen_s		(FILE* & out_fp, vcst_t filename, vcst_t mode, uint64_t offset = 0)	{
		if_true_fe(0 == filename.size());
		if_true_fef(0 == mode.size()		, "'%.*s'", GPK_VIEW_ARGS(filename));
#ifndef GPK_WINDOWS
		if_null_vef(-(errno), out_fp = fopen(filename, mode), "['%.*s','%.*s'].", GPK_VIEW_ARGS(filename), GPK_VIEW_ARGS(mode));
#else
		ERRNO	result;
		if_true_vef(-result, result = (ERRNO)abs(::fopen_s(&out_fp, filename, mode)), "%X:%u:%i:'%s':'%s'<-['%.*s','%.*s'].", GPKREP2(result), get_value_namep(result), get_value_descp(result), GPK_VIEW_ARGS(filename), GPK_VIEW_ARGS(mode));
#endif
		return 0 == offset ? 0 : fseek(out_fp, offset, FSEEK_SET);
	}
} // namespace

#endif // GPK_CSTDIO_H


