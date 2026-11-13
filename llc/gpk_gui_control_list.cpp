#include "gpk_gui_control_list.h"
#include "gpk_label.h"

gpk::cid_t			gpk::createScreenLayout(SGUI & gui, cid_t parent) {
	cid_t				controlId;
	if_fail_fe(controlId = controlCreate(gui));

	gui.Controls.Placement	[controlId].Border				= 
	gui.Controls.Placement	[controlId].Margin				= {};
	gui.Controls.Constraints[controlId].AttachSizeToControl	= {controlId, controlId};
	gui.Controls.Draw		[controlId].NoClient			= true;
	gui.Controls.States		[controlId].Mask				&= ~GUI_CONTROL_FLAG_Hovered;

	if(parent != CID_INVALID)
		if_fail_fe(gpk::controlSetParent(gui, controlId, parent));

	return controlId;
}

gpk::err_t 		gpk::controlListInitialize	(SGUI & gui, SControlList& menu)													{
	menu					= {};
	if_fail_fe(menu.IdControl = controlCreate(gui));
	gui.Controls.Placement	[menu.IdControl].Border				= {};
	gui.Controls.Placement	[menu.IdControl].Margin				= {};
	gui.Controls.Text		[menu.IdControl].Text				= " ";
	gui.Controls.Constraints[menu.IdControl].AttachSizeToText	= {false, false};
	//gui.Controls.Modes			[menu.IdControl].Design				= true;
	return 0;
}

gpk::err_t 		gpk::controlListArrange	(SGUI & gui, SControlList & menu)													{
	const SRasterFont	& selectedFont			= *gui.Fonts[in_range(gui.Controls.Text[menu.IdControl].FontSelected, (s1_t)0, (s1_t)gui.Fonts.size()) ? gui.Controls.Text[menu.IdControl].FontSelected : gui.SelectedFont];
	cid_t				iPreviousVisible		= -1;
	for(u2_t iItem = 0; iItem < menu.IdControls.size(); ++iItem) {
		const cid_t		idControl				= menu.IdControls[iItem];
		SControlConstraints	& controlConstraints	= gui.Controls.Constraints	[idControl];
		SControlPlacement	& control				= gui.Controls.Placement	[idControl];
		SControlText			& controlText			= gui.Controls.Text			[idControl];
		controlConstraints.DockToControl					= {-1, -1, -1, -1};
		controlConstraints.AttachSizeToControl			= {-1, -1};
		if(gui.Controls.States[idControl].IsHidden())
			continue;
		const n2u0_t			fontCharSize			= selectedFont.CharSize;
		control.Area.Size.y		= fontCharSize.y + controlNCSpacing(control).y;
		switch(menu.Orientation) {
		default:
		case DIRECTION_LEFT	:
		case DIRECTION_RIGHT	:
			control.Area.Size.x		= s1_t(fontCharSize.x * controlText.Text.size() + controlNCSpacing(control).x);
			control.Area.Offset.x	= 0;
			if(-1 != iPreviousVisible)
				controlConstraints.DockToControl.Right	= iPreviousVisible;
			break;
		case DIRECTION_UP	:
		case DIRECTION_DOWN	:
			controlConstraints.AttachSizeToControl.x	= menu.IdControl;
			control.Area.Offset.y	= 0;
			break;
		}
		if(-1 != iPreviousVisible) {
			if(DIRECTION_LEFT == menu.Orientation)
				controlConstraints.DockToControl.Left	= iPreviousVisible;
			else if(DIRECTION_UP == menu.Orientation)
				controlConstraints.DockToControl.Top		= iPreviousVisible;
			else if(DIRECTION_DOWN == menu.Orientation)
				controlConstraints.DockToControl.Bottom	= iPreviousVisible;
			else
				controlConstraints.DockToControl.Right	= iPreviousVisible;
		}
		iPreviousVisible			= idControl;
	}

	SControlPlacement	& controlMenu			= gui.Controls.Placement[menu.IdControl];
	controlMenu.Area.Size	= {};
	const n2u0_t			fontCharSize			= selectedFont.CharSize;
	for(u2_t iItem = 0; iItem < menu.IdControls.size(); ++iItem) {
		const u2_t				idControl			= menu.IdControls		[iItem];
		if(gui.Controls.States[idControl].IsHidden())
			continue;
		SControlPlacement	& control			= gui.Controls.Placement[idControl];
		SControlText			& controlText		= gui.Controls.Text		[idControl];
		switch(menu.Orientation) {
		default:
		case DIRECTION_LEFT	:
		case DIRECTION_RIGHT	:
			controlMenu.Area.Size.x	+= gui.Controls.Placement[idControl].Area.Size.x;
			controlMenu.Area.Size.y	= max(controlMenu.Area.Size.y, gui.Controls.Placement[idControl].Area.Size.y);
			break;
		case DIRECTION_UP		:
		case DIRECTION_DOWN		:
			controlMenu.Area.Size.y	+= gui.Controls.Placement[idControl].Area.Size.y;
			controlMenu.Area.Size.x	= max(controlMenu.Area.Size.x, (s1_t)(controlText.Text.size() * fontCharSize.x + controlNCSpacing(control).x));
			break;
		}
	}
	if(DIRECTION_DOWN == menu.Orientation || DIRECTION_UP == menu.Orientation)
		controlMenu.Area.Size.x	+= 16;

	return 0;
}

gpk::err_t 		gpk::controlListPush	(SGUI & gui, SControlList& menu, const vcst_t& text)				{
	if(menu.IdControl == -1)
		if_fail_fe(gpk::controlListInitialize(gui, menu));

	const cid_t					idControl				= controlCreate(gui);
	if_fail_fef(idControl, "%s", "Failed to create control! Out of memory?");
	controlSetParent(gui, idControl, menu.IdControl);
	SControlPlacement	& control				= gui.Controls.Placement[idControl];
	SControlText			& controlText			= gui.Controls.Text		[idControl];
	control		.Margin		= {4, 2, 4, 2};
	controlText	.Text		= {text.begin(), text.size()};
	controlText	.Align		= ALIGN_CENTER_LEFT;
	menu.IdControls	.push_back(idControl);
	gpk::controlListArrange(gui, menu);
	return 0;
}

gpk::err_t 		gpk::paletteGridInitialize	(SGUI & gui, SPaletteGrid& palette)				{
	const n2f3_t			targetSize					= {256.0, 256.0};
	palette.IdControl		= controlCreate(gui);

	SControlPlacement	& control					= gui.Controls.Placement[palette.IdControl];
	const u2_t				widthViewport				= (u2_t)(targetSize.x + ((s3_t)control.Border.Left + control.Border.Right + control.Margin.Left + control.Margin.Right));
	const u2_t				heightViewport				= (u2_t)(targetSize.y + ((s3_t)control.Border.Top + control.Border.Bottom + control.Margin.Top + control.Margin.Bottom));
	control.Area.Size		= {(s1_t)widthViewport, (s1_t)heightViewport};
	control.Align			= ALIGN_CENTER;
	return 0;
}

gpk::err_t 		gpk::paletteGridColorsSet	(SGUI & gui, SPaletteGrid& palette, const g8bgra& colors)			{
	if(-1 == palette.IdControl)
		if_fail_e(paletteGridInitialize(gui, palette));

	//gui.Controls.Area[palette.IdControl];

	const n2u2_t			& gridMetrics				= colors.metrics();
	if_fail_fe(palette.IdControls.resize(colors.size()));
	palette.Colors			= colors;

	const SControlPlacement	& controlPalette			= gui.Controls.Placement[palette.IdControl];
	const n2f3_t			targetSize					= controlPalette.Area.Size.f3() - controlNCSpacing(controlPalette).f3();
	n2f3_t				controlSize					= {targetSize.x / colors.metrics().x, targetSize.y / colors.metrics().y};

	for(u2_t y = 0; y < gridMetrics.y; ++y)
	for(u2_t x = 0; x < gridMetrics.x; ++x) {
		const s2_t				linearIndex					= y * gridMetrics.x + x;
		u2_t					iFoundColor					= 0;
		s2_t						themeIndex					= -1;
		const bgra			& gridCellColor				= colors[y][x];
		for(; iFoundColor < gui.Colors->Palette->size(); ++iFoundColor)
			if((*gui.Colors->Palette)[iFoundColor] == gridCellColor) {
				themeIndex				= iFoundColor;
				break;
			}
		if(iFoundColor >= gui.Colors->Palette->size()) {
			s2_t						indexColor			= gui.Colors->Palette->push_back(gridCellColor);
			themeIndex				= gui.Colors->ControlThemes->push_back({});
			SControlTheme		& theme				= (*gui.Colors->ControlThemes)[themeIndex];
			astu2_t<UI_CONTROL_AREA_COUNT>	& comboDisabled		= theme.ColorCombos[GUI_CONTROL_PALETTE_DISABLED	] = {};
			astu2_t<UI_CONTROL_AREA_COUNT>	& comboPressed 		= theme.ColorCombos[GUI_CONTROL_PALETTE_PRESSED	] = {};
			astu2_t<UI_CONTROL_AREA_COUNT>	& comboSelected		= theme.ColorCombos[GUI_CONTROL_PALETTE_SELECTED	] = {};
			astu2_t<UI_CONTROL_AREA_COUNT>	& comboHover 		= theme.ColorCombos[GUI_CONTROL_PALETTE_HOVER	] = {};
			astu2_t<UI_CONTROL_AREA_COUNT>	& comboNormal		= theme.ColorCombos[GUI_CONTROL_PALETTE_NORMAL	] = {};
			comboDisabled	[UI_CONTROL_AREA_BACKGROUND	]	= indexColor;
			comboDisabled	[UI_CONTROL_AREA_CLIENT		]	= comboDisabled[UI_CONTROL_AREA_BACKGROUND];
			comboNormal		[UI_CONTROL_AREA_BACKGROUND	]	= indexColor;
			comboNormal		[UI_CONTROL_AREA_CLIENT		]	= comboDisabled[UI_CONTROL_AREA_BACKGROUND];
			comboSelected	[UI_CONTROL_AREA_BACKGROUND	]	= indexColor;
			comboSelected	[UI_CONTROL_AREA_CLIENT		]	= comboDisabled[UI_CONTROL_AREA_BACKGROUND];
			comboHover		[UI_CONTROL_AREA_BACKGROUND	]	= indexColor;
			comboHover		[UI_CONTROL_AREA_CLIENT		]	= comboDisabled[UI_CONTROL_AREA_BACKGROUND];
			comboPressed	[UI_CONTROL_AREA_BACKGROUND	]	= indexColor;
			comboPressed	[UI_CONTROL_AREA_CLIENT		]	= comboDisabled[UI_CONTROL_AREA_BACKGROUND];
			//for(u2_t iState = 0; iState < theme.ColorCombos.size(); ++iState)
			//for(u2_t iArea = 0; iArea < GUI_CONTROL_COLOR_COUNT; ++iArea)
			//	theme.ColorCombos[iState][iArea]	= min((u2_t)theme.ColorCombos[iState][iArea], gui.Palette.size() - 1);
		}
		const u2_t				paletteElemIndex	= linearIndex;
		palette.IdControls[paletteElemIndex] = controlCreate(gui);
		SControlPlacement	& control			= gui.Controls.Placement[palette.IdControls[paletteElemIndex]];
		SControlDraw			& controlDraw		= gui.Controls.Draw[palette.IdControls[paletteElemIndex]];
		controlDraw.ColorTheme	= s1_t(themeIndex + 1);
		control.Area.Offset		= {(s1_t)(controlSize.x * x), (s1_t)(controlSize.y * y)};
		control.Area.Size		= controlSize.s1();
		if_fail_fe(gpk::controlSetParent(gui, palette.IdControls[paletteElemIndex], palette.IdControl));
	}
	return 0;
}

gpk::cid_t			gpk::guiCreateControlList	(SGUI & gui, DIRECTION direction, view<vcst_c> buttonText, cid_t iParent, const SControlPlacement & args, ALIGN textAlign, vcid out_ids) {
	cid_t					result						= -1;
	for(u1_t iButton = 0; iButton < buttonText.size(); ++iButton) {
		cid_t				idControl					= controlCreate(gui);
		if(out_ids.size() > iButton)
			out_ids[iButton]		= idControl;

		if(0 == iButton)
			result					= idControl;

		SControlPlacement	& control					= gui.Controls.Placement[idControl];
		control					= args;
		if(direction & (DIRECTION_LEFT | DIRECTION_RIGHT))
			control.Area.Offset.x	+= s1_t(args.Area.Size.x * iButton);
		else
			control.Area.Offset.y	+= s1_t(args.Area.Size.y * iButton);

		SControlText			& controlText				= gui.Controls.Text[idControl];
		controlText.Text		= buttonText[iButton];
		controlText.Align		= textAlign;

		SControlConstraints	& itemConstraints	= gui.Controls.Constraints[idControl];
		if(direction & (DIRECTION_UP | DIRECTION_DOWN))
			itemConstraints.AttachSizeToControl.x	= iParent;
		else if(direction & (DIRECTION_LEFT | DIRECTION_RIGHT)) {
			itemConstraints.AttachSizeToText.x		= true;
			itemConstraints.AttachSizeToControl.y	= iParent;
		}

		if(iButton) {
			if(direction & DIRECTION_LEFT)
				itemConstraints.DockToControl.Left	= out_ids[iButton - 1];
			else if(direction & DIRECTION_RIGHT)
				itemConstraints.DockToControl.Right	= out_ids[iButton - 1];
			else if(direction & DIRECTION_UP)
				itemConstraints.DockToControl.Top	= out_ids[iButton - 1];
			else if(direction & DIRECTION_DOWN)
				itemConstraints.DockToControl.Bottom	= out_ids[iButton - 1];

		}
		if_fail_fef(gpk::controlSetParent(gui, idControl, iParent), "idControl: %i, buttonText[iButton]: %.*s", idControl, GPK_VIEW_ARGS(buttonText[iButton])); // Make a tplt and macro for this pattern
	}
	return result;
}

gpk::cid_t			gpk::guiCreateControlList		(SGUI & gui, view<vcst_c> buttonText, cid_t iParent, n2u1_c & buttonSize, n2s1_c & offset, ALIGN controlAlign, ALIGN textAlign, vcid out_ids) {
	SControlPlacement		controlArgs					= {};
	controlArgs.Align	= controlAlign;
	controlArgs.Area	= {offset, buttonSize.s1()};
	controlArgs.Border	= {1, 1, 1, 1};
	controlArgs.Margin	= {1, 1, 1, 1};
	return guiCreateControlList(gui, {}, buttonText, iParent, controlArgs, textAlign, out_ids);
}
