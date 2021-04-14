#include "gpk_gui_control.h"
#include "gpk_label.h"


			::gpk::error_t												gpk::controlListInitialize				(::gpk::SGUI& gui, ::gpk::SControlList& menu)													{
	menu																	= {};
	gpk_necall(menu.IdControl = ::gpk::controlCreate(gui), "%s", "Failed to create menu control!");
	gui.Controls.Controls		[menu.IdControl].Border						= {};
	gui.Controls.Controls		[menu.IdControl].Margin						= {};
	gui.Controls.Text			[menu.IdControl].Text						= " ";
	gui.Controls.Constraints	[menu.IdControl].AttachSizeToText			= {false, false};
	//gui.Controls.Modes			[menu.IdControl].Design						= true;
	return 0;
}

			::gpk::error_t												gpk::controlListArrange					(::gpk::SGUI& gui, ::gpk::SControlList& menu)													{
	for(uint32_t iItem = 0; iItem < menu.IdControls.size(); ++iItem) {
		const uint32_t																idControl								= menu.IdControls[iItem];
		::gpk::SControlConstraints													& controlConstraints					= gui.Controls.Constraints	[idControl];
		::gpk::SControl																& control								= gui.Controls.Controls		[idControl];
		::gpk::SControlText															& controlText							= gui.Controls.Text			[idControl];
		const uint32_t																iPrevItem								= iItem - 1;
		control.Area.Size.y														= gui.FontCharSize.y + ::gpk::controlNCSpacing(control).y;
		switch(menu.Orientation) {
		default:
		case ::gpk::CONTROL_LIST_DIRECTION_HORIZONTAL	:
			control.Area.Size.x														= int16_t(gui.FontCharSize.x * controlText.Text.size() + ::gpk::controlNCSpacing(control).x);
			if(menu.IdControls.size() > iPrevItem)
				control.Area.Offset.x													= gui.Controls.Controls[menu.IdControls[iPrevItem]].Area.Limit().x;
			break;
		case ::gpk::CONTROL_LIST_DIRECTION_VERTICAL		:
			controlConstraints.AttachSizeToControl.x								= menu.IdControl;
			if(menu.IdControls.size() > iPrevItem)
				control.Area.Offset.y													= gui.Controls.Controls[menu.IdControls[iPrevItem]].Area.Limit().y;
			break;
		}
	}

	::gpk::SControl																& controlMenu							= gui.Controls.Controls[menu.IdControl];
	controlMenu.Area.Size													= {};
	for(uint32_t iItem = 0; iItem < menu.IdControls.size(); ++iItem) {
		const uint32_t																idControl								= menu.IdControls		[iItem];
		::gpk::SControl																& control								= gui.Controls.Controls	[idControl];
		::gpk::SControlText															& controlText							= gui.Controls.Text		[idControl];
		switch(menu.Orientation) {
		default:
		case ::gpk::CONTROL_LIST_DIRECTION_HORIZONTAL	:
			controlMenu.Area.Size.x													+= gui.Controls.Controls[idControl].Area.Size.x;
			controlMenu.Area.Size.y													= ::gpk::max(controlMenu.Area.Size.y, gui.Controls.Controls[idControl].Area.Size.y);
			break;
		case ::gpk::CONTROL_LIST_DIRECTION_VERTICAL		:
			controlMenu.Area.Size.y													+= gui.Controls.Controls[idControl].Area.Size.y;
			controlMenu.Area.Size.x													= ::gpk::max(controlMenu.Area.Size.x, (int16_t)(controlText.Text.size() * gui.FontCharSize.x + ::gpk::controlNCSpacing(control).x));
			break;
		}
	}
	if(::gpk::CONTROL_LIST_DIRECTION_VERTICAL == menu.Orientation)
		controlMenu.Area.Size.x													+= 16;
	return 0;
}

			::gpk::error_t												gpk::controlListPush					(::gpk::SGUI& gui, ::gpk::SControlList& menu, const ::gpk::view_const_string& text, int64_t eventCode)				{
	if(menu.IdControl == -1)
		gpk_necall(::gpk::controlListInitialize(gui, menu), "%s", "");

	const int32_t																idControl								= ::gpk::controlCreate(gui);
	gpk_necall(idControl, "%s", "Failed to create control! Out of memory?");
	::gpk::controlSetParent(gui, idControl, menu.IdControl);
	::gpk::SControl																& control								= gui.Controls.Controls		[idControl];
	::gpk::SControlText															& controlText							= gui.Controls.Text			[idControl];
	control				.Margin												= {4, 2, 4, 2};
	controlText			.Text												= {text.begin(), text.size()};
	controlText			.Align												= ::gpk::ALIGN_CENTER_LEFT;
	menu.IdControls	.push_back(idControl);
	menu.Events		.push_back(eventCode);
	gpk::controlListArrange(gui, menu);
	return 0;
}

			::gpk::error_t												gpk::viewportInitialize					(::gpk::SGUI& gui, ::gpk::SViewport& viewport)																				{
	int16_t																		heightTitleBar							= gui.FontCharSize.y + 4;
	uint32_t																	widthTarget								= 800;
	const ::gpk::SCoord2<double>												targetSize								= {(double)widthTarget, widthTarget * (9 / 16.0)};

	//if(-1 == viewport.IdControl)
	{
		viewport.IdControl														= ::gpk::controlCreate(gui);
		//::gpk::SControlMode															& controlMode							= gui.Controls.Modes		[viewport.IdControl];
		//controlMode.Design														= true;
		::gpk::SControl																& control								= gui.Controls.Controls	[viewport.IdControl];
		control.Align															= ::gpk::ALIGN_TOP_LEFT;
		control.Border															= {1, 1, 1, 1};
		control.Margin															= {1, 1, 1, 1};

		const uint32_t																widthViewport							= (uint32_t)(targetSize.x + ::gpk::controlNCSpacing(control).x);
		const uint32_t																heightViewport							= (uint32_t)(targetSize.y + ((int64_t)heightTitleBar + ::gpk::controlNCSpacing(control).y));
		control.Area.Size														= {(int16_t)widthViewport, (int16_t)heightViewport};
		for(uint32_t iElement = 0; iElement < viewport.IdControls.size(); ++iElement) {
			gpk_necall(viewport.IdControls[iElement] = ::gpk::controlCreate(gui), "%s", "This shouldn't fail");
			::gpk::SControlConstraints													& controlConstraints					= gui.Controls.Constraints	[viewport.IdControls[iElement]];
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
		::gpk::SControl																& control								= gui.Controls.Controls		[viewport.IdControls[VIEWPORT_CONTROL_CLOSE]];
		::gpk::SControlText															& controlText							= gui.Controls.Text			[viewport.IdControls[VIEWPORT_CONTROL_CLOSE]];
		control.Area.Size														= {(int16_t)heightTitleBar, (int16_t)heightTitleBar};
		control.Align															= ::gpk::ALIGN_TOP_RIGHT;
		controlText.Text														= {"X", (uint32_t)1};
		controlText.Align														= ::gpk::ALIGN_CENTER;
		::gpk::SControlConstraints													& controlConstraints					= gui.Controls.Constraints	[viewport.IdControls[VIEWPORT_CONTROL_CLOSE]];
		controlConstraints.AttachSizeToControl									= {-1, -1};
	}
	{
		::gpk::SControl																& control								= gui.Controls.Controls		[viewport.IdControls[VIEWPORT_CONTROL_TITLE]];
		::gpk::SControlText															& controlText							= gui.Controls.Text			[viewport.IdControls[VIEWPORT_CONTROL_TITLE]];
		control.Area.Size.y														= heightTitleBar;
		static	int32_t																counterVPs								= 0;
		++counterVPs;
		char																		buffer [256];
		const int32_t																titleLen								= sprintf_s(buffer, "Viewport #%i", counterVPs);
		controlText.Text														= {::gpk::label(buffer).begin(), (uint32_t)titleLen};
		controlText.Align														= ::gpk::ALIGN_CENTER;
		control.Area.Offset.x													= heightTitleBar;
		control.Align															= ::gpk::ALIGN_TOP_RIGHT;
		::gpk::SControlConstraints													& controlConstraints					= gui.Controls.Constraints	[viewport.IdControls[VIEWPORT_CONTROL_TITLE]];
		controlConstraints.DockToControl										= {-1, -1, viewport.IdControls[VIEWPORT_CONTROL_CLOSE], -1};
	}
	{
		::gpk::SControl																& control								= gui.Controls.Controls		[viewport.IdControls[VIEWPORT_CONTROL_TARGET]];
		::gpk::SControlText															& controlText							= gui.Controls.Text			[viewport.IdControls[VIEWPORT_CONTROL_TARGET]];
		controlText.Align														= ::gpk::ALIGN_CENTER_TOP;
		control.Border															= {};
		control.Margin															= {};
		control.Area.Size														= targetSize.Cast<int16_t>();
		//control.Area.Offset.y													= heightTitleBar;
		control.ColorTheme														= 60;
		::gpk::SControlConstraints													& controlConstraints					= gui.Controls.Constraints	[viewport.IdControls[VIEWPORT_CONTROL_TARGET]];
		controlConstraints.DockToControl										= {-1, -1, -1, viewport.IdControls[VIEWPORT_CONTROL_TITLE]};
		//controlConstraints.AttachSizeToControl									= {-1, viewport.IdControl};
	}
	//for(uint32_t i = 0; i < 2; ++i)
	//	::gpk::controlUpdateMetricsTopToDown(gui, viewport.IdControl, gui.LastSize, true);
	return 0;
}

			::gpk::error_t											gpk::paletteGridInitialize				(::gpk::SGUI& gui, ::gpk::SPaletteGrid& palette)				{
	const ::gpk::SCoord2<double>											targetSize								= {256.0, 256.0};
	palette.IdControl													= ::gpk::controlCreate(gui);
	::gpk::SControl															& control								= gui.Controls.Controls[palette.IdControl];
	const uint32_t															widthViewport							= (uint32_t)(targetSize.x + ((int64_t)control.Border.Left + control.Border.Right + control.Margin.Left + control.Margin.Right));
	const uint32_t															heightViewport							= (uint32_t)(targetSize.y + ((int64_t)control.Border.Top + control.Border.Bottom + control.Margin.Top + control.Margin.Bottom));
	control.Area.Size													= {(int16_t)widthViewport, (int16_t)heightViewport};
	control.Align														= ::gpk::ALIGN_CENTER;
	return 0;
}

			::gpk::error_t											gpk::paletteGridColorsSet				(::gpk::SGUI& gui, ::gpk::SPaletteGrid& palette, const ::gpk::view_grid<::gpk::SColorBGRA>& colors)			{
	if(-1 == palette.IdControl)
		::gpk::paletteGridInitialize(gui, palette);

	//gui.Controls.Metrics[palette.IdControl];

	const ::gpk::SCoord2<uint32_t>											& gridMetrics							= colors.metrics();
	palette.IdControls.resize(colors.size());
	palette.Colors														= colors;

	const ::gpk::SCoord2<double>											targetSize								= gui.Controls.Metrics[palette.IdControl].Client.Global.Size.Cast<double>(); //{256.0, 256.0};
	::gpk::SCoord2<double>													controlSize								= {targetSize.x / colors.metrics().x, targetSize.y / colors.metrics().y};

	for(uint32_t y = 0; y < gridMetrics.y; ++y)
	for(uint32_t x = 0; x < gridMetrics.x; ++x) {
		const int32_t															linearIndex								= y * gridMetrics.x + x;
		uint32_t																iFoundColor								= 0;
		int32_t																	themeIndex								= -1;
		const ::gpk::SColorBGRA													& gridCellColor							= colors[y][x];
		for(; iFoundColor < gui.Palette.size(); ++iFoundColor)
			if(gui.Palette[iFoundColor] == gridCellColor) {
				themeIndex															= iFoundColor;
				break;
			}
		if(iFoundColor >= gui.Palette.size()) {
			int32_t																	indexColor								= gui.Palette.push_back(gridCellColor);
			themeIndex															= gui.ControlThemes.push_back({});
			::gpk::SControlTheme													& theme									= gui.ControlThemes[themeIndex];
			::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>			& colorComboDisabled					= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_DISABLED	] = {};
			::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>			& colorComboPressed 					= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_PRESSED	] = {};
			::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>			& colorComboSelected					= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_SELECTED	] = {};
			::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>			& colorComboHover 						= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_HOVER	] = {};
			::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>			& colorComboNormal						= theme.ColorCombos[::gpk::GUI_CONTROL_PALETTE_NORMAL	] = {};
			colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]		= indexColor;
			colorComboDisabled	[::gpk::GUI_CONTROL_COLOR_CLIENT		]		= colorComboDisabled[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
			colorComboNormal	[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]		= indexColor;
			colorComboNormal	[::gpk::GUI_CONTROL_COLOR_CLIENT		]		= colorComboDisabled[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
			colorComboSelected	[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]		= indexColor;
			colorComboSelected	[::gpk::GUI_CONTROL_COLOR_CLIENT		]		= colorComboDisabled[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
			colorComboHover		[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]		= indexColor;
			colorComboHover		[::gpk::GUI_CONTROL_COLOR_CLIENT		]		= colorComboDisabled[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
			colorComboPressed	[::gpk::GUI_CONTROL_COLOR_BACKGROUND	]		= indexColor;
			colorComboPressed	[::gpk::GUI_CONTROL_COLOR_CLIENT		]		= colorComboDisabled[::gpk::GUI_CONTROL_COLOR_BACKGROUND];
			//for(uint32_t iState = 0; iState < theme.ColorCombos.size(); ++iState)
			//	for(uint32_t iArea = 0; iArea < ::gpk::GUI_CONTROL_COLOR_COUNT; ++iArea)
			//		theme.ColorCombos[iState][iArea]									= ::gpk::min((uint32_t)theme.ColorCombos[iState][iArea], gui.Palette.size() - 1);
		}
		const uint32_t															paletteElemIndex						= linearIndex;
		palette.IdControls[paletteElemIndex]								= ::gpk::controlCreate(gui);
		::gpk::SControl															& control								= gui.Controls.Controls[palette.IdControls[paletteElemIndex]];
		control.ColorTheme													= int16_t(themeIndex + 1);
		control.Area.Offset													= {(int16_t)(controlSize.x * x), (int16_t)(controlSize.y * y)};
		control.Area.Size													= controlSize.Cast<int16_t>();
		::gpk::controlSetParent(gui, palette.IdControls[paletteElemIndex], palette.IdControl);
	}
	return 0;
}
