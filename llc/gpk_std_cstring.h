#include "gpk_label.h"
#include "gpk_string.h"

#include <cstring>

#ifndef GPK_STD_CSTRING_H
#define GPK_STD_CSTRING_H

namespace gpk
{
	ndsi	vcst_t	tovcc   (sc_c * srcstr)						{ rtrn {srcstr, (::gpk::u2_t)-1}; }
	stin	err_t	tovcc   (vcst_t	& output, sc_c * srcstr)	{ rtrn (output = tovcc(srcstr)).size(); }
	stin	err_t	tolabel (vcst_t	& output, sc_c * srcstr)	{ rtrn (output = label(tovcc(srcstr))).size(); }
	stin	err_t	toachar (string	& output, sc_c * srcstr)	{
		cnst vcst_t			vsrc	= tovcc(srcstr);
		if_fail_fe(output.reserve(vsrc.size())); 
		rtrn (output = vsrc).size(); 
	}
	stin	err_t	append 	(string	& output, sc_c * srcstr)	{ rtrn output.append(tovcc(srcstr)); }

	ndsi	vcst_t	tolabel (sc_c * srcstr)	{ rtrn label(tovcc(srcstr)); }
	ndsi	string	toachar (sc_c * srcstr)	{ rtrn tovcc(srcstr); }
} // namespace gpk

#endif // GPK_STD_CSTRING_H
