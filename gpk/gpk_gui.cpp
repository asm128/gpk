#include "gpk_gui.h"
#include "gpk_gui_text.h"


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

																gpk::SGUI::SGUI												()	{
	DefaultColors[::gpk::GUI_CONTROL_PALETTE_NORMAL				]	= Palettes.push_back({{::gpk::BLUE		, ::gpk::GRAY	, ::gpk::YELLOW, ::gpk::MAGENTA, ::gpk::CYAN, {}, ::gpk::WHITE				,}});
	DefaultColors[::gpk::GUI_CONTROL_PALETTE_DISABLED			]	= Palettes.push_back({{::gpk::LIGHTGRAY	, ::gpk::ORANGE	, ::gpk::YELLOW, ::gpk::MAGENTA, ::gpk::CYAN, {}, ::gpk::LIGHTGRAY * 1.1	,}});
	DefaultColors[::gpk::GUI_CONTROL_PALETTE_HOVER				]	= Palettes.push_back({{::gpk::CYAN		, ::gpk::YELLOW	, ::gpk::MAGENTA, ::gpk::GREEN, ::gpk::ORANGE, {}, ::gpk::DARKCYAN / 2.0	,}});
	DefaultColors[::gpk::GUI_CONTROL_PALETTE_PRESSED			]	= Palettes.push_back({{::gpk::RED		, ::gpk::ORANGE	, ::gpk::YELLOW, ::gpk::MAGENTA, ::gpk::CYAN, {}, ::gpk::WHITE				,}});
	DefaultColors[::gpk::GUI_CONTROL_PALETTE_SELECTED			]	= Palettes.push_back({{::gpk::ORANGE	, ::gpk::ORANGE	, ::gpk::YELLOW, ::gpk::MAGENTA, ::gpk::CYAN, {}, ::gpk::WHITE				,}});
	DefaultColors[::gpk::GUI_CONTROL_PALETTE_SELECTED_DISABLED	]	= Palettes.push_back({{::gpk::BLUE		, ::gpk::ORANGE	, ::gpk::YELLOW, ::gpk::MAGENTA, ::gpk::CYAN, {}, ::gpk::WHITE				,}});
	DefaultColors[::gpk::GUI_CONTROL_PALETTE_SELECTED_HOVER		]	= Palettes.push_back({{::gpk::BLUE		, ::gpk::ORANGE	, ::gpk::YELLOW, ::gpk::MAGENTA, ::gpk::CYAN, {}, ::gpk::WHITE				,}});
	DefaultColors[::gpk::GUI_CONTROL_PALETTE_SELECTED_PRESSED	]	= Palettes.push_back({{::gpk::BLUE		, ::gpk::ORANGE	, ::gpk::YELLOW, ::gpk::MAGENTA, ::gpk::CYAN, {}, ::gpk::WHITE				,}});
	DefaultColors[::gpk::GUI_CONTROL_PALETTE_EXECUTE			]	= Palettes.push_back({{::gpk::BLUE		, ::gpk::ORANGE	, ::gpk::YELLOW, ::gpk::MAGENTA, ::gpk::CYAN, {}, ::gpk::WHITE				,}});
	DefaultColors[::gpk::GUI_CONTROL_PALETTE_OUTDATED			]	= Palettes.push_back({{::gpk::BLUE		, ::gpk::ORANGE	, ::gpk::YELLOW, ::gpk::MAGENTA, ::gpk::CYAN, {}, ::gpk::WHITE				,}});
}

			::gpk::error_t										gpk::controlInvalid										(const ::gpk::SGUI& gui, int32_t iControl)				{
	if(gui.Controls.Controls.size() <= uint32_t(iControl)	) return -1;
	if(gui.Controls.States[iControl].Unused					) return -1;
	return 0;
}

static		::gpk::error_t										paletteSetupDefault										(::gpk::array_pod<::gpk::SColorBGRA>& palette, const ::gpk::view_array<const ::gpk::SColorBGRA>& colors, uint32_t iShades)	{
	const uint32_t														newPaletteSize											= colors.size() * iShades;
	if(palette.size() < newPaletteSize)
		gpk_necall(palette.resize(newPaletteSize), "%s", "Out of memory?");
	for(uint32_t iTone = 0; iTone < colors.size(); ++iTone)
		palette[iTone * iShades]										= colors[iTone];

	for(uint32_t iShade = 0; iShade < iShades; ++iShade) {
		const ::gpk::SColorBGRA												& baseColor												= palette[iShade * iShades];
		::gpk::SColorBGRA													& paletteItem											= palette[iShade];
		paletteItem														= baseColor;
	}

	for(uint32_t iTone = 1; iTone < colors.size(); ++iTone)
		for(uint32_t iShade = 0; iShade < iShades; ++iShade) {
			if(0 == (iShade % iShades))
				continue;
			const int32_t														toneIndex												= iTone * iShades;
			const ::gpk::SColorBGRA												& baseColor												= colors[iTone];
			::gpk::SColorBGRA													& paletteItem											= palette[toneIndex + iShade];
			paletteItem														= ::gpk::SColorFloat(baseColor) / (float)iShades * (iShades - (float)iShade);
			verbose_printf("Original color: {r: 0x%X, g: 0x%X, b: 0x%X}.", baseColor	.r, baseColor	.g, baseColor	.b);
			verbose_printf("Shaded color  : {r: 0x%X, g: 0x%X, b: 0x%X}.", paletteItem	.r, paletteItem	.g, paletteItem	.b);
		}

	return 0;
}

static		::gpk::error_t										themeSetupDefault										(const ::gpk::array_pod<::gpk::SColorBGRA>& palette, int32_t iColor, ::gpk::SControlTheme& theme, uint32_t iShades)	{
	::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboDisabled									= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_DISABLED	]	= {};
	::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboPressed 									= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_PRESSED	]	= {};
	::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboSelected									= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_SELECTED	]	= {};
	::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboHover 										= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_HOVER	]	= {};
	::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboNormal										= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_NORMAL	]	= {};

	//const int32_t														colorBase												= iColor / iShades;
	const int32_t														colorShade												= iColor % iShades;
	const int32_t														colorText												= (::gpk::ASCII_COLOR_WHITE * iShades);

	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]	= iColor;//(colorShade > 7) ? iColor : iColor;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT	]	= (colorShade > 7) ? iColor - 1 : iColor + 1;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP	]	= (colorShade > 7) ? iColor - 1 : iColor + 1;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT	]	= (colorShade > 7) ? iColor - 1 : iColor + 1;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM	]	= (colorShade > 7) ? iColor - 1 : iColor + 1;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE		]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
//
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]	= iColor;//(colorShade > 7) ? iColor : iColor;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT	]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP	]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT	]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM	]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE		]	= (colorShade > 7) ? colorText + 7 : colorText + 9;
//
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]	= iColor;//(colorShade > 7) ? iColor : iColor;
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT	]	= iColor;//(colorShade > 7) ? iColor : iColor;
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP	]	= iColor;//(colorShade > 7) ? iColor : iColor;
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT	]	= iColor;//(colorShade > 7) ? iColor : iColor;
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM	]	= iColor;//(colorShade > 7) ? iColor : iColor;
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE		]	= (colorShade > 7) ? colorText : colorText + 8;
//
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]	= iColor;//(colorShade > 7) ? iColor : iColor;
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT	]	= (colorShade > 7) ? iColor - 7: iColor + 7;
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP	]	= (colorShade > 7) ? iColor - 7: iColor + 7;
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT	]	= (colorShade > 7) ? iColor - 7: iColor + 7;
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM	]	= (colorShade > 7) ? iColor - 7: iColor + 7;
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE		]	= (colorShade > 7) ? colorText + 4: colorText + 12;
//
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]	= iColor;//(colorShade > 7) ? iColor : iColor;
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT	]	= (colorShade > 7) ? iColor - 5: iColor + 5;
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP	]	= (colorShade > 7) ? iColor - 5: iColor + 5;
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT	]	= (colorShade > 7) ? iColor - 5: iColor + 5;
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM	]	= (colorShade > 7) ? iColor - 5: iColor + 5;
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE		]	= (colorShade > 7) ? colorText : colorText + 15;

	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND	]	= colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_CLIENT			]	= colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND	]	= colorComboPressed		[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_CLIENT			]	= colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND	]	= colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
	colorComboSelected	[::gpk::GUI_CONTROL_COLOR_CLIENT			]	= colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND	]	= colorComboHover		[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
	colorComboHover 	[::gpk::GUI_CONTROL_COLOR_CLIENT			]	= colorComboHover 		[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND	]	= colorComboNormal		[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
	colorComboNormal	[::gpk::GUI_CONTROL_COLOR_CLIENT			]	= colorComboNormal		[::gpk::GUI_CONTROL_COLOR_BACKGROUND];

	for(uint32_t iState = 0; iState < theme.ColorCombos.size(); ++iState)
		for(uint32_t iArea = 0; iArea < ::gpk::GUI_CONTROL_COLOR_COUNT; ++iArea)
			theme.ColorCombos[iState][iArea]							= ::gpk::min((uint32_t)theme.ColorCombos[iState][iArea], palette.size() - 1);
	return 0;
}

static		::gpk::error_t										themeSetupDefault										(const ::gpk::array_pod<::gpk::SColorBGRA>& palette, ::gpk::array_pod<::gpk::SControlTheme>& themes, uint32_t iShades)	{
	for(uint32_t iColor = 0; iColor < palette.size(); ++iColor) {
		const int32_t														indexTheme												= themes.push_back({});
		::gpk::SControlTheme												& theme													= themes[indexTheme];
		::themeSetupDefault(palette, iColor, theme, iShades);
	}
	return 0;
}

static		::gpk::error_t										controlInstanceReset									(::gpk::SGUI& gui, int32_t iControl)										{
	::gpk::SGUIControlTable												& controlTable											= gui.Controls;
	controlTable.Metrics	[iControl]								= {};
	controlTable.Children	[iControl]								= ::gpk::view_array<int32_t>{};
	controlTable.Modes		[iControl]								= {};
	controlTable.States		[iControl]								= {};
	controlTable.Images		[iControl]								= {};
	//controlTable.Modes		[iControl].UseNewPalettes				= 1;
	::gpk::SControl														& control				= controlTable.Controls		[iControl]	= {};
	::gpk::SControlConstraints											& controlConstraints	= controlTable.Constraints	[iControl]	= {};
	::gpk::SControlText													& controlText			= controlTable.Text			[iControl]	= {};
	control.IndexParent														= -1;
	control.Align															= ::gpk::ALIGN_TOP_LEFT;
	control.Area															= {{0, 0}, {16, 16}};
	::gpk::memcpy_s(control.Palettes, gui.DefaultColors);
	controlConstraints.AttachSizeToControl									= {-1, -1};
	controlConstraints.AttachSizeToText										= {};
	controlConstraints.DockToControl										= {-1, -1, -1, -1};
	controlText.Align														= ::gpk::ALIGN_CENTER;
	return 0;
}

			::gpk::error_t										gpk::controlCreateChild									(::gpk::SGUI& gui, int32_t iParent)						{
	int32_t																idControl												= ::gpk::controlCreate(gui);
	::gpk::controlSetParent(gui, idControl, iParent);
	return idControl;
}
			::gpk::error_t										gpk::controlCreate										(::gpk::SGUI& gui)										{
	static constexpr	const uint32_t									iShades													= 16;
	if(0 == gui.Palette.size()) {
		static constexpr	const SColorBGRA								paletteColors []										=
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
			, ::gpk::SColorBGRA{::gpk::ASCII_COLOR_INDEX_14	+ ::gpk::ASCII_COLOR_INDEX_1}
			, ::gpk::SColorBGRA{::gpk::ASCII_COLOR_INDEX_13	+ ::gpk::ASCII_COLOR_INDEX_2}
			, ::gpk::SColorBGRA{::gpk::ASCII_COLOR_INDEX_12	+ ::gpk::ASCII_COLOR_INDEX_3}
			, ::gpk::SColorBGRA{::gpk::ASCII_COLOR_INDEX_11	+ ::gpk::ASCII_COLOR_INDEX_4}
			, ::gpk::SColorBGRA{::gpk::ASCII_COLOR_INDEX_10	+ ::gpk::ASCII_COLOR_INDEX_5}
			, ::gpk::SColorBGRA{::gpk::ASCII_COLOR_INDEX_9 	+ ::gpk::ASCII_COLOR_INDEX_6}
			, ::gpk::SColorBGRA{::gpk::ASCII_COLOR_INDEX_8 	+ ::gpk::ASCII_COLOR_INDEX_7}
			, ::gpk::SColorBGRA{0xFFFFFFFFU & (::gpk::ASCII_COLOR_INDEX_4 * (uint64_t)::gpk::ASCII_COLOR_INDEX_11)}
			, ::gpk::SColorBGRA{0xFFFFFFFFU & (::gpk::ASCII_COLOR_INDEX_5 * (uint64_t)::gpk::ASCII_COLOR_INDEX_10)}
			, ::gpk::SColorBGRA{0xFFFFFFFFU & (::gpk::ASCII_COLOR_INDEX_7 * (uint64_t)::gpk::ASCII_COLOR_INDEX_8 )}
			, ::gpk::SColorBGRA{(uint32_t)(::gpk::ASCII_COLOR_INDEX_1 + ::gpk::ASCII_COLOR_INDEX_4)}
			, ::gpk::SColorBGRA{(uint32_t)(::gpk::ASCII_COLOR_INDEX_3 + ::gpk::ASCII_COLOR_INDEX_7)}
			, ::gpk::SColorBGRA{(uint32_t)(::gpk::ASCII_COLOR_INDEX_1 + ::gpk::ASCII_COLOR_INDEX_8)}
			, ::gpk::SColorBGRA{(uint32_t)(::gpk::ASCII_COLOR_INDEX_4 + ::gpk::ASCII_COLOR_INDEX_7)}

			, ::gpk::SColorBGRA{::gpk::BROWN}
			, ::gpk::SColorBGRA{::gpk::LIGHTORANGE}
		};
		gpk_necall(::paletteSetupDefault(gui.Palette, paletteColors, iShades), "%s", "Unknown issue!");
	}
	if(0 == gui.ControlThemes.size())
		gpk_necall(::themeSetupDefault(gui.Palette, gui.ControlThemes, iShades), "%s", "Unknown issue! Maybe the palette system got broken?");

	for(uint32_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl) {
		if(gui.Controls.States[iControl].Unused) {
			gpk_necall(::controlInstanceReset(gui, iControl), "%s", "Unknown issue!");
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
		, gui.Controls.Images
		) - 1, "%s", "Failed to resize! Out of memory?");
	gpk_necall(::controlInstanceReset(gui, iControl), "%s", "Unknown issue!");
	return iControl;
}
			::gpk::error_t										gpk::controlDelete										(::gpk::SGUI & gui, int32_t iControl, bool recursive)			{
	gpk_necall(::gpk::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	info_printf("Deleting control id: %i.", iControl);
	if(recursive) { // Delete children first.
		::gpk::array_pod<int32_t>											children												= gui.Controls.Children[iControl];
		for(uint32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
			gerror_if(errored(::gpk::controlDelete(gui, children[iChild], true)), "Failed to delete control! Invalid control id? %i.", children[iChild]);
	}
	// Remove from parent list
	const uint32_t														indexParent												= (uint32_t)gui.Controls.Controls[iControl].IndexParent;
#if defined(GPK_DEBUG_ENABLED)
	int32_t																childrenRemoved											= 0;
#endif
	if(indexParent < gui.Controls.Controls.size() && false == gui.Controls.States[indexParent].Unused) {
		::gpk::array_pod<int32_t>											& children												= gui.Controls.Children[indexParent];
		for(int32_t iChild = 0; iChild < (int32_t)children.size(); ++iChild)
			if(children[iChild] == iControl) {
				gpk_necall(children.remove(iChild), "Failed to remove child at index: %u.", iChild);
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
	::gpk::SControlState												& controlState											= gui.Controls.States[iControl];
	controlState.Unused												= true;
	return 0;
}

			::gpk::error_t										gpk::controlSetParent									(::gpk::SGUI& gui, int32_t iControl, int32_t iParent)	{
	gpk_necall(::gpk::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	const uint32_t														oldParent												= gui.Controls.Controls[iControl].IndexParent;
	::gpk::SControl														& control												= gui.Controls.Controls[iControl];
	if(control.IndexParent == iParent)	// Exit early if there is nothing to do here.
		return 0;

	control.IndexParent												= iParent;
	if(::gpk::controlInvalid(gui, iParent)) {
		ree_if(-1 != iParent, "Invalid parent control id: %u.", iParent);
		return 0;
	}

	if(oldParent < gui.Controls.Controls.size() && false == gui.Controls.States[oldParent].Unused) {
		::gpk::array_pod<int32_t>											& children												= gui.Controls.Children[oldParent];
		for(int32_t iChild = 0; iChild < (int32_t)children.size(); ++iChild)
			if(children[iChild] == iControl) {
				gpk_necall(children.remove(iChild), "Failed to remove child at index: %u.", iChild);
				break;
			}
	}

	// Set iControl to parent's children array.
	::gpk::array_pod<int32_t>											& children												= gui.Controls.Children[iParent];
	for(int32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
		if(children[iChild] == iControl)
			return 0;

	gpk_necall(children.push_back(iControl), "%s", "Out of memory?");
	return 0;
}

static						::gpk::error_t						controlUpdateMetrics									(::gpk::SGUI& gui, int32_t iControl, const ::gpk::SCoord2<uint32_t> & _targetSize)					{
	gpk_necall(::gpk::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	::gpk::SControlState												& controlState											= gui.Controls.States[iControl];
	const ::gpk::SControl												& control												= gui.Controls.Controls[iControl];
	::gpk::SCoord2<double>												scale													= gui.Zoom.DPI * gui.Zoom.ZoomLevel;
	if(fabs(1.0 - scale.x) < 0.001) scale.x = 1.0;
	if(fabs(1.0 - scale.y) < 0.001) scale.y = 1.0;
	::gpk::SCoord2<double>												scaledPosition											= {control.Area.Offset	.x * scale.x, control.Area.Offset	.y * scale.y};
	::gpk::SCoord2<double>												scaledSize												= {control.Area.Size	.x * scale.x, control.Area.Size		.y * scale.y};
	//::gpk::SRectLimits<double>											scaledBorders											= {(control.Border.Left * scale.x), (control.Border.Top * scale.y), (control.Border.Right * scale.x), (control.Border.Bottom * scale.y)};
	//::gpk::SRectLimits<double>											scaledMargins											= {(control.Margin.Left * scale.x), (control.Margin.Top * scale.y), (control.Margin.Right * scale.x), (control.Margin.Bottom * scale.y)};

	::gpk::SControlMetrics												& controlMetrics										= gui.Controls.Metrics[iControl];
	::gpk::SRectangle2D<double>											rectText												= {};
	::gpk::SControlText													& controlText											= gui.Controls.Text[iControl];
	rectText.Size													= {(double)(gui.FontCharSize.x * controlText.Text.size()), (double)gui.FontCharSize.y};
	rectText.Size.InPlaceScale(scale.x, scale.y);

	const ::gpk::SControlConstraints									& controlConstraints									= gui.Controls.Constraints[iControl];
	const ::gpk::SRectLimits<int16_t>									ncSizes													= ::gpk::controlNCRect(control);
	const ::gpk::SRectLimits<double>									ncSizesScaled											= {(ncSizes.Left * scale.x), (ncSizes.Top * scale.y), (ncSizes.Right * scale.x), (ncSizes.Bottom * scale.y)};
	const ::gpk::SCoord2<double>										ncTotalSize												= {(double)ncSizes.Left + ncSizes.Right, (double)ncSizes.Top + ncSizes.Bottom};
	const ::gpk::SCoord2<double>										ncTotalSizeScaled										= ncTotalSize.GetScaled(scale.x, scale.y);
	if(controlConstraints.AttachSizeToText.x) scaledSize.x				= rectText.Size.x + ncTotalSizeScaled.x;
	if(controlConstraints.AttachSizeToText.y) scaledSize.y				= rectText.Size.y + ncTotalSizeScaled.y;
	const bool															isValidParent											= 0 == ::gpk::controlInvalid(gui, control.IndexParent);
	const ::gpk::SCoord2<int32_t>										targetSize												= isValidParent ? gui.Controls.Metrics[control.IndexParent].Client.Global.Size : _targetSize.Cast<int32_t>();

	if(controlConstraints.AttachSizeToControl.x == iControl) { if(controlConstraints.DockToControl.Left	!= -1) {} else { scaledPosition.x = 0; } scaledSize.x = targetSize.x; } else if(false == ::gpk::controlInvalid(gui, controlConstraints.AttachSizeToControl.x)) { if(controlConstraints.DockToControl.Left != -1) {} else { scaledPosition.x = 0;} scaledSize.x = gui.Controls.Metrics[controlConstraints.AttachSizeToControl.x].Total.Global.Size.x; }
	if(controlConstraints.AttachSizeToControl.y == iControl) { if(controlConstraints.DockToControl.Top	!= -1) {} else { scaledPosition.y = 0; } scaledSize.y = targetSize.y; } else if(false == ::gpk::controlInvalid(gui, controlConstraints.AttachSizeToControl.y)) { if(controlConstraints.DockToControl.Top  != -1) {} else { scaledPosition.y = 0;} scaledSize.y = gui.Controls.Metrics[controlConstraints.AttachSizeToControl.y].Total.Global.Size.y; }

	controlMetrics.Client	.Local									= {{(int32_t)ncSizesScaled.Left, (int32_t)ncSizesScaled.Top}, (scaledSize - ncTotalSizeScaled).Cast<int32_t>()};
	::gpk::SCoord2<double> minSizeScaled = (controlConstraints.SizeMinMax.Min.Cast<double>().InPlaceScale(scale.x, scale.y));
	::gpk::SCoord2<double> maxSizeScaled = (controlConstraints.SizeMinMax.Max.Cast<double>().InPlaceScale(scale.x, scale.y));
	scaledSize.x = ::gpk::max(minSizeScaled.x, ::gpk::min(maxSizeScaled.x, scaledSize.x));
	scaledSize.y = ::gpk::max(minSizeScaled.y, ::gpk::min(maxSizeScaled.y, scaledSize.y));
	::gpk::realignRectangle(targetSize.Cast<uint32_t>(), ::gpk::SRectangle2D<int32_t>{scaledPosition.Cast<int32_t>(), scaledSize.Cast<int32_t>()}, controlMetrics.Total.Local, control.Align);
	controlMetrics.Total	.Global									= controlMetrics.Total	.Local;
	controlMetrics.Client	.Global									= controlMetrics.Client	.Local;
	controlMetrics.Client	.Global.Offset							+= controlMetrics.Total	.Local.Offset;
	if(isValidParent) {
		::gpk::SControlMetrics												& parentMetrics											= gui.Controls.Metrics[control.IndexParent];
		controlMetrics.Client	.Global.Offset							+= parentMetrics.Client.Global.Offset;
		controlMetrics.Total	.Global.Offset							+= parentMetrics.Client.Global.Offset;
	}

	const ::gpk::SRectLimits<int32_t>									& dockToControl											= controlConstraints.DockToControl;
	if(dockToControl.Right	!= -1) {
		gpk_necall(::gpk::controlInvalid(gui, dockToControl.Right), "Invalid control id: %i.", dockToControl.Right);
		::gpk::controlUpdateMetricsTopToDown(gui, dockToControl.Right, targetSize.Cast<uint32_t>(), true);
		const ::gpk::SControl												& other									= gui.Controls.Controls	[dockToControl.Right];
		const ::gpk::SControlMetrics										& otherMetrics							= gui.Controls.Metrics	[dockToControl.Right];
		if(gbit_true(other.Align, ::gpk::ALIGN_RIGHT) && gbit_false(other.Align, ::gpk::ALIGN_HCENTER)) {
			controlMetrics.Total	.Global.Offset.x						= otherMetrics.Total.Global.Offset.x - controlMetrics.Total.Global.Size.x;
			controlMetrics.Client	.Global.Offset.x						= (int32_t)(controlMetrics.Total.Global.Offset.x + ncSizesScaled.Left);
			int32_t																diffToSubstract							= targetSize.x - otherMetrics.Total.Local.Offset.x;
			if(controlConstraints.AttachSizeToControl.x == iControl) {
				controlMetrics.Total	.Global.Size.x							-= diffToSubstract;
				controlMetrics.Client	.Global.Size.x							-= diffToSubstract;
				controlMetrics.Total	.Global.Offset.x						+= diffToSubstract;
				controlMetrics.Client	.Global.Offset.x						+= diffToSubstract;
			}
		}
		else {
			controlMetrics.Total	.Global.Offset.x						= otherMetrics.Total.Global.Offset.x + otherMetrics.Total.Global.Size.x;
			controlMetrics.Client	.Global.Offset.x						= (int32_t)(controlMetrics.Total.Global.Offset.x + ncSizesScaled.Left);
		}
	}
	if(dockToControl.Bottom	!= -1) { gpk_necall(::gpk::controlInvalid(gui, dockToControl.Bottom	), "Invalid control id: %i.", dockToControl.Bottom	); const ::gpk::SControl & other = gui.Controls.Controls[dockToControl.Bottom	]; const ::gpk::SControlMetrics & otherMetrics = gui.Controls.Metrics[dockToControl.Bottom	]; if(gbit_true(other.Align, ::gpk::ALIGN_BOTTOM	) && gbit_false(other.Align, ::gpk::ALIGN_VCENTER)) { controlMetrics.Total.Global.Offset.y = otherMetrics.Total.Global.Offset.y - controlMetrics.Total.Global.Size.y; controlMetrics.Client.Global.Offset.y = (int32_t)(controlMetrics.Total.Global.Offset.y + ncSizesScaled.Top ); } else { controlMetrics.Total.Global.Offset.y = otherMetrics.Total.Global.Offset.y + otherMetrics.Total.Global.Size.y; controlMetrics.Client.Global.Offset.y = (int32_t)(controlMetrics.Total.Global.Offset.y + ncSizesScaled.Top ); } }
	if(dockToControl.Left	!= -1) {
		gpk_necall(::gpk::controlInvalid(gui, dockToControl.Left	), "Invalid control id: %i.", dockToControl.Left);
		::gpk::controlUpdateMetricsTopToDown(gui, dockToControl.Left, targetSize.Cast<uint32_t>(), true);
		const ::gpk::SControl												& other													= gui.Controls.Controls	[dockToControl.Left];
		const ::gpk::SControlMetrics										& otherMetrics											= gui.Controls.Metrics	[dockToControl.Left];
		if(gbit_true(other.Align, ::gpk::ALIGN_RIGHT) && gbit_false(other.Align, ::gpk::ALIGN_HCENTER)) {
			controlMetrics.Total	.Global.Offset.x						= otherMetrics				.Total.Global.Offset.x + otherMetrics.Total.Global.Size.x - controlMetrics.Total.Global.Size.x;
			controlMetrics.Client	.Global.Offset.x						= (int32_t)(controlMetrics	.Total.Global.Offset.x + ncSizesScaled.Left);
		}
		else {
			controlMetrics.Total	.Global.Offset.x						= otherMetrics				.Total.Global.Offset.x;
			controlMetrics.Client	.Global.Offset.x						= (int32_t)(controlMetrics	.Total.Global.Offset.x + ncSizesScaled.Left);
		}
	}
	//if(dockToControl.Top	!= -1) { gpk_necall(::controlInvalid(gui, dockToControl.Top		), "Invalid control id: %i.", dockToControl.Top		); const ::gpk::SControl & other = gui.Controls.Controls[dockToControl.Top		]; const ::gpk::SControlMetrics & otherMetrics = gui.Controls.Metrics[dockToControl.Bottom	]; if(gbit_true(other.Align, ::gpk::ALIGN_BOTTOM	) && gbit_false(other.Align, ::gpk::ALIGN_VCENTER)) { controlMetrics.Total.Global.Offset.y = otherMetrics.Total.Global.Offset.y - controlMetrics.Total.Global.Size.y; controlMetrics.Client.Global.Offset.y = controlMetrics.Total.Global.Offset.y + ncSizes.Top ; } else { controlMetrics.Total.Global.Offset.y = otherMetrics.Total.Global.Offset.y + otherMetrics.Total.Global.Size.y; controlMetrics.Client.Global.Offset.y = controlMetrics.Total.Global.Offset.y + ncSizes.Top ; } }

	{ // calculate text rectangle
		const ::gpk::SRectangle2D<int32_t>									& targetRect											= controlMetrics.Client.Global;
		controlMetrics.Text.Offset										= {};
		controlMetrics.Text.Size										= rectText.Size.Cast<int16_t>();
		::gpk::realignRectangle(targetRect.Size.Cast<uint32_t>(), controlMetrics.Text, controlMetrics.Text, controlText.Align);
		controlMetrics.Text.Offset										+= controlMetrics.Client.Global.Offset.Cast<int16_t>();
	}
	controlState.Updated											= true;
	return 0;
}

			::gpk::error_t										gpk::controlUpdateMetricsTopToDown							(::gpk::SGUI& gui, int32_t iControl, const ::gpk::SCoord2<uint32_t> & targetSize, bool forceUpdate)				{
	if(false == gui.Controls.States[iControl].Updated || forceUpdate)
		gpk_necall(::controlUpdateMetrics(gui, iControl, targetSize), "%s", "Unknown error! Maybe the control tree got broken?");
	::gpk::view_array<int32_t>											& children												= gui.Controls.Children[iControl];
	for(uint32_t iChild = 0; iChild < children.size(); ++iChild)
		gpk_necall(::gpk::controlUpdateMetricsTopToDown(gui, children[iChild], targetSize, forceUpdate), "%s", "Unknown error! Maybe the control tree got broken?");
	return 0;
}

			::gpk::error_t										gpk::guiUpdateMetrics									(::gpk::SGUI& gui, const ::gpk::SCoord2<uint32_t> & targetSize, bool forceUpdate)								{
	for(uint32_t iControl = 0; iControl < gui.Controls.Controls.size(); ++iControl)
		if(::gpk::controlInvalid(gui, gui.Controls.Controls[iControl].IndexParent) && false == ::gpk::controlInvalid(gui, iControl))
			gpk_necall(::gpk::controlUpdateMetricsTopToDown(gui, iControl, targetSize, forceUpdate), "%s", "Unknown error! Maybe the control tree got broken?");
	return 0;
}

static		::gpk::error_t										updateGUIControlHovered									(::gpk::SControlState& controlFlags, const ::gpk::SInput& inputSystem, bool disabled)								noexcept	{
	if(controlFlags.Hover) {
		if(inputSystem.ButtonDown(0) && false == controlFlags.Pressed)
			controlFlags.Pressed											= true;
		else {
			if(inputSystem.ButtonUp(0)) {
				controlFlags.Released											= true;
				if(controlFlags.Pressed) {
					controlFlags.Execute											= true;
					controlFlags.Pressed											= false;
				}
			}
		}
	}
	else
		controlFlags.Hover												= false == disabled;//controlFlags.Disabled;
	return one_if(controlFlags.Hover);
}

static		::gpk::error_t										controlProcessInput										(::gpk::SGUI& gui, ::gpk::SInput& input, int32_t iControl)														{
	::gpk::SControlState												& controlState											= gui.Controls.States[iControl];
	//--------------------
	::gpk::error_t														controlHovered											= -1;
	if(::gpk::in_range(gui.CursorPos.Cast<int32_t>(), gui.Controls.Metrics[iControl].Total.Global)) {
		if(false == gui.Controls.Modes[iControl].Design) {
			controlHovered													= iControl;
			::updateGUIControlHovered(controlState, input,  ::gpk::controlDisabled(gui, iControl));
		}
	}
	else {
		if (controlState.Hover) {
			controlState.Hover												= false;
			controlState.UnHover											= true;
		}

		if(input.ButtonUp(0) && controlState.Pressed)
			controlState.Pressed											= false;
	}
	{
		::gpk::view_array<int32_t>											& children													= gui.Controls.Children[iControl];
		for(uint32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild) {
			if(::gpk::controlHidden(gui, children[iChild]))
				continue;
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

			::gpk::error_t										gpk::guiProcessInput									(::gpk::SGUI& gui, ::gpk::SInput& input)																		{
	gerror_if(errored(::gpk::guiUpdateMetrics(gui, gui.LastSize, false)), "%s", "Why would this ever happen?");
	gui.CursorPos													+= {(float)input.MouseCurrent.Deltas.x, (float)input.MouseCurrent.Deltas.y};
	::gpk::error_t														controlHovered											= -1;
	::gpk::array_pod<uint32_t>											rootControlsToProcess									= {};
	rootControlsToProcess.resize(1000);
	rootControlsToProcess.clear();
	for(uint32_t iControl = 0, countControls = gui.Controls.Controls.size(); iControl < countControls; ++iControl) {	// Only process root parents
		::gpk::SControlState												& controlState											= gui.Controls.States[iControl];
		if(controlState.Unused || controlState.Disabled)
			continue;

		// Clear events that only last one tick.
		if (controlState.Execute	) controlState.Execute					= false;
		if (controlState.Released	) controlState.Released					= false;
		if (controlState.UnHover	) controlState.UnHover					= false;

		::gpk::SControl														& control												= gui.Controls.Controls[iControl];
		if(false == ::gpk::controlInvalid(gui, control.IndexParent))
			continue;
		rootControlsToProcess.push_back(iControl);
	}

	for(uint32_t iControl = 0, countControls = rootControlsToProcess.size(); iControl < countControls; ++iControl) {
		if(::gpk::controlHidden(gui, rootControlsToProcess[iControl]))
			continue;
		::gpk::error_t														controlPressed											= ::controlProcessInput(gui, input, rootControlsToProcess[iControl]);
		if(gui.Controls.Controls.size() > (uint32_t)controlPressed)
			controlHovered													= controlPressed;
	}
	if(controlHovered == -1)
		return gui.Controls.Controls.size();

	for(uint32_t iControl = 0, countControls = gui.Controls.Controls.size(); iControl < countControls; ++iControl) {
		if(iControl != (uint32_t)controlHovered) {
			::gpk::SControlState												& controlState											= gui.Controls.States[iControl];
			controlState.Hover												= false;
			if(0 == input.MouseCurrent.ButtonState[0])
				controlState.Pressed											= false;
		}
		//else {
		//	verbose_printf("Hovered: %u.", iControl);
		//}
	}
	return controlHovered;
}

			::gpk::error_t										gpk::controlHidden										(const ::gpk::SGUI& gui, int32_t iControl)	{
	bool																imHidden												= ::gpk::controlInvalid(gui, iControl) || gui.Controls.States[iControl].Hidden;
	return imHidden ? imHidden : (false == ::gpk::controlInvalid(gui, gui.Controls.Controls[iControl].IndexParent) && ::gpk::controlHidden(gui, gui.Controls.Controls[iControl].IndexParent));
}

			::gpk::error_t										gpk::controlDisabled									(const ::gpk::SGUI& gui, int32_t iControl)	{
	bool																imDisabled												= ::gpk::controlInvalid(gui, iControl) || gui.Controls.States[iControl].Disabled;
	return imDisabled ? imDisabled : (false == ::gpk::controlInvalid(gui, gui.Controls.Controls[iControl].IndexParent) && ::gpk::controlDisabled(gui, gui.Controls.Controls[iControl].IndexParent));
}

			::gpk::error_t										gpk::controlMetricsInvalidate							(::gpk::SGUI& gui, int32_t iControl)	{
	::gpk::SControlState												& controlState											= gui.Controls.States[iControl];
	controlState.Updated											= false;
	const ::gpk::view_array<int32_t>									& controlChildren										= gui.Controls.Children[iControl];
	for(uint32_t iChild = 0, countChild = controlChildren.size(); iChild < countChild; ++iChild)
		gpk_necall(::gpk::controlMetricsInvalidate(gui, controlChildren[iChild]), "%s", "Invalid child?");
	return 0;
}

			::gpk::error_t										gpk::guiGetProcessableControls							(::gpk::SGUI& gui, ::gpk::array_pod<uint32_t>& controlIndices)													{
	for(uint32_t iControl = 0, countControls = gui.Controls.Controls.size(); iControl < countControls; ++iControl) {	// Only process root parents
		const ::gpk::SControlState											& controlState											= gui.Controls.States[iControl];
		if(controlState.Unused || gui.Controls.Modes[iControl].Design || ::gpk::controlDisabled(gui, iControl) || ::gpk::controlHidden(gui, iControl))
			continue;
		gpk_necall(controlIndices.push_back(iControl), "%s", "Out of memory?");
	}
	return controlIndices.size();
}

			::gpk::error_t										gpk::controlTextSet										(::gpk::SGUI& gui, int32_t iControl, const ::gpk::view_const_string& text)				{
	::gpk::view_const_string												& oldText												= gui.Controls.Text[iControl].Text;
	if(text.begin	() != oldText.begin()
	|| text.size	() != oldText.size()
	) {
		oldText															= text;
		gui.Controls.States[iControl].Updated							= false;
	}
	return 0;
}
