#include "gpk_gui.h"
#include "gpk_gui_theme.h"
#include "gpk_gui_text.h"
#include "gpk_base64.h"
#include "gpk_rect_align.h"

usns gpk;

gpk::cid_t		gpk::controlCreateChild	(SGUI & gui, cid_t iParent)						{
	cid_t				idControl				= controlCreate(gui);
	controlSetParent(gui.Controls, idControl, iParent);
	return idControl;
}

gpk::cid_t		gpk::controlCreate		(SGUI & gui)							{
	if_fail_fe(guiSetupDefaultColors(gui.Colors));

	if(0 == gui.Fonts.size())
		if_fail_fe(guiSetupDefaultFonts(gui));

	if(gui.Fonts.size() <= gui.SelectedFont)
		gui.SelectedFont = u1_t(gui.Fonts.size() - 1);

	return controlCreate(gui.Controls, gui.Colors->DefaultColors);
}

sttc gpk::err_t	buildControlGeometry	(cnst SControlPlacement & control, cnst SControlArea & controlMetrics, cnst SGUIZoom & zoom, view<rect2s1_t> finalRects, view<tri2s1_t> triangles)					{
	rects1_t			scaledBorders			= {};
	n2f3_c				scaleFinal				= zoom.DPI * zoom.ZoomLevel;
	scaledBorders.Left		= (s1_t)(control.Border.Left		* scaleFinal.x);
	scaledBorders.Top		= (s1_t)(control.Border.Top		* scaleFinal.y);
	scaledBorders.Right		= (s1_t)(control.Border.Right	* scaleFinal.x);
	scaledBorders.Bottom	= (s1_t)(control.Border.Bottom	* scaleFinal.y);
	(void)scaledBorders;

	cnst rect2s1_t		& rectTotal				= controlMetrics.Total.Global;
	finalRects[GUI_CONTROL_AREA_OLD_CLIENT			]	= controlMetrics.Client.Global;
	finalRects[GUI_CONTROL_AREA_OLD_BORDER_LEFT		]	= {rectTotal.Offset , n2s1_t{(s1_t)control.Border.Left, rectTotal.Size.y}};
	finalRects[GUI_CONTROL_AREA_OLD_BORDER_TOP		]	= {rectTotal.Offset , n2s1_t{rectTotal.Size.x, (s1_t)control.Border.Top}};
	finalRects[GUI_CONTROL_AREA_OLD_BORDER_RIGHT	]	= {rectTotal.Offset + n2s1_t{s1_t(rectTotal.Size.x - control.Border.Right), 0} , n2s1_t{(s1_t)control.Border.Right, controlMetrics.Total.Global.Size.y}};
	finalRects[GUI_CONTROL_AREA_OLD_BORDER_BOTTOM	]	= {rectTotal.Offset + n2s1_t{0, s1_t(rectTotal.Size.y - control.Border.Bottom)}, n2s1_t{rectTotal.Size.x, (s1_t)control.Border.Bottom}};
	//finalRects[GUI_CONTROL_AREA_OLD_BACKGROUND	]	= rectTotal;
	finalRects[GUI_CONTROL_AREA_OLD_CLIENT			].Offset.x	-= control.Margin.Left	;
	finalRects[GUI_CONTROL_AREA_OLD_CLIENT			].Offset.y	-= control.Margin.Top	;
	finalRects[GUI_CONTROL_AREA_OLD_CLIENT			].Size	.x	+= control.Margin.Left	+ control.Margin.Right	;
	finalRects[GUI_CONTROL_AREA_OLD_CLIENT			].Size	.y	+= control.Margin.Top	+ control.Margin.Bottom	;

	// --- Draw control corners
	n2s1_t						startOffset				= controlMetrics.Total.Global.Offset;
	triangles[0]			= {startOffset, startOffset + n2u1_t{control.Border.Left, control.Border.Top}.s1(), startOffset + n2u1_t{control.Border.Left, 0U}.s1()	};
	triangles[1]			= {startOffset, startOffset + n2u1_t{0U, control.Border.Top}.s1(), startOffset + n2u1_t{control.Border.Left, control.Border.Top}.s1()	};

	s1_t						startOffsetX			= startOffset.x + controlMetrics.Total.Global.Size.x - control.Border.Right;
	startOffset				= {startOffsetX, controlMetrics.Total.Global.Offset.y};
	triangles[2]			= {startOffset, startOffset + n2u1_t{0U, control.Border.Top}.s1(), startOffset + n2u1_t{control.Border.Right, 0U}.s1()	};
	triangles[3]			= {startOffset + n2u1_t{control.Border.Right, 0U}.s1(), startOffset + n2u1_t{0U, control.Border.Top}.s1(), startOffset + n2u1_t{control.Border.Right, control.Border.Top}.s1()	};

	s1_t						startOffsetY			= startOffset.y + controlMetrics.Total.Global.Size.y - control.Border.Bottom;
	startOffset				= {controlMetrics.Total.Global.Offset.x, startOffsetY};
	triangles[4]			= {startOffset, startOffset + n2u1_t{0U, control.Border.Bottom}.s1(), startOffset + n2u1_t{control.Border.Left, 0U}.s1()	};
	triangles[5]			= {startOffset + n2u1_t{control.Border.Right, 0U}.s1(), startOffset + n2u1_t{0U, control.Border.Top}.s1(), startOffset + n2u1_t{control.Border.Right, control.Border.Top}.s1()	};

	startOffset				= controlMetrics.Total.Global.Offset + controlMetrics.Total.Global.Size - n2u1_t{control.Border.Right, control.Border.Bottom}.s1();
	triangles[6]			= {startOffset, startOffset + n2u1_t{control.Border.Right, control.Border.Bottom}.s1(), startOffset + n2u1_t{control.Border.Right, 0U}.s1()	};
	triangles[7]			= {startOffset, startOffset + n2u1_t{0U, control.Border.Bottom}.s1(), startOffset + n2u1_t{control.Border.Right, control.Border.Bottom}.s1()	};
	return 0;
}

sttc gpk::cid_t	controlDockTarget	(cnst SGUI & gui, cid_t idTarget, GUI_CONTROL_LIMIT side) nxpt {
	for(u2_t iControl = 0; iControl < gui.Controls.States.size() && idTarget != CID_INVALID; ++iControl) {
		if((u2_t)idTarget >= gui.Controls.States.size())
			return idTarget;
		if(false == controlHidden(gui.Controls, idTarget))
			return idTarget;
		cnst rectcid & dock = gui.Controls.Constraints[idTarget].DockToControl;
		switch(side) {
		default: return CID_INVALID;
		case GUI_CONTROL_LIMIT_Left  : idTarget = dock.Left  ; break;
		case GUI_CONTROL_LIMIT_Top   : idTarget = dock.Top   ; break;
		case GUI_CONTROL_LIMIT_Right : idTarget = dock.Right ; break;
		case GUI_CONTROL_LIMIT_Bottom: idTarget = dock.Bottom; break;
		}
	}
	return CID_INVALID;
}

sttc	gpk::err_t controlUpdateMetrics	(SGUI & gui, cid_t iControl, cnst n2<u1_t> & _targetSize)					{
	if_fail_fef(gpk::controlInvalid(gui.Controls, iControl), "Invalid control id: %u.", iControl);

	SControlState		& controlState			= gui.Controls.States[iControl];
	cnst SControlPlacement	& controlPlacement	= gui.Controls.Placement[iControl];
	n2f3_t				scale					= gui.Zoom.DPI * gui.Zoom.ZoomLevel;
	if(::gpk::fabs(1.0 - scale.x) < 0.001) scale.x = 1.0;
	if(::gpk::fabs(1.0 - scale.y) < 0.001) scale.y = 1.0;
	n2f3_t				scaledPosition			= {controlPlacement.Area.Offset	.x * scale.x, controlPlacement.Area.Offset	.y * scale.y};
	n2f3_t				scaledSize				= {controlPlacement.Area.Size	.x * scale.x, controlPlacement.Area.Size	.y * scale.y};
	//rect<f3_t>	scaledBorders		= {(control.Border.Left * scale.x), (control.Border.Top * scale.y), (control.Border.Right * scale.x), (control.Border.Bottom * scale.y)};
	//rect<f3_t>	scaledMargins		= {(control.Margin.Left * scale.x), (control.Margin.Top * scale.y), (control.Margin.Right * scale.x), (control.Margin.Bottom * scale.y)};

	SControlArea			& controlMetrics		= gui.Controls.Area[iControl];
	rect2f3_t				rectText				= {};
	SControlText			& controlText			= gui.Controls.Text[iControl];
	cnst SRasterFont	& selectedFont			= *gui.Fonts[in_range(controlText.FontSelected, (s1_t)0, (s1_t)gui.Fonts.size()) ? controlText.FontSelected : gui.SelectedFont];

	cnst n2u0_t			fontCharSize			= selectedFont.CharSize;
	rectText.Size			= {(f3_t)(fontCharSize.x * controlText.Text.size()), (f3_t)fontCharSize.y};
	rectText.Size.InPlaceScale(scale.x, scale.y);

	cnst SControlConstraints	& controlConstraints	= gui.Controls.Constraints[iControl];
	rectu1_c		ncSizes					= controlNCRect(controlPlacement);
	rectf3_c		ncSizesScaled			= {(ncSizes.Left * scale.x), (ncSizes.Top * scale.y), (ncSizes.Right * scale.x), (ncSizes.Bottom * scale.y)};
	n2f3_c			ncTotalSize				= {(f3_t)ncSizes.Left + ncSizes.Right, (f3_t)ncSizes.Top + ncSizes.Bottom};
	n2f3_c			ncTotalSizeScaled		= ncTotalSize.GetScaled(scale.x, scale.y);
	if(controlConstraints.AttachSizeToText.x) scaledSize.x	= rectText.Size.x + ncTotalSizeScaled.x;
	if(controlConstraints.AttachSizeToText.y) scaledSize.y	= rectText.Size.y + ncTotalSizeScaled.y;
	b8_c			isValidParent			= 0 == controlInvalid(gui.Controls, controlState.Parent);
	n2u1_c			targetSize				= isValidParent ? gui.Controls.Area[controlState.Parent].Client.Global.Size.u1() : _targetSize.u1();

	if(controlConstraints.AttachSizeToControl.x == iControl) { if(controlConstraints.DockToControl.Left	!= -1) {} else { scaledPosition.x = 0; } scaledSize.x = targetSize.x; } else if(false == controlInvalid(gui.Controls, controlConstraints.AttachSizeToControl.x)) { if(controlConstraints.DockToControl.Left != -1) {} else { scaledPosition.x = 0;} scaledSize.x = gui.Controls.Area[controlConstraints.AttachSizeToControl.x].Total.Global.Size.x; }
	if(controlConstraints.AttachSizeToControl.y == iControl) { if(controlConstraints.DockToControl.Top	!= -1) {} else { scaledPosition.y = 0; } scaledSize.y = targetSize.y; } else if(false == controlInvalid(gui.Controls, controlConstraints.AttachSizeToControl.y)) { if(controlConstraints.DockToControl.Top  != -1) {} else { scaledPosition.y = 0;} scaledSize.y = gui.Controls.Area[controlConstraints.AttachSizeToControl.y].Total.Global.Size.y; }

	controlMetrics.Client.Local	= {{(s1_t)ncSizesScaled.Left, (s1_t)ncSizesScaled.Top}, (scaledSize - ncTotalSizeScaled).s1()};
	n2f3_t				minSizeScaled			= (controlConstraints.SizeMinMax.Min.f3().InPlaceScale(scale.x, scale.y));
	n2f3_t				maxSizeScaled			= (controlConstraints.SizeMinMax.Max.f3().InPlaceScale(scale.x, scale.y));
	scaledSize.x			= max(minSizeScaled.x, min(maxSizeScaled.x, scaledSize.x));
	scaledSize.y			= max(minSizeScaled.y, min(maxSizeScaled.y, scaledSize.y));
	realignRectangle(targetSize.u2(), rect2<s1_t>{scaledPosition.s1(), scaledSize.s1()}, controlMetrics.Total.Local, controlPlacement.Align);
	controlMetrics.Total .Global	= controlMetrics.Total	.Local;
	controlMetrics.Client.Global	= controlMetrics.Client	.Local;
	controlMetrics.Client.Global.Offset	+= controlMetrics.Total	.Local.Offset;
	if(isValidParent) {
		SControlArea		& parentMetrics			= gui.Controls.Area[controlState.Parent];
		controlMetrics.Client.Global.Offset	+= parentMetrics.Client.Global.Offset;
		controlMetrics.Total .Global.Offset	+= parentMetrics.Client.Global.Offset;
	}

	cnst rectcid		& dockToControl			= controlConstraints.DockToControl;
	if(dockToControl.Right != -1) {
		cnst cid_t idDock = ::controlDockTarget(gui, dockToControl.Right, GUI_CONTROL_LIMIT_Right);
		if(idDock != CID_INVALID) {
		if_fail_fef(gpk::controlInvalid(gui.Controls, idDock), "Invalid control id: %i.", idDock);
		controlUpdateMetricsTopToDown(gui, idDock, targetSize.u1(), true);
		cnst SControlPlacement	& other				= gui.Controls.Placement[idDock];
		cnst SControlArea		& otherMetrics		= gui.Controls.Area	[idDock];
		if(bit_true(other.Align, ALIGN_RIGHT) && bit_false(other.Align, ALIGN_HCENTER)) {
			controlMetrics.Total .Global.Offset.x	= otherMetrics.Total.Global.Offset.x - controlMetrics.Total.Global.Size.x;
			controlMetrics.Client.Global.Offset.x	= (s1_t)(controlMetrics.Total.Global.Offset.x + ncSizesScaled.Left);
			s1_t						diffToSubstract		= targetSize.x - otherMetrics.Total.Local.Offset.x;
			if(controlConstraints.AttachSizeToControl.x == iControl) {
				controlMetrics.Total .Global.Size.x		-= diffToSubstract;
				controlMetrics.Client.Global.Size.x		-= diffToSubstract;
				controlMetrics.Total .Global.Offset.x	+= diffToSubstract;
				controlMetrics.Client.Global.Offset.x	+= diffToSubstract;
			}
		}
		else {
			controlMetrics.Total .Global.Offset.x	= otherMetrics.Total.Global.Offset.x + otherMetrics.Total.Global.Size.x;
			controlMetrics.Client.Global.Offset.x	= (s1_t)(controlMetrics.Total.Global.Offset.x + ncSizesScaled.Left);
		}
		}
	}
	if(dockToControl.Bottom != -1) {
		cnst cid_t idDock = ::controlDockTarget(gui, dockToControl.Bottom, GUI_CONTROL_LIMIT_Bottom);
		if(idDock != CID_INVALID) {
		if_fail_fef(gpk::controlInvalid(gui.Controls, idDock), "Invalid control id: %i.", idDock);
		cnst SControlPlacement	& other					= gui.Controls.Placement[idDock];
		cnst SControlArea		& otherMetrics			= gui.Controls.Area[idDock];
		if(bit_true(other.Align, ALIGN_BOTTOM	) && bit_false(other.Align, ALIGN_VCENTER)) {
			controlMetrics.Total.Global.Offset.y	= otherMetrics.Total.Global.Offset.y - controlMetrics.Total.Global.Size.y;
			controlMetrics.Client.Global.Offset.y	= (s1_t)(controlMetrics.Total.Global.Offset.y + ncSizesScaled.Top);
		} else {
			controlMetrics.Total.Global.Offset.y	= otherMetrics.Total.Global.Offset.y + otherMetrics.Total.Global.Size.y;
			controlMetrics.Client.Global.Offset.y	= (s1_t)(controlMetrics.Total.Global.Offset.y + ncSizesScaled.Top);
		}
		}
	}
	if(dockToControl.Left != -1) {
		cnst cid_t idDock = ::controlDockTarget(gui, dockToControl.Left, GUI_CONTROL_LIMIT_Left);
		if(idDock != CID_INVALID) {
		if_fail_fef(gpk::controlInvalid(gui.Controls, idDock), "Invalid control id: %i.", idDock);
		controlUpdateMetricsTopToDown(gui, idDock, targetSize, true);
		cnst SControlPlacement	& other			= gui.Controls.Placement[idDock];
		cnst SControlArea		& otherMetrics	= gui.Controls.Area	[idDock];
		if(bit_true(other.Align, ALIGN_RIGHT) && bit_false(other.Align, ALIGN_HCENTER)) {
			controlMetrics.Total	.Global.Offset.x	= otherMetrics				.Total.Global.Offset.x + otherMetrics.Total.Global.Size.x - controlMetrics.Total.Global.Size.x;
			controlMetrics.Client	.Global.Offset.x	= s1_t(controlMetrics	.Total.Global.Offset.x + ncSizesScaled.Left);
		}
		else {
			controlMetrics.Total	.Global.Offset.x	= otherMetrics				.Total.Global.Offset.x;
			controlMetrics.Client	.Global.Offset.x	= s1_t(controlMetrics	.Total.Global.Offset.x + ncSizesScaled.Left);
		}
		}
	}
	if(dockToControl.Top	!= -1) {
		cnst cid_t idDock = ::controlDockTarget(gui, dockToControl.Top, GUI_CONTROL_LIMIT_Top);
		if(idDock != CID_INVALID) {
		if_fail_fef(gpk::controlInvalid(gui.Controls, idDock), "Invalid control id: %i.", idDock);
		cnst SControlPlacement	& other					= gui.Controls.Placement[idDock];
		cnst SControlArea		& otherMetrics			= gui.Controls.Area	[idDock];
		if(bit_true(other.Align, ALIGN_BOTTOM	) && bit_false(other.Align, ALIGN_VCENTER)) {
			controlMetrics.Total.Global.Offset.y	= otherMetrics.Total.Global.Offset.y - controlMetrics.Total.Global.Size.y;
			controlMetrics.Client.Global.Offset.y	= controlMetrics.Total.Global.Offset.y + ncSizes.Top ;
		}
		else {
			controlMetrics.Total.Global.Offset.y	= otherMetrics.Total.Global.Offset.y + controlMetrics.Total.Global.Offset.y;
			controlMetrics.Client.Global.Offset.y	= controlMetrics.Total.Global.Offset.y + ncSizes.Top ;
		}
		}
	}
	{ // calculate text rectangle
		cnst rect2s1_t		& targetRect			= controlMetrics.Client.Global;
		controlMetrics.Text.Offset	= {};
		controlMetrics.Text.Size	= rectText.Size.s1();
		realignRectangle(targetRect.Size.u2(), controlMetrics.Text, controlMetrics.Text, controlText.Align);
		controlMetrics.Text.Offset	+= controlMetrics.Client.Global.Offset.s1();
	}
	::buildControlGeometry(controlPlacement, controlMetrics, gui.Zoom, controlMetrics.Rectangles, controlMetrics.Triangles);
	gui.Controls.SetUpdated(iControl, true);
	
	return 0;
}

gpk::err_t 		gpk::controlUpdateMetricsTopToDown							(SGUI & gui, cid_t iControl, cnst n2<u1_t> & targetSize, b8_t forceUpdate)				{
	if(false == gui.Controls.IsUpdated(iControl) || forceUpdate)
		if_fail_fef(::controlUpdateMetrics(gui, iControl, targetSize), "Failed to update metrics for control %i", iControl);

	vcid					& children												= gui.Controls.Children[iControl];
	for(u2_t iChild = 0; iChild < children.size(); ++iChild)
		if_fail_fef(gpk::controlUpdateMetricsTopToDown(gui, children[iChild], targetSize, forceUpdate), "Failed to update metrics for control %i", children[iChild]);
	return 0;
}

gpk::err_t 		gpk::guiUpdateMetrics									(SGUI & gui, cnst n2<u1_t> & targetSize, b8_t forceUpdate)								{
	for(u2_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl)
		if(controlInvalid(gui.Controls, gui.Controls.States[iControl].Parent) && false == controlInvalid(gui.Controls, (cid_t)iControl))
			if_fail_fef(gpk::controlUpdateMetricsTopToDown(gui, (cid_t)iControl, targetSize, forceUpdate), "Failed to update metrics for control %i", iControl);
	return 0;
}

sttc	cid_t	updateGUIControlHovered	(SGUI & gui, cid_t iControl, SControlState & controlState, cnst SInput & inputSystem, b8_t disabled)	nxpt	{
	if(controlState.IsHovered()) {
		if(inputSystem.ButtonDown(0) && false == controlState.IsPressed())
			gui.Controls.SetPressed(iControl, true);
		else if(inputSystem.ButtonUp(0)) {
			gui.Controls.SetPressed(iControl, false);
			gui.Controls.SetDragging(iControl, false);
		}
	}
	else {
		if(false == disabled) {
			gui.Controls.SetHovered(iControl, true);//controlFlags.Disabled;
		}
	}
	return controlState.IsHovered() ? iControl : -1;
}

sttc	cid_t	controlProcessInput		(SGUI & gui, cnst SInput & input, cid_t iControl)														{
	//--------------------
	SControlState		& controlState			= gui.Controls.States[iControl];
	cid_t				controlHovered			= -1;
	cid_t				controlBranch			= -1;
	{
		vcid					& children				= gui.Controls.Children[iControl];
		for(u2_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild) {
			if(controlHidden(gui.Controls, children[iChild]))
				continue;
			cid_t				controlPressed			= ::controlProcessInput(gui, input, children[iChild]);
			if(gui.Controls.States.size() > (u2_t)controlPressed) {
				//controlState.Hover		= false;
				gui.Controls.SetPressed(iControl, false);
				controlHovered			= controlPressed;
				controlBranch			= children[iChild];
			}
		}
	}
	if(input.ButtonDown(0) && controlInvalid(gui.Controls, controlState.Parent) && false == controlInvalid(gui.Controls, controlBranch))
		if_fail_e(controlToFront(gui.Controls, controlBranch));
	if(controlHovered == -1 && in_range(gui.CursorPos.s1(), gui.Controls.Area[iControl].Total.Global)) {
		if(controlState.IsPressed() && input.MouseCurrent.ButtonState[0] && input.MouseCurrent.Position != input.MousePrevious.Position)
			gui.Controls.SetDragging(iControl, true);
		controlHovered			= ::updateGUIControlHovered(gui, iControl, controlState, input, controlDisabled(gui.Controls, iControl) || controlHidden(gui.Controls, iControl));
	}
	else {
		gui.Controls.SetHovered(iControl, false);

		if(input.ButtonUp(0)) {
			gui.Controls.SetPressed(iControl, false);
			gui.Controls.SetDragging(iControl, false);
		}
	}
	return controlHovered;
}

gpk::err_t 		gpk::guiProcessInput	(SGUI & gui, cnst SInput & input, vpobj<SEventSystem> sysEvents)	{
	if(0 == gui.LastSize.LengthSquared()) 
		return 0;

	gui.Controls.EventQueue.clear();
	// shouldn't this be somewhere else? gui.Controls.StatesPrevious = gui.Controls.States;
	if_true_e(failed(guiUpdateMetrics(gui, gui.LastSize, false)));
	cid_t				controlHovered			= -1;
	acid					rootControlsToProcess	= {};
	rootControlsToProcess.reserve(1000);
	(void)sysEvents;
	for(u2_t iControl = 0, countControls = gui.Controls.States.size(); iControl < countControls; ++iControl) {	// Only process root parents
		cnst SControlState	& controlState			= gui.Controls.States[iControl];
		if(controlState.IsUnused())
			continue;

		gui.Controls.SetAction((cid_t)iControl, false);

		if(controlState.IsDisabled())
			continue;

		if(controlHidden(gui.Controls, (cid_t)iControl))
			continue;

		if(false == controlInvalid(gui.Controls, controlState.Parent))
			continue;

		rootControlsToProcess.push_back((cid_t)iControl);
	}

	gui.CursorPos			= {(f2_t)input.MouseCurrent.Position.x, (f2_t)input.MouseCurrent.Position.y}; //+= {(f2_t)input.MouseCurrent.Deltas.x, (f2_t)input.MouseCurrent.Deltas.y};
	for(u2_t iControl = 0, countControls = rootControlsToProcess.size(); iControl < countControls; ++iControl) {
		if(controlHidden(gui.Controls, rootControlsToProcess[iControl]))
			continue;

		cid_t				controlPressed		= ::controlProcessInput(gui, input, rootControlsToProcess[iControl]);
		if(gui.Controls.States.size() > (u2_t)controlPressed)
			controlHovered			= controlPressed;
	}
	for(u2_t iControl = 0, countControls = gui.Controls.States.size(); iControl < countControls; ++iControl) {
		if(input.ButtonDown(0))
			gui.Controls.SetFocused((cid_t)iControl, (cid_t)iControl == controlHovered);
		if(iControl != (u2_t)controlHovered) {
			gui.Controls.SetHovered((cid_t)iControl, false);;
			if(0 == input.MouseCurrent.ButtonState[0]) {
				gui.Controls.SetPressed((cid_t)iControl, false);
				gui.Controls.SetDragging((cid_t)iControl, false);
			}
		}
	}

	//for(u2_t iControl = 0, countControls = gui.Controls.States.size(); iControl < countControls; ++iControl) {
	//	cnst GUI_CONTROL_FLAG	statePrevious		= gui.Controls.StatesPrevious[iControl].Flag;
	//	cnst GUI_CONTROL_FLAG	stateCurrent		= gui.Controls.States[iControl].Flag;
	//	if(statePrevious != stateCurrent)
	//		if_fail_fe(eventEnqueueGuiControlStateChange(gui.Controls.EventQueue, (cid_t)iControl, statePrevious, stateCurrent));
	//}

	return controlHovered;
}

gpk::cid_t			gpk::createScreenLayout(SGUI & gui, cid_t parent) {
	cid_t				controlId;
	if_fail_fe(controlId = controlCreate(gui));

	gui.Controls.Placement	[controlId].Border				= 
	gui.Controls.Placement	[controlId].Margin				= {};
	gui.Controls.Constraints[controlId].AttachSizeToControl	= {controlId, controlId};
	gui.Controls.Draw		[controlId].NoClient			= true;
	gui.Controls.States		[controlId].Mask				&= ~GUI_CONTROL_FLAG_Hovered;

	if(parent != CID_INVALID)
		if_fail_fe(gpk::controlSetParent(gui.Controls, controlId, parent));

	return controlId;
}

gpk::cid_t			gpk::guiCreateControlList	(SGUI & gui, DIRECTION direction, view<vcst_c> buttonText, cid_t iParent, cnst SControlPlacement & args, ALIGN textAlign, vcid out_ids) {
	cid_t					result						= -1;
	for(u1_t iButton = 0; iButton < buttonText.size(); ++iButton) {
		cid_t				idControl					= {};
		if_fail_fe(idControl = controlCreate(gui));
		if(out_ids.size() > iButton)
			out_ids[iButton]		= idControl;

		if(0 == iButton)
			result					= idControl;

		SControlPlacement	& control					= gui.Controls.Placement[idControl];
		control					= args;
		if(direction & (DIRECTION_LEFT | DIRECTION_RIGHT))
			control.Area.Offset.x	+= s1_t(args.Area.Size.x * iButton);
		else
			control.Area.Offset.y	+= s1_t(args.Area.Size.y * iButton);

		SControlText			& controlText				= gui.Controls.Text[idControl];
		controlText.Text		= buttonText[iButton];
		controlText.Align		= textAlign;

		SControlConstraints	& itemConstraints	= gui.Controls.Constraints[idControl];
		if(direction & (DIRECTION_UP | DIRECTION_DOWN))
			itemConstraints.AttachSizeToControl.x	= iParent;
		else if(direction & (DIRECTION_LEFT | DIRECTION_RIGHT)) {
			itemConstraints.AttachSizeToText.x		= true;
			itemConstraints.AttachSizeToControl.y	= iParent;
		}

		if(iButton) {
			if(direction & DIRECTION_LEFT)
				itemConstraints.DockToControl.Left	= out_ids[iButton - 1];
			else if(direction & DIRECTION_RIGHT)
				itemConstraints.DockToControl.Right	= out_ids[iButton - 1];
			else if(direction & DIRECTION_UP)
				itemConstraints.DockToControl.Top	= out_ids[iButton - 1];
			else if(direction & DIRECTION_DOWN)
				itemConstraints.DockToControl.Bottom	= out_ids[iButton - 1];

		}
		if_fail_fef(gpk::controlSetParent(gui.Controls, idControl, iParent), "idControl: %i, buttonText[iButton]: %.*s", idControl, GPK_VIEW_ARGS(buttonText[iButton])); // Make a tplt and macro for this pattern
	}
	return result;
}

gpk::cid_t			gpk::guiCreateControlList		(SGUI & gui, view<vcst_c> buttonText, cid_t iParent, n2u1_c & buttonSize, n2s1_c & offset, ALIGN controlAlign, ALIGN textAlign, vcid out_ids) {
	SControlPlacement		controlArgs					= {};
	controlArgs.Align	= controlAlign;
	controlArgs.Area	= {offset, buttonSize.s1()};
	controlArgs.Border	= {1, 1, 1, 1};
	controlArgs.Margin	= {1, 1, 1, 1};
	return guiCreateControlList(gui, {}, buttonText, iParent, controlArgs, textAlign, out_ids);
}
