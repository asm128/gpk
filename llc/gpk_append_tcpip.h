#include "gpk_tcpip.h"
#include "gpk_string.h"

#ifndef GPK_APOD_TCPIP_H
#define GPK_APOD_TCPIP_H

namespace gpk
{
	gpk::err_t 	appendString		(::gpk::string & output, gpk::SIPv4 ip, char separator = '.');
	gpk::err_t 	appendBraced		(::gpk::string & output, gpk::SIPv4 ip, char separator = ',');
	gpk::err_t 	appendQuoted		(::gpk::string & output, gpk::SIPv4 ip, char separator = '.');
	gpk::err_t 	appendBracedPrefixed(::gpk::string & output, gpk::SIPv4 ip, bool usePrefix, char prefix = ',', char ip_separator = ',');
	gpk::err_t 	appendQuotedPrefixed(::gpk::string & output, gpk::SIPv4 ip, bool usePrefix, char prefix = ',', char ip_separator = '.');
} // namespace

#endif // GPK_APOD_TCPIP_H
