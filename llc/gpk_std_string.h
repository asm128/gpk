#include "gpk_label.h"
#include "gpk_string.h"

#include <string>

#ifndef GPK_STD_STRING_H
#define GPK_STD_STRING_H

namespace gpk
{
	ndsi	vcst_t	tovcc   (const std::string & srcstr)					{ return {srcstr.data(), (::gpk::u2_t)srcstr.size()}; }
	stin	error_t	tovcc   (vcst_t	& output, const std::string & srcstr)	{ return (output = tovcc(srcstr)).size(); }
	stin	error_t	tolabel (vcst_t	& output, const std::string & srcstr)	{ return (output = label(tovcc(srcstr))).size(); }
	stin	error_t	toachar (string	& output, const std::string & srcstr)	{ gpk_necs(output.reserve((::gpk::u2_t)srcstr.length())); return (output = tovcc(srcstr)).size(); }
	stin	error_t	append 	(string	& output, const std::string & srcstr)	{ return output.append(tovcc(srcstr)); }

	ndsi	vcst_t	tolabel (const std::string & srcstr) 					{ return label(tovcc(srcstr)); }
	ndsi	string	toachar (const std::string & srcstr) 					{ return tovcc(srcstr); }
} // namespace gpk

#endif // GPK_STD_STRING_H


