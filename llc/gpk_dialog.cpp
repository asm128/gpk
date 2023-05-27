#include "gpk_dialog.h"
#include "gpk_dialog_controls.h"
#include "gpk_png.h"

								::gpk::IDialogControl::~IDialogControl		() { 
	if(-1 != IdGUIControl && DeleteControl && Dialog->GUI) 
		::gpk::controlDelete(*Dialog->GUI, IdGUIControl, true); 
}

static ::gpk::error_t			dialogInitializeColors			(::gpk::SDialogColors & dialogColors, ::gpk::apod<::gpk::array_static<::gpk::bgra, ::gpk::GUI_CONTROL_COLOR_COUNT>> & palettes) {
	dialogColors.Control		[::gpk::GUI_CONTROL_PALETTE_NORMAL	]			= palettes.push_back({{::gpk::DARKGRAY			, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, {}, ::gpk::WHITE		* .85	,}});
	dialogColors.Control		[::gpk::GUI_CONTROL_PALETTE_DISABLED]			= palettes.push_back({{::gpk::LIGHTGRAY * 1.3	, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::LIGHTGRAY	* 1.2	,}});
	dialogColors.Control		[::gpk::GUI_CONTROL_PALETTE_HOVER	]			= palettes.push_back({{::gpk::GRAY	 			, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, {}, ::gpk::WHITE				,}});
	dialogColors.Control		[::gpk::GUI_CONTROL_PALETTE_PRESSED	]			= palettes.push_back({{::gpk::LIGHTGRAY			, ::gpk::LIGHTBLUE	, ::gpk::LIGHTBLUE	, ::gpk::LIGHTBLUE	, ::gpk::LIGHTBLUE	, {}, ::gpk::WHITE				,}});
	dialogColors.Control		[::gpk::GUI_CONTROL_PALETTE_EXECUTE	]			= palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				,}});
	dialogColors.Control		[::gpk::GUI_CONTROL_PALETTE_OUTDATED]			= palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				,}});

	dialogColors.Button			[::gpk::GUI_CONTROL_PALETTE_NORMAL	]			= palettes.push_back({{::gpk::GRAY				, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, {}, ::gpk::WHITE				,}});
	dialogColors.Button			[::gpk::GUI_CONTROL_PALETTE_DISABLED]			= palettes.push_back({{::gpk::LIGHTGRAY			, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::LIGHTGRAY * 1.1	,}});
	dialogColors.Button			[::gpk::GUI_CONTROL_PALETTE_HOVER	]			= palettes.push_back({{::gpk::GRAY	 	* 1.1	, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, {}, ::gpk::WHITE				,}});
	dialogColors.Button			[::gpk::GUI_CONTROL_PALETTE_PRESSED	]			= palettes.push_back({{::gpk::LIGHTGRAY			, ::gpk::ORANGE		, ::gpk::ORANGE		, ::gpk::ORANGE		, ::gpk::ORANGE		, {}, ::gpk::BLUE				,}});
	dialogColors.Button			[::gpk::GUI_CONTROL_PALETTE_EXECUTE	]			= palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				,}});
	dialogColors.Button			[::gpk::GUI_CONTROL_PALETTE_OUTDATED]			= palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				,}});

	dialogColors.CheckBox		[::gpk::GUI_CONTROL_PALETTE_NORMAL	]			= palettes.push_back({{::gpk::WHITE		* 0.7	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, {}, ::gpk::WHITE				,}});
	dialogColors.CheckBox		[::gpk::GUI_CONTROL_PALETTE_DISABLED]			= palettes.push_back({{::gpk::LIGHTGRAY			, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::LIGHTGRAY * 1.1	,}});
	dialogColors.CheckBox		[::gpk::GUI_CONTROL_PALETTE_HOVER	]			= palettes.push_back({{::gpk::WHITE		* 0.85	, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, {}, ::gpk::WHITE				,}});
	dialogColors.CheckBox		[::gpk::GUI_CONTROL_PALETTE_PRESSED	]			= palettes.push_back({{::gpk::WHITE		 		, ::gpk::BLUE		, ::gpk::BLUE		, ::gpk::BLUE		, ::gpk::BLUE		, {}, ::gpk::WHITE				,}});
	dialogColors.CheckBox		[::gpk::GUI_CONTROL_PALETTE_EXECUTE	]			= palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				,}});
	dialogColors.CheckBox		[::gpk::GUI_CONTROL_PALETTE_OUTDATED]			= palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				,}});

	dialogColors.Viewport		[::gpk::GUI_CONTROL_PALETTE_NORMAL	]			= palettes.push_back({{::gpk::WHITE		* 0.7	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, {}, ::gpk::WHITE				,}});
	dialogColors.Viewport		[::gpk::GUI_CONTROL_PALETTE_DISABLED]			= palettes.push_back({{::gpk::LIGHTGRAY			, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::LIGHTGRAY * 1.1	,}});
	dialogColors.Viewport		[::gpk::GUI_CONTROL_PALETTE_HOVER	]			= palettes.push_back({{::gpk::WHITE		* 0.85	, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, {}, ::gpk::WHITE				,}});
	dialogColors.Viewport		[::gpk::GUI_CONTROL_PALETTE_PRESSED	]			= palettes.push_back({{::gpk::WHITE		 		, ::gpk::BLUE		, ::gpk::BLUE		, ::gpk::BLUE		, ::gpk::BLUE		, {}, ::gpk::WHITE				,}});
	dialogColors.Viewport		[::gpk::GUI_CONTROL_PALETTE_EXECUTE	]			= palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				,}});
	dialogColors.Viewport		[::gpk::GUI_CONTROL_PALETTE_OUTDATED]			= palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				,}});

	dialogColors.ViewportTitle	[::gpk::GUI_CONTROL_PALETTE_NORMAL	]			= dialogColors.Button[::gpk::GUI_CONTROL_PALETTE_NORMAL		]; palettes.push_back({{::gpk::WHITE		* 0.7	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, {}, ::gpk::WHITE				,}});
	dialogColors.ViewportTitle	[::gpk::GUI_CONTROL_PALETTE_DISABLED]			= dialogColors.Button[::gpk::GUI_CONTROL_PALETTE_DISABLED	]; palettes.push_back({{::gpk::LIGHTGRAY			, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::LIGHTGRAY * 1.1	,}});
	dialogColors.ViewportTitle	[::gpk::GUI_CONTROL_PALETTE_HOVER	]			= dialogColors.Button[::gpk::GUI_CONTROL_PALETTE_HOVER		]; palettes.push_back({{::gpk::WHITE		* 0.85	, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, {}, ::gpk::WHITE				,}});
	dialogColors.ViewportTitle	[::gpk::GUI_CONTROL_PALETTE_PRESSED	]			= dialogColors.Button[::gpk::GUI_CONTROL_PALETTE_PRESSED	]; palettes.push_back({{::gpk::WHITE		 		, ::gpk::BLUE		, ::gpk::BLUE		, ::gpk::BLUE		, ::gpk::BLUE		, {}, ::gpk::WHITE				,}});
	dialogColors.ViewportTitle	[::gpk::GUI_CONTROL_PALETTE_EXECUTE	]			= dialogColors.Button[::gpk::GUI_CONTROL_PALETTE_EXECUTE	]; palettes.push_back({{::gpk::BLUE					, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				,}});
	dialogColors.ViewportTitle	[::gpk::GUI_CONTROL_PALETTE_OUTDATED]			= dialogColors.Button[::gpk::GUI_CONTROL_PALETTE_OUTDATED	]; palettes.push_back({{::gpk::BLUE					, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				,}});
	for(uint32_t i=0; i < 4; ++i) {
		dialogColors.Control		[::gpk::GUI_CONTROL_PALETTE_SELECTED + i]		= dialogColors.Control			[::gpk::GUI_CONTROL_PALETTE_NORMAL	+ i];
		dialogColors.Button			[::gpk::GUI_CONTROL_PALETTE_SELECTED + i]		= dialogColors.Button			[::gpk::GUI_CONTROL_PALETTE_NORMAL	+ i];
		dialogColors.CheckBox		[::gpk::GUI_CONTROL_PALETTE_SELECTED + i]		= dialogColors.CheckBox			[::gpk::GUI_CONTROL_PALETTE_NORMAL	+ i];
		dialogColors.Viewport		[::gpk::GUI_CONTROL_PALETTE_SELECTED + i]		= dialogColors.Viewport			[::gpk::GUI_CONTROL_PALETTE_NORMAL	+ i];
		dialogColors.ViewportTitle	[::gpk::GUI_CONTROL_PALETTE_SELECTED + i]		= dialogColors.ViewportTitle	[::gpk::GUI_CONTROL_PALETTE_NORMAL	+ i];
	}
	return 0;
}

static ::gpk::error_t			dialogInitialize				(::gpk::SDialog & dialog) {
	::gpk::SGUI							& gui							= *dialog.GUI; 
	gthrow_if(-1 == (dialog.Root = ::gpk::controlCreate(gui)), "%s", "Out of memory?");
	gui.Controls.Controls		[dialog.Root].Margin					= {};
	gui.Controls.Controls		[dialog.Root].Border					= {};
	gui.Controls.Constraints	[dialog.Root].AttachSizeToControl		= {dialog.Root, dialog.Root};
	gui.Controls.Modes			[dialog.Root].Design					= true;
	dialog.DefaultControlModes.NoHoverEffect	= 1;
	dialog.DefaultControlModes.FrameOut			= 1;
	dialog.DefaultControlModes.UseNewPalettes	= 1;
	if(!dialog.Colors) {
		static ::gpk::pobj<::gpk::SDialogColors>	globalDefaultColors			= {};
		if(!globalDefaultColors) {
			gpk_necs(::dialogInitializeColors(*globalDefaultColors.create(), gui.Colors->Palettes));
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

stacxpr	const uint32_t									heightOfField								= 18;
::gpk::error_t							gpk::checkBoxCreate							(::gpk::SDialog			& dialog)								{
	int32_t																	index										= -1;
	::gpk::pobj<::gpk::SDialogCheckBox>									checkBox;
	::gpk::SGUIControlTable													& controlTable								= dialog.GUI->Controls;
	gpk_necall(index = dialog.Create(checkBox), "%s", "Out of memory?");
	controlTable.Modes[checkBox->IdGUIControl]							= dialog.DefaultControlModes;
	::gpk::memcpy_s(controlTable.Controls[checkBox->IdGUIControl].Palettes.Storage, dialog.Colors->CheckBox.Storage);
	return index;
}
::gpk::error_t							gpk::checkBoxUpdate							(::gpk::SDialogCheckBox	& checkbox)								{
	::gpk::SDialog															& dialog									= *checkbox.Dialog;
	::gpk::SGUIControlTable													& controlTable								= dialog.GUI->Controls;
	::gpk::img<::gpk::bgra>											& imageCross								= dialog.ImageCrossBGRA;
	if(controlTable.States[checkbox.IdGUIControl].Execute) {
		if(dialog.ImageCross.Texels.size() < 4) {
			::gpk::pngFileLoad("../gpk_data/images/cross.png", imageCross);

			dialog.ImageCross.resize(imageCross.metrics());
			for(uint32_t iTexel = 0; iTexel < imageCross.Texels.size(); ++iTexel) {
				if(imageCross.Texels[iTexel] == (::gpk::bgra)::gpk::BLACK)
					dialog.ImageCross.View[iTexel]										=  true;
				else {
					imageCross.Texels[iTexel].a											= 0;
					dialog.ImageCross.View[iTexel]										= false;
				}
			}
		}
		checkbox.Checked													= !checkbox.Checked;
		controlTable.Controls[checkbox.IdGUIControl].Image					= (false == checkbox.Checked) ? ::gpk::view2d<::gpk::bgra>{} : imageCross.View;
	}
	return 0;
}

::gpk::error_t							gpk::sliderCreate							(::gpk::SDialog & dialog)								{
	int32_t										index										= -1;
	::gpk::pobj<::gpk::SDialogSlider>			slider;
	gpk_necall(index = dialog.Create(slider), "%s", "Out of memory?");
	gpk_necall(slider->IdButton = ::gpk::controlCreateChild(*dialog.GUI, slider->IdGUIControl), "%s", "Out of memory?");
	::gpk::SGUIControlTable						& controlTable								= dialog.GUI->Controls;
	controlTable.Controls	[slider->IdGUIControl].Margin				= {};
	controlTable.Controls	[slider->IdButton].Area.Size				= {16, 16};
	controlTable.Text		[slider->IdButton].Text						= "-";
	controlTable.Constraints[slider->IdButton].AttachSizeToControl.y	= slider->IdButton;
	controlTable.Modes		[slider->IdButton].UseNewPalettes			= true;
	controlTable.Modes		[slider->IdButton].ColorMode				= ::gpk::GUI_COLOR_MODE_3D;
	::gpk::memcpy_s(controlTable.Controls[slider->IdButton		].Palettes.Storage, dialog.Colors->Button		.Storage);
	::gpk::memcpy_s(controlTable.Controls[slider->IdGUIControl	].Palettes.Storage, dialog.Colors->CheckBox	.Storage);
	::gpk::SControlText							& tunerText									= controlTable.Text[slider->IdGUIControl];
	tunerText.Text							= {slider->ValueString, (uint32_t)snprintf(slider->ValueString, ::gpk::size(slider->ValueString) - 2, "%lli", (long long int)slider->ValueCurrent)};
	return index;
}
::gpk::error_t							gpk::sliderSetValue							(::gpk::SDialogSlider & slider, int64_t value)				{
	::gpk::SDialog								& dialog									= *slider.Dialog;
	::gpk::SGUIControlTable						& controlTable								= dialog.GUI->Controls;
	const uint64_t								newValue									= ::gpk::max(::gpk::min(value, slider.ValueLimits.Max), slider.ValueLimits.Min);;
	slider.ValueCurrent						= newValue;

	const double								proportion									= (slider.ValueCurrent - slider.ValueLimits.Min) * (1.0 / (slider.ValueLimits.Max - slider.ValueLimits.Min));

	::gpk::SControl								& controlButton								= controlTable.Controls[slider.IdButton];
	::gpk::SControl								& controlSlider								= controlTable.Controls[slider.IdGUIControl];
	::gpk::n2<double>							ncSpacing									= ::gpk::controlNCSpacing(controlSlider).Cast<double>();
	if(slider.Vertical)	
		controlButton.Area.Offset.y				= int16_t((controlSlider.Area.Size.y - controlButton.Area.Size.y - ncSpacing.y) * proportion);
	else				
		controlButton.Area.Offset.x				= int16_t((controlSlider.Area.Size.x - controlButton.Area.Size.x - ncSpacing.x) * proportion);

	::gpk::SControlConstraints					& buttonConstraints							= controlTable.Constraints[slider.IdButton];
	if(slider.Vertical) {
		buttonConstraints.AttachSizeToControl.y	= -1;
		buttonConstraints.AttachSizeToControl.x	= slider.IdButton;
	}
	else {
		buttonConstraints.AttachSizeToControl.x	= -1;
		buttonConstraints.AttachSizeToControl.y	= slider.IdButton;
	}
	::gpk::vcc									valueString;
	gpk_necs(slider.FuncValueFormat(valueString, slider.ValueCurrent, slider.ValueLimits));
	gpk_necs(slider.FuncGetString(valueString, slider.ValueCurrent, slider.ValueLimits));
	gpk_necs(::gpk::controlTextSet(*dialog.GUI, slider.IdGUIControl, valueString));
	gpk_necs(::gpk::controlMetricsInvalidate(*dialog.GUI, slider.IdGUIControl));
	return 0;
}

::gpk::error_t							gpk::sliderUpdate							(::gpk::SDialogSlider	& slider)								{
	::gpk::SDialog															& dialog									= *slider.Dialog;
	::gpk::SGUIControlTable													& controlTable								= dialog.GUI->Controls;
	const int64_t															valueRange									= slider.ValueLimits.Max - slider.ValueLimits.Min;
	if(controlTable.States[slider.IdButton].Pressed) {
		if(dialog.Input->MouseCurrent.Position != dialog.Input->MousePrevious.Position) {
			::gpk::SInput															& input										= *dialog.Input;
			//const ::gpk::SGUIZoom													& zoom										= dialog.GUI->Zoom;
			//::gpk::n2d																scale										=
			//	{ 1.0 / (zoom.ZoomLevel * zoom.DPI.x)
			//	, 1.0 / (zoom.ZoomLevel * zoom.DPI.y)
			//	};
			const ::gpk::n2d														controlSliderPos							= controlTable.Metrics[slider.IdGUIControl].Client.Global.Offset.Cast<double>();
			const ::gpk::n2d														controlSliderSize							= controlTable.Metrics[slider.IdGUIControl].Client.Global.Size.Cast<double>();
			::gpk::n2d																effectiveSize								= controlSliderSize - (controlTable.Controls[slider.IdButton].Area.Size).Cast<double>();;
			//

			::gpk::n2d																currentValue								= input.MouseCurrent.Position.Cast<double>();
			const ::gpk::n2d														valueUnit									= {1.0 / effectiveSize.x, 1.0 / effectiveSize.y};
			currentValue														-= controlSliderPos + controlTable.Controls[slider.IdButton].Area.Size.Cast<double>() / 2;
			currentValue.x														*= valueUnit.x;
			currentValue.y														*= valueUnit.y;

			gpk_necs(::gpk::sliderSetValue(slider, (int64_t)((slider.Vertical ? currentValue.y : currentValue.x) * valueRange + slider.ValueLimits.Min)));
		}
	}
	else
		if(dialog.Input->ButtonDown(0) && controlTable.States[slider.IdGUIControl].Pressed) {	//
			::gpk::SInput															& input										= *dialog.Input;
			const int64_t															valuePage									= ::gpk::max((int64_t)1LL, (valueRange > 10) ? (int64_t)(valueRange * .1) : valueRange / 2);
			const ::gpk::n2<int16_t>											& controlButtonPosition						= controlTable.Metrics[slider.IdButton].Total.Global.Offset;
			if(slider.Vertical) {
				const int64_t															finalValue									= slider.ValueCurrent + ((input.MouseCurrent.Position.y > controlButtonPosition.y) ? valuePage : -valuePage);
				gpk_necs(::gpk::sliderSetValue(slider, finalValue));
			}
			else {
				const int64_t															finalValue									= slider.ValueCurrent + ((input.MouseCurrent.Position.x > controlButtonPosition.x) ? valuePage : -valuePage);
				gpk_necs(::gpk::sliderSetValue(slider, finalValue));
			}
		}
	return 0;
}

::gpk::error_t							gpk::editBoxCreate							(::gpk::SDialog			& dialog )								{
	int32_t																	index										= -1;
	::gpk::pobj<::gpk::SDialogEditBox>									editBox;
	::gpk::SGUIControlTable													& controlTable								= dialog.GUI->Controls;
	gpk_necall(index = dialog.Create(editBox), "%s", "Out of memory?");
	controlTable.Modes[editBox->IdGUIControl].UseNewPalettes			= true;
	controlTable.Modes[editBox->IdGUIControl].ColorMode					= ::gpk::GUI_COLOR_MODE_FLAT;
	::gpk::memcpy_s(controlTable.Controls[editBox->IdGUIControl].Palettes.Storage, dialog.Colors->CheckBox.Storage);
	return index;
}
::gpk::error_t							gpk::editBoxUpdate							(::gpk::SDialogEditBox	& control)								{
	::gpk::SDialog															& dialog									= *control.Dialog;
	if(dialog.Input->ButtonDown(0)) {
	}
	return 0;
}
::gpk::error_t							gpk::viewportCreate							(::gpk::SDialog			& dialog)								{
	int32_t																	index										= -1;
	::gpk::pobj<::gpk::SDialogViewport>									viewport;
	::gpk::SGUIControlTable													& controlTable								= dialog.GUI->Controls;
	gpk_necall(index = dialog.Create(viewport), "%s", "Out of memory?");
	controlTable.Modes[viewport->IdGUIControl].UseNewPalettes			= true;
	controlTable.Modes[viewport->IdGUIControl].FrameOut					= true;
	controlTable.Modes[viewport->IdGUIControl].NoHoverEffect			= true;
	controlTable.Controls[viewport->IdGUIControl].Margin				= {};
	::gpk::memcpy_s(controlTable.Controls[viewport->IdGUIControl].Palettes.Storage, dialog.Colors->Viewport.Storage);
	{ // Create field group
		// Set up client area control
		int32_t																	idControl							= ::gpk::controlCreateChild(*dialog.GUI, viewport->IdGUIControl);
		gpk_necall(viewport->IdClient = idControl, "%s", "????");		// Create group control array
		::gpk::SControl															& control							= controlTable.Controls		[idControl];
		::gpk::SControlConstraints												& constraints						= controlTable.Constraints	[idControl];
		control.Area.Offset													= {0, heightOfField + 1};
		control.Area.Size													= {2, 2};
		control.Margin														= {};
		constraints.AttachSizeToControl.x									= idControl;

		::gpk::SControlMode														& modes								= controlTable.Modes[idControl];
		modes																= dialog.DefaultControlModes;
		control.Palettes = dialog.Colors->Viewport;
	}
	{ // Create section title
		int32_t																	idControl							= ::gpk::controlCreateChild(*dialog.GUI, viewport->IdGUIControl);
		gpk_necall(viewport->IdTitle = idControl, "%s", "????");	// Create group control array

		::gpk::SControl															& control							= controlTable.Controls		[idControl];
		::gpk::SControlConstraints												& constraints						= controlTable.Constraints	[idControl];
		control.Area.Offset													= {};
		control.Area.Size													= {2, heightOfField};
		control.Align														= ::gpk::ALIGN_TOP_LEFT;
		control.Margin.Left													= 4;
		constraints.AttachSizeToControl.x									= idControl;

		::gpk::SControlMode														& modes								= controlTable.Modes[idControl];
		modes																= dialog.DefaultControlModes;
		controlTable.Text[idControl].Text									= "Viewport";
		control.Palettes													= dialog.Colors->ViewportTitle;
	}
	viewport->Settings.Unfolded											= true;
	return index;
}

::gpk::error_t							gpk::viewportAdjustSize	(::gpk::n2<int16_t> & sizeViewport, const ::gpk::n2<int16_t> & sizeClient)		{
	sizeViewport							= sizeClient;
	sizeViewport.x							+= 4;
	sizeViewport.y							+= 4 + 1 + heightOfField;
	return 0;
}

::gpk::error_t							gpk::viewportFold		(::gpk::SDialogViewport	& control, bool fold)														{
	if((fold && control.Settings.Unfolded) || (false == control.Settings.Unfolded && false == fold)) {
		::gpk::SDialog								& dialog									= *control.Dialog;
		::gpk::SGUIControlTable						& controlTable								= dialog.GUI->Controls;
		::gpk::SControl								& controlMain								= controlTable.Controls[control.IdGUIControl];
		::gpk::SControl								& controlTitle								= controlTable.Controls[control.IdTitle];
		::gpk::SControlState						& stateClient								= controlTable.States[control.IdClient];
		if(fold && control.Settings.Unfolded)
			controlMain.Area.Size.y					= controlTitle.Area.Size.y + ::gpk::controlNCSpacing(controlMain).y;
		else if(false == control.Settings.Unfolded && false == fold) {
			::gpk::SControl								& controlClient								= controlTable.Controls[control.IdClient];
			controlMain.Area.Size.y					= controlClient.Area.Size.y + controlTitle.Area.Size.y + ::gpk::controlNCSpacing(controlMain).y + 1;
		}
		if(-1 != controlMain.Parent)
			gpk_necs(::gpk::controlMetricsInvalidate(*dialog.GUI, controlMain.Parent));
		else
			gpk_necs(::gpk::controlMetricsInvalidate(*dialog.GUI, control.IdGUIControl));
		stateClient.Hidden						= fold;
		control.Settings.Unfolded				= !fold;
	}
	return one_if(false == control.Settings.Unfolded);
}

static	::gpk::error_t					viewportDrag			(::gpk::SDialogViewport	& control, ::gpk::SControl & controlMain, ::gpk::SDialog & dialog, ::gpk::n2<bool> locked, ::gpk::n2<int32_t> mouseDeltas)																{
	mouseDeltas.InPlaceScale(1 / dialog.GUI->Zoom.DPI.x, 1 / dialog.GUI->Zoom.DPI.y);
	mouseDeltas.InPlaceScale(1 / dialog.GUI->Zoom.ZoomLevel);
	control.Settings.Dragging											= true;
	if(false == locked.x)
		controlMain.Area.Offset.x											+= (controlMain.Align & ::gpk::ALIGN_RIGHT) ? -(int16_t)mouseDeltas.x : (int16_t)mouseDeltas.x;
	if(false == locked.y)
		controlMain.Area.Offset.y											+= (controlMain.Align & ::gpk::ALIGN_BOTTOM) ? -(int16_t)mouseDeltas.y : (int16_t)mouseDeltas.y;
	::gpk::controlMetricsInvalidate(*dialog.GUI, control.IdGUIControl);
	return 0;
}

::gpk::error_t							gpk::viewportUpdate		(::gpk::SDialogViewport	& control)																{
	::gpk::SDialog								& dialog				= *control.Dialog;
	::gpk::SGUIControlTable						& controlTable			= dialog.GUI->Controls;
	::gpk::SControl								& controlMain			= controlTable.Controls[control.IdGUIControl];
	::gpk::SControlState						& controlTitle			= controlTable.States[control.IdTitle];
	if(controlTitle.Pressed) {
		::gpk::n2<bool>								locked					= {control.Settings.DisplacementLockX, control.Settings.DisplacementLockY};
		if(false == locked.x || false == locked.y) {
			::gpk::n2i32								mouseDeltas				= -(dialog.Input->MouseCurrent.Position - dialog.Input->MousePrevious.Position); //{dialog.Input->MouseCurrent.Deltas.x, dialog.Input->MouseCurrent.Deltas.y};
			if(mouseDeltas.x || mouseDeltas.y)
				gpk_necs(::viewportDrag(control, controlMain, dialog, locked, mouseDeltas));
		}
	}
	else {
		if(controlTitle.Execute && false == control.Settings.Dragging)
			gpk_necs(::gpk::viewportFold(control, control.Settings.Unfolded));
		control.Settings.Dragging				= false;
	}
	if(control.Settings.Unfolded != control.SettingsOld.Unfolded)
		if(true == control.Settings.Unfolded) {
			::gpk::n2i16								clientFinalSize			= {controlMain.Area.Size};
			clientFinalSize							-= ::gpk::controlNCSpacing(controlMain);
			clientFinalSize.y						-=  heightOfField + 1;
			::gpk::SControl								& controlClient			= controlTable.Controls[control.IdClient];
			controlClient.Area.Size					= clientFinalSize;
		}
	control.SettingsOld						= control.Settings;
	return 0;
}
