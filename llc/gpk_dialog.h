#include "gpk_array_ptr.h"
#include "gpk_gui.h"

#ifndef GPK_DIALOG_H_23627
#define GPK_DIALOG_H_23627

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
		SDialog					* Dialog			= 0;
		cid_t					IdGUIControl		= (cid_t)-1;
		cid_t					IdParent			= (cid_t)-1;
		uint64_t				DeleteControl		: 1;

		constexpr				IDialogControl		()													: DeleteControl(0) {}
		virtual					~IDialogControl		();

		virtual	::gpk::error_t	Resize				(const ::gpk::n2<uint16_t> & newSize)				{ (void)newSize; return 0; }
		virtual	::gpk::error_t	Update				(::gpk::view<const ::gpk::pobj<::gpk::SEventSystem>> eventsIn)	= 0;
	};
	//tplt<tpnm _TDialogControl>
	//::gpk::error_t				dialogCreate										(::gpk::SGUI & gui, ::gpk::pnco<_TDialogControl>& createdControl)	{
	//	::gpk::gpk_ref<_TDialogControl>								* newRef							= 0;
	//	ree_if(0 == ::gpk::ref_create(&newRef));
	//	createdControl.set_ref((::gpk::gpk_ref<::gpk::IDialogControl>*)newRef);
	//	createdControl->Dialog									= this;
	//	createdControl->IdGUIControl							= ::gpk::controlCreate(gui);
	//	return 0;
	//}
#pragma pack(pop)

	struct SDialogColors {
		::gpk::apod<::gpk::asti16<::gpk::GUI_CONTROL_PALETTE_COUNT>>	PerControlType	= {};
		::gpk::asti16<::gpk::GUI_CONTROL_PALETTE_COUNT>					Control			= {};
		::gpk::asti16<::gpk::GUI_CONTROL_PALETTE_COUNT>					Button			= {};
		::gpk::asti16<::gpk::GUI_CONTROL_PALETTE_COUNT>					CheckBox		= {};
		::gpk::asti16<::gpk::GUI_CONTROL_PALETTE_COUNT>					Viewport		= {};
		::gpk::asti16<::gpk::GUI_CONTROL_PALETTE_COUNT>					ViewportTitle	= {};
	};

	struct SDialog {
		::gpk::pobj<::gpk::SDialogColors>	Colors					= {};

		cid_t					FocusedCurrent		= (cid_t)-1;
		cid_t					FocusedPrevious		= (cid_t)-1;
		cid_t					SelectedCurrent		= (cid_t)-1;
		cid_t					SelectedPrevious	= (cid_t)-1;
		::gpk::n2u1_t			LastSize			= {};
		cid_t					Root				= (cid_t)-1;
		::gpk::as2_t			Buttons				= {};

		::gpk::GUI_CONTROL_FLAG	DefaultControlMask	= (::gpk::GUI_CONTROL_FLAG)-1;
		::gpk::SControlDraw		DefaultControlDraw	= {};

		::gpk::pobj<::gpk::SInput>					Input			= {};
		::gpk::pobj<::gpk::SGUI>					GUI				= {};
		::gpk::aobj<::gpk::pnco<IDialogControl>>	Controls		= {};
		::gpk::img<::gpk::bgra>						ImageCrossBGRA	= {};
		::gpk::imgmonou64							ImageCross		= {};

								SDialog			(const ::gpk::pobj<::gpk::SGUI> & gui);
								SDialog			(::gpk::pobj<::gpk::SGUI> & gui);
								SDialog			();

		inline	::gpk::error_t	Focused			()	const	noexcept	{ return FocusedCurrent; }

		::gpk::error_t			Focus			(cid_t iControl)		{ 
			ree_if(Controls.size() <= (uint32_t)(iControl) && iControl != -1, "Invalid control index: %i.", iControl); 
			FocusedPrevious	= FocusedCurrent; 
			FocusedCurrent	= iControl; 
			return 0; 
		}

		tplt<tpnm _TDialogControl>
		::gpk::cid_t			Create			(::gpk::pnco<_TDialogControl> & createdControl)	{
			if(0 == GUI.get_ref())
				GUI.create();
			::gpk::SGUI				& gui				= *GUI;
			cid_t					index				= (cid_t)-1;
			for(uint32_t iControl = 0; iControl < Controls.size(); ++iControl)	// Look for unused slot
				if(0 == Controls[iControl].get_ref()) {
					index				= (cid_t)iControl;
					break;
				}

			if(-1 == index)
				gpk_necs(index = (::gpk::cid_t)Controls.push_back({}));

			//::gpk::dialogCreate(createdControl);
			//Controls[index]	= createdControl;
			::gpk::gref<_TDialogControl>	* newRef	= 0;
			rees_if(0 == ::gpk::ref_create(&newRef));
			Controls[index].set_ref((::gpk::gref<::gpk::IDialogControl>*)newRef);
			Controls[index]->Dialog	= this;
			const uint32_t			idGUIControl		= Controls[index]->IdGUIControl = ::gpk::controlCreate(*GUI);
			gui.Controls.Draw[idGUIControl].UseNewPalettes	= true;
			::gpk::memcpy_s(gui.Controls.Draw[idGUIControl].Palettes.Storage, Colors->Control.Storage);
			//gpk_necall(gpk::controlSetParent(gui, idGUIControl, Root), "%s", "Invalid root id!");
			Controls[index].as(createdControl);
			return index;
		}

		tplt<tpnm _TDialogControl>
		::gpk::cid_t			Get				(cid_t index, ::gpk::pnco<_TDialogControl> & control)	{
			Controls[index].as(control);
			return index;
		}

		::gpk::error_t			Update			()		{
			if(0 == GUI.get_ref())
				GUI.create();

			::gpk::SGUI					& gui			= *GUI;
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
				LastSize				= gui.LastSize;
			}
			//if(0 != Input)
			//	gpk_necall(gpk::guiProcessInput(gui, *Input), "%s", "Unknown reason.");
			for(uint32_t iControl = 0; iControl < Controls.size(); ++iControl)
				if(-1 == Controls[iControl]->IdParent)
					Controls[iControl]->Update(gui.Controls.EventQueue);

			return 0;
		}
	};
}

#endif // GPK_DIALOG_H_23627
