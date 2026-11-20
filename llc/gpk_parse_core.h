#include "gpk_view.h"

#ifndef GPK_PARSE_CORE_H
#define GPK_PARSE_CORE_H

nmsp gpk
{
	stxp	vcst_t	DIGITS_HEX						= "0123456789abcdef";
	stxp	vcst_t	DIGITS_DECIMAL					= "0123456789";
	sinx	err_t	parseArbitraryBaseInteger		(u2_t base, vcst_t symbolList, vcst_t sourceChars, u3_t * number_) {
		cnst u2_t symbolCount = ::gpk::min(base, symbolList.size());
		cnst vcst_t symbols = {symbolList.begin(), symbolCount};
		u3_t number = 0;
		u2_t iChar = 0;
		for(; iChar < sourceChars.size() && sourceChars[iChar]; ++iChar) {
			sc_t sourceChar = sourceChars[iChar];
			if(sourceChar >= 'A' && sourceChar <= 'Z')
				sourceChar = sc_t(sourceChar + ('a' - 'A'));
			cnst err_t iSymbol = symbols.find(sourceChar);
			if(0 > iSymbol)
				break;
			number = number * base + iSymbol;
		}
		if(number_)
			*number_ = number;
		rtrn iChar;
	}

	tplt<tpnm T>
	inxp err_t parseIntegerDecimal(vcst_t sourceChars, T & number_) {
		u3_t number = 0;
		cnst err_t countDigits = parseArbitraryBaseInteger(10, DIGITS_DECIMAL, sourceChars, &number);
		number_ = (T)number;
		rtrn countDigits;
	}

	tplt<tpnm T>
	inxp err_t parseIntegerHexadecimal(vcst_t sourceChars, T & number_) {
		u3_t number = 0;
		cnst err_t countDigits = parseArbitraryBaseInteger(16, DIGITS_HEX, sourceChars, &number);
		number_ = (T)number;
		rtrn countDigits;
	}
}

#endif // GPK_PARSE_CORE_H
