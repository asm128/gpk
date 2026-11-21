#include "gpk_string_tcpip.h"
#include "gpk_parse.h"
#include "gpk_json.h"


gpk::err_t			gpk::tcpipAddress		(vcst_t strIP, u2_t & address, uint16_t & port) {
	u2_t					iOffset					= tcpipAddress(strIP, address);
	return (iOffset < strIP.size())
		? iOffset + parseIntegerDecimal({&strIP[iOffset], strIP.size() - iOffset}, port)
		: iOffset
		;
}

gpk::err_t			jsonTcpipAddress		(gpk::vcst_t strIP, gpk::u2_t & ipv4) {
	gpk::SJSONReader			reader					= {};
	if_fail_fef(gpk::jsonParse(reader, strIP), "Failed to parse ip from string: '%.*s'.", GPK_VIEW_ARGS(strIP));
	ipv4					= 0;
	{ int32_t iToken; if_fail_fe(iToken = jsonArrayValueGet(reader, 0, 0)); ipv4 |= gpk::byte_to<gpk::u2_t>((uint8_t)reader[iToken]->Token->Value, 0); }
	{ int32_t iToken; if_fail_fe(iToken = jsonArrayValueGet(reader, 0, 1)); ipv4 |= gpk::byte_to<gpk::u2_t>((uint8_t)reader[iToken]->Token->Value, 1); }
	{ int32_t iToken; if_fail_fe(iToken = jsonArrayValueGet(reader, 0, 2)); ipv4 |= gpk::byte_to<gpk::u2_t>((uint8_t)reader[iToken]->Token->Value, 2); }
	{ int32_t iToken; if_fail_fe(iToken = jsonArrayValueGet(reader, 0, 3)); ipv4 |= gpk::byte_to<gpk::u2_t>((uint8_t)reader[iToken]->Token->Value, 3); }
	return 0;
}
gpk::err_t	gpk::tcpipAddress		(vcst_t strIP, vcst_t strPort, SIPv4End & ipv4) {
	if(0 == strPort.size()) 
		return tcpipAddress(strIP, ipv4.IP, ipv4.Port);

	const gpk::u2_t				iOffset					= tcpipAddress(strIP, ipv4.IP);
	return iOffset + parseIntegerDecimal(strPort, ipv4.Port);
}

gpk::err_t			gpk::tcpipAddress		(vcst_t strIP, u2_t & ipv4)	{
	ipv4					= 0;
	ltrim(strIP);
	if(0 == strIP.size()) 
		return 0;

	u2_t					iOffset					= 0;
	u2_t					iEnd					= 0;
	for(uint8_t iVal = 0; iVal < 4 && iOffset < strIP.size(); ++iVal) {
		while(iEnd < strIP.size()) {
			sc_t curChar = strIP[iEnd];
			if(0 == iVal && curChar == '[') 
				return jsonTcpipAddress(strIP, ipv4);

			if( curChar == ','
			 || curChar == '.'
			 || curChar == ':'
			 || curChar == '\0'
			 || (iEnd - iOffset) > 3	// 3 digit max
			)
				break;
			++iEnd;
		}
		uint8_t			valueRead	= 0;
		if_fail_fe(gpk::parseIntegerDecimal({&strIP[iOffset], iEnd - iOffset}, valueRead));

		ipv4	|= byte_to<u2_t>(valueRead, iVal);

		iOffset		= iEnd + 1;
		iEnd		= iOffset;
	}
	return iOffset;
}

