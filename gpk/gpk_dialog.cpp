#include "gpk_dialog.h"

												::gpk::IDialogControl::~IDialogControl		()									{ ::gpk::controlDelete(Dialog->GUI, IdGUIControl, false); }

::gpk::error_t									gpk::sliderCreate							(::gpk::SDialog & dialog)			{
	int32_t												index										= -1;
	::gpk::ptr_obj<::gpk::SDialogSlider>				slider;
	gpk_necall(index = dialog.Create(slider), "%s", "Out of memory?");
	gpk_necall(slider->IdButton = ::gpk::controlCreateChild(dialog.GUI, slider->IdGUIControl), "%s", "Out of memory?");
	return index;
}

::gpk::error_t									gpk::sliderSetValue							(::gpk::SDialogSlider	& slider, int32_t value)	{
	slider.ValueCur									= ::gpk::max(::gpk::min(value, slider.ValueMax), slider.ValueMin);
	const double										proportion							= (slider.ValueCur - slider.ValueMin) * (1.0 / (slider.ValueMax - slider.ValueMin)); 

	::gpk::SControl										& controlButton						= slider.Dialog->GUI.Controls.Controls[slider.IdButton];
	::gpk::SControl										& controlSlider						= slider.Dialog->GUI.Controls.Controls[slider.IdGUIControl];
	::gpk::SCoord2<double>								ncSpacing							= ::gpk::controlNCSpacing(controlSlider).Cast<double>();
	if(slider.Vertical) 
		controlButton.Area.Offset.y						= (int16_t)int32_t((controlSlider.Area.Size.y - 16 - ncSpacing.y) * proportion); 
	else
		controlButton.Area.Offset.x						= (int16_t)int32_t((controlSlider.Area.Size.x - 16 - ncSpacing.x) * proportion); 
	return 0;
}

