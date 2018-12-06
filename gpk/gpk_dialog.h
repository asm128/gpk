#include "gpk_ptr.h"
#include "gpk_gui.h"

#ifndef GPK_DIALOG_H_67383889976
#define GPK_DIALOG_H_67383889976

namespace gpk
{
	enum DIALOG_CONTROL : int8_t
		{ DIALOG_CONTROL_Label			= 0
		, DIALOG_CONTROL_Button
		, DIALOG_CONTROL_ListBox
		, DIALOG_CONTROL_Slider
		, DIALOG_CONTROL_ScrollBar
		, DIALOG_CONTROL_EditBox
		, DIALOG_CONTROL_COUNT
		};

	struct SDialog;
	struct IDialogControl {
		SDialog												* Dialog							= 0;
		int32_t												IdGUIControl						= -1;

	    virtual												~IDialogControl						();

        virtual	::gpk::error_t								Update								()				= 0;
	};

	struct SDialog {
		int32_t												ColorsControl	[::gpk::GUI_CONTROL_PALETTE_COUNT]						= {};
		int32_t												ColorsButton	[::gpk::GUI_CONTROL_PALETTE_COUNT]						= {};
		int32_t												ColorsCheckBox	[::gpk::GUI_CONTROL_PALETTE_COUNT]						= {};
		int32_t												FocusedCurrent						= -1;
		int32_t												FocusedPrevious						= -1;
		int32_t												SelectedCurrent						= -1;
		int32_t												SelectedPrevious					= -1;

		::gpk::ptr_obj<::gpk::SInput>						Input;
		::gpk::SImageMonochrome<uint32_t>					ImageCross							= {};
		::gpk::SImage<::gpk::SColorBGRA>					ImageCrossBGRA						= {};

		::gpk::SGUI											GUI;
		int32_t												Root;
		::gpk::array_obj<::gpk::ptr_nco<IDialogControl>>	Controls;

															SDialog								();

		template<typename _TDialogControl>
		::gpk::error_t										Create								(::gpk::ptr_nco<_TDialogControl>& createdControl)		{
			int32_t													index								= -1;
			for(uint32_t iControl = 0; iControl < Controls.size(); ++iControl)	// Look for unused slot
				if(0 == Controls[iControl]) {
					index												= iControl;
					break;
				}

			if(-1 == index)
				gpk_necall(index = Controls.push_back({}), "%s", "Out of memory?");

			::gpk::gpk_ref<_TDialogControl>							* newRef							= 0;
			ree_if(0 == ::gpk::ref_create(&newRef), "%s", "Out of memory?");
			Controls[index].set_ref((::gpk::gpk_ref<::gpk::IDialogControl>*)newRef);
			Controls[index]->Dialog								= this;
			Controls[index]->IdGUIControl						= ::gpk::controlCreate(GUI);
			GUI.Controls.Modes[Controls[index]->IdGUIControl].UseNewPalettes	= true;
			::gpk::memcpy_s(GUI.Controls.Controls[Controls[index]->IdGUIControl].Palettes, ColorsControl);
			gpk_necall(::gpk::controlSetParent(GUI, Controls[index]->IdGUIControl, Root), "Invalid parent id!");
			Controls[index].as(createdControl);
			return index;
		}

		::gpk::error_t										Update								()	{
			if(0 != Input)
				gpk_necall(::gpk::guiProcessInput(GUI, *Input), "%s", "Unknown reason.");
			for(uint32_t iControl = 0; iControl < Controls.size(); ++iControl) {
				if(0 == Controls[iControl]) 
					continue;
				if(::gpk::controlInvalid(GUI, Controls[iControl]->IdGUIControl))	
					Controls[iControl]									= {};
				else
					Controls[iControl]->Update();
			}
			return 0;
		}


		::gpk::error_t										Focus								(int32_t iControl)	{ ree_if(Controls.size() <= (uint32_t)(iControl) && iControl != -1, "Invalid control index: %i.", iControl); FocusedPrevious = FocusedCurrent; FocusedCurrent = iControl; return 0; }
		::gpk::error_t										Focused								()					{ return FocusedCurrent; }

	};

	struct SDialogSlider;
	::gpk::error_t										sliderCreate						(::gpk::SDialog			& dialog);
	::gpk::error_t										sliderSetValue						(::gpk::SDialogSlider	& control, int64_t value);
	::gpk::error_t										sliderUpdate						(::gpk::SDialogSlider	& control);

	struct SDialogTuner;
	::gpk::error_t										tunerCreate							(::gpk::SDialog			& dialog);
	::gpk::error_t										tunerUpdate							(::gpk::SDialogTuner	& control);

	struct SDialogCheckBox;
	::gpk::error_t										checkBoxCreate						(::gpk::SDialog			& dialog);
	::gpk::error_t										checkBoxUpdate						(::gpk::SDialogCheckBox	& control);

	struct SDialogEditBox;
	::gpk::error_t										editBoxCreate						(::gpk::SDialog			& dialog);
	::gpk::error_t										editBoxUpdate						(::gpk::SDialogEditBox	& control);

	struct SDialogEditBox : public ::gpk::IDialogControl {
				::gpk::SMinMax<uint32_t>					Selection							= {};
				::gpk::array_pod<char_t>					String								= {};

		virtual	::gpk::error_t								Update								()							{ return ::gpk::editBoxUpdate(*this); }
	};

	struct SDialogCheckBox : public ::gpk::IDialogControl {
				bool										Checked								= false;

		virtual	::gpk::error_t								Update								()							{ return ::gpk::checkBoxUpdate(*this); }
	};

	struct SDialogSlider : public ::gpk::IDialogControl {
				int32_t										IdButton							= -1;

				::gpk::SMinMax<int64_t>						ValueLimits							= {0, 100};// 0x7fFFffFF};
				int64_t										ValueCurrent						= -1;
				bool										Vertical							= false;

		virtual	::gpk::error_t								Update								()							{ return ::gpk::sliderUpdate(*this); }
	};

	struct SDialogTuner : public ::gpk::IDialogControl {
				int32_t										IdDecrease							= -1;
				int32_t										IdIncrease							= -1;

				::gpk::SMinMax<int64_t>						ValueLimits							= {0xC0000000, 0x3fFFffFF};
				int64_t										ValueCurrent						= -1;
				char_t										ValueString	[32]					= {};

		virtual	::gpk::error_t								Update								()							{ return ::gpk::tunerUpdate(*this); }
	};

	static inline	::gpk::error_t						sliderCreate						(::gpk::SDialog			& dialog, ::gpk::ptr_nco<SDialogSlider	>& createdControl)	 { int32_t index = -1; gpk_necall(index = sliderCreate		(dialog), "%s", "Out of memory?"); dialog.Controls[index].as(createdControl); return index; }
	static inline	::gpk::error_t						tunerCreate							(::gpk::SDialog			& dialog, ::gpk::ptr_nco<SDialogTuner	>& createdControl)	 { int32_t index = -1; gpk_necall(index = tunerCreate		(dialog), "%s", "Out of memory?"); dialog.Controls[index].as(createdControl); return index; }
	static inline	::gpk::error_t						checkBoxCreate						(::gpk::SDialog			& dialog, ::gpk::ptr_nco<SDialogCheckBox>& createdControl)	 { int32_t index = -1; gpk_necall(index = checkBoxCreate	(dialog), "%s", "Out of memory?"); dialog.Controls[index].as(createdControl); return index; }

}

#endif // GPK_DIALOG_H_67383889976
