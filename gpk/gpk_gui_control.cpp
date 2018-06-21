#include "gpk_gui_control.h"
#include "gpk_label.h"


			::gpk::error_t												gpk::controlListInitialize				(::gpk::SGUI& gui, ::gpk::SControlList& menu)													{ menu = {}; gpk_necall(menu.IdControl = ::gpk::controlCreate(gui), "Failed to create menu control!"); return 0; }
			::gpk::error_t												gpk::controlListPush					(::gpk::SGUI& gui, ::gpk::SControlList& menu, const ::gpk::view_const_string& text)				{
	if(menu.IdControl == -1) 
		gpk_necall(::gpk::controlListInitialize(gui, menu), "");

	const int32_t																idControl								= ::gpk::controlCreate(gui);
	gpk_necall(idControl, "Failed to create control! Out of memory?");
	::gpk::controlSetParent(gui, idControl, menu.IdControl);
	::gpk::SControl																& control								= gui.Controls.Controls		[idControl];
	::gpk::SControlText															& controlText							= gui.Controls.Text			[idControl];
	::gpk::SControlConstraints													& controlConstraints					= gui.Controls.Constraints	[idControl];
	//control				.Align												= ::gpk::ALIGN_TOP_RIGHT;
	//control				.Align												= ::gpk::ALIGN_BOTTOM_LEFT;
	control				.Margin												= {4, 4, 4, 4};
	//control				.Margin												= {2, 2, 2, 2};
	controlText			.Text												= {text.begin(), text.size()};
	controlText			.Align												= ::gpk::ALIGN_CENTER_LEFT;
	controlConstraints	.AttachSizeToText.y									= true; //menu.Orientation == ::gme::MENU_ORIENTATION_VERTICAL	;
	controlConstraints	.AttachSizeToText.x									= true; //menu.Orientation == ::gme::MENU_ORIENTATION_HORIZONTAL;
	switch(menu.Orientation) {	
	default:
	case ::gpk::CONTROL_LIST_DIRECTION_HORIZONTAL	: controlConstraints.AttachSizeToControl.y = menu.IdControl; if(menu.IdControls.size()) controlConstraints.DockToControl.Right	 = menu.IdControls[menu.IdControls.size() - 1]; break;
	case ::gpk::CONTROL_LIST_DIRECTION_VERTICAL		: controlConstraints.AttachSizeToControl.x = menu.IdControl; if(menu.IdControls.size()) controlConstraints.DockToControl.Bottom	 = menu.IdControls[menu.IdControls.size() - 1]; break;
	}
	::gpk::controlUpdateMetrics(gui, idControl, gui.LastSize);

	switch(menu.Orientation) {	
	default:
	case ::gpk::CONTROL_LIST_DIRECTION_HORIZONTAL	: gui.Controls.Controls[menu.IdControl].Area.Size.x					+= gui.Controls.Metrics[idControl].Total.Global.Size.x; break;
	case ::gpk::CONTROL_LIST_DIRECTION_VERTICAL		: gui.Controls.Controls[menu.IdControl].Area.Size.y					+= gui.Controls.Metrics[idControl].Total.Global.Size.y; break;
	}
	return menu.IdControls.push_back(idControl);			
}

			::gpk::error_t												gpk::viewportInitialize					(::gpk::SGUI& gui, ::gpk::SViewport& viewport)																				{
	uint32_t																	heightTitleBar							= gui.FontCharSize.y + 4;
	uint32_t																	widthTarget								= 800;
	const ::gpk::SCoord2<double>												targetSize								= {(double)widthTarget, widthTarget * (9 / 16.0)};

	//if(-1 == viewport.IdControl) 
	{
		viewport.IdControl														= ::gpk::controlCreate(gui);
		//::gpk::SControlMode															& controlMode							= gui.Controls.Modes		[viewport.IdControl];
		//controlMode.Design														= true;
		::gpk::SControl																& control								= gui.Controls.Controls	[viewport.IdControl];
		const uint32_t																widthViewport							= (uint32_t)(targetSize.x + ((int64_t)control.Border.Left + control.Border.Right + control.Margin.Left + control.Margin.Right					));
		const uint32_t																heightViewport							= (uint32_t)(targetSize.y + ((int64_t)heightTitleBar + control.Border.Top + control.Border.Bottom + control.Margin.Top + control.Margin.Bottom	));
		control.Area.Size														= {(int32_t)widthViewport, (int32_t)heightViewport};
		control.Align															= ::gpk::ALIGN_CENTER;
		for(uint32_t iElement = 0; iElement < viewport.IdControls.size(); ++iElement) {
			gpk_necall(viewport.IdControls[iElement] = ::gpk::controlCreate(gui), "This shouldn't fail");
			::gpk::SControlConstraints													& controlConstraints					= gui.Controls.Constraints	[viewport.IdControls[iElement]];
			controlConstraints.AttachSizeToControl.x								= viewport.IdControls[iElement];
			gpk_necall(::gpk::controlSetParent(gui, viewport.IdControls[iElement], viewport.IdControl), " Maybe the tree integrity has been broken");
		}
	}
	{
		::gpk::SControl																& control								= gui.Controls.Controls		[viewport.IdControls[VIEWPORT_CONTROL_CLOSE]];
		
		::gpk::SControlText															& controlText							= gui.Controls.Text			[viewport.IdControls[VIEWPORT_CONTROL_CLOSE]];
		::gpk::SControlConstraints													& controlConstraints					= gui.Controls.Constraints	[viewport.IdControls[VIEWPORT_CONTROL_CLOSE]];
				control.Area.Size.x														= heightTitleBar;
		control.Area.Size.y														= heightTitleBar;
		control.Align															= ::gpk::ALIGN_TOP_RIGHT;
		controlConstraints.AttachSizeToControl									= {-1, -1};
		controlConstraints.AttachSizeToText										= {false, false};
		//static	int32_t																counterVPs								= 0;
		//++counterVPs;
		//char																		buffer [256];
		//const int32_t																titleLen								= sprintf_s(buffer, "Viewport #%i", counterVPs);
		controlText.Text														= {"X", (uint32_t)1};
		controlText.Align														= ::gpk::ALIGN_CENTER;
	}
	{
		::gpk::SControl																& control								= gui.Controls.Controls		[viewport.IdControls[VIEWPORT_CONTROL_TITLE]];
		::gpk::SControlText															& controlText							= gui.Controls.Text			[viewport.IdControls[VIEWPORT_CONTROL_TITLE]];
		//::gpk::SControlConstraints													& controlConstraints					= gui.Controls.Constraints	[viewport.IdControls[VIEWPORT_CONTROL_TITLE]];
		control.Area.Size.y														= heightTitleBar;
		static	int32_t																counterVPs								= 0;
		++counterVPs;
		char																		buffer [256];
		const int32_t																titleLen								= sprintf_s(buffer, "Viewport #%i", counterVPs);
		controlText.Text														= {::gpk::label(buffer).begin(), (uint32_t)titleLen};
		controlText.Align														= ::gpk::ALIGN_CENTER;
		control.Align															= ::gpk::ALIGN_TOP_RIGHT;
		//controlConstraints.DockToControl.Right									= viewport.IdControls[VIEWPORT_CONTROL_CLOSE];
	}
	{
		::gpk::SControl																& control								= gui.Controls.Controls		[viewport.IdControls[VIEWPORT_CONTROL_TARGET]];
		::gpk::SControlText															& controlText							= gui.Controls.Text			[viewport.IdControls[VIEWPORT_CONTROL_TARGET]];
		controlText.Align														= ::gpk::ALIGN_CENTER_TOP;
		control.Border = control.Margin											= {};
		control.Area.Size														= targetSize.Cast<int32_t>();
		control.Area.Offset	.y													= heightTitleBar;
		control.ColorTheme														= 60;
	}
	return 0;
}

			::gpk::error_t											gpk::paletteGridInitialize				(::gpk::SGUI& gui, ::gpk::SPaletteGrid& palette)				{
	const ::gpk::SCoord2<double>											targetSize								= {256.0, 256.0};
	palette.IdControl													= ::gpk::controlCreate(gui);
	::gpk::SControl															& control								= gui.Controls.Controls[palette.IdControl];
	const uint32_t															widthViewport							= (uint32_t)(targetSize.x + ((int64_t)control.Border.Left + control.Border.Right + control.Margin.Left + control.Margin.Right));
	const uint32_t															heightViewport							= (uint32_t)(targetSize.y + ((int64_t)control.Border.Top + control.Border.Bottom + control.Margin.Top + control.Margin.Bottom));
	control.Area.Size													= {(int32_t)widthViewport, (int32_t)heightViewport};
	control.Align														= ::gpk::ALIGN_CENTER;
	return 0;
}

			::gpk::error_t											gpk::paletteGridColorsSet				(::gpk::SGUI& gui, ::gpk::SPaletteGrid& palette, const ::gpk::grid_view<::gpk::SColorBGRA>& colors)			{
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
			::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>			& colorComboDisabled					= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_DISABLED	]	= {};
			::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>			& colorComboPressed 					= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_PRESSED		]	= {};
			::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>			& colorComboSelected					= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_SELECTED	]	= {};
			::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>			& colorComboHover 						= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_HOVER		]	= {};
			::gpk::array_static<uint32_t, ::gpk::GUI_CONTROL_COLOR_COUNT>			& colorComboNormal						= theme.ColorCombos[::gpk::GUI_CONTROL_STATE_COLORS_NORMAL		]	= {};
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
		control.ColorTheme													= themeIndex + 1;
		control.Area.Offset													= {(int32_t)(controlSize.x * x), (int32_t)(controlSize.y * y)};
		control.Area.Size													= controlSize.Cast<int32_t>();
		::gpk::controlSetParent(gui, palette.IdControls[paletteElemIndex], palette.IdControl);
	}
	return 0;
}
