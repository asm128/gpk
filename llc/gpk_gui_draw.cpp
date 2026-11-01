#include "gpk_gui.h"

#include "gpk_grid_copy.h"
#include "gpk_bitmap_target.h"
#include "gpk_gui_text.h"
#include "gpk_rect_align.h"
#include "gpk_view_color.h"

GPK_USING_TYPEINT();
GPK_USING_VIEW();
GPK_USING_N2();

static	gpk::err_t controlTextDraw		(::gpk::SGUI & gui, s2_t iControl, ::gpk::g8bgra & target, bool bDisabled)				{
	::gpk::SControlState		& controlState		= gui.Controls.States	[iControl];
	::gpk::SControlDraw			& controlDraw		= gui.Controls.Draw	[iControl];
	::gpk::bgra					colorFace			= {0xFF, 0x00, 0xFF, 0xFF};
	if(0 == gui.Controls.Draw[iControl].UseNewPalettes) {
		const ::gpk::SControlTheme	& theme				= (*gui.Colors->ControlThemes)[(0 == controlDraw.ColorTheme) ? gui.ThemeDefault : controlDraw.ColorTheme - 1];
		const ::gpk::astu2_t<::gpk::UI_CONTROL_AREA_COUNT>	
									& colorCombo		= theme.ColorCombos
			[ bDisabled					? ::gpk::GUI_CONTROL_PALETTE_DISABLED
			: controlState.IsPressed () ? ::gpk::GUI_CONTROL_PALETTE_PRESSED
			: controlState.IsSelected() ? ::gpk::GUI_CONTROL_PALETTE_SELECTED
			: controlState.IsHovered () ? ::gpk::GUI_CONTROL_PALETTE_HOVER
			: ::gpk::GUI_CONTROL_PALETTE_NORMAL
			];
		colorFace				= (*gui.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_TEXT_FACE]];
		//const ::gpk::bgra			colorBack			= gui.Palette[colorCombo[::gpk::UI_CONTROL_AREA_TEXT_BACKGROUND]];
	}
	else {
		const ::gpk::astatic<::gpk::bgra, ::gpk::UI_CONTROL_AREA_COUNT>	& colorCombo											=
			gui.Colors->Palettes[controlDraw.Palettes
				[ bDisabled					? ::gpk::GUI_CONTROL_PALETTE_DISABLED
				: controlState.IsPressed () ? ::gpk::GUI_CONTROL_PALETTE_PRESSED
				: controlState.IsSelected() ? ::gpk::GUI_CONTROL_PALETTE_SELECTED
				: controlState.IsHovered () ? ::gpk::GUI_CONTROL_PALETTE_HOVER
				: ::gpk::GUI_CONTROL_PALETTE_NORMAL
				]]
				;
		colorFace				= colorCombo[::gpk::UI_CONTROL_AREA_TEXT_FACE];
	}

	::gpk::SControlArea		& controlMetrics	= gui.Controls.Area[iControl];
	::gpk::rect2s1_t				rectText			= controlMetrics.Text;
	if(controlState.IsPressed() || controlState.IsHovered()) { // shift text offset by one pixel to give the effect of being pushed.
		rectText.Offset			+= ::gpk::n2s1_t{1, 1};
		rectText.Size			-= ::gpk::n2s1_t{1, 1};
		if(rectText.Size.x < 0) rectText.Size.x = 0;
		if(rectText.Size.y < 0) rectText.Size.y = 0;
	} // This isn't done in updateMetrics() because the function is only executed for outdated metrics.
	// Changhing the state from idle to hover however doesn't cause the control metrics to become outdated (and in general it's pointless for other than the effect we're applying here).

	::gpk::SControlText			& controlText		= gui.Controls.Text		[iControl];
	::gpk::apod<::gpk::n2u1_t>	dstCoords;
	const u2_t				iFont				= ::gpk::in_range(controlText.FontSelected, (s1_t)0, (s1_t)gui.Fonts.size()) ? controlText.FontSelected : gui.SelectedFont;
	const ::gpk::SRasterFont	& selectedFont		= *gui.Fonts[iFont];
	if_fail_fe(gpk::textLineRaster(target.metrics(), selectedFont.CharSize, rectText, selectedFont.Texture, controlText.Text, dstCoords));
	for(u2_t iCoord = 0; iCoord < dstCoords.size(); ++iCoord) {
		const ::gpk::n2u1_t			dstCoord			= dstCoords[iCoord];
		target[dstCoord.y][dstCoord.x]	= colorFace;
		//::gpk::drawPixelLight(target, dstCoords[iCoord], colorFace, controlState.Pressed ? 0.75f : 0.5f, controlState.Pressed ? 1.0f : 0.95);
	}
	return 0;
}
static	::gpk::GUI_CONTROL_PALETTE	paletteIndexFromState	(bool disabled, const ::gpk::SControlState & controlState)					{
	return disabled					? ::gpk::GUI_CONTROL_PALETTE_DISABLED
		: controlState.IsPressed () ? ::gpk::GUI_CONTROL_PALETTE_PRESSED
		: controlState.IsSelected() ? ::gpk::GUI_CONTROL_PALETTE_SELECTED
		: controlState.IsHovered () ? ::gpk::GUI_CONTROL_PALETTE_HOVER
		: ::gpk::GUI_CONTROL_PALETTE_NORMAL
		;
}
static	gpk::err_t fillColorTableBorders3D			(const ::gpk::SControlState & controlState, ::gpk::view<::gpk::bgra> & colors)					{
	f3_t						bright;
	f3_t						shaded;
	bright					= controlState.IsPressed() ? 0.4 : controlState.IsHovered() ? 0.8 : 1.2;
	shaded					= controlState.IsPressed() ? 1.5 : controlState.IsHovered() ? 1.2 : 0.8;
	::gpk::bgra					colorBackground											= colors[::gpk::GUI_CONTROL_AREA_OLD_CLIENT];
	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_LEFT		]	= colorBackground * bright;
	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_TOP		]	= colorBackground * bright;
	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_RIGHT		]	= colorBackground * shaded;
	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_BOTTOM	]	= colorBackground * shaded;
	return 0;					  
}
static	gpk::err_t fillColorTableNew				(::gpk::SGUI & gui, ::gpk::GUI_COLOR_MODE colorMode, const ::gpk::SControlState & controlState, const ::gpk::SControlDraw & controlDraw, bool disabled, ::gpk::v8bgra & colors)					{
	const ::gpk::astatic<::gpk::bgra, ::gpk::UI_CONTROL_AREA_COUNT>
								& colorCombo					= gui.Colors->Palettes[controlDraw.Palettes[::paletteIndexFromState(disabled, controlState)]];
	//colors[::gpk::GUI_CONTROL_AREA_OLD_BACKGROUND	]	= colorCombo[::gpk::UI_CONTROL_AREA_BACKGROUND	];
	colors[::gpk::GUI_CONTROL_AREA_OLD_CLIENT		]	= colorCombo[::gpk::UI_CONTROL_AREA_CLIENT		];
	if(colorMode == ::gpk::GUI_COLOR_MODE_3D || colorMode == ::gpk::GUI_COLOR_MODE_Default)
		return ::fillColorTableBorders3D(controlState, colors);

	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_LEFT	]	= colorCombo[::gpk::UI_CONTROL_AREA_BORDER_LEFT	];
	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_TOP	]	= colorCombo[::gpk::UI_CONTROL_AREA_BORDER_TOP	];
	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_RIGHT	]	= colorCombo[::gpk::UI_CONTROL_AREA_BORDER_RIGHT	];
	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_BOTTOM]	= colorCombo[::gpk::UI_CONTROL_AREA_BORDER_BOTTOM	];
	return 0;
}
static	gpk::err_t fillColorTableOld				(::gpk::SGUI & gui, ::gpk::GUI_COLOR_MODE colorMode, const ::gpk::SControlState & controlState, const ::gpk::SControlDraw & controlDraw, bool disabled, ::gpk::v8bgra & colors)					{
	const ::gpk::SControlTheme	& theme							= (*gui.Colors->ControlThemes)[(0 == controlDraw.ColorTheme) ? gui.ThemeDefault : controlDraw.ColorTheme - 1];
	const ::gpk::astu2_t<::gpk::UI_CONTROL_AREA_COUNT>	
								& colorCombo					= theme.ColorCombos[::paletteIndexFromState(disabled, controlState)];
	//colors[::gpk::GUI_CONTROL_AREA_OLD_BACKGROUND	]	= (*gui.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_BACKGROUND	]];
	colors[::gpk::GUI_CONTROL_AREA_OLD_CLIENT		]	= (*gui.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_CLIENT		]];
	if(colorMode == ::gpk::GUI_COLOR_MODE_3D || colorMode == ::gpk::GUI_COLOR_MODE_Default)
		return ::fillColorTableBorders3D(controlState, colors);

	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_LEFT	]	= (*gui.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_BORDER_LEFT	]];
	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_TOP	]	= (*gui.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_BORDER_TOP	]];
	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_RIGHT	]	= (*gui.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_BORDER_RIGHT	]];
	colors[::gpk::GUI_CONTROL_AREA_OLD_BORDER_BOTTOM]	= (*gui.Colors->Palette)[colorCombo[::gpk::UI_CONTROL_AREA_BORDER_BOTTOM]];
	return 0;
}
static	gpk::err_t fillColorTable					(::gpk::SGUI & gui, ::gpk::cid_t iControl, bool disabled, ::gpk::view<::gpk::bgra> colors)					{
	const ::gpk::SControlDraw	& draw							= gui.Controls.Draw		[iControl];
	const ::gpk::SControlState	& state							= gui.Controls.States	[iControl];
	::gpk::GUI_COLOR_MODE		colorMode						= (::gpk::GUI_COLOR_MODE)((draw.ColorMode == ::gpk::GUI_COLOR_MODE_Default) ? gui.ColorModeDefault : draw.ColorMode);
	if_fail_fe((false == draw.UseNewPalettes) 
		? fillColorTableOld(gui, colorMode, state, draw, disabled, colors) 
		: fillColorTableNew(gui, colorMode, state, draw, disabled, colors)
		);

	return 0;
}
static	gpk::err_t actualControlDraw				(::gpk::SGUI & gui, ::gpk::cid_t iControl, ::gpk::g8bgra & target)					{
	::gpk::bgra					colors[::gpk::GUI_CONTROL_AREA_OLD_COUNT]	= {}; // -- Fill color table
	const bool					disabled						= ::gpk::controlDisabled(gui, iControl);
	const ::gpk::SControlArea	& controlMetrics				= gui.Controls.Area[iControl];
	const ::gpk::SControlDraw	& controlDraw					= gui.Controls.Draw  [iControl];
	{
		::fillColorTable(gui, iControl, disabled, colors);

		for(u2_t iElement = 0; iElement < ::gpk::GUI_CONTROL_AREA_OLD_COUNT; ++iElement) {
			if( (controlDraw.NoBorder && iElement != ::gpk::GUI_CONTROL_AREA_OLD_CLIENT) // (iElement >= ::gpk::GUI_CONTROL_AREA_OLD_BORDER_LEFT || iElement <= ::gpk::GUI_CONTROL_AREA_OLD_BORDER_BOTTOM))
			 || (controlDraw.NoClient && iElement == ::gpk::GUI_CONTROL_AREA_OLD_CLIENT) //(iElement == ::gpk::GUI_CONTROL_AREA_OLD_BACKGROUND || iElement == ::gpk::GUI_CONTROL_AREA_OLD_CLIENT))
			)
				continue;
			::gpk::drawRectangle(target, colors[iElement], controlMetrics.Rectangles[iElement]);
		}
	}
	s2_t						colorIndices [8]				=
		{ ::gpk::GUI_CONTROL_AREA_OLD_BORDER_TOP
		, ::gpk::GUI_CONTROL_AREA_OLD_BORDER_LEFT
		, ::gpk::GUI_CONTROL_AREA_OLD_BORDER_TOP
		, ::gpk::GUI_CONTROL_AREA_OLD_BORDER_RIGHT
		, ::gpk::GUI_CONTROL_AREA_OLD_BORDER_LEFT
		, ::gpk::GUI_CONTROL_AREA_OLD_BORDER_BOTTOM
		, ::gpk::GUI_CONTROL_AREA_OLD_BORDER_RIGHT
		, ::gpk::GUI_CONTROL_AREA_OLD_BORDER_BOTTOM
		};
	if(false == controlDraw.NoBorder && ::gpk::controlNCSpacing(gui.Controls.Placement[iControl]).Area()) {
		for(u2_t iTri = 0; iTri < 8; ++iTri)
			::gpk::drawTriangle(target, colors[colorIndices[iTri]], controlMetrics.Triangles[iTri]);
	}

	const ::gpk::SControlImage	& controlImage			= gui.Controls.Images[iControl];
	if(controlImage.Image.metrics().x && controlImage.Image.metrics().y) {
		const ::gpk::SControlState	& controlState			= gui.Controls.States[iControl];
		//::gpk::rect2<s2_t>	rectImage						=
		//	{ controlMetrics.Client.Global.Offset
		//	, controlMetrics.Client.Global.Size
		//		+ ::gpk::n2<s2_t>
		//			{ ::gpk::min(0, ::gpk::min(controlMetrics.Client.Global.Offset.x, controlMetrics.Client.Local.Offset.x))
		//			, ::gpk::min(0, ::gpk::min(controlMetrics.Client.Global.Offset.y, controlMetrics.Client.Local.Offset.y))
		//			}
		//	};
		::gpk::rect2s1_t				rectImage				= {{}, controlImage.Image.metrics().s1()};
		::gpk::realignRectangle(controlMetrics.Client.Global.Size, rectImage, rectImage, (::gpk::ALIGN)controlImage.ImageAlign);
		rectImage.Offset		+= controlMetrics.Client.Global.Offset;
		rectImage.Size.x		= ::gpk::min(rectImage.Limit().x, controlMetrics.Client.Global.Limit().x) - rectImage.Offset.x;
		rectImage.Size.y		= ::gpk::min(rectImage.Limit().y, controlMetrics.Client.Global.Limit().y) - rectImage.Offset.y;
		if(controlState.IsHovered())
			rectImage.Offset		+= {1, 1};
		if(0 == controlImage.ImageInvertY)
			::gpk::grid_copy_blend(target, controlImage.Image, rectImage, controlImage.ImageOffset);
		else {
			::gpk::img8bgra				& inverted				= gui.Controls.Images[iControl].Temp.Color;
			if_fail_fe(gui.Controls.Images[iControl].Temp.resize(controlImage.Image.metrics()));
			::gpk::grid_mirror_y(inverted.View, controlImage.Image);
			::gpk::grid_copy_blend(target, inverted.View, rectImage, controlImage.ImageOffset);
		}
	}

	if_true_e(::gpk::failed(::controlTextDraw(gui, iControl, target, disabled)));
	return 0;
}
gpk::err_t 		gpk::controlDrawHierarchy		(::gpk::SGUI & gui, ::gpk::cid_t iControl, ::gpk::g8bgra target)								{
	if_fail_fef(gpk::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	if(gui.LastSize != target.metrics16()) {
		for(u2_t iOutdated = 0; iOutdated < gui.Controls.States.size(); ++iOutdated)
			gui.Controls.SetUpdated((::gpk::cid_t)iOutdated, false);
		gui.LastSize			= target.metrics().u1();
	}
	if(false == ::gpk::controlHidden(gui, iControl)) {
		if_fail_fe(::actualControlDraw(gui, iControl, target));
		::gpk::vccid				children						= gui.Controls.Children[iControl];
		for(u2_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
			if_fail_fe(gpk::controlDrawHierarchy(gui, children[iChild], target));
	}
	return 0;
}
gpk::err_t 		gpk::guiDraw					(::gpk::SGUI & gui, ::gpk::g8bgra target)													{
	if(gui.LastSize != target.metrics().u1() || gui.LastZoom != gui.Zoom) {
		for(u2_t iOutdated = 0; iOutdated < gui.Controls.States.size(); ++iOutdated)
			gui.Controls.SetUpdated((::gpk::cid_t)iOutdated, false);
		gui.LastSize			= target.metrics().u1();
		gui.LastZoom			= gui.Zoom;
	}
	if_fail_fe(gpk::guiUpdateMetrics(gui, gui.LastSize, false));;
	for(u2_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl)
		if(false == ::gpk::controlInvalid(gui, (::gpk::cid_t)iControl) && ::gpk::controlInvalid(gui, gui.Controls.States[iControl].Parent))
			if_true_e(::gpk::failed(::gpk::controlDrawHierarchy(gui, (::gpk::cid_t)iControl, target)));
	return 0;
}
