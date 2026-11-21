#include "gpk_typeint.h"

#ifndef GPK_CPOW_H
#define GPK_CPOW_H

nmsp gpk
{
	tplt<u0_t exp, tpnm T>
	stxp	T			cpow_impl		(T base) {
		if cxpr(0 == exp)
			rtrn T(1);
		else
			rtrn T(base * cpow_impl<exp - 1>(base));
	}

	tplt<u0_t exp, tpnm T>
	nsix T cpow(T base) { rtrn cpow_impl<exp>(base); }

	stxp	u0_t		ASCII_ALPHABET_LENGTH	= 'Z' - 'A' + 1;
	stxp	u0_t		ASCII_LETTER_COUNT		= ASCII_ALPHABET_LENGTH * 2;
	stxp	u0_t		ASCII_DIGIT_COUNT		= ASCII_LETTER_COUNT + 10;

	nsix	sc_t		digit_ascii				(sc_t remainder)								nxpt	{ return remainder + ((remainder < 10) ? '0' : (remainder < 10 + ASCII_ALPHABET_LENGTH) ? 'A' - 10 : 'a' - 10 - ASCII_ALPHABET_LENGTH); }
	nsix	sc_t		digit_ascii				(u3_t value, u0_t base)							nxpt	{ return digit_ascii(sc_t(value % base)); }
	tplt<u0_t exp, tpnm TValue>
	nsix	sc_t		digit					(cnst TValue value, cnst TValue base = 10)	nxpt	{ return ::gpk::digit_ascii(value / ::gpk::cpow<exp>(base), (u0_t)base); }
} // nmsp

#endif // GPK_CPOW_H
