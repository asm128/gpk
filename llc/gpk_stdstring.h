#include "gpk_view.h"
#include "gpk_safe.h"

#ifndef GPK_STDSTRING_H_23627
#define GPK_STDSTRING_H_23627
namespace gpk
{
	//sinx	::gpk::error_t	toupper		(char input)				{ return input (curByte >= 'A' && curByte <= 'Z') & ~0x20; }
	//sinx	::gpk::error_t	tolower		(char input)				{ return input (curByte >= 'A' && curByte <= 'Z') | 0x20; }
	sinx	::gpk::error_t	toupper		(char & input)				{ return (input >= 'a' && input <= 'z') ? input &= ~0x20U : 0; }
	sinx	::gpk::error_t	tolower		(char & input)				{ return (input >= 'A' && input <= 'Z') ? input |= 0x20U  : 0; }

	::gpk::error_t			tolower		(::gpk::view<char> input);
	::gpk::error_t			toupper		(::gpk::view<char> input);
	::gpk::error_t			stoull		(::gpk::vcsc_c & input, uint64_t & output);
}
#endif // GPK_STDSTRING_H_23627

