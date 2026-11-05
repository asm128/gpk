#include "gpk_parse.h"

#ifndef GPK_STRING_TIME_H_2026
#define GPK_STRING_TIME_H_2026

namespace gpk
{
	// Local calendar bindings: yy, MM, dd, hh, mm, ss, ff (hundredths), all two digits.
	err_t	timeStringFormat	(vcst_t format, s3_t timeInMs, asc_t & output);
	err_t	timeStringFormat	(vcst_t format, view<cnst SStripLiteralType> tokens, s3_t timeInMs, asc_t & output);
}


#endif // GPK_STRING_TIME_H_2026