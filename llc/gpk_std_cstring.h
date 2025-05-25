#include "gpk_label.h"
#include "gpk_array_pod.h"

#include <string>

#ifndef GPK_STD_CSTRING_H
#define GPK_STD_CSTRING_H

namespace gpk
{
	ndsi	::gpk::vcc		tovcc   (const char * srcstr)							{ return srcstr ? ::gpk::vcs{srcstr, (uint32_t)-1} : ::gpk::vcs{}; }
	stin	::gpk::error_t	tovcc   (::gpk::vcc		& output, const char * srcstr)	{ return (output = ::gpk::tovcc(srcstr)).size(); }
	stin	::gpk::error_t	tolabel (::gpk::vcc		& output, const char * srcstr)	{ return (output = ::gpk::label(::gpk::tovcc(srcstr))).size(); }
	stin	::gpk::error_t	toachar (::gpk::achar	& output, const char * srcstr)	{ 
		const ::gpk::vcc			vsrc	= ::gpk::tovcc(srcstr);
		gpk_necs(output.reserve(vsrc.size())); 
		return (output = vsrc).size(); 
	}
	stin	::gpk::error_t	append 	(::gpk::achar	& output, const char * srcstr)	{ return output.append(::gpk::tovcc(srcstr)); }

	ndsi	::gpk::vcc		tolabel (const char * srcstr)	{ return ::gpk::label(::gpk::tovcc(srcstr)); }
	ndsi	::gpk::achar	toachar (const char * srcstr)	{ return ::gpk::tovcc(srcstr); }
} // namespace gpk

#endif // GPK_STD_CSTRING_H
