#include "gpk_gui_control.h"


gpk::err_t 	gpk::controlTextSet		(SControlTable & controls, cid_t iControl, vcst_t text)				{
	vcst_t			& oldText				= controls.Text[iControl].Text;
	if(text == oldText)
		rtrn 0;
	oldText		= text;
	rtrn one_if(controls.SetUpdated(iControl, false));
}

gpk::err_t 	gpk::controlFontSet		(SControlTable & controls, cid_t iControl, s1_t iFont)				{
	s1_t			& oldFont				= controls.Text[iControl].FontSelected;
	if(iFont == oldFont)
		rtrn 0;
	oldFont		= iFont;
	rtrn one_if(controls.SetUpdated(iControl, false));
}

gpk::err_t 	gpk::controlDeselect		(SControlTable & controls)	{
	for(u2_t iControl = 0; iControl < controls.States.size(); ++iControl)
		controls.SetSelected((cid_t)iControl, false);

	return 0;
}

gpk::err_t 		gpk::controlSelect		(SControlTable & controls, cid_t controlToSelect)	{
	if_true_fe(controlInvalid(controls, controlToSelect));
	cnst cid_t parent = controls.States[controlToSelect].Parent;
	for(u2_t iControl = 0; iControl < controls.States.size(); ++iControl)
		if(controls.States[iControl].Parent == parent)
			controls.SetSelected((cid_t)iControl, controlToSelect == (cid_t)iControl);

	return 0;
}

gpk::err_t 		gpk::controlHidden		(cnst SControlTable & controls, cid_t iControl)	{
	b8_t						imHidden				= controlInvalid(controls, iControl) || controls.States[iControl].IsHidden();
	return imHidden ? imHidden : (0 == controlInvalid(controls, controls.States[iControl].Parent) && controlHidden(controls, controls.States[iControl].Parent));
}

gpk::err_t 		gpk::controlDisabled	(cnst SControlTable & controls, cid_t iControl)	{
	b8_t						imDisabled				= controlInvalid(controls, iControl) || controls.States[iControl].IsDisabled();
	return imDisabled ? imDisabled : (false == controlInvalid(controls, controls.States[iControl].Parent) && controlDisabled(controls, controls.States[iControl].Parent));
}

gpk::err_t 		gpk::controlMetricsInvalidate	(SControlTable & controls, cid_t iControl)	{
	controls.SetUpdated(iControl, false);
	cnst vcid			& controlChildren		= controls.Children[iControl];
	for(u2_t iChild = 0, countChild = controlChildren.size(); iChild < countChild; ++iChild)
		if_fail_fef(gpk::controlMetricsInvalidate(controls, controlChildren[iChild]), "%s", "Invalid child?");

	return 0;
}

gpk::err_t 		gpk::controlInvalid			(cnst SControlTable & controls, cid_t iControl)				{
	if(controls.States.size() <= u2_t(iControl)	) 
		return -1;
	if(controls.IsUnused(iControl)) 
		return -1;
	return 0;
}

gpk::err_t 		gpk::controlDelete		(SControlTable & controls, cid_t iControl, b8_t recursive)			{
	if_fail_fef(controlInvalid(controls, iControl), "Invalid control id: %u.", iControl);
	info_printf("Deleting control id: %i.", iControl);
	if(recursive) { // Delete children first.
		acid					children				= controls.Children[iControl];
		for(u2_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
			if_true_ef(failed(controlDelete(controls, children[iChild], true)), "Failed to delete control! Invalid control id? %i.", children[iChild]);
	}
	// Remove from parent list
	cnst cid_t			indexParent				= controls.States[iControl].Parent;
#if defined(GPK_DEBUG_ENABLED)
	u2_t					childrenRemoved			= 0;
#endif
	if((u2_t)indexParent < controls.States.size() && false == controls.IsUnused(indexParent)) {
		acid					& children				= controls.Children[indexParent];
		for(u2_t iChild = 0; iChild < children.size(); ++iChild)
			if(children[iChild] == iControl) {
				if_fail_fef(children.remove_unordered(iChild), "Failed to remove child at index: %u.", iChild);
#if defined(GPK_DEBUG_ENABLED)
				--iChild;
				++childrenRemoved;
#else
				break;
#endif
			}
	}
#if defined(GPK_DEBUG_ENABLED)
	if_true_ef(childrenRemoved > 1, "Parent should not reference a child control more than once. References found: %i.", childrenRemoved);
#endif
	controls.SetUnused(iControl, true);
	return 0;
}

gpk::err_t		gpk::controlSetParent	(SControlTable & controls, cid_t iControl, cid_t iParent)	{
	if_fail_fef(controlInvalid(controls, iControl), "Invalid control id: %u.", iControl);
	cnst cid_t			oldParent				= controls.States[iControl].Parent;
	SControlState		& control				= controls.States[iControl];
	if(control.Parent == iParent)	// Exit early if there is nothing to do here.
		return 0;

	control.Parent		= iParent;
	if((u2_t)oldParent < controls.States.size() && false == controls.IsUnused(oldParent)) {
		acid					& children				= controls.Children[oldParent];
		for(s2_t iChild = 0; iChild < (s2_t)children.size(); ++iChild)
			if(children[iChild] == iControl) {
				controls.SetUpdated(oldParent, false);
				if_fail_fef(children.remove(iChild), "Failed to remove child at index: %u.", iChild);
				break;
			}
	}
	if(controlInvalid(controls, iParent)) {
		if_true_vef(-1, -1 != iParent, "Invalid parent control id: %u.", iParent);
		return 0;
	}

	// Set iControl to parent's children array.
	acid					& children				= controls.Children[iParent];
	for(s2_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
		if(children[iChild] == iControl)
			return 0;

	controls.SetUpdated(iParent , false);
	controls.SetUpdated(iControl, false);
	if_fail_fe(children.push_back(iControl));
	return 0;
}

gpk::err_t 		gpk::controlActions	(cnst SControlTable & controls, acid & controlIndices)													{
	for(u2_t iControl = 0, countControls = controls.States.size(); iControl < countControls; ++iControl) {	// Only process root parents
		cnst SControlState	& controlState					= controls.States[iControl];
		if(controlState.IsUnused() || controlDisabled(controls, (cid_t)iControl) || controlHidden(controls, (cid_t)iControl))
			continue;
		if_fail_fe(controlIndices.push_back((cid_t)iControl));
	}
	return controlIndices.size();
}

gpk::err_t 		gpk::controlHandle		(cnst SControlTable & controls, cnst function<err_t(cid_t iControl)> & funcOnExecute) {
	acid					controlsToProcess			= {};
	if_fail_fe(controlActions(controls, controlsToProcess));
	return controlHandle(controls, controlsToProcess, funcOnExecute);
}

gpk::err_t 		gpk::controlHandle		(cnst SControlTable & controls, vcid controlsToProcess, cnst function<err_t(cid_t iControl)> & funcOnExecute) {
	for(u2_t iControl = 0, countControls = controlsToProcess.size(); iControl < countControls; ++iControl) {
		cnst cid_t			idControl					= controlsToProcess[iControl];
		cnst SControlState	& controlState				= controls.States[idControl];
		if(controlState.IsAction()) {
			info_printf("Executed control %i (0x%x).", idControl, idControl);
			return funcOnExecute(idControl);
		}
	}
	return 0;
}
gpk::err_t gpk::controlInstanceReset	(SControlTable & controlTable, cid_t iControl, vs1_t defaultColors)										{
	controlTable.States			[iControl]	= {};
	controlTable.Previous		[iControl]	= {};
	controlTable.Placement		[iControl]	= {};
	controlTable.Area			[iControl]	= {};
	controlTable.Constraints	[iControl]	= {};
	controlTable.Draw			[iControl]	= {};
	controlTable.Text			[iControl]	= {};
	controlTable.ControlTextState[iControl].clear();
	controlTable.Images			[iControl]	= {};
	controlTable.Children		[iControl]	= vcid{};
	
	SControlDraw			& controlDraw			= controlTable.Draw[iControl];
	for(u2_t iColor = 0, countColors = min(controlDraw.Palettes.size(), defaultColors.size()); iColor < countColors; ++iColor)
		controlDraw.Palettes[iColor] = defaultColors[iColor];

	return 0;
}
gpk::cid_t			gpk::controlCreate		(SControlTable & controlTable, vs1_t defaultColors)	{
	for(u2_t iControl = 0; iControl < controlTable.States.size(); ++iControl) {
		if(controlTable.IsUnused((cid_t)iControl)) {
			if_fail_fef(controlInstanceReset(controlTable, (cid_t)iControl, defaultColors), "iControl: %i", iControl);
			return (cid_t)iControl;
		}
	}
	err_t				iControl				= -1;
	if_fail_fe(iControl = resize(controlTable.States.size() + 1
		, controlTable.States
		, controlTable.Previous
		, controlTable.Placement
		, controlTable.Area
		, controlTable.Constraints
		, controlTable.Draw
		, controlTable.Text
		, controlTable.ControlTextState
		, controlTable.Images
		, controlTable.Children
		) - 1);
	if_fail_fe(controlInstanceReset(controlTable, (cid_t)iControl, defaultColors));
	return (cid_t)iControl;
}
