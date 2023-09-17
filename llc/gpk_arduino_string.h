#include "gpk_label.h"
#include "gpk_array_pod.h"

#ifndef GPK_ARDUINO_STRING_H
#define GPK_ARDUINO_STRING_H

#ifdef GPK_ARDUINO
#include <WString.h>

namespace gpk
{
	nodstin	::gpk::vcc		tovcc   (const String & srcstr)						    	{ return srcstr.length() ? ::gpk::vcc{srcstr.begin(), srcstr.length()} : vcs{}; }
	stainli	::gpk::error_t	tovcc   (::gpk::vcc		& output, const String	& srcstr)	{ return (output = ::gpk::tovcc(srcstr)).size(); }
	stainli	::gpk::error_t	tolabel (::gpk::vcc		& output, const String	& srcstr)	{ return (output = ::gpk::label(::gpk::tovcc(srcstr))).size(); }
	stainli	::gpk::error_t	toachar (::gpk::achar	& output, const String	& srcstr)	{ gpk_necs(output.reserve(srcstr.length())); return (output = ::gpk::tovcc(srcstr)).size(); }
	stainli	::gpk::error_t	append 	(::gpk::achar	& output, const String	& srcstr)	{ return output.append(::gpk::tovcc(srcstr)); }

	nodstin	::gpk::vcc		tolabel (const String & srcstr) 		{ return ::gpk::label(::gpk::tovcc(srcstr)); }
	nodstin	::gpk::achar	toachar (const String & srcstr) 		{ return ::gpk::tovcc(srcstr); }
} // namespace
#endif // GPK_ARDUINO

#endif // GPK_ARDUINO_STRING_H
