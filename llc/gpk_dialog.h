#include "gpk_array_ptr.h"
#include "gpk_gui.h"

#ifndef GPK_DIALOG_H_23627
#define GPK_DIALOG_H_23627

nmsp gpk
{
#pragma pack(push, 1)
	enum DIALOG_CONTROL : s0_t
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
		u3_t				DeleteControl		: 1;

		cxpr				IDialogControl		()													: DeleteControl(0) {}
		virtual					~IDialogControl		();

		virtual	err_t Resize				(cnst n2u1_t & newSize)					{ (void)newSize; return 0; }
		virtual	err_t Update				(view<cnst pobj<SEventSystem>> eventsIn)	= 0;
	};
	//tplt<tpnm _TDialogControl>
	//err_t 			dialogCreate										(SGUI & gui, pnco<_TDialogControl>& createdControl)	{
	//	gpk_ref<_TDialogControl>								* newRef							= 0;
	//	if_true_fef(0 == ref_create(&newRef));
	//	createdControl.set_ref((gpk_ref<IDialogControl>*)newRef);
	//	createdControl->Dialog									= this;
	//	createdControl->IdGUIControl							= controlCreate(gui);
	//	return 0;
	//}
#pragma pack(pop)

	struct SDialogColors {
		apod<asts1_t<GUI_CONTROL_PALETTE_COUNT>>	PerControlType	= {};
		asts1_t<GUI_CONTROL_PALETTE_COUNT>			Control			= {};
		asts1_t<GUI_CONTROL_PALETTE_COUNT>			Button			= {};
		asts1_t<GUI_CONTROL_PALETTE_COUNT>			CheckBox		= {};
		asts1_t<GUI_CONTROL_PALETTE_COUNT>			Viewport		= {};
		asts1_t<GUI_CONTROL_PALETTE_COUNT>			ViewportTitle	= {};
	};

	struct SDialog {
		pobj<SDialogColors>			Colors				= {};

		cid_t						FocusedCurrent		= (cid_t)-1;
		cid_t						FocusedPrevious		= (cid_t)-1;
		cid_t						SelectedCurrent		= (cid_t)-1;
		cid_t						SelectedPrevious	= (cid_t)-1;
		n2u1_t						LastSize			= {};
		cid_t						Root				= (cid_t)-1;
		as2_t						Buttons				= {};

		GUI_CONTROL_FLAG			DefaultControlMask	= (GUI_CONTROL_FLAG)-1;
		SControlDraw				DefaultControlDraw	= {};

		pobj<SInput>				Input				= {};
		pobj<SGUI>					GUI					= {};
		aobj<pnco<IDialogControl>>	Controls			= {};
		img<bgra>					ImageCrossBGRA		= {};
		imgmonou3_t					ImageCross			= {};

									SDialog				(cnst pobj<SGUI> & gui);
									SDialog				(pobj<SGUI> & gui);
									SDialog				();

		inln	err_t				Focused				()	csnx	{ return FocusedCurrent; }

		err_t 						Focus				(cid_t iControl)		{ 
			if_true_fef(Controls.size() <= (u2_t)(iControl) && iControl != -1, "Invalid control index: %i.", iControl); 
			FocusedPrevious	= FocusedCurrent; 
			FocusedCurrent	= iControl; 
			return 0; 
		}

		tplt<tpnm _TDialogControl>
		cid_t						Create				(pnco<_TDialogControl> & createdControl)	{
			if(0 == GUI.get_ref())
				GUI.create();
			SGUI						& gui				= *GUI;
			cid_t						index				= (cid_t)-1;
			for(u2_t iControl = 0; iControl < Controls.size(); ++iControl)	// Look for unused slot
				if(0 == Controls[iControl].get_ref()) {
					index				= (cid_t)iControl;
					break;
				}

			if(-1 == index)
				if_fail_fe(index = (cid_t)Controls.push_back({}));

			//dialogCreate(createdControl);
			//Controls[index]	= createdControl;
			gref<_TDialogControl>		* newRef			= 0;
			if_true_ve(-1, 0 == ref_create(newRef));
			Controls[index].set_ref((gref<IDialogControl>*)newRef);
			Controls[index]->Dialog	= this;
			u2_c			idGUIControl		= Controls[index]->IdGUIControl = controlCreate(*GUI);
			memcpy_s(gui.Controls.Draw[idGUIControl].Palettes.Storage, Colors->Control.Storage);
			//if_fail_fef(controlSetParent(gui.Controls, idGUIControl, Root), "%s", "Invalid root id!");
			Controls[index].as(createdControl);
			return index;
		}

		tplt<tpnm _TDialogControl>
		cid_t			Get				(cid_t index, pnco<_TDialogControl> & control)	{
			Controls[index].as(control);
			return index;
		}

		err_t 		Update			()		{
			if(0 == GUI.get_ref())
				GUI.create();

			SGUI					& gui			= *GUI;
			for(u2_t iControl = 0; iControl < Controls.size(); ++iControl) {
				if(0 == Controls[iControl].get_ref() || controlInvalid(gui.Controls, Controls[iControl]->IdGUIControl)) {
					if_fail_ef(Controls.remove_unordered(iControl), "iControl=%u", iControl);
					--iControl;
					continue;
				}
			}
			if(LastSize != gui.Controls.LastSize) {
				for(u2_t iControl = 0; iControl < Controls.size(); ++iControl)
					if_fail_ef(Controls[iControl]->Resize(gui.Controls.LastSize), "iControl=%u, cid_t=%i", iControl, Controls[iControl]->IdGUIControl);
				LastSize				= gui.Controls.LastSize;
			}
			//if(0 != Input)
			//	if_fail_fef(guiProcessInput(gui, *Input), "%s", "Unknown reason.");
			for(u2_t iControl = 0; iControl < Controls.size(); ++iControl)
				if(-1 == Controls[iControl]->IdParent)
					if_fail_ef(Controls[iControl]->Update(gui.Controls.EventQueue), "iControl=%u", iControl, Controls[iControl]->IdGUIControl);

			return 0;
		}
	};

	tplt<tpnm _TDialogControl>
	err_t		dialogControlGet	(SDialog & dialog, cid_t idGUIControl, pnco<_TDialogControl> & control) {
		control = {};
		for(u2_t iControl = 0; iControl < dialog.Controls.size(); ++iControl) {
			if(0 == dialog.Controls[iControl].get_ref() || dialog.Controls[iControl]->IdGUIControl != idGUIControl)
				continue;
			dialog.Controls[iControl].as(control);
			break;
		}
		if_true_fef(0 == control.get_ref(), "Dialog control not found for GUI control id: %i.", idGUIControl);
		return 0;
	}

	//stin	err_t	withControlState		(cnst SDialog & dialog, cid_t idControl, cnst FError<cnst SControlState			&> & funcWithControl) { rtrn withControlState		(*dialog.GUI, idControl, funcWithControl); }
	//stin	err_t	withControlPrevious		(cnst SDialog & dialog, cid_t idControl, cnst FError<cnst SControlState			&> & funcWithControl) { rtrn withControlPrevious	(*dialog.GUI, idControl, funcWithControl); }
	//stin	err_t	withControlPlacement	(cnst SDialog & dialog, cid_t idControl, cnst FError<cnst SControlPlacement		&> & funcWithControl) { rtrn withControlPlacement	(*dialog.GUI, idControl, funcWithControl); }
	//stin	err_t	withControlArea			(cnst SDialog & dialog, cid_t idControl, cnst FError<cnst SControlArea			&> & funcWithControl) { rtrn withControlArea		(*dialog.GUI, idControl, funcWithControl); }
	//stin	err_t	withControlConstraints	(cnst SDialog & dialog, cid_t idControl, cnst FError<cnst SControlConstraints	&> & funcWithControl) { rtrn withControlConstraints	(*dialog.GUI, idControl, funcWithControl); }
	//stin	err_t	withControlDraw			(cnst SDialog & dialog, cid_t idControl, cnst FError<cnst SControlDraw			&> & funcWithControl) { rtrn withControlDraw		(*dialog.GUI, idControl, funcWithControl); }
	//stin	err_t	withControlText			(cnst SDialog & dialog, cid_t idControl, cnst FError<cnst SControlText			&> & funcWithControl) { rtrn withControlText		(*dialog.GUI, idControl, funcWithControl); }
	//stin	err_t	withControlImages		(cnst SDialog & dialog, cid_t idControl, cnst FError<cnst SControlImage			&> & funcWithControl) { rtrn withControlImages		(*dialog.GUI, idControl, funcWithControl); }
	//stin	err_t	withControlChildren		(cnst SDialog & dialog, cid_t idControl, cnst FError<cnst acid					&> & funcWithControl) { rtrn withControlChildren	(*dialog.GUI, idControl, funcWithControl); }
	//stin	err_t	withControlChildren		(cnst SDialog & dialog, cid_t idControl, cnst FError<cnst cid_t					&> & funcWithNesting) { rtrn withControlChildren	(*dialog.GUI, idControl, funcWithNesting); }
	//stin	err_t	withControlState		(SDialog & dialog, cid_t idControl, cnst FError< SControlState			&> & funcWithControl)	{ if(not dialog.GUI) if_null_fe(dialog.GUI.create()); rtrn withControlState			(*dialog.GUI, idControl, funcWithControl); }
	//stin	err_t	withControlPrevious		(SDialog & dialog, cid_t idControl, cnst FError< SControlState			&> & funcWithControl)	{ if(not dialog.GUI) if_null_fe(dialog.GUI.create()); rtrn withControlPrevious		(*dialog.GUI, idControl, funcWithControl); }
	//stin	err_t	withControlPlacement	(SDialog & dialog, cid_t idControl, cnst FError< SControlPlacement		&> & funcWithControl)	{ if(not dialog.GUI) if_null_fe(dialog.GUI.create()); rtrn withControlPlacement		(*dialog.GUI, idControl, funcWithControl); }
	//stin	err_t	withControlArea			(SDialog & dialog, cid_t idControl, cnst FError< SControlArea			&> & funcWithControl)	{ if(not dialog.GUI) if_null_fe(dialog.GUI.create()); rtrn withControlArea			(*dialog.GUI, idControl, funcWithControl); }
	//stin	err_t	withControlConstraints	(SDialog & dialog, cid_t idControl, cnst FError< SControlConstraints	&> & funcWithControl)	{ if(not dialog.GUI) if_null_fe(dialog.GUI.create()); rtrn withControlConstraints	(*dialog.GUI, idControl, funcWithControl); }
	//stin	err_t	withControlDraw			(SDialog & dialog, cid_t idControl, cnst FError< SControlDraw			&> & funcWithControl)	{ if(not dialog.GUI) if_null_fe(dialog.GUI.create()); rtrn withControlDraw			(*dialog.GUI, idControl, funcWithControl); }
	//stin	err_t	withControlText			(SDialog & dialog, cid_t idControl, cnst FError< SControlText			&> & funcWithControl)	{ if(not dialog.GUI) if_null_fe(dialog.GUI.create()); rtrn withControlText			(*dialog.GUI, idControl, funcWithControl); }
	//stin	err_t	withControlImages		(SDialog & dialog, cid_t idControl, cnst FError< SControlImage			&> & funcWithControl)	{ if(not dialog.GUI) if_null_fe(dialog.GUI.create()); rtrn withControlImages		(*dialog.GUI, idControl, funcWithControl); }
	//stin	err_t	withControlChildren		(SDialog & dialog, cid_t idControl, cnst FError< acid					&> & funcWithControl)	{ if(not dialog.GUI) if_null_fe(dialog.GUI.create()); rtrn withControlChildren		(*dialog.GUI, idControl, funcWithControl); }
	//stin	err_t	withControlChildren		(SDialog & dialog, cid_t idControl, cnst FError< cid_t					&> & funcWithNesting)	{ if(not dialog.GUI) if_null_fe(dialog.GUI.create()); rtrn withControlChildren		(*dialog.GUI, idControl, funcWithNesting); }
}

#endif // GPK_DIALOG_H_23627
