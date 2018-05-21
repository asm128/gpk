#include "gpk_ascii_target.h"

				::gpk::error_t									gpk::asciiTargetCreate							(::gpk::SASCIITarget& console, uint32_t width, uint32_t height)		{ 
	console.Characters												= {(uint8_t *)::malloc(sizeof(uint8_t ) * width * height), width, height}; 
	console.Colors													= {(uint16_t*)::malloc(sizeof(uint16_t) * width * height), width, height}; 
	return 0; 
}

				::gpk::error_t									gpk::asciiTargetDestroy						(::gpk::SASCIITarget& console)											{ 
	if(console.Characters	.begin()) ::free(console.Characters	.begin());
	if(console.Colors		.begin()) ::free(console.Colors		.begin());
	return 0; 
}

				::gpk::error_t									gpk::asciiTargetClear							(::gpk::SASCIITarget& console, uint8_t character, uint16_t color)		{ 
	uint8_t																* characters									= console.Characters	.begin();
	uint16_t															* colors										= console.Colors		.begin();
	for(uint32_t iCell = 0; iCell < console.Colors.size(); ++iCell) {
		characters	[iCell]												= character;
		colors		[iCell]												= color;
	}
	return 0; 
}
