#include "gpk_gui_control.h"
#include "gpk_label.h"


::gpk::error_t			gpk::controlListInitialize			(::gpk::SGUI & gui, ::gpk::SControlList& menu)													{
	menu					= {};
	gpk_necall(menu.IdControl = ::gpk::controlCreate(gui), "%s", "Failed to create menu control!");
	gui.Controls.Controls		[menu.IdControl].Border				= {};
	gui.Controls.Controls		[menu.IdControl].Margin				= {};
	gui.Controls.Text			[menu.IdControl].Text				= " ";
	gui.Controls.Constraints	[menu.IdControl].AttachSizeToText	= {false, false};
	//gui.Controls.Modes			[menu.IdControl].Design				= true;
	return 0;
}

::gpk::error_t			gpk::controlListArrange				(::gpk::SGUI & gui, ::gpk::SControlList& menu)													{
	const ::gpk::SRasterFont	& selectedFont						= *gui.Fonts[::gpk::in_range(gui.Controls.Text[menu.IdControl].FontSelected, (int16_t)0, (int16_t)gui.Fonts.size()) ? gui.Controls.Text[menu.IdControl].FontSelected : gui.SelectedFont];
	for(uint32_t iItem = 0; iItem < menu.IdControls.size(); ++iItem) {
		const uint32_t				idControl							= menu.IdControls[iItem];
		::gpk::SControlConstraints	& controlConstraints				= gui.Controls.Constraints	[idControl];
		::gpk::SControl				& control							= gui.Controls.Controls		[idControl];
		::gpk::SControlText			& controlText						= gui.Controls.Text			[idControl];
		const uint32_t				iPrevItem							= iItem - 1;
		const ::gpk::n2u8			fontCharSize						= selectedFont.CharSize;
		control.Area.Size.y		= fontCharSize.y + ::gpk::controlNCSpacing(control).y;
		switch(menu.Orientation) {
		default:
		case ::gpk::CONTROL_LIST_DIRECTION_HORIZONTAL	:
			control.Area.Size.x												= int16_t(fontCharSize.x * controlText.Text.size() + ::gpk::controlNCSpacing(control).x);
			if(menu.IdControls.size() > iPrevItem)
				control.Area.Offset.x											= gui.Controls.Controls[menu.IdControls[iPrevItem]].Area.Limit().x;
			break;
		case ::gpk::CONTROL_LIST_DIRECTION_VERTICAL		:
			controlConstraints.AttachSizeToControl.x						= menu.IdControl;
			if(menu.IdControls.size() > iPrevItem)
				control.Area.Offset.y											= gui.Controls.Controls[menu.IdControls[iPrevItem]].Area.Limit().y;
			break;
		}
	}

	::gpk::SControl				& controlMenu							= gui.Controls.Controls[menu.IdControl];
	controlMenu.Area.Size	= {};
	const ::gpk::n2u8			fontCharSize							= selectedFont.CharSize;
	for(uint32_t iItem = 0; iItem < menu.IdControls.size(); ++iItem) {
		const uint32_t				idControl								= menu.IdControls		[iItem];
		::gpk::SControl				& control								= gui.Controls.Controls	[idControl];
		::gpk::SControlText			& controlText							= gui.Controls.Text		[idControl];
		switch(menu.Orientation) {
		default:
		case ::gpk::CONTROL_LIST_DIRECTION_HORIZONTAL	:
			controlMenu.Area.Size.x	+= gui.Controls.Controls[idControl].Area.Size.x;
			controlMenu.Area.Size.y	= ::gpk::max(controlMenu.Area.Size.y, gui.Controls.Controls[idControl].Area.Size.y);
			break;
		case ::gpk::CONTROL_LIST_DIRECTION_VERTICAL		:
			controlMenu.Area.Size.y	+= gui.Controls.Controls[idControl].Area.Size.y;
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
	::gpk::SControl				& control				= gui.Controls.Controls		[idControl];
	::gpk::SControlText			& controlText			= gui.Controls.Text			[idControl];
	control		.Margin		= {4, 2, 4, 2};
	controlText	.Text		= {text.begin(), text.size()};
	controlText	.Align		= ::gpk::ALIGN_CENTER_LEFT;
	menu.IdControls	.push_back(idControl);
	menu.Events		.push_back(eventCode);
	gpk::controlListArrange(gui, menu);
	return 0;
}

::gpk::error_t			gpk::viewportInitialize	(::gpk::SGUI & gui, ::gpk::SViewport& viewport)			{
	const ::gpk::n2u8			fontCharSize			= gui.Fonts[gui.SelectedFont]->CharSize;
	int16_t						heightTitleBar			= fontCharSize.y + 4;
	uint32_t					widthTarget				= 800;
	const ::gpk::n2f64			targetSize				= {(double)widthTarget, widthTarget * (9 / 16.0)};

	//if(-1 == viewport.IdControl)
	{
		viewport.IdControl		= ::gpk::controlCreate(gui);
		//::gpk::SControlMode			& controlMode						= gui.Controls.Modes		[viewport.IdControl];
		//controlMode.Design		= true;
		::gpk::SControl				& control							= gui.Controls.Controls	[viewport.IdControl];
		control.Align			= ::gpk::ALIGN_TOP_LEFT;
		control.Border			= {1, 1, 1, 1};
		control.Margin			= {1, 1, 1, 1};

		const uint32_t				widthViewport						= (uint32_t)(targetSize.x + ::gpk::controlNCSpacing(control).x);
		const uint32_t				heightViewport						= (uint32_t)(targetSize.y + ((int64_t)heightTitleBar + ::gpk::controlNCSpacing(control).y));
		control.Area.Size		= {(int16_t)widthViewport, (int16_t)heightViewport};
		for(uint32_t iElement = 0; iElement < viewport.IdControls.size(); ++iElement) {
			gpk_necall(viewport.IdControls[iElement] = ::gpk::controlCreate(gui), "%s", "This shouldn't fail");
			::gpk::SControlConstraints	& controlConstraints					= gui.Controls.Constraints	[viewport.IdControls[iElement]];
			controlConstraints.AttachSizeToControl.x								= viewport.IdControls[iElement];
			gpk_necall(::gpk::controlSetParent(gui, viewport.IdControls[iElement], viewport.IdControl), "%s", "Maybe the tree integrity has been broken");
		}
		for(uint32_t iElement = 0; iElement < VIEWPORT_CONTROL_TITLE; ++iElement) {
			gui.Controls.Controls		[viewport.IdControls[iElement]].Area.Size			= {16, 16};
			gui.Controls.Controls		[viewport.IdControls[iElement]].Area.Offset			= {-2, -2};
			gui.Controls.Constraints	[viewport.IdControls[iElement]].AttachSizeToControl	= {-1, -1};
		}
	}
	{
		gui.Controls.Controls[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_LEFT			]].Align = ::gpk::ALIGN_CENTER_LEFT		;
		gui.Controls.Controls[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_TOP			]].Align = ::gpk::ALIGN_CENTER_TOP		;
		gui.Controls.Controls[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_RIGHT			]].Align = ::gpk::ALIGN_CENTER_RIGHT	;
		gui.Controls.Controls[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_BOTTOM		]].Align = ::gpk::ALIGN_CENTER_BOTTOM	;
		gui.Controls.Controls[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_TOP_LEFT		]].Align = ::gpk::ALIGN_TOP_LEFT		;
		gui.Controls.Controls[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_TOP_RIGHT		]].Align = ::gpk::ALIGN_TOP_RIGHT		;
		gui.Controls.Controls[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_BOTTOM_LEFT	]].Align = ::gpk::ALIGN_BOTTOM_LEFT		;
		gui.Controls.Controls[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_BOTTOM_RIGHT	]].Align = ::gpk::ALIGN_BOTTOM_RIGHT	;

	}
	{
		::gpk::SControl								& control								= gui.Controls.Controls		[viewport.IdControls[VIEWPORT_CONTROL_CLOSE]];
		::gpk::SControlText							& controlText							= gui.Controls.Text			[viewport.IdControls[VIEWPORT_CONTROL_CLOSE]];
		control.Area.Size						= {(int16_t)heightTitleBar, (int16_t)heightTitleBar};
		control.Align							= ::gpk::ALIGN_TOP_RIGHT;
		controlText.Text						= {"X", (uint32_t)1};
		controlText.Align						= ::gpk::ALIGN_CENTER;
		::gpk::SControlConstraints					& controlConstraints					= gui.Controls.Constraints	[viewport.IdControls[VIEWPORT_CONTROL_CLOSE]];
		controlConstraints.AttachSizeToControl	= {-1, -1};
	}
	{
		::gpk::SControl								& control								= gui.Controls.Controls		[viewport.IdControls[VIEWPORT_CONTROL_TITLE]];
		::gpk::SControlText							& controlText							= gui.Controls.Text			[viewport.IdControls[VIEWPORT_CONTROL_TITLE]];
		control.Area.Size.y						= heightTitleBar;
		static	int32_t								counterVPs								= 0;
		++counterVPs;
		char										buffer [256];
		const int32_t								titleLen								= snprintf(buffer, ::gpk::size(buffer) - 2, "Viewport #%i", counterVPs);
		controlText.Text						= {::gpk::label(buffer).begin(), (uint32_t)titleLen};
		controlText.Align						= ::gpk::ALIGN_CENTER;
		control.Area.Offset.x					= heightTitleBar;
		control.Align							= ::gpk::ALIGN_TOP_RIGHT;
		::gpk::SControlConstraints					& controlConstraints					= gui.Controls.Constraints	[viewport.IdControls[VIEWPORT_CONTROL_TITLE]];
		controlConstraints.DockToControl		= {-1, -1, viewport.IdControls[VIEWPORT_CONTROL_CLOSE], -1};
	}
	{
		::gpk::SControl								& control								= gui.Controls.Controls		[viewport.IdControls[VIEWPORT_CONTROL_TARGET]];
		::gpk::SControlText							& controlText							= gui.Controls.Text			[viewport.IdControls[VIEWPORT_CONTROL_TARGET]];
		controlText.Align						= ::gpk::ALIGN_CENTER_TOP;
		control.Border							= {};
		control.Margin							= {};
		control.Area.Size						= targetSize.i16();
		//control.Area.Offset.y					= heightTitleBar;
		control.ColorTheme						= 60;
		::gpk::SControlConstraints					& controlConstraints					= gui.Controls.Constraints	[viewport.IdControls[VIEWPORT_CONTROL_TARGET]];
		controlConstraints.DockToControl		= {-1, -1, -1, viewport.IdControls[VIEWPORT_CONTROL_TITLE]};
		//controlConstraints.AttachSizeToControl	= {-1, viewport.IdControl};
	}
	//for(uint32_t i = 0; i < 2; ++i)
	//	::gpk::controlUpdateMetricsTopToDown(gui, viewport.IdControl, gui.LastSize, true);
	return 0;
}

::gpk::error_t			gpk::paletteGridInitialize	(::gpk::SGUI & gui, ::gpk::SPaletteGrid& palette)				{
	const ::gpk::n2f64			targetSize					= {256.0, 256.0};
	palette.IdControl		= ::gpk::controlCreate(gui);
	::gpk::SControl				& control					= gui.Controls.Controls[palette.IdControl];
	const uint32_t				widthViewport				= (uint32_t)(targetSize.x + ((int64_t)control.Border.Left + control.Border.Right + control.Margin.Left + control.Margin.Right));
	const uint32_t				heightViewport				= (uint32_t)(targetSize.y + ((int64_t)control.Border.Top + control.Border.Bottom + control.Margin.Top + control.Margin.Bottom));
	control.Area.Size		= {(int16_t)widthViewport, (int16_t)heightViewport};
	control.Align			= ::gpk::ALIGN_CENTER;
	return 0;
}

::gpk::error_t			gpk::paletteGridColorsSet				(::gpk::SGUI & gui, ::gpk::SPaletteGrid& palette, const ::gpk::view2d<::gpk::bgra>& colors)			{
	if(-1 == palette.IdControl)
		::gpk::paletteGridInitialize(gui, palette);

	//gui.Controls.Metrics[palette.IdControl];

	const ::gpk::n2u32			& gridMetrics							= colors.metrics();
	palette.IdControls.resize(colors.size());
	palette.Colors			= colors;

	const ::gpk::n2f64			targetSize								= gui.Controls.Metrics[palette.IdControl].Client.Global.Size.f64(); //{256.0, 256.0};
	::gpk::n2f64				controlSize								= {targetSize.x / colors.metrics().x, targetSize.y / colors.metrics().y};

	for(uint32_t y = 0; y < gridMetrics.y; ++y)
	for(uint32_t x = 0; x < gridMetrics.x; ++x) {
		const int32_t				linearIndex								= y * gridMetrics.x + x;
		uint32_t					iFoundColor								= 0;
		int32_t						themeIndex								= -1;
		const ::gpk::bgra			& gridCellColor							= colors[y][x];
		for(; iFoundColor < gui.Colors->Palette->size(); ++iFoundColor)
			if((*gui.Colors->Palette)[iFoundColor] == gridCellColor) {
				themeIndex															= iFoundColor;
				break;
			}
		if(iFoundColor >= gui.Colors->Palette->size()) {
			int32_t											indexColor								= gui.Colors->Palette->push_back(gridCellColor);
			themeIndex									= gui.Colors->ControlThemes->push_back({});
			::gpk::SControlTheme							& theme									= (*gui.Colors->ControlThemes)[themeIndex];
			::gpk::astu32<::gpk::GUI_CONTROL_AREA_EX_COUNT>	& comboDisabled		= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_DISABLED	] = {};
			::gpk::astu32<::gpk::GUI_CONTROL_AREA_EX_COUNT>	& comboPressed 		= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_PRESSED	] = {};
			::gpk::astu32<::gpk::GUI_CONTROL_AREA_EX_COUNT>	& comboSelected		= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_SELECTED	] = {};
			::gpk::astu32<::gpk::GUI_CONTROL_AREA_EX_COUNT>	& comboHover 		= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_HOVER	] = {};
			::gpk::astu32<::gpk::GUI_CONTROL_AREA_EX_COUNT>	& comboNormal		= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_NORMAL	] = {};
			comboDisabled	[::gpk::GUI_CONTROL_AREA_EX_BACKGROUND	]	= indexColor;
			comboDisabled	[::gpk::GUI_CONTROL_AREA_EX_CLIENT		]	= comboDisabled[::gpk::GUI_CONTROL_AREA_EX_BACKGROUND];
			comboNormal		[::gpk::GUI_CONTROL_AREA_EX_BACKGROUND	]	= indexColor;
			comboNormal		[::gpk::GUI_CONTROL_AREA_EX_CLIENT		]	= comboDisabled[::gpk::GUI_CONTROL_AREA_EX_BACKGROUND];
			comboSelected	[::gpk::GUI_CONTROL_AREA_EX_BACKGROUND	]	= indexColor;
			comboSelected	[::gpk::GUI_CONTROL_AREA_EX_CLIENT		]	= comboDisabled[::gpk::GUI_CONTROL_AREA_EX_BACKGROUND];
			comboHover		[::gpk::GUI_CONTROL_AREA_EX_BACKGROUND	]	= indexColor;
			comboHover		[::gpk::GUI_CONTROL_AREA_EX_CLIENT		]	= comboDisabled[::gpk::GUI_CONTROL_AREA_EX_BACKGROUND];
			comboPressed	[::gpk::GUI_CONTROL_AREA_EX_BACKGROUND	]	= indexColor;
			comboPressed	[::gpk::GUI_CONTROL_AREA_EX_CLIENT		]	= comboDisabled[::gpk::GUI_CONTROL_AREA_EX_BACKGROUND];
			//for(uint32_t iState = 0; iState < theme.ColorCombos.size(); ++iState)
			//	for(uint32_t iArea = 0; iArea < ::gpk::GUI_CONTROL_COLOR_COUNT; ++iArea)
			//		theme.ColorCombos[iState][iArea]									= ::gpk::min((uint32_t)theme.ColorCombos[iState][iArea], gui.Palette.size() - 1);
		}
		const uint32_t															paletteElemIndex						= linearIndex;
		palette.IdControls[paletteElemIndex]								= ::gpk::controlCreate(gui);
		::gpk::SControl															& control								= gui.Controls.Controls[palette.IdControls[paletteElemIndex]];
		control.ColorTheme													= int16_t(themeIndex + 1);
		control.Area.Offset													= {(int16_t)(controlSize.x * x), (int16_t)(controlSize.y * y)};
		control.Area.Size													= controlSize.i16();
		gpk_necs(::gpk::controlSetParent(gui, palette.IdControls[paletteElemIndex], palette.IdControl));
	}
	return 0;
}

::gpk::error_t			gpk::guiSetupButtonList			(::gpk::SGUI & gui, ::gpk::view<const ::gpk::vcc> buttonText, int32_t iParent, const ::gpk::n2<uint16_t> & buttonSize, const ::gpk::n2<int16_t> & offset, ::gpk::ALIGN controlAlign, ::gpk::ALIGN textAlign, ::gpk::view<int32_t> out_ids) {
	int32_t								result							= -1;
	for(uint16_t iButton = 0; iButton < buttonText.size(); ++iButton) {
		int32_t								idControl						= ::gpk::controlCreate(gui);
		if(out_ids.size() > iButton)
			out_ids[iButton] = idControl;

		if(0 == iButton)
			result = idControl;
		::gpk::SControl						& control						= gui.Controls.Controls[idControl];
		control.Area					= {{offset.x, (int16_t)(buttonSize.y * iButton + offset.y)}, buttonSize.i16()};
		control.Border					= {1, 1, 1, 1};//{10, 10, 10, 10};
		control.Margin					= {1, 1, 1, 1};
		control.Align					= controlAlign;
		::gpk::SControlText					& controlText					= gui.Controls.Text[idControl];
		controlText.Text				= buttonText[iButton];
		controlText.Align				= textAlign;
		gpk_necall(::gpk::controlSetParent(gui, idControl, iParent), "idControl: %i, buttonText[iButton]: %s", idControl, ::gpk::toString(buttonText[iButton]).begin()); // Make a template and macro for this pattern
	}
	return result;
}


::gpk::error_t			gpk::virtualKeyboardSetup437(::gpk::SGUI & gui, ::gpk::SVirtualKeyboard & vk)	{
	::gpk::apod<uint16_t>			keys;
	for(uint16_t i = 1; i < 255; ++i) {
		keys.push_back(i);
	}
	keys.push_back('!');
	keys.push_back('?');
	gpk_necs(::gpk::virtualKeyboardSetup(gui, vk, 32, ::gpk::view_const_uint16{keys}));
	gui.Controls.States[vk.IdRoot].Hidden	= true;
	return vk.IdRoot;
}


::gpk::error_t			gpk::virtualKeyboardSetup	(::gpk::SGUI & gui, ::gpk::SVirtualKeyboard & vk, uint8_t rowWidth, const ::gpk::view<const uint16_t> & keys) {
	vk.Keys							= keys;

	vk.IdRoot						= ::gpk::controlCreate(gui);
	const ::gpk::n2u8		charSize					= gui.Fonts[gui.SelectedFont]->CharSize;
	const ::gpk::n2<int16_t>		sizeKey						= (charSize + ::gpk::n2u8{6, 6}).i16();
	stacxpr	uint16_t			SIZE_BUTTON					= 88;
	const ::gpk::n2<int16_t>		sizeKeypad					= {int16_t(sizeKey.x * rowWidth + 4 + SIZE_BUTTON), int16_t(sizeKey.y * (keys.size() / rowWidth) + 4)};

	{
		::gpk::SControl						& controlRoot				= gui.Controls.Controls[vk.IdRoot];
		controlRoot.Area.Size			= sizeKeypad;
		controlRoot.Align				= ::gpk::ALIGN_CENTER_BOTTOM;
	}
	for(uint32_t iKey = 0; iKey < vk.Keys.size(); ++iKey) {
		char								text[8]		= {};
		sprintf_s(text, "%c", vk.Keys[iKey]);

		int32_t								idKey			= ::gpk::controlCreate(gui);
		::gpk::SControl						& controlKey	= gui.Controls.Controls[idKey];
		controlKey.Area.Size			= sizeKey;
		controlKey.Area.Offset.x		= (int16_t)((iKey % rowWidth) * sizeKey.x);
		controlKey.Area.Offset.y		= (int16_t)((iKey / rowWidth) * sizeKey.y);
		controlKey.Border				= {1, 1, 1, 1};
		//controlKey.ColorTheme			= ::gpk::ASCII_COLOR_DARKGREY * 16 + 8;
		gui.Controls.Text[idKey].Align	= ::gpk::ALIGN_CENTER;

		gpk_necs(::gpk::controlSetParent(gui, idKey, vk.IdRoot));
		gpk_necs(::gpk::controlTextSet(gui, idKey, ::gpk::label(text)));
	}

	gpk_necs(::gpk::guiSetupButtonList<::gpk::VK_SCANCODE>(gui, vk.IdRoot, SIZE_BUTTON, 0, ::gpk::ALIGN_TOP_RIGHT));
	return 0;
}

::gpk::error_t			gpk::inputBoxCreate		(::gpk::SUIInputBox & inputBox, ::gpk::SGUI & gui, int32_t iParent)	{
		gpk_necs(inputBox.IdRoot = ::gpk::controlCreate(gui));
		gui.Controls.Controls	[inputBox.IdRoot].Border			= {};
		gui.Controls.Controls	[inputBox.IdRoot].Margin			= {};
		gui.Controls.Modes		[inputBox.IdRoot].Design			= true;
		gui.Controls.Modes		[inputBox.IdRoot].NoBackgroundRect	= true;

		gpk_necs(inputBox.IdText = ::gpk::controlCreate(gui));
		gui.Controls.Controls	[inputBox.IdText].Border	= {2, 2, 2, 2};
		gui.Controls.Controls	[inputBox.IdText].Margin	= {1, 1, 1, 1};
		gui.Controls.Controls	[inputBox.IdText].Area.Size	= {0, 22};
		gui.Controls.Text		[inputBox.IdText].Align		= ::gpk::ALIGN_CENTER;

		gpk_necs(::gpk::virtualKeyboardSetup437(gui, inputBox.VirtualKeyboard));
		gui.Controls.States		[inputBox.VirtualKeyboard.IdRoot].Hidden		= false;
		gui.Controls.Controls	[inputBox.VirtualKeyboard.IdRoot].Align			= gui.Controls.Controls[inputBox.IdText].Align;
		gui.Controls.Controls	[inputBox.VirtualKeyboard.IdRoot].Area.Offset.y
			= gui.Controls.Controls	[inputBox.IdText].Area.Offset.y
			+ gui.Controls.Controls	[inputBox.IdText].Area.Size.y
			;
		gui.Controls.Controls	[inputBox.IdText].Area.Size.x	= gui.Controls.Controls	[inputBox.VirtualKeyboard.IdRoot].Area.Size.x;

		gui.Controls.Controls[inputBox.IdRoot].Area.Size.x	= ::gpk::max(gui.Controls.Controls[inputBox.IdText].Area.Size.x, gui.Controls.Controls[inputBox.VirtualKeyboard.IdRoot].Area.Size.x);
		gui.Controls.Controls[inputBox.IdRoot].Area.Size.y	+= gui.Controls.Controls[inputBox.VirtualKeyboard.IdRoot].Area.Size.y;

		gpk_necs(::gpk::controlSetParent(gui, inputBox.IdText					, inputBox.IdRoot));
		gpk_necs(::gpk::controlSetParent(gui, inputBox.VirtualKeyboard.IdRoot	, inputBox.IdRoot));
		if(iParent >= 0)
			gpk_necall(::gpk::controlSetParent(gui, inputBox.IdRoot, iParent), "iParent: %i", iParent);

		return inputBox.IdRoot;
	}
