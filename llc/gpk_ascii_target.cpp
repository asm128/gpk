#include "gpk_ascii_target.h"

gpk::err_t 		gpk::asciiTargetClear							(::gpk::SASCIITarget& console, u0_t character, u1_t color)		{
	::gpk::vu0_t{console.Characters.Texels.begin(), console.Colors.Texels.size()}.fill(character);
	console.Colors.Texels.fill(color);
	return 0;
}
