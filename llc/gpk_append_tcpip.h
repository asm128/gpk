#include "gpk_tcpip.h"
#include "gpk_string.h"

#ifndef GPK_APOD_TCPIP_H
#define GPK_APOD_TCPIP_H

nmsp gpk
{
	err_t 	appendString		(string & output, SIPv4 ip, char separator = '.');
	err_t 	appendBraced		(string & output, SIPv4 ip, char separator = ',');
	err_t 	appendQuoted		(string & output, SIPv4 ip, char separator = '.');
	err_t 	appendBracedPrefixed(string & output, SIPv4 ip, bool usePrefix, char prefix = ',', char ip_separator = ',');
	err_t 	appendQuotedPrefixed(string & output, SIPv4 ip, bool usePrefix, char prefix = ',', char ip_separator = '.');
} // nmsp

#endif // GPK_APOD_TCPIP_H
