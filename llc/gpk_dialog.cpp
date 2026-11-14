#include "gpk_dialog.h"
#include "gpk_dialog_controls.h"
#include "gpk_png.h"

GPK_USING_TYPEINT();
GPK_USING_VIEW();
GPK_USING_APOD();
GPK_USING_ASTATIC();
GPK_USING_N2();
GPK_USING_N3();

								::gpk::IDialogControl::~IDialogControl		() { 
	if(-1 != IdGUIControl && DeleteControl && Dialog->GUI) 
		::gpk::controlDelete(*Dialog->GUI, IdGUIControl, true); 
}

static	s2_t dialogPalettePush				(::gpk::apod<::gpk::astatic<::gpk::bgra, ::gpk::UI_CONTROL_AREA_COUNT>> & palettes, ::gpk::bgra background, ::gpk::bgra client, ::gpk::bgra borderLeft, ::gpk::bgra borderTop, ::gpk::bgra borderRight, ::gpk::bgra borderBottom, ::gpk::bgra textFace) {
	::gpk::astatic<::gpk::bgra, ::gpk::UI_CONTROL_AREA_COUNT>	palette	= {};
	palette[::gpk::UI_CONTROL_AREA_BACKGROUND]				= background;
	palette[::gpk::UI_CONTROL_AREA_CLIENT]					= client;
	palette[::gpk::UI_CONTROL_AREA_BORDER_LEFT]				= borderLeft;
	palette[::gpk::UI_CONTROL_AREA_BORDER_TOP]				= borderTop;
	palette[::gpk::UI_CONTROL_AREA_BORDER_RIGHT]				= borderRight;
	palette[::gpk::UI_CONTROL_AREA_BORDER_BOTTOM]			= borderBottom;
	palette[::gpk::UI_CONTROL_AREA_TEXT_BACKGROUND]			= background;
	palette[::gpk::UI_CONTROL_AREA_TEXT_FACE]				= textFace;
	palette[::gpk::UI_CONTROL_AREA_MARGIN_LEFT]				= background;
	palette[::gpk::UI_CONTROL_AREA_MARGIN_TOP]				= background;
	palette[::gpk::UI_CONTROL_AREA_MARGIN_RIGHT]				= background;
	palette[::gpk::UI_CONTROL_AREA_MARGIN_BOTTOM]			= background;
	palette[::gpk::UI_CONTROL_AREA_CORNER_LEFT_TOP_TOP]		= borderTop;
	palette[::gpk::UI_CONTROL_AREA_CORNER_LEFT_TOP_LEFT]		= borderLeft;
	palette[::gpk::UI_CONTROL_AREA_CORNER_RIGHT_TOP_TOP]		= borderTop;
	palette[::gpk::UI_CONTROL_AREA_CORNER_RIGHT_TOP_RIGHT]	= borderRight;
	palette[::gpk::UI_CONTROL_AREA_CORNER_LEFT_BOTTOM_LEFT]	= borderLeft;
	palette[::gpk::UI_CONTROL_AREA_CORNER_LEFT_BOTTOM_BOTTOM]	= borderBottom;
	palette[::gpk::UI_CONTROL_AREA_CORNER_RIGHT_BOTTOM_RIGHT]	= borderRight;
	palette[::gpk::UI_CONTROL_AREA_CORNER_RIGHT_BOTTOM_BOTTOM]	= borderBottom;
	return palettes.push_back(palette);
}

static	gpk::err_t dialogInitializeColors			(::gpk::SDialogColors & dialogColors, ::gpk::apod<::gpk::astatic<::gpk::bgra, ::gpk::UI_CONTROL_AREA_COUNT>> & palettes) {
	dialogColors.Control		[::gpk::GUI_CONTROL_PALETTE_NORMAL	]			= (u1_t)::dialogPalettePush(palettes, ::gpk::DARKGRAY			, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, {}, ::gpk::WHITE		* .85);
	dialogColors.Control		[::gpk::GUI_CONTROL_PALETTE_DISABLED]			= (u1_t)::dialogPalettePush(palettes, ::gpk::LIGHTGRAY * 1.3	, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::LIGHTGRAY	* 1.2);
	dialogColors.Control		[::gpk::GUI_CONTROL_PALETTE_HOVER	]			= (u1_t)::dialogPalettePush(palettes, ::gpk::GRAY	 			, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, {}, ::gpk::WHITE				);
	dialogColors.Control		[::gpk::GUI_CONTROL_PALETTE_PRESSED	]			= (u1_t)::dialogPalettePush(palettes, ::gpk::LIGHTGRAY			, ::gpk::LIGHTBLUE	, ::gpk::LIGHTBLUE	, ::gpk::LIGHTBLUE	, ::gpk::LIGHTBLUE	, {}, ::gpk::WHITE				);
	dialogColors.Control		[::gpk::GUI_CONTROL_PALETTE_DRAGGING]			= (u1_t)::dialogPalettePush(palettes, ::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				);
	dialogColors.Control		[::gpk::GUI_CONTROL_PALETTE_OUTDATED]			= (u1_t)::dialogPalettePush(palettes, ::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				);

	dialogColors.Button			[::gpk::GUI_CONTROL_PALETTE_NORMAL	]			= (u1_t)::dialogPalettePush(palettes, ::gpk::GRAY				, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, {}, ::gpk::WHITE				);
	dialogColors.Button			[::gpk::GUI_CONTROL_PALETTE_DISABLED]			= (u1_t)::dialogPalettePush(palettes, ::gpk::LIGHTGRAY			, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::LIGHTGRAY * 1.1	);
	dialogColors.Button			[::gpk::GUI_CONTROL_PALETTE_HOVER	]			= (u1_t)::dialogPalettePush(palettes, ::gpk::GRAY	 	* 1.1	, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, {}, ::gpk::WHITE				);
	dialogColors.Button			[::gpk::GUI_CONTROL_PALETTE_PRESSED	]			= (u1_t)::dialogPalettePush(palettes, ::gpk::LIGHTGRAY			, ::gpk::ORANGE		, ::gpk::ORANGE		, ::gpk::ORANGE		, ::gpk::ORANGE		, {}, ::gpk::BLUE				);
	dialogColors.Button			[::gpk::GUI_CONTROL_PALETTE_DRAGGING]			= (u1_t)::dialogPalettePush(palettes, ::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				);
	dialogColors.Button			[::gpk::GUI_CONTROL_PALETTE_OUTDATED]			= (u1_t)::dialogPalettePush(palettes, ::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				);

	dialogColors.CheckBox		[::gpk::GUI_CONTROL_PALETTE_NORMAL	]			= (u1_t)::dialogPalettePush(palettes, ::gpk::WHITE		* 0.7	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, {}, ::gpk::WHITE				);
	dialogColors.CheckBox		[::gpk::GUI_CONTROL_PALETTE_DISABLED]			= (u1_t)::dialogPalettePush(palettes, ::gpk::LIGHTGRAY			, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::LIGHTGRAY * 1.1	);
	dialogColors.CheckBox		[::gpk::GUI_CONTROL_PALETTE_HOVER	]			= (u1_t)::dialogPalettePush(palettes, ::gpk::WHITE		* 0.85	, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, {}, ::gpk::WHITE				);
	dialogColors.CheckBox		[::gpk::GUI_CONTROL_PALETTE_PRESSED	]			= (u1_t)::dialogPalettePush(palettes, ::gpk::WHITE		 		, ::gpk::BLUE		, ::gpk::BLUE		, ::gpk::BLUE		, ::gpk::BLUE		, {}, ::gpk::WHITE				);
	dialogColors.CheckBox		[::gpk::GUI_CONTROL_PALETTE_DRAGGING]			= (u1_t)::dialogPalettePush(palettes, ::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				);
	dialogColors.CheckBox		[::gpk::GUI_CONTROL_PALETTE_OUTDATED]			= (u1_t)::dialogPalettePush(palettes, ::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				);

	dialogColors.Viewport		[::gpk::GUI_CONTROL_PALETTE_NORMAL	]			= (u1_t)::dialogPalettePush(palettes, ::gpk::WHITE		* 0.7	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, {}, ::gpk::WHITE				);
	dialogColors.Viewport		[::gpk::GUI_CONTROL_PALETTE_DISABLED]			= (u1_t)::dialogPalettePush(palettes, ::gpk::LIGHTGRAY			, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::LIGHTGRAY * 1.1	);
	dialogColors.Viewport		[::gpk::GUI_CONTROL_PALETTE_HOVER	]			= (u1_t)::dialogPalettePush(palettes, ::gpk::WHITE		* 0.85	, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, {}, ::gpk::WHITE				);
	dialogColors.Viewport		[::gpk::GUI_CONTROL_PALETTE_PRESSED	]			= (u1_t)::dialogPalettePush(palettes, ::gpk::WHITE		 		, ::gpk::BLUE		, ::gpk::BLUE		, ::gpk::BLUE		, ::gpk::BLUE		, {}, ::gpk::WHITE				);
	dialogColors.Viewport		[::gpk::GUI_CONTROL_PALETTE_DRAGGING]			= (u1_t)::dialogPalettePush(palettes, ::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				);
	dialogColors.Viewport		[::gpk::GUI_CONTROL_PALETTE_OUTDATED]			= (u1_t)::dialogPalettePush(palettes, ::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				);

	dialogColors.ViewportTitle	[::gpk::GUI_CONTROL_PALETTE_NORMAL	]			= dialogColors.Button[::gpk::GUI_CONTROL_PALETTE_NORMAL		]; ::dialogPalettePush(palettes, ::gpk::WHITE		* 0.7	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, {}, ::gpk::WHITE				);
	dialogColors.ViewportTitle	[::gpk::GUI_CONTROL_PALETTE_DISABLED]			= dialogColors.Button[::gpk::GUI_CONTROL_PALETTE_DISABLED	]; ::dialogPalettePush(palettes, ::gpk::LIGHTGRAY			, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::LIGHTGRAY * 1.1	);
	dialogColors.ViewportTitle	[::gpk::GUI_CONTROL_PALETTE_HOVER	]			= dialogColors.Button[::gpk::GUI_CONTROL_PALETTE_HOVER		]; ::dialogPalettePush(palettes, ::gpk::WHITE		* 0.85	, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, {}, ::gpk::WHITE				);
	dialogColors.ViewportTitle	[::gpk::GUI_CONTROL_PALETTE_PRESSED	]			= dialogColors.Button[::gpk::GUI_CONTROL_PALETTE_PRESSED	]; ::dialogPalettePush(palettes, ::gpk::WHITE		 		, ::gpk::BLUE		, ::gpk::BLUE		, ::gpk::BLUE		, ::gpk::BLUE		, {}, ::gpk::WHITE				);
	dialogColors.ViewportTitle	[::gpk::GUI_CONTROL_PALETTE_DRAGGING]			= dialogColors.Button[::gpk::GUI_CONTROL_PALETTE_DRAGGING]; ::dialogPalettePush(palettes, ::gpk::BLUE					, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				);
	dialogColors.ViewportTitle	[::gpk::GUI_CONTROL_PALETTE_OUTDATED]			= dialogColors.Button[::gpk::GUI_CONTROL_PALETTE_OUTDATED	]; ::dialogPalettePush(palettes, ::gpk::BLUE					, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				);
	for(u2_t i=0; i < 4; ++i) {
		dialogColors.Control		[::gpk::GUI_CONTROL_PALETTE_SELECTED + i]		= dialogColors.Control			[::gpk::GUI_CONTROL_PALETTE_NORMAL	+ i];
		dialogColors.Button			[::gpk::GUI_CONTROL_PALETTE_SELECTED + i]		= dialogColors.Button			[::gpk::GUI_CONTROL_PALETTE_NORMAL	+ i];
		dialogColors.CheckBox		[::gpk::GUI_CONTROL_PALETTE_SELECTED + i]		= dialogColors.CheckBox			[::gpk::GUI_CONTROL_PALETTE_NORMAL	+ i];
		dialogColors.Viewport		[::gpk::GUI_CONTROL_PALETTE_SELECTED + i]		= dialogColors.Viewport			[::gpk::GUI_CONTROL_PALETTE_NORMAL	+ i];
		dialogColors.ViewportTitle	[::gpk::GUI_CONTROL_PALETTE_SELECTED + i]		= dialogColors.ViewportTitle	[::gpk::GUI_CONTROL_PALETTE_NORMAL	+ i];
	}
	return 0;
}

static	gpk::err_t dialogInitialize				(::gpk::SDialog & dialog) {
	::gpk::SGUI						& gui							= *dialog.GUI; 
	if_fail_fe(dialog.Root = ::gpk::controlCreate(gui));
	gui.Controls.Placement		[dialog.Root].Margin				= {};
	gui.Controls.Placement		[dialog.Root].Border				= {};
	gui.Controls.Constraints	[dialog.Root].AttachSizeToControl	= {dialog.Root, dialog.Root};
	gui.Controls.States			[dialog.Root].Mask					&= ~::gpk::GUI_CONTROL_FLAG_Hovered;
	gui.Controls.States			[dialog.Root].Mask					&= ~::gpk::GUI_CONTROL_FLAG_Action;
	gui.Controls.States			[dialog.Root].Mask					&= ~::gpk::GUI_CONTROL_FLAG_Dragging;
	gui.Controls.States			[dialog.Root].Mask					&= ~::gpk::GUI_CONTROL_FLAG_Pressed;
	dialog.DefaultControlDraw.FrameOut			= 1;
	dialog.DefaultControlDraw.UseNewPalettes	= 1;
	if(!dialog.Colors) {
		static ::gpk::pobj<::gpk::SDialogColors>	globalDefaultColors			= {};
		if(!globalDefaultColors) {
			if_fail_fe(::dialogInitializeColors(*globalDefaultColors.create(), gui.Colors->Palettes));
		}
		dialog.Colors = globalDefaultColors;
	}

	return 0;
}

::gpk::SDialog::SDialog			()										{ GUI.create(); ::dialogInitialize(*this); }
::gpk::SDialog::SDialog			(::gpk::pobj<::gpk::SGUI> & pgui)	{
	if(!pgui)
		pgui.create();
	GUI								= pgui;
	::dialogInitialize(*this);
}
::gpk::SDialog::SDialog			(const ::gpk::pobj<::gpk::SGUI> & pgui)	{
	GUI								= pgui;
	if(!GUI)
		GUI.create();
	::dialogInitialize(*this);
}

stxp	u2_c	heightOfField			= 18;
::gpk::cid_t			gpk::checkBoxCreate		(::gpk::SDialog			& dialog)								{
	cid_t						index					= -1;
	::gpk::pobj<::gpk::SDialogCheckBox>	checkBox;
	::gpk::SControlTable		& controlTable			= dialog.GUI->Controls;
	if_fail_fe(index = dialog.Create(checkBox));
	controlTable.States[checkBox->IdGUIControl].Mask	= dialog.DefaultControlMask;
	::gpk::memcpy_s(controlTable.Draw[checkBox->IdGUIControl].Palettes.Storage, dialog.Colors->CheckBox.Storage);
	return index;
}
gpk::err_t			gpk::checkBoxUpdate			(::gpk::SDialogCheckBox	& checkbox)								{
	::gpk::SDialog				& dialog					= *checkbox.Dialog;
	::gpk::SControlTable		& controlTable				= dialog.GUI->Controls;
	::gpk::img<::gpk::bgra>		& imageCross				= dialog.ImageCrossBGRA;
	if(controlTable.IsAction(checkbox.IdGUIControl)) {
		if(dialog.ImageCross.Texels.size() < 4) {
#if !(defined(GPK_ESP32) || defined(GPK_ARDUINO))
			if_fail_e(::gpk::pngLoadFile("../gpk_data/images/cross.png", imageCross));
#endif
			dialog.ImageCross.resize(imageCross.metrics());
			for(u2_t iTexel = 0; iTexel < imageCross.Texels.size(); ++iTexel) {
				if(imageCross.Texels[iTexel] == (::gpk::bgra)::gpk::BLACK)
					dialog.ImageCross.View[iTexel]	=  true;
				else {
					imageCross.Texels[iTexel].a		= 0;
					dialog.ImageCross.View[iTexel]	= false;
				}
			}
		}
		checkbox.Checked									= !checkbox.Checked;
		controlTable.Images[checkbox.IdGUIControl].Image	= (false == checkbox.Checked) ? ::gpk::g8bgra{} : imageCross.View;
	}
	return 0;
}

gpk::cid_t			gpk::sliderCreate							(SDialog & dialog)								{
	cid_t				index										= -1;
	pobj<SDialogSlider>	slider;
	if_fail_fe(index = dialog.Create(slider));
	if_fail_fe(slider->IdButton = ::gpk::controlCreateChild(*dialog.GUI, slider->IdGUIControl));
	SControlTable		& controlTable								= dialog.GUI->Controls;
	controlTable.Placement	[slider->IdGUIControl].Margin				= {};
	controlTable.Placement	[slider->IdButton].Area.Size				= {16, 16};
	controlTable.Text		[slider->IdButton].Text						= "-";
	controlTable.Constraints[slider->IdButton].AttachSizeToControl.y	= slider->IdButton;
	controlTable.Draw		[slider->IdButton].UseNewPalettes			= true;
	controlTable.Draw		[slider->IdButton].ColorMode				= ::gpk::GUI_COLOR_MODE_3D;
	memcpy_s(controlTable.Draw[slider->IdButton		].Palettes.Storage, dialog.Colors->Button	.Storage);
	memcpy_s(controlTable.Draw[slider->IdGUIControl	].Palettes.Storage, dialog.Colors->CheckBox	.Storage);
	SControlText			& tunerText									= controlTable.Text[slider->IdGUIControl];
	tunerText.Text			= {slider->ValueString.Storage, (u2_t)sprintf_s(slider->ValueString.Storage, "%lli", (long long int)slider->ValueCurrent)};
	return index;
}
gpk::err_t			gpk::sliderSetValue		(::gpk::SDialogSlider & slider, s3_t value)				{
	SDialog				& dialog				= *slider.Dialog;
	SControlTable		& controlTable			= dialog.GUI->Controls;
	const s3_t					newValue				= ::gpk::max(::gpk::min(value, slider.ValueLimits.Max), slider.ValueLimits.Min);;
	bool						updateMetrics			= slider.ValueCurrent != newValue;
	slider.ValueCurrent			= newValue;

	const f3_t				proportion				= (slider.ValueCurrent - slider.ValueLimits.Min) * (1.0 / (slider.ValueLimits.Max - slider.ValueLimits.Min));

	::gpk::SControlPlacement	& controlButton			= controlTable.Placement[slider.IdButton];
	cnst ::gpk::n2f3_t			controlSliderSize		= controlTable.Area[slider.IdGUIControl].Client.Global.Size.f3();
	if(slider.Vertical)	
		controlButton.Area.Offset.y	= s1_t((controlSliderSize.y - controlButton.Area.Size.y) * proportion);
	else				
		controlButton.Area.Offset.x	= s1_t((controlSliderSize.x - controlButton.Area.Size.x) * proportion);

	::gpk::SControlConstraints	& buttonConstraints		= controlTable.Constraints[slider.IdButton];
	if(slider.Vertical) {
		buttonConstraints.AttachSizeToControl.y	= -1;
		buttonConstraints.AttachSizeToControl.x	= slider.IdButton;
	}
	else {
		buttonConstraints.AttachSizeToControl.x	= -1;
		buttonConstraints.AttachSizeToControl.y	= slider.IdButton;
	}
	::gpk::vcst_t				valueString;
	if_fail_fe(slider.FuncValueFormat(valueString, slider.ValueCurrent, slider.ValueLimits));
	if_fail_fe(slider.FuncGetString(valueString, slider.ValueCurrent, slider.ValueLimits));
	if_fail_fe(gpk::controlTextSet(*dialog.GUI, slider.IdGUIControl, valueString));
	if(updateMetrics)
		if_fail_fe(gpk::controlMetricsInvalidate(*dialog.GUI, slider.IdGUIControl));
	return 0;
}

gpk::err_t			gpk::sliderUpdate			(::gpk::SDialogSlider	& slider)								{
	::gpk::SDialog				& dialog					= *slider.Dialog;
	::gpk::SControlTable		& controlTable				= dialog.GUI->Controls;
	const s3_t				valueRange					= slider.ValueLimits.Max - slider.ValueLimits.Min;
	if(controlTable.States[slider.IdButton].IsPressed()) {
		if(dialog.Input->MouseCurrent.Position != dialog.Input->MousePrevious.Position) {
			::gpk::SInput											& input										= *dialog.Input;
			//const ::gpk::SGUIZoom									& zoom										= dialog.GUI->Zoom;
			//::gpk::n2d												scale										=
			//	{ 1.0 / (zoom.ZoomLevel * zoom.DPI.x)
			//	, 1.0 / (zoom.ZoomLevel * zoom.DPI.y)
			//	};
			const ::gpk::n2f3_t			controlSliderPos			= controlTable.Area[slider.IdGUIControl].Client.Global.Offset.f3();
			const ::gpk::n2f3_t			controlSliderSize			= controlTable.Area[slider.IdGUIControl].Client.Global.Size.f3();
			::gpk::n2f3_t				effectiveSize				= controlSliderSize - (controlTable.Placement[slider.IdButton].Area.Size).f3();;
			//

			::gpk::n2f3_t				currentValue				= input.MouseCurrent.Position.f3();
			const ::gpk::n2f3_t			valueUnit					= {1.0 / effectiveSize.x, 1.0 / effectiveSize.y};
			currentValue			-= controlSliderPos + controlTable.Placement[slider.IdButton].Area.Size.f3() / 2.0;
			currentValue.x			*= valueUnit.x;
			currentValue.y			*= valueUnit.y;

			if_fail_fe(gpk::sliderSetValue(slider, (s3_t)((slider.Vertical ? currentValue.y : currentValue.x) * valueRange + slider.ValueLimits.Min)));
		}
	}
	else
		if(dialog.Input->ButtonDown(0) && controlTable.States[slider.IdGUIControl].IsPressed()) {	//
			::gpk::SInput				& input						= *dialog.Input;
			const s3_t				valuePage					= ::gpk::max((s3_t)1LL, (valueRange > 10) ? (s3_t)(valueRange * .1) : valueRange / 2);
			const ::gpk::n2<s1_t>	& controlButtonPosition		= controlTable.Area[slider.IdButton].Total.Global.Offset;
			if(slider.Vertical) {
				const s3_t				finalValue					= slider.ValueCurrent + ((input.MouseCurrent.Position.y > controlButtonPosition.y) ? valuePage : -valuePage);
				if_fail_fe(gpk::sliderSetValue(slider, finalValue));
			}
			else {
				const s3_t				finalValue					= slider.ValueCurrent + ((input.MouseCurrent.Position.x > controlButtonPosition.x) ? valuePage : -valuePage);
				if_fail_fe(gpk::sliderSetValue(slider, finalValue));
			}
		}
	return 0;
}

::gpk::cid_t			gpk::editBoxCreate		(::gpk::SDialog & dialog)								{
	::gpk::cid_t				index					= -1;
	::gpk::pobj<::gpk::SDialogEditBox>	editBox;
	::gpk::SControlTable		& controlTable			= dialog.GUI->Controls;
	if_fail_fe(index = dialog.Create(editBox));
	controlTable.Draw[editBox->IdGUIControl].UseNewPalettes	= true;
	controlTable.Draw[editBox->IdGUIControl].ColorMode		= ::gpk::GUI_COLOR_MODE_Flat;
	::gpk::memcpy_s(controlTable.Draw[editBox->IdGUIControl].Palettes.Storage, dialog.Colors->CheckBox.Storage);
	return index;
}
gpk::err_t			gpk::editBoxUpdate		(::gpk::SDialogEditBox	& control)								{
	::gpk::SDialog				& dialog				= *control.Dialog;
	if(dialog.Input->ButtonDown(0)) {
	}
	return 0;
}
::gpk::cid_t			gpk::viewportCreate		(::gpk::SDialog & dialog) {
	::gpk::cid_t				index					= -1;
	::gpk::pobj<::gpk::SDialogViewport>	viewport;
	::gpk::SControlTable		& controlTable			= dialog.GUI->Controls;
	if_fail_fe(index = dialog.Create(viewport));
	if_fail_fe(::gpk::controlSetParent(*dialog.GUI, viewport->IdGUIControl, dialog.Root));
	controlTable.Draw[viewport->IdGUIControl].UseNewPalettes	= true;
	controlTable.Draw[viewport->IdGUIControl].FrameOut			= true;
	controlTable.States[viewport->IdGUIControl].Mask			&= ~::gpk::GUI_CONTROL_FLAG_Hovered;
	controlTable.Placement[viewport->IdGUIControl].Margin		= {};
	::gpk::memcpy_s(controlTable.Draw[viewport->IdGUIControl].Palettes.Storage, dialog.Colors->Viewport.Storage);
	{ // Create close control
		::gpk::cid_t				idControl;
		if_fail_fe(viewport->IdClose = idControl = ::gpk::controlCreateChild(*dialog.GUI, viewport->IdGUIControl));
		::gpk::SControlPlacement	& control				= controlTable.Placement[idControl];
		control.Area.Size		= {heightOfField, heightOfField};
		control.Align			= ::gpk::ALIGN_TOP_RIGHT;
		controlTable.Constraints[idControl].AttachSizeToControl	= {-1, -1};
		controlTable.States[idControl].Mask		= dialog.DefaultControlMask;
		controlTable.Draw[idControl].UseNewPalettes	= true;
		controlTable.Draw[idControl].Palettes		= dialog.Colors->ViewportTitle;
		controlTable.Text[idControl].Text			= "X";
		controlTable.Text[idControl].Align			= ::gpk::ALIGN_CENTER;
	}
	{ // Create bottom-right resize control
		::gpk::cid_t				idControl;
		if_fail_fe(viewport->IdResizeBottomRight = idControl = ::gpk::controlCreateChild(*dialog.GUI, viewport->IdGUIControl));
		::gpk::SControlPlacement	& control				= controlTable.Placement[idControl];
		control.Area.Size		= {16, 16};
		control.Area.Offset		= {-2, -2};
		control.Align			= ::gpk::ALIGN_BOTTOM_RIGHT;
		controlTable.Constraints[idControl].AttachSizeToControl	= {-1, -1};
		controlTable.States[idControl].Mask		= dialog.DefaultControlMask;
		controlTable.Draw[idControl].UseNewPalettes	= true;
		controlTable.Draw[idControl].Palettes		= dialog.Colors->Viewport;
	}
	{ // Create field group
		// Set up client area control
		::gpk::cid_t				idControl;
		if_fail_fe(viewport->IdClient = idControl = ::gpk::controlCreateChild(*dialog.GUI, viewport->IdGUIControl));		// Create group control array
		::gpk::SControlPlacement	& control				= controlTable.Placement[idControl];
		control.Area.Offset		= {0, heightOfField + 1};
		control.Area.Size		= {2, 2};
		control.Margin			= {};
		controlTable.Constraints[idControl].AttachSizeToControl.x	= idControl;
		controlTable.States[idControl].Mask		= dialog.DefaultControlMask;
		controlTable.States[idControl].Mask		&= ~::gpk::GUI_CONTROL_FLAG_Hovered;
		controlTable.States[idControl].Mask		&= ~::gpk::GUI_CONTROL_FLAG_Action;
		controlTable.Draw [idControl].Palettes	= dialog.Colors->Viewport;
	}
	{ // Create section title
		::gpk::cid_t				idControl;
		if_fail_fef(viewport->IdTitle = idControl = ::gpk::controlCreateChild(*dialog.GUI, viewport->IdGUIControl), "%s", "????");	// Create group control array

		::gpk::SControlPlacement	& control				= controlTable.Placement[idControl];
		control.Area.Offset		= {};
		control.Area.Size		= {2, heightOfField};
		control.Align			= ::gpk::ALIGN_TOP_LEFT;
		control.Margin.Left		= 4;
		controlTable.Constraints[idControl].AttachSizeToControl.x	= idControl;
		controlTable.Constraints[idControl].DockToControl.Right	= viewport->IdClose;
		controlTable.States[idControl].Mask		= dialog.DefaultControlMask;
		controlTable.Draw [idControl].Palettes	= dialog.Colors->ViewportTitle;
		controlTable.Text [idControl].Text		= "Viewport";
	}
	viewport->Settings.Unfolded	= true;
	return index;
}

gpk::err_t			gpk::viewportAdjustSize	(::gpk::n2<s1_t> & sizeViewport, const ::gpk::n2<s1_t> & sizeClient)		{
	sizeViewport			= sizeClient;
	sizeViewport.x			+= 4;
	sizeViewport.y			+= 4 + 1 + heightOfField;
	return 0;
}

gpk::err_t			gpk::viewportFold		(::gpk::SDialogViewport	& control, bool fold)														{
	if((fold && control.Settings.Unfolded) || (false == control.Settings.Unfolded && false == fold)) {
		::gpk::SDialog				& dialog				= *control.Dialog;
		::gpk::SGUI					& gui					= *dialog.GUI;
		::gpk::SControlTable		& controlTable			= gui.Controls;
		::gpk::SControlPlacement	& placementMain			= controlTable.Placement[control.IdGUIControl];
		::gpk::SControlPlacement	& placementTitle		= controlTable.Placement[control.IdTitle];
		if(fold && control.Settings.Unfolded)
			placementMain.Area.Size.y	= placementTitle.Area.Size.y + ::gpk::controlNCSpacing(placementMain).y;
		else if(false == control.Settings.Unfolded && false == fold) {
			::gpk::SControlPlacement	& controlClient			= controlTable.Placement[control.IdClient];
			placementMain.Area.Size.y	= controlClient.Area.Size.y + placementTitle.Area.Size.y + ::gpk::controlNCSpacing(placementMain).y + 1;
		}

		::gpk::SControlState			& controlMain	= controlTable.States[control.IdGUIControl];
		if(-1 != controlMain.Parent) {
			if_fail_fe(gpk::controlMetricsInvalidate(gui, controlMain.Parent));
		}
		else
			if_fail_fe(gpk::controlMetricsInvalidate(gui, control.IdGUIControl));

		::gpk::SControlState		& stateClient			= controlTable.States[control.IdClient];
		stateClient.SetHidden(control.IdClient, controlTable.EventQueue, fold);
		control.Settings.Unfolded	= !fold;
	}
	return one_if(false == control.Settings.Unfolded);
}

static	gpk::err_t viewportDrag			(::gpk::SDialogViewport	& control, ::gpk::SControlPlacement & controlMain, ::gpk::SDialog & dialog, ::gpk::n2<bool> locked, ::gpk::n2s1_t mouseDeltas) {
	mouseDeltas.InPlaceScale(1 / dialog.GUI->Zoom.DPI.x, 1 / dialog.GUI->Zoom.DPI.y);
	mouseDeltas.InPlaceScale(1 / dialog.GUI->Zoom.ZoomLevel);
	control.Settings.Dragging	= true;
	if(false == locked.x)
		controlMain.Area.Offset.x	+= ((controlMain.Align & ::gpk::ALIGN_HCENTER) == ::gpk::ALIGN_RIGHT) ? -mouseDeltas.x : mouseDeltas.x;
	if(false == locked.y)
		controlMain.Area.Offset.y	+= ((controlMain.Align & ::gpk::ALIGN_VCENTER) == ::gpk::ALIGN_BOTTOM) ? -mouseDeltas.y : mouseDeltas.y;
	::gpk::controlMetricsInvalidate(*dialog.GUI, control.IdGUIControl);
	return 0;
}

gpk::err_t			gpk::viewportUpdate		(::gpk::SDialogViewport	& control)																{
	::gpk::SDialog				& dialog				= *control.Dialog;
	::gpk::SGUI					& gui					= *dialog.GUI;
	::gpk::SControlTable		& controlTable			= dialog.GUI->Controls;
	if(controlTable.IsAction(control.IdClose)) {
		controlTable.SetHidden(control.IdGUIControl, true);
		return 0;
	}
	::gpk::SControlPlacement	& controlMain			= controlTable.Placement[control.IdGUIControl];
	::gpk::SControlState		& controlTitleState		= controlTable.States[control.IdTitle];
	::gpk::SControlState		& controlResizeState	= controlTable.States[control.IdResizeBottomRight];
	if(controlTitleState.IsDragging()) {
		const ::gpk::n2<bool>		locked					= {control.Settings.DisplacementLockX, control.Settings.DisplacementLockY};
		if(false == locked.x || false == locked.y) {
			::gpk::n2s1_t				mouseDeltas				= (dialog.Input->MouseCurrent.Position - dialog.Input->MousePrevious.Position).s1(); //{dialog.Input->MouseCurrent.Deltas.x, dialog.Input->MouseCurrent.Deltas.y};
			if(mouseDeltas.x || mouseDeltas.y)
				if_fail_fe(::viewportDrag(control, controlMain, dialog, locked, mouseDeltas));
		}
	}
	else if(controlResizeState.IsDragging()) {
		::gpk::n2s1_t				mouseDeltas			= dialog.Input->MouseCurrent.Position - dialog.Input->MousePrevious.Position;
		if(mouseDeltas.x || mouseDeltas.y) {
			mouseDeltas.InPlaceScale(1 / dialog.GUI->Zoom.DPI.x, 1 / dialog.GUI->Zoom.DPI.y);
			mouseDeltas.InPlaceScale(1 / dialog.GUI->Zoom.ZoomLevel);
			::gpk::SControlPlacement	& controlClient			= controlTable.Placement[control.IdClient];
			mouseDeltas.x								= ::gpk::max(mouseDeltas.x, (s1_t)-controlClient.Area.Size.x);
			mouseDeltas.y								= ::gpk::max(mouseDeltas.y, (s1_t)-controlClient.Area.Size.y);
			controlMain.Area.Size						+= mouseDeltas;
			controlClient.Area.Size						+= mouseDeltas;
			if_fail_fe(::gpk::controlMetricsInvalidate(gui, control.IdGUIControl));
		}
	}
	else {
		if(controlTable.IsAction(control.IdTitle) && false == control.Settings.Dragging)
			if_fail_fe(gpk::viewportFold(control, control.Settings.Unfolded));
		control.Settings.Dragging	= false;
	}

	if(control.Settings.Unfolded != control.SettingsOld.Unfolded)
		if(true == control.Settings.Unfolded) {
			::gpk::n2s1_t				clientFinalSize			= {controlMain.Area.Size};
			clientFinalSize			-= ::gpk::controlNCSpacing(controlMain).s1();
			clientFinalSize.y		-=  heightOfField + 1;
			::gpk::SControlPlacement	& controlClient			= controlTable.Placement[control.IdClient];
			controlClient.Area.Size	= clientFinalSize;
		}
	if(false == ::gpk::controlInvalid(gui, control.IdScrollBarY)) {
		if(controlTable.States[control.IdScrollBarY].Parent != control.IdClient)
			if_fail_fe(::gpk::controlSetParent(gui, control.IdScrollBarY, control.IdClient));
		::gpk::SControlPlacement	& placementScroll		= controlTable.Placement[control.IdScrollBarY];
		placementScroll.Area.Offset							= {};
		placementScroll.Area.Size.x							= 12;
		placementScroll.Align								= ::gpk::ALIGN_TOP_RIGHT;
		::gpk::SControlConstraints	& constraintsScroll		= controlTable.Constraints[control.IdScrollBarY];
		constraintsScroll.AttachSizeToControl				= {-1, control.IdScrollBarY};
		constraintsScroll.DockToControl						= {-1, -1, -1, -1};
		controlTable.SetHidden(control.IdScrollBarY, false == control.Settings.ScrollBarY || false == control.Settings.Unfolded);
	}
	if(false == ::gpk::controlInvalid(gui, control.IdScrollBarX)) {
		if(controlTable.States[control.IdScrollBarX].Parent != control.IdClient)
			if_fail_fe(::gpk::controlSetParent(gui, control.IdScrollBarX, control.IdClient));
		::gpk::SControlPlacement	& placementScroll		= controlTable.Placement[control.IdScrollBarX];
		placementScroll.Area.Offset							= {};
		placementScroll.Area.Size.y							= 12;
		placementScroll.Align								= ::gpk::ALIGN_BOTTOM_LEFT;
		::gpk::SControlConstraints	& constraintsScroll		= controlTable.Constraints[control.IdScrollBarX];
		constraintsScroll.AttachSizeToControl				= {control.IdScrollBarX, -1};
		constraintsScroll.DockToControl						= {-1, -1, control.Settings.ScrollBarY ? control.IdScrollBarY : -1, -1};
		controlTable.SetHidden(control.IdScrollBarX, false == control.Settings.ScrollBarX || false == control.Settings.Unfolded);
	}
	if(control.Settings.ScrollBarX != control.SettingsOld.ScrollBarX || control.Settings.ScrollBarY != control.SettingsOld.ScrollBarY) {
		if(false == ::gpk::controlInvalid(gui, control.IdScrollBarX))
			if_fail_fe(::gpk::controlMetricsInvalidate(gui, control.IdScrollBarX));
		if(false == ::gpk::controlInvalid(gui, control.IdScrollBarY))
			if_fail_fe(::gpk::controlMetricsInvalidate(gui, control.IdScrollBarY));
	}
	control.SettingsOld		= control.Settings;
	return 0;
}
