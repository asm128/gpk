#include "gpk_dialog.h"
#include "gpk_dialog_controls.h"
#include "gpk_png.h"

																	::gpk::IDialogControl::~IDialogControl		()																{ if(-1 != IdGUIControl && DeleteControl && Dialog->GUI) ::gpk::controlDelete(*Dialog->GUI, IdGUIControl, true); }
																	::gpk::SDialog::SDialog						()																{
	GUI.create();
	::gpk::SGUI																& gui										= *GUI;
	gthrow_if(-1 == (Root = ::gpk::controlCreate(gui)), "%s", "Out of memory?");
	gui.Controls.Controls		[Root].Margin							= {};
	gui.Controls.Controls		[Root].Border							= {};
	gui.Controls.Constraints	[Root].AttachSizeToControl				= {Root, Root};
	gui.Controls.Modes			[Root].Design							= true;
	gui.ColorModeDefault												= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault													= ::gpk::ASCII_COLOR_CYAN * 16 + 8;

	DefaultControlModes.ColorMode										= ::gpk::GUI_COLOR_MODE_3D;
	DefaultControlModes.NoHoverEffect									= 1;
	DefaultControlModes.FrameOut										= 1;
	DefaultControlModes.UseNewPalettes									= 1;

	ColorsControl		[::gpk::GUI_CONTROL_PALETTE_NORMAL	]			= gui.Palettes.push_back({{::gpk::DARKGRAY			, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, {}, ::gpk::WHITE		* .85	,}});
	ColorsControl		[::gpk::GUI_CONTROL_PALETTE_DISABLED]			= gui.Palettes.push_back({{::gpk::LIGHTGRAY * 1.3	, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::LIGHTGRAY	* 1.2	,}});
	ColorsControl		[::gpk::GUI_CONTROL_PALETTE_HOVER	]			= gui.Palettes.push_back({{::gpk::GRAY	 			, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, {}, ::gpk::WHITE				,}});
	ColorsControl		[::gpk::GUI_CONTROL_PALETTE_PRESSED	]			= gui.Palettes.push_back({{::gpk::LIGHTGRAY			, ::gpk::LIGHTBLUE	, ::gpk::LIGHTBLUE	, ::gpk::LIGHTBLUE	, ::gpk::LIGHTBLUE	, {}, ::gpk::WHITE				,}});
	ColorsControl		[::gpk::GUI_CONTROL_PALETTE_EXECUTE	]			= gui.Palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				,}});
	ColorsControl		[::gpk::GUI_CONTROL_PALETTE_OUTDATED]			= gui.Palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				,}});

	ColorsButton		[::gpk::GUI_CONTROL_PALETTE_NORMAL	]			= gui.Palettes.push_back({{::gpk::GRAY				, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, {}, ::gpk::WHITE				,}});
	ColorsButton		[::gpk::GUI_CONTROL_PALETTE_DISABLED]			= gui.Palettes.push_back({{::gpk::LIGHTGRAY			, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::LIGHTGRAY * 1.1	,}});
	ColorsButton		[::gpk::GUI_CONTROL_PALETTE_HOVER	]			= gui.Palettes.push_back({{::gpk::GRAY	 	* 1.1	, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, {}, ::gpk::WHITE				,}});
	ColorsButton		[::gpk::GUI_CONTROL_PALETTE_PRESSED	]			= gui.Palettes.push_back({{::gpk::LIGHTGRAY			, ::gpk::ORANGE		, ::gpk::ORANGE		, ::gpk::ORANGE		, ::gpk::ORANGE		, {}, ::gpk::BLUE				,}});
	ColorsButton		[::gpk::GUI_CONTROL_PALETTE_EXECUTE	]			= gui.Palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				,}});
	ColorsButton		[::gpk::GUI_CONTROL_PALETTE_OUTDATED]			= gui.Palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				,}});

	ColorsCheckBox		[::gpk::GUI_CONTROL_PALETTE_NORMAL	]			= gui.Palettes.push_back({{::gpk::WHITE		* 0.7	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, {}, ::gpk::WHITE				,}});
	ColorsCheckBox		[::gpk::GUI_CONTROL_PALETTE_DISABLED]			= gui.Palettes.push_back({{::gpk::LIGHTGRAY			, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::LIGHTGRAY * 1.1	,}});
	ColorsCheckBox		[::gpk::GUI_CONTROL_PALETTE_HOVER	]			= gui.Palettes.push_back({{::gpk::WHITE		* 0.85	, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, {}, ::gpk::WHITE				,}});
	ColorsCheckBox		[::gpk::GUI_CONTROL_PALETTE_PRESSED	]			= gui.Palettes.push_back({{::gpk::WHITE		 		, ::gpk::BLUE		, ::gpk::BLUE		, ::gpk::BLUE		, ::gpk::BLUE		, {}, ::gpk::WHITE				,}});
	ColorsCheckBox		[::gpk::GUI_CONTROL_PALETTE_EXECUTE	]			= gui.Palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				,}});
	ColorsCheckBox		[::gpk::GUI_CONTROL_PALETTE_OUTDATED]			= gui.Palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				,}});

	ColorsViewport		[::gpk::GUI_CONTROL_PALETTE_NORMAL	]			= gui.Palettes.push_back({{::gpk::WHITE		* 0.7	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, {}, ::gpk::WHITE				,}});
	ColorsViewport		[::gpk::GUI_CONTROL_PALETTE_DISABLED]			= gui.Palettes.push_back({{::gpk::LIGHTGRAY			, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::LIGHTGRAY * 1.1	,}});
	ColorsViewport		[::gpk::GUI_CONTROL_PALETTE_HOVER	]			= gui.Palettes.push_back({{::gpk::WHITE		* 0.85	, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, {}, ::gpk::WHITE				,}});
	ColorsViewport		[::gpk::GUI_CONTROL_PALETTE_PRESSED	]			= gui.Palettes.push_back({{::gpk::WHITE		 		, ::gpk::BLUE		, ::gpk::BLUE		, ::gpk::BLUE		, ::gpk::BLUE		, {}, ::gpk::WHITE				,}});
	ColorsViewport		[::gpk::GUI_CONTROL_PALETTE_EXECUTE	]			= gui.Palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				,}});
	ColorsViewport		[::gpk::GUI_CONTROL_PALETTE_OUTDATED]			= gui.Palettes.push_back({{::gpk::BLUE				, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				,}});

	ColorsViewportTitle	[::gpk::GUI_CONTROL_PALETTE_NORMAL	]			= ColorsButton[::gpk::GUI_CONTROL_PALETTE_NORMAL	]; gui.Palettes.push_back({{::gpk::WHITE		* 0.7	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, ::gpk::DARKGRAY	, {}, ::gpk::WHITE				,}});
	ColorsViewportTitle	[::gpk::GUI_CONTROL_PALETTE_DISABLED]			= ColorsButton[::gpk::GUI_CONTROL_PALETTE_DISABLED	]; gui.Palettes.push_back({{::gpk::LIGHTGRAY			, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::LIGHTGRAY * 1.1	,}});
	ColorsViewportTitle	[::gpk::GUI_CONTROL_PALETTE_HOVER	]			= ColorsButton[::gpk::GUI_CONTROL_PALETTE_HOVER		]; gui.Palettes.push_back({{::gpk::WHITE		* 0.85	, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, ::gpk::GRAY		, {}, ::gpk::WHITE				,}});
	ColorsViewportTitle	[::gpk::GUI_CONTROL_PALETTE_PRESSED	]			= ColorsButton[::gpk::GUI_CONTROL_PALETTE_PRESSED	]; gui.Palettes.push_back({{::gpk::WHITE		 		, ::gpk::BLUE		, ::gpk::BLUE		, ::gpk::BLUE		, ::gpk::BLUE		, {}, ::gpk::WHITE				,}});
	ColorsViewportTitle	[::gpk::GUI_CONTROL_PALETTE_EXECUTE	]			= ColorsButton[::gpk::GUI_CONTROL_PALETTE_EXECUTE	]; gui.Palettes.push_back({{::gpk::BLUE					, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				,}});
	ColorsViewportTitle	[::gpk::GUI_CONTROL_PALETTE_OUTDATED]			= ColorsButton[::gpk::GUI_CONTROL_PALETTE_OUTDATED	]; gui.Palettes.push_back({{::gpk::BLUE					, ::gpk::ORANGE		, ::gpk::YELLOW		, ::gpk::MAGENTA	, ::gpk::CYAN		, {}, ::gpk::WHITE				,}});
	for(uint32_t i=0; i < 4; ++i) {
		ColorsControl		[::gpk::GUI_CONTROL_PALETTE_SELECTED + i]		= ColorsControl			[::gpk::GUI_CONTROL_PALETTE_NORMAL	+ i];
		ColorsButton		[::gpk::GUI_CONTROL_PALETTE_SELECTED + i]		= ColorsButton			[::gpk::GUI_CONTROL_PALETTE_NORMAL	+ i];
		ColorsCheckBox		[::gpk::GUI_CONTROL_PALETTE_SELECTED + i]		= ColorsCheckBox		[::gpk::GUI_CONTROL_PALETTE_NORMAL	+ i];
		ColorsViewport		[::gpk::GUI_CONTROL_PALETTE_SELECTED + i]		= ColorsViewport		[::gpk::GUI_CONTROL_PALETTE_NORMAL	+ i];
		ColorsViewportTitle	[::gpk::GUI_CONTROL_PALETTE_SELECTED + i]		= ColorsViewportTitle	[::gpk::GUI_CONTROL_PALETTE_NORMAL	+ i];
	}
}

static constexpr	const uint32_t									heightOfField								= 18;
		::gpk::error_t												gpk::checkBoxCreate							(::gpk::SDialog			& dialog)								{
	int32_t																	index										= -1;
	::gpk::ptr_obj<::gpk::SDialogCheckBox>									checkBox;
	::gpk::SGUIControlTable													& controlTable								= dialog.GUI->Controls;
	gpk_necall(index = dialog.Create(checkBox), "%s", "Out of memory?");
	controlTable.Modes[checkBox->IdGUIControl]							= dialog.DefaultControlModes;
	::gpk::memcpy_s(controlTable.Controls[checkBox->IdGUIControl].Palettes.Storage, dialog.ColorsCheckBox.Storage);
	return index;
}
		::gpk::error_t												gpk::checkBoxUpdate							(::gpk::SDialogCheckBox	& checkbox)								{
	::gpk::SDialog															& dialog									= *checkbox.Dialog;
	::gpk::SGUIControlTable													& controlTable								= dialog.GUI->Controls;
	::gpk::SImage<::gpk::SColorBGRA>										& imageCross								= dialog.ImageCrossBGRA;
	if(controlTable.States[checkbox.IdGUIControl].Execute) {
		if(dialog.ImageCross.Texels.size() < 4) {
			::gpk::pngFileLoad("../gpk_data/images/cross.png", imageCross);

			dialog.ImageCross.resize(imageCross.metrics());
			for(uint32_t iTexel = 0; iTexel < imageCross.Texels.size(); ++iTexel) {
				if(imageCross.Texels[iTexel] == ::gpk::BLACK)
					dialog.ImageCross.View[iTexel]										=  true;
				else {
					imageCross.Texels[iTexel].a											= 0;
					dialog.ImageCross.View[iTexel]										= false;
				}
			}
		}
		checkbox.Checked													= !checkbox.Checked;
		controlTable.Controls[checkbox.IdGUIControl].Image					= (false == checkbox.Checked) ? ::gpk::view_grid<::gpk::SColorBGRA>{} : imageCross.View;
	}
	return 0;
}
		::gpk::error_t												gpk::tunerCreate							(::gpk::SDialog			& dialog)								{
	int32_t																	index										= -1;
	::gpk::ptr_obj<::gpk::SDialogTuner>										tuner;
	gpk_necall(index = dialog.Create(tuner), "%s", "Out of memory?");
	tuner->ValueCurrent													= 0; //tuner->ValueLimits.Min;
	gpk_necall(tuner->IdDecrease = ::gpk::controlCreateChild(*dialog.GUI, tuner->IdGUIControl), "%s", "Out of memory?");
	gpk_necall(tuner->IdIncrease = ::gpk::controlCreateChild(*dialog.GUI, tuner->IdGUIControl), "%s", "Out of memory?");
	::gpk::SGUIControlTable													& controlTable								= dialog.GUI->Controls;
	controlTable.Controls[tuner->IdGUIControl].Margin					= {};
	for(int32_t iControl = tuner->IdDecrease; iControl < tuner->IdIncrease + 1; ++iControl) {
		::gpk::SControl															& controlButton								= controlTable.Controls[iControl];
		controlButton.Align													= (iControl == tuner->IdDecrease) ? ::gpk::ALIGN_CENTER_LEFT : ::gpk::ALIGN_CENTER_RIGHT;
		::gpk::memcpy_s(controlButton.Palettes.Storage, dialog.ColorsButton.Storage);
		controlTable.Text			[iControl].Text							= (iControl == tuner->IdDecrease) ? "-" : "+";
		controlTable.Modes			[iControl].UseNewPalettes				= true;
		controlTable.Modes			[iControl].ColorMode					= ::gpk::GUI_COLOR_MODE_3D;
		controlTable.Constraints	[iControl].AttachSizeToControl.y		= iControl;
	}
	::gpk::SControlText														& tunerText									= controlTable.Text[tuner->IdGUIControl];
	tunerText.Text														= {tuner->ValueString, (uint32_t)snprintf(tuner->ValueString, ::gpk::size(tuner->ValueString) - 2, "%lli", (long long int)tuner->ValueCurrent)};
	tunerText.Align														= ::gpk::ALIGN_CENTER;
	return index;
}
		::gpk::error_t												gpk::tunerSetValue							(::gpk::SDialogTuner	& control, int64_t value)				{
	if(value < control.ValueLimits.Min)
		value																= control.ValueLimits.Min;
	else if(value > control.ValueLimits.Max)
		value																= control.ValueLimits.Max;
	control.ValueCurrent												= value;
	control.Dialog->GUI->Controls.Text[control.IdGUIControl].Text		= {control.ValueString, (uint32_t)snprintf(control.ValueString, ::gpk::size(control.ValueString) - 2, "%lli", (long long int)control.ValueCurrent)};
	::gpk::controlMetricsInvalidate(*control.Dialog->GUI, control.IdGUIControl);
	return 0;
}
		::gpk::error_t												gpk::tunerUpdate							(::gpk::SDialogTuner	& tuner)								{
	::gpk::SDialog															& dialog									= *tuner.Dialog;
	::gpk::SGUIControlTable													& controlTable								= dialog.GUI->Controls;
	if(controlTable.States[tuner.IdDecrease].Execute || controlTable.States[tuner.IdIncrease].Execute) {
		if(controlTable.States[tuner.IdDecrease].Execute)
			::gpk::tunerSetValue(tuner, tuner.ValueCurrent - 1);
		else if(controlTable.States[tuner.IdIncrease].Execute)
			::gpk::tunerSetValue(tuner, tuner.ValueCurrent + 1);
	}
	return 0;
}
		::gpk::error_t												gpk::sliderCreate							(::gpk::SDialog			& dialog)								{
	int32_t																	index										= -1;
	::gpk::ptr_obj<::gpk::SDialogSlider>									slider;
	gpk_necall(index = dialog.Create(slider), "%s", "Out of memory?");
	gpk_necall(slider->IdButton = ::gpk::controlCreateChild(*dialog.GUI, slider->IdGUIControl), "%s", "Out of memory?");
	::gpk::SGUIControlTable													& controlTable								= dialog.GUI->Controls;
	controlTable.Controls	[slider->IdGUIControl].Margin				= {};
	controlTable.Controls	[slider->IdButton].Area.Size				= {16, 16};
	controlTable.Text		[slider->IdButton].Text						= "-";
	controlTable.Constraints[slider->IdButton].AttachSizeToControl.y	= slider->IdButton;
	controlTable.Modes		[slider->IdButton].UseNewPalettes			= true;
	controlTable.Modes		[slider->IdButton].ColorMode				= ::gpk::GUI_COLOR_MODE_3D;
	::gpk::memcpy_s(controlTable.Controls[slider->IdButton		].Palettes.Storage, dialog.ColorsButton		.Storage);
	::gpk::memcpy_s(controlTable.Controls[slider->IdGUIControl	].Palettes.Storage, dialog.ColorsCheckBox	.Storage);
	return index;
}
		::gpk::error_t												gpk::sliderSetValue							(::gpk::SDialogSlider	& slider, int64_t value)				{
	::gpk::SDialog															& dialog									= *slider.Dialog;
	::gpk::SGUIControlTable													& controlTable								= dialog.GUI->Controls;
	slider.ValueCurrent													= ::gpk::max(::gpk::min(value, slider.ValueLimits.Max), slider.ValueLimits.Min);
	const double															proportion									= (slider.ValueCurrent - slider.ValueLimits.Min) * (1.0 / (slider.ValueLimits.Max - slider.ValueLimits.Min));

	::gpk::SControl															& controlButton								= controlTable.Controls[slider.IdButton];
	::gpk::SControl															& controlSlider								= controlTable.Controls[slider.IdGUIControl];
	::gpk::SCoord2<double>													ncSpacing									= ::gpk::controlNCSpacing(controlSlider).Cast<double>();
	if(slider.Vertical)
		controlButton.Area.Offset.y											= (int16_t)int32_t((controlSlider.Area.Size.y - 16 - ncSpacing.y) * proportion);
	else
		controlButton.Area.Offset.x											= (int16_t)int32_t((controlSlider.Area.Size.x - 16 - ncSpacing.x) * proportion);

	::gpk::SControlConstraints												& buttonConstraints							= controlTable.Constraints[slider.IdButton];
	if(slider.Vertical) {
		buttonConstraints.AttachSizeToControl.y								= -1;
		buttonConstraints.AttachSizeToControl.x								= slider.IdButton;
	}
	else {
		buttonConstraints.AttachSizeToControl.x								= -1;
		buttonConstraints.AttachSizeToControl.y								= slider.IdButton;
	}
	::gpk::controlMetricsInvalidate(*dialog.GUI, slider.IdGUIControl);
	return 0;
}
		::gpk::error_t												gpk::sliderUpdate							(::gpk::SDialogSlider	& slider)								{
	::gpk::SDialog															& dialog									= *slider.Dialog;
	::gpk::SGUIControlTable													& controlTable								= dialog.GUI->Controls;
	const int64_t															valueRange									= slider.ValueLimits.Max - slider.ValueLimits.Min;
	if(controlTable.States[slider.IdButton].Pressed) {
		if(dialog.Input->MouseCurrent.Position != dialog.Input->MousePrevious.Position) {
			::gpk::SInput															& input										= *dialog.Input;
			const ::gpk::SGUIZoom													& zoom										= dialog.GUI->Zoom;
			::gpk::SCoord2<double>													scale										=
				{ 1.0 / (zoom.ZoomLevel * zoom.DPI.x)
				, 1.0 / (zoom.ZoomLevel * zoom.DPI.y)
				};
			const ::gpk::SCoord2<int16_t>											& controlSliderSize							= controlTable.Controls[slider.IdGUIControl].Area.Size;

			::gpk::SCoord2<double>													valueDisplacement							=
				{ input.MouseCurrent.Deltas.x * scale.x * (valueRange * (1.0 / controlSliderSize.x))
				, input.MouseCurrent.Deltas.y * scale.y * (valueRange * (1.0 / controlSliderSize.y))
				};

			if (valueDisplacement.y > 0 && valueDisplacement.y <  1) valueDisplacement.y	=  1;
			if (valueDisplacement.y < 0 && valueDisplacement.y > -1) valueDisplacement.y	= -1;
			if (valueDisplacement.x > 0 && valueDisplacement.x <  1) valueDisplacement.x	=  1;
			if (valueDisplacement.x < 0 && valueDisplacement.x > -1) valueDisplacement.x	= -1;

			::gpk::sliderSetValue(slider, slider.ValueCurrent + (int64_t)(slider.Vertical ? valueDisplacement.y : valueDisplacement.x));
		}
	}
	else
		if(dialog.Input->ButtonDown(0) && controlTable.States[slider.IdGUIControl].Pressed) {	//
			::gpk::SInput															& input										= *dialog.Input;
			const int64_t															valuePage									= ::gpk::max((int64_t)1LL, (valueRange > 10) ? (int64_t)(valueRange * .1) : valueRange / 2);
			const ::gpk::SCoord2<int16_t>											& controlButtonPosition						= controlTable.Metrics[slider.IdButton].Total.Global.Offset;
			if(slider.Vertical) {
				const int64_t															finalValue									= slider.ValueCurrent + ((input.MouseCurrent.Position.y > controlButtonPosition.y) ? valuePage : -valuePage);
				::gpk::sliderSetValue(slider, finalValue);
			}
			else {
				const int64_t															finalValue									= slider.ValueCurrent + ((input.MouseCurrent.Position.x > controlButtonPosition.x) ? valuePage : -valuePage);
				::gpk::sliderSetValue(slider, finalValue);
			}
		}
	return 0;
}
		::gpk::error_t												gpk::editBoxCreate							(::gpk::SDialog			& dialog )								{
	int32_t																	index										= -1;
	::gpk::ptr_obj<::gpk::SDialogEditBox>									editBox;
	::gpk::SGUIControlTable													& controlTable								= dialog.GUI->Controls;
	gpk_necall(index = dialog.Create(editBox), "%s", "Out of memory?");
	controlTable.Modes[editBox->IdGUIControl].UseNewPalettes			= true;
	controlTable.Modes[editBox->IdGUIControl].ColorMode					= ::gpk::GUI_COLOR_MODE_FLAT;
	::gpk::memcpy_s(controlTable.Controls[editBox->IdGUIControl].Palettes.Storage, dialog.ColorsCheckBox.Storage);
	return index;
}
		::gpk::error_t												gpk::editBoxUpdate							(::gpk::SDialogEditBox	& control)								{
	::gpk::SDialog															& dialog									= *control.Dialog;
	if(dialog.Input->ButtonDown(0)) {
	}
	return 0;
}
		::gpk::error_t												gpk::viewportCreate							(::gpk::SDialog			& dialog)								{
	int32_t																	index										= -1;
	::gpk::ptr_obj<::gpk::SDialogViewport>									viewport;
	::gpk::SGUIControlTable													& controlTable								= dialog.GUI->Controls;
	gpk_necall(index = dialog.Create(viewport), "%s", "Out of memory?");
	controlTable.Modes[viewport->IdGUIControl].UseNewPalettes			= true;
	controlTable.Modes[viewport->IdGUIControl].FrameOut					= true;
	controlTable.Modes[viewport->IdGUIControl].NoHoverEffect			= true;
	controlTable.Controls[viewport->IdGUIControl].Margin				= {};
	::gpk::memcpy_s(controlTable.Controls[viewport->IdGUIControl].Palettes.Storage, dialog.ColorsViewport.Storage);
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
		::gpk::memcpy_s(control.Palettes.Storage, dialog.ColorsViewport.Storage);
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
		::gpk::memcpy_s(control.Palettes.Storage, dialog.ColorsViewportTitle.Storage);
	}
	viewport->Settings.Unfolded											= true;
	return index;
}

		::gpk::error_t												gpk::viewportAdjustSize					(::gpk::SCoord2<int16_t> & sizeViewport, const ::gpk::SCoord2<int16_t> & sizeClient)		{
	sizeViewport														= sizeClient;
	sizeViewport.x														+= 4;
	sizeViewport.y														+= 4 + 1 + heightOfField;
	return 0;
}

		::gpk::error_t												gpk::viewportFold						(::gpk::SDialogViewport	& control, bool fold)														{
	if((fold && control.Settings.Unfolded) || (false == control.Settings.Unfolded && false == fold)) {
		::gpk::SDialog															& dialog									= *control.Dialog;
		::gpk::SGUIControlTable													& controlTable								= dialog.GUI->Controls;
		::gpk::SControl															& controlMain								= controlTable.Controls[control.IdGUIControl];
		::gpk::SControl															& controlTitle								= controlTable.Controls[control.IdTitle];
		::gpk::SControlState													& stateClient								= controlTable.States[control.IdClient];
		if(fold && control.Settings.Unfolded)
			controlMain.Area.Size.y												= controlTitle.Area.Size.y + ::gpk::controlNCSpacing(controlMain).y;
		else if(false == control.Settings.Unfolded && false == fold) {
			::gpk::SControl															& controlClient								= controlTable.Controls[control.IdClient];
			controlMain.Area.Size.y												= controlClient.Area.Size.y + controlTitle.Area.Size.y + ::gpk::controlNCSpacing(controlMain).y + 1;
		}
		if(-1 != controlMain.IndexParent)
			::gpk::controlMetricsInvalidate(*dialog.GUI, controlMain.IndexParent);
		else
			::gpk::controlMetricsInvalidate(*dialog.GUI, control.IdGUIControl);
		stateClient.Hidden													= fold;
		control.Settings.Unfolded											= !fold;
	}
	return one_if(false == control.Settings.Unfolded);
}

static	::gpk::error_t												viewportDrag								(::gpk::SDialogViewport	& control, ::gpk::SControl & controlMain, ::gpk::SDialog & dialog, ::gpk::SCoord2<bool> locked, ::gpk::SCoord2<int32_t> mouseDeltas)																{
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

		::gpk::error_t												gpk::viewportUpdate							(::gpk::SDialogViewport	& control)																{
	::gpk::SDialog															& dialog									= *control.Dialog;
	::gpk::SGUIControlTable													& controlTable								= dialog.GUI->Controls;
	::gpk::SControl															& controlMain								= controlTable.Controls[control.IdGUIControl];
	::gpk::SControlState													& controlTitle								= controlTable.States[control.IdTitle];
	if(controlTitle.Pressed) {
		::gpk::SCoord2<bool>													locked										= {control.Settings.DisplacementLockX, control.Settings.DisplacementLockY};
		if(false == locked.x || false == locked.y) {
			::gpk::SCoord2<int32_t>													mouseDeltas									= {dialog.Input->MouseCurrent.Deltas.x, dialog.Input->MouseCurrent.Deltas.y};
			if(mouseDeltas.x || mouseDeltas.y)
				::viewportDrag(control, controlMain, dialog, locked, mouseDeltas);
		}
	}
	else {
		if(controlTitle.Execute && false == control.Settings.Dragging)
			::gpk::viewportFold(control, control.Settings.Unfolded);
		control.Settings.Dragging											= false;
	}
	if(control.Settings.Unfolded != control.SettingsOld.Unfolded)
		if(true == control.Settings.Unfolded) {
			::gpk::SCoord2<int16_t>													clientFinalSize								= {controlMain.Area.Size};
			clientFinalSize														-= ::gpk::controlNCSpacing(controlMain);
			clientFinalSize.y													-=  heightOfField + 1;
			::gpk::SControl															& controlClient								= controlTable.Controls[control.IdClient];
			controlClient.Area.Size												= clientFinalSize;
		}
	control.SettingsOld													= control.Settings;
	return 0;
}
