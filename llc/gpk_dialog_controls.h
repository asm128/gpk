#include "gpk_dialog.h"

#ifndef GPK_DIALOG_CONTROLS_H_23627
#define GPK_DIALOG_CONTROLS_H_23627

namespace gpk
{
	struct SDialogSlider;
	cid_t			sliderCreate		(SDialog			& dialog);
	err_t			sliderSetValue		(SDialogSlider		& control, s3_t value);
	err_t			sliderUpdate		(SDialogSlider		& control);

	struct SDialogCheckBox;
	cid_t			checkBoxCreate		(SDialog			& dialog);
	err_t			checkBoxUpdate		(SDialogCheckBox	& control);

	struct SDialogEditBox;
	cid_t			editBoxCreate		(SDialog			& dialog);
	err_t			editBoxUpdate		(SDialogEditBox		& control);

	struct SDialogViewport;
	cid_t			viewportCreate		(SDialog			& dialog);
	err_t			viewportUpdate		(SDialogViewport	& control);
	err_t			viewportFold		(SDialogViewport	& control, bool fold);
	err_t			viewportAdjustSize	(n2<s1_t>			& sizeViewport, const n2<s1_t> & sizeClient);

	stin	cid_t	sliderCreate		(SDialog			& dialog, pnco<SDialogSlider	>	& createdControl)	 { cid_t index = -1; if_fail_fe(index = sliderCreate	(dialog)); dialog.Controls[index].as(createdControl); return index; }
	stin	cid_t	checkBoxCreate		(SDialog			& dialog, pnco<SDialogCheckBox	>	& createdControl)	 { cid_t index = -1; if_fail_fe(index = checkBoxCreate	(dialog)); dialog.Controls[index].as(createdControl); return index; }
	stin	cid_t	editBoxCreate		(SDialog			& dialog, pnco<SDialogEditBox	>	& createdControl)	 { cid_t index = -1; if_fail_fe(index = editBoxCreate	(dialog)); dialog.Controls[index].as(createdControl); return index; }
	stin	cid_t	viewportCreate		(SDialog			& dialog, pnco<SDialogViewport	>	& createdControl)	 { cid_t index = -1; if_fail_fe(index = viewportCreate	(dialog)); dialog.Controls[index].as(createdControl); return index; }

#pragma pack(push, 1)
	struct SDialogViewportSettings {
		bool					Dragging			: 1;
		bool					Unfolded			: 1;
		bool					DisplacementLockX	: 1;
		bool					DisplacementLockY	: 1;
		bool					ScrollBarX			: 1;
		bool					ScrollBarY			: 1;
	};

	struct SDialogViewport : public IDialogControl {
		cid_t					IdClose				= -1;
		cid_t					IdMinimize			= -1;
		cid_t					IdMaximize			= -1;
		cid_t					IdScrollBarX		= -1;
		cid_t					IdScrollBarY		= -1;
		cid_t					IdTitle				= -1;
		cid_t					IdClient			= -1;
		SDialogViewportSettings	Settings			= {};
		SDialogViewportSettings	SettingsOld			= {};

		virtual	err_t	Update				(view<const pobj<SEventSystem>> )	{ return viewportUpdate(*this); }
	};
#pragma pack(pop)

	struct SDialogEditBox : public IDialogControl {
		minmax<u2_t>	Selection			= {};
		string			String				= {};

		virtual	err_t	Update				(view<const pobj<SEventSystem>> )	{ return editBoxUpdate(*this); }
	};

	struct SDialogCheckBox : public IDialogControl {
		bool					Checked				= false;

		virtual	err_t	Update				(view<const pobj<SEventSystem>> )	{ return checkBoxUpdate(*this); }
	};

	struct SDialogSlider : public IDialogControl {
		typedef	function<err_t(vcst_t & format, s3_t value, const minmax<s3_t> & limits)>	
						TCallback;

		cid_t			IdButton			= (cid_t)-1;
		bool			Vertical			= false;
		minmax<s3_t>	ValueLimits			= {0, 100};// 0x7fFFffFF};
		s3_t			ValueCurrent		= -1;
		astsc_t<64>		ValueString			= {};

		TCallback		FuncValueFormat		= []	(vcst_t & format, s3_t, const minmax<s3_t> &)		mutable	{ format = "%lli"; return 0; };
		TCallback		FuncGetString		= [this](vcst_t & inouts, s3_t value, const minmax<s3_t> &)			{ 
			inouts	= {ValueString.Storage, (u2_t)sprintf_s(ValueString.Storage, inouts, value)}; 
			return 0; 
		};

		virtual	err_t	Update				(view<const pobj<SEventSystem>> )	{ return sliderUpdate(*this); }
	};

	tplt<tpnm _tValue>
	struct SDialogTuner : public IDialogControl {
		typedef	function<err_t(vcst_t & format, _tValue value, const minmax<_tValue> & limits)>	
								TCallback;

		cid_t					IdDecrease			= -1;
		cid_t					IdIncrease			= -1;

		minmax<_tValue>	ValueLimits			= {0, 0x7F};
		_tValue					ValueCurrent		= 0;
		astsc_t<64>				ValueString			= {};

		TCallback				FuncValueFormat	= [this](vcst_t & format, _tValue/*value*/, const minmax<_tValue> &)	mutable	{ format = "%lli"; return 0; };
		TCallback				FuncGetString	= [this](vcst_t & inouts, _tValue value, const minmax<_tValue> &)				{ 
			inouts	= {ValueString.Storage, (u2_t)sprintf_s(ValueString.Storage, inouts, value)}; 
			return 0; 
		};

		virtual	err_t	Update				(view<const pobj<SEventSystem>> eventsIn) { 
			SDialog				& dialog			= *Dialog;
			SControlTable		& controlTable		= dialog.GUI->Controls;
			for(u2_t iEvent = 0; iEvent < eventsIn.size(); ++iEvent) {
				if(eventsIn[iEvent]->Type == SYSTEM_EVENT_GUI) {
					eventExtractAndHandle<EVENT_GUI_CONTROL>(*eventsIn[iEvent], [this, &controlTable](const SEventView<EVENT_GUI_CONTROL> & eventGui) {
						switch(eventGui.Type) {
						default: break;
						case EVENT_GUI_CONTROL_StateChange: {
							const SChangeControlState	stateChange		= *(const SChangeControlState*)eventGui.Data.begin();
							if((stateChange.Set & GUI_CONTROL_FLAG_Action) && stateChange.Id == IdDecrease) 
								 return SetValue(ValueCurrent - 1);
							else if((stateChange.Set & GUI_CONTROL_FLAG_Action) && stateChange.Id == IdIncrease) 
								return SetValue(ValueCurrent + 1);
							break;
						}
						}
						return 0;
					});
				}
			}

			return 0;
		}

		err_t			SetValue			(_tValue value)	{
			if(value < ValueLimits.Min)
				value					= ValueLimits.Min;
			else if(value > ValueLimits.Max)
				value					= ValueLimits.Max;
			ValueCurrent			= value;

			view<sc_t>				valueString;
			FuncValueFormat	(valueString, value, ValueLimits);
			FuncGetString	(valueString, ValueCurrent, ValueLimits);
			if_fail_fe(gpk::controlTextSet(*Dialog->GUI, IdGUIControl, valueString));
			if_fail_fe(gpk::controlMetricsInvalidate(*Dialog->GUI, IdGUIControl));
			return 0;
		}
	};

	tplt<tpnm _tValue>
	cid_t			tunerCreate							(SDialog	& dialog)								{
		cid_t				index								= -1;
		pobj<SDialogTuner<_tValue>>	tuner;
		if_fail_fe(index = dialog.Create(tuner));
		tuner->ValueCurrent		= 0; //tuner->ValueLimits.Min;
		if_fail_fe(tuner->IdDecrease = controlCreateChild(*dialog.GUI, tuner->IdGUIControl));
		if_fail_fe(tuner->IdIncrease = controlCreateChild(*dialog.GUI, tuner->IdGUIControl));
		SControlTable		& controlTable						= dialog.GUI->Controls;
		controlTable.Placement[tuner->IdGUIControl].Margin	= {};
		for(s2_t iControl = tuner->IdDecrease; iControl < tuner->IdIncrease + 1; ++iControl) {
			SControlPlacement	& controlButton						= controlTable.Placement[iControl];
			controlButton.Align		= (iControl == tuner->IdDecrease) ? ALIGN_CENTER_LEFT : ALIGN_CENTER_RIGHT;
			memcpy_s(controlTable.Draw[iControl].Palettes.Storage, dialog.Colors->Button.Storage);
			controlTable.Text			[iControl].Text						= (iControl == tuner->IdDecrease) ? "-" : "+";
			controlTable.Draw			[iControl].UseNewPalettes			= true;
			controlTable.Draw			[iControl].ColorMode				= GUI_COLOR_MODE_3D;
			controlTable.Constraints	[iControl].AttachSizeToControl.y	= (cid_t)iControl;
		}
		tuner->SetValue(tuner->ValueCurrent);
		controlTable.Text[tuner->IdGUIControl].Align		= ALIGN_CENTER;
		return index;
	}

	tplt<tpnm _tValue>	
	stin	cid_t	tunerCreate							(SDialog & dialog, pnco<SDialogTuner<_tValue>>	& createdControl)	 { 
		cid_t				index								= -1; 
		if_fail_fe(index = tunerCreate<_tValue>(dialog)); 
		dialog.Controls[index].as(createdControl); 
		return index;
	}

}

#endif // GPK_DIALOG_CONTROLS_H_23627
