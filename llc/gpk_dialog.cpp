#include "gpk_dialog.h"
#include "gpk_dialog_controls.h"
#include "gpk_png.h"

usns gpk;

gpk::IDialogControl::~IDialogControl		() { 
	if(-1 != IdGUIControl && DeleteControl && Dialog->GUI) 
		controlDelete(*Dialog->GUI, IdGUIControl, true); 
}

sttc s2_t dialogPalettePush				(apod<astatic<bgra, UI_CONTROL_AREA_COUNT>> & palettes, bgra background, bgra client, bgra borderLeft, bgra borderTop, bgra borderRight, bgra borderBottom, bgra textFace) {
	astatic<bgra, UI_CONTROL_AREA_COUNT>	palette	= {};
	palette[UI_CONTROL_AREA_BACKGROUND]					= background;
	palette[UI_CONTROL_AREA_CLIENT]						= client;
	palette[UI_CONTROL_AREA_BORDER_LEFT]				= borderLeft;
	palette[UI_CONTROL_AREA_BORDER_TOP]					= borderTop;
	palette[UI_CONTROL_AREA_BORDER_RIGHT]				= borderRight;
	palette[UI_CONTROL_AREA_BORDER_BOTTOM]				= borderBottom;
	palette[UI_CONTROL_AREA_TEXT_BACKGROUND]			= background;
	palette[UI_CONTROL_AREA_TEXT_FACE]					= textFace;
	palette[UI_CONTROL_AREA_MARGIN_LEFT]				= 
	palette[UI_CONTROL_AREA_MARGIN_TOP]					= 
	palette[UI_CONTROL_AREA_MARGIN_RIGHT]				= 
	palette[UI_CONTROL_AREA_MARGIN_BOTTOM]				= background;
	palette[UI_CONTROL_AREA_CORNER_LEFT_TOP_TOP]		= borderTop;
	palette[UI_CONTROL_AREA_CORNER_LEFT_TOP_LEFT]		= borderLeft;
	palette[UI_CONTROL_AREA_CORNER_RIGHT_TOP_TOP]		= borderTop;
	palette[UI_CONTROL_AREA_CORNER_RIGHT_TOP_RIGHT]		= borderRight;
	palette[UI_CONTROL_AREA_CORNER_LEFT_BOTTOM_LEFT]	= borderLeft;
	palette[UI_CONTROL_AREA_CORNER_LEFT_BOTTOM_BOTTOM]	= borderBottom;
	palette[UI_CONTROL_AREA_CORNER_RIGHT_BOTTOM_RIGHT]	= borderRight;
	palette[UI_CONTROL_AREA_CORNER_RIGHT_BOTTOM_BOTTOM]	= borderBottom;
	return palettes.push_back(palette);
}

sttc gpk::err_t dialogInitializeColors			(SDialogColors & dialogColors, apod<astatic<bgra, UI_CONTROL_AREA_COUNT>> & palettes) {
	dialogColors.Control		[GUI_CONTROL_PALETTE_NORMAL	]			= (u1_t)::dialogPalettePush(palettes, DARKGRAY			, DARKGRAY	, DARKGRAY	, DARKGRAY	, DARKGRAY	, {}, WHITE		* .85);
	dialogColors.Control		[GUI_CONTROL_PALETTE_DISABLED]			= (u1_t)::dialogPalettePush(palettes, LIGHTGRAY * 1.3	, ORANGE		, YELLOW		, MAGENTA	, CYAN		, {}, LIGHTGRAY	* 1.2);
	dialogColors.Control		[GUI_CONTROL_PALETTE_HOVER	]			= (u1_t)::dialogPalettePush(palettes, GRAY	 			, GRAY		, GRAY		, GRAY		, GRAY		, {}, WHITE				);
	dialogColors.Control		[GUI_CONTROL_PALETTE_PRESSED	]			= (u1_t)::dialogPalettePush(palettes, LIGHTGRAY			, LIGHTBLUE	, LIGHTBLUE	, LIGHTBLUE	, LIGHTBLUE	, {}, WHITE				);
	dialogColors.Control		[GUI_CONTROL_PALETTE_DRAGGING]			= (u1_t)::dialogPalettePush(palettes, BLUE				, ORANGE		, YELLOW		, MAGENTA	, CYAN		, {}, WHITE				);
	dialogColors.Control		[GUI_CONTROL_PALETTE_OUTDATED]			= (u1_t)::dialogPalettePush(palettes, BLUE				, ORANGE		, YELLOW		, MAGENTA	, CYAN		, {}, WHITE				);

	dialogColors.Button			[GUI_CONTROL_PALETTE_NORMAL	]			= (u1_t)::dialogPalettePush(palettes, GRAY				, DARKGRAY	, DARKGRAY	, DARKGRAY	, DARKGRAY	, {}, WHITE				);
	dialogColors.Button			[GUI_CONTROL_PALETTE_DISABLED]			= (u1_t)::dialogPalettePush(palettes, LIGHTGRAY			, ORANGE		, YELLOW		, MAGENTA	, CYAN		, {}, LIGHTGRAY * 1.1	);
	dialogColors.Button			[GUI_CONTROL_PALETTE_HOVER	]			= (u1_t)::dialogPalettePush(palettes, GRAY	 	* 1.1	, GRAY		, GRAY		, GRAY		, GRAY		, {}, WHITE				);
	dialogColors.Button			[GUI_CONTROL_PALETTE_PRESSED	]			= (u1_t)::dialogPalettePush(palettes, LIGHTGRAY			, ORANGE		, ORANGE		, ORANGE		, ORANGE		, {}, BLUE				);
	dialogColors.Button			[GUI_CONTROL_PALETTE_DRAGGING]			= (u1_t)::dialogPalettePush(palettes, BLUE				, ORANGE		, YELLOW		, MAGENTA	, CYAN		, {}, WHITE				);
	dialogColors.Button			[GUI_CONTROL_PALETTE_OUTDATED]			= (u1_t)::dialogPalettePush(palettes, BLUE				, ORANGE		, YELLOW		, MAGENTA	, CYAN		, {}, WHITE				);

	dialogColors.CheckBox		[GUI_CONTROL_PALETTE_NORMAL	]			= (u1_t)::dialogPalettePush(palettes, WHITE		* 0.7	, DARKGRAY	, DARKGRAY	, DARKGRAY	, DARKGRAY	, {}, WHITE				);
	dialogColors.CheckBox		[GUI_CONTROL_PALETTE_DISABLED]			= (u1_t)::dialogPalettePush(palettes, LIGHTGRAY			, ORANGE		, YELLOW		, MAGENTA	, CYAN		, {}, LIGHTGRAY * 1.1	);
	dialogColors.CheckBox		[GUI_CONTROL_PALETTE_HOVER	]			= (u1_t)::dialogPalettePush(palettes, WHITE		* 0.85	, GRAY		, GRAY		, GRAY		, GRAY		, {}, WHITE				);
	dialogColors.CheckBox		[GUI_CONTROL_PALETTE_PRESSED	]			= (u1_t)::dialogPalettePush(palettes, WHITE		 		, BLUE		, BLUE		, BLUE		, BLUE		, {}, WHITE				);
	dialogColors.CheckBox		[GUI_CONTROL_PALETTE_DRAGGING]			= (u1_t)::dialogPalettePush(palettes, BLUE				, ORANGE		, YELLOW		, MAGENTA	, CYAN		, {}, WHITE				);
	dialogColors.CheckBox		[GUI_CONTROL_PALETTE_OUTDATED]			= (u1_t)::dialogPalettePush(palettes, BLUE				, ORANGE		, YELLOW		, MAGENTA	, CYAN		, {}, WHITE				);

	dialogColors.Viewport		[GUI_CONTROL_PALETTE_NORMAL	]			= (u1_t)::dialogPalettePush(palettes, WHITE		* 0.7	, DARKGRAY	, DARKGRAY	, DARKGRAY	, DARKGRAY	, {}, WHITE				);
	dialogColors.Viewport		[GUI_CONTROL_PALETTE_DISABLED]			= (u1_t)::dialogPalettePush(palettes, LIGHTGRAY			, ORANGE		, YELLOW		, MAGENTA	, CYAN		, {}, LIGHTGRAY * 1.1	);
	dialogColors.Viewport		[GUI_CONTROL_PALETTE_HOVER	]			= (u1_t)::dialogPalettePush(palettes, WHITE		* 0.85	, GRAY		, GRAY		, GRAY		, GRAY		, {}, WHITE				);
	dialogColors.Viewport		[GUI_CONTROL_PALETTE_PRESSED	]			= (u1_t)::dialogPalettePush(palettes, WHITE		 		, BLUE		, BLUE		, BLUE		, BLUE		, {}, WHITE				);
	dialogColors.Viewport		[GUI_CONTROL_PALETTE_DRAGGING]			= (u1_t)::dialogPalettePush(palettes, BLUE				, ORANGE		, YELLOW		, MAGENTA	, CYAN		, {}, WHITE				);
	dialogColors.Viewport		[GUI_CONTROL_PALETTE_OUTDATED]			= (u1_t)::dialogPalettePush(palettes, BLUE				, ORANGE		, YELLOW		, MAGENTA	, CYAN		, {}, WHITE				);

	dialogColors.ViewportTitle	[GUI_CONTROL_PALETTE_NORMAL	]			= dialogColors.Button[GUI_CONTROL_PALETTE_NORMAL		]; ::dialogPalettePush(palettes, WHITE		* 0.7	, DARKGRAY	, DARKGRAY	, DARKGRAY	, DARKGRAY	, {}, WHITE				);
	dialogColors.ViewportTitle	[GUI_CONTROL_PALETTE_DISABLED]			= dialogColors.Button[GUI_CONTROL_PALETTE_DISABLED	]; ::dialogPalettePush(palettes, LIGHTGRAY			, ORANGE		, YELLOW		, MAGENTA	, CYAN		, {}, LIGHTGRAY * 1.1	);
	dialogColors.ViewportTitle	[GUI_CONTROL_PALETTE_HOVER	]			= dialogColors.Button[GUI_CONTROL_PALETTE_HOVER		]; ::dialogPalettePush(palettes, WHITE		* 0.85	, GRAY		, GRAY		, GRAY		, GRAY		, {}, WHITE				);
	dialogColors.ViewportTitle	[GUI_CONTROL_PALETTE_PRESSED	]			= dialogColors.Button[GUI_CONTROL_PALETTE_PRESSED	]; ::dialogPalettePush(palettes, WHITE		 		, BLUE		, BLUE		, BLUE		, BLUE		, {}, WHITE				);
	dialogColors.ViewportTitle	[GUI_CONTROL_PALETTE_DRAGGING]			= dialogColors.Button[GUI_CONTROL_PALETTE_DRAGGING]; ::dialogPalettePush(palettes, BLUE					, ORANGE		, YELLOW		, MAGENTA	, CYAN		, {}, WHITE				);
	dialogColors.ViewportTitle	[GUI_CONTROL_PALETTE_OUTDATED]			= dialogColors.Button[GUI_CONTROL_PALETTE_OUTDATED	]; ::dialogPalettePush(palettes, BLUE					, ORANGE		, YELLOW		, MAGENTA	, CYAN		, {}, WHITE				);
	for(u2_t i=0; i < 4; ++i) {
		dialogColors.Control		[GUI_CONTROL_PALETTE_SELECTED + i]		= dialogColors.Control			[GUI_CONTROL_PALETTE_NORMAL	+ i];
		dialogColors.Button			[GUI_CONTROL_PALETTE_SELECTED + i]		= dialogColors.Button			[GUI_CONTROL_PALETTE_NORMAL	+ i];
		dialogColors.CheckBox		[GUI_CONTROL_PALETTE_SELECTED + i]		= dialogColors.CheckBox			[GUI_CONTROL_PALETTE_NORMAL	+ i];
		dialogColors.Viewport		[GUI_CONTROL_PALETTE_SELECTED + i]		= dialogColors.Viewport			[GUI_CONTROL_PALETTE_NORMAL	+ i];
		dialogColors.ViewportTitle	[GUI_CONTROL_PALETTE_SELECTED + i]		= dialogColors.ViewportTitle	[GUI_CONTROL_PALETTE_NORMAL	+ i];
	}
	return 0;
}

sttc gpk::err_t dialogInitialize				(SDialog & dialog) {
	gpk::SGUI						& gui							= *dialog.GUI; 
	if_fail_fe(dialog.Root = controlCreate(gui));
	gui.Controls.Placement		[dialog.Root].Margin				= {};
	gui.Controls.Placement		[dialog.Root].Border				= {};
	gui.Controls.Constraints	[dialog.Root].AttachSizeToControl	= {dialog.Root, dialog.Root};
	gui.Controls.States			[dialog.Root].Mask					&= ~GUI_CONTROL_FLAG_Hovered;
	gui.Controls.States			[dialog.Root].Mask					&= ~GUI_CONTROL_FLAG_Action;
	gui.Controls.States			[dialog.Root].Mask					&= ~GUI_CONTROL_FLAG_Dragging;
	gui.Controls.States			[dialog.Root].Mask					&= ~GUI_CONTROL_FLAG_Pressed;
	dialog.DefaultControlDraw.FrameOut			= 1;
	dialog.DefaultControlDraw.UseNewPalettes	= 1;
	if(!dialog.Colors) {
		static pobj<SDialogColors>	globalDefaultColors			= {};
		if(!globalDefaultColors) {
			if_fail_fe(::dialogInitializeColors(*globalDefaultColors.create(), gui.Colors->Palettes));
		}
		dialog.Colors = globalDefaultColors;
	}

	return 0;
}

gpk::SDialog::SDialog			()										{ GUI.create(); ::dialogInitialize(*this); }
gpk::SDialog::SDialog			(pobj<SGUI> & pgui)	{
	if(!pgui)
		pgui.create();
	GUI								= pgui;
	::dialogInitialize(*this);
}
gpk::SDialog::SDialog			(const pobj<SGUI> & pgui)	{
	GUI								= pgui;
	if(!GUI)
		GUI.create();
	::dialogInitialize(*this);
}

stxp	gpk::u2_c	heightOfField			= 18;
gpk::cid_t			gpk::checkBoxCreate		(SDialog			& dialog)								{
	cid_t						index					= -1;
	pobj<SDialogCheckBox>	checkBox;
	SControlTable		& controlTable			= dialog.GUI->Controls;
	if_fail_fe(index = dialog.Create(checkBox));
	controlTable.States[checkBox->IdGUIControl].Mask	= dialog.DefaultControlMask;
	memcpy_s(controlTable.Draw[checkBox->IdGUIControl].Palettes.Storage, dialog.Colors->CheckBox.Storage);
	return index;
}
gpk::err_t			gpk::checkBoxUpdate			(SDialogCheckBox	& checkbox)								{
	SDialog				& dialog					= *checkbox.Dialog;
	SControlTable		& controlTable				= dialog.GUI->Controls;
	img<bgra>		& imageCross				= dialog.ImageCrossBGRA;
	if(controlTable.IsAction(checkbox.IdGUIControl)) {
		if(dialog.ImageCross.Texels.size() < 4) {
#if !(defined(GPK_ESP32) || defined(GPK_ARDUINO))
			if_fail_e(pngLoadFile("../gpk_data/images/cross.png", imageCross));
#endif
			dialog.ImageCross.resize(imageCross.metrics());
			for(u2_t iTexel = 0; iTexel < imageCross.Texels.size(); ++iTexel) {
				if(imageCross.Texels[iTexel] == (bgra)BLACK)
					dialog.ImageCross.View[iTexel]	=  true;
				else {
					imageCross.Texels[iTexel].a		= 0;
					dialog.ImageCross.View[iTexel]	= false;
				}
			}
		}
		checkbox.Checked									= !checkbox.Checked;
		controlTable.Images[checkbox.IdGUIControl].Image	= (false == checkbox.Checked) ? g8bgra{} : imageCross.View;
	}
	return 0;
}

gpk::cid_t			gpk::sliderCreate							(SDialog & dialog)								{
	cid_t				index										= -1;
	pobj<SDialogSlider>	slider;
	if_fail_fe(index = dialog.Create(slider));
	if_fail_fe(slider->IdButton = controlCreateChild(*dialog.GUI, slider->IdGUIControl));
	SControlTable		& controlTable								= dialog.GUI->Controls;
	controlTable.Placement	[slider->IdGUIControl].Margin				= {};
	controlTable.Placement	[slider->IdButton].Area.Size				= {16, 16};
	controlTable.Text		[slider->IdButton].Text						= "-";
	controlTable.Constraints[slider->IdButton].AttachSizeToControl.y	= slider->IdButton;
	controlTable.Draw		[slider->IdButton].UseNewPalettes			= true;
	controlTable.Draw		[slider->IdButton].ColorMode				= GUI_COLOR_MODE_3D;
	memcpy_s(controlTable.Draw[slider->IdButton		].Palettes.Storage, dialog.Colors->Button	.Storage);
	memcpy_s(controlTable.Draw[slider->IdGUIControl	].Palettes.Storage, dialog.Colors->CheckBox	.Storage);
	SControlText			& tunerText									= controlTable.Text[slider->IdGUIControl];
	tunerText.Text			= {slider->ValueString.Storage, (u2_t)gpk::sprintf_s(slider->ValueString.Storage, "%lli", (long long int)slider->ValueCurrent)};
	return index;
}
gpk::err_t			gpk::sliderSetValue		(SDialogSlider & slider, s3_t value)				{
	SDialog				& dialog				= *slider.Dialog;
	SControlTable		& controlTable			= dialog.GUI->Controls;
	const s3_t					newValue				= max(min(value, slider.ValueLimits.Max), slider.ValueLimits.Min);;
	bool						updateMetrics			= slider.ValueCurrent != newValue;
	slider.ValueCurrent			= newValue;

	const f3_t				proportion				= (slider.ValueCurrent - slider.ValueLimits.Min) * (1.0 / (slider.ValueLimits.Max - slider.ValueLimits.Min));

	SControlPlacement	& controlButton			= controlTable.Placement[slider.IdButton];
	cnst n2f3_t			controlSliderSize		= controlTable.Area[slider.IdGUIControl].Client.Global.Size.f3();
	if(slider.Vertical)	
		controlButton.Area.Offset.y	= s1_t((controlSliderSize.y - controlButton.Area.Size.y) * proportion);
	else				
		controlButton.Area.Offset.x	= s1_t((controlSliderSize.x - controlButton.Area.Size.x) * proportion);

	SControlConstraints	& buttonConstraints		= controlTable.Constraints[slider.IdButton];
	if(slider.Vertical) {
		buttonConstraints.AttachSizeToControl.y	= -1;
		buttonConstraints.AttachSizeToControl.x	= slider.IdButton;
	}
	else {
		buttonConstraints.AttachSizeToControl.x	= -1;
		buttonConstraints.AttachSizeToControl.y	= slider.IdButton;
	}
	vcst_t				valueString;
	if_fail_fe(slider.FuncValueFormat(valueString, slider.ValueCurrent, slider.ValueLimits));
	if_fail_fe(slider.FuncGetString(valueString, slider.ValueCurrent, slider.ValueLimits));
	if_fail_fe(controlTextSet(*dialog.GUI, slider.IdGUIControl, valueString));
	if(updateMetrics)
		if_fail_fe(controlMetricsInvalidate(*dialog.GUI, slider.IdGUIControl));
	return 0;
}

gpk::err_t			gpk::sliderUpdate			(SDialogSlider	& slider)								{
	SDialog				& dialog					= *slider.Dialog;
	SControlTable		& controlTable				= dialog.GUI->Controls;
	const s3_t				valueRange					= slider.ValueLimits.Max - slider.ValueLimits.Min;
	if(controlTable.States[slider.IdButton].IsPressed()) {
		if(dialog.Input->MouseCurrent.Position != dialog.Input->MousePrevious.Position) {
			SInput											& input										= *dialog.Input;
			//const SGUIZoom									& zoom										= dialog.GUI->Zoom;
			//n2d												scale										=
			//	{ 1.0 / (zoom.ZoomLevel * zoom.DPI.x)
			//	, 1.0 / (zoom.ZoomLevel * zoom.DPI.y)
			//	};
			const n2f3_t			controlSliderPos			= controlTable.Area[slider.IdGUIControl].Client.Global.Offset.f3();
			const n2f3_t			controlSliderSize			= controlTable.Area[slider.IdGUIControl].Client.Global.Size.f3();
			n2f3_t				effectiveSize				= controlSliderSize - (controlTable.Placement[slider.IdButton].Area.Size).f3();;
			//

			n2f3_t				currentValue				= input.MouseCurrent.Position.f3();
			const n2f3_t			valueUnit					= {1.0 / effectiveSize.x, 1.0 / effectiveSize.y};
			currentValue			-= controlSliderPos + controlTable.Placement[slider.IdButton].Area.Size.f3() / 2.0;
			currentValue.x			*= valueUnit.x;
			currentValue.y			*= valueUnit.y;

			if_fail_fe(gpk::sliderSetValue(slider, (s3_t)((slider.Vertical ? currentValue.y : currentValue.x) * valueRange + slider.ValueLimits.Min)));
		}
	}
	else
		if(dialog.Input->ButtonDown(0) && controlTable.States[slider.IdGUIControl].IsPressed()) {	//
			SInput				& input						= *dialog.Input;
			const s3_t				valuePage					= max((s3_t)1LL, (valueRange > 10) ? (s3_t)(valueRange * .1) : valueRange / 2);
			const n2<s1_t>	& controlButtonPosition		= controlTable.Area[slider.IdButton].Total.Global.Offset;
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

gpk::cid_t			gpk::editBoxCreate		(SDialog & dialog)								{
	cid_t				index					= -1;
	pobj<SDialogEditBox>	editBox;
	SControlTable		& controlTable			= dialog.GUI->Controls;
	if_fail_fe(index = dialog.Create(editBox));
	controlTable.Draw[editBox->IdGUIControl].UseNewPalettes	= true;
	controlTable.Draw[editBox->IdGUIControl].ColorMode		= GUI_COLOR_MODE_Flat;
	memcpy_s(controlTable.Draw[editBox->IdGUIControl].Palettes.Storage, dialog.Colors->CheckBox.Storage);
	return index;
}
gpk::err_t			gpk::editBoxUpdate		(SDialogEditBox	& control)								{
	SDialog				& dialog				= *control.Dialog;
	if(dialog.Input->ButtonDown(0)) {
	}
	return 0;
}
gpk::cid_t			gpk::viewportCreate		(SDialog & dialog) {
	cid_t				index					= -1;
	pobj<SDialogViewport>	viewport;
	SControlTable		& controlTable			= dialog.GUI->Controls;
	if_fail_fe(index = dialog.Create(viewport));
	if_fail_fe(controlSetParent(*dialog.GUI, viewport->IdGUIControl, dialog.Root));
	controlTable.Draw[viewport->IdGUIControl].UseNewPalettes	= true;
	controlTable.Draw[viewport->IdGUIControl].FrameOut			= true;
	controlTable.States[viewport->IdGUIControl].Mask			&= ~GUI_CONTROL_FLAG_Hovered;
	controlTable.Placement[viewport->IdGUIControl].Margin		= {};
	memcpy_s(controlTable.Draw[viewport->IdGUIControl].Palettes.Storage, dialog.Colors->Viewport.Storage);
	{ // Create close control
		cid_t				idControl;
		if_fail_fe(viewport->IdClose = idControl = controlCreateChild(*dialog.GUI, viewport->IdGUIControl));
		SControlPlacement	& control				= controlTable.Placement[idControl];
		control.Area.Size		= {heightOfField, heightOfField};
		control.Align			= ALIGN_TOP_RIGHT;
		controlTable.Constraints[idControl].AttachSizeToControl	= {-1, -1};
		controlTable.States[idControl].Mask		= dialog.DefaultControlMask;
		controlTable.Draw[idControl].UseNewPalettes	= true;
		controlTable.Draw[idControl].Palettes		= dialog.Colors->ViewportTitle;
		controlTable.Text[idControl].Text			= "X";
		controlTable.Text[idControl].Align			= ALIGN_CENTER;
	}
	{ // Create bottom-right resize control
		cid_t				idControl;
		if_fail_fe(viewport->IdResizeBottomRight = idControl = controlCreateChild(*dialog.GUI, viewport->IdGUIControl));
		SControlPlacement	& control				= controlTable.Placement[idControl];
		control.Area.Size		= {16, 16};
		control.Area.Offset		= {-2, -2};
		control.Align			= ALIGN_BOTTOM_RIGHT;
		controlTable.Constraints[idControl].AttachSizeToControl	= {-1, -1};
		controlTable.States[idControl].Mask		= dialog.DefaultControlMask;
		controlTable.Draw[idControl].UseNewPalettes	= true;
		controlTable.Draw[idControl].Palettes		= dialog.Colors->Viewport;
	}
	{ // Create field group
		// Set up client area control
		cid_t				idControl;
		if_fail_fe(viewport->IdClient = idControl = controlCreateChild(*dialog.GUI, viewport->IdGUIControl));		// Create group control array
		SControlPlacement	& control				= controlTable.Placement[idControl];
		control.Area.Offset		= {0, heightOfField + 1};
		control.Area.Size		= {2, 2};
		control.Margin			= {};
		controlTable.Constraints[idControl].AttachSizeToControl.x	= idControl;
		controlTable.States[idControl].Mask		= dialog.DefaultControlMask;
		controlTable.States[idControl].Mask		&= ~GUI_CONTROL_FLAG_Hovered;
		controlTable.States[idControl].Mask		&= ~GUI_CONTROL_FLAG_Action;
		controlTable.Draw [idControl].Palettes	= dialog.Colors->Viewport;
	}
	{ // Create section title
		cid_t				idControl;
		if_fail_fef(viewport->IdTitle = idControl = controlCreateChild(*dialog.GUI, viewport->IdGUIControl), "%s", "????");	// Create group control array

		SControlPlacement	& control				= controlTable.Placement[idControl];
		control.Area.Offset		= {};
		control.Area.Size		= {2, heightOfField};
		control.Align			= ALIGN_TOP_LEFT;
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

gpk::err_t			gpk::viewportAdjustSize	(n2<s1_t> & sizeViewport, const n2<s1_t> & sizeClient)		{
	sizeViewport			= sizeClient;
	sizeViewport.x			+= 4;
	sizeViewport.y			+= 4 + 1 + heightOfField;
	return 0;
}

gpk::err_t			gpk::viewportFold		(SDialogViewport	& control, bool fold)														{
	if((fold && control.Settings.Unfolded) || (false == control.Settings.Unfolded && false == fold)) {
		SDialog				& dialog				= *control.Dialog;
		SGUI					& gui					= *dialog.GUI;
		SControlTable		& controlTable			= gui.Controls;
		SControlPlacement	& placementMain			= controlTable.Placement[control.IdGUIControl];
		SControlPlacement	& placementTitle		= controlTable.Placement[control.IdTitle];
		if(fold && control.Settings.Unfolded)
			placementMain.Area.Size.y	= placementTitle.Area.Size.y + controlNCSpacing(placementMain).y;
		else if(false == control.Settings.Unfolded && false == fold) {
			SControlPlacement	& controlClient			= controlTable.Placement[control.IdClient];
			placementMain.Area.Size.y	= controlClient.Area.Size.y + placementTitle.Area.Size.y + controlNCSpacing(placementMain).y + 1;
		}

		SControlState			& controlMain	= controlTable.States[control.IdGUIControl];
		if(-1 != controlMain.Parent) {
			if_fail_fe(controlMetricsInvalidate(gui, controlMain.Parent));
		}
		else
			if_fail_fe(controlMetricsInvalidate(gui, control.IdGUIControl));

		SControlState		& stateClient			= controlTable.States[control.IdClient];
		stateClient.SetHidden(control.IdClient, controlTable.EventQueue, fold);
		control.Settings.Unfolded	= !fold;
	}
	return one_if(false == control.Settings.Unfolded);
}

sttc gpk::err_t viewportDrag		(SDialogViewport	& control, SControlPlacement & controlMain, SDialog & dialog, n2<bool> locked, n2s1_t mouseDeltas) {
	mouseDeltas.InPlaceScale(1 / dialog.GUI->Zoom.DPI.x, 1 / dialog.GUI->Zoom.DPI.y);
	mouseDeltas.InPlaceScale(1 / dialog.GUI->Zoom.ZoomLevel);
	control.Settings.Dragging	= true;
	if(false == locked.x)
		controlMain.Area.Offset.x	+= ((controlMain.Align & ALIGN_HCENTER) == ALIGN_RIGHT) ? -mouseDeltas.x : mouseDeltas.x;
	if(false == locked.y)
		controlMain.Area.Offset.y	+= ((controlMain.Align & ALIGN_VCENTER) == ALIGN_BOTTOM) ? -mouseDeltas.y : mouseDeltas.y;
	controlMetricsInvalidate(*dialog.GUI, control.IdGUIControl);
	return 0;
}

gpk::err_t	gpk::viewportUpdate		(SDialogViewport	& control)																{
	SDialog				& dialog				= *control.Dialog;
	SGUI					& gui					= *dialog.GUI;
	SControlTable		& controlTable			= dialog.GUI->Controls;
	if(controlTable.IsAction(control.IdClose)) {
		controlTable.SetHidden(control.IdGUIControl, true);
		return 0;
	}
	SControlPlacement	& controlMain			= controlTable.Placement[control.IdGUIControl];
	SControlState		& controlTitleState		= controlTable.States[control.IdTitle];
	SControlState		& controlResizeState	= controlTable.States[control.IdResizeBottomRight];
	if(dialog.Input->ButtonDown(0) && controlTitleState.IsPressed())
		if_fail_e(controlToFront(gui, control.IdGUIControl));
	if(controlTitleState.IsDragging()) {
		 n2b8_c		locked					= {control.Settings.DisplacementLockX, control.Settings.DisplacementLockY};
		if(false == locked.x || false == locked.y) {
			n2s1_t				mouseDeltas				= (dialog.Input->MouseCurrent.Position - dialog.Input->MousePrevious.Position).s1(); //{dialog.Input->MouseCurrent.Deltas.x, dialog.Input->MouseCurrent.Deltas.y};
			if(mouseDeltas.x || mouseDeltas.y)
				if_fail_e(::viewportDrag(control, controlMain, dialog, locked, mouseDeltas));
		}
	}
	else if(controlResizeState.IsDragging()) {
		n2s1_t				mouseDeltas			= dialog.Input->MouseCurrent.Position - dialog.Input->MousePrevious.Position;
		if(mouseDeltas.x || mouseDeltas.y) {
			mouseDeltas.InPlaceScale(1 / dialog.GUI->Zoom.DPI.x, 1 / dialog.GUI->Zoom.DPI.y);
			mouseDeltas.InPlaceScale(1 / dialog.GUI->Zoom.ZoomLevel);
			SControlPlacement	& controlClient			= controlTable.Placement[control.IdClient];
			mouseDeltas.x								= max(mouseDeltas.x, (s1_t)-controlClient.Area.Size.x);
			mouseDeltas.y								= max(mouseDeltas.y, (s1_t)-controlClient.Area.Size.y);
			controlMain.Area.Size						+= mouseDeltas;
			controlClient.Area.Size						+= mouseDeltas;
			if_fail_fe(controlMetricsInvalidate(gui, control.IdGUIControl));
		}
	}
	else {
		if(controlTable.IsAction(control.IdTitle) && false == control.Settings.Dragging)
			if_fail_e(viewportFold(control, control.Settings.Unfolded));
	}

	if(control.Settings.Unfolded != control.SettingsOld.Unfolded)
		if(true == control.Settings.Unfolded) {
			n2s1_t				clientFinalSize			= {controlMain.Area.Size};
			clientFinalSize			-= controlNCSpacing(controlMain).s1();
			clientFinalSize.y		-=  heightOfField + 1;

			SControlPlacement	& controlClient			= controlTable.Placement[control.IdClient];
			controlClient.Area.Size	= clientFinalSize;
		}
	if(false == controlInvalid(gui, control.IdScrollBarY)) {
		if(controlTable.States[control.IdScrollBarY].Parent != control.IdClient)
			if_fail_e(controlSetParent(gui, control.IdScrollBarY, control.IdClient))
			//else {
				SControlPlacement	& placementScroll		= controlTable.Placement[control.IdScrollBarY];
				placementScroll.Area.Offset							= {};
				placementScroll.Area.Size.x							= 12;
				placementScroll.Align								= ALIGN_TOP_RIGHT;

				SControlConstraints	& constraintsScroll		= controlTable.Constraints[control.IdScrollBarY];
				constraintsScroll.AttachSizeToControl				= {-1, control.IdScrollBarY};
				constraintsScroll.DockToControl						= {-1, -1, -1, -1};
				controlTable.SetHidden(control.IdScrollBarY, false == control.Settings.ScrollBarY || false == control.Settings.Unfolded);
			//}
	}
	if(false == controlInvalid(gui, control.IdScrollBarX)) {
		if(controlTable.States[control.IdScrollBarX].Parent != control.IdClient)
			if_fail_e(controlSetParent(gui, control.IdScrollBarX, control.IdClient))
			//else {
				SControlPlacement	& placementScroll			= controlTable.Placement[control.IdScrollBarX];
				placementScroll.Area.Offset					= {};
				placementScroll.Area.Size.y					= 12;
				placementScroll.Align						= ALIGN_BOTTOM_LEFT;

				SControlConstraints	& constraintsScroll			= controlTable.Constraints[control.IdScrollBarX];
				constraintsScroll.AttachSizeToControl.x		= control.IdScrollBarX;
				constraintsScroll.DockToControl				= {-1, -1, control.Settings.ScrollBarY ? control.IdScrollBarY : (cid_t)-1, -1};
				controlTable.SetHidden(control.IdScrollBarX, false == control.Settings.ScrollBarX || false == control.Settings.Unfolded);
			//}
	}
	if(control.Settings.ScrollBarX != control.SettingsOld.ScrollBarX || control.Settings.ScrollBarY != control.SettingsOld.ScrollBarY) {
		if(false == controlInvalid(gui, control.IdScrollBarX))
			if_fail_e(controlMetricsInvalidate(gui, control.IdScrollBarX));
		if(false == controlInvalid(gui, control.IdScrollBarY))
			if_fail_e(controlMetricsInvalidate(gui, control.IdScrollBarY));
	}
	control.SettingsOld		= control.Settings;
	return 0;
}
