#include "gpk_typeint.h"

#ifndef GPK_ARDUINO_STREAM_H
#define GPK_ARDUINO_STREAM_H

#ifdef GPK_ARDUINO
#include <Stream.h>

namespace gpk
{
	//ndsi	vcst_t		tovcc   (cnst Stream   & srcstr)							{ rtrn srcstr.length() ? vcst_t{srcstr.begin(), srcstr.length()} : vcs{}; }
	//stin	err_t	tovcc   (vcst_t		& output, cnst Stream	& srcstr)	{ rtrn (output = tovcc(srcstr)).size(); }
	//stin	err_t	tolabel (vcst_t		& output, cnst Stream	& srcstr)	{ rtrn (output = label(tovcc(srcstr))).size(); }
	//stin	err_t	toachar (asc_t	& output, cnst Stream	& srcstr)	{ gpk_necs(output.reserve(srcstr.length())); rtrn (output = tovcc(srcstr)).size(); }
	//stin	err_t	append 	(asc_t	& output, cnst Stream	& srcstr)	{ rtrn output.append(tovcc(srcstr)); }

	//ndsi	vcst_t		tolabel (cnst Stream & srcstr)	{ rtrn label(tovcc(srcstr)); }
	//ndsi	asc_t	toachar (cnst Stream & srcstr)	{ rtrn tovcc(srcstr); }
	//ndsi	vcst_t		str		(cnst Stream & srcstr)	{ rtrn tovcc(srcstr); }
	ndsi	u2_t 		size	(cnst Stream & srcstr)	{ rtrn (*((Stream*)&srcstr)).available(); }
	ndsi	u2_t 		size	(Stream & srcstr)		{ rtrn srcstr.available(); }
} // namespace
#endif // GPK_ARDUINO

#endif // GPK_ARDUINO_STREAM_H


