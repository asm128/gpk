#include "gpk_ptr.h"
#include "gpk_gui.h"
#include "gpk_enum.h"

#ifndef GPK_DIALOG_H_67383889976
#define GPK_DIALOG_H_67383889976

namespace gpk
{
#pragma pack(push, 1)
	enum DIALOG_CONTROL : int8_t
		{ DIALOG_CONTROL_Label			= 0
		, DIALOG_CONTROL_Button
		, DIALOG_CONTROL_ListBox
		, DIALOG_CONTROL_Slider
		, DIALOG_CONTROL_ScrollBar
		, DIALOG_CONTROL_EditBox
		, DIALOG_CONTROL_VirtualKeyboard
		, DIALOG_CONTROL_COUNT
		};

	struct SDialog;
	struct IDialogControl {
		SDialog												* Dialog											= 0;
		int32_t												IdGUIControl										= -1;
		int32_t												IdParent											= -1;
		uint64_t											DeleteControl										: 1;

		constexpr											IDialogControl										()													: DeleteControl(0) {}
		virtual												~IDialogControl										();

		virtual	::gpk::error_t								Resize												(::gpk::SCoord2<uint32_t>& newSize)					{ (void)newSize; return 0; }
		virtual	::gpk::error_t								Update												()													= 0;
	};
	//template<typename _TDialogControl>
	//::gpk::error_t										dialogCreate										(::gpk::SGUI & gui, ::gpk::ptr_nco<_TDialogControl>& createdControl)	{
	//	::gpk::gpk_ref<_TDialogControl>								* newRef							= 0;
	//	ree_if(0 == ::gpk::ref_create(&newRef), "%s", "Out of memory?");
	//	createdControl.set_ref((::gpk::gpk_ref<::gpk::IDialogControl>*)newRef);
	//	createdControl->Dialog									= this;
	//	createdControl->IdGUIControl							= ::gpk::controlCreate(gui);
	//	return 0;
	//}
#pragma pack(pop)

	struct SDialogColors {
		::gpk::apod<::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_PALETTE_COUNT>>	PerControlType										= {};
		::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_PALETTE_COUNT>						Control												= {};
		::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_PALETTE_COUNT>						Button												= {};
		::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_PALETTE_COUNT>						CheckBox											= {};
		::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_PALETTE_COUNT>						Viewport											= {};
		::gpk::array_static<int32_t, ::gpk::GUI_CONTROL_PALETTE_COUNT>						ViewportTitle										= {};
	};

	struct SDialog {
		::gpk::pobj<::gpk::SDialogColors>													Colors												= {};
		
		int32_t																				FocusedCurrent										= -1;
		int32_t																				FocusedPrevious										= -1;
		int32_t																				SelectedCurrent										= -1;
		int32_t																				SelectedPrevious									= -1;
		::gpk::SCoord2<uint32_t>															LastSize											= {};
		int32_t																				Root												= -1;
		::gpk::SControlMode																	DefaultControlModes									= {};

		::gpk::pobj<::gpk::SInput>															Input												= {};
		::gpk::pobj<::gpk::SGUI>															GUI													= {};
		::gpk::aobj<::gpk::ptr_nco<IDialogControl>>											Controls											= {};
		::gpk::img<::gpk::SColorBGRA>														ImageCrossBGRA										= {};
		::gpk::SImageMonochrome<uint32_t>													ImageCross											= {};

																							SDialog												(const ::gpk::pobj<::gpk::SGUI> & gui);
																							SDialog												(::gpk::pobj<::gpk::SGUI> & gui);
																							SDialog												();

		::gpk::error_t																		Focus												(int32_t iControl)									{ ree_if(Controls.size() <= (uint32_t)(iControl) && iControl != -1, "Invalid control index: %i.", iControl); FocusedPrevious = FocusedCurrent; FocusedCurrent = iControl; return 0; }
		::gpk::error_t																		Focused												()													{ return FocusedCurrent; }

		template<typename _TDialogControl>
		::gpk::error_t																		Create												(::gpk::ptr_nco<_TDialogControl>& createdControl)	{
			if(0 == GUI.get_ref())
				GUI.create();
			::gpk::SGUI																				& gui												= *GUI;
			int32_t																					index												= -1;
			for(uint32_t iControl = 0; iControl < Controls.size(); ++iControl)	// Look for unused slot
				if(0 == Controls[iControl].get_ref()) {
					index																		= iControl;
					break;
				}

			if(-1 == index)
				gpk_necall(index = Controls.push_back({}), "%s", "Out of memory?");

			//::gpk::dialogCreate(createdControl);
			//Controls[index]														= createdControl;
			::gpk::gpk_ref<_TDialogControl>											* newRef											= 0;
			ree_if(0 == ::gpk::ref_create(&newRef), "%s", "Out of memory?");
			Controls[index].set_ref((::gpk::gpk_ref<::gpk::IDialogControl>*)newRef);
			Controls[index]->Dialog												= this;
			Controls[index]->IdGUIControl										= ::gpk::controlCreate(*GUI);
			gui.Controls.Modes[Controls[index]->IdGUIControl].UseNewPalettes	= true;
			::gpk::memcpy_s(gui.Controls.Controls[Controls[index]->IdGUIControl].Palettes.Storage, Colors->Control.Storage);
			//gpk_necall(::gpk::controlSetParent(gui, Controls[index]->IdGUIControl, Root), "%s", "Invalid root id!");
			Controls[index].as(createdControl);
			return index;
		}

		template<typename _TDialogControl>
		::gpk::error_t														Get												(int32_t index, ::gpk::ptr_nco<_TDialogControl>& control)	{
			Controls[index].as(control);
			return index;
		}

		::gpk::error_t														Update												()													{
			if(0 == GUI.get_ref())
				GUI.create();
			::gpk::SGUI																& gui												= *GUI;
			for(uint32_t iControl = 0; iControl < Controls.size(); ++iControl) {
				if(0 == Controls[iControl].get_ref() || ::gpk::controlInvalid(gui, Controls[iControl]->IdGUIControl)) {
					Controls.remove_unordered(iControl);
					--iControl;
					continue;
				}
			}
			if(LastSize != gui.LastSize) {
				for(uint32_t iControl = 0; iControl < Controls.size(); ++iControl)
					Controls[iControl]->Resize(gui.LastSize);
				LastSize															= gui.LastSize;
			}
			//if(0 != Input)
			//	gpk_necall(::gpk::guiProcessInput(gui, *Input), "%s", "Unknown reason.");
			for(uint32_t iControl = 0; iControl < Controls.size(); ++iControl)
				if(-1 == Controls[iControl]->IdParent)
					Controls[iControl]->Update();
			return 0;
		}
	};
}

#endif // GPK_DIALOG_H_67383889976
