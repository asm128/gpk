	#include "gpk_ptr.h"
#include "gpk_gui.h"

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
		, DIALOG_CONTROL_COUNT
		};

	struct SDialog;
	struct IDialogControl {
		SDialog												* Dialog											= 0;
		int32_t												IdGUIControl										= -1;
		int32_t												IdParent											= -1;

		virtual												~IDialogControl										();

		virtual	::gpk::error_t								Resize												(::gpk::SCoord2<uint32_t>& newSize)					{ newSize; return 0; }
		virtual	::gpk::error_t								Update												()													= 0;
	};
	//template<typename _TDialogControl>
	//::gpk::error_t										dialogCreate										(::gpk::SGUI & gui, ::gpk::ptr_nco<_TDialogControl>& createdControl)	{
	//	::gpk::gpk_ref<_TDialogControl>							* newRef							= 0;
	//	ree_if(0 == ::gpk::ref_create(&newRef), "%s", "Out of memory?");
	//	createdControl.set_ref((::gpk::gpk_ref<::gpk::IDialogControl>*)newRef);
	//	createdControl->Dialog								= this;
	//	createdControl->IdGUIControl						= ::gpk::controlCreate(gui);
	//	return 0;
	//}
#pragma pack(pop)

	struct SDialog {
		int32_t												ColorsControl	[::gpk::GUI_CONTROL_PALETTE_COUNT]	= {};
		int32_t												ColorsButton	[::gpk::GUI_CONTROL_PALETTE_COUNT]	= {};
		int32_t												ColorsCheckBox	[::gpk::GUI_CONTROL_PALETTE_COUNT]	= {};
		int32_t												FocusedCurrent										= -1;
		int32_t												FocusedPrevious										= -1;
		int32_t												SelectedCurrent										= -1;
		int32_t												SelectedPrevious									= -1;
		::gpk::SCoord2<uint32_t>							LastSize											= {};
		int32_t												Root												= -1;

		::gpk::ptr_obj<::gpk::SInput>						Input												= {};
		::gpk::SGUI											GUI													= {};
		::gpk::array_obj<::gpk::ptr_nco<IDialogControl>>	Controls											= {};
		::gpk::SImage<::gpk::SColorBGRA>					ImageCrossBGRA										= {};
		::gpk::SImageMonochrome<uint32_t>					ImageCross											= {};

															SDialog												();

		::gpk::error_t										Focus												(int32_t iControl)									{ ree_if(Controls.size() <= (uint32_t)(iControl) && iControl != -1, "Invalid control index: %i.", iControl); FocusedPrevious = FocusedCurrent; FocusedCurrent = iControl; return 0; }
		::gpk::error_t										Focused												()													{ return FocusedCurrent; }

		template<typename _TDialogControl>
		::gpk::error_t										Create												(::gpk::ptr_nco<_TDialogControl>& createdControl)	{
			int32_t													index												= -1;
			for(uint32_t iControl = 0; iControl < Controls.size(); ++iControl)	// Look for unused slot
				if(0 == Controls[iControl]) {
					index												= iControl;
					break;
				}

			if(-1 == index)
				gpk_necall(index = Controls.push_back({}), "%s", "Out of memory?");

			//::gpk::dialogCreate(createdControl);
			//Controls[index]										= createdControl;
			::gpk::gpk_ref<_TDialogControl>							* newRef											= 0;
			ree_if(0 == ::gpk::ref_create(&newRef), "%s", "Out of memory?");
			Controls[index].set_ref((::gpk::gpk_ref<::gpk::IDialogControl>*)newRef);
			Controls[index]->Dialog								= this;
			Controls[index]->IdGUIControl						= ::gpk::controlCreate(GUI);
			GUI.Controls.Modes[Controls[index]->IdGUIControl].UseNewPalettes	= true;
			::gpk::memcpy_s(GUI.Controls.Controls[Controls[index]->IdGUIControl].Palettes, ColorsControl);
			gpk_necall(::gpk::controlSetParent(GUI, Controls[index]->IdGUIControl, Root), "%s", "Invalid root id!");
			Controls[index].as(createdControl);
			return index;
		}

		::gpk::error_t										Update												()													{
			for(uint32_t iControl = 0; iControl < Controls.size(); ++iControl) {
				if(0 == Controls[iControl] || ::gpk::controlInvalid(GUI, Controls[iControl]->IdGUIControl)) {
					Controls.remove_unordered(iControl);
					--iControl;
					continue;
				}
			}
			if(LastSize != GUI.LastSize) {
				for(uint32_t iControl = 0; iControl < Controls.size(); ++iControl) 
					Controls[iControl]->Resize(GUI.LastSize);
				LastSize											= GUI.LastSize;
			}
			if(0 != Input)
				gpk_necall(::gpk::guiProcessInput(GUI, *Input), "%s", "Unknown reason.");
			for(uint32_t iControl = 0; iControl < Controls.size(); ++iControl) 
				if(-1 == Controls[iControl]->IdParent)
					Controls[iControl]->Update();
			return 0;
		}
	};
}

#endif // GPK_DIALOG_H_67383889976
