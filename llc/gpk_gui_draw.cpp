#include "gpk_gui.h"

#include "gpk_grid_copy.h"
#include "gpk_bitmap_target.h"
#include "gpk_gui_text.h"
#include "gpk_rect_align.h"

static	::gpk::error_t	controlTextDraw		(::gpk::SGUI & gui, int32_t iControl, ::gpk::v2<::gpk::bgra> & target, bool bDisabled)				{
	const ::gpk::SControl		& control			= gui.Controls.Controls	[iControl];
	::gpk::SControlState		& controlState		= gui.Controls.States	[iControl];
	::gpk::bgra					colorFace			= {0xFF, 0x00, 0xFF, 0xFF};
	if(0 == gui.Controls.Draw[iControl].UseNewPalettes) {
		const ::gpk::SControlTheme	& theme				= (*gui.Colors->ControlThemes)[(0 == control.ColorTheme) ? gui.ThemeDefault : control.ColorTheme - 1];
		const ::gpk::astu32<::gpk::UI_CONTROL_AREA_COUNT>	
									& colorCombo		= theme.ColorCombos
			[ bDisabled				? ::gpk::GUI_CONTROL_PALETTE_DISABLED
			: controlState.Pressed	? ::gpk::GUI_CONTROL_PALETTE_PRESSED
			: controlState.Selected	? ::gpk::GUI_CONTROL_PALETTE_SELECTED
			: controlState.Hovered	? ::gpk::GUI_CONTROL_PALETTE_HOVER
			: ::gpk::GUI_CONTROL_PALETTE_NORMAL
			];
		colorFace				= (*gui.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_TEXT_FACE]];
		//const ::gpk::bgra			colorBack			= gui.Palette[colorCombo[::gpk::UI_CONTROL_AREA_TEXT_BACKGROUND]];
	}
	else {
		const ::gpk::astatic<::gpk::bgra, ::gpk::UI_CONTROL_AREA_COUNT>	& colorCombo											=
			gui.Colors->Palettes[control.Palettes
				[ bDisabled				? ::gpk::GUI_CONTROL_PALETTE_DISABLED
				: controlState.Pressed	? ::gpk::GUI_CONTROL_PALETTE_PRESSED
				: controlState.Selected	? ::gpk::GUI_CONTROL_PALETTE_SELECTED
				: controlState.Hovered	? ::gpk::GUI_CONTROL_PALETTE_HOVER
				: ::gpk::GUI_CONTROL_PALETTE_NORMAL
				]]
				;
		colorFace				= colorCombo[::gpk::UI_CONTROL_AREA_TEXT_FACE];
	}

	::gpk::SControlMetrics		& controlMetrics	= gui.Controls.Metrics	[iControl];
	::gpk::rect2i16				rectText			= controlMetrics.Text;
	if(false == gui.Controls.Modes[iControl].NoHover && (controlState.Pressed || controlState.Hovered)) { // shift text offset by one pixel to give the effect of being pushed.
		rectText.Offset			+= ::gpk::n2<int16_t>{1, 1};
		rectText.Size			-= ::gpk::n2<int16_t>{1, 1};
		if(rectText.Size.x < 0) rectText.Size.x = 0;
		if(rectText.Size.y < 0) rectText.Size.y = 0;
	} // This isn't done in updateMetrics() because the function is only executed for outdated metrics.
	// Changhing the state from idle to hover however doesn't cause the control metrics to become outdated (and in general it's pointless for other than the effect we're applying here).

	::gpk::SControlText			& controlText		= gui.Controls.Text		[iControl];
	::gpk::apod<::gpk::n2u16>	dstCoords;
	const uint32_t				iFont				= ::gpk::in_range(controlText.FontSelected, (int16_t)0, (int16_t)gui.Fonts.size()) ? controlText.FontSelected : gui.SelectedFont;
	const ::gpk::SRasterFont	& selectedFont		= *gui.Fonts[iFont];
	gpk_necs(::gpk::textLineRaster(target.metrics(), selectedFont.CharSize, rectText, selectedFont.Texture, controlText.Text, dstCoords));
	for(uint32_t iCoord = 0; iCoord < dstCoords.size(); ++iCoord) {
		const ::gpk::n2u16			dstCoord			= dstCoords[iCoord];
		target[dstCoord.y][dstCoord.x]	= colorFace;
		//::gpk::drawPixelLight(target, dstCoords[iCoord], colorFace, controlState.Pressed ? 0.75f : 0.5f, controlState.Pressed ? 1.0f : 0.95);
	}
	return 0;
}
static	::gpk::GUI_CONTROL_PALETTE	paletteIndexFromState	(bool disabled, const ::gpk::SControlState & controlState)					{
	return
		disabled				? ::gpk::GUI_CONTROL_PALETTE_DISABLED
		: controlState.Pressed	? ::gpk::GUI_CONTROL_PALETTE_PRESSED
		: controlState.Selected	? ::gpk::GUI_CONTROL_PALETTE_SELECTED
		: controlState.Hovered	? ::gpk::GUI_CONTROL_PALETTE_HOVER
		: ::gpk::GUI_CONTROL_PALETTE_NORMAL
		;
}
static	::gpk::error_t	fillColorTableBorders3D			(const ::gpk::SControlState & controlState, const ::gpk::SControlDraw & controlDraw, const ::gpk::SControlMode & controlMode, ::gpk::view<::gpk::bgra> & colors)					{
	double						bright;
	double						shaded;
	if(controlMode.NoHover) {
		bright					= controlDraw.FrameOut ? 1.2 : 0.8;
		shaded					= controlDraw.FrameOut ? 0.8 : 1.2;
	}
	else {
		bright					= controlState.Pressed ? 0.4 : controlState.Hovered ? 0.8 : 1.2;
		shaded					= controlState.Pressed ? 1.5 : controlState.Hovered ? 1.2 : 0.8;
	}
	::gpk::bgra					colorBackground											= colors[::gpk::GUI_CONTROL_AREA_OLD_BACKGROUND];
	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_LEFT		]	= colorBackground * bright;
	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_TOP		]	= colorBackground * bright;
	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_RIGHT		]	= colorBackground * shaded;
	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_BOTTOM	]	= colorBackground * shaded;
	return 0;					  
}
static	::gpk::error_t	fillColorTableNew				(::gpk::SGUI & gui, ::gpk::GUI_COLOR_MODE colorMode, const ::gpk::SControl & control, const ::gpk::SControlState & controlState, const ::gpk::SControlDraw & controlDraw, const ::gpk::SControlMode & controlMode, bool disabled, ::gpk::view<::gpk::bgra> & colors)					{
	const ::gpk::astatic<::gpk::bgra, ::gpk::UI_CONTROL_AREA_COUNT>
								& colorCombo					= gui.Colors->Palettes[control.Palettes[::paletteIndexFromState(disabled, controlState)]];
	colors[::gpk::GUI_CONTROL_AREA_OLD_BACKGROUND	]	= colorCombo[::gpk::UI_CONTROL_AREA_BACKGROUND	];
	colors[::gpk::GUI_CONTROL_AREA_OLD_CLIENT		]	= colorCombo[::gpk::UI_CONTROL_AREA_CLIENT		];
	if(colorMode == ::gpk::GUI_COLOR_MODE_3D || colorMode == ::gpk::GUI_COLOR_MODE_Default)
		return ::fillColorTableBorders3D(controlState, controlDraw, controlMode, colors);

	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_LEFT	]	= colorCombo[::gpk::UI_CONTROL_AREA_BORDER_LEFT	];
	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_TOP	]	= colorCombo[::gpk::UI_CONTROL_AREA_BORDER_TOP	];
	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_RIGHT	]	= colorCombo[::gpk::UI_CONTROL_AREA_BORDER_RIGHT	];
	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_BOTTOM]	= colorCombo[::gpk::UI_CONTROL_AREA_BORDER_BOTTOM	];
	return 0;
}
static	::gpk::error_t	fillColorTableOld				(::gpk::SGUI & gui, ::gpk::GUI_COLOR_MODE colorMode, const ::gpk::SControl & control, const ::gpk::SControlState & controlState, const ::gpk::SControlDraw & controlDraw, const ::gpk::SControlMode & controlMode, bool disabled, ::gpk::view<::gpk::bgra> & colors)					{
	const ::gpk::SControlTheme	& theme							= (*gui.Colors->ControlThemes)[(0 == control.ColorTheme) ? gui.ThemeDefault : control.ColorTheme - 1];
	const ::gpk::astaticu32<::gpk::UI_CONTROL_AREA_COUNT>	
								& colorCombo					= theme.ColorCombos[::paletteIndexFromState(disabled, controlState)];
	colors[::gpk::GUI_CONTROL_AREA_OLD_BACKGROUND	]	= (*gui.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_BACKGROUND	]];
	colors[::gpk::GUI_CONTROL_AREA_OLD_CLIENT		]	= (*gui.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_CLIENT		]];
	if(colorMode == ::gpk::GUI_COLOR_MODE_3D || colorMode == ::gpk::GUI_COLOR_MODE_Default)
		return ::fillColorTableBorders3D(controlState, controlDraw, controlMode, colors);

	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_LEFT	]	= (*gui.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_BORDER_LEFT	]];
	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_TOP	]	= (*gui.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_BORDER_TOP	]];
	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_RIGHT	]	= (*gui.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_BORDER_RIGHT	]];
	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_BOTTOM]	= (*gui.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_BORDER_BOTTOM	]];
	return 0;
}
static	::gpk::error_t	fillColorTable					(::gpk::SGUI & gui, int32_t iControl, bool disabled, ::gpk::view<::gpk::bgra> colors)					{
	const ::gpk::SControl		& control						= gui.Controls.Controls	[iControl];
	const ::gpk::SControlDraw	& draw							= gui.Controls.Draw		[iControl];
	const ::gpk::SControlState	& state							= gui.Controls.States	[iControl];
	const ::gpk::SControlMode	& mode							= gui.Controls.Modes	[iControl];
	::gpk::GUI_COLOR_MODE		colorMode						= (::gpk::GUI_COLOR_MODE)((draw.ColorMode == ::gpk::GUI_COLOR_MODE_Default) ? gui.ColorModeDefault : draw.ColorMode);
	gpk_necs((false == draw.UseNewPalettes) 
		? fillColorTableOld(gui, colorMode, control, state, draw, mode, disabled, colors) 
		: fillColorTableNew(gui, colorMode, control, state, draw, mode, disabled, colors)
		);

	return 0;
}
static	::gpk::error_t	actualControlDraw				(::gpk::SGUI & gui, int32_t iControl, ::gpk::v2<::gpk::bgra> & target)					{
	const ::gpk::SControl		& control						= gui.Controls.Controls	[iControl];
	::gpk::bgra					colors[::gpk::GUI_CONTROL_AREA_OLD_COUNT]	= {}; // -- Fill color table
	const bool					disabled						= ::gpk::controlDisabled(gui, iControl);
	const ::gpk::SControlMetrics	& controlMetrics			= gui.Controls.Metrics[iControl];
	{
		const ::gpk::SControlDraw	& controlDraw					= gui.Controls.Draw  [iControl];
		::fillColorTable(gui, iControl, disabled, colors);

		for(uint32_t iElement = 0; iElement < ::gpk::GUI_CONTROL_AREA_OLD_COUNT; ++iElement)
			if(iElement != ::gpk::GUI_CONTROL_AREA_OLD_CLIENT && (iElement != ::gpk::GUI_CONTROL_AREA_OLD_BACKGROUND || false == controlDraw.NoBackgroundRect))
				::gpk::drawRectangle(target, colors[iElement], controlMetrics.Rectangles[iElement]);
	}
	int32_t						colorIndices [8]				=
		{ ::gpk::GUI_CONTROL_AREA_OLD_BORDER_TOP
		, ::gpk::GUI_CONTROL_AREA_OLD_BORDER_LEFT
		, ::gpk::GUI_CONTROL_AREA_OLD_BORDER_TOP
		, ::gpk::GUI_CONTROL_AREA_OLD_BORDER_RIGHT
		, ::gpk::GUI_CONTROL_AREA_OLD_BORDER_LEFT
		, ::gpk::GUI_CONTROL_AREA_OLD_BORDER_BOTTOM
		, ::gpk::GUI_CONTROL_AREA_OLD_BORDER_RIGHT
		, ::gpk::GUI_CONTROL_AREA_OLD_BORDER_BOTTOM
		};
	for(uint32_t iTri = 0; iTri < 8; ++iTri)
		::gpk::drawTriangle(target, colors[colorIndices[iTri]], controlMetrics.Triangles[iTri]);

	if(control.Image.metrics().x && control.Image.metrics().y) {
		const ::gpk::SControlState	& controlState					= gui.Controls.States[iControl];
		//::gpk::rect2<int32_t>	rectImage						=
		//	{ controlMetrics.Client.Global.Offset
		//	, controlMetrics.Client.Global.Size
		//		+ ::gpk::n2<int32_t>
		//			{ ::gpk::min(0, ::gpk::min(controlMetrics.Client.Global.Offset.x, controlMetrics.Client.Local.Offset.x))
		//			, ::gpk::min(0, ::gpk::min(controlMetrics.Client.Global.Offset.y, controlMetrics.Client.Local.Offset.y))
		//			}
		//	};
		::gpk::rect2i16				rectImage							= {{}, control.Image.metrics().i16()};
		::gpk::realignRectangle(controlMetrics.Client.Global.Size.u32(), rectImage, rectImage, control.ImageAlign);
		rectImage.Offset		+= controlMetrics.Client.Global.Offset;
		rectImage.Size.x		= ::gpk::min(rectImage.Limit().x, controlMetrics.Client.Global.Limit().x) - rectImage.Offset.x;
		rectImage.Size.y		= ::gpk::min(rectImage.Limit().y, controlMetrics.Client.Global.Limit().y) - rectImage.Offset.y;
		if(controlState.Hovered)
			rectImage.Offset += {1, 1};
		if(0 == control.ImageInvertY)
			::gpk::grid_copy_blend(target, control.Image, rectImage, control.ImageOffset);
		else {
			::gpk::img8bgra				& inverted							= gui.Controls.Images[iControl].Temp;
			inverted.resize(control.Image.metrics());
			::gpk::grid_mirror_y(inverted.View, control.Image);
			::gpk::grid_copy_blend(target, inverted.View, rectImage, control.ImageOffset);
		}
	}

	es_if(errored(::controlTextDraw(gui, iControl, target, disabled)));
	return 0;
}
::gpk::error_t			gpk::controlDrawHierarchy		(::gpk::SGUI & gui, int32_t iControl, ::gpk::v2<::gpk::bgra> target)								{
	gpk_necall(::gpk::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	if(gui.LastSize != target.metrics().u16()) {
		for(uint32_t iOutdated = 0; iOutdated < gui.Controls.Controls.size(); ++iOutdated)
			gui.Controls.States[iOutdated].Updated	= false;
		gui.LastSize			= target.metrics().u16();
	}
	if(false == ::gpk::controlHidden(gui, iControl)) {
		//::gpk::SControlMode		controlModes					= gui.Controls.Modes[iControl];
		gpk_necs(::actualControlDraw(gui, iControl, target));
		::gpk::vci32				children						= gui.Controls.Children[iControl];
		for(uint32_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
			gpk_necs(::gpk::controlDrawHierarchy(gui, children[iChild], target));
	}
	return 0;
}
::gpk::error_t			gpk::guiDraw					(::gpk::SGUI & gui, ::gpk::v2<::gpk::bgra> target)													{
	if(gui.LastSize != target.metrics().u16() || gui.LastZoom != gui.Zoom) {
		for(uint32_t iOutdated = 0; iOutdated < gui.Controls.Controls.size(); ++iOutdated)
			gui.Controls.States[iOutdated].Updated	= false;
		gui.LastSize			= target.metrics().u16();
		gui.LastZoom			= gui.Zoom;
	}
	gpk_necs(::gpk::guiUpdateMetrics(gui, gui.LastSize, false));;
	for(uint32_t iControl = 0; iControl < gui.Controls.Controls.size(); ++iControl)
		if(false == ::gpk::controlInvalid(gui, iControl) && ::gpk::controlInvalid(gui, gui.Controls.Controls[iControl].Parent))
			es_if(errored(::gpk::controlDrawHierarchy(gui, iControl, target)));
	return 0;
}
