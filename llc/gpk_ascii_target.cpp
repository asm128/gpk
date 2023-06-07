#include "gpk_ascii_target.h"

::gpk::error_t			gpk::asciiTargetClear							(::gpk::SASCIITarget& console, uint8_t character, uint16_t color)		{
	uint8_t																* characters									= console.Characters	.Texels.begin();
	uint16_t															* colors										= console.Colors		.Texels.begin();
	for(uint32_t iCell = 0; iCell < console.Colors.Texels.size(); ++iCell) {
		characters	[iCell]												= character;
		colors		[iCell]												= color;
	}
	return 0;
}
