#include "gpk_gui.h"
#include "gpk_png.h"

#ifndef GPK_GUI_WIDGETS_H_23627
#define GPK_GUI_WIDGETS_H_23627

namespace gpk {
	struct SCheckBox {
		cid_t IdControl = -1;
		bool Checked = false;
		::gpk::img<::gpk::bgra> ImageCrossBGRA = {};
		::gpk::imgmonou3_t ImageCross = {};
	};

	struct SSlider {
		typedef ::gpk::function<gpk::err_t(::gpk::vcsc_t & format, int64_t value, const ::gpk::minmax<int64_t> & limits)> TCallback;
		cid_t IdControl = -1;
		cid_t IdButton = -1;
		bool Vertical = false;
		::gpk::minmax<int64_t> ValueLimits = {0, 100};
		int64_t ValueCurrent = -1;
		sc_t ValueString[64] = {};
		TCallback FuncValueFormat = [](::gpk::vcsc_t & format, int64_t, const ::gpk::minmax<int64_t> &) mutable { format = ::gpk::vcst_t("%lli"); return 0; };
		TCallback FuncGetString = [this](::gpk::vcsc_t & inouts, int64_t value, const ::gpk::minmax<int64_t> &) {
			inouts = {ValueString, (uint32_t)snprintf(ValueString, ::gpk::size(ValueString) - 2, inouts.begin(), value)};
			return 0;
		};
	};

	tplt<tpnm _tValue>
	struct STuner {
		typedef ::gpk::function<gpk::err_t(::gpk::vcsc_t & format, _tValue value, const ::gpk::minmax<_tValue> & limits)> TCallback;
		cid_t IdControl = -1;
		cid_t IdDecrease = -1;
		cid_t IdIncrease = -1;
		::gpk::minmax<_tValue> ValueLimits = {0, 0x7F};
		_tValue ValueCurrent = 0;
		sc_t ValueString[64] = {};
		TCallback FuncValueFormat = [this](::gpk::vcsc_t & format, _tValue, const ::gpk::minmax<_tValue> &) mutable { format = ::gpk::vcst_t("%lli"); return 0; };
		TCallback FuncGetString = [this](::gpk::vcsc_t & inouts, _tValue value, const ::gpk::minmax<_tValue> &) {
			inouts = {ValueString, (uint32_t)snprintf(ValueString, ::gpk::size(ValueString) - 2, inouts.begin(), value)};
			return 0;
		};

		gpk::err_t SetValue(::gpk::SGUI & gui, _tValue value) {
			if(value < ValueLimits.Min)
				value = ValueLimits.Min;
			else if(value > ValueLimits.Max)
				value = ValueLimits.Max;
			ValueCurrent = value;
			::gpk::vcsc_t valueString;
			FuncValueFormat(valueString, value, ValueLimits);
			FuncGetString(valueString, ValueCurrent, ValueLimits);
			if_fail_fe(gpk::controlTextSet(gui.Controls, IdControl, valueString));
			if_fail_fe(gpk::controlMetricsInvalidate(gui, IdControl));
			return 0;
		}

		gpk::err_t Update(::gpk::SGUI & gui, ::gpk::view<const ::gpk::pobj<::gpk::SEventSystem>> eventsIn) {
			::gpk::SControlTable & controlTable = gui.Controls;
			for(uint32_t iEvent = 0; iEvent < eventsIn.size(); ++iEvent) {
				if(eventsIn[iEvent]->Type == ::gpk::SYSTEM_EVENT_GUI) {
					::gpk::eventExtractAndHandle<::gpk::EVENT_GUI_CONTROL>(*eventsIn[iEvent], [this, &gui, &controlTable](const ::gpk::SEventView<::gpk::EVENT_GUI_CONTROL> & eventGui) {
						switch(eventGui.Type) {
						default: break;
						case ::gpk::EVENT_GUI_CONTROL_StateChange: {
							const ::gpk::SChangeControlState stateChange = *(const ::gpk::SChangeControlState*)eventGui.Data.begin();
							if((stateChange.Set & ::gpk::GUI_CONTROL_FLAG_Action) && stateChange.Id == IdDecrease)
								return SetValue(gui, ValueCurrent - 1);
							else if((stateChange.Set & ::gpk::GUI_CONTROL_FLAG_Action) && stateChange.Id == IdIncrease)
								return SetValue(gui, ValueCurrent + 1);
							break;
						}
						}
						return 0;
					});
				}
			}
			return 0;
		}
	};

	tplt<tpnm _tValue>
	::gpk::cid_t tunerCreate(::gpk::SGUI & gui, ::gpk::STuner<_tValue> & tuner) {
		if_fail_fe(tuner.IdControl = ::gpk::controlCreate(gui));
		gui.Controls.Draw[tuner.IdControl].UseNewPalettes = false;
		if_fail_fe(tuner.IdDecrease = ::gpk::controlCreateChild(gui, tuner.IdControl));
		if_fail_fe(tuner.IdIncrease = ::gpk::controlCreateChild(gui, tuner.IdControl));
		::gpk::SControlTable & controlTable = gui.Controls;
		controlTable.Placement[tuner.IdControl].Margin = {};
		for(int32_t iControl = tuner.IdDecrease; iControl < tuner.IdIncrease + 1; ++iControl) {
			::gpk::SControlPlacement & controlButton = controlTable.Placement[iControl];
			controlButton.Align = (iControl == tuner.IdDecrease) ? ::gpk::ALIGN_CENTER_LEFT : ::gpk::ALIGN_CENTER_RIGHT;
			controlTable.Text[iControl].Text = (iControl == tuner.IdDecrease) ? "-" : "+";
			controlTable.Draw[iControl].UseNewPalettes = true;
			controlTable.Draw[iControl].ColorMode = ::gpk::GUI_COLOR_MODE_3D;
			controlTable.Constraints[iControl].AttachSizeToControl.y = (::gpk::cid_t)iControl;
		}
		if_fail_fe(tuner.SetValue(gui, tuner.ValueCurrent));
		controlTable.Text[tuner.IdControl].Align = ::gpk::ALIGN_CENTER;
		return tuner.IdControl;
	}

	::gpk::cid_t sliderCreate(::gpk::SGUI & gui, ::gpk::SSlider & slider);
	gpk::err_t sliderSetValue(::gpk::SGUI & gui, ::gpk::SSlider & slider, int64_t value);
	gpk::err_t sliderUpdate(::gpk::SGUI & gui, ::gpk::SInput & input, ::gpk::SSlider & slider);
	::gpk::cid_t checkBoxCreate(::gpk::SGUI & gui, ::gpk::SCheckBox & checkbox);
	gpk::err_t checkBoxUpdate(::gpk::SGUI & gui, ::gpk::SCheckBox & checkbox);
}

#endif
