#include "gpk_append_tcpip.h"
#include "gpk_string_compose.h"

::gpk::error_t	gpk::appendString			(::gpk::asc_t & output, gpk::SIPv4 ip, char separator) { 
	char				buffer[16]	= {};
	::gpk::error_t		len;
	gpk_necs(len = ip.print(buffer, separator));
	gpk_necs(output.append_string(buffer));
	return len;
}

::gpk::error_t	gpk::appendBraced			(::gpk::asc_t & output, gpk::SIPv4 ip, char separator) { 
	char				buffer[16]	= {};
	gpk_necs(ip.print(buffer, separator));
	return ::gpk::appendBraced(output, buffer);
}

::gpk::error_t	gpk::appendQuoted			(::gpk::asc_t & output, gpk::SIPv4 ip, char separator) { 
	char				buffer[16]	= {};
	gpk_necs(ip.print(buffer, separator));
	return ::gpk::appendQuoted(output, buffer);
}

::gpk::error_t	gpk::appendBracedPrefixed	(::gpk::asc_t & output, gpk::SIPv4 ip, bool usePrefix, char prefix, char ip_separator) { 
	if(false == usePrefix) 
		return ::gpk::appendBraced(output, ip, ip_separator); 
	gpk_necs(output.push_back(prefix)); 
	return 1 + ::gpk::appendBraced(output, ip, ip_separator); 
}

::gpk::error_t	gpk::appendQuotedPrefixed	(::gpk::asc_t & output, gpk::SIPv4 ip, bool usePrefix, char prefix, char ip_separator) { 
	if(false == usePrefix) 
		return ::gpk::appendQuoted(output, ip, ip_separator); 
	gpk_necs(output.push_back(prefix)); 
	return 1 + ::gpk::appendQuoted(output, ip, ip_separator); 
}
