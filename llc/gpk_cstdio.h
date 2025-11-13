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
	GDEFINE_ENUM_TYPE(FSEEK, uint8_t);
	GDEFINE_ENUM_VALUE(FSEEK, SET, SEEK_SET);
	GDEFINE_ENUM_VALUE(FSEEK, CUR, SEEK_CUR);
	GDEFINE_ENUM_VALUE(FSEEK, END, SEEK_END);
#ifdef GPK_WINDOWS
	ndsi err_t	fseek		(FILE * fp, int64_t offset, FSEEK origin) { if_true_vef(-1, ::_fseeki64(fp, offset, origin), "%p, %" GPK_FMT_S3 ", %X'%s'.", fp, offset, origin, get_value_namep(origin)); return 0; }
#else
	ndsi err_t	fseek		(FILE * fp, int64_t offset, FSEEK origin) { if_true_vef(-1, ::fseek(fp, offset, origin), "%p, %" GPK_FMT_S3 ", %X'%s'.", fp, offset, origin, get_value_namep(origin)); return 0; }
#endif
	// fopen_s
	ndsi err_t	fopen_s		(FILE* * out_fp, vcs filename, vcs mode) {
		rees_if(0 == filename.size());
		ree_if(0 == out_fp			, "'%s'", filename.begin());
		ree_if(0 == mode.size()		, "'%s'", filename.begin());
#ifndef GPK_WINDOWS
		return (0 == (*out_fp = fopen(filename, mode))) ? -(errno) : 0;
#else
		::gpk::ERRNO	result;
		if_true_vef(result, result = (::gpk::ERRNO)-abs(::fopen_s(out_fp, filename, mode)), "%X:%u:%i:'%s'<-['%s','%s'].", GPKREP2(result), get_value_namep(result), get_value_descp(result), filename.begin(), mode.begin());
		return 0;
#endif
	}
	ndsi			err_t	fopen_s		(FILE* * out_fp, vs filename, vcs mode)						{ return fopen_s(out_fp, filename.cc(), mode); }
	tplN1u	ndsi	err_t	fopen_s		(FILE* * out_fp, sc_c (&filename)[N], vcs mode)				{ return fopen_s(out_fp, vcsc_t{filename}, mode); }
	ndsi			err_t	fopen_s		(FILE* * out_fp, vcs filename, vcs mode, uint64_t offset)	{ gpk_necs(::gpk::fopen_s(out_fp, filename, mode)); return 0 == offset ? 0 : ::gpk::fseek(*out_fp, offset, FSEEK_SET); }
} // namespace

#endif // GPK_CSTDIO_H
