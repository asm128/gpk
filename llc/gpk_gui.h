#include "gpk_gui_control.h"
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

	struct SGUIColors {
		papod<bgra>				Palette					= {};
		papod<SControlTheme>	ControlThemes			= {};

		asts1_t<GUI_CONTROL_PALETTE_COUNT>	DefaultColors			= {};

		apod<asttc<bgra, UI_CONTROL_AREA_COUNT>>
											Palettes;
	};
	
	struct SGUI {
		n2u1_t				LastSize			= {};
		n2f2_t				CursorPos			= {};
		SControlTable		Controls			= {};
		pobj<SGUIColors>	Colors;
		// Font
		apobj<SRasterFont>	Fonts				= {};
		SGUIZoom			LastZoom			= {};
		SGUIZoom			Zoom				= {};
		u1_t				SelectedFont		= 7;
		u1_t				ThemeDefault		= ASCII_COLOR_CYAN * 16 + 8;
		GUI_COLOR_MODE		ColorModeDefault	= GUI_COLOR_MODE_3D;//GUI_COLOR_MODE_DEFAULT;
		b8_t				InputModeTouch		= true;
	};

	err_t				guiProcessInput					(SGUI & gui, cnst SInput & input, vpobj<SEventSystem> in_Events = {});
	err_t				guiUpdate						(SGUI & gui, cnst SInput & input, cnst n2u1_t & targetMetrics, vpobj<SEventSystem> in_Events = {});
	err_t				guiUpdateMetrics				(SGUI & gui, cnst n2u1_t & targetSize, b8_t forceUpdate);
	err_t				guiDraw							(SGUI & gui, cnst g8bgra & target);
	cid_t 				controlCreate					(SGUI & gui);
	cid_t				controlCreateChild				(SGUI & gui, cid_t iParent);
	err_t				controlDrawHierarchy			(SGUI & gui, cid_t iControl, g8bgra target);
	//err_t				controlUpdateMetrics			(SGUI & gui, s2_t iControl, cnst n2<u2_t> & targetSize);
	err_t				controlUpdateMetricsTopToDown	(SGUI & gui, cid_t iControl, cnst n2u1_t & targetSize, b8_t forceUpdate);

	cid_t				guiCreateControlList			(SGUI & gui, DIRECTION direction, view<vcst_c> buttonText, cid_t iParent, cnst SControlPlacement & args, ALIGN textAlign, vcid out_ids);
	cid_t				guiCreateControlList			(SGUI & gui, view<vcst_c> buttonText, cid_t iParent, cnst n2u1_t & buttonSize, n2s1_c & offset, ALIGN controlAlign, ALIGN textAlign, vcid out_ids);
	stin	cid_t		guiCreateControlList			(SGUI & gui, DIRECTION direction, view<vcst_c> buttonText, cid_t iParent, cnst SControlPlacement & args, ALIGN textAlign, acid & out_ids)									{ 
		u2_c offset = out_ids.size();
		if_fail_fe(out_ids.resize(offset + buttonText.size()));
		return guiCreateControlList(gui, direction, buttonText, iParent, args, textAlign, vcid{out_ids.begin() + offset, buttonText.size()}); 
	}
	tplt<tpnm _tUIEnum>
	stin	cid_t		guiCreateControlList			(SGUI & gui, DIRECTION direction, cid_t iParent, cnst SControlPlacement & args, ALIGN textAlign, acid & out_ids) {
		return guiCreateControlList(gui, direction, get_value_labels<_tUIEnum>(), iParent, args, textAlign, out_ids);
	}
	tplt<tpnm _tUIEnum>
	stin	cid_t		guiCreateControlList			(SGUI & gui, cid_t iParent, cnst n2u1_t & buttonSize, n2s1_c & offset, ALIGN controlAlign, ALIGN textAlign, vcid out_ids) {
		return guiCreateControlList(gui, get_value_labels<_tUIEnum>(), iParent, buttonSize, offset, controlAlign, textAlign, out_ids);
	}
	stin	cid_t		guiCreateControlList			(SGUI & gui, view<vcst_c> buttonText, cid_t iParent, cnst n2u1_t & buttonSize, n2s1_c & offset, ALIGN controlAlign, ALIGN textAlign, acid & out_ids)	{ 
		u2_c offsetId = out_ids.size();
		out_ids.resize(offsetId + buttonText.size()); 
		return guiCreateControlList(gui, buttonText, iParent, buttonSize, offset, controlAlign, textAlign, vcid{out_ids.begin() + offsetId, buttonText.size()}); 
	}
	tplt<tpnm _tUIEnum>
	stin	cid_t		guiCreateControlList			(SGUI & gui, cid_t iParent, u1_t buttonWidth, s1_t yOffset, ALIGN controlAlign, ALIGN textAlign, acid & out_ids) {
		return guiCreateControlList(gui, get_value_labels<_tUIEnum>(), iParent, {buttonWidth, 20}, {0, yOffset}, controlAlign, textAlign, out_ids);
	}
	tplt<tpnm _tUIEnum>
	stin	cid_t		guiCreateControlList			(SGUI & gui, cid_t iParent, u1_t buttonWidth, n2s1_c & offset, ALIGN controlAlign, ALIGN textAlign, acid & out_ids) {
		return guiCreateControlList(gui, get_value_labels<_tUIEnum>(), iParent, {buttonWidth, 20}, offset, controlAlign, textAlign, out_ids);
	}
	tplt<tpnm _tUIEnum>
	stin	cid_t		guiCreateControlList			(SGUI & gui, cid_t iParent, cnst n2u1_t & buttonSize, n2s1_c & offset, ALIGN controlAlign, ALIGN textAlign, acid & out_ids) {
		return guiCreateControlList(gui, get_value_labels<_tUIEnum>(), iParent, buttonSize, offset, controlAlign, textAlign, out_ids);
	} 

	cid_t				createScreenLayout				(SGUI & gui, cid_t parent = -1);
	tplt<tpnm _tEnum>
	err_t				setupControlListWithRoot		(SGUI & gui, acid & outIds, cid_t parent, cnst n2u1_t & buttonSize, ALIGN controlAlign, ALIGN textAlign = ALIGN_CENTER) {
		cid_t					root							= outIds[outIds.push_back(createScreenLayout(gui, parent))];
		view<vcst_c>			labels							= {get_value_labels<_tEnum>()};
		if_fail_fe(labels.slice(labels, 1));
		return guiCreateControlList(gui, labels, root, buttonSize, {}, controlAlign, textAlign, outIds);
	}

	tydf	FError
		< SControlState			&
		, STextState			&
		, vcst_t				&
		, SControlConstraints	&
		, SControlPlacement		&
		, SControlArea			&
		, SControlDraw			&
		, SControlText			&
		, SControlImage			&
		>				FControlInitialize;
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
	stin	err_t		controlHidden					(cnst SGUI & gui, cid_t iControl)					{ rtrn controlHidden   (gui.Controls, iControl); }
	stin	err_t		controlDisabled					(cnst SGUI & gui, cid_t iControl)					{ rtrn controlDisabled (gui.Controls, iControl); }
	stin	err_t		controlInvalid					(cnst SGUI & gui, cid_t iControl)					{ rtrn controlInvalid  (gui.Controls, iControl); }
	stin	cid_t		controlParent					(cnst SGUI & gui, cid_t iControl)					{ rtrn controlParent   (gui.Controls, iControl); }
	stin	err_t		controlMetricsInvalidate		(SGUI & gui, cid_t iControl)						{ rtrn controlMetricsInvalidate (gui.Controls, iControl); }
	stin	err_t		controlDelete					(SGUI & gui, cid_t iControl, b8_t recursive = true)	{ rtrn controlDelete   (gui.Controls, iControl, recursive); }
	stin	err_t		controlSetParent				(SGUI & gui, cid_t iControl, cid_t	iParent)		{ rtrn controlSetParent(gui.Controls, iControl, iParent); }
	stin	err_t		controlFontSet					(SGUI & gui, cid_t iControl, s1_t	iFont)			{ rtrn controlFontSet  (gui.Controls, iControl, iFont); }
	stin	err_t		controlTextSet					(SGUI & gui, cid_t iControl, vcst_t	text)			{ rtrn controlTextSet  (gui.Controls, iControl, text); }
	stin	err_t		controlToFront					(SGUI & gui, cid_t iControl)						{ rtrn controlToFront  (gui.Controls, iControl); }
	stin	err_t		guiSelect						(SGUI & gui, cid_t controlToSelect)					{ rtrn controlSelect   (gui.Controls, controlToSelect); }
	stin	err_t		guiDeselect						(SGUI & gui)										{ rtrn controlDeselect (gui.Controls); }
	stin	err_t		guiGetProcessableControls		(cnst SGUI & gui, acid & controlIndices)														{ rtrn controlActions(gui.Controls, controlIndices)	; }		
	stin	err_t		guiProcessControls				(cnst SGUI & gui, cnst function<err_t(cid_t iControl)> & funcOnAction)							{ rtrn controlHandle(gui.Controls, funcOnAction)	; }			
	stin	err_t		guiProcessControls				(cnst SGUI & gui, vcid controlsToProcess, cnst function<err_t(cid_t iControl)> & funcOnAction)	{ rtrn controlHandle(gui.Controls, controlsToProcess, funcOnAction)	; }		

	//stin	err_t	withControlState		(cnst SGUI & gui, cid_t idControl, cnst FError<cnst	 SControlState			&> & funcWithControl) { rtrn withControlState		(gui.Controls, idControl, funcWithControl); }
	//stin	err_t	withControlPrevious		(cnst SGUI & gui, cid_t idControl, cnst FError<cnst	 SControlState			&> & funcWithControl) { rtrn withControlPrevious	(gui.Controls, idControl, funcWithControl); }
	//stin	err_t	withControlPlacement	(cnst SGUI & gui, cid_t idControl, cnst FError<cnst	 SControlPlacement		&> & funcWithControl) { rtrn withControlPlacement	(gui.Controls, idControl, funcWithControl); }
	//stin	err_t	withControlArea			(cnst SGUI & gui, cid_t idControl, cnst FError<cnst	 SControlArea			&> & funcWithControl) { rtrn withControlArea		(gui.Controls, idControl, funcWithControl); }
	//stin	err_t	withControlConstraints	(cnst SGUI & gui, cid_t idControl, cnst FError<cnst	 SControlConstraints	&> & funcWithControl) { rtrn withControlConstraints	(gui.Controls, idControl, funcWithControl); }
	//stin	err_t	withControlDraw			(cnst SGUI & gui, cid_t idControl, cnst FError<cnst	 SControlDraw			&> & funcWithControl) { rtrn withControlDraw		(gui.Controls, idControl, funcWithControl); }
	//stin	err_t	withControlText			(cnst SGUI & gui, cid_t idControl, cnst FError<cnst	 SControlText			&> & funcWithControl) { rtrn withControlText		(gui.Controls, idControl, funcWithControl); }
	//stin	err_t	withControlImages		(cnst SGUI & gui, cid_t idControl, cnst FError<cnst	 SControlImage			&> & funcWithControl) { rtrn withControlImages		(gui.Controls, idControl, funcWithControl); }
	//stin	err_t	withControlChildren		(cnst SGUI & gui, cid_t idControl, cnst FError<cnst	 acid					&> & funcWithControl) { rtrn withControlChildren	(gui.Controls, idControl, funcWithControl); }
	//stin	err_t	withControlChildren		(cnst SGUI & gui, cid_t idControl, cnst FError<cnst	 cid_t					&> & funcWithNesting) { rtrn withControlChildren	(gui.Controls, idControl, funcWithNesting); }

	//stin	err_t	withControlState		(SGUI & gui, cid_t idControl, cnst FError<SControlState			&> & funcWithControl) { rtrn withControlState		(gui.Controls, idControl, funcWithControl); }
	//stin	err_t	withControlPrevious		(SGUI & gui, cid_t idControl, cnst FError<SControlState			&> & funcWithControl) { rtrn withControlPrevious	(gui.Controls, idControl, funcWithControl); }
	//stin	err_t	withControlPlacement	(SGUI & gui, cid_t idControl, cnst FError<SControlPlacement		&> & funcWithControl) { rtrn withControlPlacement	(gui.Controls, idControl, funcWithControl); }
	//stin	err_t	withControlArea			(SGUI & gui, cid_t idControl, cnst FError<SControlArea			&> & funcWithControl) { rtrn withControlArea		(gui.Controls, idControl, funcWithControl); }
	//stin	err_t	withControlConstraints	(SGUI & gui, cid_t idControl, cnst FError<SControlConstraints	&> & funcWithControl) { rtrn withControlConstraints	(gui.Controls, idControl, funcWithControl); }
	//stin	err_t	withControlDraw			(SGUI & gui, cid_t idControl, cnst FError<SControlDraw			&> & funcWithControl) { rtrn withControlDraw		(gui.Controls, idControl, funcWithControl); }
	//stin	err_t	withControlText			(SGUI & gui, cid_t idControl, cnst FError<SControlText			&> & funcWithControl) { rtrn withControlText		(gui.Controls, idControl, funcWithControl); }
	//stin	err_t	withControlImages		(SGUI & gui, cid_t idControl, cnst FError<SControlImage			&> & funcWithControl) { rtrn withControlImages		(gui.Controls, idControl, funcWithControl); }
	//stin	err_t	withControlChildren		(SGUI & gui, cid_t idControl, cnst FError<acid					&> & funcWithControl) { rtrn withControlChildren	(gui.Controls, idControl, funcWithControl); }
	//stin	err_t	withControlChildren		(SGUI & gui, cid_t idControl, cnst FError<cid_t					&> & funcWithNesting) { rtrn withControlChildren	(gui.Controls, idControl, funcWithNesting); }

} // nmsp

#endif // GPK_GUI_H_23627
