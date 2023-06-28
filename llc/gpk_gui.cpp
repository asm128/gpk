#include "gpk_gui.h"
#include "gpk_gui_text.h"
#include "gpk_base64.h"
#include "gpk_rect_align.h"
#include "gpk_apod_color.h"

::gpk::error_t			gpk::controlInvalid			(const ::gpk::SGUI & gui, cid_t iControl)				{
	if(gui.Controls.States.size() <= uint32_t(iControl)	) 
		return -1;
	if(gui.Controls.IsUnused(iControl)) 
		return -1;
	return 0;
}

static	::gpk::error_t	paletteSetupDefault			(::gpk::a8bgra & palette, const ::gpk::view<const ::gpk::bgra> & colors, uint32_t iShades)	{
	const uint32_t				newPaletteSize				= colors.size() * iShades;
	if(palette.size() < newPaletteSize)
		gpk_necs(palette.resize(newPaletteSize));

	for(uint32_t iTone = 0; iTone < colors.size(); ++iTone)
		palette[iTone * iShades]	= colors[iTone];

	for(uint32_t iShade = 0; iShade < iShades; ++iShade) {
		const ::gpk::bgra			& baseColor					= palette[iShade * iShades];
		::gpk::bgra					& paletteItem				= palette[iShade];
		paletteItem				= baseColor;
	}

	for(uint32_t iTone = 1; iTone < colors.size(); ++iTone)
		for(uint32_t iShade = 0; iShade < iShades; ++iShade) {
			if(0 == (iShade % iShades))
				continue;

			const int32_t				toneIndex					= iTone * iShades;
			const ::gpk::bgra			& baseColor					= colors[iTone];
			::gpk::bgra					& paletteItem				= palette[toneIndex + iShade];
			paletteItem				= ::gpk::rgbaf(baseColor) / (float)iShades * (iShades - (float)iShade);
			paletteItem.a			= 1;
			verbose_printf("Original color: {r: 0x%X, g: 0x%X, b: 0x%X}.", baseColor	.r, baseColor	.g, baseColor	.b);
			verbose_printf("Shaded color  : {r: 0x%X, g: 0x%X, b: 0x%X}.", paletteItem	.r, paletteItem	.g, paletteItem	.b);
		}

	return 0;
}

static	::gpk::error_t	themeSetupDefault						(const ::gpk::view<const ::gpk::bgra> & palette, int32_t iColor, ::gpk::SControlTheme & theme, uint32_t iShades)	{
	::gpk::astu32<::gpk::UI_CONTROL_AREA_COUNT>		& colorComboDisabled									= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_DISABLED	]	= {};
	::gpk::astu32<::gpk::UI_CONTROL_AREA_COUNT>		& colorComboPressed 									= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_PRESSED	]	= {};
	::gpk::astu32<::gpk::UI_CONTROL_AREA_COUNT>		& colorComboSelected									= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_SELECTED	]	= {};
	::gpk::astu32<::gpk::UI_CONTROL_AREA_COUNT>		& colorComboHover 										= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_HOVER	]	= {};
	::gpk::astu32<::gpk::UI_CONTROL_AREA_COUNT>		& colorComboNormal										= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_NORMAL	]	= {};

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
			theme.ColorCombos[iState][iArea]	= ::gpk::min((uint32_t)theme.ColorCombos[iState][iArea], palette.size() - 1);
		}
	return 0;
}

static	::gpk::error_t	themeSetupDefault	(const ::gpk::a8bgra & palette, ::gpk::apod<::gpk::SControlTheme>& themes, uint32_t iShades)	{
	for(uint32_t iColor = 0; iColor < palette.size(); ++iColor) {
		const int32_t				indexTheme			= themes.push_back({});
		::gpk::SControlTheme		& theme				= themes[indexTheme];
		::themeSetupDefault(palette, iColor, theme, iShades);
	}
	return 0;
}

static	::gpk::error_t	initDefaults		(::gpk::pobj<::gpk::a8bgra> & palette, ::gpk::pobj<::gpk::apod<::gpk::SControlTheme>> & controlThemes) {
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

static	::gpk::error_t	paletteSetupDefaultColors	(::gpk::pobj<::gpk::a8bgra> & palette, ::gpk::pobj<::gpk::apod<::gpk::SControlTheme>> & controlThemes)	{
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
	guiColors.DefaultColors[::gpk::GUI_CONTROL_PALETTE_ACTION				]	= (uint16_t)guiColors.Palettes.push_back({{::gpk::BLUE		, ::gpk::ORANGE	, ::gpk::YELLOW, ::gpk::MAGENTA, ::gpk::CYAN, {}, ::gpk::WHITE				,}});
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

::gpk::cid_t			gpk::controlCreateChild	(::gpk::SGUI & gui, cid_t iParent)						{
	::gpk::cid_t				idControl				= ::gpk::controlCreate(gui);
	::gpk::controlSetParent(gui, idControl, iParent);
	return idControl;
}

::gpk::cid_t			gpk::controlCreate		(::gpk::SGUI & gui)							{
	gpk_necs(::uiColorsSetupDefault(gui.Colors));

	if(0 == gui.Fonts.size())
		gpk_necs(::setupDefaultFontTexture(gui));

	if(gui.Fonts.size() <= gui.SelectedFont)
		gui.SelectedFont = uint16_t(gui.Fonts.size() - 1);

	return ::gpk::controlCreate(gui.Controls, gui.Colors->DefaultColors);
}

::gpk::error_t			gpk::controlDelete		(::gpk::SGUI & gui, cid_t iControl, bool recursive)			{
	gpk_necall(::gpk::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	info_printf("Deleting control id: %i.", iControl);
	if(recursive) { // Delete children first.
		::gpk::acid					children				= gui.Controls.Children[iControl];
		for(uint32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
			e_if(errored(::gpk::controlDelete(gui, children[iChild], true)), "Failed to delete control! Invalid control id? %i.", children[iChild]);
	}
	// Remove from parent list
	const ::gpk::cid_t			indexParent				= gui.Controls.States[iControl].Parent;
#if defined(GPK_DEBUG_ENABLED)
	uint32_t					childrenRemoved			= 0;
#endif
	if((uint32_t)indexParent < gui.Controls.States.size() && false == gui.Controls.IsUnused(indexParent)) {
		::gpk::acid					& children				= gui.Controls.Children[indexParent];
		for(uint32_t iChild = 0; iChild < children.size(); ++iChild)
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
	gerror_if(childrenRemoved > 1, "Parent should not reference a child control more than once. References found: %i.", childrenRemoved);
#endif
	gui.Controls.SetUnused(iControl, true);
	return 0;
}

::gpk::error_t			gpk::controlSetParent	(::gpk::SGUI & gui, cid_t iControl, cid_t iParent)	{
	gpk_necall(::gpk::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	const ::gpk::cid_t			oldParent				= gui.Controls.States[iControl].Parent;
	::gpk::SControlState		& control				= gui.Controls.States[iControl];
	if(control.Parent == iParent)	// Exit early if there is nothing to do here.
		return 0;

	control.Parent			= iParent;
	if(::gpk::controlInvalid(gui, iParent)) {
		ree_if(-1 != iParent, "Invalid parent control id: %u.", iParent);
		return 0;
	}

	if((uint32_t)oldParent < gui.Controls.States.size() && false == gui.Controls.IsUnused(oldParent)) {
		::gpk::acid					& children				= gui.Controls.Children[oldParent];
		for(int32_t iChild = 0; iChild < (int32_t)children.size(); ++iChild)
			if(children[iChild] == iControl) {
				gui.Controls.SetUpdated(oldParent, false);
				gpk_necall(children.remove(iChild), "Failed to remove child at index: %u.", iChild);
				break;
			}
	}

	// Set iControl to parent's children array.
	::gpk::acid					& children				= gui.Controls.Children[iParent];
	for(int32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
		if(children[iChild] == iControl)
			return 0;

	gui.Controls.SetUpdated(iParent , false);
	gui.Controls.SetUpdated(iControl, false);
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
	finalRects[::gpk::GUI_CONTROL_AREA_OLD_CLIENT			]	= controlMetrics.Client.Global;
	finalRects[::gpk::GUI_CONTROL_AREA_OLD_BORDER_LEFT		]	= {rectTotal.Offset , ::gpk::n2i16{(int16_t)control.Border.Left, rectTotal.Size.y}};
	finalRects[::gpk::GUI_CONTROL_AREA_OLD_BORDER_TOP		]	= {rectTotal.Offset , ::gpk::n2i16{rectTotal.Size.x, (int16_t)control.Border.Top}};
	finalRects[::gpk::GUI_CONTROL_AREA_OLD_BORDER_RIGHT		]	= {rectTotal.Offset + ::gpk::n2i16{int16_t(rectTotal.Size.x - control.Border.Right), 0} , ::gpk::n2i16{(int16_t)control.Border.Right, controlMetrics.Total.Global.Size.y}};
	finalRects[::gpk::GUI_CONTROL_AREA_OLD_BORDER_BOTTOM	]	= {rectTotal.Offset + ::gpk::n2i16{0, int16_t(rectTotal.Size.y - control.Border.Bottom)}, ::gpk::n2i16{rectTotal.Size.x, (int16_t)control.Border.Bottom}};
	//finalRects[::gpk::GUI_CONTROL_AREA_OLD_BACKGROUND		]	= rectTotal;
	finalRects[::gpk::GUI_CONTROL_AREA_OLD_CLIENT			].Offset.x	-= control.Margin.Left	;
	finalRects[::gpk::GUI_CONTROL_AREA_OLD_CLIENT			].Offset.y	-= control.Margin.Top	;
	finalRects[::gpk::GUI_CONTROL_AREA_OLD_CLIENT			].Size	.x	+= control.Margin.Left	+ control.Margin.Right	;
	finalRects[::gpk::GUI_CONTROL_AREA_OLD_CLIENT			].Size	.y	+= control.Margin.Top	+ control.Margin.Bottom	;

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

static	::gpk::error_t	controlUpdateMetrics	(::gpk::SGUI & gui, ::gpk::cid_t iControl, const ::gpk::n2<uint16_t> & _targetSize)					{
	gpk_necall(::gpk::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);

	::gpk::SControlState		& controlState			= gui.Controls.States[iControl];
	const ::gpk::SControlPlacement	& controlPlacement	= gui.Controls.Placement[iControl];
	::gpk::n2f64				scale					= gui.Zoom.DPI * gui.Zoom.ZoomLevel;
	if(fabs(1.0 - scale.x) < 0.001) scale.x = 1.0;
	if(fabs(1.0 - scale.y) < 0.001) scale.y = 1.0;
	::gpk::n2f64				scaledPosition			= {controlPlacement.Area.Offset	.x * scale.x, controlPlacement.Area.Offset	.y * scale.y};
	::gpk::n2f64				scaledSize				= {controlPlacement.Area.Size	.x * scale.x, controlPlacement.Area.Size	.y * scale.y};
	//::gpk::rect<double>	scaledBorders		= {(control.Border.Left * scale.x), (control.Border.Top * scale.y), (control.Border.Right * scale.x), (control.Border.Bottom * scale.y)};
	//::gpk::rect<double>	scaledMargins		= {(control.Margin.Left * scale.x), (control.Margin.Top * scale.y), (control.Margin.Right * scale.x), (control.Margin.Bottom * scale.y)};

	::gpk::SControlArea			& controlMetrics		= gui.Controls.Area[iControl];
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
	const bool					isValidParent			= 0 == ::gpk::controlInvalid(gui, controlState.Parent);
	const ::gpk::n2u16			targetSize				= isValidParent ? gui.Controls.Area[controlState.Parent].Client.Global.Size.u16() : _targetSize.u16();

	if(controlConstraints.AttachSizeToControl.x == iControl) { if(controlConstraints.DockToControl.Left	!= -1) {} else { scaledPosition.x = 0; } scaledSize.x = targetSize.x; } else if(false == ::gpk::controlInvalid(gui, controlConstraints.AttachSizeToControl.x)) { if(controlConstraints.DockToControl.Left != -1) {} else { scaledPosition.x = 0;} scaledSize.x = gui.Controls.Area[controlConstraints.AttachSizeToControl.x].Total.Global.Size.x; }
	if(controlConstraints.AttachSizeToControl.y == iControl) { if(controlConstraints.DockToControl.Top	!= -1) {} else { scaledPosition.y = 0; } scaledSize.y = targetSize.y; } else if(false == ::gpk::controlInvalid(gui, controlConstraints.AttachSizeToControl.y)) { if(controlConstraints.DockToControl.Top  != -1) {} else { scaledPosition.y = 0;} scaledSize.y = gui.Controls.Area[controlConstraints.AttachSizeToControl.y].Total.Global.Size.y; }

	controlMetrics.Client.Local	= {{(int16_t)ncSizesScaled.Left, (int16_t)ncSizesScaled.Top}, (scaledSize - ncTotalSizeScaled).i16()};
	::gpk::n2f64				minSizeScaled			= (controlConstraints.SizeMinMax.Min.f64().InPlaceScale(scale.x, scale.y));
	::gpk::n2f64				maxSizeScaled			= (controlConstraints.SizeMinMax.Max.f64().InPlaceScale(scale.x, scale.y));
	scaledSize.x			= ::gpk::max(minSizeScaled.x, ::gpk::min(maxSizeScaled.x, scaledSize.x));
	scaledSize.y			= ::gpk::max(minSizeScaled.y, ::gpk::min(maxSizeScaled.y, scaledSize.y));
	::gpk::realignRectangle(targetSize.u32(), ::gpk::rect2<int16_t>{scaledPosition.i16(), scaledSize.i16()}, controlMetrics.Total.Local, controlPlacement.Align);
	controlMetrics.Total .Global	= controlMetrics.Total	.Local;
	controlMetrics.Client.Global	= controlMetrics.Client	.Local;
	controlMetrics.Client.Global.Offset	+= controlMetrics.Total	.Local.Offset;
	if(isValidParent) {
		::gpk::SControlArea		& parentMetrics			= gui.Controls.Area[controlState.Parent];
		controlMetrics.Client.Global.Offset	+= parentMetrics.Client.Global.Offset;
		controlMetrics.Total .Global.Offset	+= parentMetrics.Client.Global.Offset;
	}

	const ::gpk::rectcid		& dockToControl			= controlConstraints.DockToControl;
	if(dockToControl.Right != -1) {
		gpk_necall(::gpk::controlInvalid(gui, dockToControl.Right), "Invalid control id: %i.", dockToControl.Right);
		::gpk::controlUpdateMetricsTopToDown(gui, dockToControl.Right, targetSize.u16(), true);
		const ::gpk::SControlPlacement	& other				= gui.Controls.Placement[dockToControl.Right];
		const ::gpk::SControlArea		& otherMetrics		= gui.Controls.Area	[dockToControl.Right];
		if(::gpk::bit_true(other.Align, ::gpk::ALIGN_RIGHT) && ::gpk::bit_false(other.Align, ::gpk::ALIGN_HCENTER)) {
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
	if(dockToControl.Bottom != -1) {
		gpk_necall(::gpk::controlInvalid(gui, dockToControl.Bottom), "Invalid control id: %i.", dockToControl.Bottom);
		const ::gpk::SControlPlacement	& other					= gui.Controls.Placement[dockToControl.Bottom];
		const ::gpk::SControlArea		& otherMetrics			= gui.Controls.Area[dockToControl.Bottom	];
		if(::gpk::bit_true(other.Align, ::gpk::ALIGN_BOTTOM	) && ::gpk::bit_false(other.Align, ::gpk::ALIGN_VCENTER)) {
			controlMetrics.Total.Global.Offset.y	= otherMetrics.Total.Global.Offset.y - controlMetrics.Total.Global.Size.y;
			controlMetrics.Client.Global.Offset.y	= (int16_t)(controlMetrics.Total.Global.Offset.y + ncSizesScaled.Top);
		} else {
			controlMetrics.Total.Global.Offset.y	= otherMetrics.Total.Global.Offset.y + otherMetrics.Total.Global.Size.y;
			controlMetrics.Client.Global.Offset.y	= (int16_t)(controlMetrics.Total.Global.Offset.y + ncSizesScaled.Top);
		}
	}
	if(dockToControl.Left != -1) {
		gpk_necall(::gpk::controlInvalid(gui, dockToControl.Left), "Invalid control id: %i.", dockToControl.Left);
		::gpk::controlUpdateMetricsTopToDown(gui, dockToControl.Left, targetSize, true);
		const ::gpk::SControlPlacement	& other			= gui.Controls.Placement[dockToControl.Left];
		const ::gpk::SControlArea		& otherMetrics	= gui.Controls.Area	[dockToControl.Left];
		if(::gpk::bit_true(other.Align, ::gpk::ALIGN_RIGHT) && ::gpk::bit_false(other.Align, ::gpk::ALIGN_HCENTER)) {
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
		const ::gpk::SControlArea		& otherMetrics			= gui.Controls.Area	[dockToControl.Top ];
		if(::gpk::bit_true(other.Align, ::gpk::ALIGN_BOTTOM	) && ::gpk::bit_false(other.Align, ::gpk::ALIGN_VCENTER)) {
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
	gui.Controls.SetUpdated(iControl, true);
	
	return 0;
}

::gpk::error_t			gpk::controlUpdateMetricsTopToDown							(::gpk::SGUI & gui, ::gpk::cid_t iControl, const ::gpk::n2<uint16_t> & targetSize, bool forceUpdate)				{
	if(false == gui.Controls.IsUpdated(iControl) || forceUpdate)
		gpk_necall(::controlUpdateMetrics(gui, iControl, targetSize), "%s", "Unknown error! Maybe the control tree got broken?");
	::gpk::vcid					& children												= gui.Controls.Children[iControl];
	for(uint32_t iChild = 0; iChild < children.size(); ++iChild)
		gpk_necall(::gpk::controlUpdateMetricsTopToDown(gui, children[iChild], targetSize, forceUpdate), "%s", "Unknown error! Maybe the control tree got broken?");
	return 0;
}

::gpk::error_t			gpk::guiUpdateMetrics									(::gpk::SGUI & gui, const ::gpk::n2<uint16_t> & targetSize, bool forceUpdate)								{
	for(uint32_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl)
		if(::gpk::controlInvalid(gui, gui.Controls.States[iControl].Parent) && false == ::gpk::controlInvalid(gui, (::gpk::cid_t)iControl))
			gpk_necall(::gpk::controlUpdateMetricsTopToDown(gui, (::gpk::cid_t)iControl, targetSize, forceUpdate), "%s", "Unknown error! Maybe the control tree got broken?");
	return 0;
}

static	::gpk::cid_t	updateGUIControlHovered	(::gpk::SGUI & gui, ::gpk::cid_t iControl, ::gpk::SControlState & controlState, const ::gpk::SInput & inputSystem, bool disabled)	noexcept	{
	if(controlState.IsHovered()) {
		if(inputSystem.ButtonDown(0) && false == controlState.IsPressed())
			gui.Controls.SetPressed(iControl, true);
		else if(inputSystem.ButtonUp(0))
			gui.Controls.SetPressed(iControl, false);
	}
	else {
		if(false == disabled) {
			gui.Controls.SetHovered(iControl, true);//controlFlags.Disabled;
		}
	}
	return controlState.IsHovered() ? iControl : -1;
}

static	::gpk::cid_t	controlProcessInput		(::gpk::SGUI & gui, const ::gpk::SInput & input, ::gpk::cid_t iControl)														{
	//--------------------
	::gpk::SControlState		& controlState			= gui.Controls.States[iControl];
	::gpk::cid_t				controlHovered			= -1;
	{
		::gpk::vcid					& children				= gui.Controls.Children[iControl];
		for(uint32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild) {
			if(::gpk::controlHidden(gui, children[iChild]))
				continue;
			::gpk::cid_t				controlPressed			= ::controlProcessInput(gui, input, children[iChild]);
			if(gui.Controls.States.size() > (uint32_t)controlPressed) {
				//controlState.Hover		= false;
				gui.Controls.SetPressed(iControl, false);
				controlHovered			= controlPressed;
			}
		}
	}
	if(controlHovered == -1 && ::gpk::in_range(gui.CursorPos.i16(), gui.Controls.Area[iControl].Total.Global))
		controlHovered		= ::updateGUIControlHovered(gui, iControl, controlState, input, ::gpk::controlDisabled(gui, iControl) || ::gpk::controlHidden(gui, iControl));
	else {
		gui.Controls.SetHovered(iControl, false);

		if(input.ButtonUp(0)) 
			gui.Controls.SetPressed(iControl, false);
	}
	return controlHovered;
}

::gpk::error_t			gpk::guiProcessInput	(::gpk::SGUI & gui, const ::gpk::SInput & input, ::gpk::vpobj<::gpk::SSystemEvent> sysEvents)	{
	if(0 == gui.LastSize.LengthSquared()) 
		return 0;

	gui.Controls.EventQueue.clear();
	es_if(errored(::gpk::guiUpdateMetrics(gui, gui.LastSize, false)));
	::gpk::cid_t				controlHovered			= -1;
	::gpk::acid					rootControlsToProcess	= {};
	rootControlsToProcess.reserve(1000);
	(void)sysEvents;
	for(uint32_t iControl = 0, countControls = gui.Controls.States.size(); iControl < countControls; ++iControl) {	// Only process root parents
		const ::gpk::SControlState	& controlState			= gui.Controls.States[iControl];
		if(controlState.IsUnused())
			continue;

		gui.Controls.SetAction((::gpk::cid_t)iControl, false);

		if(controlState.IsDisabled())
			continue;

		if(::gpk::controlHidden(gui, (::gpk::cid_t)iControl))
			continue;

		if(false == ::gpk::controlInvalid(gui, controlState.Parent))
			continue;

		rootControlsToProcess.push_back((::gpk::cid_t)iControl);
	}

	gui.CursorPos			= {(float)input.MouseCurrent.Position.x, (float)input.MouseCurrent.Position.y}; //+= {(float)input.MouseCurrent.Deltas.x, (float)input.MouseCurrent.Deltas.y};
	for(uint32_t iControl = 0, countControls = rootControlsToProcess.size(); iControl < countControls; ++iControl) {
		if(::gpk::controlHidden(gui, rootControlsToProcess[iControl]))
			continue;
		::gpk::cid_t				controlPressed		= ::controlProcessInput(gui, input, rootControlsToProcess[iControl]);
		if(gui.Controls.States.size() > (uint32_t)controlPressed)
			controlHovered			= controlPressed;
	}
	for(uint32_t iControl = 0, countControls = gui.Controls.States.size(); iControl < countControls; ++iControl) {
		if(iControl != (uint32_t)controlHovered) {
			gui.Controls.SetHovered((::gpk::cid_t)iControl, false);;
			if(0 == input.MouseCurrent.ButtonState[0])
				gui.Controls.SetPressed((::gpk::cid_t)iControl, false);
		}
	}

	return controlHovered;
}

::gpk::error_t			gpk::guiDeselect		(::gpk::SGUI & gui)	{
	for(uint32_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl)
		gui.Controls.SetSelected((::gpk::cid_t)iControl, false);

	return 0;
}

::gpk::error_t			gpk::guiSelect			(::gpk::SGUI & gui, ::gpk::cid_t controlToSelect)	{
	for(uint32_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl) 
		gui.Controls.SetSelected((::gpk::cid_t)iControl, controlToSelect == (::gpk::cid_t)iControl);

	return 0;
}

::gpk::error_t			gpk::controlHidden		(const ::gpk::SGUI & gui, ::gpk::cid_t iControl)	{
	bool						imHidden				= ::gpk::controlInvalid(gui, iControl) || gui.Controls.States[iControl].IsHidden();
	return imHidden ? imHidden : (0 == ::gpk::controlInvalid(gui, gui.Controls.States[iControl].Parent) && ::gpk::controlHidden(gui, gui.Controls.States[iControl].Parent));
}

::gpk::error_t			gpk::controlDisabled	(const ::gpk::SGUI & gui, ::gpk::cid_t iControl)	{
	bool						imDisabled				= ::gpk::controlInvalid(gui, iControl) || gui.Controls.States[iControl].IsDisabled();
	return imDisabled ? imDisabled : (false == ::gpk::controlInvalid(gui, gui.Controls.States[iControl].Parent) && ::gpk::controlDisabled(gui, gui.Controls.States[iControl].Parent));
}

::gpk::error_t			gpk::controlMetricsInvalidate	(::gpk::SGUI & gui, ::gpk::cid_t iControl)	{
	gui.Controls.SetUpdated(iControl, false);
	const ::gpk::vcid			& controlChildren		= gui.Controls.Children[iControl];
	for(uint32_t iChild = 0, countChild = controlChildren.size(); iChild < countChild; ++iChild)
		gpk_necall(::gpk::controlMetricsInvalidate(gui, controlChildren[iChild]), "%s", "Invalid child?");

	return 0;
}

::gpk::error_t			gpk::guiGetProcessableControls	(const ::gpk::SGUI & gui, ::gpk::acid & controlIndices)													{
	for(uint32_t iControl = 0, countControls = gui.Controls.States.size(); iControl < countControls; ++iControl) {	// Only process root parents
		const ::gpk::SControlState	& controlState					= gui.Controls.States[iControl];
		if(controlState.IsUnused() || ::gpk::controlDisabled(gui, (::gpk::cid_t)iControl) || ::gpk::controlHidden(gui, (::gpk::cid_t)iControl))
			continue;
		gpk_necs(controlIndices.push_back((::gpk::cid_t)iControl));
	}
	return controlIndices.size();
}

::gpk::error_t			gpk::guiProcessControls		(const ::gpk::SGUI & gui, const ::std::function<::gpk::error_t(::gpk::cid_t iControl)> & funcOnExecute) {
	::gpk::acid					controlsToProcess			= {};
	gpk_necs(::gpk::guiGetProcessableControls(gui, controlsToProcess));
	return guiProcessControls(gui, controlsToProcess, funcOnExecute);
}

::gpk::error_t			gpk::guiProcessControls		(const ::gpk::SGUI & gui, ::gpk::vcid controlsToProcess, const ::std::function<::gpk::error_t(::gpk::cid_t iControl)> & funcOnExecute) {
	for(uint32_t iControl = 0, countControls = controlsToProcess.size(); iControl < countControls; ++iControl) {
		const ::gpk::cid_t			idControl					= controlsToProcess[iControl];
		const ::gpk::SControlState	& controlState				= gui.Controls.States[idControl];
		if(controlState.IsAction()) {
			info_printf("Executed control %i (0x%x).", idControl, idControl);
			return funcOnExecute(idControl);
		}
	}
	return 0;
}

::gpk::error_t			gpk::controlTextSet		(::gpk::SGUI & gui, ::gpk::cid_t iControl, const ::gpk::vcs & text)				{
	::gpk::vcs					& oldText				= gui.Controls.Text[iControl].Text;
	if(text.begin	() != oldText.begin()
	 || text.size	() != oldText.size()
	 ) {
		oldText					= text;
		gui.Controls.SetUpdated(iControl, false);
	}
	return 0;
}

::gpk::error_t			gpk::controlFontSet		(::gpk::SGUI & gui, ::gpk::cid_t iControl, int16_t iFont)				{
	int16_t						& oldText				= gui.Controls.Text[iControl].FontSelected;
	if(iFont != oldText) {
		oldText					= iFont;
		gui.Controls.SetUpdated(iControl, false);
	}
	return 0;
}

static	::gpk::error_t	controlInstanceReset	(::gpk::SControlTable & controlTable, ::gpk::cid_t iControl, ::gpk::vi16 defaultColors)										{
	controlTable.States		[iControl]	= {};
	controlTable.Placement	[iControl]	= {};
	controlTable.Area		[iControl]	= {};
	controlTable.Constraints[iControl]	= {};
	controlTable.Draw		[iControl]	= {};
	controlTable.Text		[iControl]	= {};
	controlTable.Children	[iControl]	= ::gpk::vcid{};
	controlTable.Images		[iControl]	= {};
	
	::gpk::SControlDraw			& controlDraw			= controlTable.Draw[iControl];
	for(uint32_t iColor = 0, countColors = ::gpk::min(controlDraw.Palettes.size(), defaultColors.size()); iColor < countColors; ++iColor)
		controlDraw.Palettes[iColor] = defaultColors[iColor];

	return 0;
}


::gpk::cid_t			gpk::controlCreate		(::gpk::SControlTable & controlTable, ::gpk::vi16 defaultColors)	{
	for(uint32_t iControl = 0; iControl < controlTable.States.size(); ++iControl) {
		if(controlTable.IsUnused((::gpk::cid_t)iControl)) {
			gpk_necall(::controlInstanceReset(controlTable, (::gpk::cid_t)iControl, defaultColors), "iControl: %i", iControl);
			return (::gpk::cid_t)iControl;
		}
	}

	::gpk::cid_t				iControl				= -1;
	gpk_necs(iControl = (::gpk::cid_t)::gpk::resize(controlTable.States.size() + 1
		, controlTable.States
		, controlTable.Placement
		, controlTable.Area
		, controlTable.Constraints
		, controlTable.Draw
		, controlTable.Text
		, controlTable.Images
		, controlTable.Children
		) - 1);
	gpk_necs(::controlInstanceReset(controlTable, iControl, defaultColors));
	return iControl;
}
