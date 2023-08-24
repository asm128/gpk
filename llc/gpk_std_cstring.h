#include "gpk_label.h"
#include "gpk_array_pod.h"

#include <string>

#ifndef GPK_STD_CSTRING_H
#define GPK_STD_CSTRING_H

namespace gpk
{
	::gpk::vcc		tovcc   (const char * srcstr)							{ return srcstr ? ::gpk::vcs{srcstr, (uint32_t)-1} : ::gpk::vcs{}; }
	::gpk::error_t	tovcc   (::gpk::vcc		& output, const char * srcstr)	{ return (output = ::gpk::tovcc  (srcstr)).size(); }
	::gpk::error_t	toachar (::gpk::achar	& output, const char * srcstr)	{ return (output = ::gpk::tovcc  (srcstr)).size(); }
	::gpk::error_t	tolabel (::gpk::vcc		& output, const char * srcstr)	{ return (output = ::gpk::label(::gpk::tovcc(srcstr))).size(); }
	::gpk::error_t	append 	(::gpk::achar	& output, const char * srcstr)	{
        ::gpk::vcc      vsrc    = ::gpk::tovcc(srcstr);
		gpk_necall(output.append(vsrc), "srcstr: '%s'.", srcstr ? srcstr : ""); 
		return vsrc.size();
	}

	::gpk::vcc		tolabel (const char * srcstr)	{ return ::gpk::label(::gpk::tovcc(srcstr)); }
	::gpk::achar	toachar (const char * srcstr)	{ return ::gpk::tovcc(srcstr); }
} // namespace gpk

#endif // GPK_STD_CSTRING_H
