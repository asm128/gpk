#include "gpk_dialog.h"

#include <functional>

#ifndef GPK_DIALOG_CONTROLS_H_209837402937
#define GPK_DIALOG_CONTROLS_H_209837402937

namespace gpk
{
	struct SDialogSlider;
	::gpk::error_t			sliderCreate		(::gpk::SDialog					& dialog);
	::gpk::error_t			sliderSetValue		(::gpk::SDialogSlider			& control, int64_t value);
	::gpk::error_t			sliderUpdate		(::gpk::SDialogSlider			& control);

	struct SDialogCheckBox;
	::gpk::error_t			checkBoxCreate		(::gpk::SDialog					& dialog);
	::gpk::error_t			checkBoxUpdate		(::gpk::SDialogCheckBox			& control);

	struct SDialogEditBox;
	::gpk::error_t			editBoxCreate		(::gpk::SDialog					& dialog);
	::gpk::error_t			editBoxUpdate		(::gpk::SDialogEditBox			& control);

	struct SDialogViewport;
	::gpk::error_t			viewportCreate		(::gpk::SDialog					& dialog);
	::gpk::error_t			viewportUpdate		(::gpk::SDialogViewport			& control);
	::gpk::error_t			viewportFold		(::gpk::SDialogViewport			& control, bool fold);
	::gpk::error_t			viewportAdjustSize	(::gpk::n2<int16_t>		& sizeViewport, const ::gpk::n2<int16_t> & sizeClient);

	stainli	::gpk::error_t	sliderCreate		(::gpk::SDialog			& dialog, ::gpk::pnco<SDialogSlider		>	& createdControl)	 { int32_t index = -1; gpk_necs(index = sliderCreate	(dialog)); dialog.Controls[index].as(createdControl); return index; }
	stainli	::gpk::error_t	checkBoxCreate		(::gpk::SDialog			& dialog, ::gpk::pnco<SDialogCheckBox	>	& createdControl)	 { int32_t index = -1; gpk_necs(index = checkBoxCreate	(dialog)); dialog.Controls[index].as(createdControl); return index; }
	stainli	::gpk::error_t	editBoxCreate		(::gpk::SDialog			& dialog, ::gpk::pnco<SDialogEditBox	>	& createdControl)	 { int32_t index = -1; gpk_necs(index = editBoxCreate	(dialog)); dialog.Controls[index].as(createdControl); return index; }
	stainli	::gpk::error_t	viewportCreate		(::gpk::SDialog			& dialog, ::gpk::pnco<SDialogViewport	>	& createdControl)	 { int32_t index = -1; gpk_necs(index = viewportCreate	(dialog)); dialog.Controls[index].as(createdControl); return index; }

#pragma pack(push, 1)
	struct SDialogViewportSettings {
		bool					Dragging			: 1;
		bool					Unfolded			: 1;
		bool					DisplacementLockX	: 1;
		bool					DisplacementLockY	: 1;
		bool					ScrollBarX			: 1;
		bool					ScrollBarY			: 1;
	};

	struct SDialogViewport : public ::gpk::IDialogControl {
		cid_t					IdClose				= -1;
		cid_t					IdMinimize			= -1;
		cid_t					IdMaximize			= -1;
		cid_t					IdScrollBarX		= -1;
		cid_t					IdScrollBarY		= -1;
		cid_t					IdTitle				= -1;
		cid_t					IdClient			= -1;
		SDialogViewportSettings	Settings			= {};
		SDialogViewportSettings	SettingsOld			= {};

		virtual	::gpk::error_t	Update				()							{ return ::gpk::viewportUpdate(*this); }
	};
#pragma pack(pop)

	struct SDialogEditBox : public ::gpk::IDialogControl {
		::gpk::minmax<uint32_t>	Selection			= {};
		::gpk::apod<char>		String				= {};

		virtual	::gpk::error_t	Update				()							{ return ::gpk::editBoxUpdate(*this); }
	};

	struct SDialogCheckBox : public ::gpk::IDialogControl {
		bool					Checked				= false;

		virtual	::gpk::error_t	Update				()							{ return ::gpk::checkBoxUpdate(*this); }
	};

	struct SDialogSlider : public ::gpk::IDialogControl {
		typedef	::std::function<::gpk::error_t(::gpk::vcc & format, int64_t value, const ::gpk::minmax<int64_t> & limits)>	
								TCallback;

		cid_t					IdButton			= (cid_t)-1;
		bool					Vertical			= false;
		::gpk::minmax<int64_t>	ValueLimits			= {0, 100};// 0x7fFFffFF};
		int64_t					ValueCurrent		= -1;
		char					ValueString	[64]	= {};

		TCallback				FuncValueFormat		= []	(::gpk::vcc & format, int64_t, const ::gpk::minmax<int64_t> &)			mutable	{ format = ::gpk::vcs("%lli"); return 0; };
		TCallback				FuncGetString		= [this](::gpk::vcc & inouts, int64_t value, const ::gpk::minmax<int64_t> &)	mutable	{ 
			inouts	= {this->ValueString, (uint32_t)snprintf(this->ValueString, ::gpk::size(this->ValueString) - 2, ::gpk::toString(inouts).begin(), value)}; 
			return 0; 
		};

		virtual	::gpk::error_t	Update				()							{ return ::gpk::sliderUpdate(*this); }
	};

	template<typename _tValue>
	struct SDialogTuner : public ::gpk::IDialogControl {
		typedef	::std::function<::gpk::error_t(::gpk::vcc & format, _tValue value, const ::gpk::minmax<_tValue> & limits)>	
								TCallback;

		cid_t					IdDecrease			= -1;
		cid_t					IdIncrease			= -1;

		::gpk::minmax<_tValue>	ValueLimits			= {0, 0x7F};
		_tValue					ValueCurrent		= 0;
		char					ValueString	[64]	= {};

		TCallback				FuncValueFormat	= [this](::gpk::vcc & format, _tValue/*value*/, const ::gpk::minmax<_tValue> &)	mutable	{ format = ::gpk::vcs("%lli"); return 0; };
		TCallback				FuncGetString	= [this](::gpk::vcc & inouts, _tValue value, const ::gpk::minmax<_tValue> &)		mutable	{ 
			inouts	= {this->ValueString, (uint32_t)snprintf(this->ValueString, ::gpk::size(this->ValueString) - 2, ::gpk::toString(inouts).begin(), value)}; 
			return 0; 
		};

		virtual	::gpk::error_t	Update				()							{ 
			::gpk::SDialog				& dialog			= *Dialog;
			::gpk::SControlTable		& controlTable		= dialog.GUI->Controls;
			if(controlTable.Events[IdDecrease].Execute || controlTable.Events[IdIncrease].Execute) {
				if(controlTable.Events[IdDecrease].Execute)
					return SetValue(ValueCurrent - 1);
				else if(controlTable.Events[IdIncrease].Execute)
					return SetValue(ValueCurrent + 1);
			}
			return 0;
		}

		::gpk::error_t			SetValue			(_tValue value)	{
			if(value < ValueLimits.Min)
				value					= ValueLimits.Min;
			else if(value > ValueLimits.Max)
				value					= ValueLimits.Max;
			ValueCurrent			= value;

			::gpk::vcc					valueString;
			FuncValueFormat	(valueString, value, ValueLimits);
			FuncGetString	(valueString, ValueCurrent, ValueLimits);
			gpk_necs(::gpk::controlTextSet(*Dialog->GUI, IdGUIControl, valueString));
			gpk_necs(::gpk::controlMetricsInvalidate(*Dialog->GUI, IdGUIControl));
			return 0;
		}
	};

	template<typename _tValue>
	::gpk::error_t			tunerCreate							(::gpk::SDialog	& dialog)								{
		int32_t						index								= -1;
		::gpk::pobj<::gpk::SDialogTuner<_tValue>>	tuner;
		gpk_necs(index = dialog.Create(tuner));
		tuner->ValueCurrent		= 0; //tuner->ValueLimits.Min;
		gpk_necs(tuner->IdDecrease = ::gpk::controlCreateChild(*dialog.GUI, tuner->IdGUIControl));
		gpk_necs(tuner->IdIncrease = ::gpk::controlCreateChild(*dialog.GUI, tuner->IdGUIControl));
		::gpk::SControlTable		& controlTable						= dialog.GUI->Controls;
		controlTable.Placement[tuner->IdGUIControl].Margin	= {};
		for(int32_t iControl = tuner->IdDecrease; iControl < tuner->IdIncrease + 1; ++iControl) {
			::gpk::SControlPlacement	& controlButton						= controlTable.Placement[iControl];
			controlButton.Align		= (iControl == tuner->IdDecrease) ? ::gpk::ALIGN_CENTER_LEFT : ::gpk::ALIGN_CENTER_RIGHT;
			::gpk::memcpy_s(controlTable.Draw[iControl].Palettes.Storage, dialog.Colors->Button.Storage);
			controlTable.Text			[iControl].Text						= (iControl == tuner->IdDecrease) ? "-" : "+";
			controlTable.Draw			[iControl].UseNewPalettes			= true;
			controlTable.Draw			[iControl].ColorMode				= ::gpk::GUI_COLOR_MODE_3D;
			controlTable.Constraints	[iControl].AttachSizeToControl.y	= iControl;
		}
		tuner->SetValue(tuner->ValueCurrent);
		controlTable.Text[tuner->IdGUIControl].Align		= ::gpk::ALIGN_CENTER;
		return index;
	}

	template<typename _tValue>	
	stainli	::gpk::error_t	tunerCreate							(::gpk::SDialog & dialog, ::gpk::pnco<::gpk::SDialogTuner<_tValue>>	& createdControl)	 { 
		int32_t						index								= -1; 
		gpk_necs(index = ::gpk::tunerCreate<_tValue>(dialog)); 
		dialog.Controls[index].as(createdControl); 
		return index;
	}

}

#endif // GPK_DIALOG_CONTROLS_H_209837402937
