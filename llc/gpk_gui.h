#include "gpk_gui_control_table.h"
#include "gpk_input.h"
#include "gpk_font.h"

#include "gpk_ascii_color.h"
#ifndef GPK_GUI_H_23627
#define GPK_GUI_H_23627

nmsp gpk
{
	struct SGUIZoom {
		n2f3_t				DPI					= {1.0, 1.0};
		minmaxf3_t			ZoomLimits			= {0.1, 10.0};
		f3_t				ZoomLevel			= 1.0;

		b8_t				oper==				(cnst SGUIZoom & other)		csnx	{
			return DPI			== other.DPI
				&& ZoomLevel	== other.ZoomLevel
				;
		}
		inln	b8_t		oper!=				(cnst SGUIZoom & other)		csnx	{ return !oper==(other); }
	};


	struct SGUI {
		n2f2_t				CursorPos			= {};
		SControlTable		Controls			= {};
		pobj<SControlColors>	Colors;
		// Font
		apobj<SRasterFont>	Fonts				= {};
		SGUIZoom			LastZoom			= {};
		SGUIZoom			Zoom				= {};
		u1_t				SelectedFont		= 7;
		u1_t				ThemeDefault		= ASCII_COLOR_Cyan * 16 + 8;
		GUI_COLOR_MODE		ColorModeDefault	= GUI_COLOR_MODE_3D;//GUI_COLOR_MODE_DEFAULT;
		b8_t				InputModeTouch		= true;
	};

	cid_t 				controlCreate					(SGUI & gui);
	cid_t				controlCreateChild				(SGUI & gui, cid_t iParent);
	err_t				controlDrawHierarchy			(SGUI & gui, cid_t iControl, g8bgra target);
	err_t				controlUpdateMetricsTopToDown	(SGUI & gui, cid_t iControl, cnst n2u1_t & targetSize, b8_t forceUpdate);
	err_t				controlUpdateHierarchy			(SGUI & guigui, cid_t iControl, n2u1_c & targetMetrics);
	err_t				controlDrawHierarchy			(SGUI & gui, cid_t iControl, g8bgra target);
	err_t				guiControlDraw					(SGUI & gui, cid_t iControl, g8bgra & target);

	err_t				guiSetupDefaults				(SGUI & gui);
	err_t				guiProcessInput					(SGUI & gui, cnst SInput & input, vpobj<SEventSystem> in_Events = {});
	err_t				guiUpdate						(SGUI & gui, n2u1_c & targetMetrics); //, vpobj<SEventSystem> in_Events = {});
	stin	err_t		guiUpdateMetrics				(SGUI & gui, n2u1_c & targetSize, b8_t forceUpdate) {
		for(u2_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl)
			if(controlInvalid(gui.Controls, gui.Controls.States[iControl].Parent) && false == controlInvalid(gui.Controls, (cid_t)iControl))
				if_fail_fef(controlUpdateMetricsTopToDown(gui, (cid_t)iControl, targetSize, forceUpdate), "iControl:%i", iControl);
		return 0;
	}
	stin	err_t		guiDraw							(SGUI & gui, g8bgra target)	{
		for(u2_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl)
			if(false == controlInvalid(gui.Controls, (cid_t)iControl) && controlInvalid(gui.Controls, gui.Controls.States[iControl].Parent))
				if_fail_ef(controlDrawHierarchy(gui, (cid_t)iControl, target), "iControl:%i", iControl);
		return 0;
	}
	stin	cid_t 		controlCreate					(SGUI & gui, cnst FControlInitialize & funcInitialize) {
		cid_t				iControl;
		if_fail_fe(iControl = controlCreate(gui));
		if_true_block_log(error_printf, failed(funcInitialize
			( gui.Controls.States			[iControl]  
			, gui.Controls.TextState		[iControl]  
			, gui.Controls.Identifier		[iControl]
			, gui.Controls.Constraints		[iControl]  
			//, gui.Controls.Previous			[iControl]  
			, gui.Controls.Placement		[iControl]  
			, gui.Controls.Area				[iControl]  
			, gui.Controls.Draw				[iControl]  
			, gui.Controls.Text				[iControl]  
			, gui.Controls.Images			[iControl]  
			)), {
				controlDelete(gui.Controls, iControl);
				return -1;
			}
		);
		return iControl;
	}
	//stin	err_t		controlHidden					(cnst SGUI & gui, cid_t iControl)					{ rtrn controlHidden   (gui.Controls, iControl); }
	//stin	err_t		controlDisabled					(cnst SGUI & gui, cid_t iControl)					{ rtrn controlDisabled (gui.Controls, iControl); }
	//stin	err_t		controlInvalid					(cnst SGUI & gui, cid_t iControl)					{ rtrn controlInvalid  (gui.Controls, iControl); }
	//stin	cid_t		controlParent					(cnst SGUI & gui, cid_t iControl)					{ rtrn controlParent   (gui.Controls, iControl); }
	//stin	err_t		controlMetricsInvalidate		(SGUI & gui, cid_t iControl)						{ rtrn controlMetricsInvalidate (gui.Controls, iControl); }
	//stin	err_t		controlDelete					(SGUI & gui, cid_t iControl, b8_t recursive = true)	{ rtrn controlDelete   (gui.Controls, iControl, recursive); }
	//stin	err_t		controlSetParent				(SGUI & gui, cid_t iControl, cid_t	iParent)		{ rtrn controlSetParent(gui.Controls, iControl, iParent); }
	//stin	err_t		controlFontSet					(SGUI & gui, cid_t iControl, s1_t	iFont)			{ rtrn controlFontSet  (gui.Controls, iControl, iFont); }
	//stin	err_t		controlTextSet					(SGUI & gui, cid_t iControl, vcst_t	text)			{ rtrn controlTextSet  (gui.Controls, iControl, text); }
	//stin	err_t		controlToFront					(SGUI & gui, cid_t iControl)						{ rtrn controlToFront  (gui.Controls, iControl); }
	//stin	err_t		guiSelect						(SGUI & gui, cid_t controlToSelect)					{ rtrn controlSelect   (gui.Controls, controlToSelect); }
	//stin	err_t		guiDeselect						(SGUI & gui)										{ rtrn controlDeselect (gui.Controls); }
	//stin	err_t		guiGetProcessableControls		(cnst SGUI & gui, acid & controlIndices)														{ rtrn controlActions(gui.Controls, controlIndices)	; }		
	//stin	err_t		guiProcessControls				(cnst SGUI & gui, cnst function<err_t(cid_t iControl)> & funcOnAction)							{ rtrn controlHandle(gui.Controls, funcOnAction)	; }			
	//stin	err_t		guiProcessControls				(cnst SGUI & gui, vcid controlsToProcess, cnst function<err_t(cid_t iControl)> & funcOnAction)	{ rtrn controlHandle(gui.Controls, controlsToProcess, funcOnAction)	; }		

} // nmsp

#endif // GPK_GUI_H_23627
