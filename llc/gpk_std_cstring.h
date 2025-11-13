#include "gpk_label.h"
#include "gpk_array_pod.h"

#include <string>

#ifndef GPK_STD_CSTRING_H
#define GPK_STD_CSTRING_H

namespace gpk
{
	ndsi	::gpk::vcsc_t		tovcc   (const char * srcstr)							{ return srcstr ? ::gpk::vcs{srcstr, (uint32_t)-1} : ::gpk::vcs{}; }
	stin	::gpk::error_t	tovcc   (::gpk::vcsc_t		& output, const char * srcstr)	{ return (output = ::gpk::tovcc(srcstr)).size(); }
	stin	::gpk::error_t	tolabel (::gpk::vcsc_t		& output, const char * srcstr)	{ return (output = ::gpk::label(::gpk::tovcc(srcstr))).size(); }
	stin	::gpk::error_t	toachar (::gpk::asc_t	& output, const char * srcstr)	{ 
		const ::gpk::vcsc_t			vsrc	= ::gpk::tovcc(srcstr);
		gpk_necs(output.reserve(vsrc.size())); 
		return (output = vsrc).size(); 
	}
	stin	::gpk::error_t	append 	(::gpk::asc_t	& output, const char * srcstr)	{ return output.append(::gpk::tovcc(srcstr)); }

	ndsi	::gpk::vcsc_t		tolabel (const char * srcstr)	{ return ::gpk::label(::gpk::tovcc(srcstr)); }
	ndsi	::gpk::asc_t	toachar (const char * srcstr)	{ return ::gpk::tovcc(srcstr); }
} // namespace gpk

#endif // GPK_STD_CSTRING_H
