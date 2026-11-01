#include "gpk_typeint.h"

#ifndef GPK_CPOW_H
#define GPK_CPOW_H

namespace gpk
{
	tplt<u0_t exp, tpnm T>
	stxp	T			cpow_impl		(T base) {
		if constexpr(0 == exp)
			rtrn T(1);
		else
			rtrn T(base * cpow_impl<exp - 1>(base));
	}

	tplt<u0_t exp>	nsix	u0_t	cpow	(u0_t base)	{ rtrn cpow_impl<exp>(base); }
	tplt<u0_t exp>	nsix	u1_t	cpow	(u1_t base)	{ rtrn cpow_impl<exp>(base); }
	tplt<u0_t exp>	nsix	u2_t	cpow	(u2_t base)	{ rtrn cpow_impl<exp>(base); }
	tplt<u0_t exp>	nsix	u3_t	cpow	(u3_t base)	{ rtrn cpow_impl<exp>(base); }
	tplt<u0_t exp>	nsix	s0_t	cpow	(s0_t base)	{ rtrn cpow_impl<exp>(base); }
	tplt<u0_t exp>	nsix	s1_t	cpow	(s1_t base)	{ rtrn cpow_impl<exp>(base); }
	tplt<u0_t exp>	nsix	s2_t	cpow	(s2_t base)	{ rtrn cpow_impl<exp>(base); }
	tplt<u0_t exp>	nsix	s3_t	cpow	(s3_t base)	{ rtrn cpow_impl<exp>(base); }
	tplt<u0_t exp>	nsix	f2_t	cpow	(f2_t base)	{ rtrn cpow_impl<exp>(base); }
	tplt<u0_t exp>	nsix	f3_t	cpow	(f3_t base)	{ rtrn cpow_impl<exp>(base); }

	stxp	u0_t		ASCII_ALPHABET_LENGTH	= 'Z' - 'A' + 1;
	stxp	u0_t		ASCII_LETTER_COUNT		= ASCII_ALPHABET_LENGTH * 2;
	stxp	u0_t		ASCII_DIGIT_COUNT		= ASCII_LETTER_COUNT + 10;

	nsix	char		digit_ascii				(char remainder)								nxpt	{ return remainder + ((remainder < 10) ? '0' : (remainder < 10 + ASCII_ALPHABET_LENGTH) ? 'A' - 10 : 'a' - 10 - ASCII_ALPHABET_LENGTH); }
	nsix	char		digit_ascii				(u3_t value, u0_t base)							nxpt	{ return digit_ascii(char(value % base)); }
	tplt<u0_t exp, tpnm TValue>
	nsix	char		digit					(const TValue value, const TValue base = 10)	nxpt	{ return ::gpk::digit_ascii(value / ::gpk::cpow<exp>(base), (u0_t)base); }
} // namespace

#endif // GPK_CPOW_H
