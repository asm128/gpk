#include "gpk_label.h"
#include "gpk_array_pod.h"

#include <cstring>

#ifndef GPK_STD_CSTRING_H
#define GPK_STD_CSTRING_H

namespace gpk
{
	ndsi	vcsc_t	tovcc   (sc_c * srcstr)							{ rtrn srcstr ? vcs{srcstr, (uint32_t)-1} : vcs{}; }
	stin	err_t	tovcc   (vcsc_t		& output, sc_c * srcstr)	{ rtrn (output = tovcc(srcstr)).size(); }
	stin	err_t	tolabel (vcsc_t		& output, sc_c * srcstr)	{ rtrn (output = label(tovcc(srcstr))).size(); }
	stin	err_t	toachar (asc_t	& output, sc_c * srcstr)	{ 
		cnst vcsc_t			vsrc	= tovcc(srcstr);
		gpk_necs(output.reserve(vsrc.size())); 
		rtrn (output = vsrc).size(); 
	}
	stin	err_t	append 	(asc_t	& output, sc_c * srcstr)	{ rtrn output.append(tovcc(srcstr)); }

	ndsi	vcsc_t	tolabel (sc_c * srcstr)	{ rtrn label(tovcc(srcstr)); }
	ndsi	asc_t	toachar (sc_c * srcstr)	{ rtrn tovcc(srcstr); }
} // namespace gpk

#endif // GPK_STD_CSTRING_H
