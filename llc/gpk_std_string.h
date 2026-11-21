#include "llc_label.h"
#include "llc_string.h"

#include <string>

#ifndef LLC_STD_STRING_H
#define LLC_STD_STRING_H

namespace llc
{
	ndsi	vcst_t	tovcc   (const std::string & srcstr)					{ return {srcstr.data(), (::llc::u2_t)srcstr.size()}; }
	stin	err_t	tovcc   (vcst_t	& output, const std::string & srcstr)	{ return (output = tovcc(srcstr)).size(); }
	stin	err_t	tolabel (vcst_t	& output, const std::string & srcstr)	{ return (output = label(tovcc(srcstr))).size(); }
	stin	err_t	toachar (string	& output, const std::string & srcstr)	{ if_fail_fe(output.reserve((::llc::u2_t)srcstr.length())); return (output = tovcc(srcstr)).size(); }
	stin	err_t	append 	(string	& output, const std::string & srcstr)	{ return output.append(tovcc(srcstr)); }

	ndsi	vcst_t	tolabel (const std::string & srcstr) 					{ return label(tovcc(srcstr)); }
	ndsi	string	toachar (const std::string & srcstr) 					{ return tovcc(srcstr); }
} // namespace llc

#endif // LLC_STD_STRING_H
