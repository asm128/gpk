#include "gpk_gui.h"
#include "gpk_gui_text.h"
#include "gpk_base64.h"
#include "gpk_rect_align.h"

// GUI_CONTROL_COLOR_BACKGROUND
// GUI_CONTROL_COLOR_BORDER_LEFT
// GUI_CONTROL_COLOR_BORDER_TOP
// GUI_CONTROL_COLOR_BORDER_RIGHT
// GUI_CONTROL_COLOR_BORDER_BOTTOM
// GUI_CONTROL_COLOR_TEXT_BACKGROUND
// GUI_CONTROL_COLOR_TEXT_FACE
// GUI_CONTROL_COLOR_MARGIN_LEFT
// GUI_CONTROL_COLOR_MARGIN_TOP
// GUI_CONTROL_COLOR_MARGIN_RIGHT
// GUI_CONTROL_COLOR_MARGIN_BOTTOM
// GUI_CONTROL_COLOR_CORNER_LEFT_TOP_TOP
// GUI_CONTROL_COLOR_CORNER_LEFT_TOP_LEFT
// GUI_CONTROL_COLOR_CORNER_RIGHT_TOP_TOP
// GUI_CONTROL_COLOR_CORNER_RIGHT_TOP_RIGHT
// GUI_CONTROL_COLOR_CORNER_LEFT_BOTTOM_LEFT
// GUI_CONTROL_COLOR_CORNER_LEFT_BOTTOM_BOTTOM
// GUI_CONTROL_COLOR_CORNER_RIGHT_BOTTOM_RIGHT
// GUI_CONTROL_COLOR_CORNER_RIGHT_BOTTOM_BOTTOM
// GUI_CONTROL_COLOR_CLIENT

::gpk::error_t			gpk::controlInvalid						(const ::gpk::SGUI & gui, int32_t iControl)				{
	if(gui.Controls.Controls.size() <= uint32_t(iControl)	) return -1;
	if(gui.Controls.States[iControl].Unused					) return -1;
	return 0;
}

static	::gpk::error_t	paletteSetupDefault										(::gpk::apod<::gpk::bgra>& palette, const ::gpk::view<const ::gpk::bgra>& colors, uint32_t iShades)	{
	const uint32_t				newPaletteSize											= colors.size() * iShades;
	if(palette.size() < newPaletteSize)
		gpk_necall(palette.resize(newPaletteSize), "%s", "Out of memory?");
	for(uint32_t iTone = 0; iTone < colors.size(); ++iTone)
		palette[iTone * iShades]										= colors[iTone];

	for(uint32_t iShade = 0; iShade < iShades; ++iShade) {
		const ::gpk::bgra												& baseColor												= palette[iShade * iShades];
		::gpk::bgra													& paletteItem											= palette[iShade];
		paletteItem														= baseColor;
	}

	for(uint32_t iTone = 1; iTone < colors.size(); ++iTone)
		for(uint32_t iShade = 0; iShade < iShades; ++iShade) {
			if(0 == (iShade % iShades))
				continue;
			const int32_t														toneIndex												= iTone * iShades;
			const ::gpk::bgra												& baseColor												= colors[iTone];
			::gpk::bgra													& paletteItem											= palette[toneIndex + iShade];
			paletteItem														= ::gpk::rgbaf(baseColor) / (float)iShades * (iShades - (float)iShade);
			paletteItem.a = 1;
			verbose_printf("Original color: {r: 0x%X, g: 0x%X, b: 0x%X}.", baseColor	.r, baseColor	.g, baseColor	.b);
			verbose_printf("Shaded color  : {r: 0x%X, g: 0x%X, b: 0x%X}.", paletteItem	.r, paletteItem	.g, paletteItem	.b);
		}

	return 0;
}

static	::gpk::error_t	themeSetupDefault						(const ::gpk::view<const ::gpk::bgra>& palette, int32_t iColor, ::gpk::SControlTheme& theme, uint32_t iShades)	{
	::gpk::array_static<uint32_t, ::gpk::UI_CONTROL_AREA_COUNT>		& colorComboDisabled									= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_DISABLED	]	= {};
	::gpk::array_static<uint32_t, ::gpk::UI_CONTROL_AREA_COUNT>		& colorComboPressed 									= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_PRESSED	]	= {};
	::gpk::array_static<uint32_t, ::gpk::UI_CONTROL_AREA_COUNT>		& colorComboSelected									= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_SELECTED	]	= {};
	::gpk::array_static<uint32_t, ::gpk::UI_CONTROL_AREA_COUNT>		& colorComboHover 										= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_HOVER	]	= {};
	::gpk::array_static<uint32_t, ::gpk::UI_CONTROL_AREA_COUNT>		& colorComboNormal										= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_NORMAL	]	= {};

	//const int32_t														colorBase												= iColor / iShades;
	const int32_t														colorShade												= iColor % iShades;
	const int32_t														colorText												= (::gpk::ASCII_COLOR_WHITE * iShades);

	colorComboDisabled	[::gpk::UI_CONTROL_AREA_BACKGROUND	]	= iColor;//(colorShade > 7) ? iColor : iColor;
	colorComboDisabled	[::gpk::UI_CONTROL_AREA_BORDER_LEFT	]	= (colorShade > 7) ? iColor - 1 : iColor + 1;
	colorComboDisabled	[::gpk::UI_CONTROL_AREA_BORDER_TOP	]	= (colorShade > 7) ? iColor - 1 : iColor + 1;
	colorComboDisabled	[::gpk::UI_CONTROL_AREA_BORDER_RIGHT	]	= (colorShade > 7) ? iColor - 1 : iColor + 1;
	colorComboDisabled	[::gpk::UI_CONTROL_AREA_BORDER_BOTTOM	]	= (colorShade > 7) ? iColor - 1 : iColor + 1;
	colorComboDisabled	[::gpk::UI_CONTROL_AREA_TEXT_FACE		]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
//
	colorComboPressed 	[::gpk::UI_CONTROL_AREA_BACKGROUND	]	= iColor;//(colorShade > 7) ? iColor : iColor;
	colorComboPressed 	[::gpk::UI_CONTROL_AREA_BORDER_LEFT	]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
	colorComboPressed 	[::gpk::UI_CONTROL_AREA_BORDER_TOP	]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
	colorComboPressed 	[::gpk::UI_CONTROL_AREA_BORDER_RIGHT	]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
	colorComboPressed 	[::gpk::UI_CONTROL_AREA_BORDER_BOTTOM	]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
	colorComboPressed 	[::gpk::UI_CONTROL_AREA_TEXT_FACE		]	= (colorShade > 7) ? colorText + 7 : colorText + 9;
//
	colorComboSelected	[::gpk::UI_CONTROL_AREA_BACKGROUND	]	= (colorShade > 7) ? colorText - 7 : colorText + 9;
	colorComboSelected	[::gpk::UI_CONTROL_AREA_BORDER_LEFT	]	= (colorShade > 7) ? colorText - 7 : colorText + 9;
	colorComboSelected	[::gpk::UI_CONTROL_AREA_BORDER_TOP	]	= (colorShade > 7) ? colorText - 7 : colorText + 9;
	colorComboSelected	[::gpk::UI_CONTROL_AREA_BORDER_RIGHT	]	= (colorShade > 7) ? colorText - 7 : colorText + 9;
	colorComboSelected	[::gpk::UI_CONTROL_AREA_BORDER_BOTTOM	]	= (colorShade > 7) ? colorText - 7 : colorText + 9;
	colorComboSelected	[::gpk::UI_CONTROL_AREA_TEXT_FACE		]	= (colorShade > 7) ? colorText : colorText + 8;
//
	colorComboHover 	[::gpk::UI_CONTROL_AREA_BACKGROUND	]	= iColor;//(colorShade > 7) ? iColor : iColor;
	colorComboHover 	[::gpk::UI_CONTROL_AREA_BORDER_LEFT	]	= (colorShade > 7) ? iColor - 7: iColor + 7;
	colorComboHover 	[::gpk::UI_CONTROL_AREA_BORDER_TOP	]	= (colorShade > 7) ? iColor - 7: iColor + 7;
	colorComboHover 	[::gpk::UI_CONTROL_AREA_BORDER_RIGHT	]	= (colorShade > 7) ? iColor - 7: iColor + 7;
	colorComboHover 	[::gpk::UI_CONTROL_AREA_BORDER_BOTTOM	]	= (colorShade > 7) ? iColor - 7: iColor + 7;
	colorComboHover 	[::gpk::UI_CONTROL_AREA_TEXT_FACE		]	= (colorShade > 7) ? colorText + 4: colorText + 12;
//
	colorComboNormal	[::gpk::UI_CONTROL_AREA_BACKGROUND	]	= iColor;//(colorShade > 7) ? iColor : iColor;
	colorComboNormal	[::gpk::UI_CONTROL_AREA_BORDER_LEFT	]	= (colorShade > 7) ? iColor - 5: iColor + 5;
	colorComboNormal	[::gpk::UI_CONTROL_AREA_BORDER_TOP	]	= (colorShade > 7) ? iColor - 5: iColor + 5;
	colorComboNormal	[::gpk::UI_CONTROL_AREA_BORDER_RIGHT	]	= (colorShade > 7) ? iColor - 5: iColor + 5;
	colorComboNormal	[::gpk::UI_CONTROL_AREA_BORDER_BOTTOM	]	= (colorShade > 7) ? iColor - 5: iColor + 5;
	colorComboNormal	[::gpk::UI_CONTROL_AREA_TEXT_FACE		]	= (colorShade > 7) ? colorText : colorText + 15;

	colorComboDisabled	[::gpk::UI_CONTROL_AREA_TEXT_BACKGROUND	]	= colorComboDisabled	[::gpk::UI_CONTROL_AREA_BACKGROUND];
	colorComboDisabled	[::gpk::UI_CONTROL_AREA_CLIENT			]	= colorComboDisabled	[::gpk::UI_CONTROL_AREA_BACKGROUND];
	colorComboPressed 	[::gpk::UI_CONTROL_AREA_TEXT_BACKGROUND	]	= colorComboPressed		[::gpk::UI_CONTROL_AREA_BACKGROUND];
	colorComboPressed 	[::gpk::UI_CONTROL_AREA_CLIENT			]	= colorComboPressed 	[::gpk::UI_CONTROL_AREA_BACKGROUND];
	colorComboSelected	[::gpk::UI_CONTROL_AREA_TEXT_BACKGROUND	]	= colorComboSelected	[::gpk::UI_CONTROL_AREA_BACKGROUND];
	colorComboSelected	[::gpk::UI_CONTROL_AREA_CLIENT			]	= colorComboSelected	[::gpk::UI_CONTROL_AREA_BACKGROUND];
	colorComboHover 	[::gpk::UI_CONTROL_AREA_TEXT_BACKGROUND	]	= colorComboHover		[::gpk::UI_CONTROL_AREA_BACKGROUND];
	colorComboHover 	[::gpk::UI_CONTROL_AREA_CLIENT			]	= colorComboHover 		[::gpk::UI_CONTROL_AREA_BACKGROUND];
	colorComboNormal	[::gpk::UI_CONTROL_AREA_TEXT_BACKGROUND	]	= colorComboNormal		[::gpk::UI_CONTROL_AREA_BACKGROUND];
	colorComboNormal	[::gpk::UI_CONTROL_AREA_CLIENT			]	= colorComboNormal		[::gpk::UI_CONTROL_AREA_BACKGROUND];

	for(uint32_t iState = 0; iState < theme.ColorCombos.size(); ++iState)
		for(uint32_t iArea = 0; iArea < ::gpk::UI_CONTROL_AREA_COUNT; ++iArea) {
			theme.ColorCombos[iState][iArea]							= ::gpk::min((uint32_t)theme.ColorCombos[iState][iArea], palette.size() - 1);
		}
	return 0;
}

static	::gpk::error_t	themeSetupDefault	(const ::gpk::apod<::gpk::bgra>& palette, ::gpk::apod<::gpk::SControlTheme>& themes, uint32_t iShades)	{
	for(uint32_t iColor = 0; iColor < palette.size(); ++iColor) {
		const int32_t				indexTheme			= themes.push_back({});
		::gpk::SControlTheme		& theme				= themes[indexTheme];
		::themeSetupDefault(palette, iColor, theme, iShades);
	}
	return 0;
}

static	::gpk::error_t	initDefaults		(::gpk::pobj<::gpk::apod<::gpk::bgra>> & palette, ::gpk::pobj<::gpk::apod<::gpk::SControlTheme>> & controlThemes) {
	stacxpr	uint32_t			iShades				= 16;
	static	::gpk::bgra			paletteColors []	=
		// 16 Base colors
		{ ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_0	}
		, ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_1	}
		, ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_2	}
		, ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_3	}
		, ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_4	}
		, ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_5	}
		, ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_6	}
		, ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_7	}
		, ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_8	}
		, ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_9	}
		, ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_10	}
		, ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_11	}
		, ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_12	}
		, ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_13	}
		, ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_14	}
		, ::gpk::SColorRGBA{::gpk::ASCII_COLOR_INDEX_15	}
		// 16 Extended colors
		, ::gpk::bgra{::gpk::ASCII_COLOR_INDEX_14	+ ::gpk::ASCII_COLOR_INDEX_1}
		, ::gpk::bgra{::gpk::ASCII_COLOR_INDEX_13	+ ::gpk::ASCII_COLOR_INDEX_2}
		, ::gpk::bgra{::gpk::ASCII_COLOR_INDEX_12	+ ::gpk::ASCII_COLOR_INDEX_3}
		, ::gpk::bgra{::gpk::ASCII_COLOR_INDEX_11	+ ::gpk::ASCII_COLOR_INDEX_4}
		, ::gpk::bgra{::gpk::ASCII_COLOR_INDEX_10	+ ::gpk::ASCII_COLOR_INDEX_5}
		, ::gpk::bgra{::gpk::ASCII_COLOR_INDEX_9 	+ ::gpk::ASCII_COLOR_INDEX_6}
		, ::gpk::bgra{::gpk::ASCII_COLOR_INDEX_8 	+ ::gpk::ASCII_COLOR_INDEX_7}
		, ::gpk::bgra{0xFFFFFFFFU & (::gpk::ASCII_COLOR_INDEX_4 * (uint64_t)::gpk::ASCII_COLOR_INDEX_11)}
		, ::gpk::bgra{0xFFFFFFFFU & (::gpk::ASCII_COLOR_INDEX_5 * (uint64_t)::gpk::ASCII_COLOR_INDEX_10)}
		, ::gpk::bgra{0xFFFFFFFFU & (::gpk::ASCII_COLOR_INDEX_7 * (uint64_t)::gpk::ASCII_COLOR_INDEX_8 )}
		, ::gpk::bgra{(uint32_t)(::gpk::ASCII_COLOR_INDEX_1 + ::gpk::ASCII_COLOR_INDEX_4)}
		, ::gpk::bgra{(uint32_t)(::gpk::ASCII_COLOR_INDEX_3 + ::gpk::ASCII_COLOR_INDEX_7)}
		, ::gpk::bgra{(uint32_t)(::gpk::ASCII_COLOR_INDEX_1 + ::gpk::ASCII_COLOR_INDEX_8)}
		, ::gpk::bgra{(uint32_t)(::gpk::ASCII_COLOR_INDEX_4 + ::gpk::ASCII_COLOR_INDEX_7)}

		, ::gpk::bgra{::gpk::BROWN}
		, ::gpk::bgra{::gpk::LIGHTORANGE}
	};
	for(uint32_t iColor = 0; iColor < ::gpk::size(paletteColors); ++iColor) 
		paletteColors[iColor].a = 255;

	if(palette && 0 == palette->size())
		gpk_necs(::paletteSetupDefault(*palette, paletteColors, iShades));
	else if(!palette) 
		gpk_necs(::paletteSetupDefault(*palette.create(), paletteColors, iShades));

	if(controlThemes && 0 == controlThemes->size())
		gpk_necs(::themeSetupDefault(*palette, *controlThemes, iShades));
	else if(!controlThemes)
		gpk_necs(::themeSetupDefault(*palette, *controlThemes.create(), iShades));
	return 0;
}

static	::gpk::error_t	paletteSetupDefaultColors	(::gpk::pobj<::gpk::apod<::gpk::bgra>> & palette, ::gpk::pobj<::gpk::apod<::gpk::SControlTheme>> & controlThemes)	{
	static ::gpk::papod<::gpk::bgra>			globalDefaultPalette		= {};
	static ::gpk::papod<::gpk::SControlTheme>	globalDefaultTheme			= {};
	
	if(!globalDefaultPalette || 0 == globalDefaultPalette->size()) 
		::initDefaults(globalDefaultPalette, globalDefaultTheme);

	palette			= globalDefaultPalette;
	controlThemes	= globalDefaultTheme;
	return 0;
}

static	::gpk::error_t	uiColorsSetupDefault (::gpk::SGUIColors & guiColors) {
	guiColors.DefaultColors[::gpk::GUI_CONTROL_PALETTE_NORMAL				]	= (uint16_t)guiColors.Palettes.push_back({{::gpk::BLUE		, ::gpk::GRAY	, ::gpk::YELLOW, ::gpk::MAGENTA, ::gpk::CYAN, {}, ::gpk::WHITE				,}});
	guiColors.DefaultColors[::gpk::GUI_CONTROL_PALETTE_DISABLED				]	= (uint16_t)guiColors.Palettes.push_back({{::gpk::LIGHTGRAY	, ::gpk::ORANGE	, ::gpk::YELLOW, ::gpk::MAGENTA, ::gpk::CYAN, {}, ::gpk::LIGHTGRAY * 1.1	,}});
	guiColors.DefaultColors[::gpk::GUI_CONTROL_PALETTE_HOVER				]	= (uint16_t)guiColors.Palettes.push_back({{::gpk::CYAN		, ::gpk::YELLOW	, ::gpk::MAGENTA, ::gpk::GREEN, ::gpk::ORANGE, {}, ::gpk::DARKCYAN / 2.0	,}});
	guiColors.DefaultColors[::gpk::GUI_CONTROL_PALETTE_PRESSED				]	= (uint16_t)guiColors.Palettes.push_back({{::gpk::RED		, ::gpk::ORANGE	, ::gpk::YELLOW, ::gpk::MAGENTA, ::gpk::CYAN, {}, ::gpk::WHITE				,}});
	guiColors.DefaultColors[::gpk::GUI_CONTROL_PALETTE_SELECTED				]	= (uint16_t)guiColors.Palettes.push_back({{::gpk::ORANGE	, ::gpk::ORANGE	, ::gpk::YELLOW, ::gpk::MAGENTA, ::gpk::CYAN, {}, ::gpk::WHITE				,}});
	guiColors.DefaultColors[::gpk::GUI_CONTROL_PALETTE_SELECTED_DISABLED	]	= (uint16_t)guiColors.Palettes.push_back({{::gpk::BLUE		, ::gpk::ORANGE	, ::gpk::YELLOW, ::gpk::MAGENTA, ::gpk::CYAN, {}, ::gpk::WHITE				,}});
	guiColors.DefaultColors[::gpk::GUI_CONTROL_PALETTE_SELECTED_HOVER		]	= (uint16_t)guiColors.Palettes.push_back({{::gpk::BLUE		, ::gpk::ORANGE	, ::gpk::YELLOW, ::gpk::MAGENTA, ::gpk::CYAN, {}, ::gpk::WHITE				,}});
	guiColors.DefaultColors[::gpk::GUI_CONTROL_PALETTE_SELECTED_PRESSED		]	= (uint16_t)guiColors.Palettes.push_back({{::gpk::BLUE		, ::gpk::ORANGE	, ::gpk::YELLOW, ::gpk::MAGENTA, ::gpk::CYAN, {}, ::gpk::WHITE				,}});
	guiColors.DefaultColors[::gpk::GUI_CONTROL_PALETTE_EXECUTE				]	= (uint16_t)guiColors.Palettes.push_back({{::gpk::BLUE		, ::gpk::ORANGE	, ::gpk::YELLOW, ::gpk::MAGENTA, ::gpk::CYAN, {}, ::gpk::WHITE				,}});
	guiColors.DefaultColors[::gpk::GUI_CONTROL_PALETTE_OUTDATED				]	= (uint16_t)guiColors.Palettes.push_back({{::gpk::BLUE		, ::gpk::ORANGE	, ::gpk::YELLOW, ::gpk::MAGENTA, ::gpk::CYAN, {}, ::gpk::WHITE				,}});
	return 0;
}

static	::gpk::error_t	uiColorsSetupDefault (::gpk::pobj<::gpk::SGUIColors> & guiColors) {
	if(!guiColors) {
		static ::gpk::pobj<::gpk::SGUIColors>	globalDefaultColors			= {};
		if(!globalDefaultColors) {
			gpk_necs(::uiColorsSetupDefault(*globalDefaultColors.create()));
			gpk_necs(::paletteSetupDefaultColors(globalDefaultColors->Palette, globalDefaultColors->ControlThemes));
		}
		guiColors	= globalDefaultColors;
	}
	return 0;
}

static	::gpk::error_t	setupDefaultFontTexture	(::gpk::SGUI & gui)			{
	static ::gpk::SRasterFontManager	g_managedFonts;
	static const uint32_t				g_resultFontsLoaded	= ::gpk::rasterFontDefaults(g_managedFonts);
	(void)g_resultFontsLoaded;
	gui.Fonts.resize(g_managedFonts.Fonts.size());
	for(uint32_t iFont = 0; iFont < g_managedFonts.Fonts.size(); ++iFont)
		gui.Fonts[iFont] = g_managedFonts.Fonts[iFont];

	gpk_necs(::gpk::guiUpdateMetrics(gui, gui.LastSize, true));
	return 0;
}

static	::gpk::error_t	controlInstanceReset	(::gpk::SControlTable & controlTable, int32_t iControl, ::gpk::vi16 defaultColors)										{
	controlTable.Metrics	[iControl]	= {};
	controlTable.Children	[iControl]	= ::gpk::vcid{};
	controlTable.Modes		[iControl]	= {};
	controlTable.States		[iControl]	= {};
	controlTable.Images		[iControl]	= {};
	//controlTable.Modes		[iControl].UseNewPalettes				= 1;
	::gpk::SControl				& control				= controlTable.Controls		[iControl]	= {};
	::gpk::SControlPlacement	& controlPlacement		= controlTable.Placement	[iControl]	= {};
	::gpk::SControlDraw			& controlDraw			= controlTable.Draw			[iControl]	= {};
	::gpk::SControlConstraints	& controlConstraints	= controlTable.Constraints	[iControl]	= {};
	::gpk::SControlText			& controlText			= controlTable.Text			[iControl]	= {};
	control.Parent			= -1;
	controlPlacement.Align	= ::gpk::ALIGN_TOP_LEFT;
	controlPlacement.Area	= {{0, 0}, {16, 16}};
	for(uint32_t iColor = 0, countColors = ::gpk::min(controlDraw.Palettes.size(), defaultColors.size()); iColor < countColors; ++iColor)
		controlDraw.Palettes[iColor] = defaultColors[iColor];

	controlConstraints.AttachSizeToControl	= {-1, -1};
	controlConstraints.AttachSizeToText		= {};
	controlConstraints.DockToControl		= {-1, -1, -1, -1};
	controlText.Align						= ::gpk::ALIGN_CENTER;
	return 0;
}

::gpk::error_t			gpk::controlCreateChild	(::gpk::SGUI & gui, int32_t iParent)						{
	int32_t						idControl				= ::gpk::controlCreate(gui);
	::gpk::controlSetParent(gui, idControl, iParent);
	return idControl;
}


::gpk::error_t			gpk::controlCreate		(::gpk::SControlTable & controlTable, ::gpk::vi16 defaultColors)	{
	for(uint32_t iControl = 0; iControl < controlTable.States.size(); ++iControl) {
		if(controlTable.States[iControl].Unused) {
			gpk_necall(::controlInstanceReset(controlTable, iControl, defaultColors), "iControl: %i", iControl);
			return iControl;
		}
	}

	::gpk::error_t				iControl				= -1;
	gpk_necs(iControl = ::gpk::resize(controlTable.Controls.size() + 1
		, controlTable.Controls		
		, controlTable.Placement		
		, controlTable.Metrics			
		, controlTable.Text			
		, controlTable.Constraints		
		, controlTable.States			
		, controlTable.Modes			
		, controlTable.Draw			
		, controlTable.Events			
		, controlTable.RelativeBit		
		, controlTable.Children		
		, controlTable.Images			
		) - 1);
	gpk_necs(::controlInstanceReset(controlTable, iControl, defaultColors));
	return iControl;
}

::gpk::error_t			gpk::controlCreate		(::gpk::SGUI & gui)							{
	gpk_necs(::uiColorsSetupDefault(gui.Colors));

	if(0 == gui.Fonts.size())
		gpk_necs(::setupDefaultFontTexture(gui));

	if(gui.Fonts.size() <= gui.SelectedFont)
		gui.SelectedFont = gui.Fonts.size() - 1;

	::gpk::error_t				iControl				= ::gpk::controlCreate(gui.Controls, gui.Colors->DefaultColors);
	return iControl;
}
::gpk::error_t			gpk::controlDelete		(::gpk::SGUI & gui, int32_t iControl, bool recursive)			{
	gpk_necall(::gpk::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	info_printf("Deleting control id: %i.", iControl);
	if(recursive) { // Delete children first.
		::gpk::ai32					children				= gui.Controls.Children[iControl];
		for(uint32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
			e_if(errored(::gpk::controlDelete(gui, children[iChild], true)), "Failed to delete control! Invalid control id? %i.", children[iChild]);
	}
	// Remove from parent list
	const uint32_t				indexParent				= (uint32_t)gui.Controls.Controls[iControl].Parent;
#if defined(GPK_DEBUG_ENABLED)
	int32_t						childrenRemoved			= 0;
#endif
	if(indexParent < gui.Controls.Controls.size() && false == gui.Controls.States[indexParent].Unused) {
		::gpk::ai32					& children				= gui.Controls.Children[indexParent];
		for(int32_t iChild = 0; iChild < (int32_t)children.size(); ++iChild)
			if(children[iChild] == iControl) {
				gpk_necall(children.remove_unordered(iChild), "Failed to remove child at index: %u.", iChild);
#if defined(GPK_DEBUG_ENABLED)
				--iChild;
				++childrenRemoved;
#else
				break;
#endif
			}
	}
#if defined(GPK_DEBUG_ENABLED)
	gerror_if(childrenRemoved > 1, "%s", "Parent should not reference a child control more than once.");
#endif
	::gpk::SControlState		& controlState			= gui.Controls.States[iControl];
	controlState.Unused		= true;
	return 0;
}

::gpk::error_t			gpk::controlSetParent	(::gpk::SGUI & gui, int32_t iControl, int32_t iParent)	{
	gpk_necall(::gpk::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	const uint32_t				oldParent				= gui.Controls.Controls[iControl].Parent;
	::gpk::SControl				& control				= gui.Controls.Controls[iControl];
	if(control.Parent == iParent)	// Exit early if there is nothing to do here.
		return 0;

	control.Parent			= iParent;
	if(::gpk::controlInvalid(gui, iParent)) {
		ree_if(-1 != iParent, "Invalid parent control id: %u.", iParent);
		return 0;
	}

	if(oldParent < gui.Controls.Controls.size() && false == gui.Controls.States[oldParent].Unused) {
		::gpk::ai32					& children				= gui.Controls.Children[oldParent];
		for(int32_t iChild = 0; iChild < (int32_t)children.size(); ++iChild)
			if(children[iChild] == iControl) {
				gui.Controls.States[oldParent].Updated	= false;
				gpk_necall(children.remove(iChild), "Failed to remove child at index: %u.", iChild);
				break;
			}
	}

	// Set iControl to parent's children array.
	::gpk::ai32					& children				= gui.Controls.Children[iParent];
	for(int32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
		if(children[iChild] == iControl)
			return 0;

	gui.Controls.States[iParent].Updated	= false;
	gui.Controls.States[iControl].Updated	= false;
	gpk_necall(children.push_back(iControl), "%s", "Out of memory?");
	return 0;
}

static	::gpk::error_t	buildControlGeometry	(const ::gpk::SControlPlacement & control, const ::gpk::SControlArea & controlMetrics, const ::gpk::SGUIZoom & zoom, ::gpk::view<::gpk::rect2i16> finalRects, ::gpk::view<::gpk::tri2i16> triangles)					{
	::gpk::recti16				scaledBorders			= {};
	const ::gpk::n2f64			scaleFinal				= zoom.DPI * zoom.ZoomLevel;
	scaledBorders.Left		= (int16_t)(control.Border.Left		* scaleFinal.x);
	scaledBorders.Top		= (int16_t)(control.Border.Top		* scaleFinal.y);
	scaledBorders.Right		= (int16_t)(control.Border.Right	* scaleFinal.x);
	scaledBorders.Bottom	= (int16_t)(control.Border.Bottom	* scaleFinal.y);
	(void)scaledBorders;

	const ::gpk::rect2i16		& rectTotal				= controlMetrics.Total.Global;
	finalRects[::gpk::GUI_CONTROL_AREA_OLD_BACKGROUND		]	= rectTotal;
	finalRects[::gpk::GUI_CONTROL_AREA_OLD_CLIENT			]	= controlMetrics.Client.Global;
	finalRects[::gpk::GUI_CONTROL_AREA_OLD_BORDER_LEFT		]	= {rectTotal.Offset , ::gpk::n2i16{(int16_t)control.Border.Left, rectTotal.Size.y}};
	finalRects[::gpk::GUI_CONTROL_AREA_OLD_BORDER_TOP		]	= {rectTotal.Offset , ::gpk::n2i16{rectTotal.Size.x, (int16_t)control.Border.Top}};
	finalRects[::gpk::GUI_CONTROL_AREA_OLD_BORDER_RIGHT		]	= {rectTotal.Offset + ::gpk::n2i16{int16_t(rectTotal.Size.x - control.Border.Right), 0} , ::gpk::n2i16{(int16_t)control.Border.Right, controlMetrics.Total.Global.Size.y}};
	finalRects[::gpk::GUI_CONTROL_AREA_OLD_BORDER_BOTTOM	]	= {rectTotal.Offset + ::gpk::n2i16{0, int16_t(rectTotal.Size.y - control.Border.Bottom)}, ::gpk::n2i16{rectTotal.Size.x, (int16_t)control.Border.Bottom}};
	finalRects[::gpk::GUI_CONTROL_AREA_OLD_BACKGROUND		].Offset.x	+= control.Border.Left	;
	finalRects[::gpk::GUI_CONTROL_AREA_OLD_BACKGROUND		].Offset.y	+= control.Border.Top	;
	finalRects[::gpk::GUI_CONTROL_AREA_OLD_BACKGROUND		].Size	.x	-= control.Border.Left	+ control.Border.Right	;
	finalRects[::gpk::GUI_CONTROL_AREA_OLD_BACKGROUND		].Size	.y	-= control.Border.Top	+ control.Border.Bottom	;

	// --- Draw control corners
	::gpk::n2i16				startOffset				= controlMetrics.Total.Global.Offset;
	triangles[0]			= {startOffset, startOffset + ::gpk::n2u16{control.Border.Left, control.Border.Top}.i16(), startOffset + ::gpk::n2u16{control.Border.Left, 0U}.i16()	};
	triangles[1]			= {startOffset, startOffset + ::gpk::n2u16{0U, control.Border.Top}.i16(), startOffset + ::gpk::n2u16{control.Border.Left, control.Border.Top}.i16()	};

	int16_t						startOffsetX			= startOffset.x + controlMetrics.Total.Global.Size.x - control.Border.Right;
	startOffset				= {startOffsetX, controlMetrics.Total.Global.Offset.y};
	triangles[2]			= {startOffset, startOffset + ::gpk::n2u16{0U, control.Border.Top}.i16(), startOffset + ::gpk::n2u16{control.Border.Right, 0U}.i16()	};
	triangles[3]			= {startOffset + ::gpk::n2u16{control.Border.Right, 0U}.i16(), startOffset + ::gpk::n2u16{0U, control.Border.Top}.i16(), startOffset + ::gpk::n2u16{control.Border.Right, control.Border.Top}.i16()	};

	int16_t						startOffsetY			= startOffset.y + controlMetrics.Total.Global.Size.y - control.Border.Bottom;
	startOffset				= {controlMetrics.Total.Global.Offset.x, startOffsetY};
	triangles[4]			= {startOffset, startOffset + ::gpk::n2u16{0U, control.Border.Bottom}.i16(), startOffset + ::gpk::n2u16{control.Border.Left, 0U}.i16()	};
	triangles[5]			= {startOffset + ::gpk::n2u16{control.Border.Right, 0U}.i16(), startOffset + ::gpk::n2u16{0U, control.Border.Top}.i16(), startOffset + ::gpk::n2u16{control.Border.Right, control.Border.Top}.i16()	};

	startOffset				= controlMetrics.Total.Global.Offset + controlMetrics.Total.Global.Size - ::gpk::n2u16{control.Border.Right, control.Border.Bottom}.i16();
	triangles[6]			= {startOffset, startOffset + ::gpk::n2u16{control.Border.Right, control.Border.Bottom}.i16(), startOffset + ::gpk::n2u16{control.Border.Right, 0U}.i16()	};
	triangles[7]			= {startOffset, startOffset + ::gpk::n2u16{0U, control.Border.Bottom}.i16(), startOffset + ::gpk::n2u16{control.Border.Right, control.Border.Bottom}.i16()	};
	return 0;
}

static	::gpk::error_t	controlUpdateMetrics	(::gpk::SGUI & gui, int32_t iControl, const ::gpk::n2<uint16_t> & _targetSize)					{
	gpk_necall(::gpk::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);

	::gpk::SControlState		& controlState			= gui.Controls.States[iControl];
	const ::gpk::SControlPlacement	& controlPlacement	= gui.Controls.Placement[iControl];
	const ::gpk::SControl		& control				= gui.Controls.Controls[iControl];
	::gpk::n2f64				scale					= gui.Zoom.DPI * gui.Zoom.ZoomLevel;
	if(fabs(1.0 - scale.x) < 0.001) scale.x = 1.0;
	if(fabs(1.0 - scale.y) < 0.001) scale.y = 1.0;
	::gpk::n2f64				scaledPosition			= {controlPlacement.Area.Offset	.x * scale.x, controlPlacement.Area.Offset	.y * scale.y};
	::gpk::n2f64				scaledSize				= {controlPlacement.Area.Size	.x * scale.x, controlPlacement.Area.Size		.y * scale.y};
	//::gpk::rect<double>	scaledBorders		= {(control.Border.Left * scale.x), (control.Border.Top * scale.y), (control.Border.Right * scale.x), (control.Border.Bottom * scale.y)};
	//::gpk::rect<double>	scaledMargins		= {(control.Margin.Left * scale.x), (control.Margin.Top * scale.y), (control.Margin.Right * scale.x), (control.Margin.Bottom * scale.y)};

	::gpk::SControlArea		& controlMetrics		= gui.Controls.Metrics[iControl];
	::gpk::rect2f64				rectText				= {};
	::gpk::SControlText			& controlText			= gui.Controls.Text[iControl];
	const ::gpk::SRasterFont	& selectedFont			= *gui.Fonts[::gpk::in_range(controlText.FontSelected, (int16_t)0, (int16_t)gui.Fonts.size()) ? controlText.FontSelected : gui.SelectedFont];

	const ::gpk::n2u8			fontCharSize			= selectedFont.CharSize;
	rectText.Size			= {(double)(fontCharSize.x * controlText.Text.size()), (double)fontCharSize.y};
	rectText.Size.InPlaceScale(scale.x, scale.y);

	const ::gpk::SControlConstraints	& controlConstraints	= gui.Controls.Constraints[iControl];
	const ::gpk::rectu16		ncSizes					= ::gpk::controlNCRect(controlPlacement);
	const ::gpk::rectf64		ncSizesScaled			= {(ncSizes.Left * scale.x), (ncSizes.Top * scale.y), (ncSizes.Right * scale.x), (ncSizes.Bottom * scale.y)};
	const ::gpk::n2f64			ncTotalSize				= {(double)ncSizes.Left + ncSizes.Right, (double)ncSizes.Top + ncSizes.Bottom};
	const ::gpk::n2f64			ncTotalSizeScaled		= ncTotalSize.GetScaled(scale.x, scale.y);
	if(controlConstraints.AttachSizeToText.x) scaledSize.x	= rectText.Size.x + ncTotalSizeScaled.x;
	if(controlConstraints.AttachSizeToText.y) scaledSize.y	= rectText.Size.y + ncTotalSizeScaled.y;
	const bool					isValidParent			= 0 == ::gpk::controlInvalid(gui, control.Parent);
	const ::gpk::n2u16			targetSize				= isValidParent ? gui.Controls.Metrics[control.Parent].Client.Global.Size.u16() : _targetSize.u16();

	if(controlConstraints.AttachSizeToControl.x == iControl) { if(controlConstraints.DockToControl.Left	!= -1) {} else { scaledPosition.x = 0; } scaledSize.x = targetSize.x; } else if(false == ::gpk::controlInvalid(gui, controlConstraints.AttachSizeToControl.x)) { if(controlConstraints.DockToControl.Left != -1) {} else { scaledPosition.x = 0;} scaledSize.x = gui.Controls.Metrics[controlConstraints.AttachSizeToControl.x].Total.Global.Size.x; }
	if(controlConstraints.AttachSizeToControl.y == iControl) { if(controlConstraints.DockToControl.Top	!= -1) {} else { scaledPosition.y = 0; } scaledSize.y = targetSize.y; } else if(false == ::gpk::controlInvalid(gui, controlConstraints.AttachSizeToControl.y)) { if(controlConstraints.DockToControl.Top  != -1) {} else { scaledPosition.y = 0;} scaledSize.y = gui.Controls.Metrics[controlConstraints.AttachSizeToControl.y].Total.Global.Size.y; }

	controlMetrics.Client	.Local	= {{(int16_t)ncSizesScaled.Left, (int16_t)ncSizesScaled.Top}, (scaledSize - ncTotalSizeScaled).i16()};
	::gpk::n2f64				minSizeScaled			= (controlConstraints.SizeMinMax.Min.f64().InPlaceScale(scale.x, scale.y));
	::gpk::n2f64				maxSizeScaled			= (controlConstraints.SizeMinMax.Max.f64().InPlaceScale(scale.x, scale.y));
	scaledSize.x			= ::gpk::max(minSizeScaled.x, ::gpk::min(maxSizeScaled.x, scaledSize.x));
	scaledSize.y			= ::gpk::max(minSizeScaled.y, ::gpk::min(maxSizeScaled.y, scaledSize.y));
	::gpk::realignRectangle(targetSize.u32(), ::gpk::rect2<int16_t>{scaledPosition.i16(), scaledSize.i16()}, controlMetrics.Total.Local, controlPlacement.Align);
	controlMetrics.Total .Global	= controlMetrics.Total	.Local;
	controlMetrics.Client.Global	= controlMetrics.Client	.Local;
	controlMetrics.Client.Global.Offset	+= controlMetrics.Total	.Local.Offset;
	if(isValidParent) {
		::gpk::SControlArea		& parentMetrics			= gui.Controls.Metrics[control.Parent];
		controlMetrics.Client.Global.Offset	+= parentMetrics.Client.Global.Offset;
		controlMetrics.Total .Global.Offset	+= parentMetrics.Client.Global.Offset;
	}

	const ::gpk::recti32		& dockToControl			= controlConstraints.DockToControl;
	if(dockToControl.Right	!= -1) {
		gpk_necall(::gpk::controlInvalid(gui, dockToControl.Right), "Invalid control id: %i.", dockToControl.Right);
		::gpk::controlUpdateMetricsTopToDown(gui, dockToControl.Right, targetSize.u16(), true);
		const ::gpk::SControlPlacement	& other				= gui.Controls.Placement[dockToControl.Right];
		const ::gpk::SControlArea		& otherMetrics		= gui.Controls.Metrics	[dockToControl.Right];
		if(gbit_true(other.Align, ::gpk::ALIGN_RIGHT) && gbit_false(other.Align, ::gpk::ALIGN_HCENTER)) {
			controlMetrics.Total .Global.Offset.x	= otherMetrics.Total.Global.Offset.x - controlMetrics.Total.Global.Size.x;
			controlMetrics.Client.Global.Offset.x	= (int16_t)(controlMetrics.Total.Global.Offset.x + ncSizesScaled.Left);
			int16_t						diffToSubstract		= targetSize.x - otherMetrics.Total.Local.Offset.x;
			if(controlConstraints.AttachSizeToControl.x == iControl) {
				controlMetrics.Total .Global.Size.x		-= diffToSubstract;
				controlMetrics.Client.Global.Size.x		-= diffToSubstract;
				controlMetrics.Total .Global.Offset.x	+= diffToSubstract;
				controlMetrics.Client.Global.Offset.x	+= diffToSubstract;
			}
		}
		else {
			controlMetrics.Total .Global.Offset.x	= otherMetrics.Total.Global.Offset.x + otherMetrics.Total.Global.Size.x;
			controlMetrics.Client.Global.Offset.x	= (int16_t)(controlMetrics.Total.Global.Offset.x + ncSizesScaled.Left);
		}
	}
	if(dockToControl.Bottom	!= -1) {
		gpk_necall(::gpk::controlInvalid(gui, dockToControl.Bottom), "Invalid control id: %i.", dockToControl.Bottom);
		const ::gpk::SControlPlacement	& other					= gui.Controls.Placement[dockToControl.Bottom];
		const ::gpk::SControlArea		& otherMetrics			= gui.Controls.Metrics[dockToControl.Bottom	];
		if(gbit_true(other.Align, ::gpk::ALIGN_BOTTOM	) && gbit_false(other.Align, ::gpk::ALIGN_VCENTER)) {
			controlMetrics.Total.Global.Offset.y	= otherMetrics.Total.Global.Offset.y - controlMetrics.Total.Global.Size.y;
			controlMetrics.Client.Global.Offset.y	= (int16_t)(controlMetrics.Total.Global.Offset.y + ncSizesScaled.Top);
		} else {
			controlMetrics.Total.Global.Offset.y	= otherMetrics.Total.Global.Offset.y + otherMetrics.Total.Global.Size.y;
			controlMetrics.Client.Global.Offset.y	= (int16_t)(controlMetrics.Total.Global.Offset.y + ncSizesScaled.Top);
		}
	}
	if(dockToControl.Left	!= -1) {
		gpk_necall(::gpk::controlInvalid(gui, dockToControl.Left), "Invalid control id: %i.", dockToControl.Left);
		::gpk::controlUpdateMetricsTopToDown(gui, dockToControl.Left, targetSize, true);
		const ::gpk::SControlPlacement	& other			= gui.Controls.Placement[dockToControl.Left];
		const ::gpk::SControlArea		& otherMetrics	= gui.Controls.Metrics	[dockToControl.Left];
		if(gbit_true(other.Align, ::gpk::ALIGN_RIGHT) && gbit_false(other.Align, ::gpk::ALIGN_HCENTER)) {
			controlMetrics.Total	.Global.Offset.x	= otherMetrics				.Total.Global.Offset.x + otherMetrics.Total.Global.Size.x - controlMetrics.Total.Global.Size.x;
			controlMetrics.Client	.Global.Offset.x	= int16_t(controlMetrics	.Total.Global.Offset.x + ncSizesScaled.Left);
		}
		else {
			controlMetrics.Total	.Global.Offset.x	= otherMetrics				.Total.Global.Offset.x;
			controlMetrics.Client	.Global.Offset.x	= int16_t(controlMetrics	.Total.Global.Offset.x + ncSizesScaled.Left);
		}
	}
	if(dockToControl.Top	!= -1) {
		gpk_necall(::gpk::controlInvalid(gui, dockToControl.Top), "Invalid control id: %i.", dockToControl.Top);
		const ::gpk::SControlPlacement	& other					= gui.Controls.Placement[dockToControl.Top ];
		const ::gpk::SControlArea		& otherMetrics			= gui.Controls.Metrics	[dockToControl.Top ];
		if(gbit_true(other.Align, ::gpk::ALIGN_BOTTOM	) && gbit_false(other.Align, ::gpk::ALIGN_VCENTER)) {
			controlMetrics.Total.Global.Offset.y	= otherMetrics.Total.Global.Offset.y - controlMetrics.Total.Global.Size.y;
			controlMetrics.Client.Global.Offset.y	= controlMetrics.Total.Global.Offset.y + ncSizes.Top ;
		}
		else {
			controlMetrics.Total.Global.Offset.y	= otherMetrics.Total.Global.Offset.y + controlMetrics.Total.Global.Offset.y;
			controlMetrics.Client.Global.Offset.y	= controlMetrics.Total.Global.Offset.y + ncSizes.Top ;
		}
	}
	{ // calculate text rectangle
		const ::gpk::rect2i16		& targetRect			= controlMetrics.Client.Global;
		controlMetrics.Text.Offset	= {};
		controlMetrics.Text.Size	= rectText.Size.i16();
		::gpk::realignRectangle(targetRect.Size.u32(), controlMetrics.Text, controlMetrics.Text, controlText.Align);
		controlMetrics.Text.Offset	+= controlMetrics.Client.Global.Offset.i16();
	}
	::buildControlGeometry(controlPlacement, controlMetrics, gui.Zoom, controlMetrics.Rectangles, controlMetrics.Triangles);
	controlState.Updated	= true;
	return 0;
}

::gpk::error_t			gpk::controlUpdateMetricsTopToDown							(::gpk::SGUI & gui, int32_t iControl, const ::gpk::n2<uint16_t> & targetSize, bool forceUpdate)				{
	if(false == gui.Controls.States[iControl].Updated || forceUpdate)
		gpk_necall(::controlUpdateMetrics(gui, iControl, targetSize), "%s", "Unknown error! Maybe the control tree got broken?");
	::gpk::view<int32_t>		& children												= gui.Controls.Children[iControl];
	for(uint32_t iChild = 0; iChild < children.size(); ++iChild)
		gpk_necall(::gpk::controlUpdateMetricsTopToDown(gui, children[iChild], targetSize, forceUpdate), "%s", "Unknown error! Maybe the control tree got broken?");
	return 0;
}

::gpk::error_t			gpk::guiUpdateMetrics									(::gpk::SGUI & gui, const ::gpk::n2<uint16_t> & targetSize, bool forceUpdate)								{
	for(uint32_t iControl = 0; iControl < gui.Controls.Controls.size(); ++iControl)
		if(::gpk::controlInvalid(gui, gui.Controls.Controls[iControl].Parent) && false == ::gpk::controlInvalid(gui, iControl))
			gpk_necall(::gpk::controlUpdateMetricsTopToDown(gui, iControl, targetSize, forceUpdate), "%s", "Unknown error! Maybe the control tree got broken?");
	return 0;
}

static	::gpk::error_t	updateGUIControlHovered	(::gpk::SControlState & controlState, ::gpk::SControlEvent & controlEvent, const ::gpk::SInput & inputSystem, bool disabled)	noexcept	{
	if(controlState.Hovered) {
		if(inputSystem.ButtonDown(0) && false == controlState.Pressed) {
			controlState.Pressed	= true;
			controlEvent.Pushed		= true;
		}
		else {
			if(inputSystem.ButtonUp(0)) {
				controlEvent.Released		= true;
				if(controlState.Pressed) {
					controlEvent.Execute		= true;
					controlState.Pressed		= false;
				}
			}
		}
	}
	else {
		if(false == disabled) {
			if(false == controlState.Hovered)
				controlEvent.MouseIn	= true;

			controlState.Hovered = true;//controlFlags.Disabled;
		}
	}
	return one_if(controlState.Hovered);
}

static	::gpk::error_t	controlProcessInput		(::gpk::SGUI & gui, const ::gpk::SInput& input, int32_t iControl)														{
	//--------------------
	::gpk::SControlState		& controlState			= gui.Controls.States[iControl];
	::gpk::SControlEvent		& controlEvents			= gui.Controls.Events[iControl];
	::gpk::error_t				controlHovered			= -1;
	if(::gpk::in_range(gui.CursorPos.i16(), gui.Controls.Metrics[iControl].Total.Global)) {
		if(false == gui.Controls.Modes[iControl].NoHover) {
			controlHovered		= iControl;
			::updateGUIControlHovered(controlState, controlEvents, input, ::gpk::controlDisabled(gui, iControl));
		}
	}
	else {
		if (controlState.Hovered) {
			controlState.Hovered	= false;
			controlEvents.MouseOut	= true;
		}

		if(input.ButtonUp(0) && controlState.Pressed) {
			controlState.Pressed	= false;
			controlEvents.Released	= true;
		}
	}
	{
		::gpk::vi32					& children				= gui.Controls.Children[iControl];
		for(uint32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild) {
			if(::gpk::controlHidden(gui, children[iChild]))
				continue;
			::gpk::error_t				controlPressed			= ::controlProcessInput(gui, input, children[iChild]);
			if(gui.Controls.Controls.size() > (uint32_t)controlPressed) {
				//controlState.Hover		= false;
				controlState.Pressed	= false;
				controlHovered			= controlPressed;
			}
		}
	}
	return controlHovered;
}

::gpk::error_t			gpk::guiProcessInput	(::gpk::SGUI & gui, const ::gpk::SInput& input, ::gpk::v1<const ::gpk::SSysEvent> sysEvents)	{
	if(0 == gui.LastSize.LengthSquared()) 
		return 0;

	es_if(errored(::gpk::guiUpdateMetrics(gui, gui.LastSize, false)));
	::gpk::error_t				controlHovered			= -1;
	::gpk::au32					rootControlsToProcess	= {};
	rootControlsToProcess.reserve(1000);
	(void)sysEvents;
	for(uint32_t iControl = 0, countControls = gui.Controls.Controls.size(); iControl < countControls; ++iControl) {	// Only process root parents
		const ::gpk::SControlState	& controlState			= gui.Controls.States[iControl];
		if(controlState.Unused)
			continue;

		if(controlState.Disabled)
			continue;

		::gpk::SControlEvent		& controlEvent			= gui.Controls.Events[iControl];// Clear events that only last one tick.
		controlEvent			= {};

		const ::gpk::SControl		& control				= gui.Controls.Controls[iControl];
		if(false == ::gpk::controlInvalid(gui, control.Parent))
			continue;

		rootControlsToProcess.push_back(iControl);
	}

	gui.CursorPos			= {(float)input.MouseCurrent.Position.x, (float)input.MouseCurrent.Position.y}; //+= {(float)input.MouseCurrent.Deltas.x, (float)input.MouseCurrent.Deltas.y};
	for(uint32_t iControl = 0, countControls = rootControlsToProcess.size(); iControl < countControls; ++iControl) {
		if(::gpk::controlHidden(gui, rootControlsToProcess[iControl]))
			continue;
		::gpk::error_t				controlPressed		= ::controlProcessInput(gui, input, rootControlsToProcess[iControl]);
		if(gui.Controls.Controls.size() > (uint32_t)controlPressed)
			controlHovered			= controlPressed;
	}
	if(controlHovered == -1)
		return gui.Controls.Controls.size();

	for(uint32_t iControl = 0, countControls = gui.Controls.Controls.size(); iControl < countControls; ++iControl) {
		if(iControl != (uint32_t)controlHovered) {
			::gpk::SControlState		& controlState		= gui.Controls.States[iControl];
			controlState.Hovered		= false;
			if(0 == input.MouseCurrent.ButtonState[0])
				controlState.Pressed	= false;
		}
		//else {
		//	verbose_printf("Hovered: %u.", iControl);
		//}
	}
	return controlHovered;
}

::gpk::error_t			gpk::guiDeselect		(::gpk::SGUI & gui)	{
	for(uint32_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl) {
		gui.Controls.States[iControl].Selected	= false;
	}
	return 0;
}

::gpk::error_t			gpk::guiSelect			(::gpk::SGUI & gui, int32_t controlToSelect)	{
	for(uint32_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl) {
		gui.Controls.States[iControl].Selected	= controlToSelect == (int32_t)iControl;
	}
	return 0;
}

::gpk::error_t			gpk::controlHidden		(const ::gpk::SGUI & gui, int32_t iControl)	{
	bool						imHidden				= ::gpk::controlInvalid(gui, iControl) || gui.Controls.States[iControl].Hidden;
	return imHidden ? imHidden : (false == ::gpk::controlInvalid(gui, gui.Controls.Controls[iControl].Parent) && ::gpk::controlHidden(gui, gui.Controls.Controls[iControl].Parent));
}

::gpk::error_t			gpk::controlDisabled	(const ::gpk::SGUI & gui, int32_t iControl)	{
	bool						imDisabled				= ::gpk::controlInvalid(gui, iControl) || gui.Controls.States[iControl].Disabled;
	return imDisabled ? imDisabled : (false == ::gpk::controlInvalid(gui, gui.Controls.Controls[iControl].Parent) && ::gpk::controlDisabled(gui, gui.Controls.Controls[iControl].Parent));
}

::gpk::error_t			gpk::controlMetricsInvalidate	(::gpk::SGUI & gui, int32_t iControl)	{
	::gpk::SControlState		& controlState			= gui.Controls.States[iControl];
	controlState.Updated	= false;
	const ::gpk::vi32			& controlChildren		= gui.Controls.Children[iControl];
	for(uint32_t iChild = 0, countChild = controlChildren.size(); iChild < countChild; ++iChild)
		gpk_necall(::gpk::controlMetricsInvalidate(gui, controlChildren[iChild]), "%s", "Invalid child?");

	return 0;
}

::gpk::error_t			gpk::guiGetProcessableControls	(const ::gpk::SGUI & gui, ::gpk::au32& controlIndices)													{
	for(uint32_t iControl = 0, countControls = gui.Controls.Controls.size(); iControl < countControls; ++iControl) {	// Only process root parents
		const ::gpk::SControlState	& controlState					= gui.Controls.States[iControl];
		if(controlState.Unused || gui.Controls.Modes[iControl].NoExecute || ::gpk::controlDisabled(gui, iControl) || ::gpk::controlHidden(gui, iControl))
			continue;
		gpk_necs(controlIndices.push_back(iControl));
	}
	return controlIndices.size();
}

::gpk::error_t			gpk::controlTextSet		(::gpk::SGUI & gui, int32_t iControl, const ::gpk::vcs& text)				{
	::gpk::vcs					& oldText				= gui.Controls.Text[iControl].Text;
	if(text.begin	() != oldText.begin()
	 || text.size	() != oldText.size()
	 ) {
		oldText					= text;
		gui.Controls.States[iControl].Updated	= false;
	}
	return 0;
}

::gpk::error_t			gpk::controlFontSet		(::gpk::SGUI & gui, int32_t iControl, int16_t iFont)				{
	int16_t						& oldText				= gui.Controls.Text[iControl].FontSelected;
	if(iFont != oldText) {
		oldText					= iFont;
		gui.Controls.States[iControl].Updated	= false;
	}
	return 0;
}

::gpk::error_t			gpk::guiProcessControls		(const ::gpk::SGUI & gui, const ::std::function<::gpk::error_t(uint32_t iControl)> & funcOnExecute) {
	::gpk::au32					controlsToProcess			= {};
	gpk_necs(::gpk::guiGetProcessableControls(gui, controlsToProcess));
	return guiProcessControls(gui, controlsToProcess, funcOnExecute);
}

::gpk::error_t			gpk::guiProcessControls		(const ::gpk::SGUI & gui, ::gpk::vcu32 controlsToProcess, const ::std::function<::gpk::error_t(uint32_t iControl)> & funcOnExecute) {
	for(uint32_t iControl = 0, countControls = controlsToProcess.size(); iControl < countControls; ++iControl) {
		const uint32_t				idControl					= controlsToProcess[iControl];
		const ::gpk::SControlEvent	& controlState				= gui.Controls.Events[idControl];
		if(controlState.Execute) {
			info_printf("Executed control %i (0x%x).", idControl, idControl);
			return funcOnExecute(idControl);
		}
	}
	return 0;
}

