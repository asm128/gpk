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
		::gpk::ptr_obj<::gpk::SInput>						Input;
		::gpk::SGUI											GUI;
		int32_t												Root;
		::gpk::array_obj<::gpk::ptr_nco<IDialogControl>>	Controls;

															SDialog								()				{
			Root												= ::gpk::controlCreate(GUI);													
			throw_if(-1 == Root, "%s", "Out of memory?");
		}

		template<typename _TDialogControl>
		::gpk::error_t										Create								(::gpk::ptr_nco<_TDialogControl>& createdControl)		{
			int32_t													index									= -1;
			for(uint32_t iControl = 0; iControl < Controls.size(); ++iControl)	// Look for unused slot
				if(0 == Controls[iControl]) {
					index												= iControl;
					break;
				}

			if(-1 == index)
				gpk_necall(index = Controls.push_back({}), "%s", "Out of memory?");

			::gpk::gpk_ref<_TDialogControl>							* newRef								= 0;
			ree_if(0 == ::gpk::ref_create(&newRef), "%s", "Out of memory?");
			Controls[index].set_ref((::gpk::gpk_ref<::gpk::IDialogControl>*)newRef);
			Controls[index]->Dialog								= this;
			Controls[index]->IdGUIControl						= ::gpk::controlCreate(GUI);
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
				if(-1 == Controls[iControl]->IdGUIControl)	
					Controls[iControl]									= {};
				else
					Controls[iControl]->Update();
			}
			return 0;
		}
	};

	struct SDialogSlider;
	::gpk::error_t										sliderCreate						(::gpk::SDialog			& dialog);
	::gpk::error_t										sliderSetValue						(::gpk::SDialogSlider	& slider, int32_t value);

	struct SDialogSlider : public ::gpk::IDialogControl {
		int32_t												IdButton							= -1;

		int32_t												ValueMin							= 0;
		int32_t												ValueMax							= (-1) & 0x7FFFFFFF;
		int32_t												ValueCur							= -1;
		bool												Vertical							= false;

		virtual												~SDialogSlider						()							{}

		virtual	::gpk::error_t								Update								()							{
			if(Dialog->GUI.Controls.States[IdButton].Pressed) {
				if(Dialog->Input->MouseCurrent.Position != Dialog->Input->MousePrevious.Position) {
					::gpk::SCoord2<double>											scale								= 
						{ 1.0 / (Dialog->GUI.Zoom.ZoomLevel * (Dialog->GUI.Zoom.DPI.x / 96.0))
						, 1.0 / (Dialog->GUI.Zoom.ZoomLevel * (Dialog->GUI.Zoom.DPI.y / 96.0))
						};
					::gpk::SCoord2<double>											slideInPixels		 				= 
						{ Dialog->Input->MouseCurrent.Deltas.x * scale.x * ((ValueMax - ValueMin)) * (1.0 / (Dialog->GUI.Controls.Metrics[IdGUIControl].Total.Global.Size.x))
						, Dialog->Input->MouseCurrent.Deltas.y * scale.y * ((ValueMax - ValueMin)) * (1.0 / (Dialog->GUI.Controls.Metrics[IdGUIControl].Total.Global.Size.y))
						};

					if(slideInPixels.y > 0 && slideInPixels.y < 1)
						slideInPixels.y = 1;
					if(slideInPixels.y < 0 && slideInPixels.y > -1)
						slideInPixels.y = -1;
		
					if(slideInPixels.x > 0 && slideInPixels.x < 1)
						slideInPixels.x = 1;
					if(slideInPixels.x < 0 && slideInPixels.x > -1)
						slideInPixels.x = -1;

					int32_t															slide								= Vertical
						? (int32_t)slideInPixels.y
						: (int32_t)slideInPixels.x
						;
					::gpk::sliderSetValue(*this, ValueCur + slide);
					
				}
			}
			else
				if(Dialog->Input->ButtonDown(0)) {	// 
				
				}
			return 0;
		}
	};

}

#endif // GPK_DIALOG_H_67383889976
