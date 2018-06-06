#include "gpk_gui.h"
#include "gpk_bitmap_target.h"
#include "gpk_grid_copy.h"
#include "gpk_encoding.h"

static		::gpk::error_t										controlInvalid											(const ::gpk::SGUI& gui, int32_t iControl)				{
	if(gui.Controls.Controls.size() <= uint32_t(iControl)	) return -1;
	if(gui.Controls.States[iControl].Unused					) return -1;
	return 0;
}

static		::gpk::error_t										themeSetupLite											(::gpk::SControlTheme& theme)															{	
	::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboDisabled									= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_DISABLED	]	= {};
	::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboPressed 									= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_PRESSED		]	= {};
	::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboSelected									= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_SELECTED	]	= {};
	::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboHover 										= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_HOVER		]	= {};
	::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboNormal										= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_NORMAL		]	= {};

	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BACKGROUND]		= ::gpk::ASCII_COLOR_LIGHTGREY		* 16;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_CLIENT	]		= colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT]		= ::gpk::ASCII_COLOR_LIGHTGREY		* 16;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP]		= ::gpk::ASCII_COLOR_LIGHTGREY		* 16;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT]		= ::gpk::ASCII_COLOR_LIGHTGREY		* 16;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM]	= ::gpk::ASCII_COLOR_LIGHTGREY		* 16;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND]	= ::gpk::ASCII_COLOR_LIGHTGREY		* 16;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE]		= ::gpk::ASCII_COLOR_DARKGREY		* 16;
																	    
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BACKGROUND]		= ::gpk::ASCII_COLOR_DARKGREY		* 16;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_CLIENT	]		= colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT]		= ::gpk::ASCII_COLOR_BLACK			* 16;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP]		= ::gpk::ASCII_COLOR_BLACK			* 16;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT]		= ::gpk::ASCII_COLOR_BLACK			* 16;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM]	= ::gpk::ASCII_COLOR_BLACK			* 16;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND]	= ::gpk::ASCII_COLOR_BLACK			* 16;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE]		= ::gpk::ASCII_COLOR_BLUE			* 16;

	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BACKGROUND]		= ::gpk::ASCII_COLOR_DARKGREY		* 16;
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_CLIENT	]		= colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT]		= ::gpk::ASCII_COLOR_LIGHTGREY		* 16;
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP]		= ::gpk::ASCII_COLOR_LIGHTGREY		* 16;
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT]		= ::gpk::ASCII_COLOR_LIGHTGREY		* 16;
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM]	= ::gpk::ASCII_COLOR_LIGHTGREY		* 16;
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND]	= ::gpk::ASCII_COLOR_DARKGREY		* 16;
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE]		= ::gpk::ASCII_COLOR_BLACK			* 16;

	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BACKGROUND]		= ::gpk::ASCII_COLOR_DARKGREY		* 16;
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_CLIENT	]		= colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT]		= ::gpk::ASCII_COLOR_DARKGREY		* 16;
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP]		= ::gpk::ASCII_COLOR_DARKGREY		* 16;
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT]		= ::gpk::ASCII_COLOR_DARKGREY		* 16;
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM]	= ::gpk::ASCII_COLOR_DARKGREY		* 16;
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND]	= ::gpk::ASCII_COLOR_DARKGREY		* 16;
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE]		= ::gpk::ASCII_COLOR_BLACK			* 16;

	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BACKGROUND]		= ::gpk::ASCII_COLOR_LIGHTGREY		* 16;
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_CLIENT	]		= colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT]		= ::gpk::ASCII_COLOR_DARKGREY		* 16;
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP]		= ::gpk::ASCII_COLOR_DARKGREY		* 16;
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT]		= ::gpk::ASCII_COLOR_DARKGREY		* 16;
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM]	= ::gpk::ASCII_COLOR_DARKGREY		* 16;
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND]	= ::gpk::ASCII_COLOR_LIGHTGREY		* 16;
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE]		= ::gpk::ASCII_COLOR_BLACK			* 16;
	return 0; 
}
static		::gpk::error_t										themeSetupDark											(::gpk::SControlTheme& theme)															{	
	::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboDisabled									= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_DISABLED	]	= {};
	::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboPressed 									= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_PRESSED		]	= {};
	::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboSelected									= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_SELECTED	]	= {};
	::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboHover 										= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_HOVER		]	= {};
	::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboNormal										= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_NORMAL		]	= {};

	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BACKGROUND]		= ::gpk::ASCII_COLOR_WHITE * 16 + 15;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_CLIENT	]		= colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT]		= ::gpk::ASCII_COLOR_DARKGREY * 16;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP]		= ::gpk::ASCII_COLOR_DARKGREY * 16;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT]		= ::gpk::ASCII_COLOR_DARKGREY * 16;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM]	= ::gpk::ASCII_COLOR_DARKGREY * 16;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE]		= ::gpk::ASCII_COLOR_WHITE * 16 + 11;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND]	= colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
																	   
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BACKGROUND]		= ::gpk::ASCII_COLOR_WHITE * 16 + 9;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_CLIENT	]		= colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT]		= ::gpk::ASCII_COLOR_DARKGREY * 16;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP]		= ::gpk::ASCII_COLOR_DARKGREY * 16;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT]		= ::gpk::ASCII_COLOR_DARKGREY * 16;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM]	= ::gpk::ASCII_COLOR_DARKGREY * 16;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE]		= ::gpk::ASCII_COLOR_WHITE * 16 + 4;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND]	= colorComboPressed		[::gpk::GUI_CONTROL_COLOR_BACKGROUND];

	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BACKGROUND]		= ::gpk::ASCII_COLOR_WHITE * 16 + 11;
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_CLIENT	]		= colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT]		= ::gpk::ASCII_COLOR_DARKGREY * 16;
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP]		= ::gpk::ASCII_COLOR_DARKGREY * 16;
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT]		= ::gpk::ASCII_COLOR_DARKGREY * 16;
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM]	= ::gpk::ASCII_COLOR_DARKGREY * 16;
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE]		= ::gpk::ASCII_COLOR_WHITE * 16;
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND]	= colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];

	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BACKGROUND]		= ::gpk::ASCII_COLOR_WHITE * 16 + 11;
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_CLIENT	]		= colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT]		= ::gpk::ASCII_COLOR_DARKGREY * 16;
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP]		= ::gpk::ASCII_COLOR_DARKGREY * 16;
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT]		= ::gpk::ASCII_COLOR_DARKGREY * 16;
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM]	= ::gpk::ASCII_COLOR_DARKGREY * 16;
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE]		= ::gpk::ASCII_COLOR_WHITE * 16;
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND]	= colorComboHover		[::gpk::GUI_CONTROL_COLOR_BACKGROUND];

	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BACKGROUND]		= ::gpk::ASCII_COLOR_WHITE * 16 + 14;
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_CLIENT	]		= colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT]		= ::gpk::ASCII_COLOR_DARKGREY	* 16;
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP]		= ::gpk::ASCII_COLOR_DARKGREY	* 16;
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT]		= ::gpk::ASCII_COLOR_DARKGREY	* 16;
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM]	= ::gpk::ASCII_COLOR_DARKGREY	* 16;
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE]		= ::gpk::ASCII_COLOR_WHITE		* 16;
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND]	= colorComboNormal		[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
	return 0; 
}

static		::gpk::error_t										themeSetupDefault										(::gpk::array_pod<::gpk::SControlTheme>& themes, ::gpk::array_pod<::gpk::SColorBGRA>& palette)										{
	if(themes.size() < 2)
		gpk_necall(themes.resize(2), "Out of memory?");
	themeSetupLite(themes[0]);
	themeSetupDark(themes[1]);
	if(palette.size() < 256)
		gpk_necall(palette.resize(256), "Out of memory?");
	palette[ 0 * 16]												= ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_0	};
	palette[ 1 * 16]												= ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_1	};
	palette[ 2 * 16]												= ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_2	};
	palette[ 3 * 16]												= ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_3	};
	palette[ 4 * 16]												= ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_4	};
	palette[ 5 * 16]												= ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_5	};
	palette[ 6 * 16]												= ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_6	};
	palette[ 7 * 16]												= ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_7	};
	palette[ 8 * 16]												= ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_8	};
	palette[ 9 * 16]												= ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_9	};
	palette[10 * 16]												= ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_10	};
	palette[11 * 16]												= ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_11	};
	palette[12 * 16]												= ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_12	};
	palette[13 * 16]												= ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_13	};
	palette[14 * 16]												= ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_14	};
	palette[15 * 16]												= ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_15	};
	for(uint32_t iTone = 0; iTone < 16; ++iTone)
		for(uint32_t iShade = 0; iShade < 16; ++iShade) {
			if(0 == (iShade % 16))
				continue;
			const int32_t														toneIndex												= iTone * 16;
			::gpk::SColorBGRA													& baseColor												= palette[toneIndex];
			::gpk::SColorBGRA													& paletteItem											= palette[toneIndex + iShade];
			paletteItem														= ::gpk::SColorFloat(baseColor) / 16.0 * (16.0 - iShade);
			info_printf("Original color: {r: 0x%X, g: 0x%X, b: 0x%X}.", baseColor	.r, baseColor	.g, baseColor	.b);
			info_printf("Shaded color  : {r: 0x%X, g: 0x%X, b: 0x%X}.", paletteItem	.r, paletteItem	.g, paletteItem	.b);
		}

	for(uint32_t iColor = 0; iColor < palette.size(); ++iColor) {
		const int32_t														indexTheme												= themes.push_back({});
		::gpk::SControlTheme												& theme													= themes[indexTheme];
		::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboDisabled									= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_DISABLED	]	= {};
		::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboPressed 									= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_PRESSED		]	= {};
		::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboSelected									= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_SELECTED	]	= {};
		::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboHover 										= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_HOVER		]	= {};
		::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboNormal										= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_NORMAL		]	= {};

		const int32_t														colorBase												= iColor / 16;
		const int32_t														colorShade												= iColor % 16;
		const int32_t														colorText												= (::gpk::ASCII_COLOR_WHITE * 16);

		colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]	= (colorShade > 7) ? iColor : iColor;
		colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT	]	= (colorShade > 7) ? iColor - 1 : iColor + 1;
		colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP	]	= (colorShade > 7) ? iColor - 1 : iColor + 1;
		colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT	]	= (colorShade > 7) ? iColor - 1 : iColor + 1;
		colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM	]	= (colorShade > 7) ? iColor - 1 : iColor + 1;
		colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE		]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
//
		colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]	= (colorShade > 7) ? iColor : iColor;
		colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT	]	= (colorShade > 7) ? iColor - 3: iColor + 3;
		colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP	]	= (colorShade > 7) ? iColor - 3: iColor + 3;
		colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT	]	= (colorShade > 7) ? iColor - 3: iColor + 3;
		colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM	]	= (colorShade > 7) ? iColor - 3: iColor + 3;
		colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE		]	= (colorShade > 7) ? colorText + 7 : colorText + 9;
//
		colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]	= (colorShade > 7) ? iColor : iColor;
		colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT	]	= (colorShade > 7) ? iColor : iColor;
		colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP	]	= (colorShade > 7) ? iColor : iColor;
		colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT	]	= (colorShade > 7) ? iColor : iColor;
		colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM	]	= (colorShade > 7) ? iColor : iColor;
		colorComboSelected	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE		]	= (colorShade > 7) ? colorText : colorText + 8;
//
		colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]	= (colorShade > 7) ? iColor : iColor;
		colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT	]	= (colorShade > 7) ? iColor - 7: iColor + 7;
		colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP	]	= (colorShade > 7) ? iColor - 7: iColor + 7;
		colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT	]	= (colorShade > 7) ? iColor - 7: iColor + 7;
		colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM	]	= (colorShade > 7) ? iColor - 7: iColor + 7;
		colorComboHover 	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE		]	= (colorShade > 7) ? colorText + 4: colorText + 12;
//
		colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]	= (colorShade > 7) ? iColor : iColor;
		colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT	]	= (colorShade > 7) ? iColor - 5: iColor + 5;
		colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP	]	= (colorShade > 7) ? iColor - 5: iColor + 5;
		colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT	]	= (colorShade > 7) ? iColor - 5: iColor + 5;
		colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM	]	= (colorShade > 7) ? iColor - 5: iColor + 5;
		colorComboNormal	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE		]	= (colorShade > 7) ? colorText : colorText + 15;

		colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND]	= colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
		colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_CLIENT	]		= colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
		colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND]	= colorComboPressed		[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
		colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_CLIENT	]		= colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
		colorComboSelected	[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND]	= colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
		colorComboSelected	[::gpk::GUI_CONTROL_COLOR_CLIENT	]		= colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
		colorComboHover 	[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND]	= colorComboHover		[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
		colorComboHover 	[::gpk::GUI_CONTROL_COLOR_CLIENT	]		= colorComboHover 		[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
		colorComboNormal	[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND]	= colorComboNormal		[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
		colorComboNormal	[::gpk::GUI_CONTROL_COLOR_CLIENT	]		= colorComboNormal		[::gpk::GUI_CONTROL_COLOR_BACKGROUND];

		for(uint32_t iState = 0; iState < theme.ColorCombos.size(); ++iState) {
			for(uint32_t iArea = 0; iArea < ::gpk::GUI_CONTROL_COLOR_COUNT; ++iArea) 
				theme.ColorCombos[iState][iArea]							= ::gpk::min((uint32_t)theme.ColorCombos[iState][iArea], palette.size() - 1);
		}
	}

	return 0;
}

static		::gpk::error_t										controlInstanceReset									(::gpk::SGUIControlTable& gui, int32_t iControl)										{
	::gpk::SGUIControlTable												& controlTable											= gui;
	controlTable.Metrics	[iControl]								= {};
	controlTable.Children	[iControl]								= ::gpk::array_view<int32_t>{};
	controlTable.Modes		[iControl]								= {};	

	::gpk::SControl														& control				= controlTable.Controls		[iControl]	= {};
	::gpk::SControlState												& controlStates			= controlTable.States		[iControl]	= {};
	::gpk::SControlConstraints											& controlConstraints	= controlTable.Constraints	[iControl]	= {};	
	::gpk::SControlText													& controlText			= controlTable.Text			[iControl]	= {};
	controlStates.Outdated											= true;
	control.IndexParent												= -1;
	control.Align													= ::gpk::ALIGN_TOP_LEFT;
	control.Area													= {{0, 0}, {16, 16}};
	control.Border													= {1, 1, 1, 1};
	control.Margin													= {1, 1, 1, 1};
	controlConstraints												= {-1, -1};
	controlConstraints.DockToControl								= {-1, -1};
	controlText.Align												= ::gpk::ALIGN_CENTER;
	return 0;
}

			::gpk::error_t										gpk::controlCreate										(::gpk::SGUI& gui)										{
	if(0 == gui.ControlThemes.size())
		::themeSetupDefault(gui.ControlThemes, gui.Palette);

	for(uint32_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl) {
		if(gui.Controls.States[iControl].Unused) {
			::controlInstanceReset(gui.Controls, iControl);
			return iControl;
		}
	}
	::gpk::error_t														iControl												= -1;
	gpk_necall(iControl = ::gpk::resize( gui.Controls.Controls.size() + 1
		, gui.Controls.Controls
		, gui.Controls.States
		, gui.Controls.Metrics
		, gui.Controls.Text
		, gui.Controls.Children
		, gui.Controls.Constraints
		, gui.Controls.Modes
		) - 1, "Failed to resize! Out of memory?");
	::controlInstanceReset(gui.Controls, iControl);
	return iControl; 
}

			::gpk::error_t										gpk::controlDelete										(::gpk::SGUI& gui, int32_t iControl)					{ 
	gpk_necall(::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	::gpk::SControlState												& controlState											= gui.Controls.States[iControl];
	controlState.Unused												= true;
	const uint32_t														indexParent												= (uint32_t)gui.Controls.Controls[iControl].IndexParent;
	if(indexParent < gui.Controls.Controls.size() && false == gui.Controls.States[indexParent].Unused) {			
		::gpk::array_pod<int32_t>											& children												= gui.Controls.Children[indexParent];
		for(uint32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
			if(children[iChild] == iControl) {
				gpk_necall(children.remove(iChild), "Failed to remove child at index: %u.", iChild);
				break;
			}
	}
	::gpk::array_view<int32_t>											& children												= gui.Controls.Children[iControl];
	for(uint32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
		gui.Controls.States[children[iChild]].Unused					= true;

	return 0; 
}

	::gpk::error_t												gpk::controlSetParent									(::gpk::SGUI& gui, int32_t iControl, int32_t iParent)	{
	gpk_necall(::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	::gpk::SControl														& control												= gui.Controls.Controls[iControl];
	if(control.IndexParent == iParent)	// Exit early if there is nothing to do here.
		return 0;

	control.IndexParent												= iParent;
	if(::controlInvalid(gui, iParent)) {
		ree_if(-1 != iParent, "Invalid parent control id: %u.", iParent);
		return 0;
	}

	// Set iControl to parent's children array.
	::gpk::array_pod<int32_t>											& children												= gui.Controls.Children[iParent];
	for(uint32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
		if(children[iChild] == iControl)
			return 0;

	gpk_necall(children.push_back(iControl), "Out of memory?");
	return 0;
}

	static		::gpk::error_t									controlUpdateMetrics									(::gpk::SGUI& gui, int32_t iControl, const ::gpk::SCoord2<uint32_t> & _targetSize, bool forceUpdate)					{
	gpk_necall(::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	::gpk::SControlState												& controlState											= gui.Controls.States[iControl];
	const ::gpk::SControl												& control												= gui.Controls.Controls[iControl];
	::gpk::SCoord2<int32_t>												targetSize												= _targetSize.Cast<int32_t>();
	const bool															isValidParent											= 0 == ::controlInvalid(gui, control.IndexParent);
	if(isValidParent) {
		::controlUpdateMetrics(gui, control.IndexParent, _targetSize, forceUpdate);
		targetSize														= gui.Controls.Metrics[control.IndexParent].Client.Global.Size;
	}
	if(false == controlState.Outdated && false == forceUpdate)
		return 0;

	::gpk::SCoord2<double>												scale													= gui.Zoom.DPI * gui.Zoom.ZoomLevel;
	::gpk::SCoord2<int32_t>												scaledPosition											= ::gpk::SCoord2<double>{control.Area.Offset	.x * scale.x, control.Area.Offset	.y * scale.y}.Cast<int32_t>();
	::gpk::SCoord2<int32_t>												scaledSize												= ::gpk::SCoord2<double>{control.Area.Size		.x * scale.x, control.Area.Size		.y * scale.y}.Cast<int32_t>();
	::gpk::SRectLimits<int32_t>											scaledBorders											= {(int32_t)(control.Border.Left * scale.x), (int32_t)(control.Border.Top * scale.y), (int32_t)(control.Border.Right * scale.x), (int32_t)(control.Border.Bottom * scale.y)};
	::gpk::SRectLimits<int32_t>											scaledMargins											= {(int32_t)(control.Margin.Left * scale.x), (int32_t)(control.Margin.Top * scale.y), (int32_t)(control.Margin.Right * scale.x), (int32_t)(control.Margin.Bottom * scale.y)};
	::gpk::SControlMetrics												& controlMetrics										= gui.Controls.Metrics[iControl];
	::gpk::SRectangle2D<int32_t>										& rectText												= controlMetrics.Text;
	::gpk::SControlText													& controlText											= gui.Controls.Text[iControl];
	rectText.Size													= {(int32_t)gui.FontCharSize.x * (int32_t)controlText.Text.size(), (int32_t)gui.FontCharSize.y};
	const ::gpk::SControlConstraints									& controlConstraints									= gui.Controls.Constraints[iControl];
	const ::gpk::SRectLimits<int32_t>									ncSizes													= 
		{ scaledMargins.Left	+ scaledBorders.Left
		, scaledMargins.Top		+ scaledBorders.Top		
		, scaledMargins.Right	+ scaledBorders.Right	
		, scaledMargins.Bottom	+ scaledBorders.Bottom	
		};
	const ::gpk::SCoord2<int32_t>										ncTotalSize												= 
		{ ncSizes.Left	+ ncSizes.Right		
		, ncSizes.Top	+ ncSizes.Bottom	
		};

	if(controlConstraints.AttachSizeToText.x) { 
		scaledSize.x = int32_t(rectText.Size.x * scale.x + ncTotalSize.x); } 
	if(controlConstraints.AttachSizeToText.y) { 
		scaledSize.y = int32_t(rectText.Size.y * scale.y + ncTotalSize.y); } 

	if(controlConstraints.AttachSizeToControl.x == iControl) { scaledPosition.x = 0; scaledSize.x = targetSize.x; } else if(gui.Controls.Controls.size() > (uint32_t)controlConstraints.AttachSizeToControl.x && false == gui.Controls.States[controlConstraints.AttachSizeToControl.x].Unused) { scaledPosition.x = 0; scaledSize.x = gui.Controls.Metrics[controlConstraints.AttachSizeToControl.x].Total.Global.Size.x; }
	if(controlConstraints.AttachSizeToControl.y == iControl) { scaledPosition.y = 0; scaledSize.y = targetSize.y; } else if(gui.Controls.Controls.size() > (uint32_t)controlConstraints.AttachSizeToControl.y && false == gui.Controls.States[controlConstraints.AttachSizeToControl.y].Unused) { scaledPosition.y = 0; scaledSize.y = gui.Controls.Metrics[controlConstraints.AttachSizeToControl.y].Total.Global.Size.y; }
	
	controlMetrics.Client.Local								=  {{ncSizes.Left, ncSizes.Top}, scaledSize - ncTotalSize};

	controlMetrics.Total	.Local									= {scaledPosition, scaledSize};
		 if(::gpk::bit_true(control.Align, ::gpk::ALIGN_HCENTER	))	{ controlMetrics.Total.Local.Offset.x = targetSize.x / 2 - scaledSize.x / 2 + scaledPosition.x; }
	else if(::gpk::bit_true(control.Align, ::gpk::ALIGN_RIGHT	))	{ controlMetrics.Total.Local.Offset.x = targetSize.x - (scaledSize.x + scaledPosition.x); }
	else															{}

		 if(::gpk::bit_true(control.Align, ::gpk::ALIGN_VCENTER	))	{ controlMetrics.Total.Local.Offset.y = targetSize.y / 2 - scaledSize.y / 2 + scaledPosition.y; }
	else if(::gpk::bit_true(control.Align, ::gpk::ALIGN_BOTTOM	))	{ controlMetrics.Total.Local.Offset.y = targetSize.y - (scaledSize.y + scaledPosition.y); }
	else															{}

	controlMetrics.Total	.Global									= controlMetrics.Total	.Local;
	controlMetrics.Client	.Global									= controlMetrics.Client	.Local;
	controlMetrics.Client	.Global.Offset.x						+= controlMetrics.Total.Local.Offset.x;
	controlMetrics.Client	.Global.Offset.y						+= controlMetrics.Total.Local.Offset.y;

	if(isValidParent) {
		::gpk::SControlMetrics												& parentMetrics											= gui.Controls.Metrics[control.IndexParent];
		controlMetrics.Client	.Global.Offset							+= parentMetrics.Client.Global.Offset;
		controlMetrics.Total	.Global.Offset							+= parentMetrics.Client.Global.Offset;
	}

	const ::gpk::SCoord2<int32_t>										& dockToControl											= controlConstraints.DockToControl;
	if(dockToControl.x != -1) { gpk_necall(::controlInvalid(gui, dockToControl.x), "Invalid control id: %i.", dockToControl.x); const ::gpk::SControl & other = gui.Controls.Controls[dockToControl.x]; const ::gpk::SControlMetrics & otherMetrics = gui.Controls.Metrics[dockToControl.x]; if(gbit_true(other.Align, ::gpk::ALIGN_RIGHT	) && gbit_false(other.Align, ::gpk::ALIGN_HCENTER)) {} else { controlMetrics.Total.Global.Offset.x = otherMetrics.Total.Global.Offset.x + otherMetrics.Total.Global.Size.x; controlMetrics.Client.Global.Offset.x = controlMetrics.Total.Global.Offset.x + ncSizes.Left; } }
	if(dockToControl.y != -1) { gpk_necall(::controlInvalid(gui, dockToControl.y), "Invalid control id: %i.", dockToControl.y); const ::gpk::SControl & other = gui.Controls.Controls[dockToControl.y]; const ::gpk::SControlMetrics & otherMetrics = gui.Controls.Metrics[dockToControl.y]; if(gbit_true(other.Align, ::gpk::ALIGN_BOTTOM	) && gbit_false(other.Align, ::gpk::ALIGN_VCENTER)) {} else { controlMetrics.Total.Global.Offset.y = otherMetrics.Total.Global.Offset.y + otherMetrics.Total.Global.Size.y; controlMetrics.Client.Global.Offset.y = controlMetrics.Total.Global.Offset.y + ncSizes.Top ; } }

	{ // calculate text rectangle
		const ::gpk::SRectangle2D<int32_t>									& targetRect											= controlMetrics.Client.Global;
		rectText.Offset													= targetRect.Offset;
			 if (::gpk::bit_true(controlText.Align, ::gpk::ALIGN_HCENTER)) { rectText.Offset.x = (targetRect.Offset.x + targetRect.Size.x / 2) - rectText.Size.x / 2;	}
		else if (::gpk::bit_true(controlText.Align, ::gpk::ALIGN_RIGHT	)) { rectText.Offset.x = (targetRect.Offset.x + targetRect.Size.x) - rectText.Size.x;			}
		
			 if (::gpk::bit_true(controlText.Align, ::gpk::ALIGN_VCENTER)) { rectText.Offset.y = (targetRect.Offset.y + targetRect.Size.y / 2) - rectText.Size.y / 2;	}
		else if (::gpk::bit_true(controlText.Align, ::gpk::ALIGN_BOTTOM	)) { rectText.Offset.y = (targetRect.Offset.y + targetRect.Size.y) - rectText.Size.y;			}
		controlMetrics.Text												= controlMetrics.Text;
	}
	controlState.Outdated											= false;
	return 0;
}

static constexpr	const char									gpk_codepage_437_b64	[]								= "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";

			::gpk::error_t										setupDefaultFontTexture									(::gpk::SGUI& gui)																						{
	gui.FontTexture.resize(288, 128);
	::gpk::array_pod<ubyte_t>											decoded;
	::gpk::base64Decode(gpk_codepage_437_b64, decoded);
	memcpy(gui.FontTexture.Texels.begin(), decoded.begin(), decoded.size());
	return 0;
}

static		::gpk::error_t										controlTextDraw											(::gpk::SGUI& gui, int32_t iControl, ::gpk::grid_view<::gpk::SColorBGRA>& target)						{
	::gpk::SControlText													& controlText											= gui.Controls.Text		[iControl];
	::gpk::SControlState												& controlState											= gui.Controls.States	[iControl];
	::gpk::SControlMetrics												& controlMetrics										= gui.Controls.Metrics	[iControl];
	if(0 == gui.FontTexture.Texels.size())
		::setupDefaultFontTexture(gui);
	if(0 == gui.FontTexture.Texels.size() || 0 == gui.FontTexture.Pitch)
		return -1;
	const ::gpk::SControl												& control												= gui.Controls.Controls	[iControl];
	const ::gpk::SControlTheme											& theme													= gui.ControlThemes[(0 == control.ColorTheme) ? gui.ThemeDefault : control.ColorTheme - 1];
	const ::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>	& colorCombo											= theme.ColorCombos
		[ controlState.Disabled	? ::gpk::GUI_CONTROL_STATE_COLORS_DISABLED
		: controlState.Pressed	? ::gpk::GUI_CONTROL_STATE_COLORS_PRESSED 
		: controlState.Selected	? ::gpk::GUI_CONTROL_STATE_COLORS_SELECTED
		: controlState.Hover	? ::gpk::GUI_CONTROL_STATE_COLORS_HOVER 
		: ::gpk::GUI_CONTROL_STATE_COLORS_NORMAL
		];
	const ::gpk::SColorBGRA												colorBack												= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND]];
	const ::gpk::SColorBGRA												colorFace												= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_TEXT_FACE]];
	const uint32_t														charsPerRow												= gui.FontTexture.Pitch / gui.FontCharSize.x;
	const ::gpk::SRectangle2D<int32_t>									& rectText												= controlMetrics.Text;
	::gpk::array_pod<::gpk::SCoord2<uint32_t>>							dstCoords;
	for(uint32_t iChar = 0, countChars = (uint32_t)controlText.Text.size(); iChar < countChars; ++iChar) {
		char																charToDraw												= controlText.Text[iChar];
		const int32_t														coordTableX												= charToDraw % charsPerRow;
		const int32_t														coordTableY												= charToDraw / charsPerRow;
		const ::gpk::SCoord2<int32_t>										coordCharTable											= ::gpk::SCoord2<uint32_t>{coordTableX * gui.FontCharSize.x, coordTableY * gui.FontCharSize.y}.Cast<int32_t>();
		const ::gpk::SCoord2<int32_t>										dstOffset1												= {(int32_t)(rectText.Offset.x + gui.FontCharSize.x * iChar), rectText.Offset.y};
		const ::gpk::SRectangle2D<int32_t>									srcRect0												= {coordCharTable, gui.FontCharSize.Cast<int32_t>()};
		//error_if(errored(::gpk::grid_copy_alpha_bit(target, gui.FontTexture.View, dstOffset1, {charsPerRow * gui.FontCharSize.x, 8 * gui.FontCharSize.y}, fontColor, srcRect0)), "I believe this never fails.");
		dstCoords.clear();
		error_if(errored(::gpk::grid_raster_alpha_bit(target, gui.FontTexture.View, dstOffset1, {charsPerRow * gui.FontCharSize.x, 8 * gui.FontCharSize.y}, srcRect0, dstCoords)), "I believe this never fails.");
		for(uint32_t iCoord = 0; iCoord < dstCoords.size(); ++iCoord)
			::gpk::drawPixelLight(target, dstCoords[iCoord], colorFace, controlState.Pressed ? 0.075f : 0.05f, controlState.Pressed ? 1.0f : 0.75);
	}
	gui, iControl, target;
	return 0;
}

static		::gpk::error_t										actualControlDraw										(::gpk::SGUI& gui, int32_t iControl, ::gpk::grid_view<::gpk::SColorBGRA>& target)					{
	::controlUpdateMetrics(gui, iControl, target.metrics(), false);
	const ::gpk::SControlMode											& mode													= gui.Controls.Modes	[iControl];
	if(mode.Design || gui.Controls.Constraints[iControl].Hidden)
		return 0;

	const ::gpk::SControlState											& controlState											= gui.Controls.States	[iControl];
	const ::gpk::SControl												& control												= gui.Controls.Controls	[iControl];
	::gpk::SColorBGRA													colors			[::gpk::GUI_CONTROL_AREA_COUNT]			= {}; // -- Fill color table
	::gpk::GUI_COLOR_MODE												colorMode												= (mode.ColorMode == ::gpk::GUI_COLOR_MODE_DEFAULT) ? gui.ColorModeDefault : mode.ColorMode;
	const ::gpk::SControlTheme											& theme													= gui.ControlThemes[(0 == control.ColorTheme) ? gui.ThemeDefault : control.ColorTheme - 1];
	const ::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>	& colorCombo											= theme.ColorCombos
		[ controlState.Disabled	? ::gpk::GUI_CONTROL_STATE_COLORS_DISABLED
		: controlState.Pressed	? ::gpk::GUI_CONTROL_STATE_COLORS_PRESSED 
		: controlState.Selected	? ::gpk::GUI_CONTROL_STATE_COLORS_SELECTED
		: controlState.Hover	? ::gpk::GUI_CONTROL_STATE_COLORS_HOVER 
		: ::gpk::GUI_CONTROL_STATE_COLORS_NORMAL
		];
	colors[::gpk::GUI_CONTROL_AREA_BACKGROUND			]			= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]];
	colors[::gpk::GUI_CONTROL_AREA_CLIENT				]			= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_CLIENT		]];

	if(colorMode == ::gpk::GUI_COLOR_MODE_THEME) { 
		colors[::gpk::GUI_CONTROL_AREA_CLIENT			]				= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_CLIENT		]];
		colors[::gpk::GUI_CONTROL_AREA_BORDER_LEFT		]				= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT	]];
		colors[::gpk::GUI_CONTROL_AREA_BORDER_TOP		]				= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_BORDER_TOP	]];
		colors[::gpk::GUI_CONTROL_AREA_BORDER_RIGHT		]				= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT	]];
		colors[::gpk::GUI_CONTROL_AREA_BORDER_BOTTOM	]				= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM	]];
	}
	else { // 3d borders and ::gpk::GUI_COLOR_MODE_DEFAULT, or just unrecognized mode. This is simpler because here we define the colors ourselves.
		colors[::gpk::GUI_CONTROL_AREA_CLIENT			]				= colors[::gpk::GUI_CONTROL_AREA_CLIENT		] * (controlState.Pressed ? 1.0 : controlState.Hover ? 1.0 : 1.0);
		colors[::gpk::GUI_CONTROL_AREA_BORDER_LEFT		]				= colors[::gpk::GUI_CONTROL_AREA_BACKGROUND	] * (controlState.Pressed ? 0.4 : controlState.Hover ? 0.8 : 1.2); 
		colors[::gpk::GUI_CONTROL_AREA_BORDER_TOP		]				= colors[::gpk::GUI_CONTROL_AREA_BACKGROUND	] * (controlState.Pressed ? 0.4 : controlState.Hover ? 0.8 : 1.2);
		colors[::gpk::GUI_CONTROL_AREA_BORDER_RIGHT		]				= colors[::gpk::GUI_CONTROL_AREA_BACKGROUND	] * (controlState.Pressed ? 1.5 : controlState.Hover ? 1.2 : 0.8); 
		colors[::gpk::GUI_CONTROL_AREA_BORDER_BOTTOM	]				= colors[::gpk::GUI_CONTROL_AREA_BACKGROUND	] * (controlState.Pressed ? 1.5 : controlState.Hover ? 1.2 : 0.8); 
	}
	const ::gpk::SControlMetrics										& controlMetrics										= gui.Controls.Metrics[iControl];
	::gpk::SRectangle2D<int32_t>										finalRects	[::gpk::GUI_CONTROL_AREA_COUNT]				= {};
	::gpk::SRectLimits<int32_t>											scaledBorders											= {};

	scaledBorders.Left												= (int32_t)(control.Border.Left		* gui.Zoom.DPI.x * gui.Zoom.ZoomLevel);
	scaledBorders.Top												= (int32_t)(control.Border.Top		* gui.Zoom.DPI.y * gui.Zoom.ZoomLevel);
	scaledBorders.Right												= (int32_t)(control.Border.Right	* gui.Zoom.DPI.x * gui.Zoom.ZoomLevel);
	scaledBorders.Bottom											= (int32_t)(control.Border.Bottom	* gui.Zoom.DPI.y * gui.Zoom.ZoomLevel);

	finalRects[::gpk::GUI_CONTROL_AREA_BACKGROUND		]			= controlMetrics.Total.Global ; 
	finalRects[::gpk::GUI_CONTROL_AREA_CLIENT			]			= controlMetrics.Client.Global; 
	finalRects[::gpk::GUI_CONTROL_AREA_BORDER_LEFT		]			= {controlMetrics.Total.Global.Offset, ::gpk::SCoord2<int32_t>{control.Border.Left, controlMetrics.Total.Global.Size.y}}; 
	finalRects[::gpk::GUI_CONTROL_AREA_BORDER_TOP		]			= {controlMetrics.Total.Global.Offset, ::gpk::SCoord2<int32_t>{controlMetrics.Total.Global.Size.x, control.Border.Top}}; 
	finalRects[::gpk::GUI_CONTROL_AREA_BORDER_RIGHT		]			= {controlMetrics.Total.Global.Offset + ::gpk::SCoord2<int32_t>{controlMetrics.Total.Global.Size.x - control.Border.Right, 0}, ::gpk::SCoord2<int32_t>{control.Border.Right, controlMetrics.Total.Global.Size.y}};
	finalRects[::gpk::GUI_CONTROL_AREA_BORDER_BOTTOM	]			= {controlMetrics.Total.Global.Offset + ::gpk::SCoord2<int32_t>{0, controlMetrics.Total.Global.Size.y - control.Border.Bottom}, ::gpk::SCoord2<int32_t>{controlMetrics.Total.Global.Size.x, control.Border.Bottom}};

	for(uint32_t iElement = 0; iElement < ::gpk::GUI_CONTROL_AREA_COUNT; ++iElement)
		if(iElement != ::gpk::GUI_CONTROL_AREA_CLIENT)
			::gpk::drawRectangle(target, colors[iElement], finalRects[iElement]);

	// --- Draw control corners
	::gpk::SCoord2<int32_t>												startOffset												= controlMetrics.Total.Global.Offset;
	::gpk::STriangle2D<int32_t>											triangles [8]											= {};
	triangles[0]													= {startOffset, startOffset + ::gpk::SCoord2<int32_t>{control.Border.Left, control.Border.Top}, startOffset + ::gpk::SCoord2<int32_t>{control.Border.Left, 0}	};
	triangles[1]													= {startOffset, startOffset + ::gpk::SCoord2<int32_t>{0, control.Border.Top}, startOffset + ::gpk::SCoord2<int32_t>{control.Border.Left, control.Border.Top}	};

	int32_t																startOffsetX											= startOffset.x + controlMetrics.Total.Global.Size.x - control.Border.Right;
	startOffset														= {startOffsetX, controlMetrics.Total.Global.Offset.y};
	triangles[2]													= {startOffset, startOffset + ::gpk::SCoord2<int32_t>{0, control.Border.Top}, startOffset + ::gpk::SCoord2<int32_t>{control.Border.Right, 0}	};
	triangles[3]													= {startOffset + ::gpk::SCoord2<int32_t>{control.Border.Right, 0}, startOffset + ::gpk::SCoord2<int32_t>{0, control.Border.Top}, startOffset + ::gpk::SCoord2<int32_t>{control.Border.Right, control.Border.Top}	};

	int32_t																startOffsetY											= startOffset.y + controlMetrics.Total.Global.Size.y - control.Border.Bottom;
	startOffset														= {controlMetrics.Total.Global.Offset.x, startOffsetY};
	triangles[4]													= {startOffset, startOffset + ::gpk::SCoord2<int32_t>{0, control.Border.Bottom}, startOffset + ::gpk::SCoord2<int32_t>{control.Border.Left, 0}	};
	triangles[5]													= {startOffset + ::gpk::SCoord2<int32_t>{control.Border.Right, 0}, startOffset + ::gpk::SCoord2<int32_t>{0, control.Border.Top}, startOffset + ::gpk::SCoord2<int32_t>{control.Border.Right, control.Border.Top}	};

	startOffset														= controlMetrics.Total.Global.Offset + controlMetrics.Total.Global.Size - ::gpk::SCoord2<int32_t>{control.Border.Right, control.Border.Bottom};
	triangles[6]													= {startOffset, startOffset + ::gpk::SCoord2<int32_t>{control.Border.Right, control.Border.Bottom}, startOffset + ::gpk::SCoord2<int32_t>{control.Border.Right, 0}	};
	triangles[7]													= {startOffset, startOffset + ::gpk::SCoord2<int32_t>{0, control.Border.Bottom}, startOffset + ::gpk::SCoord2<int32_t>{control.Border.Right, control.Border.Bottom}	};
	int32_t																colorIndices [8]										=
		{ ::gpk::GUI_CONTROL_AREA_BORDER_TOP		
		, ::gpk::GUI_CONTROL_AREA_BORDER_LEFT		
		, ::gpk::GUI_CONTROL_AREA_BORDER_TOP		
		, ::gpk::GUI_CONTROL_AREA_BORDER_RIGHT	
		, ::gpk::GUI_CONTROL_AREA_BORDER_LEFT		
		, ::gpk::GUI_CONTROL_AREA_BORDER_BOTTOM	
		, ::gpk::GUI_CONTROL_AREA_BORDER_RIGHT	
		, ::gpk::GUI_CONTROL_AREA_BORDER_BOTTOM	
		};
	for(uint32_t iTri = 0; iTri < 8; ++iTri)
		::gpk::drawTriangle(target, colors[colorIndices[iTri]], triangles[iTri]);

	::controlTextDraw(gui, iControl, target);
	return 0;
}

static		::gpk::error_t										controlUpdateMetricsTopToDown							(::gpk::SGUI& gui, int32_t iControl, const ::gpk::SCoord2<uint32_t> & targetSize)					{
	::controlUpdateMetrics(gui, iControl, targetSize, true);
	::gpk::array_view<int32_t>											& children												= gui.Controls.Children[iControl];
	for(uint32_t iChild = 0; iChild < children.size(); ++iChild)
		::controlUpdateMetricsTopToDown(gui, children[iChild], targetSize);
	return 0;
}

			::gpk::error_t										gpk::guiUpdateMetrics									(::gpk::SGUI& gui, const ::gpk::SCoord2<uint32_t> & targetSize)										{
	for(uint32_t iControl = 0; iControl < gui.Controls.Controls.size(); ++iControl)
		if(::controlInvalid(gui, gui.Controls.Controls[iControl].IndexParent))
			::controlUpdateMetricsTopToDown(gui, iControl, targetSize);
	return 0;
}

			::gpk::error_t										gpk::controlUpdateMetrics								(::gpk::SGUI& gui, int32_t iControl, const ::gpk::SCoord2<uint32_t> & targetSize)					{
	gpk_necall(::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	::controlUpdateMetricsTopToDown(gui, iControl, targetSize);
	return 0;
}

			::gpk::error_t										gpk::controlDrawHierarchy								(::gpk::SGUI& gui, int32_t iControl, ::gpk::grid_view<::gpk::SColorBGRA>& target)	{
	gpk_necall(::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	if(gui.LastSize != target.metrics()) {
		for(uint32_t iOutdated = 0; iOutdated < gui.Controls.Controls.size(); ++iOutdated)
			gui.Controls.States[iOutdated].Outdated							= true;
		//::controlUpdateMetricsTopToDown(gui, iControl, target.metrics());
		gui.LastSize													= target.metrics();
	}
	if(false == gui.Controls.Constraints[iControl].Hidden) {
		::actualControlDraw(gui, iControl, target);
		::gpk::array_view<int32_t>											& children												= gui.Controls.Children[iControl];
		for(uint32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild) 
			::gpk::controlDrawHierarchy(gui, children[iChild], target);
	}
	return 0;
}

static		::gpk::error_t										updateGUIControlHovered									(::gpk::SControlState& controlFlags, const ::gpk::SInput& inputSystem)	noexcept	{ 
	if(controlFlags.Hover) {
		if(inputSystem.ButtonDown(0) && false == controlFlags.Pressed) 
			controlFlags.Pressed											= true;
		else if(inputSystem.ButtonUp(0) && controlFlags.Pressed) {
			controlFlags.Execute											= true;
			controlFlags.Pressed											= false;
		}
	}
	else 
		controlFlags.Hover												= false == controlFlags.Disabled;
	return controlFlags.Hover;
}

static		::gpk::error_t										controlProcessInput										(::gpk::SGUI& gui, ::gpk::SInput& input, int32_t iControl)							{
	::gpk::SControlState												& controlState											= gui.Controls.States[iControl];
	::gpk::SControlConstraints											& controlConstraints									= gui.Controls.Constraints[iControl];
	if(controlConstraints.Hidden)
		return -1;
	// EXECUTE only lasts one tick.
	if (controlState.Execute)
		controlState.Execute											= false;
	//--------------------
	::gpk::error_t														controlHovered											= -1;
	if(::gpk::in_range(gui.CursorPos.Cast<int32_t>(), gui.Controls.Metrics[iControl].Total.Global)) {
		if(false == gui.Controls.Modes[iControl].Design) {
			controlHovered													= iControl;
			::updateGUIControlHovered(controlState, input);
		}
	}
	else {
		if (controlState.Hover) 
			controlState.Hover												= false;

		if(input.ButtonUp(0) && controlState.Pressed)
			controlState.Pressed											= false;
	}
	{
		::gpk::array_view<int32_t>											& children													= gui.Controls.Children[iControl];
		for(uint32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild) {
			::gpk::error_t														controlPressed												= ::controlProcessInput(gui, input, children[iChild]);
			if(gui.Controls.Controls.size() > (uint32_t)controlPressed) {
				controlState.Hover												= false;
				//controlState.Pressed											= false;
				controlHovered													= controlPressed;
			}
		}
	}
	return controlHovered;
}

			::gpk::error_t										gpk::guiProcessInput									(::gpk::SGUI& gui, ::gpk::SInput& input)											{
	gui.CursorPos													+= {(float)input.MouseCurrent.Deltas.x, (float)input.MouseCurrent.Deltas.y};
	::gpk::error_t														controlHovered											= -1;
	for(uint32_t iControl = 0, countControls = gui.Controls.Controls.size(); iControl < countControls; ++iControl) {
		::gpk::SControlState												& controlState											= gui.Controls.States[iControl];
		if(controlState.Unused || controlState.Disabled)
			continue;
		::gpk::SControl														& control												= gui.Controls.Controls[iControl];
		if(gui.Controls.Controls.size() > (uint32_t)control.IndexParent)	// Only process root parents
			continue;
		::gpk::error_t														controlPressed											= ::controlProcessInput(gui, input, iControl);
		if(gui.Controls.Controls.size() > (uint32_t)controlPressed && false == gui.Controls.States[iControl].Unused)
			controlHovered													= controlPressed;
	}
	if(controlHovered == -1) 
		return gui.Controls.Controls.size();
	for(uint32_t iControl = 0, countControls = gui.Controls.Controls.size(); iControl < countControls; ++iControl) {
		if(iControl != (uint32_t)controlHovered) {
			gui.Controls.States[iControl].Hover								= false;
			if(0 == input.MouseCurrent.ButtonState[0])
				gui.Controls.States[iControl].Pressed							= false;
		}
		else {
			verbose_printf("Hovered: %u.", iControl);
		}
	}
	return controlHovered;
}

			::gpk::error_t										gpk::guiDraw								(::gpk::SGUI& gui, ::gpk::grid_view<::gpk::SColorBGRA>& target)	{
	if(gui.LastSize != target.metrics()) {
		for(uint32_t iOutdated = 0; iOutdated < gui.Controls.Controls.size(); ++iOutdated)
			gui.Controls.States[iOutdated].Outdated							= true;
		gui.LastSize													= target.metrics();
	}
	for(uint32_t iControl = 0; iControl < gui.Controls.Controls.size(); ++iControl)
		if(false == ::controlInvalid(gui, iControl) && ::controlInvalid(gui, gui.Controls.Controls[iControl].IndexParent))
			::gpk::controlDrawHierarchy(gui, iControl, target);
	return 0;
}

