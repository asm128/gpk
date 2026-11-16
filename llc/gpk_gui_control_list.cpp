#include "gpk_gui_control_list.h"
#include "gpk_label.h"

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

gpk::err_t		gpk::controlListArrange	(SGUI & gui, SControlList & menu)													{
	const SRasterFont	& selectedFont			= *gui.Fonts[in_range(gui.Controls.Text[menu.IdControl].FontSelected, (int16_t)0, (int16_t)gui.Fonts.size()) ? gui.Controls.Text[menu.IdControl].FontSelected : gui.SelectedFont];
	for(u2_t iItem = 0; iItem < menu.IdControls.size(); ++iItem) {
		cnst cid_t		idControl				= menu.IdControls[iItem];
		SControlConstraints	& controlConstraints	= gui.Controls.Constraints	[idControl];
		SControlPlacement	& control				= gui.Controls.Placement	[idControl];
		SControlText			& controlText			= gui.Controls.Text			[idControl];
		const uint32_t				iPrevItem				= iItem - 1;
		const n2u0_t			fontCharSize			= selectedFont.CharSize;
		control.Area.Size.y		= fontCharSize.y + controlNCSpacing(control).y;
		switch(menu.Orientation) {
		default:
		case DIRECTION_RIGHT	:
			control.Area.Size.x		= int16_t(fontCharSize.x * controlText.Text.size() + controlNCSpacing(control).x);
			if(menu.IdControls.size() > iPrevItem)
				control.Area.Offset.x	= gui.Controls.Placement[menu.IdControls[iPrevItem]].Area.Limit().x;
			break;
		case DIRECTION_DOWN:
			controlConstraints.AttachSizeToControl.x	= menu.IdControl;
			if(menu.IdControls.size() > iPrevItem)
				control.Area.Offset.y	= gui.Controls.Placement[menu.IdControls[iPrevItem]].Area.Limit().y;
			break;
		}
	}

	SControlPlacement			& controlMenu			= gui.Controls.Placement[menu.IdControl];
	controlMenu.Area.Size	= {};
	n2u0_c						fontCharSize			= selectedFont.CharSize;
	for(uint32_t iItem = 0; iItem < menu.IdControls.size(); ++iItem) {
		u2_c						idControl			= menu.IdControls		[iItem];
		SControlPlacement			& control			= gui.Controls.Placement[idControl];
		SControlText				& controlText		= gui.Controls.Text		[idControl];
		switch(menu.Orientation) {
		default:
		case DIRECTION_RIGHT	:
			controlMenu.Area.Size.x	+= gui.Controls.Placement[idControl].Area.Size.x;
			controlMenu.Area.Size.y	= max(controlMenu.Area.Size.y, gui.Controls.Placement[idControl].Area.Size.y);
			break;
		case DIRECTION_DOWN		:
			controlMenu.Area.Size.y	+= gui.Controls.Placement[idControl].Area.Size.y;
			controlMenu.Area.Size.x	= max(controlMenu.Area.Size.x, (s1_t)(controlText.Text.size() * fontCharSize.x + controlNCSpacing(control).x));
			break;
		}
	}
	if(DIRECTION_DOWN == menu.Orientation)
		controlMenu.Area.Size.x	+= 16;

	return 0;
}

gpk::err_t			gpk::controlListPush	(SGUI & gui, SControlList & menu, vcst_t text, s2_t eventCode)				{
	if(menu.IdControl == -1)
		if_fail_fe(gpk::controlListInitialize(gui, menu));

	cid_t						idControl;
	if_fail_fe(idControl = controlCreate(gui));
	controlSetParent(gui, idControl, menu.IdControl);
	SControlPlacement			& control				= gui.Controls.Placement[idControl];
	SControlText				& controlText			= gui.Controls.Text		[idControl];
	control		.Margin		= {4, 2, 4, 2};
	controlText	.Text		= {text.begin(), text.size()};
	controlText	.Align		= ALIGN_CENTER_LEFT;
	menu.IdControls	.push_back(idControl);
	menu.Events		.push_back(eventCode);
	gpk::controlListArrange(gui, menu);
	return 0;
}

gpk::err_t 		gpk::paletteGridInitialize	(SGUI & gui, SPaletteGrid & palette)				{
	cnst n2f3_t			targetSize					= {256.0, 256.0};
	palette.IdControl		= controlCreate(gui);

	SControlPlacement	& control					= gui.Controls.Placement[palette.IdControl];
	u2_c				widthViewport				= (u2_t)(targetSize.x + ((s3_t)control.Border.Left + control.Border.Right + control.Margin.Left + control.Margin.Right));
	u2_c				heightViewport				= (u2_t)(targetSize.y + ((s3_t)control.Border.Top + control.Border.Bottom + control.Margin.Top + control.Margin.Bottom));
	control.Area.Size		= {(s1_t)widthViewport, (s1_t)heightViewport};
	control.Align			= ALIGN_CENTER;
	return 0;
}

gpk::err_t 		gpk::paletteGridColorsSet	(SGUI & gui, SPaletteGrid & palette, cnst g8bgra & colors)			{
	if(-1 == palette.IdControl)
		if_fail_e(paletteGridInitialize(gui, palette));

	//gui.Controls.Area[palette.IdControl];

	cnst n2u2_t			& gridMetrics				= colors.metrics();
	if_fail_fe(palette.IdControls.resize(colors.size()));
	palette.Colors			= colors;

	const n2f3_t				targetSize		= gui.Controls.Area[palette.IdControl].Client.Local.Size.f3(); //{256.0, 256.0};
	n2f3_t						controlSize		= {targetSize.x / colors.metrics().x, targetSize.y / colors.metrics().y};

	for(u2_t y = 0; y < gridMetrics.y; ++y)
	for(u2_t x = 0; x < gridMetrics.x; ++x) {
		cnst s2_t			linearIndex					= y * gridMetrics.x + x;
		u2_t				iFoundColor					= 0;
		s2_t				themeIndex					= -1;
		cnst bgra			& gridCellColor				= colors[y][x];
		for(; iFoundColor < gui.Colors->Palette->size(); ++iFoundColor)
			if((*gui.Colors->Palette)[iFoundColor] == gridCellColor) {
				themeIndex		= iFoundColor;
				break;
			}
		if(iFoundColor >= gui.Colors->Palette->size()) {
			s2_t				indexColor			= gui.Colors->Palette->push_back(gridCellColor);
			themeIndex		= gui.Colors->ControlThemes->push_back({});
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
		u2_c						paletteElemIndex	= linearIndex;
		palette.IdControls[paletteElemIndex]	= controlCreate(gui);
		SControlPlacement			& control			= gui.Controls.Placement[palette.IdControls[paletteElemIndex]];
		SControlDraw				& controlDraw		= gui.Controls.Draw[palette.IdControls[paletteElemIndex]];
		controlDraw.ColorTheme	= s1_t(themeIndex + 1);
		control.Area.Offset		= {(s1_t)(controlSize.x * x), (s1_t)(controlSize.y * y)};
		control.Area.Size		= controlSize.s1();
		if_fail_fe(gpk::controlSetParent(gui, palette.IdControls[paletteElemIndex], palette.IdControl));
	}
	return 0;
}
