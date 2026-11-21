#include "gpk_gui_control.h"
#include "gpk_gui_control_flag.h"
#include "gpk_gui_text_flag.h"
#include "gpk_gui_image_flag.h"

#include "gpk_font.h"
#include "gpk_img_color.h"

#ifndef GPK_GUI_CONTROL_TABLE_H
#define GPK_GUI_CONTROL_TABLE_H

nmsp gpk
{
    struct SControlTable {
        n2u1_t						LastSize			= {};
        apod<SControlState		>	States				= {};
        aobj<apod<STextState>	>	TextState			= {};
        aobj<vcst_t				>	Identifier			= {};
        aobj<string				>	Script				= {};
        apod<SControlConstraints>	Constraints			= {};
        apod<SControlPlacement	>	Placement			= {};
        apod<SControlArea		>	Area				= {};
        apod<SControlDraw		>	Draw				= {};
        aobj<SControlText		>	Text				= {};
        aobj<SControlImage		>	Images				= {};
        aobj<acid				>	Children			= {};
        //
        apobj<SEventSystem		>	EventQueue			= {};
        //
        apobj<rtbgra8d32		>	Temp				= {};
        aobj<g8bgra				>	Views				= {};

        err_t			Save					(au0_t & output) const;
        // Identifier and Text views borrow input bytes; keep them alive for this table's lifetime. Images retain their separate storage.
        err_t			Load					(vcu0_t & input);

        inln	b8_t	SetUnused				(cid_t iControl, b8_t value)	nxpt	{ return States[iControl].SetUnused  (iControl, EventQueue, value); }
        inln	b8_t	SetHidden				(cid_t iControl, b8_t value)	nxpt	{ return States[iControl].SetHidden  (iControl, EventQueue, value); }
        inln	b8_t	SetDisabled				(cid_t iControl, b8_t value)	nxpt	{ return States[iControl].SetDisabled(iControl, EventQueue, value); }
        inln	b8_t	SetHovered				(cid_t iControl, b8_t value)	nxpt	{ return States[iControl].SetHovered (iControl, EventQueue, value); }
        inln	b8_t	SetFocused				(cid_t iControl, b8_t value)	nxpt	{ return States[iControl].SetFocused (iControl, EventQueue, value); }
        inln	b8_t	SetPressed				(cid_t iControl, b8_t value)	nxpt	{ return States[iControl].SetPressed (iControl, EventQueue, value); }
        inln	b8_t	SetDragging				(cid_t iControl, b8_t value)	nxpt	{ return States[iControl].SetDragging(iControl, EventQueue, value); }
        inln	b8_t	SetSelected				(cid_t iControl, b8_t value)	nxpt	{ return States[iControl].SetSelected(iControl, EventQueue, value); }
        inln	b8_t	SetUpdated				(cid_t iControl, b8_t value)	nxpt	{ return States[iControl].SetUpdated (iControl, EventQueue, value); }
        inln	b8_t	SetAction				(cid_t iControl, b8_t value)	nxpt	{ return States[iControl].SetAction  (iControl, EventQueue, value); }

        inln	b8_t	IsUnused				(cid_t iControl)				csnx	{ return States[iControl].IsUnused   (); }
        inln	b8_t	IsHidden				(cid_t iControl)				csnx	{ return States[iControl].IsHidden   (); }
        inln	b8_t	IsDisabled				(cid_t iControl)				csnx	{ return States[iControl].IsDisabled (); }
        inln	b8_t	IsHovered				(cid_t iControl)				csnx	{ return States[iControl].IsHovered  (); }
        inln	b8_t	IsFocused				(cid_t iControl)				csnx	{ return States[iControl].IsFocused  (); }
        inln	b8_t	IsPressed				(cid_t iControl)				csnx	{ return States[iControl].IsPressed  (); }
        inln	b8_t	IsDragging				(cid_t iControl)				csnx	{ return States[iControl].IsDragging (); }
        inln	b8_t	IsSelected				(cid_t iControl)				csnx	{ return States[iControl].IsSelected (); }
        inln	b8_t	IsUpdated				(cid_t iControl)				csnx	{ return States[iControl].IsUpdated  (); }
        inln	b8_t	IsAction				(cid_t iControl)				csnx	{ return States[iControl].IsAction   (); }
    };
    err_t			controlHidden            (cnst SControlTable & controls, cid_t iControl);
    err_t			controlDisabled          (cnst SControlTable & controls, cid_t iControl);
    err_t			controlInvalid           (cnst SControlTable & controls, cid_t iControl);
    err_t			controlActions           (cnst SControlTable & controls, acid & controlIndices);
    err_t			controlHandle            (cnst SControlTable & controls, cnst function<err_t(cid_t iControl)> & funcOnExecute);
    err_t			controlHandle            (cnst SControlTable & controls, vcid controlsToProcess, cnst function<err_t(cid_t iControl)> & funcOnExecute);
    ndsi	cid_t	controlParent            (cnst SControlTable & controls, cid_t iControl)	{ return controlInvalid(controls, iControl) ? CID_INVALID : controls.States[iControl].Parent; }
    ndsi	err_t	controlAction            (cnst SControlTable & controls) {
        acid			controlIndices;
        if_fail_fe(controlActions(controls, controlIndices));
        rtrn controlIndices.size() ? controlIndices[0] : 0;
    }
    cid_t			controlCreate               (SControlTable & controls, vs1_t defaultColors);
    err_t			controlDeselect             (SControlTable & controls);
    err_t			controlSelect               (SControlTable & controls, cid_t controlToSelect);
    err_t			controlDelete               (SControlTable & controls, cid_t iControl, b8_t recursive = true);
    err_t			controlSetParent            (SControlTable & controls, cid_t iControl, cid_t iParent);
    err_t			controlTextSet              (SControlTable & controls, cid_t iControl, vcst_t text);
    err_t			controlFontSet              (SControlTable & controls, cid_t iControl, s1_t iFont);
    err_t			controlMetricsInvalidate    (SControlTable & controls, cid_t iControl);
    err_t			controlUpdateHierarchy      (SControlTable & controls, cid_t iControl, n2u1_c & targetMetrics);
    err_t			controlInstanceReset        (SControlTable & controlTable, cid_t iControl, vs1_t defaultColors);
    ndsi	err_t	controlToFront              (SControlTable & controls, cid_t iControl) {
        cid_t			iParent		= controlParent(controls, iControl);
        if_fail_fe(controlSetParent(controls, iControl, -1));
        if_fail_fe(controlSetParent(controls, iControl, iParent));
        return 0;
    }
    stct SControlColors {
        papod<bgra>				Palette					= {};
        papod<SControlTheme>	ControlThemes			= {};

        asts1_t<GUI_CONTROL_PALETTE_COUNT>	DefaultColors			= {};

        apod<asttc<bgra, UI_CONTROL_AREA_COUNT>>
                                Palettes;
    };


	tydf	FError
		< SControlState			&
		, apod<STextState>		&
		, vcst_t				&
		, SControlConstraints	&
		, SControlPlacement		&
		, SControlArea			&
		, SControlDraw			&
		, SControlText			&
		, SControlImage			&
		>				FControlInitialize;
    //stin	err_t		withControlState		(SControlTable & controls, cid_t idControl, cnst FError<SControlState		&> & funcWithControl) { rtrn funcWithControl(controls.States		[idControl]); }
    //stin	err_t		withControlPrevious		(SControlTable & controls, cid_t idControl, cnst FError<SControlState		&> & funcWithControl) { rtrn funcWithControl(controls.Previous		[idControl]); }
    //stin	err_t		withControlPlacement	(SControlTable & controls, cid_t idControl, cnst FError<SControlPlacement	&> & funcWithControl) { rtrn funcWithControl(controls.Placement		[idControl]); }
    //stin	err_t		withControlArea			(SControlTable & controls, cid_t idControl, cnst FError<SControlArea		&> & funcWithControl) { rtrn funcWithControl(controls.Area			[idControl]); }
    //stin	err_t		withControlConstraints	(SControlTable & controls, cid_t idControl, cnst FError<SControlConstraints	&> & funcWithControl) { rtrn funcWithControl(controls.Constraints	[idControl]); }
    //stin	err_t		withControlDraw			(SControlTable & controls, cid_t idControl, cnst FError<SControlDraw		&> & funcWithControl) { rtrn funcWithControl(controls.Draw			[idControl]); }
    //stin	err_t		withControlText			(SControlTable & controls, cid_t idControl, cnst FError<SControlText		&> & funcWithControl) { rtrn funcWithControl(controls.Text			[idControl]); }
    //stin	err_t		withControlImages		(SControlTable & controls, cid_t idControl, cnst FError<SControlImage		&> & funcWithControl) { rtrn funcWithControl(controls.Images		[idControl]); }
    //stin	err_t		withControlChildren		(SControlTable & controls, cid_t idControl, cnst FError<acid				&> & funcWithControl) { rtrn funcWithControl(controls.Children		[idControl]); }
    //stin	err_t		withControlChildren		(SControlTable & controls, cid_t idControl, cnst FError<cid_t				&> & funcWithNesting) {
    //	rtrn withControlChildren(controls, idControl, [&](acid & nested) {
    //		return funcWithNesting(nested[idControl]);
    //	});
    //}
    //stin	err_t		withControlState		(cnst SControlTable & controls, cid_t idControl, cnst FError<cnst SControlState			&> & funcWithControl) { rtrn funcWithControl(controls.States		[idControl]); }
    //stin	err_t		withControlPrevious		(cnst SControlTable & controls, cid_t idControl, cnst FError<cnst SControlState			&> & funcWithControl) { rtrn funcWithControl(controls.Previous		[idControl]); }
    //stin	err_t		withControlPlacement	(cnst SControlTable & controls, cid_t idControl, cnst FError<cnst SControlPlacement		&> & funcWithControl) { rtrn funcWithControl(controls.Placement		[idControl]); }
    //stin	err_t		withControlArea			(cnst SControlTable & controls, cid_t idControl, cnst FError<cnst SControlArea			&> & funcWithControl) { rtrn funcWithControl(controls.Area			[idControl]); }
    //stin	err_t		withControlConstraints	(cnst SControlTable & controls, cid_t idControl, cnst FError<cnst SControlConstraints	&> & funcWithControl) { rtrn funcWithControl(controls.Constraints	[idControl]); }
    //stin	err_t		withControlDraw			(cnst SControlTable & controls, cid_t idControl, cnst FError<cnst SControlDraw			&> & funcWithControl) { rtrn funcWithControl(controls.Draw			[idControl]); }
    //stin	err_t		withControlText			(cnst SControlTable & controls, cid_t idControl, cnst FError<cnst SControlText			&> & funcWithControl) { rtrn funcWithControl(controls.Text			[idControl]); }
    //stin	err_t		withControlImages		(cnst SControlTable & controls, cid_t idControl, cnst FError<cnst SControlImage			&> & funcWithControl) { rtrn funcWithControl(controls.Images		[idControl]); }
    //stin	err_t		withControlChildren		(cnst SControlTable & controls, cid_t idControl, cnst FError<cnst acid					&> & funcWithControl) { rtrn funcWithControl(controls.Children		[idControl]); }
    //stin	err_t		withControlChildren		(cnst SControlTable & controls, cid_t idControl, cnst FError<cnst cid_t					&> & funcWithNesting) {
    //	rtrn withControlChildren(controls, idControl, [&](cnst acid & nested) {
    //		return funcWithNesting(nested[idControl]);
    //	});
    //}
} // nmsp 

#endif // GPK_GUI_CONTROL_TABLE_H
