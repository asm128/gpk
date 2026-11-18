#include "gpk_gui_control_theme.h"
#include "gpk_apod_color.h"
#include "gpk_ascii_color.h"

GPK_USING_TYPEINT();

gpk::err_t gpk::controlFontsDefault	(apobj<SRasterFont> & output)			{
	sttc SRasterFontManager	s_managedFonts;
	sttc u2_c				s_resultFontsLoaded	= rasterFontDefaults(s_managedFonts);
	(void)s_resultFontsLoaded;
	if_fail_fe(output.resize(s_managedFonts.Fonts.size()));
	for(u2_t iFont = 0; iFont < s_managedFonts.Fonts.size(); ++iFont)
		output[iFont] = s_managedFonts.Fonts[iFont];
	return 0;
}

usns gpk;

sttc gpk::err_t	paletteSetupDefault		(a8bgra & palette, cnst view<cnst bgra> & colors, u2_t iShades)	{
	if_true_fe(0 == iShades);
	u2_c				newPaletteSize				= colors.size() * iShades;
	if(palette.size() < newPaletteSize)
		if_fail_fe(palette.resize(newPaletteSize));

	for(u2_t iTone = 0; iTone < colors.size(); ++iTone)
		for(u2_t iShade = 0; iShade < iShades; ++iShade) {
			cnst s2_t				toneIndex					= iTone * iShades;
			cnst bgra			& baseColor					= colors[iTone];
			bgra					& paletteItem				= palette[toneIndex + iShade];
			paletteItem				= rgbaf(baseColor) / (f2_t)iShades * (iShades - (f2_t)iShade);
			paletteItem.a			= 0xFF;
		}

	return 0;
}

sttc gpk::err_t themeSetupDefault	(cnst view<cnst bgra> & palette, s2_t iColor, SControlTheme & theme, u2_t iShades)	{
	astu2_t<UI_CONTROL_AREA_COUNT>		& colorComboDisabled									= theme.ColorCombos[GUI_CONTROL_PALETTE_DISABLED	]	= {};
	astu2_t<UI_CONTROL_AREA_COUNT>		& colorComboPressed 									= theme.ColorCombos[GUI_CONTROL_PALETTE_PRESSED	]	= {};
	astu2_t<UI_CONTROL_AREA_COUNT>		& colorComboDragging									= theme.ColorCombos[GUI_CONTROL_PALETTE_DRAGGING	]	= {};
	astu2_t<UI_CONTROL_AREA_COUNT>		& colorComboSelected									= theme.ColorCombos[GUI_CONTROL_PALETTE_SELECTED	]	= {};
	astu2_t<UI_CONTROL_AREA_COUNT>		& colorComboHover 										= theme.ColorCombos[GUI_CONTROL_PALETTE_HOVER	]	= {};
	astu2_t<UI_CONTROL_AREA_COUNT>		& colorComboNormal										= theme.ColorCombos[GUI_CONTROL_PALETTE_NORMAL	]	= {};

	//cnst s2_t														colorBase												= iColor / iShades;
	s2_c			colorShade												= iColor % iShades;
	s2_c			colorText												= (ASCII_COLOR_White * iShades);
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

sttc	gpk::err_t themeSetupDefault	(cnst a8bgra & palette, apod<SControlTheme>& themes, u2_t iShades)	{
	for(u2_t iColor = 0; iColor < palette.size(); ++iColor) {
		cnst s2_t				indexTheme			= themes.push_back({});
		SControlTheme		& theme				= themes[indexTheme];
		::themeSetupDefault(palette, iColor, theme, iShades);
	}
	return 0;
}

sttc	gpk::err_t initDefaults		(pobj<a8bgra> & palette, pobj<apod<SControlTheme>> & controlThemes) {
	stxp	u2_t			iShades				= 16;
	sttc	rgba			paletteVGA []		=
		{ {  0,   0,   0} // Black
		, {  0,   0, 170} // Blue
		, {  0, 170,   0} // Green
		, {  0, 170, 170} // Cyan
		, {170,   0,   0} // Red
		, {170,   0, 170} // Magenta
		, {170,  85,   0} // Brown
		, {170, 170, 170} // Light Grey
		, { 85,  85,  85} // Dark Grey
		, { 85,  85, 255} // Light Blue
		, { 85, 255,  85} // Light Green
		, { 85, 255, 255} // Light Cyan
		, {255,  85,  85} // Light Red
		, {255,  85, 255} // Light Magenta
		, {255, 255,  85} // Yellow
		, {255, 255, 255} // White
	};
	stxp	u2_t			countVGA			= size(paletteVGA);
	a8bgra				paletteColors		= {};
	if_fail_fe(paletteColors.resize(countVGA * 2 + 3));
	for(u2_t iColor = 0; iColor < countVGA; ++iColor) {
		paletteColors[iColor]			= paletteVGA[iColor];
		paletteColors[countVGA + iColor]	= paletteVGA[iColor] * 0.75 + rgba{32, 32, 32};
	}
	// Additional UI tones. The common generator produces their 16-shade ranges.
	paletteColors[countVGA * 2 + 0]	= rgba{ 72, 208, 248}; // Azure
	paletteColors[countVGA * 2 + 1]	= rgba{248, 176,  32}; // Amber
	paletteColors[countVGA * 2 + 2]	= rgba{232, 232, 232}; // Mono

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

sttc	gpk::err_t paletteSetupDefaultColors	(pobj<a8bgra> & palette, pobj<apod<::gpk::SControlTheme>> & controlThemes)	{
	sttc papod<bgra>					globalDefaultPalette		= {};
	sttc papod<::gpk::SControlTheme>	globalDefaultTheme			= {};
	
	if(0 == globalDefaultPalette->size()) 
		::initDefaults(globalDefaultPalette, globalDefaultTheme);

	palette			= globalDefaultPalette;
	controlThemes	= globalDefaultTheme;
	return 0;
}

gpk::err_t gpk::controlColorsShade
	( asttc<bgra, UI_CONTROL_AREA_COUNT> & output
	, view<cnst bgra> base
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

gpk::err_t gpk::controlColorsDefault (SControlColors & guiColors) {
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
	if_fail_fe(controlColorsShade(shaded, guiColors.Palettes[guiColors.DefaultColors[GUI_CONTROL_PALETTE_NORMAL]], 1, shades));
	guiColors.DefaultColors[GUI_CONTROL_PALETTE_DRAGGING] = (u1_t)guiColors.Palettes.push_back(shaded);
	if_fail_fe(controlColorsShade(shaded, guiColors.Palettes[guiColors.DefaultColors[GUI_CONTROL_PALETTE_NORMAL]], 8, shades));
	guiColors.DefaultColors[GUI_CONTROL_PALETTE_SELECTED] = (u1_t)guiColors.Palettes.push_back(shaded);
	if_fail_fe(controlColorsShade(shaded, guiColors.Palettes[guiColors.DefaultColors[GUI_CONTROL_PALETTE_DISABLED]], 8, shades));
	guiColors.DefaultColors[GUI_CONTROL_PALETTE_SELECTED_DISABLED] = (u1_t)guiColors.Palettes.push_back(shaded);
	if_fail_fe(controlColorsShade(shaded, guiColors.Palettes[guiColors.DefaultColors[GUI_CONTROL_PALETTE_HOVER]], 8, shades));
	guiColors.DefaultColors[GUI_CONTROL_PALETTE_SELECTED_HOVER] = (u1_t)guiColors.Palettes.push_back(shaded);
	if_fail_fe(controlColorsShade(shaded, guiColors.Palettes[guiColors.DefaultColors[GUI_CONTROL_PALETTE_PRESSED]], 8, shades));
	guiColors.DefaultColors[GUI_CONTROL_PALETTE_SELECTED_PRESSED] = (u1_t)guiColors.Palettes.push_back(shaded);
	if_fail_fe(controlColorsShade(shaded, guiColors.Palettes[guiColors.DefaultColors[GUI_CONTROL_PALETTE_DRAGGING]], 8, shades));
	guiColors.DefaultColors[GUI_CONTROL_PALETTE_SELECTED_DRAGGING] = (u1_t)guiColors.Palettes.push_back(shaded);
	guiColors.DefaultColors[GUI_CONTROL_PALETTE_OUTDATED] = (u1_t)guiColors.Palettes.push_back({{
		BLUE, ORANGE, YELLOW, MAGENTA, CYAN, {},
		WHITE, WHITE,
		WHITE, WHITE, WHITE, WHITE,
		WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,
	}});
	return 0;
}
gpk::err_t gpk::controlColorsDefault (pobj<SControlColors> & guiColors) {
	if(!guiColors) {
		sttc pobj<SControlColors>	globalDefaultColors			= {};
		if(!globalDefaultColors) {
			if_fail_fe(controlColorsDefault(*globalDefaultColors.create()));
			if_fail_fe(::paletteSetupDefaultColors(globalDefaultColors->Palette, globalDefaultColors->ControlThemes));
		}
		guiColors	= globalDefaultColors;
	}
	return 0;
}
