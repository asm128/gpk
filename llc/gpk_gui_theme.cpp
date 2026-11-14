#include "gpk_gui_theme.h"
#include "gpk_apod_color.h"
#include "gpk_font.h"
#include "gpk_ascii_color.h"

gpk::err_t gpk::guiSetupDefaultFonts	(SGUI & gui)			{
	sttc SRasterFontManager	g_managedFonts;
	sttc u2_c				g_resultFontsLoaded	= rasterFontDefaults(g_managedFonts);
	(void)g_resultFontsLoaded;
	if_fail_fe(gui.Fonts.resize(g_managedFonts.Fonts.size()));
	for(u2_t iFont = 0; iFont < g_managedFonts.Fonts.size(); ++iFont)
		gui.Fonts[iFont] = g_managedFonts.Fonts[iFont];

	if_fail_fe(guiUpdateMetrics(gui, gui.LastSize, true));
	return 0;
}

usns gpk;

sttc gpk::err_t	paletteSetupDefault		(a8bgra & palette, const view<const bgra> & colors, u2_t iShades)	{
	if_true_fe(0 == iShades);
	const u2_t				newPaletteSize				= colors.size() * iShades;
	if(palette.size() < newPaletteSize)
		if_fail_fe(palette.resize(newPaletteSize));

	for(u2_t iTone = 0; iTone < colors.size(); ++iTone)
		for(u2_t iShade = 0; iShade < iShades; ++iShade) {
			const s2_t				toneIndex					= iTone * iShades;
			const bgra			& baseColor					= colors[iTone];
			bgra					& paletteItem				= palette[toneIndex + iShade];
			paletteItem				= rgbaf(baseColor) / (f2_t)iShades * (iShades - (f2_t)iShade);
			paletteItem.a			= 0xFF;
		}

	return 0;
}

sttc gpk::err_t themeSetupDefault	(const view<const bgra> & palette, s2_t iColor, SControlTheme & theme, u2_t iShades)	{
	astu2_t<UI_CONTROL_AREA_COUNT>		& colorComboDisabled									= theme.ColorCombos[GUI_CONTROL_PALETTE_DISABLED	]	= {};
	astu2_t<UI_CONTROL_AREA_COUNT>		& colorComboPressed 									= theme.ColorCombos[GUI_CONTROL_PALETTE_PRESSED	]	= {};
	astu2_t<UI_CONTROL_AREA_COUNT>		& colorComboDragging									= theme.ColorCombos[GUI_CONTROL_PALETTE_DRAGGING	]	= {};
	astu2_t<UI_CONTROL_AREA_COUNT>		& colorComboSelected									= theme.ColorCombos[GUI_CONTROL_PALETTE_SELECTED	]	= {};
	astu2_t<UI_CONTROL_AREA_COUNT>		& colorComboHover 										= theme.ColorCombos[GUI_CONTROL_PALETTE_HOVER	]	= {};
	astu2_t<UI_CONTROL_AREA_COUNT>		& colorComboNormal										= theme.ColorCombos[GUI_CONTROL_PALETTE_NORMAL	]	= {};

	//const s2_t														colorBase												= iColor / iShades;
	s2_c			colorShade												= iColor % iShades;
	s2_c			colorText												= (ASCII_COLOR_WHITE * iShades);
	s2_c			halfShades												= (s2_t)iShades / 2;
	s2_c			selectedShades										= (s2_t)iShades / 8;
	s2_c			colorSelected											= iColor + ((colorShade < halfShades) ? selectedShades : -selectedShades);
	s2_c			colorDragging											= iColor + ((colorShade < halfShades) ? 1 : -1);
	s2_c			draggingShade											= colorDragging % iShades;

	colorComboDisabled	[UI_CONTROL_AREA_BACKGROUND	]	= iColor;//(colorShade > 7) ? iColor : iColor;
	colorComboDisabled	[UI_CONTROL_AREA_BORDER_LEFT	]	= (colorShade > 7) ? iColor - 1 : iColor + 1;
	colorComboDisabled	[UI_CONTROL_AREA_BORDER_TOP	]	= (colorShade > 7) ? iColor - 1 : iColor + 1;
	colorComboDisabled	[UI_CONTROL_AREA_BORDER_RIGHT	]	= (colorShade > 7) ? iColor - 1 : iColor + 1;
	colorComboDisabled	[UI_CONTROL_AREA_BORDER_BOTTOM	]	= (colorShade > 7) ? iColor - 1 : iColor + 1;
	colorComboDisabled	[UI_CONTROL_AREA_TEXT_FACE		]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
//
	colorComboPressed 	[UI_CONTROL_AREA_BACKGROUND	]	= iColor;//(colorShade > 7) ? iColor : iColor;
	colorComboPressed 	[UI_CONTROL_AREA_BORDER_LEFT	]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
	colorComboPressed 	[UI_CONTROL_AREA_BORDER_TOP	]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
	colorComboPressed 	[UI_CONTROL_AREA_BORDER_RIGHT	]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
	colorComboPressed 	[UI_CONTROL_AREA_BORDER_BOTTOM	]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
	colorComboPressed 	[UI_CONTROL_AREA_TEXT_FACE		]	= (colorShade > 7) ? colorText : colorText + 15;
//
	colorComboDragging	[UI_CONTROL_AREA_BACKGROUND	]	= colorDragging;
	colorComboDragging	[UI_CONTROL_AREA_BORDER_LEFT	]	= (draggingShade > 7) ? colorDragging - 3 : colorDragging + 3;
	colorComboDragging	[UI_CONTROL_AREA_BORDER_TOP	]	= (draggingShade > 7) ? colorDragging - 3 : colorDragging + 3;
	colorComboDragging	[UI_CONTROL_AREA_BORDER_RIGHT	]	= (draggingShade > 7) ? colorDragging - 3 : colorDragging + 3;
	colorComboDragging	[UI_CONTROL_AREA_BORDER_BOTTOM	]	= (draggingShade > 7) ? colorDragging - 3 : colorDragging + 3;
	colorComboDragging	[UI_CONTROL_AREA_TEXT_FACE		]	= colorComboPressed[UI_CONTROL_AREA_TEXT_FACE];
//
	colorComboSelected	[UI_CONTROL_AREA_BACKGROUND	]	= colorSelected;
	colorComboSelected	[UI_CONTROL_AREA_BORDER_LEFT	]	= colorSelected;
	colorComboSelected	[UI_CONTROL_AREA_BORDER_TOP	]	= colorSelected;
	colorComboSelected	[UI_CONTROL_AREA_BORDER_RIGHT	]	= colorSelected;
	colorComboSelected	[UI_CONTROL_AREA_BORDER_BOTTOM	]	= colorSelected;
	colorComboSelected	[UI_CONTROL_AREA_TEXT_FACE		]	= (colorShade > 7) ? colorText + 4 : colorText + 12;
//
	colorComboHover 	[UI_CONTROL_AREA_BACKGROUND	]	= iColor;//(colorShade > 7) ? iColor : iColor;
	colorComboHover 	[UI_CONTROL_AREA_BORDER_LEFT	]	= (colorShade > 7) ? iColor - 7: iColor + 7;
	colorComboHover 	[UI_CONTROL_AREA_BORDER_TOP	]	= (colorShade > 7) ? iColor - 7: iColor + 7;
	colorComboHover 	[UI_CONTROL_AREA_BORDER_RIGHT	]	= (colorShade > 7) ? iColor - 7: iColor + 7;
	colorComboHover 	[UI_CONTROL_AREA_BORDER_BOTTOM	]	= (colorShade > 7) ? iColor - 7: iColor + 7;
	colorComboHover 	[UI_CONTROL_AREA_TEXT_FACE		]	= (colorShade > 7) ? colorText + 4: colorText + 12;
//
	colorComboNormal	[UI_CONTROL_AREA_BACKGROUND	]	= iColor;//(colorShade > 7) ? iColor : iColor;
	colorComboNormal	[UI_CONTROL_AREA_BORDER_LEFT	]	= (colorShade > 7) ? iColor - 5: iColor + 5;
	colorComboNormal	[UI_CONTROL_AREA_BORDER_TOP	]	= (colorShade > 7) ? iColor - 5: iColor + 5;
	colorComboNormal	[UI_CONTROL_AREA_BORDER_RIGHT	]	= (colorShade > 7) ? iColor - 5: iColor + 5;
	colorComboNormal	[UI_CONTROL_AREA_BORDER_BOTTOM	]	= (colorShade > 7) ? iColor - 5: iColor + 5;
	colorComboNormal	[UI_CONTROL_AREA_TEXT_FACE		]	= (colorShade > 7) ? colorText + 4: colorText + 12;

	colorComboDisabled	[UI_CONTROL_AREA_TEXT_BACKGROUND	]	= colorComboDisabled	[UI_CONTROL_AREA_BACKGROUND];
	colorComboDisabled	[UI_CONTROL_AREA_CLIENT			]	= colorComboDisabled	[UI_CONTROL_AREA_BACKGROUND];
	colorComboPressed 	[UI_CONTROL_AREA_TEXT_BACKGROUND	]	= colorComboPressed		[UI_CONTROL_AREA_BACKGROUND];
	colorComboPressed 	[UI_CONTROL_AREA_CLIENT			]	= colorComboPressed 	[UI_CONTROL_AREA_BACKGROUND];
	colorComboDragging	[UI_CONTROL_AREA_TEXT_BACKGROUND	]	= colorComboDragging	[UI_CONTROL_AREA_BACKGROUND];
	colorComboDragging	[UI_CONTROL_AREA_CLIENT			]	= colorComboDragging	[UI_CONTROL_AREA_BACKGROUND];
	colorComboSelected	[UI_CONTROL_AREA_TEXT_BACKGROUND	]	= colorComboSelected	[UI_CONTROL_AREA_BACKGROUND];
	colorComboSelected	[UI_CONTROL_AREA_CLIENT			]	= colorComboSelected	[UI_CONTROL_AREA_BACKGROUND];
	colorComboHover 	[UI_CONTROL_AREA_TEXT_BACKGROUND	]	= colorComboHover		[UI_CONTROL_AREA_BACKGROUND];
	colorComboHover 	[UI_CONTROL_AREA_CLIENT			]	= colorComboHover 		[UI_CONTROL_AREA_BACKGROUND];
	colorComboNormal	[UI_CONTROL_AREA_TEXT_BACKGROUND	]	= colorComboNormal		[UI_CONTROL_AREA_BACKGROUND];
	colorComboNormal	[UI_CONTROL_AREA_CLIENT			]	= colorComboNormal		[UI_CONTROL_AREA_BACKGROUND];

	for(u2_t iState = 0; iState < theme.ColorCombos.size(); ++iState)
		for(u2_t iArea = 0; iArea < UI_CONTROL_AREA_COUNT; ++iArea) {
			theme.ColorCombos[iState][iArea]	= min((u2_t)theme.ColorCombos[iState][iArea], palette.size() - 1);
		}
	return 0;
}

static	gpk::err_t themeSetupDefault	(const a8bgra & palette, apod<SControlTheme>& themes, u2_t iShades)	{
	for(u2_t iColor = 0; iColor < palette.size(); ++iColor) {
		const s2_t				indexTheme			= themes.push_back({});
		SControlTheme		& theme				= themes[indexTheme];
		::themeSetupDefault(palette, iColor, theme, iShades);
	}
	return 0;
}

static	gpk::err_t initDefaults		(pobj<a8bgra> & palette, pobj<apod<SControlTheme>> & controlThemes) {
	stxp	u2_t			iShades				= 16;
	static	bgra			paletteColors []	=
		// 16 Base colors
		{ SColorRGBA{ASCII_COLOR_INDEX_0	}
		, SColorRGBA{ASCII_COLOR_INDEX_1	}
		, SColorRGBA{ASCII_COLOR_INDEX_2	}
		, SColorRGBA{ASCII_COLOR_INDEX_3	}
		, SColorRGBA{ASCII_COLOR_INDEX_4	}
		, SColorRGBA{ASCII_COLOR_INDEX_5	}
		, SColorRGBA{ASCII_COLOR_INDEX_6	}
		, SColorRGBA{ASCII_COLOR_INDEX_7	}
		, SColorRGBA{ASCII_COLOR_INDEX_8	}
		, SColorRGBA{ASCII_COLOR_INDEX_9	}
		, SColorRGBA{ASCII_COLOR_INDEX_10	}
		, SColorRGBA{ASCII_COLOR_INDEX_11	}
		, SColorRGBA{ASCII_COLOR_INDEX_12	}
		, SColorRGBA{ASCII_COLOR_INDEX_13	}
		, SColorRGBA{ASCII_COLOR_INDEX_14	}
		, SColorRGBA{ASCII_COLOR_INDEX_15	}
		// 16 Extended colors
		, SColorRGBA{ASCII_COLOR_INDEX_14	+ ASCII_COLOR_INDEX_1}
		, SColorRGBA{ASCII_COLOR_INDEX_13	+ ASCII_COLOR_INDEX_2}
		, SColorRGBA{ASCII_COLOR_INDEX_12	+ ASCII_COLOR_INDEX_3}
		, SColorRGBA{ASCII_COLOR_INDEX_11	+ ASCII_COLOR_INDEX_4}
		, SColorRGBA{ASCII_COLOR_INDEX_10	+ ASCII_COLOR_INDEX_5}
		, SColorRGBA{ASCII_COLOR_INDEX_9 	+ ASCII_COLOR_INDEX_6}
		, SColorRGBA{ASCII_COLOR_INDEX_8 	+ ASCII_COLOR_INDEX_7}
		, SColorRGBA{0xFFFFFFFFU & (ASCII_COLOR_INDEX_4 * (u3_t)ASCII_COLOR_INDEX_11)}
		, SColorRGBA{0xFFFFFFFFU & (ASCII_COLOR_INDEX_5 * (u3_t)ASCII_COLOR_INDEX_10)}
		, SColorRGBA{0xFFFFFFFFU & (ASCII_COLOR_INDEX_7 * (u3_t)ASCII_COLOR_INDEX_8 )}
		, SColorRGBA{(u2_t)(ASCII_COLOR_INDEX_1 + ASCII_COLOR_INDEX_4)}
		, SColorRGBA{(u2_t)(ASCII_COLOR_INDEX_3 + ASCII_COLOR_INDEX_7)}
		, SColorRGBA{(u2_t)(ASCII_COLOR_INDEX_1 + ASCII_COLOR_INDEX_8)}
		, SColorRGBA{(u2_t)(ASCII_COLOR_INDEX_4 + ASCII_COLOR_INDEX_7)}

		, BROWN
		, LIGHTORANGE
		// Additional UI tones. The common generator produces their 16-shade ranges.
		, SColorBGRA{248, 208,  72} // Azure
		, SColorBGRA{ 32, 176, 248} // Amber
		, SColorBGRA{232, 232, 232} // Mono
	};
	for(u2_t iColor = 0; iColor < size(paletteColors); ++iColor) 
		paletteColors[iColor].a = 255;

	if(palette && 0 == palette->size()) {
		if_fail_fe(::paletteSetupDefault(*palette, paletteColors, iShades));
	}
	else if(!palette) 
		if_fail_fe(::paletteSetupDefault(*palette.create(), paletteColors, iShades));

	if(controlThemes && 0 == controlThemes->size()) {
		if_fail_fe(::themeSetupDefault(*palette, *controlThemes, iShades));
	}
	else if(!controlThemes)
		if_fail_fe(::themeSetupDefault(*palette, *controlThemes.create(), iShades));
	return 0;
}

static	gpk::err_t paletteSetupDefaultColors	(pobj<a8bgra> & palette, pobj<apod<SControlTheme>> & controlThemes)	{
	static papod<bgra>			globalDefaultPalette		= {};
	static papod<SControlTheme>	globalDefaultTheme			= {};
	
	if(!globalDefaultPalette || 0 == globalDefaultPalette->size()) 
		::initDefaults(globalDefaultPalette, globalDefaultTheme);

	palette			= globalDefaultPalette;
	controlThemes	= globalDefaultTheme;
	return 0;
}

static	gpk::err_t uiColorsShade
	( asttc<bgra, UI_CONTROL_AREA_COUNT> & output
	, const asttc<bgra, UI_CONTROL_AREA_COUNT> & base
	, u2_t shade
	, u2_t shades
	) {
	if_true_fe(0 == shades);
	if_true_fe(shade >= shades);
	for(u2_t iArea = 0; iArea < output.size(); ++iArea) {
		output[iArea] = rgbaf(base[iArea]) / (f2_t)shades * (f2_t)(shades - shade);
		output[iArea].a = base[iArea].a;
	}
	return 0;
}

static	gpk::err_t uiColorsSetupDefault (SGUIColors & guiColors) {
	stxp u2_t shades = 16;
	asttc<bgra, UI_CONTROL_AREA_COUNT> shaded = {};
	guiColors.DefaultColors[GUI_CONTROL_PALETTE_NORMAL] = (u1_t)guiColors.Palettes.push_back({{
		BLUE, GRAY, YELLOW, MAGENTA, CYAN, {},
		WHITE, WHITE,
		WHITE, WHITE, WHITE, WHITE,
		WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,
	}});
	guiColors.DefaultColors[GUI_CONTROL_PALETTE_DISABLED] = (u1_t)guiColors.Palettes.push_back({{
		LIGHTGRAY, ORANGE, YELLOW, MAGENTA, CYAN, {},
		LIGHTGRAY * 1.1, LIGHTGRAY * 1.1,
		LIGHTGRAY * 1.1, LIGHTGRAY * 1.1, LIGHTGRAY * 1.1, LIGHTGRAY * 1.1,
		LIGHTGRAY * 1.1, LIGHTGRAY * 1.1, LIGHTGRAY * 1.1, LIGHTGRAY * 1.1, LIGHTGRAY * 1.1, LIGHTGRAY * 1.1, LIGHTGRAY * 1.1, LIGHTGRAY * 1.1,
	}});
	guiColors.DefaultColors[GUI_CONTROL_PALETTE_HOVER] = (u1_t)guiColors.Palettes.push_back({{
		CYAN, YELLOW, MAGENTA, GREEN, ORANGE, {},
		DARKCYAN / 2.0, DARKCYAN / 2.0,
		DARKCYAN / 2.0, DARKCYAN / 2.0, DARKCYAN / 2.0, DARKCYAN / 2.0,
		DARKCYAN / 2.0, DARKCYAN / 2.0, DARKCYAN / 2.0, DARKCYAN / 2.0, DARKCYAN / 2.0, DARKCYAN / 2.0, DARKCYAN / 2.0, DARKCYAN / 2.0,
	}});
	guiColors.DefaultColors[GUI_CONTROL_PALETTE_PRESSED] = (u1_t)guiColors.Palettes.push_back({{
		RED, ORANGE, YELLOW, MAGENTA, CYAN, {},
		WHITE, WHITE,
		WHITE, WHITE, WHITE, WHITE,
		WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,
	}});
	if_fail_fe(::uiColorsShade(shaded, guiColors.Palettes[guiColors.DefaultColors[GUI_CONTROL_PALETTE_NORMAL]], 1, shades));
	guiColors.DefaultColors[GUI_CONTROL_PALETTE_DRAGGING] = (u1_t)guiColors.Palettes.push_back(shaded);
	if_fail_fe(::uiColorsShade(shaded, guiColors.Palettes[guiColors.DefaultColors[GUI_CONTROL_PALETTE_NORMAL]], 8, shades));
	guiColors.DefaultColors[GUI_CONTROL_PALETTE_SELECTED] = (u1_t)guiColors.Palettes.push_back(shaded);
	if_fail_fe(::uiColorsShade(shaded, guiColors.Palettes[guiColors.DefaultColors[GUI_CONTROL_PALETTE_DISABLED]], 8, shades));
	guiColors.DefaultColors[GUI_CONTROL_PALETTE_SELECTED_DISABLED] = (u1_t)guiColors.Palettes.push_back(shaded);
	if_fail_fe(::uiColorsShade(shaded, guiColors.Palettes[guiColors.DefaultColors[GUI_CONTROL_PALETTE_HOVER]], 8, shades));
	guiColors.DefaultColors[GUI_CONTROL_PALETTE_SELECTED_HOVER] = (u1_t)guiColors.Palettes.push_back(shaded);
	if_fail_fe(::uiColorsShade(shaded, guiColors.Palettes[guiColors.DefaultColors[GUI_CONTROL_PALETTE_PRESSED]], 8, shades));
	guiColors.DefaultColors[GUI_CONTROL_PALETTE_SELECTED_PRESSED] = (u1_t)guiColors.Palettes.push_back(shaded);
	if_fail_fe(::uiColorsShade(shaded, guiColors.Palettes[guiColors.DefaultColors[GUI_CONTROL_PALETTE_DRAGGING]], 8, shades));
	guiColors.DefaultColors[GUI_CONTROL_PALETTE_SELECTED_DRAGGING] = (u1_t)guiColors.Palettes.push_back(shaded);
	guiColors.DefaultColors[GUI_CONTROL_PALETTE_OUTDATED] = (u1_t)guiColors.Palettes.push_back({{
		BLUE, ORANGE, YELLOW, MAGENTA, CYAN, {},
		WHITE, WHITE,
		WHITE, WHITE, WHITE, WHITE,
		WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,
	}});
	return 0;
}
gpk::err_t gpk::guiSetupDefaultColors (pobj<SGUIColors> & guiColors) {
	if(!guiColors) {
		static pobj<SGUIColors>	globalDefaultColors			= {};
		if(!globalDefaultColors) {
			if_fail_fe(::uiColorsSetupDefault(*globalDefaultColors.create()));
			if_fail_fe(::paletteSetupDefaultColors(globalDefaultColors->Palette, globalDefaultColors->ControlThemes));
		}
		guiColors	= globalDefaultColors;
	}
	return 0;
}
