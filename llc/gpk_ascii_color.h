#include "gpk_typeint.h"	// for uint16_t and uint32_t

#ifndef GPK_ASCII_COLOR_H_230487298374
#define GPK_ASCII_COLOR_H_230487298374

namespace gpk
{
	// This type is used to represent the palette array of the ASCII console. Having : uint16_t here makes ASCII_COLOR_INDEX to have the same memory footprint as an unsigned short.
	enum ASCII_COLOR_INDEX : uint32_t
		{	ASCII_COLOR_INDEX_0			= 0x000000		// BLACK
		,	ASCII_COLOR_INDEX_1 		= 0x800000	 	// DARKBLUE
		,	ASCII_COLOR_INDEX_2 		= 0x008000	 	// DARKGREEN
		,	ASCII_COLOR_INDEX_3 		= 0x808000	 	// DARKCYAN
		,	ASCII_COLOR_INDEX_4 		= 0x000080	 	// DARKRED
		,	ASCII_COLOR_INDEX_5 		= 0x800080	 	// DARKMAGENTA
		,	ASCII_COLOR_INDEX_6 		= 0x008080	 	// DARKYELLOW
		,	ASCII_COLOR_INDEX_7 		= 0xC0C0C0	 	// LIGHTGREY
		,	ASCII_COLOR_INDEX_8 		= 0x808080	 	// DARKGREY
		,	ASCII_COLOR_INDEX_9 		= 0xFF0000	 	// BLUE
		,	ASCII_COLOR_INDEX_10		= 0x00FF00		// GREEN
		,	ASCII_COLOR_INDEX_11		= 0xFFFF00		// CYAN
		,	ASCII_COLOR_INDEX_12		= 0x0000FF		// RED
		,	ASCII_COLOR_INDEX_13		= 0xFF00FF		// MAGENTA
		,	ASCII_COLOR_INDEX_14		= 0x00FFFF		// YELLOW
		,	ASCII_COLOR_INDEX_15		= 0xFFFFFF		// WHITE
		,	ASCII_COLOR_INDEX_COUNT		= 16			// COUNT
		};

	static constexpr uint32_t ASCII_PALETTE [] = 
		{	0xFF000000		// BLACK
		,	0xFF800000	 	// DARKBLUE
		,	0xFF008000	 	// DARKGREEN
		,	0xFF808000	 	// DARKCYAN
		,	0xFF000080	 	// DARKRED
		,	0xFF800080	 	// DARKMAGENTA
		,	0xFF008080	 	// DARKYELLOW
		,	0xFFC0C0C0	 	// LIGHTGREY
		,	0xFF808080	 	// DARKGREY
		,	0xFFFF0000	 	// BLUE
		,	0xFF00FF00		// GREEN
		,	0xFFFFFF00		// CYAN
		,	0xFF0000FF		// RED
		,	0xFFFF00FF		// MAGENTA
		,	0xFF00FFFF		// YELLOW
		,	0xFFFFFFFF		// WHITE
		};

	// 3-Channel color array with a max value of 0xFF each. This means we use 24 bit for each color but as there is no fundamental type of 24-bit in C++ we gpkose the next larger, in this case an unsigned int of 32-bit size.
	// This particular channel arrangement has the red channel stored in the first 8 bits (the first byte), the green channel in the next 8 bits (the second byte), and the blue channel in the last 8 bits (the third byte) of the channel array.
	// The remaining 8 bits must be set to 0 and I don't remember if they have an actual meaning for the console system. More complex graphics systems may use those remaining bits to represent transparency (the alpha channel) or other rendering-related information.
	enum ASCII_COLOR : uint16_t
		{	ASCII_COLOR_BLACK			=  0
		,	ASCII_COLOR_DARKBLUE
		,	ASCII_COLOR_DARKGREEN
		,	ASCII_COLOR_DARKCYAN
		,	ASCII_COLOR_DARKRED
		,	ASCII_COLOR_DARKMAGENTA
		,	ASCII_COLOR_DARKYELLOW
		,	ASCII_COLOR_LIGHTGREY
		,	ASCII_COLOR_DARKGREY
		,	ASCII_COLOR_BLUE
		,	ASCII_COLOR_GREEN
		,	ASCII_COLOR_CYAN
		,	ASCII_COLOR_RED
		,	ASCII_COLOR_MAGENTA
		,	ASCII_COLOR_YELLOW
		,	ASCII_COLOR_WHITE
		,	ASCII_COLOR_COUNT
		};

	// Swaps the background color and the font color.
	inline constexpr	ASCII_COLOR			ascii_color_invert		(const ASCII_COLOR color)				{ return (ASCII_COLOR)(((color & 0xF0) >> 4) | ((color & 0xF) << 4));	}
}

// BLINK	128
// #define invert_color_ascii(_col) (((_col & 0xF0)>>4) | ((_col & 0xF)<<4))

#endif // GPK_ASCII_COLOR_H_230487298374
