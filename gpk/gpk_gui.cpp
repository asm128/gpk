#include "gpk_gui.h"
#include "gpk_gui_text.h"
#include "gpk_bitmap_target.h"
#include "gpk_grid_copy.h"
#include "gpk_encoding.h"

static		::gpk::error_t										controlInvalid											(const ::gpk::SGUI& gui, int32_t iControl)				{
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
	::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboDisabled									= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_DISABLED	]	= {};
	::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboPressed 									= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_PRESSED		]	= {};
	::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboSelected									= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_SELECTED	]	= {};
	::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboHover 										= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_HOVER		]	= {};
	::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>		& colorComboNormal										= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_NORMAL		]	= {};

	//const int32_t														colorBase												= iColor / iShades;
	const int32_t														colorShade												= iColor % iShades;
	const int32_t														colorText												= (::gpk::ASCII_COLOR_WHITE * iShades);

	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]	= (colorShade > 7) ? iColor : iColor;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT	]	= (colorShade > 7) ? iColor - 1 : iColor + 1;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP	]	= (colorShade > 7) ? iColor - 1 : iColor + 1;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT	]	= (colorShade > 7) ? iColor - 1 : iColor + 1;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM	]	= (colorShade > 7) ? iColor - 1 : iColor + 1;
	colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_TEXT_FACE		]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
//
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]	= (colorShade > 7) ? iColor : iColor;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT	]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BORDER_TOP	]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT	]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
	colorComboPressed 	[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM	]	= (colorShade > 7) ? iColor - 3 : iColor + 3;
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

static		::gpk::error_t										controlInstanceReset									(::gpk::SGUIControlTable& gui, int32_t iControl)										{
	::gpk::SGUIControlTable												& controlTable											= gui;
	controlTable.Metrics	[iControl]								= {};
	controlTable.Children	[iControl]								= ::gpk::view_array<int32_t>{};
	controlTable.Modes		[iControl]								= {};	
	controlTable.States		[iControl]								= {};

	::gpk::SControl														& control				= controlTable.Controls		[iControl]	= {};
	::gpk::SControlConstraints											& controlConstraints	= controlTable.Constraints	[iControl]	= {};	
	::gpk::SControlText													& controlText			= controlTable.Text			[iControl]	= {};
	control.IndexParent												= -1;
	control.Align													= ::gpk::ALIGN_TOP_LEFT;
	control.Area													= {{0, 0}, {16, 16}};
	control.Border													= {1, 1, 1, 1};
	control.Margin													= {1, 0, 1, 0};
	controlConstraints.AttachSizeToControl							= {-1, -1};
	controlConstraints.AttachSizeToText								= {};
	controlConstraints.DockToControl								= {-1, -1, -1, -1};
	controlText.Align												= ::gpk::ALIGN_CENTER;
	return 0;
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
			gpk_necall(::controlInstanceReset(gui.Controls, iControl), "%s", "Unknown issue!");
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
		) - 1, "%s", "Failed to resize! Out of memory?");
	gpk_necall(::controlInstanceReset(gui.Controls, iControl), "%s", "Unknown issue!");
	return iControl; 
}

			::gpk::error_t										gpk::controlDelete										(::gpk::SGUI& gui, int32_t iControl)					{ 
	info_printf("Deleting control id: %i.", iControl);
	gpk_necall(::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	{ // Delete children first.
		::gpk::array_pod<int32_t>											children												= gui.Controls.Children[iControl];
		for(uint32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
			error_if(errored(::gpk::controlDelete(gui, children[iChild])), "Failed to delete control! Invalid control id? %i.", children[iChild]);
		//	info_printf("Deleting child control id: %i.", children[iChild]);
	}

	// Remove from parent list
	const uint32_t														indexParent												= (uint32_t)gui.Controls.Controls[iControl].IndexParent;
#if defined(GPK_DEBUG_ENABLED)
	int32_t																childrenRemoved = 0;
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
	error_if(childrenRemoved > 1, "%s", "Parent should not reference a child control more than once.");
#endif
	::gpk::SControlState												& controlState											= gui.Controls.States[iControl];
	controlState.Unused												= true;
	return 0; 
}

			::gpk::error_t										gpk::controlSetParent									(::gpk::SGUI& gui, int32_t iControl, int32_t iParent)	{
	gpk_necall(::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	const uint32_t														oldParent												= gui.Controls.Controls[iControl].IndexParent;
	::gpk::SControl														& control												= gui.Controls.Controls[iControl];
	if(control.IndexParent == iParent)	// Exit early if there is nothing to do here.
		return 0;

	control.IndexParent												= iParent;
	if(::controlInvalid(gui, iParent)) {
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
	gpk_necall(::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
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
	const bool															isValidParent											= 0 == ::controlInvalid(gui, control.IndexParent);
	const ::gpk::SCoord2<int32_t>										targetSize												= isValidParent ? gui.Controls.Metrics[control.IndexParent].Client.Global.Size : _targetSize.Cast<int32_t>();

	if(controlConstraints.AttachSizeToControl.x == iControl) { if(controlConstraints.DockToControl.Left	!= -1) {} else { scaledPosition.x = 0; } scaledSize.x = targetSize.x; } else if(false == ::controlInvalid(gui, controlConstraints.AttachSizeToControl.x)) { if(controlConstraints.DockToControl.Left != -1) {} else { scaledPosition.x = 0;} scaledSize.x = gui.Controls.Metrics[controlConstraints.AttachSizeToControl.x].Total.Global.Size.x; }
	if(controlConstraints.AttachSizeToControl.y == iControl) { if(controlConstraints.DockToControl.Top	!= -1) {} else { scaledPosition.y = 0; } scaledSize.y = targetSize.y; } else if(false == ::controlInvalid(gui, controlConstraints.AttachSizeToControl.y)) { if(controlConstraints.DockToControl.Top  != -1) {} else { scaledPosition.y = 0;} scaledSize.y = gui.Controls.Metrics[controlConstraints.AttachSizeToControl.y].Total.Global.Size.y; }
	
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
		gpk_necall(::controlInvalid(gui, dockToControl.Right), "Invalid control id: %i.", dockToControl.Right); 
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
	if(dockToControl.Bottom	!= -1) { gpk_necall(::controlInvalid(gui, dockToControl.Bottom	), "Invalid control id: %i.", dockToControl.Bottom	); const ::gpk::SControl & other = gui.Controls.Controls[dockToControl.Bottom	]; const ::gpk::SControlMetrics & otherMetrics = gui.Controls.Metrics[dockToControl.Bottom	]; if(gbit_true(other.Align, ::gpk::ALIGN_BOTTOM	) && gbit_false(other.Align, ::gpk::ALIGN_VCENTER)) { controlMetrics.Total.Global.Offset.y = otherMetrics.Total.Global.Offset.y - controlMetrics.Total.Global.Size.y; controlMetrics.Client.Global.Offset.y = (int32_t)(controlMetrics.Total.Global.Offset.y + ncSizesScaled.Top ); } else { controlMetrics.Total.Global.Offset.y = otherMetrics.Total.Global.Offset.y + otherMetrics.Total.Global.Size.y; controlMetrics.Client.Global.Offset.y = (int32_t)(controlMetrics.Total.Global.Offset.y + ncSizesScaled.Top ); } }
	if(dockToControl.Left	!= -1) { 
		gpk_necall(::controlInvalid(gui, dockToControl.Left	), "Invalid control id: %i.", dockToControl.Left); 
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

static constexpr	const char									gpk_codepage_437_b64	[]								= "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";

static		::gpk::error_t										setupDefaultFontTexture									(::gpk::SGUI& gui)																						{
	gui.FontCharSize												= {9, 16};
	gui.FontTexture.resize(gui.FontCharSize.x, gui.FontCharSize.y * 256);
	::gpk::array_pod<ubyte_t>											decoded;
	gpk_necall(::gpk::base64Decode(gpk_codepage_437_b64, decoded), "%s", "Maybe the decode function got broken?");
	memcpy(gui.FontTexture.Texels.begin(), decoded.begin(), decoded.size());
	return 0;
}

static		::gpk::error_t										textLineRaster											(const ::gpk::SCoord2<uint32_t> & targetMetrics, const ::gpk::SCoord2<uint16_t> & fontCharSize, const ::gpk::SRectangle2D<int16_t> & targetRect, const ::gpk::SImageMonochrome<uint32_t>& fontTexture, const ::gpk::view_const_string& textToRaster, ::gpk::array_pod<::gpk::SCoord2<int32_t>> & dstCoords)												{
	const uint32_t														charsPerRow												= fontTexture.Pitch / fontCharSize.x;
	for(uint32_t iChar = 0, countChars = (uint32_t)textToRaster.size(); iChar < countChars; ++iChar) {
		char																charToDraw												= textToRaster[iChar];
		const int32_t														coordTableX												= charToDraw % (int32_t)charsPerRow;
		const int32_t														coordTableY												= charToDraw / (int32_t)charsPerRow;
		const ::gpk::SCoord2<int32_t>										coordCharTable											= ::gpk::SCoord2<int32_t>{coordTableX * fontCharSize.x, coordTableY * fontCharSize.y};
		const ::gpk::SRectangle2D<int32_t>									srcRect0												= {coordCharTable, fontCharSize.Cast<int32_t>()};
		::gpk::SRectangle2D<int32_t>										dstRect1												= {{((int32_t)targetRect.Offset.x + (int32_t)fontCharSize.x * (int32_t)iChar), targetRect.Offset.y}, fontCharSize.Cast<int32_t>()};
		//dstRect1.Size.x													+= ::gpk::min(0, (int32_t)targetRect.Offset.x);
		dstRect1.Size.y													+= ::gpk::min(0, (int32_t)targetRect.Offset.y);
		error_if(errored(::gpk::grid_raster_alpha_bit(targetMetrics, fontTexture.View, {charsPerRow * fontCharSize.x, 256U * fontCharSize.y}, dstRect1, srcRect0.Offset, dstCoords)), "%s", "I believe this never fails.");
	}
	return 0;
}

static		::gpk::error_t										controlTextDraw											(::gpk::SGUI& gui, int32_t iControl, ::gpk::view_grid<::gpk::SColorBGRA>& target)						{
	if(0 == gui.FontTexture.Texels.size())
		gpk_necall(::setupDefaultFontTexture(gui), "%s", "Failed to set up default texture!");

	gpk_necall(0 == gui.FontTexture.Texels.size() || 0 == gui.FontTexture.Pitch, "%s", "Invalid font texture!");

	const ::gpk::SControl												& control												= gui.Controls.Controls	[iControl];
	::gpk::SControlState												& controlState											= gui.Controls.States	[iControl];
	const ::gpk::SControlTheme											& theme													= gui.ControlThemes[(0 == control.ColorTheme) ? gui.ThemeDefault : control.ColorTheme - 1];
	const ::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>	& colorCombo											= theme.ColorCombos
		[ ::gpk::controlDisabled(gui, iControl)	? ::gpk::GUI_CONTROL_STATE_COLORS_DISABLED
		: controlState.Pressed	? ::gpk::GUI_CONTROL_STATE_COLORS_PRESSED 
		: controlState.Selected	? ::gpk::GUI_CONTROL_STATE_COLORS_SELECTED
		: controlState.Hover	? ::gpk::GUI_CONTROL_STATE_COLORS_HOVER 
		: ::gpk::GUI_CONTROL_STATE_COLORS_NORMAL
		];
	//const ::gpk::SColorBGRA												colorBack												= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND]];
	const ::gpk::SColorBGRA												colorFace												= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_TEXT_FACE]];

	::gpk::SControlMetrics												& controlMetrics										= gui.Controls.Metrics	[iControl];
	::gpk::SRectangle2D<int16_t>										rectText												= controlMetrics.Text;
	if(controlState.Pressed || controlState.Hover) { // shift text offset by one pixel to give the effect of being pushed. 
		rectText.Offset													+= ::gpk::SCoord2<int16_t>{1, 1};
		rectText.Size													-= ::gpk::SCoord2<int16_t>{1, 1};
		if(rectText.Size.x < 0) rectText.Size.x = 0;
		if(rectText.Size.y < 0) rectText.Size.y = 0;
	} // This isn't done in updateMetrics() because the function is only executed for outdated metrics. 
	// Changhing the state from idle to hover however doesn't cause the control metrics to become outdated (and in general it's pointless for other than the effect we're applying here).

	::gpk::SControlText													& controlText											= gui.Controls.Text		[iControl];
	::gpk::array_pod<::gpk::SCoord2<int32_t>>							dstCoords;
	::textLineRaster(target.metrics(), gui.FontCharSize, rectText, gui.FontTexture, controlText.Text, dstCoords);
	for(uint32_t iCoord = 0; iCoord < dstCoords.size(); ++iCoord)
		::gpk::drawPixelLight(target, dstCoords[iCoord], colorFace, controlState.Pressed ? 0.75f : 0.5f, controlState.Pressed ? 1.0f : 0.95);
	return 0;
}

static		::gpk::error_t										actualControlDraw										(::gpk::SGUI& gui, int32_t iControl, ::gpk::view_grid<::gpk::SColorBGRA>& target)					{
	const ::gpk::SControlState											& controlState											= gui.Controls.States	[iControl];
	const ::gpk::SControlMode											& mode													= gui.Controls.Modes	[iControl];
	const ::gpk::SControl												& control												= gui.Controls.Controls	[iControl];
	::gpk::SColorBGRA													colors			[::gpk::GUI_CONTROL_AREA_COUNT]			= {}; // -- Fill color table
	::gpk::GUI_COLOR_MODE												colorMode												= (mode.ColorMode == ::gpk::GUI_COLOR_MODE_DEFAULT) ? gui.ColorModeDefault : mode.ColorMode;
	const ::gpk::SControlTheme											& theme													= gui.ControlThemes[(0 == control.ColorTheme) ? gui.ThemeDefault : control.ColorTheme - 1];
	const ::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>	& colorCombo											= theme.ColorCombos
		[ ::gpk::controlDisabled(gui, iControl)	? ::gpk::GUI_CONTROL_STATE_COLORS_DISABLED
		: controlState.Pressed	? ::gpk::GUI_CONTROL_STATE_COLORS_PRESSED 
		: controlState.Selected	? ::gpk::GUI_CONTROL_STATE_COLORS_SELECTED
		: controlState.Hover	? ::gpk::GUI_CONTROL_STATE_COLORS_HOVER 
		: ::gpk::GUI_CONTROL_STATE_COLORS_NORMAL
		];
	colors[::gpk::GUI_CONTROL_AREA_BACKGROUND			]			= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]];
	colors[::gpk::GUI_CONTROL_AREA_CLIENT				]			= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_CLIENT		]];

	if(colorMode == ::gpk::GUI_COLOR_MODE_THEME) { 
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
	const ::gpk::SCoord2<double>										scaleFinal												= gui.Zoom.DPI * gui.Zoom.ZoomLevel;
	scaledBorders.Left												= (int32_t)(control.Border.Left		* scaleFinal.x);
	scaledBorders.Top												= (int32_t)(control.Border.Top		* scaleFinal.y);
	scaledBorders.Right												= (int32_t)(control.Border.Right	* scaleFinal.x);
	scaledBorders.Bottom											= (int32_t)(control.Border.Bottom	* scaleFinal.y);

	const ::gpk::SRectangle2D<int32_t>									& rectTotal												= controlMetrics.Total.Global;
	finalRects[::gpk::GUI_CONTROL_AREA_BACKGROUND		]			= rectTotal; 
	finalRects[::gpk::GUI_CONTROL_AREA_CLIENT			]			= controlMetrics.Client.Global; 
	finalRects[::gpk::GUI_CONTROL_AREA_BORDER_LEFT		]			= {rectTotal.Offset , ::gpk::SCoord2<int32_t>{control.Border.Left, rectTotal.Size.y}}; 
	finalRects[::gpk::GUI_CONTROL_AREA_BORDER_TOP		]			= {rectTotal.Offset , ::gpk::SCoord2<int32_t>{rectTotal.Size.x, control.Border.Top}}; 
	finalRects[::gpk::GUI_CONTROL_AREA_BORDER_RIGHT		]			= {rectTotal.Offset + ::gpk::SCoord2<int32_t>{rectTotal.Size.x - control.Border.Right, 0}, ::gpk::SCoord2<int32_t>{control.Border.Right, controlMetrics.Total.Global.Size.y}};
	finalRects[::gpk::GUI_CONTROL_AREA_BORDER_BOTTOM	]			= {rectTotal.Offset + ::gpk::SCoord2<int32_t>{0, rectTotal.Size.y - control.Border.Bottom}, ::gpk::SCoord2<int32_t>{rectTotal.Size.x, control.Border.Bottom}};
	finalRects[::gpk::GUI_CONTROL_AREA_BACKGROUND		].Offset.x	+= control.Border.Left	;
	finalRects[::gpk::GUI_CONTROL_AREA_BACKGROUND		].Offset.y	+= control.Border.Top	;
	finalRects[::gpk::GUI_CONTROL_AREA_BACKGROUND		].Size	.x	-= control.Border.Left	+ control.Border.Right	;
	finalRects[::gpk::GUI_CONTROL_AREA_BACKGROUND		].Size	.y	-= control.Border.Top	+ control.Border.Bottom	;

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

	if(control.Image.metrics().LengthSquared())
		::gpk::grid_copy(target, control.Image, //controlMetrics.Client.Global);
			::gpk::SRectangle2D<int32_t>
				{ controlMetrics.Client.Global.Offset
				, controlMetrics.Client.Global.Size + ::gpk::SCoord2<int32_t>{::gpk::min(0, ::gpk::min(controlMetrics.Client.Global.Offset.x, controlMetrics.Client.Local.Offset.x)), ::gpk::min(0, ::gpk::min(controlMetrics.Client.Global.Offset.y, controlMetrics.Client.Local.Offset.y))}
				});

	error_if(errored(::controlTextDraw(gui, iControl, target)), "%s", "Why would this ever happen?");
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
		if(::controlInvalid(gui, gui.Controls.Controls[iControl].IndexParent) && false == ::controlInvalid(gui, iControl))
			gpk_necall(::gpk::controlUpdateMetricsTopToDown(gui, iControl, targetSize, forceUpdate), "%s", "Unknown error! Maybe the control tree got broken?");
	return 0;
}

			::gpk::error_t										gpk::controlDrawHierarchy								(::gpk::SGUI& gui, int32_t iControl, ::gpk::view_grid<::gpk::SColorBGRA>& target)								{
	gpk_necall(::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	if(gui.LastSize != target.metrics()) {
		for(uint32_t iOutdated = 0; iOutdated < gui.Controls.Controls.size(); ++iOutdated)
			gui.Controls.States[iOutdated].Updated							= false;
		gui.LastSize													= target.metrics();
	}
	::gpk::SControlState												controlState											= gui.Controls.States[iControl];
	if(false == controlState.Hidden) {
		if(false == controlState.Design)
			gpk_necall(::actualControlDraw(gui, iControl, target), "%s", "Unknown issue!");
		::gpk::view_array<int32_t>											& children												= gui.Controls.Children[iControl];
		for(uint32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild) 
			gpk_necall(::gpk::controlDrawHierarchy(gui, children[iChild], target), "%s", "Unknown issue!");
	}
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
	if(controlState.Hidden)
		return -1;
	//--------------------
	::gpk::error_t														controlHovered											= -1;
	if(::gpk::in_range(gui.CursorPos.Cast<int32_t>(), gui.Controls.Metrics[iControl].Total.Global)) {
		if(false == controlState.Design) {
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
	error_if(errored(::gpk::guiUpdateMetrics(gui, gui.LastSize, false)), "%s", "Why would this ever happen?");
	gui.CursorPos													+= {(float)input.MouseCurrent.Deltas.x, (float)input.MouseCurrent.Deltas.y};
	::gpk::error_t														controlHovered											= -1;

	::gpk::array_pod<uint32_t>											rootControlsToProcess									= {};
	for(uint32_t iControl = 0, countControls = gui.Controls.Controls.size(); iControl < countControls; ++iControl) {	// Only process root parents
		::gpk::SControlState												& controlState											= gui.Controls.States[iControl];
		if(controlState.Unused || controlState.Disabled)
			continue;

		// Clear events that only last one tick.
		if (controlState.Execute	) controlState.Execute					= false;
		if (controlState.Released	) controlState.Released					= false;
		if (controlState.UnHover	) controlState.UnHover					= false;

		::gpk::SControl														& control												= gui.Controls.Controls[iControl];
		if(false == ::controlInvalid(gui, control.IndexParent))	
			continue;
		rootControlsToProcess.push_back(iControl);
	}

	for(uint32_t iControl = 0, countControls = rootControlsToProcess.size(); iControl < countControls; ++iControl) {
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

			::gpk::error_t										gpk::guiDraw											(::gpk::SGUI& gui, ::gpk::view_grid<::gpk::SColorBGRA>& target)													{
	if(gui.LastSize != target.metrics()) {
		for(uint32_t iOutdated = 0; iOutdated < gui.Controls.Controls.size(); ++iOutdated)
			gui.Controls.States[iOutdated].Updated							= false;
		gui.LastSize													= target.metrics();
	}
	gpk_necall(::gpk::guiUpdateMetrics(gui, gui.LastSize, false), "%s", "Unknown issue!");;
	for(uint32_t iControl = 0; iControl < gui.Controls.Controls.size(); ++iControl)
		if(false == ::controlInvalid(gui, iControl) && ::controlInvalid(gui, gui.Controls.Controls[iControl].IndexParent))
			error_if(errored(::gpk::controlDrawHierarchy(gui, iControl, target)),"%s",  "Unknown issue!");
	return 0;
}

			::gpk::error_t										gpk::controlHidden										(::gpk::SGUI& gui, int32_t iControl)	{
	bool																imHidden												= ::controlInvalid(gui, iControl) || gui.Controls.States[iControl].Hidden;
	return imHidden ? imHidden : (false == ::controlInvalid(gui, gui.Controls.Controls[iControl].IndexParent) && ::gpk::controlHidden(gui, gui.Controls.Controls[iControl].IndexParent));
}

			::gpk::error_t										gpk::controlDisabled									(::gpk::SGUI& gui, int32_t iControl)	{
	bool																imDisabled												= ::controlInvalid(gui, iControl) || gui.Controls.States[iControl].Disabled;
	return imDisabled ? imDisabled : (false == ::controlInvalid(gui, gui.Controls.Controls[iControl].IndexParent) && ::gpk::controlDisabled(gui, gui.Controls.Controls[iControl].IndexParent));
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
		if(controlState.Unused || ::gpk::controlDisabled(gui, iControl) || ::gpk::controlHidden(gui, iControl))
			continue;
		gpk_necall(controlIndices.push_back(iControl), "%s", "Out of memory?");
	}
	return controlIndices.size();
}
