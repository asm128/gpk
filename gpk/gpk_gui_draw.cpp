#include "gpk_gui.h"
#include "gpk_grid_copy.h"
#include "gpk_encoding.h"
#include "gpk_bitmap_target.h"

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
static constexpr	const char									gpk_codepage_437_b64	[]								= "AAAAAAAAAAAAAAAAAAAAAAAAAAD4CVQqUKBemQIF8gMAAAAAAAD4+bft379h5/798wMAAAAAAAAAAGDjz58/f3xwQAAAAAAAAAAAAICAg48/PjggAAAAAAAAAAAAwMCDx7lz5zBg4AEAAAAAAAAAwMDDz79/fjBg4AEAAAAAAAAAAAAAAAYePDAAAAAAAAAA//79+/fv37lhw879+/fv379/AAAAAACAhxkhQszwAAAAAAAA//79+/dvWKZevTIN+/fv379/AADggYOFiYcZM2bM8AAAAAAAAADwMGPGjBkeGPxgwAAAAAAAAADwY8aPAQMGDBw8OAAAAAAAAAD4M+bPmDFjxsydOzMAAAAAAAAAwIBhG49zPLZhwAAAAAAAAAIMOPDgw58PDw4MCAAAAAAAAICAgYOHzx8+eOCAAQIAAAAAAABg4OEHAwYMfnhgAAAAAAAAAACYMWPGjBkzZgCYMQMAAAAAAAD427Ztmzds2LBhwwYAAAAAAHyMMcDBxpgxNjjAGOMDAAAAAAAAAAAAAAAAf/78+QMAAAAAAABg4OEHAwYMfnhg8AMAAAAAAABg4OEHAwYMGDBgwAAAAAAAAABgwIABAwYMGPzwwAAAAAAAAAAAAAAAA4w/MDAAAAAAAAAAAAAAAACAgYE/BhgAAAAAAAAAAAAAAAAAwIABA/4AAAAAAAAAAAAAAACAhJl/ZkgAAAAAAAAAAAAAAICAAwcfPv78AQAAAAAAAAAAAPDnjw8fHDggAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABg4MGDBwYMGABgwAAAAAAAAMyYMUMCAAAAAAAAAAAAAAAAAAAAsGHjjw0bNv7YsAEAAAAAGDD4GDNkgA8wYMKM8YEBAwAAAAAAADBkDAwMDAyMCQMAAAAAAABwsGGDg5sdM2bMcAMAAAAAABgwYGAAAAAAAAAAAAAAAAAAAADAwMCAAQMGDBhggAEAAAAAAAAwwAADBgwYMGBgYAAAAAAAAAAAAADADI9/PMwAAAAAAAAAAAAAAAAAAwY/GDAAAAAAAAAAAAAAAAAAAAAAADBgwMAAAAAAAAAAAAAAAIA/AAAAAAAAAAAAAAAAAAAAAAAAAABgwAAAAAAAAAAAAAAEDAwMDAwMCAAAAAAAAADwMDNs2LZtw4aZ4QEAAAAAAABg4OABAwYMGDBg8AMAAAAAAAD4GAMGBgYGBgaM+QMAAAAAAAD4GAMGDA8wYMCM8QEAAAAAAADAwMHDxow/MGDAwAMAAAAAAAD8GTBgwA8wYMCM8QEAAAAAAABwMDBgwI8xY8aM8QEAAAAAAAD8GQMGDAwMDBgwYAAAAAAAAAD4GDNmjI8xY8aM8QEAAAAAAAD4GDNmjB8wYMDA8AAAAAAAAAAAAIABAwAAADBgAAAAAAAAAAAAAIABAwAAADBgYAAAAAAAAAAAAAMDAwMDDDDAAAMAAAAAAAAAAADADwAAfgAAAAAAAAAAAAAAMMAAAwwwMDAwMAAAAAAAAAD4GDMGBgYMGABgwAAAAAAAAAAA8DFmzJ49e3YM8AEAAAAAAAAg4GBjzJg/Y8aMGQMAAAAAAAD8MGPGjA8zZsyY+QEAAAAAAADwMDNkwIABA4aY4QEAAAAAAAB8sGHGjBkzZszY+AAAAAAAAAD8MWPEggcLBoyY+QMAAAAAAAD8MWPEggcLBgwYeAAAAAAAAADwMDNkwIA9Y8aY4QIAAAAAAACMGTNmzJ8xY8aMGQMAAAAAAADwwIABAwYMGDBg4AEAAAAAAADggQEDBgwYM2bM8AAAAAAAAACcMWPGhgcPNsyYOQMAAAAAAAA8MGDAgAEDBoyY+QMAAAAAAAAMO/fv37Zhw4YNGwYAAAAAAACMOfPmz545Y8aMGQMAAAAAAAD4GDNmzJgxY8aM8QEAAAAAAAD8MGPGjA8DBgwYeAAAAAAAAAD4GDNmzJgxY9bs8QEDDgAAAAD8MGPGjA8bZsyYOQMAAAAAAAD4GDPGAAcYYMaM8QEAAAAAAAD825YJAwYMGDBg4AEAAAAAAACMGTNmzJgxY8aM8QEAAAAAAAAMGzZs2LBhw8zwwAAAAAAAAAAMGzZs2LBt2/6ZMQMAAAAAAAAMG2aGBwYMPMwMGwYAAAAAAAAMGzbMDA8MGDBg4AEAAAAAAAD8GxYGBgYGBgYN+wcAAAAAAADwYMCAAQMGDBgw4AEAAAAAAAAACDDggAMOOOCAAQIAAAAAAADwgAEDBgwYMGDA4AEAAAAACDjYGAMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADgHwAADBhgAAAAAAAAAAAAAAAAAAAAAAAAAADAAwwfM2bMcAMAAAAAAAAcMGDAgw0zZsyY8QEAAAAAAAAAAADAx5gBAwaM8QEAAAAAAADggAGDh40ZM2bMcAMAAAAAAAAAAADAx5g/AwaM8QEAAAAAAABwsGHCwAMDBgwYeAAAAAAAAAAAAADAzYwZM2bM8AFjhgcAAAAcMGDAhhszZsyYOQMAAAAAAABgwACAAwYMGDBg4AEAAAAAAACAAQMADhgwYMCAAWPGDA8AAAAcMGDAjA0PHmyYOQMAAAAAAABwwIABAwYMGDBg4AEAAAAAAAAAAADgzL9t27Zt2wYAAAAAAAAAAABghxkzZsyYMQMAAAAAAAAAAADAx5gxY8aM8QEAAAAAAAAAAABghxkzZsyY8WHAwAMAAAAAAADAzYwZM2bM8AEDBh4AAAAAAABghxszBgwYeAAAAAAAAAAAAADAxxgDHGCM8QEAAAAAAAAgYMDgBwMGDBiwwQEAAAAAAAAAAABgxowZM2bMcAMAAAAAAAAAAABg2LBhw8zwwAAAAAAAAAAAAABg2LBh27b9MwMAAAAAAAAAAABgmBkeGHiYGQYAAAAAAAAAAABgzJgxY8aM8QMGxgcAAAAAAADgzwwMDAyM+QMAAAAAAADAwYABgwMMGDBggAMAAAAAAABgwIABAwAMGDBgwAAAAAAAAAA4wIABAxwMGDBgcAAAAAAAAAC42QEAAAAAAAAAAAAAAAAAAAAAAICAg40xY8b8AQAAAAAAAADwMDNkwIABQ8zwgAHGBwAAAADMAABgxowZM2bMcAMAAAAAAGBgYADAx5g/AwaM8QEAAAAAABBwsAHAAwwfM2bMcAMAAAAAAADMAADAAwwfM2bMcAMAAAAAAAwwwADAAwwfM2bMcAMAAAAAADjY4ADAAwwfM2bMcAMAAAAAAAAAAMDDjAEDZnjAAMMDAAAAABBwsAHAx5g/AwaM8QEAAAAAAACMAQDAx5g/AwaM8QEAAAAAAAwwwADAx5g/AwaM8QEAAAAAAACYAQCAAwYMGDBg4AEAAAAAADDwMAOAAwYMGDBg4AEAAAAAAAwwwACAAwYMGDBg4AEAAAAAAMYAQMDBxpgxf8aMGQMAAAAAHGxwAMDBxpgxf8aMGQMAAAAAGBgYAPDHjAEfBgyY+QMAAAAAAAAAAADADjdsfjbscAcAAAAAAADwsTFjxp8ZM2bMmAMAAAAAABBwsAHAx5gxY8aM8QEAAAAAAACMAQDAx5gxY8aM8QEAAAAAAAwwwADAx5gxY8aM8QEAAAAAABh4mAFgxowZM2bMcAMAAAAAAAwwwABgxowZM2bMcAMAAAAAAACMAQBgzJgxY8aM8QMGhgcAAMYA8DFmzJgxY8aM8QEAAAAAAMYAGDNmzJgxY8aM8QEAAAAAADBg8DNswIABw/xgwAAAAAAAADjYMGHggQEDBgyc+QEAAAAAAAAMM8MDwz8M/zBgwAAAAAAAAH6YMePDiBl7ZsyYeQYAAAAAAOBgw4ABgx8MGDBgwLDBAQAAADAwMADAAwwfM2bMcAMAAAAAAGBgYACAAwYMGDBg4AEAAAAAADAwMADAx5gxY8aM8QEAAAAAADAwMABgxowZM2bMcAMAAAAAAAC42QFghxkzZsyYMQMAAAAAbnYAGHPmzZ89c8aMGQMAAAAAAHjYsMEHgB8AAAAAAAAAAAAAADjYsMEBgA8AAAAAAAAAAAAAAAAwYACAAQMDA8aM8QEAAAAAAAAAAAAAwJ8BAwYMAAAAAAAAAAAAAAAAwB8wYMCAAQAAAAAAAAYMGDJmBgYGBuZkAwMDHwAAAAYMGDJmBgYGZuak4QMGDAAAAABgwAAAAwYMPHjwwAAAAAAAAAAAAACAjY0NNtgAAAAAAAAAAAAAAABggw02NjYAAAAAAAAAiEUgFoFYBGIRiEUgFoFYBGIRqquorqK6iuoqqquorqK6iuoqu93vdr/b/W73u93vdr/b/W73GDBgwIABAwYMGDBgwIABAwYMGDBgwIABA4YPGDBgwIABAwYMGDBgwIDhA4YPGDBgwIABAwYMbNiwYcOGDZs3bNiwYcOGDRs2AAAAAAAAAIA/bNiwYcOGDRs2AAAAAADgA4YPGDBgwIABAwYMbNiwYcPmDZg3bNiwYcOGDRs2bNiwYcOGDRs2bNiwYcOGDRs2AAAAAADgD5g3bNiwYcOGDRs2bNiwYcPmDZg/AAAAAAAAAAAAbNiwYcOGDZs/AAAAAAAAAAAAGDBgwIDhA4YPAAAAAAAAAAAAAAAAAAAAAIAPGDBgwIABAwYMGDBgwIABAwb8AAAAAAAAAAAAGDBgwIABA4b/AAAAAAAAAAAAAAAAAAAAAID/GDBgwIABAwYMGDBgwIABAwb8GDBgwIABAwYMAAAAAAAAAID/AAAAAAAAAAAAGDBgwIABA4b/GDBgwIABAwYMGDBgwIABPwb8GDBgwIABAwYMbNiwYcOGDRv2bNiwYcOGDRs2bNiwYcOGPQP+AAAAAAAAAAAAAAAAAACAPwP2bNiwYcOGDRs2bNiwYcPmPYD/AAAAAAAAAAAAAAAAAADgP4D3bNiwYcOGDRs2bNiwYcOGPQP2bNiwYcOGDRs2AAAAAADgP4D/AAAAAAAAAAAAbNiwYcPmPYD3bNiwYcOGDRs2GDBgwIDhP4D/AAAAAAAAAAAAbNiwYcOGDZv/AAAAAAAAAAAAAAAAAADgP4D/GDBgwIABAwYMAAAAAAAAAID/bNiwYcOGDRs2bNiwYcOGDRv+AAAAAAAAAAAAGDBgwIABPwb8AAAAAAAAAAAAAAAAAAAAPwb8GDBgwIABAwYMAAAAAAAAAAD+bNiwYcOGDRs2bNiwYcOGDZv/bNiwYcOGDRs2GDBgwIDhP4b/GDBgwIABAwYMGDBgwIABA4YPAAAAAAAAAAAAAAAAAAAAAAD8GDBgwIABAwYM////////////////////////AAAAAAAAAID/////////////Dx48ePDgwYMHDx48ePDgwYMH8OHDhw8fPnz48OHDhw8fPnz4/////////38AAAAAAAAAAAAAAAAAAADAzY4NGzbscAMAAAAAAAB4mDFjxoYZY8aMmQEAAAAAAAD8GTNmwIABAwYMGAAAAAAAAAAAAPDHhg0bNmzYsAEAAAAAAAAA+DPGAAMMDAyM+QMAAAAAAAAAAADAz4YNGzZscAAAAAAAAAAAAGDGjBkzZnwYMDAAAAAAAAAAAOBmBwYMGDBgwAAAAAAAAAAA8IOBhxkzZnhg8AMAAAAAAAAA4GBjzJg/Y8bY4AAAAAAAAABwsDFmzBgbNmzYuAMAAAAAAADgYYABBh8zZsyY4QEAAAAAAAAAAADAz7Zt2/wAAAAAAAAAAAAAAAbGz7Ztz/wYGAAAAAAAAADgYGDAgA8DBgwwwAEAAAAAAAAA8DFmzJgxY8aMGQMAAAAAAAAAAPAHAIA/AAD8AQAAAAAAAAAAAIABgx8MGAAA+AcAAAAAAAAAYIABBhgYGBgA8AMAAAAAAAAAgIGBgQEGGGAA8AMAAAAAAADAwYYNAwYMGDBgwIABAwYMGDBgwIABAwYMGzZscAAAAAAAAAAAAIABAwA/ADBgAAAAAAAAAAAAAADAzQ4AbnYAAAAAAAAAADjYsMEBAAAAAAAAAAAAAAAAAAAAAAAAAAAMGAAAAAAAAAAAAAAAAAAAAAAAGAAAAAAAAAAAAODBgAEDBowbNmzwwAEAAAAAADbYsGHDhg0AAAAAAAAAAAAAABxsYGBgwgcAAAAAAAAAAAAAAAAAAODDhw8fPnz4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAzc3NzQ==";
static		::gpk::error_t										setupDefaultFontTexture									(::gpk::SGUI& gui)																						{
	gui.FontCharSize												= {9, 16};
	gui.FontTexture.resize(gui.FontCharSize.x, gui.FontCharSize.y * 256);
	::gpk::array_pod<ubyte_t>											decoded;
	gpk_necall(::gpk::base64Decode(gpk_codepage_437_b64, decoded), "%s", "Maybe the decode function got broken?");
	memcpy(gui.FontTexture.Texels.begin(), decoded.begin(), decoded.size());
	return 0;
}
static		::gpk::error_t										controlTextDraw											(::gpk::SGUI& gui, int32_t iControl, ::gpk::view_grid<::gpk::SColorBGRA>& target, bool bDisabled)				{
	if(0 == gui.FontTexture.Texels.size())
		gpk_necall(::setupDefaultFontTexture(gui), "%s", "Failed to set up default texture!");

	gpk_necall(0 == gui.FontTexture.Texels.size() || 0 == gui.FontTexture.Pitch, "%s", "Invalid font texture!");

	const ::gpk::SControl												& control												= gui.Controls.Controls	[iControl];
	::gpk::SControlState												& controlState											= gui.Controls.States	[iControl];
	::gpk::SColorBGRA													colorFace												= {0xFF, 0x00, 0xFF, 0xFF};
	if(0 == gui.Controls.Modes[iControl].UseNewPalettes) {
		const ::gpk::SControlTheme											& theme													= gui.ControlThemes[(0 == control.ColorTheme) ? gui.ThemeDefault : control.ColorTheme - 1];
		const ::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>	& colorCombo											= theme.ColorCombos
			[ bDisabled				? ::gpk::GUI_CONTROL_PALETTE_DISABLED
			: controlState.Pressed	? ::gpk::GUI_CONTROL_PALETTE_PRESSED
			: controlState.Selected	? ::gpk::GUI_CONTROL_PALETTE_SELECTED
			: controlState.Hover	? ::gpk::GUI_CONTROL_PALETTE_HOVER
			: ::gpk::GUI_CONTROL_PALETTE_NORMAL
			];
		colorFace														= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_TEXT_FACE]];
		//const ::gpk::SColorBGRA												colorBack												= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_TEXT_BACKGROUND]];
	}
	else {
		const ::gpk::array_static<::gpk::SColorBGRA, ::gpk::GUI_CONTROL_COLOR_COUNT>	& colorCombo											=
			gui.Palettes[control.Palettes
				[ bDisabled				? ::gpk::GUI_CONTROL_PALETTE_DISABLED
				: controlState.Pressed	? ::gpk::GUI_CONTROL_PALETTE_PRESSED
				: controlState.Selected	? ::gpk::GUI_CONTROL_PALETTE_SELECTED
				: controlState.Hover	? ::gpk::GUI_CONTROL_PALETTE_HOVER
				: ::gpk::GUI_CONTROL_PALETTE_NORMAL
				]]
				;
		colorFace														= colorCombo[::gpk::GUI_CONTROL_COLOR_TEXT_FACE];
	}

	::gpk::SControlMetrics												& controlMetrics										= gui.Controls.Metrics	[iControl];
	::gpk::SRectangle2D<int16_t>										rectText												= controlMetrics.Text;
	if(false == gui.Controls.Modes[iControl].NoHoverEffect && (controlState.Pressed || controlState.Hover)) { // shift text offset by one pixel to give the effect of being pushed.
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
static		::gpk::GUI_CONTROL_PALETTE							paletteIndexFromState									(bool disabled, const ::gpk::SControlState& controlState)					{
	return
		disabled				? ::gpk::GUI_CONTROL_PALETTE_DISABLED
		: controlState.Pressed	? ::gpk::GUI_CONTROL_PALETTE_PRESSED
		: controlState.Selected	? ::gpk::GUI_CONTROL_PALETTE_SELECTED
		: controlState.Hover	? ::gpk::GUI_CONTROL_PALETTE_HOVER
		: ::gpk::GUI_CONTROL_PALETTE_NORMAL
		;
}
static		::gpk::error_t										fillColorTableBorders3D									(const ::gpk::SControlState& controlState, const ::gpk::SControlMode& controlMode, ::gpk::view_array<::gpk::SColorBGRA>& colors)					{ 
	double																bright;
	double																shaded;
	if(controlMode.NoHoverEffect) {
		bright															= controlMode.FrameOut ? 1.2 : 0.8;
		shaded															= controlMode.FrameOut ? 0.8 : 1.2;
	}
	else {
		bright															= controlState.Pressed ? 0.4 : controlState.Hover ? 0.8 : 1.2;
		shaded															= controlState.Pressed ? 1.5 : controlState.Hover ? 1.2 : 0.8;
	}
	::gpk::SColorBGRA													colorBackground											= colors[::gpk::GUI_CONTROL_AREA_BACKGROUND];
	colors[::gpk::GUI_CONTROL_AREA_BORDER_LEFT		]				= colorBackground * bright;
	colors[::gpk::GUI_CONTROL_AREA_BORDER_TOP		]				= colorBackground * bright;
	colors[::gpk::GUI_CONTROL_AREA_BORDER_RIGHT		]				= colorBackground * shaded;
	colors[::gpk::GUI_CONTROL_AREA_BORDER_BOTTOM	]				= colorBackground * shaded;
	return 0;
}
static		::gpk::error_t										fillColorTableNew										(::gpk::SGUI& gui, ::gpk::GUI_COLOR_MODE colorMode, const ::gpk::SControl& control, const ::gpk::SControlState& controlState, const ::gpk::SControlMode& controlMode, bool disabled, ::gpk::view_array<::gpk::SColorBGRA>& colors)					{ 
	const ::gpk::array_static<::gpk::SColorBGRA, ::gpk::GUI_CONTROL_COLOR_COUNT>	
																		& colorCombo											= gui.Palettes[control.Palettes[::paletteIndexFromState(disabled, controlState)]];
	colors[::gpk::GUI_CONTROL_AREA_BACKGROUND		]				= colorCombo[::gpk::GUI_CONTROL_COLOR_BACKGROUND	];
	colors[::gpk::GUI_CONTROL_AREA_CLIENT			]				= colorCombo[::gpk::GUI_CONTROL_COLOR_CLIENT		];
	if(colorMode == ::gpk::GUI_COLOR_MODE_3D || colorMode == ::gpk::GUI_COLOR_MODE_DEFAULT) 
		return ::fillColorTableBorders3D(controlState, controlMode, colors);

	colors[::gpk::GUI_CONTROL_AREA_BORDER_LEFT		]				= colorCombo[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT	];
	colors[::gpk::GUI_CONTROL_AREA_BORDER_TOP		]				= colorCombo[::gpk::GUI_CONTROL_COLOR_BORDER_TOP	];
	colors[::gpk::GUI_CONTROL_AREA_BORDER_RIGHT		]				= colorCombo[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT	];
	colors[::gpk::GUI_CONTROL_AREA_BORDER_BOTTOM	]				= colorCombo[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM	];
	return 0;
}
static		::gpk::error_t										fillColorTableOld										(::gpk::SGUI& gui, ::gpk::GUI_COLOR_MODE colorMode, const ::gpk::SControl& control, const ::gpk::SControlState& controlState, const ::gpk::SControlMode& controlMode, bool disabled, ::gpk::view_array<::gpk::SColorBGRA>& colors)					{ 
	const ::gpk::SControlTheme											& theme													= gui.ControlThemes[(0 == control.ColorTheme) ? gui.ThemeDefault : control.ColorTheme - 1];
	const ::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>	& colorCombo											= theme.ColorCombos[::paletteIndexFromState(disabled, controlState)];
	colors[::gpk::GUI_CONTROL_AREA_BACKGROUND			]			= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]];
	colors[::gpk::GUI_CONTROL_AREA_CLIENT				]			= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_CLIENT		]];
	if(colorMode == ::gpk::GUI_COLOR_MODE_3D || colorMode == ::gpk::GUI_COLOR_MODE_DEFAULT) 
		return ::fillColorTableBorders3D(controlState, controlMode, colors);

	colors[::gpk::GUI_CONTROL_AREA_BORDER_LEFT		]				= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_BORDER_LEFT	]];
	colors[::gpk::GUI_CONTROL_AREA_BORDER_TOP		]				= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_BORDER_TOP	]];
	colors[::gpk::GUI_CONTROL_AREA_BORDER_RIGHT		]				= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_BORDER_RIGHT	]];
	colors[::gpk::GUI_CONTROL_AREA_BORDER_BOTTOM	]				= gui.Palette[colorCombo[::gpk::GUI_CONTROL_COLOR_BORDER_BOTTOM	]];
	return 0;
}
static		::gpk::error_t										fillColorTable											(::gpk::SGUI& gui, int32_t iControl, bool disabled, ::gpk::view_array<::gpk::SColorBGRA> colors)					{
	const ::gpk::SControl												& control												= gui.Controls.Controls	[iControl];
	const ::gpk::SControlMode											& mode													= gui.Controls.Modes	[iControl];
	const ::gpk::SControlState											& controlState											= gui.Controls.States	[iControl];
	::gpk::GUI_COLOR_MODE												colorMode												= (mode.ColorMode == ::gpk::GUI_COLOR_MODE_DEFAULT) ? gui.ColorModeDefault : mode.ColorMode;
	if(mode.UseNewPalettes) 
		fillColorTableNew(gui, colorMode, control, controlState, mode, disabled, colors);
	else 
		fillColorTableOld(gui, colorMode, control, controlState, mode, disabled, colors);
	return 0;
}
static		::gpk::error_t										buildControlGeometry									(const ::gpk::SControl & control, const ::gpk::SControlMetrics & controlMetrics, const ::gpk::SGUIZoom& zoom, ::gpk::view_array<::gpk::SRectangle2D<int32_t>> finalRects, ::gpk::view_array<::gpk::STriangle2D<int32_t>> triangles)					{
	::gpk::SRectLimits<int32_t>											scaledBorders											= {};
	const ::gpk::SCoord2<double>										scaleFinal												= zoom.DPI * zoom.ZoomLevel;
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

	// --- Draw control corners
	::gpk::SCoord2<int32_t>												startOffset												= controlMetrics.Total.Global.Offset;
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
	return 0;
}
static		::gpk::error_t										actualControlDraw										(::gpk::SGUI& gui, int32_t iControl, ::gpk::view_grid<::gpk::SColorBGRA>& target)					{
	const ::gpk::SControl												& control												= gui.Controls.Controls	[iControl];
	::gpk::SColorBGRA													colors			[::gpk::GUI_CONTROL_AREA_COUNT]			= {}; // -- Fill color table
	const bool															disabled												= ::gpk::controlDisabled(gui, iControl);
	::fillColorTable(gui, iControl, disabled, colors);
	::gpk::STriangle2D<int32_t>											triangles	[8]											= {};
	::gpk::SRectangle2D<int32_t>										rectangles	[::gpk::GUI_CONTROL_AREA_COUNT]				= {};
	const ::gpk::SControlMetrics										& controlMetrics										= gui.Controls.Metrics[iControl];
	::buildControlGeometry(control, controlMetrics, gui.Zoom, rectangles, triangles);

	for(uint32_t iElement = 0; iElement < ::gpk::GUI_CONTROL_AREA_COUNT; ++iElement)
		if(iElement != ::gpk::GUI_CONTROL_AREA_CLIENT)
			::gpk::drawRectangle(target, colors[iElement], rectangles[iElement]);

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

	if(control.Image.metrics().x > 0 && control.Image.metrics().y > 0) {
		const ::gpk::SControlState										& state								= gui.Controls.States[iControl];
		::gpk::SRectangle2D<int32_t>									rectImage							=
			{ controlMetrics.Client.Global.Offset
			, controlMetrics.Client.Global.Size 
				+ ::gpk::SCoord2<int32_t>
					{ ::gpk::min(0, ::gpk::min(controlMetrics.Client.Global.Offset.x, controlMetrics.Client.Local.Offset.x))
					, ::gpk::min(0, ::gpk::min(controlMetrics.Client.Global.Offset.y, controlMetrics.Client.Local.Offset.y))
					}
			};
		if(0 == state.ImageInvertY)
			::gpk::grid_copy_blend(target, control.Image, rectImage);
		else {
			::gpk::SImage<::gpk::SColorBGRA>								& inverted							= gui.Controls.Images[iControl].Temp;
			inverted.resize(control.Image.metrics());
			::gpk::grid_mirror_y(inverted.View, control.Image);
			::gpk::grid_copy_blend(target, inverted.View, rectImage);
		}
	}

	error_if(errored(::controlTextDraw(gui, iControl, target, disabled)), "%s", "Why would this ever happen?");
	return 0;
}
			::gpk::error_t										gpk::controlDrawHierarchy								(::gpk::SGUI& gui, int32_t iControl, ::gpk::view_grid<::gpk::SColorBGRA>& target)								{
	gpk_necall(::gpk::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	if(gui.LastSize != target.metrics()) {
		for(uint32_t iOutdated = 0; iOutdated < gui.Controls.Controls.size(); ++iOutdated)
			gui.Controls.States[iOutdated].Updated							= false;
		gui.LastSize													= target.metrics();
	}
	if(false == ::gpk::controlHidden(gui, iControl)) {
		::gpk::SControlMode													controlModes										= gui.Controls.Modes[iControl];
		if(false == controlModes.Design)
			gpk_necall(::actualControlDraw(gui, iControl, target), "%s", "Unknown issue!");
		::gpk::view_array<int32_t>											& children												= gui.Controls.Children[iControl];
		for(uint32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
			gpk_necall(::gpk::controlDrawHierarchy(gui, children[iChild], target), "%s", "Unknown issue!");
	}
	return 0;
}
			::gpk::error_t										gpk::guiDraw											(::gpk::SGUI& gui, ::gpk::view_grid<::gpk::SColorBGRA>& target)													{
	if(gui.LastSize != target.metrics() || gui.LastZoom != gui.Zoom) {
		for(uint32_t iOutdated = 0; iOutdated < gui.Controls.Controls.size(); ++iOutdated)
			gui.Controls.States[iOutdated].Updated							= false;
		gui.LastSize													= target.metrics();
		gui.LastZoom													= gui.Zoom;
	}
	gpk_necall(::gpk::guiUpdateMetrics(gui, gui.LastSize, false), "%s", "Unknown issue!");;
	for(uint32_t iControl = 0; iControl < gui.Controls.Controls.size(); ++iControl)
		if(false == ::gpk::controlInvalid(gui, iControl) && ::gpk::controlInvalid(gui, gui.Controls.Controls[iControl].IndexParent))
			error_if(errored(::gpk::controlDrawHierarchy(gui, iControl, target)),"%s",  "Unknown issue!");
	return 0;
}
