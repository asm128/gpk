#include "gpk_tcpip.h"
#include "gpk_array_pod.h"

#ifndef GPK_APOD_TCPIP_H
#define GPK_APOD_TCPIP_H

namespace gpk
{
	::gpk::error_t		appendString		(::gpk::achar & output, gpk::SIPv4 ip, char separator = '.');
	::gpk::error_t		appendBraced		(::gpk::achar & output, gpk::SIPv4 ip, char separator = ',');
	::gpk::error_t		appendQuoted		(::gpk::achar & output, gpk::SIPv4 ip, char separator = '.');
	::gpk::error_t		appendBracedPrefixed(::gpk::achar & output, gpk::SIPv4 ip, bool usePrefix, char prefix = ',', char ip_separator = ',');
	::gpk::error_t		appendQuotedPrefixed(::gpk::achar & output, gpk::SIPv4 ip, bool usePrefix, char prefix = ',', char ip_separator = '.');
} // namespace

#endif // GPK_APOD_TCPIP_H