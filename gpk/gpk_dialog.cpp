#include "gpk_dialog.h"

												::gpk::IDialogControl::~IDialogControl		()									{ if(-1 != IdGUIControl) ::gpk::controlDelete(Dialog->GUI, IdGUIControl, true); }

::gpk::error_t									gpk::checkBoxCreate							(::gpk::SDialog & dialog)			{
	int32_t												index										= -1;
	::gpk::ptr_obj<::gpk::SDialogCheckBox>				checkBox;
	gpk_necall(index = dialog.Create(checkBox), "%s", "Out of memory?");
	dialog.GUI.Controls.Controls[checkBox->IdGUIControl].Border		= {1, 1, 1, 1};
	dialog.GUI.Controls.Controls[checkBox->IdGUIControl].Margin		= {};
	dialog.GUI.Controls.Controls[checkBox->IdGUIControl].Area.Size	= {16, 16};
	dialog.GUI.Controls.Modes[checkBox->IdGUIControl].UseNewPalettes	= true;
	dialog.GUI.Controls.Modes[checkBox->IdGUIControl].ColorMode			= ::gpk::GUI_COLOR_MODE_THEME;
	::gpk::memcpy_s(dialog.GUI.Controls.Controls[checkBox->IdGUIControl].Palettes, dialog.ColorsButton);
	return index;
}

::gpk::error_t									gpk::checkBoxUpdate							(::gpk::SDialogCheckBox & checkbox)		{
	if(checkbox.Dialog->GUI.Controls.States[checkbox.IdGUIControl].Execute) {
		checkbox.Checked								= !checkbox.Checked;
		if(false == checkbox.Checked) 
			checkbox.Dialog->GUI.Controls.Controls[checkbox.IdGUIControl].Image = {};
	}
	return 0;
}

::gpk::error_t									gpk::tunerCreate							(::gpk::SDialog & dialog)			{
	int32_t												index										= -1;
	::gpk::ptr_obj<::gpk::SDialogTuner>			tuner;
	gpk_necall(index = dialog.Create(tuner), "%s", "Out of memory?");
	tuner->ValueCurrent								= tuner->ValueLimits.Min;
	gpk_necall(tuner->IdDecrease = ::gpk::controlCreateChild(dialog.GUI, tuner->IdGUIControl), "%s", "Out of memory?");
	gpk_necall(tuner->IdIncrease = ::gpk::controlCreateChild(dialog.GUI, tuner->IdGUIControl), "%s", "Out of memory?");
	dialog.GUI.Controls.Controls[tuner->IdGUIControl].Margin		= {};
	for(int32_t iControl = tuner->IdDecrease; iControl < tuner->IdIncrease + 1; ++iControl) {
		dialog.GUI.Controls.Controls[iControl].Align					= (iControl == tuner->IdDecrease) ? ::gpk::ALIGN_CENTER_LEFT : ::gpk::ALIGN_CENTER_RIGHT;
		dialog.GUI.Controls.Controls[iControl].Area.Size				= {16, 16};
		dialog.GUI.Controls.Modes	[iControl].UseNewPalettes			= true;
		dialog.GUI.Controls.Modes	[iControl].ColorMode				= ::gpk::GUI_COLOR_MODE_3D;
		::gpk::memcpy_s(dialog.GUI.Controls.Controls[iControl].Palettes, dialog.ColorsButton);
		dialog.GUI.Controls.Constraints[iControl].AttachSizeToControl.y	= iControl;
	}
	::gpk::SControlText									& tunerText				= dialog.GUI.Controls.Text[tuner->IdGUIControl];
	tunerText.Text									= {tuner->ValueString, (uint32_t)sprintf_s(tuner->ValueString, "%lli", tuner->ValueCurrent)};
	tunerText.Align									= ::gpk::ALIGN_CENTER;
	return index;
}

::gpk::error_t									gpk::tunerUpdate							(::gpk::SDialogTuner & tuner)			{
	if(tuner.Dialog->GUI.Controls.States[tuner.IdDecrease].Execute || tuner.Dialog->GUI.Controls.States[tuner.IdIncrease].Execute) {
			 if(tuner.Dialog->GUI.Controls.States[tuner.IdDecrease].Execute)
			--tuner.ValueCurrent;
		else if(tuner.Dialog->GUI.Controls.States[tuner.IdIncrease].Execute)
			++tuner.ValueCurrent;
		if (tuner.ValueCurrent < tuner.ValueLimits.Min)
			tuner.ValueCurrent = tuner.ValueLimits.Min;
		if (tuner.ValueCurrent > tuner.ValueLimits.Max)
			tuner.ValueCurrent = tuner.ValueLimits.Max;
		::gpk::SControlText									& tunerText									= tuner.Dialog->GUI.Controls.Text[tuner.IdGUIControl];
		tunerText.Text									= {tuner.ValueString, (uint32_t)sprintf_s(tuner.ValueString, "%lli", tuner.ValueCurrent)};
		::gpk::controlMetricsInvalidate(tuner.Dialog->GUI, tuner.IdGUIControl);
	}
	return 0;
}

::gpk::error_t									gpk::sliderCreate							(::gpk::SDialog & dialog)			{
	int32_t												index										= -1;
	::gpk::ptr_obj<::gpk::SDialogSlider>				slider;
	gpk_necall(index = dialog.Create(slider), "%s", "Out of memory?");
	gpk_necall(slider->IdButton = ::gpk::controlCreateChild(dialog.GUI, slider->IdGUIControl), "%s", "Out of memory?");
	dialog.GUI.Controls.Controls[slider->IdGUIControl].Margin	= {};
	dialog.GUI.Controls.Controls[slider->IdButton].Area.Size	= {16, 16};
	dialog.GUI.Controls.Constraints[slider->IdButton].AttachSizeToControl.y	= slider->IdButton;
	dialog.GUI.Controls.Modes[slider->IdButton].UseNewPalettes	= true;
	dialog.GUI.Controls.Modes[slider->IdButton].ColorMode		= ::gpk::GUI_COLOR_MODE_3D;
	::gpk::memcpy_s(dialog.GUI.Controls.Controls[slider->IdButton].Palettes, dialog.ColorsButton);
	return index;
}

::gpk::error_t									gpk::sliderSetValue							(::gpk::SDialogSlider	& slider, int64_t value)	{
	slider.ValueCurrent								= ::gpk::max(::gpk::min(value, slider.ValueLimits.Max), slider.ValueLimits.Min);
	const double										proportion									= (slider.ValueCurrent - slider.ValueLimits.Min) * (1.0 / (slider.ValueLimits.Max - slider.ValueLimits.Min)); 

	::gpk::SControl										& controlButton								= slider.Dialog->GUI.Controls.Controls[slider.IdButton];
	::gpk::SControl										& controlSlider								= slider.Dialog->GUI.Controls.Controls[slider.IdGUIControl];
	::gpk::SCoord2<double>								ncSpacing									= ::gpk::controlNCSpacing(controlSlider).Cast<double>();
	if(slider.Vertical) 
		controlButton.Area.Offset.y						= (int16_t)int32_t((controlSlider.Area.Size.y - 16 - ncSpacing.y) * proportion); 
	else
		controlButton.Area.Offset.x						= (int16_t)int32_t((controlSlider.Area.Size.x - 16 - ncSpacing.x) * proportion); 

	::gpk::SControlConstraints							& buttonConstraints							= slider.Dialog->GUI.Controls.Constraints[slider.IdButton];
	if(slider.Vertical) 
		buttonConstraints.AttachSizeToControl.x			= slider.IdButton;
	else
		buttonConstraints.AttachSizeToControl.y			= slider.IdButton;
	::gpk::controlMetricsInvalidate(slider.Dialog->GUI, slider.IdGUIControl);
	return 0;
}

::gpk::error_t									gpk::sliderUpdate							(::gpk::SDialogSlider	& slider)	{
	const int64_t										valueRange									= slider.ValueLimits.Max - slider.ValueLimits.Min;
	if(slider.Dialog->GUI.Controls.States[slider.IdButton].Pressed) {
		if(slider.Dialog->Input->MouseCurrent.Position != slider.Dialog->Input->MousePrevious.Position) {
			::gpk::SInput										& input										= *slider.Dialog->Input;
			const ::gpk::SGUIZoom								& zoom										= slider.Dialog->GUI.Zoom;
			::gpk::SCoord2<double>								scale										= 
				{ 1.0 / (zoom.ZoomLevel * zoom.DPI.x)
				, 1.0 / (zoom.ZoomLevel * zoom.DPI.y)
				};
			const ::gpk::SCoord2<int16_t>						& controlSliderSize							= slider.Dialog->GUI.Controls.Controls[slider.IdGUIControl].Area.Size;

			::gpk::SCoord2<double>								valueDisplacement							= 
				{ input.MouseCurrent.Deltas.x * scale.x * (valueRange * (1.0 / controlSliderSize.x))
				, input.MouseCurrent.Deltas.y * scale.y * (valueRange * (1.0 / controlSliderSize.y))
				};

			if (valueDisplacement.y > 0 && valueDisplacement.y <  1) valueDisplacement.y =  1;
			if (valueDisplacement.y < 0 && valueDisplacement.y > -1) valueDisplacement.y = -1;
			if (valueDisplacement.x > 0 && valueDisplacement.x <  1) valueDisplacement.x =  1;
			if (valueDisplacement.x < 0 && valueDisplacement.x > -1) valueDisplacement.x = -1;

			::gpk::sliderSetValue(slider, slider.ValueCurrent + (int64_t)(slider.Vertical ? valueDisplacement.y : valueDisplacement.x));
		}
	}
	else
		if(slider.Dialog->Input->ButtonDown(0) && slider.Dialog->GUI.Controls.States[slider.IdGUIControl].Pressed) {	// 
			::gpk::SInput										& input										= *slider.Dialog->Input;
			const int64_t										valuePage									= ::gpk::max(1LL, (valueRange > 10) ? (int64_t)(valueRange * .1) : valueRange / 2);
			const ::gpk::SCoord2<int32_t>						& controlButtonPosition						= slider.Dialog->GUI.Controls.Metrics[slider.IdButton].Total.Global.Offset;
			if(slider.Vertical) {
				const int64_t										finalValue									= slider.ValueCurrent + ((input.MouseCurrent.Position.y > controlButtonPosition.y) ? valuePage : -valuePage);
				::gpk::sliderSetValue(slider, finalValue);
			}
			else {
				const int64_t										finalValue									= slider.ValueCurrent + ((input.MouseCurrent.Position.x > controlButtonPosition.x) ? valuePage : -valuePage);
				::gpk::sliderSetValue(slider, finalValue);
			}
		}
	return 0;
}

