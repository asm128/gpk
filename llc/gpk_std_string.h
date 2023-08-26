#include "gpk_label.h"
#include "gpk_array_pod.h"

#include <string>

#ifndef GPK_STD_STRING_H
#define GPK_STD_STRING_H

namespace gpk
{
	nodstin	::gpk::vcc		tovcc   (const std::string & srcstr)							{ return srcstr.size() ? ::gpk::vcc{&srcstr[0], srcstr.size()} : ::gpk::vcs{}; }
	stainli	::gpk::error_t	tovcc   (::gpk::vcc		& output, const std::string & srcstr)	{ return (output = ::gpk::tovcc(srcstr)).size(); }
	stainli	::gpk::error_t	tolabel (::gpk::vcc		& output, const std::string & srcstr)	{ return (output = ::gpk::label(::gpk::tovcc(srcstr))).size(); }
	stainli	::gpk::error_t	toachar (::gpk::achar	& output, const std::string & srcstr)	{ gpk_necs(output.reserve(srcstr.length())); return (output = ::gpk::tovcc(srcstr)).size(); }
	stainli	::gpk::error_t	append 	(::gpk::achar	& output, const std::string & srcstr)	{ return output.append(::gpk::tovcc(srcstr)); }

	nodstin	::gpk::vcc		tolabel (const std::string & srcstr) 		{ return ::gpk::label(::gpk::tovcc(srcstr)); }
	nodstin	::gpk::achar	toachar (const std::string & srcstr) 		{ return ::gpk::tovcc(srcstr); }
} // namespace gpk

#endif // GPK_STD_STRING_H
