#include "gpk_arduino_stream.h"

#include "gpk_label.h"
#include "gpk_string.h"

#ifndef GPK_ARDUINO_STRING_H
#define GPK_ARDUINO_STRING_H

#ifdef GPK_ARDUINO
#	include <WString.h>

namespace gpk
{
	ndsi	vcst_t	tovcc   (cnst String & srcstr)					{ rtrn {srcstr.begin(), (u2_t)srcstr.length()}; }
	stin	err_t	tovcc   (vcst_t	& output, cnst String & srcstr)	{ rtrn (output = tovcc(srcstr)).size(); }
	stin	err_t	tolabel (vcst_t	& output, cnst String & srcstr)	{ rtrn (output = label(tovcc(srcstr))).size(); }
	stin	err_t	toachar (string	& output, cnst String & srcstr)	{ if_fail_fe(output.reserve(srcstr.length())); rtrn (output = tovcc(srcstr)).size(); }
	stin	err_t	append 	(string	& output, cnst String & srcstr)	{ rtrn output.append(tovcc(srcstr)); }

	ndsi	vcst_t	tolabel (cnst String & srcstr)	{ rtrn label(tovcc(srcstr)); }
	ndsi	string	toachar (cnst String & srcstr)	{ rtrn tovcc(srcstr); }
	ndsi	vcst_t	str		(cnst String & srcstr)	{ rtrn tovcc(srcstr); }
	ndsi	u2_t 	size	(cnst String & srcstr)	{ rtrn srcstr.length(); }
			err_t	rtrim	(String & trimmed, const String & input);
} // namespace
#endif // GPK_ARDUINO

#endif // GPK_ARDUINO_STRING_H
