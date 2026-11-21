#include "gpk_gui_widgets.h"

gpk::cid_t gpk::checkBoxCreate(SGUI & gui, SCheckBox & checkbox) {
	if_fail_fe(checkbox.IdControl = controlCreate(gui));
	gui.Controls.Draw[checkbox.IdControl].UseNewPalettes = false;
	return checkbox.IdControl;
}

gpk::err_t gpk::checkBoxUpdate(SGUI & gui, SCheckBox & checkbox) {
	SControlTable & controlTable = gui.Controls;
	img<bgra> & imageCross = checkbox.ImageCrossBGRA;
	if(controlTable.IsAction(checkbox.IdControl)) {
		if(checkbox.ImageCross.Texels.size() < 4) {
#if !(defined(GPK_ESP32) || defined(GPK_ARDUINO))
			if_fail_e(pngLoadFile("../gpk_data/images/cross.png", imageCross));
#endif
			checkbox.ImageCross.resize(imageCross.metrics());
			for(uint32_t iTexel = 0; iTexel < imageCross.Texels.size(); ++iTexel) {
				if(imageCross.Texels[iTexel] == (bgra)BLACK)
					checkbox.ImageCross.View[iTexel] = true;
				else {
					imageCross.Texels[iTexel].a = 0;
					checkbox.ImageCross.View[iTexel] = false;
				}
			}
		}
		checkbox.Checked = !checkbox.Checked;
		controlTable.Views[checkbox.IdControl] = (false == checkbox.Checked) ? g8bgra{} : imageCross.View;
	}
	return 0;
}

gpk::cid_t gpk::sliderCreate(SGUI & gui, SSlider & slider) {
	if_fail_fe(slider.IdControl = controlCreate(gui));
	gui.Controls.Draw[slider.IdControl].UseNewPalettes = false;
	if_fail_fe(slider.IdButton = controlCreateChild(gui, slider.IdControl));
	SControlTable & controlTable = gui.Controls;
	controlTable.Placement[slider.IdControl].Margin = {};
	controlTable.Placement[slider.IdButton].Area.Size = {16, 16};
	controlTable.Text[slider.IdButton].Text = "-";
	controlTable.Constraints[slider.IdButton].AttachSizeToControl.y = slider.IdButton;
	controlTable.Draw[slider.IdButton].UseNewPalettes = true;
	controlTable.Draw[slider.IdButton].ColorMode = GUI_COLOR_MODE_3D;
	SControlText & tunerText = controlTable.Text[slider.IdControl];
	tunerText.Text = {slider.ValueString, (uint32_t)snprintf(slider.ValueString, size(slider.ValueString) - 2, "%lli", (long long int)slider.ValueCurrent)};
	return slider.IdControl;
}

gpk::err_t gpk::sliderSetValue(SGUI & gui, SSlider & slider, int64_t value) {
	SControlTable & controlTable = gui.Controls;
	const int64_t newValue = max(min(value, slider.ValueLimits.Max), slider.ValueLimits.Min);
	bool updateMetrics = slider.ValueCurrent != newValue;
	slider.ValueCurrent = newValue;
	const double proportion = (slider.ValueCurrent - slider.ValueLimits.Min) * (1.0 / (slider.ValueLimits.Max - slider.ValueLimits.Min));
	SControlPlacement & controlButton = controlTable.Placement[slider.IdButton];
	SControlPlacement & controlSlider = controlTable.Placement[slider.IdControl];
	n2f3_t ncSpacing = controlNCSpacing(controlSlider).f3();
	if(slider.Vertical)
		controlButton.Area.Offset.y = int16_t((controlSlider.Area.Size.y - controlButton.Area.Size.y - ncSpacing.y) * proportion);
	else
		controlButton.Area.Offset.x = int16_t((controlSlider.Area.Size.x - controlButton.Area.Size.x - ncSpacing.x) * proportion);
	SControlConstraints & buttonConstraints = controlTable.Constraints[slider.IdButton];
	if(slider.Vertical) {
		buttonConstraints.AttachSizeToControl.y = -1;
		buttonConstraints.AttachSizeToControl.x = slider.IdButton;
	}
	else {
		buttonConstraints.AttachSizeToControl.x = -1;
		buttonConstraints.AttachSizeToControl.y = slider.IdButton;
	}
	vcst_t valueString;
	if_fail_fe(slider.FuncValueFormat(valueString, slider.ValueCurrent, slider.ValueLimits));
	if_fail_fe(slider.FuncGetString(valueString, slider.ValueCurrent, slider.ValueLimits));
	if_fail_fe(controlTextSet(controlTable, slider.IdControl, valueString));
	if(updateMetrics)
		if_fail_fe(controlMetricsInvalidate(controlTable, slider.IdControl));
	return 0;
}

gpk::err_t gpk::sliderUpdate(SGUI & gui, SInput & input, SSlider & slider) {
	SControlTable & controlTable = gui.Controls;
	const int64_t valueRange = slider.ValueLimits.Max - slider.ValueLimits.Min;
	if(controlTable.States[slider.IdButton].IsPressed()) {
		if(input.MouseCurrent.Position != input.MousePrevious.Position) {
			const n2f3_t controlSliderPos = controlTable.Area[slider.IdControl].Client.Global.Offset.f3();
			const n2f3_t controlSliderSize = controlTable.Area[slider.IdControl].Client.Global.Size.f3();
			n2f3_t effectiveSize = controlSliderSize - (controlTable.Placement[slider.IdButton].Area.Size).f3();
			n2f3_t currentValue = input.MouseCurrent.Position.f3();
			const n2f3_t valueUnit = {1.0 / effectiveSize.x, 1.0 / effectiveSize.y};
			currentValue -= controlSliderPos + controlTable.Placement[slider.IdButton].Area.Size.f3() / 2.0;
			currentValue.x *= valueUnit.x;
			currentValue.y *= valueUnit.y;
			if_fail_fe(sliderSetValue(gui, slider, (int64_t)((slider.Vertical ? currentValue.y : currentValue.x) * valueRange + slider.ValueLimits.Min)));
		}
	}
	else if(input.ButtonDown(0) && controlTable.States[slider.IdControl].IsPressed()) {
		const int64_t valuePage = max((int64_t)1LL, (valueRange > 10) ? (int64_t)(valueRange * .1) : valueRange / 2);
		const n2<int16_t> & controlButtonPosition = controlTable.Area[slider.IdButton].Total.Global.Offset;
		if(slider.Vertical) {
			const int64_t finalValue = slider.ValueCurrent + ((input.MouseCurrent.Position.y > controlButtonPosition.y) ? valuePage : -valuePage);
			if_fail_fe(sliderSetValue(gui, slider, finalValue));
		}
		else {
			const int64_t finalValue = slider.ValueCurrent + ((input.MouseCurrent.Position.x > controlButtonPosition.x) ? valuePage : -valuePage);
			if_fail_fe(sliderSetValue(gui, slider, finalValue));
		}
	}
	return 0;
}
