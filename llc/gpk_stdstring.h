#include "gpk_view.h"

#ifndef GPK_STDSTRING_H_23627
#define GPK_STDSTRING_H_23627

namespace gpk
{
	//sinx err_t		toupper		(char input)		{ return input (curByte >= 'A' && curByte <= 'Z') & ~0x20; }
	//sinx err_t		tolower		(char input)		{ return input (curByte >= 'A' && curByte <= 'Z') | 0x20; }
	sinx err_t		toupper		(sc_t & input)		{ return (input >= 'a' && input <= 'z') ? input &= ~0x20U : 0; }
	sinx err_t		tolower		(sc_t & input)		{ return (input >= 'A' && input <= 'Z') ? input |= 0x20U  : 0; }
	stin err_t		toupper		(vs0_t input)	{
		for(::gpk::u2_t iByte = 0, sizeHeader = input.size(); iByte < sizeHeader; ++iByte)
			toupper(input[iByte]);
		return 0;
	}
	stin err_t		tolower		(vs0_t input)	{
		for(::gpk::u2_t iByte = 0, sizeHeader = input.size(); iByte < sizeHeader; ++iByte)
			tolower(input[iByte]);
		return 0;
	}
	err_t			stoull		(vcst_t input, uint64_t & output);
}
#endif // GPK_STDSTRING_H_23627


