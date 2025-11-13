#include "gpk_label.h"
#include "gpk_array_pod.h"

#ifndef GPK_ARDUINO_STRING_H
#define GPK_ARDUINO_STRING_H

#ifdef GPK_ARDUINO
#include <WString.h>

namespace gpk
{
	ndstain	::gpk::vcsc_t		tovcc   (const String & srcstr)						    	{ return srcstr.length() ? ::gpk::vcsc_t{srcstr.begin(), srcstr.length()} : vcs{}; }
	stainli	::gpk::error_t	tovcc   (::gpk::vcsc_t		& output, const String	& srcstr)	{ return (output = ::gpk::tovcc(srcstr)).size(); }
	stainli	::gpk::error_t	tolabel (::gpk::vcsc_t		& output, const String	& srcstr)	{ return (output = ::gpk::label(::gpk::tovcc(srcstr))).size(); }
	stainli	::gpk::error_t	toachar (::gpk::asc_t	& output, const String	& srcstr)	{ gpk_necs(output.reserve(srcstr.length())); return (output = ::gpk::tovcc(srcstr)).size(); }
	stainli	::gpk::error_t	append 	(::gpk::asc_t	& output, const String	& srcstr)	{ return output.append(::gpk::tovcc(srcstr)); }

	ndstain	::gpk::vcsc_t		tolabel (const String & srcstr) 		{ return ::gpk::label(::gpk::tovcc(srcstr)); }
	ndstain	::gpk::asc_t	toachar (const String & srcstr) 		{ return ::gpk::tovcc(srcstr); }
} // namespace
#endif // GPK_ARDUINO

#endif // GPK_ARDUINO_STRING_H
