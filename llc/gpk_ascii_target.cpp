#include "gpk_ascii_target.h"

::gpk::err_t			gpk::asciiTargetClear							(::gpk::SASCIITarget& console, u0_t character, u1_t color)		{
	u0_t																* characters									= console.Characters	.Texels.begin();
	u1_t															* colors										= console.Colors		.Texels.begin();
	for(u2_t iCell = 0; iCell < console.Colors.Texels.size(); ++iCell) {
		characters	[iCell]												= character;
		colors		[iCell]												= color;
	}
	return 0;
}
