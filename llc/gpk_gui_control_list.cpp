#include "gpk_gui_control_list.h"
#include "gpk_label.h"

::gpk::error_t			gpk::controlListInitialize			(::gpk::SGUI & gui, ::gpk::SControlList& menu)													{
	menu					= {};
	gpk_necall(menu.IdControl = ::gpk::controlCreate(gui), "%s", "Failed to create menu control!");
	gui.Controls.Placement[menu.IdControl].Border				= {};
	gui.Controls.Placement[menu.IdControl].Margin				= {};
	gui.Controls.Text			[menu.IdControl].Text				= " ";
	gui.Controls.Constraints	[menu.IdControl].AttachSizeToText	= {false, false};
	//gui.Controls.Modes			[menu.IdControl].Design				= true;
	return 0;
}

::gpk::error_t			gpk::controlListArrange	(::gpk::SGUI & gui, ::gpk::SControlList& menu)													{
	const ::gpk::SRasterFont	& selectedFont			= *gui.Fonts[::gpk::in_range(gui.Controls.Text[menu.IdControl].FontSelected, (int16_t)0, (int16_t)gui.Fonts.size()) ? gui.Controls.Text[menu.IdControl].FontSelected : gui.SelectedFont];
	for(uint32_t iItem = 0; iItem < menu.IdControls.size(); ++iItem) {
		const uint32_t				idControl				= menu.IdControls[iItem];
		::gpk::SControlConstraints	& controlConstraints	= gui.Controls.Constraints	[idControl];
		::gpk::SControlPlacement	& control				= gui.Controls.Placement	[idControl];
		::gpk::SControlText			& controlText			= gui.Controls.Text			[idControl];
		const uint32_t				iPrevItem				= iItem - 1;
		const ::gpk::n2u8			fontCharSize			= selectedFont.CharSize;
		control.Area.Size.y		= fontCharSize.y + ::gpk::controlNCSpacing(control).y;
		switch(menu.Orientation) {
		default:
		case ::gpk::CONTROL_LIST_DIRECTION_HORIZONTAL	:
			control.Area.Size.x		= int16_t(fontCharSize.x * controlText.Text.size() + ::gpk::controlNCSpacing(control).x);
			if(menu.IdControls.size() > iPrevItem)
				control.Area.Offset.x	= gui.Controls.Placement[menu.IdControls[iPrevItem]].Area.Limit().x;
			break;
		case ::gpk::CONTROL_LIST_DIRECTION_VERTICAL		:
			controlConstraints.AttachSizeToControl.x	= menu.IdControl;
			if(menu.IdControls.size() > iPrevItem)
				control.Area.Offset.y	= gui.Controls.Placement[menu.IdControls[iPrevItem]].Area.Limit().y;
			break;
		}
	}

	::gpk::SControlPlacement	& controlMenu			= gui.Controls.Placement[menu.IdControl];
	controlMenu.Area.Size	= {};
	const ::gpk::n2u8			fontCharSize			= selectedFont.CharSize;
	for(uint32_t iItem = 0; iItem < menu.IdControls.size(); ++iItem) {
		const uint32_t				idControl			= menu.IdControls		[iItem];
		::gpk::SControlPlacement	& control			= gui.Controls.Placement[idControl];
		::gpk::SControlText			& controlText		= gui.Controls.Text		[idControl];
		switch(menu.Orientation) {
		default:
		case ::gpk::CONTROL_LIST_DIRECTION_HORIZONTAL	:
			controlMenu.Area.Size.x	+= gui.Controls.Placement[idControl].Area.Size.x;
			controlMenu.Area.Size.y	= ::gpk::max(controlMenu.Area.Size.y, gui.Controls.Placement[idControl].Area.Size.y);
			break;
		case ::gpk::CONTROL_LIST_DIRECTION_VERTICAL		:
			controlMenu.Area.Size.y	+= gui.Controls.Placement[idControl].Area.Size.y;
			controlMenu.Area.Size.x	= ::gpk::max(controlMenu.Area.Size.x, (int16_t)(controlText.Text.size() * fontCharSize.x + ::gpk::controlNCSpacing(control).x));
			break;
		}
	}
	if(::gpk::CONTROL_LIST_DIRECTION_VERTICAL == menu.Orientation)
		controlMenu.Area.Size.x	+= 16;

	return 0;
}

::gpk::error_t			gpk::controlListPush	(::gpk::SGUI & gui, ::gpk::SControlList& menu, const ::gpk::vcs& text, int64_t eventCode)				{
	if(menu.IdControl == -1)
		gpk_necs(::gpk::controlListInitialize(gui, menu));

	const int32_t				idControl				= ::gpk::controlCreate(gui);
	gpk_necall(idControl, "%s", "Failed to create control! Out of memory?");
	::gpk::controlSetParent(gui, idControl, menu.IdControl);
	::gpk::SControlPlacement	& control				= gui.Controls.Placement[idControl];
	::gpk::SControlText			& controlText			= gui.Controls.Text		[idControl];
	control		.Margin		= {4, 2, 4, 2};
	controlText	.Text		= {text.begin(), text.size()};
	controlText	.Align		= ::gpk::ALIGN_CENTER_LEFT;
	menu.IdControls	.push_back(idControl);
	menu.Events		.push_back(eventCode);
	gpk::controlListArrange(gui, menu);
	return 0;
}

::gpk::error_t			gpk::paletteGridInitialize	(::gpk::SGUI & gui, ::gpk::SPaletteGrid& palette)				{
	const ::gpk::n2f64			targetSize					= {256.0, 256.0};
	palette.IdControl		= ::gpk::controlCreate(gui);

	::gpk::SControlPlacement	& control					= gui.Controls.Placement[palette.IdControl];
	const uint32_t				widthViewport				= (uint32_t)(targetSize.x + ((int64_t)control.Border.Left + control.Border.Right + control.Margin.Left + control.Margin.Right));
	const uint32_t				heightViewport				= (uint32_t)(targetSize.y + ((int64_t)control.Border.Top + control.Border.Bottom + control.Margin.Top + control.Margin.Bottom));
	control.Area.Size		= {(int16_t)widthViewport, (int16_t)heightViewport};
	control.Align			= ::gpk::ALIGN_CENTER;
	return 0;
}

::gpk::error_t			gpk::paletteGridColorsSet	(::gpk::SGUI & gui, ::gpk::SPaletteGrid& palette, const ::gpk::view2d<::gpk::bgra>& colors)			{
	if(-1 == palette.IdControl)
		::gpk::paletteGridInitialize(gui, palette);

	//gui.Controls.Metrics[palette.IdControl];

	const ::gpk::n2u32			& gridMetrics				= colors.metrics();
	palette.IdControls.resize(colors.size());
	palette.Colors			= colors;

	const ::gpk::n2f64			targetSize					= gui.Controls.Metrics[palette.IdControl].Client.Global.Size.f64(); //{256.0, 256.0};
	::gpk::n2f64				controlSize					= {targetSize.x / colors.metrics().x, targetSize.y / colors.metrics().y};

	for(uint32_t y = 0; y < gridMetrics.y; ++y)
	for(uint32_t x = 0; x < gridMetrics.x; ++x) {
		const int32_t				linearIndex					= y * gridMetrics.x + x;
		uint32_t					iFoundColor					= 0;
		int32_t						themeIndex					= -1;
		const ::gpk::bgra			& gridCellColor				= colors[y][x];
		for(; iFoundColor < gui.Colors->Palette->size(); ++iFoundColor)
			if((*gui.Colors->Palette)[iFoundColor] == gridCellColor) {
				themeIndex				= iFoundColor;
				break;
			}
		if(iFoundColor >= gui.Colors->Palette->size()) {
			int32_t						indexColor			= gui.Colors->Palette->push_back(gridCellColor);
			themeIndex				= gui.Colors->ControlThemes->push_back({});
			::gpk::SControlTheme		& theme				= (*gui.Colors->ControlThemes)[themeIndex];
			::gpk::astu32<::gpk::UI_CONTROL_AREA_COUNT>	& comboDisabled		= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_DISABLED	] = {};
			::gpk::astu32<::gpk::UI_CONTROL_AREA_COUNT>	& comboPressed 		= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_PRESSED	] = {};
			::gpk::astu32<::gpk::UI_CONTROL_AREA_COUNT>	& comboSelected		= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_SELECTED	] = {};
			::gpk::astu32<::gpk::UI_CONTROL_AREA_COUNT>	& comboHover 		= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_HOVER	] = {};
			::gpk::astu32<::gpk::UI_CONTROL_AREA_COUNT>	& comboNormal		= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_NORMAL	] = {};
			comboDisabled	[::gpk::UI_CONTROL_AREA_BACKGROUND	]	= indexColor;
			comboDisabled	[::gpk::UI_CONTROL_AREA_CLIENT		]	= comboDisabled[::gpk::UI_CONTROL_AREA_BACKGROUND];
			comboNormal		[::gpk::UI_CONTROL_AREA_BACKGROUND	]	= indexColor;
			comboNormal		[::gpk::UI_CONTROL_AREA_CLIENT		]	= comboDisabled[::gpk::UI_CONTROL_AREA_BACKGROUND];
			comboSelected	[::gpk::UI_CONTROL_AREA_BACKGROUND	]	= indexColor;
			comboSelected	[::gpk::UI_CONTROL_AREA_CLIENT		]	= comboDisabled[::gpk::UI_CONTROL_AREA_BACKGROUND];
			comboHover		[::gpk::UI_CONTROL_AREA_BACKGROUND	]	= indexColor;
			comboHover		[::gpk::UI_CONTROL_AREA_CLIENT		]	= comboDisabled[::gpk::UI_CONTROL_AREA_BACKGROUND];
			comboPressed	[::gpk::UI_CONTROL_AREA_BACKGROUND	]	= indexColor;
			comboPressed	[::gpk::UI_CONTROL_AREA_CLIENT		]	= comboDisabled[::gpk::UI_CONTROL_AREA_BACKGROUND];
			//for(uint32_t iState = 0; iState < theme.ColorCombos.size(); ++iState)
			//for(uint32_t iArea = 0; iArea < ::gpk::GUI_CONTROL_COLOR_COUNT; ++iArea)
			//	theme.ColorCombos[iState][iArea]	= ::gpk::min((uint32_t)theme.ColorCombos[iState][iArea], gui.Palette.size() - 1);
		}
		const uint32_t				paletteElemIndex	= linearIndex;
		palette.IdControls[paletteElemIndex] = ::gpk::controlCreate(gui);
		::gpk::SControlPlacement	& control			= gui.Controls.Placement[palette.IdControls[paletteElemIndex]];
		::gpk::SControlDraw			& controlDraw		= gui.Controls.Draw[palette.IdControls[paletteElemIndex]];
		controlDraw.ColorTheme	= int8_t(themeIndex + 1);
		control.Area.Offset		= {(int16_t)(controlSize.x * x), (int16_t)(controlSize.y * y)};
		control.Area.Size		= controlSize.i16();
		gpk_necs(::gpk::controlSetParent(gui, palette.IdControls[paletteElemIndex], palette.IdControl));
	}
	return 0;
}

::gpk::error_t			gpk::guiCreateControlList		(::gpk::SGUI & gui, ::gpk::DIRECTION direction, ::gpk::view<const ::gpk::vcc> buttonText, cid_t iParent, const ::gpk::SControlPlacement & args, ::gpk::ALIGN textAlign, ::gpk::vcid out_ids) {
	int32_t						result						= -1;
	for(uint16_t iButton = 0; iButton < buttonText.size(); ++iButton) {
		int32_t						idControl					= ::gpk::controlCreate(gui);
		if(out_ids.size() > iButton)
			out_ids[iButton]		= idControl;

		if(0 == iButton)
			result					= idControl;

		::gpk::SControlPlacement	& control					= gui.Controls.Placement[idControl];
		control					= args;
		if(direction & (::gpk::DIRECTION_LEFT | ::gpk::DIRECTION_RIGHT))
			control.Area.Offset.x	+= int16_t(args.Area.Size.x * iButton);
		else
			control.Area.Offset.y	+= int16_t(args.Area.Size.y * iButton);

		::gpk::SControlText			& controlText				= gui.Controls.Text[idControl];
		controlText.Text		= buttonText[iButton];
		controlText.Align		= textAlign;

		::gpk::SControlConstraints	& itemConstraints	= gui.Controls.Constraints[idControl];
		if(direction & (::gpk::DIRECTION_UP | ::gpk::DIRECTION_DOWN))
			itemConstraints.AttachSizeToControl.x	= iParent;
		else if(direction & (::gpk::DIRECTION_LEFT | ::gpk::DIRECTION_RIGHT)) {
			itemConstraints.AttachSizeToText.x		= true;
			itemConstraints.AttachSizeToControl.y	= iParent;
		}

		if(iButton) {
			if(direction & ::gpk::DIRECTION_LEFT)
				itemConstraints.DockToControl.Left	= out_ids[iButton - 1];
			else if(direction & ::gpk::DIRECTION_RIGHT)
				itemConstraints.DockToControl.Right	= out_ids[iButton - 1];
			else if(direction & ::gpk::DIRECTION_UP)
				itemConstraints.DockToControl.Top	= out_ids[iButton - 1];
			else if(direction & ::gpk::DIRECTION_DOWN)
				itemConstraints.DockToControl.Bottom	= out_ids[iButton - 1];

		}
		gpk_necall(::gpk::controlSetParent(gui, idControl, iParent), "idControl: %i, buttonText[iButton]: %s", idControl, ::gpk::toString(buttonText[iButton]).begin()); // Make a template and macro for this pattern
	}
	return result;
}

::gpk::error_t			gpk::guiCreateControlList		(::gpk::SGUI & gui, ::gpk::view<const ::gpk::vcc> buttonText, int32_t iParent, const ::gpk::n2u16 & buttonSize, const ::gpk::n2i16 & offset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::vcid out_ids) {
	SControlPlacement			controlArgs					= {};
	controlArgs.Align		= controlAlign;
	controlArgs.Area		= {offset, buttonSize.i16()};
	controlArgs.Border		= {1, 1, 1, 1};
	controlArgs.Margin		= {1, 1, 1, 1};
	return ::gpk::guiCreateControlList(gui, {}, buttonText, iParent, controlArgs, textAlign, out_ids);
}
