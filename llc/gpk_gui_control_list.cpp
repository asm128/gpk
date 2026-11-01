#include "gpk_gui_control_list.h"
#include "gpk_label.h"

::gpk::cid_t			gpk::createScreenLayout(::gpk::SGUI & gui, ::gpk::cid_t parent) {
	::gpk::cid_t				controlId;
	if_fail_fe(controlId = ::gpk::controlCreate(gui));

	gui.Controls.Placement	[controlId].Border				= 
	gui.Controls.Placement	[controlId].Margin				= {};
	gui.Controls.Constraints[controlId].AttachSizeToControl	= {controlId, controlId};
	gui.Controls.Draw		[controlId].NoClient			= true;
	gui.Controls.States		[controlId].Mask				&= ~::gpk::GUI_CONTROL_FLAG_Hovered;

	if(parent != ::gpk::CID_INVALID)
		if_fail_fe(gpk::controlSetParent(gui, controlId, parent));

	return controlId;
}

gpk::err_t 		gpk::controlListInitialize	(::gpk::SGUI & gui, ::gpk::SControlList& menu)													{
	menu					= {};
	if_fail_fe(menu.IdControl = ::gpk::controlCreate(gui));
	gui.Controls.Placement	[menu.IdControl].Border				= {};
	gui.Controls.Placement	[menu.IdControl].Margin				= {};
	gui.Controls.Text		[menu.IdControl].Text				= " ";
	gui.Controls.Constraints[menu.IdControl].AttachSizeToText	= {false, false};
	//gui.Controls.Modes			[menu.IdControl].Design				= true;
	return 0;
}

gpk::err_t 		gpk::controlListArrange	(::gpk::SGUI & gui, ::gpk::SControlList& menu)													{
	const ::gpk::SRasterFont	& selectedFont			= *gui.Fonts[::gpk::in_range(gui.Controls.Text[menu.IdControl].FontSelected, (s1_t)0, (s1_t)gui.Fonts.size()) ? gui.Controls.Text[menu.IdControl].FontSelected : gui.SelectedFont];
	for(u2_t iItem = 0; iItem < menu.IdControls.size(); ++iItem) {
		const u2_t				idControl				= menu.IdControls[iItem];
		::gpk::SControlConstraints	& controlConstraints	= gui.Controls.Constraints	[idControl];
		::gpk::SControlPlacement	& control				= gui.Controls.Placement	[idControl];
		::gpk::SControlText			& controlText			= gui.Controls.Text			[idControl];
		const u2_t				iPrevItem				= iItem - 1;
		const ::gpk::n2u0_t			fontCharSize			= selectedFont.CharSize;
		control.Area.Size.y		= fontCharSize.y + ::gpk::controlNCSpacing(control).y;
		switch(menu.Orientation) {
		default:
		case ::gpk::DIRECTION_RIGHT	:
			control.Area.Size.x		= s1_t(fontCharSize.x * controlText.Text.size() + ::gpk::controlNCSpacing(control).x);
			if(menu.IdControls.size() > iPrevItem)
				control.Area.Offset.x	= gui.Controls.Placement[menu.IdControls[iPrevItem]].Area.Limit().x;
			break;
		case ::gpk::DIRECTION_DOWN:
			controlConstraints.AttachSizeToControl.x	= menu.IdControl;
			if(menu.IdControls.size() > iPrevItem)
				control.Area.Offset.y	= gui.Controls.Placement[menu.IdControls[iPrevItem]].Area.Limit().y;
			break;
		}
	}

	::gpk::SControlPlacement	& controlMenu			= gui.Controls.Placement[menu.IdControl];
	controlMenu.Area.Size	= {};
	const ::gpk::n2u0_t			fontCharSize			= selectedFont.CharSize;
	for(u2_t iItem = 0; iItem < menu.IdControls.size(); ++iItem) {
		const u2_t				idControl			= menu.IdControls		[iItem];
		::gpk::SControlPlacement	& control			= gui.Controls.Placement[idControl];
		::gpk::SControlText			& controlText		= gui.Controls.Text		[idControl];
		switch(menu.Orientation) {
		default:
		case ::gpk::DIRECTION_RIGHT	:
			controlMenu.Area.Size.x	+= gui.Controls.Placement[idControl].Area.Size.x;
			controlMenu.Area.Size.y	= ::gpk::max(controlMenu.Area.Size.y, gui.Controls.Placement[idControl].Area.Size.y);
			break;
		case ::gpk::DIRECTION_DOWN		:
			controlMenu.Area.Size.y	+= gui.Controls.Placement[idControl].Area.Size.y;
			controlMenu.Area.Size.x	= ::gpk::max(controlMenu.Area.Size.x, (s1_t)(controlText.Text.size() * fontCharSize.x + ::gpk::controlNCSpacing(control).x));
			break;
		}
	}
	if(::gpk::DIRECTION_DOWN == menu.Orientation)
		controlMenu.Area.Size.x	+= 16;

	return 0;
}

gpk::err_t 		gpk::controlListPush	(::gpk::SGUI & gui, ::gpk::SControlList& menu, const ::gpk::vcst_t& text, s3_t eventCode)				{
	if(menu.IdControl == -1)
		if_fail_fe(gpk::controlListInitialize(gui, menu));

	const cid_t					idControl				= ::gpk::controlCreate(gui);
	if_fail_fef(idControl, "%s", "Failed to create control! Out of memory?");
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

gpk::err_t 		gpk::paletteGridInitialize	(::gpk::SGUI & gui, ::gpk::SPaletteGrid& palette)				{
	const ::gpk::n2f3_t			targetSize					= {256.0, 256.0};
	palette.IdControl		= ::gpk::controlCreate(gui);

	::gpk::SControlPlacement	& control					= gui.Controls.Placement[palette.IdControl];
	const u2_t				widthViewport				= (u2_t)(targetSize.x + ((s3_t)control.Border.Left + control.Border.Right + control.Margin.Left + control.Margin.Right));
	const u2_t				heightViewport				= (u2_t)(targetSize.y + ((s3_t)control.Border.Top + control.Border.Bottom + control.Margin.Top + control.Margin.Bottom));
	control.Area.Size		= {(s1_t)widthViewport, (s1_t)heightViewport};
	control.Align			= ::gpk::ALIGN_CENTER;
	return 0;
}

gpk::err_t 		gpk::paletteGridColorsSet	(::gpk::SGUI & gui, ::gpk::SPaletteGrid& palette, const ::gpk::g8bgra& colors)			{
	if(-1 == palette.IdControl)
		if_fail_e(::gpk::paletteGridInitialize(gui, palette));

	//gui.Controls.Area[palette.IdControl];

	const ::gpk::n2u2_t			& gridMetrics				= colors.metrics();
	if_fail_fe(palette.IdControls.resize(colors.size()));
	palette.Colors			= colors;

	const ::gpk::n2f3_t			targetSize					= gui.Controls.Area[palette.IdControl].Client.Global.Size.f3(); //{256.0, 256.0};
	::gpk::n2f3_t				controlSize					= {targetSize.x / colors.metrics().x, targetSize.y / colors.metrics().y};

	for(u2_t y = 0; y < gridMetrics.y; ++y)
	for(u2_t x = 0; x < gridMetrics.x; ++x) {
		const s2_t				linearIndex					= y * gridMetrics.x + x;
		u2_t					iFoundColor					= 0;
		s2_t						themeIndex					= -1;
		const ::gpk::bgra			& gridCellColor				= colors[y][x];
		for(; iFoundColor < gui.Colors->Palette->size(); ++iFoundColor)
			if((*gui.Colors->Palette)[iFoundColor] == gridCellColor) {
				themeIndex				= iFoundColor;
				break;
			}
		if(iFoundColor >= gui.Colors->Palette->size()) {
			s2_t						indexColor			= gui.Colors->Palette->push_back(gridCellColor);
			themeIndex				= gui.Colors->ControlThemes->push_back({});
			::gpk::SControlTheme		& theme				= (*gui.Colors->ControlThemes)[themeIndex];
			::gpk::astu2_t<::gpk::UI_CONTROL_AREA_COUNT>	& comboDisabled		= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_DISABLED	] = {};
			::gpk::astu2_t<::gpk::UI_CONTROL_AREA_COUNT>	& comboPressed 		= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_PRESSED	] = {};
			::gpk::astu2_t<::gpk::UI_CONTROL_AREA_COUNT>	& comboSelected		= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_SELECTED	] = {};
			::gpk::astu2_t<::gpk::UI_CONTROL_AREA_COUNT>	& comboHover 		= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_HOVER	] = {};
			::gpk::astu2_t<::gpk::UI_CONTROL_AREA_COUNT>	& comboNormal		= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_NORMAL	] = {};
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
			//for(u2_t iState = 0; iState < theme.ColorCombos.size(); ++iState)
			//for(u2_t iArea = 0; iArea < ::gpk::GUI_CONTROL_COLOR_COUNT; ++iArea)
			//	theme.ColorCombos[iState][iArea]	= ::gpk::min((u2_t)theme.ColorCombos[iState][iArea], gui.Palette.size() - 1);
		}
		const u2_t				paletteElemIndex	= linearIndex;
		palette.IdControls[paletteElemIndex] = ::gpk::controlCreate(gui);
		::gpk::SControlPlacement	& control			= gui.Controls.Placement[palette.IdControls[paletteElemIndex]];
		::gpk::SControlDraw			& controlDraw		= gui.Controls.Draw[palette.IdControls[paletteElemIndex]];
		controlDraw.ColorTheme	= s0_t(themeIndex + 1);
		control.Area.Offset		= {(s1_t)(controlSize.x * x), (s1_t)(controlSize.y * y)};
		control.Area.Size		= controlSize.s1();
		if_fail_fe(gpk::controlSetParent(gui, palette.IdControls[paletteElemIndex], palette.IdControl));
	}
	return 0;
}

::gpk::cid_t			gpk::guiCreateControlList		(::gpk::SGUI & gui, ::gpk::DIRECTION direction, ::gpk::view<vcst_c> buttonText, ::gpk::cid_t iParent, const ::gpk::SControlPlacement & args, ::gpk::ALIGN textAlign, ::gpk::vcid out_ids) {
	::gpk::cid_t				result						= -1;
	for(u1_t iButton = 0; iButton < buttonText.size(); ++iButton) {
		::gpk::cid_t				idControl					= ::gpk::controlCreate(gui);
		if(out_ids.size() > iButton)
			out_ids[iButton]		= idControl;

		if(0 == iButton)
			result					= idControl;

		::gpk::SControlPlacement	& control					= gui.Controls.Placement[idControl];
		control					= args;
		if(direction & (::gpk::DIRECTION_LEFT | ::gpk::DIRECTION_RIGHT))
			control.Area.Offset.x	+= s1_t(args.Area.Size.x * iButton);
		else
			control.Area.Offset.y	+= s1_t(args.Area.Size.y * iButton);

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
		if_fail_fef(gpk::controlSetParent(gui, idControl, iParent), "idControl: %i, buttonText[iButton]: %.*s", idControl, GPK_VIEW_ARGS(buttonText[iButton])); // Make a tplt and macro for this pattern
	}
	return result;
}

::gpk::cid_t			gpk::guiCreateControlList		(::gpk::SGUI & gui, ::gpk::view<vcst_c> buttonText, ::gpk::cid_t iParent, ::gpk::n2u1_c & buttonSize, ::gpk::n2s1_c & offset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::vcid out_ids) {
	::gpk::SControlPlacement	controlArgs					= {};
	controlArgs.Align		= controlAlign;
	controlArgs.Area		= {offset, buttonSize.s1()};
	controlArgs.Border		= {1, 1, 1, 1};
	controlArgs.Margin		= {1, 1, 1, 1};
	return ::gpk::guiCreateControlList(gui, {}, buttonText, iParent, controlArgs, textAlign, out_ids);
}
