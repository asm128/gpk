#include "gpk_gui.h"
#include "gpk_bitmap_target.h"
#include "gpk_grid_copy.h"
#include "gpk_encoding.h"

static		::gpk::error_t										controlInvalid											(const ::gpk::SGUI& gui, int32_t iControl)				{
	if(gui.Controls.Controls.size() <= uint32_t(iControl)	) return -1;
	if(gui.Controls.States[iControl].Unused					) return -1;
	return 0;
}

static		::gpk::error_t										paletteSetupDefault										(::gpk::array_pod<::gpk::SColorBGRA>& palette, const ::gpk::array_view<const ::gpk::SColorBGRA>& colors, uint32_t iShades)	{
	const uint32_t														newPaletteSize											= colors.size() * iShades;
	if(palette.size() < newPaletteSize)
		gpk_necall(palette.resize(newPaletteSize), "Out of memory?");
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
			, ::gpk::SColorBGRA{(uint32_t)0xFFFFFFFF & (::gpk::ASCII_COLOR_INDEX_4 * (uint64_t)::gpk::ASCII_COLOR_INDEX_11)}
			, ::gpk::SColorBGRA{(uint32_t)0xFFFFFFFF & (::gpk::ASCII_COLOR_INDEX_5 * (uint64_t)::gpk::ASCII_COLOR_INDEX_10)}
			, ::gpk::SColorBGRA{(uint32_t)0xFFFFFFFF & (::gpk::ASCII_COLOR_INDEX_7 * (uint64_t)::gpk::ASCII_COLOR_INDEX_8 )}
			, ::gpk::SColorBGRA{(uint32_t)(::gpk::ASCII_COLOR_INDEX_1 + ::gpk::ASCII_COLOR_INDEX_4 )}
			, ::gpk::SColorBGRA{(uint32_t)(::gpk::ASCII_COLOR_INDEX_3 + ::gpk::ASCII_COLOR_INDEX_7 )}
			, ::gpk::SColorBGRA{(uint32_t)(::gpk::ASCII_COLOR_INDEX_1 + ::gpk::ASCII_COLOR_INDEX_8 )}
			, ::gpk::SColorBGRA{(uint32_t)(::gpk::ASCII_COLOR_INDEX_4 + ::gpk::ASCII_COLOR_INDEX_7)}

			, ::gpk::SColorBGRA{::gpk::BROWN}
			, ::gpk::SColorBGRA{::gpk::LIGHTORANGE}
		};
		gpk_necall(::paletteSetupDefault(gui.Palette, paletteColors, iShades), "Unknown issue!");
	}
	if(0 == gui.ControlThemes.size()) 
		gpk_necall(::themeSetupDefault(gui.Palette, gui.ControlThemes, iShades), "Unknown issue! Maybe the palette system got broken?");

	for(uint32_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl) {
		if(gui.Controls.States[iControl].Unused) {
			gpk_necall(::controlInstanceReset(gui.Controls, iControl), "Unknown issue!");
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
	gpk_necall(::controlInstanceReset(gui.Controls, iControl), "Unknown issue!");
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
	error_if(childrenRemoved > 1, "Parent should not reference a child control more than once.");
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

	gpk_necall(children.push_back(iControl), "Out of memory?");
	return 0;
}

static		::gpk::error_t										controlUpdateMetrics									(::gpk::SGUI& gui, int32_t iControl, const ::gpk::SCoord2<uint32_t> & _targetSize, bool forceUpdate)					{
	gpk_necall(::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	::gpk::SControlState												& controlState											= gui.Controls.States[iControl];
	if(false == controlState.Outdated && false == forceUpdate)
		return 0;

	const ::gpk::SControl												& control												= gui.Controls.Controls[iControl];
	::gpk::SCoord2<double>												scale													= gui.Zoom.DPI * gui.Zoom.ZoomLevel;
	::gpk::SCoord2<int32_t>												scaledPosition											= ::gpk::SCoord2<double>{control.Area.Offset	.x * scale.x, control.Area.Offset	.y * scale.y}.Cast<int32_t>();
	::gpk::SCoord2<int32_t>												scaledSize												= ::gpk::SCoord2<double>{control.Area.Size		.x * scale.x, control.Area.Size		.y * scale.y}.Cast<int32_t>();
	::gpk::SRectLimits<int32_t>											scaledBorders											= {(int32_t)(control.Border.Left * scale.x), (int32_t)(control.Border.Top * scale.y), (int32_t)(control.Border.Right * scale.x), (int32_t)(control.Border.Bottom * scale.y)};
	::gpk::SRectLimits<int32_t>											scaledMargins											= {(int32_t)(control.Margin.Left * scale.x), (int32_t)(control.Margin.Top * scale.y), (int32_t)(control.Margin.Right * scale.x), (int32_t)(control.Margin.Bottom * scale.y)};
	::gpk::SControlMetrics												& controlMetrics										= gui.Controls.Metrics[iControl];
	::gpk::SRectangle2D<int16_t>										& rectText												= controlMetrics.Text;
	::gpk::SControlText													& controlText											= gui.Controls.Text[iControl];
	rectText.Size													= {(int16_t)(gui.FontCharSize.x * controlText.Text.size()), (int16_t)gui.FontCharSize.y};
	const ::gpk::SControlConstraints									& controlConstraints									= gui.Controls.Constraints[iControl];
	const ::gpk::SRectLimits<int32_t>									ncSizes													= ::gpk::controlNCRect(control);
	const ::gpk::SCoord2<int32_t>										ncTotalSize												= 
		{ ncSizes.Left	+ ncSizes.Right		
		, ncSizes.Top	+ ncSizes.Bottom	
		};

	if(controlConstraints.AttachSizeToText.x)	scaledSize.x			= int32_t(rectText.Size.x * scale.x + ncTotalSize.x);
	if(controlConstraints.AttachSizeToText.y)	scaledSize.y			= int32_t(rectText.Size.y * scale.y + ncTotalSize.y);

	const bool															isValidParent											= 0 == ::controlInvalid(gui, control.IndexParent);
	::gpk::SCoord2<int32_t>												targetSize												= isValidParent ? gui.Controls.Metrics[control.IndexParent].Client.Global.Size : _targetSize.Cast<int32_t>();
	//if(isValidParent) 
	//	gpk_necall(::controlUpdateMetrics(gui, control.IndexParent, _targetSize, forceUpdate), "Unknown issue!");

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
	controlMetrics.Client	.Global.Offset							+= controlMetrics.Total.Local.Offset;
	//controlMetrics.Client	.Global.Offset.x						+= controlMetrics.Total.Local.Offset.x;
	//controlMetrics.Client	.Global.Offset.y						+= controlMetrics.Total.Local.Offset.y;

	if(isValidParent) {
		::gpk::SControlMetrics												& parentMetrics											= gui.Controls.Metrics[control.IndexParent];
		controlMetrics.Client	.Global.Offset							+= parentMetrics.Client.Global.Offset;
		controlMetrics.Total	.Global.Offset							+= parentMetrics.Client.Global.Offset;
	}

	const ::gpk::SRectLimits<int32_t>									& dockToControl											= controlConstraints.DockToControl;
	if(dockToControl.Right	!= -1) { gpk_necall(::controlInvalid(gui, dockToControl.Right	), "Invalid control id: %i.", dockToControl.Right	); const ::gpk::SControl & other = gui.Controls.Controls[dockToControl.Right	]; const ::gpk::SControlMetrics & otherMetrics = gui.Controls.Metrics[dockToControl.Right	]; if(gbit_true(other.Align, ::gpk::ALIGN_RIGHT		) && gbit_false(other.Align, ::gpk::ALIGN_HCENTER)) { controlMetrics.Total.Global.Offset.x = otherMetrics.Total.Global.Offset.x - controlMetrics.Total.Global.Size.x; controlMetrics.Client.Global.Offset.x = controlMetrics.Total.Global.Offset.x + ncSizes.Left; } else { controlMetrics.Total.Global.Offset.x = otherMetrics.Total.Global.Offset.x + otherMetrics.Total.Global.Size.x; controlMetrics.Client.Global.Offset.x = controlMetrics.Total.Global.Offset.x + ncSizes.Left; } }
	if(dockToControl.Bottom	!= -1) { gpk_necall(::controlInvalid(gui, dockToControl.Bottom	), "Invalid control id: %i.", dockToControl.Bottom	); const ::gpk::SControl & other = gui.Controls.Controls[dockToControl.Bottom	]; const ::gpk::SControlMetrics & otherMetrics = gui.Controls.Metrics[dockToControl.Bottom	]; if(gbit_true(other.Align, ::gpk::ALIGN_BOTTOM	) && gbit_false(other.Align, ::gpk::ALIGN_VCENTER)) { controlMetrics.Total.Global.Offset.y = otherMetrics.Total.Global.Offset.y - controlMetrics.Total.Global.Size.y; controlMetrics.Client.Global.Offset.y = controlMetrics.Total.Global.Offset.y + ncSizes.Top ; } else { controlMetrics.Total.Global.Offset.y = otherMetrics.Total.Global.Offset.y + otherMetrics.Total.Global.Size.y; controlMetrics.Client.Global.Offset.y = controlMetrics.Total.Global.Offset.y + ncSizes.Top ; } }
	if(dockToControl.Left	!= -1) { gpk_necall(::controlInvalid(gui, dockToControl.Left	), "Invalid control id: %i.", dockToControl.Left	); const ::gpk::SControl & other = gui.Controls.Controls[dockToControl.Left		]; const ::gpk::SControlMetrics & otherMetrics = gui.Controls.Metrics[dockToControl.Left	]; if(gbit_true(other.Align, ::gpk::ALIGN_RIGHT		) && gbit_false(other.Align, ::gpk::ALIGN_HCENTER)) { controlMetrics.Total.Global.Offset.x = otherMetrics.Total.Global.Offset.x + otherMetrics.Total.Global.Size.x - controlMetrics.Total.Global.Size.x; controlMetrics.Client.Global.Offset.x = controlMetrics.Total.Global.Offset.x + ncSizes.Left; } else { controlMetrics.Total.Global.Offset.x = otherMetrics.Total.Global.Offset.x; controlMetrics.Client.Global.Offset.x = controlMetrics.Total.Global.Offset.x + ncSizes.Left; } }
	//if(dockToControl.Top	!= -1) { gpk_necall(::controlInvalid(gui, dockToControl.Top		), "Invalid control id: %i.", dockToControl.Top		); const ::gpk::SControl & other = gui.Controls.Controls[dockToControl.Top		]; const ::gpk::SControlMetrics & otherMetrics = gui.Controls.Metrics[dockToControl.Bottom	]; if(gbit_true(other.Align, ::gpk::ALIGN_BOTTOM	) && gbit_false(other.Align, ::gpk::ALIGN_VCENTER)) { controlMetrics.Total.Global.Offset.y = otherMetrics.Total.Global.Offset.y - controlMetrics.Total.Global.Size.y; controlMetrics.Client.Global.Offset.y = controlMetrics.Total.Global.Offset.y + ncSizes.Top ; } else { controlMetrics.Total.Global.Offset.y = otherMetrics.Total.Global.Offset.y + otherMetrics.Total.Global.Size.y; controlMetrics.Client.Global.Offset.y = controlMetrics.Total.Global.Offset.y + ncSizes.Top ; } }

	{ // calculate text rectangle
		const ::gpk::SRectangle2D<int32_t>									& targetRect											= controlMetrics.Client.Global;
		rectText.Offset													= targetRect.Offset.Cast<int16_t>();
			 if (::gpk::bit_true(controlText.Align, ::gpk::ALIGN_HCENTER)) { rectText.Offset.x = (int16_t)((targetRect.Offset.x + targetRect.Size.x / 2) - rectText.Size.x / 2	); }
		else if (::gpk::bit_true(controlText.Align, ::gpk::ALIGN_RIGHT	)) { rectText.Offset.x = (int16_t)((targetRect.Offset.x + targetRect.Size.x) - rectText.Size.x			); }
		
			 if (::gpk::bit_true(controlText.Align, ::gpk::ALIGN_VCENTER)) { rectText.Offset.y = (int16_t)((targetRect.Offset.y + targetRect.Size.y / 2) - rectText.Size.y / 2	); }
		else if (::gpk::bit_true(controlText.Align, ::gpk::ALIGN_BOTTOM	)) { rectText.Offset.y = (int16_t)((targetRect.Offset.y + targetRect.Size.y) - rectText.Size.y			); }
	}
	controlState.Outdated											= false;
	return 0;
}

static constexpr	const char									gpk_codepage_437_b64	[]								= "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";
// original static constexpr	const char									gpk_codepage_437_b64	[]								= "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";

static		::gpk::error_t										setupDefaultFontTexture									(::gpk::SGUI& gui)																						{
	//gui.FontTexture.resize(288, 128);
	gui.FontCharSize												= {9, 16};
	gui.FontTexture.resize(gui.FontCharSize.x, gui.FontCharSize.y * 256);
	::gpk::array_pod<ubyte_t>											decoded;
	gpk_necall(::gpk::base64Decode(gpk_codepage_437_b64, decoded), "Maybe the decode function got broken?");
	memcpy(gui.FontTexture.Texels.begin(), decoded.begin(), decoded.size());
	return 0;
}

static		::gpk::error_t										controlTextDraw											(::gpk::SGUI& gui, int32_t iControl, ::gpk::grid_view<::gpk::SColorBGRA>& target)						{
	::gpk::SControlText													& controlText											= gui.Controls.Text		[iControl];
	::gpk::SControlState												& controlState											= gui.Controls.States	[iControl];
	::gpk::SControlMetrics												& controlMetrics										= gui.Controls.Metrics	[iControl];
	if(0 == gui.FontTexture.Texels.size())
		gpk_necall(::setupDefaultFontTexture(gui), "Failed to set up default texture!");

	gpk_necall(0 == gui.FontTexture.Texels.size() || 0 == gui.FontTexture.Pitch, "Invalid font texture!");

	const ::gpk::SControl												& control												= gui.Controls.Controls	[iControl];
	const ::gpk::SControlTheme											& theme													= gui.ControlThemes[(0 == control.ColorTheme) ? gui.ThemeDefault : control.ColorTheme - 1];
	const ::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>	& colorCombo											= theme.ColorCombos
		[ controlState.Disabled	? ::gpk::GUI_CONTROL_STATE_COLORS_DISABLED
		: controlState.Pressed	? ::gpk::GUI_CONTROL_STATE_COLORS_PRESSED 
		: controlState.Selected	? ::gpk::GUI_CONTROL_STATE_COLORS_SELECTED
		: controlState.Hover	? ::gpk::GUI_CONTROL_STATE_COLORS_HOVER 
		: ::gpk::GUI_CONTROL_STATE_COLORS_NORMAL
		];
	const ::gpk::SColorBGRA												colorBack												= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND]];
	const ::gpk::SColorBGRA												colorFace												= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_TEXT_FACE]];
	const uint32_t														charsPerRow												= gui.FontTexture.Pitch / gui.FontCharSize.x;
	::gpk::SRectangle2D<int16_t>										rectText												= controlMetrics.Text;
	if(controlState.Pressed || controlState.Hover)
		rectText.Offset													+= ::gpk::SCoord2<int16_t>{1, 1};

	::gpk::array_pod<::gpk::SCoord2<uint32_t>>							dstCoords;
	for(uint32_t iChar = 0, countChars = (uint32_t)controlText.Text.size(); iChar < countChars; ++iChar) {
		char																charToDraw												= controlText.Text[iChar];
		const int32_t														coordTableX												= 0;//charToDraw % charsPerRow;//0;			//charToDraw % charsPerRow;
		const int32_t														coordTableY												= charToDraw;//charToDraw / charsPerRow;//charToDraw;	//charToDraw / charsPerRow;
		const ::gpk::SCoord2<int32_t>										coordCharTable											= ::gpk::SCoord2<uint32_t>{coordTableX * gui.FontCharSize.x, coordTableY * gui.FontCharSize.y}.Cast<int32_t>();
		const ::gpk::SRectangle2D<int32_t>									dstRect1												= {{(int32_t)(rectText.Offset.x + gui.FontCharSize.x * iChar), rectText.Offset.y}, gui.FontCharSize.Cast<int32_t>()};
		const ::gpk::SRectangle2D<int32_t>									srcRect0												= {coordCharTable, gui.FontCharSize.Cast<int32_t>()};
		//error_if(errored(::gpk::grid_copy_alpha_bit(target, gui.FontTexture.View, dstOffset1, {charsPerRow * gui.FontCharSize.x, 8 * gui.FontCharSize.y}, fontColor, srcRect0)), "I believe this never fails.");
		dstCoords.clear();
		//error_if(errored(::gpk::grid_raster_alpha_bit(target, gui.FontTexture.View, dstRect1.Offset, {charsPerRow * gui.FontCharSize.x, 256 * gui.FontCharSize.y}, srcRect0, dstCoords)), "I believe this never fails.");
		error_if(errored(::gpk::grid_raster_alpha_bit(target.metrics(), gui.FontTexture.View, {charsPerRow * gui.FontCharSize.x, 256U * gui.FontCharSize.y}, dstRect1, srcRect0.Offset, dstCoords)), "I believe this never fails.");
		for(uint32_t iCoord = 0; iCoord < dstCoords.size(); ++iCoord)
			::gpk::drawPixelLight(target, dstCoords[iCoord], colorFace, controlState.Pressed ? 0.75f : 0.5f, controlState.Pressed ? 1.0f : 0.95);
	}
	return 0;
}

static		::gpk::error_t										actualControlDraw										(::gpk::SGUI& gui, int32_t iControl, ::gpk::grid_view<::gpk::SColorBGRA>& target)					{
	const ::gpk::SControlMode											& mode													= gui.Controls.Modes	[iControl];
	const ::gpk::SControlState											& controlState											= gui.Controls.States	[iControl];
	if(mode.Design || controlState.Hidden)
		return 0;

	const ::gpk::SControl												& control												= gui.Controls.Controls	[iControl];
	::gpk::SColorBGRA													colors			[::gpk::GUI_CONTROL_AREA_COUNT]			= {}; // -- Fill color table
	::gpk::GUI_COLOR_MODE												colorMode												= (mode.ColorMode == ::gpk::GUI_COLOR_MODE_DEFAULT) ? gui.ColorModeDefault : mode.ColorMode;
	const ::gpk::SControlTheme											& theme													= gui.ControlThemes[(0 == control.ColorTheme) ? gui.ThemeDefault : control.ColorTheme - 1];
	const ::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>	& colorCombo											= theme.ColorCombos
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
	const ::gpk::SCoord2<double>										scaleFinal												= gui.Zoom.DPI * gui.Zoom.ZoomLevel;
	scaledBorders.Left												= (int32_t)(control.Border.Left		* scaleFinal.x);
	scaledBorders.Top												= (int32_t)(control.Border.Top		* scaleFinal.y);
	scaledBorders.Right												= (int32_t)(control.Border.Right	* scaleFinal.x);
	scaledBorders.Bottom											= (int32_t)(control.Border.Bottom	* scaleFinal.y);

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

	if(control.Image.metrics().LengthSquared())
		::gpk::grid_copy(target, control.Image, controlMetrics.Client.Global);

	::controlTextDraw(gui, iControl, target);
	return 0;
}

static		::gpk::error_t										controlUpdateMetricsTopToDown							(::gpk::SGUI& gui, int32_t iControl, const ::gpk::SCoord2<uint32_t> & targetSize, bool forceUpdate)				{
	gpk_necall(::controlUpdateMetrics(gui, iControl, targetSize, forceUpdate), "Unknown error! Maybe the control tree got broken?");
	::gpk::array_view<int32_t>											& children												= gui.Controls.Children[iControl];
	for(uint32_t iChild = 0; iChild < children.size(); ++iChild)
		gpk_necall(::controlUpdateMetricsTopToDown(gui, children[iChild], targetSize, forceUpdate), "Unknown error! Maybe the control tree got broken?");
	return 0;
}

			::gpk::error_t										gpk::guiUpdateMetrics									(::gpk::SGUI& gui, const ::gpk::SCoord2<uint32_t> & targetSize, bool forceUpdate)								{
	for(uint32_t iControl = 0; iControl < gui.Controls.Controls.size(); ++iControl)
		if(::controlInvalid(gui, gui.Controls.Controls[iControl].IndexParent) && false == ::controlInvalid(gui, iControl))
			gpk_necall(::controlUpdateMetricsTopToDown(gui, iControl, targetSize, forceUpdate), "Unknown error! Maybe the control tree got broken?");
	return 0;
}

			::gpk::error_t										gpk::controlUpdateMetrics								(::gpk::SGUI& gui, int32_t iControl, const ::gpk::SCoord2<uint32_t> & targetSize)								{
	gpk_necall(::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	gpk_necall(::controlUpdateMetricsTopToDown(gui, iControl, targetSize, true), "Unknown error! Maybe the control tree got broken?");
	return 0;
}

			::gpk::error_t										gpk::controlDrawHierarchy								(::gpk::SGUI& gui, int32_t iControl, ::gpk::grid_view<::gpk::SColorBGRA>& target)								{
	gpk_necall(::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	if(gui.LastSize != target.metrics()) {
		for(uint32_t iOutdated = 0; iOutdated < gui.Controls.Controls.size(); ++iOutdated)
			gui.Controls.States[iOutdated].Outdated							= true;
		gui.LastSize													= target.metrics();
	}
	if(false == gui.Controls.States[iControl].Hidden) {
		gpk_necall(::actualControlDraw(gui, iControl, target), "Unknown issue!");
		::gpk::array_view<int32_t>											& children												= gui.Controls.Children[iControl];
		for(uint32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild) 
			gpk_necall(::gpk::controlDrawHierarchy(gui, children[iChild], target), "Unknown issue!");
	}
	return 0;
}

static		::gpk::error_t										updateGUIControlHovered									(::gpk::SControlState& controlFlags, const ::gpk::SInput& inputSystem)								noexcept	{ 
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
	return one_if(controlFlags.Hover);
}

static		::gpk::error_t										controlProcessInput										(::gpk::SGUI& gui, ::gpk::SInput& input, int32_t iControl)														{
	::gpk::SControlState												& controlState											= gui.Controls.States[iControl];
	if(controlState.Hidden)
		return -1;
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

			::gpk::error_t										gpk::guiProcessInput									(::gpk::SGUI& gui, ::gpk::SInput& input)																		{
	error_if(errored(::gpk::guiUpdateMetrics(gui, gui.LastSize, false)), "Why would this ever happen?");
	gui.CursorPos													+= {(float)input.MouseCurrent.Deltas.x, (float)input.MouseCurrent.Deltas.y};
	::gpk::error_t														controlHovered											= -1;

	::gpk::array_pod<uint32_t>											rootControlsToProcess									= {};
	for(uint32_t iControl = 0, countControls = gui.Controls.Controls.size(); iControl < countControls; ++iControl) {	// Only process root parents
		::gpk::SControlState												& controlState											= gui.Controls.States[iControl];
		if(controlState.Unused || controlState.Disabled)
			continue;
		if (controlState.Execute)	// EXECUTE only lasts one tick.
			controlState.Execute											= false;
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
			gui.Controls.States[iControl].Hover								= false;
			if(0 == input.MouseCurrent.ButtonState[0])
				gui.Controls.States[iControl].Pressed							= false;
		}
		//else {
		//	verbose_printf("Hovered: %u.", iControl);
		//}
	}
	return controlHovered;
}

			::gpk::error_t										gpk::guiDraw											(::gpk::SGUI& gui, ::gpk::grid_view<::gpk::SColorBGRA>& target)													{
	if(gui.LastSize != target.metrics()) {
		for(uint32_t iOutdated = 0; iOutdated < gui.Controls.Controls.size(); ++iOutdated)
			gui.Controls.States[iOutdated].Outdated							= true;
		gui.LastSize													= target.metrics();
	}
	gpk_necall(::gpk::guiUpdateMetrics(gui, gui.LastSize, false), "Unknown issue!");;
	for(uint32_t iControl = 0; iControl < gui.Controls.Controls.size(); ++iControl)
		if(false == ::controlInvalid(gui, iControl) && ::controlInvalid(gui, gui.Controls.Controls[iControl].IndexParent))
			error_if(errored(::gpk::controlDrawHierarchy(gui, iControl, target)), "Unknown issue!");
	return 0;
}

			::gpk::error_t										gpk::controlHidden										(::gpk::SGUI& gui, int32_t iControl)	{
	bool																imHidden												= ::controlInvalid(gui, iControl) || gui.Controls.States[iControl].Hidden;
	return imHidden ? imHidden : (false == ::controlInvalid(gui, gui.Controls.Controls[iControl].IndexParent) && ::gpk::controlHidden(gui, gui.Controls.Controls[iControl].IndexParent));
}

			::gpk::error_t										gpk::controlMetricsInvalidate							(::gpk::SGUI& gui, int32_t iControl)	{
	::gpk::SControlState												& controlState											= gui.Controls.States[iControl];
	controlState.Outdated											= true;
	const ::gpk::array_view<int32_t>									& controlChildren										= gui.Controls.Children[iControl];
	for(uint32_t iChild = 0, countChild = controlChildren.size(); iChild < countChild; ++iChild) 
		gpk_necall(::gpk::controlMetricsInvalidate(gui, controlChildren[iChild]), "Invalid child?");
	return 0;
}

			::gpk::error_t										gpk::guiGetProcessableControls							(::gpk::SGUI& gui, ::gpk::array_pod<uint32_t>& controlIndices)													{
	for(uint32_t iControl = 0, countControls = gui.Controls.Controls.size(); iControl < countControls; ++iControl) {	// Only process root parents
		const ::gpk::SControlState											& controlState											= gui.Controls.States[iControl];
		if(controlState.Unused || controlState.Disabled || ::gpk::controlHidden(gui, iControl))
			continue;
		gpk_necall(controlIndices.push_back(iControl), "Out of memory?");
	}
	return controlIndices.size();
}
