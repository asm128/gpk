#include "gpk_gui.h"
#include "gpk_gui_theme.h"
#include "gpk_gui_text.h"
#include "gpk_base64.h"
#include "gpk_rect_align.h"

usns gpk;

gpk::err_t 		gpk::controlInvalid			(const SGUI & gui, cid_t iControl)				{
	if(gui.Controls.States.size() <= u2_t(iControl)	) 
		return -1;
	if(gui.Controls.IsUnused(iControl)) 
		return -1;
	return 0;
}

cid_t			gpk::controlCreateChild	(SGUI & gui, cid_t iParent)						{
	cid_t				idControl				= controlCreate(gui);
	controlSetParent(gui, idControl, iParent);
	return idControl;
}

cid_t			gpk::controlCreate		(SGUI & gui)							{
	if_fail_fe(guiSetupDefaultColors(gui.Colors));

	if(0 == gui.Fonts.size())
		if_fail_fe(guiSetupDefaultFonts(gui));

	if(gui.Fonts.size() <= gui.SelectedFont)
		gui.SelectedFont = u1_t(gui.Fonts.size() - 1);

	return controlCreate(gui.Controls, gui.Colors->DefaultColors);
}

gpk::err_t 		gpk::controlDelete		(SGUI & gui, cid_t iControl, bool recursive)			{
	if_fail_fef(controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	info_printf("Deleting control id: %i.", iControl);
	if(recursive) { // Delete children first.
		acid					children				= gui.Controls.Children[iControl];
		for(u2_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
			if_true_ef(failed(controlDelete(gui, children[iChild], true)), "Failed to delete control! Invalid control id? %i.", children[iChild]);
	}
	// Remove from parent list
	const cid_t			indexParent				= gui.Controls.States[iControl].Parent;
#if defined(GPK_DEBUG_ENABLED)
	u2_t					childrenRemoved			= 0;
#endif
	if((u2_t)indexParent < gui.Controls.States.size() && false == gui.Controls.IsUnused(indexParent)) {
		acid					& children				= gui.Controls.Children[indexParent];
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
	gui.Controls.SetUnused(iControl, true);
	return 0;
}

gpk::err_t		gpk::controlSetParent	(SGUI & gui, cid_t iControl, cid_t iParent)	{
	if_fail_fef(controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);
	const cid_t			oldParent				= gui.Controls.States[iControl].Parent;
	SControlState		& control				= gui.Controls.States[iControl];
	if(control.Parent == iParent)	// Exit early if there is nothing to do here.
		return 0;

	control.Parent		= iParent;
	if(controlInvalid(gui, iParent)) {
		if_true_vef(-1, -1 != iParent, "Invalid parent control id: %u.", iParent);
		return 0;
	}

	if((u2_t)oldParent < gui.Controls.States.size() && false == gui.Controls.IsUnused(oldParent)) {
		acid					& children				= gui.Controls.Children[oldParent];
		for(s2_t iChild = 0; iChild < (s2_t)children.size(); ++iChild)
			if(children[iChild] == iControl) {
				gui.Controls.SetUpdated(oldParent, false);
				if_fail_fef(children.remove(iChild), "Failed to remove child at index: %u.", iChild);
				break;
			}
	}

	// Set iControl to parent's children array.
	acid					& children				= gui.Controls.Children[iParent];
	for(s2_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild)
		if(children[iChild] == iControl)
			return 0;

	gui.Controls.SetUpdated(iParent , false);
	gui.Controls.SetUpdated(iControl, false);
	if_fail_fe(children.push_back(iControl));
	return 0;
}

sttc gpk::err_t	buildControlGeometry	(const SControlPlacement & control, const SControlArea & controlMetrics, const SGUIZoom & zoom, view<rect2s1_t> finalRects, view<tri2s1_t> triangles)					{
	rects1_t			scaledBorders			= {};
	n2f3_c				scaleFinal				= zoom.DPI * zoom.ZoomLevel;
	scaledBorders.Left		= (s1_t)(control.Border.Left		* scaleFinal.x);
	scaledBorders.Top		= (s1_t)(control.Border.Top		* scaleFinal.y);
	scaledBorders.Right		= (s1_t)(control.Border.Right	* scaleFinal.x);
	scaledBorders.Bottom	= (s1_t)(control.Border.Bottom	* scaleFinal.y);
	(void)scaledBorders;

	const rect2s1_t		& rectTotal				= controlMetrics.Total.Global;
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

sttc	gpk::err_t controlUpdateMetrics	(SGUI & gui, cid_t iControl, const n2<u1_t> & _targetSize)					{
	if_fail_fef(gpk::controlInvalid(gui, iControl), "Invalid control id: %u.", iControl);

	SControlState		& controlState			= gui.Controls.States[iControl];
	const SControlPlacement	& controlPlacement	= gui.Controls.Placement[iControl];
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
	const SRasterFont	& selectedFont			= *gui.Fonts[in_range(controlText.FontSelected, (s1_t)0, (s1_t)gui.Fonts.size()) ? controlText.FontSelected : gui.SelectedFont];

	const n2u0_t			fontCharSize			= selectedFont.CharSize;
	rectText.Size			= {(f3_t)(fontCharSize.x * controlText.Text.size()), (f3_t)fontCharSize.y};
	rectText.Size.InPlaceScale(scale.x, scale.y);

	const SControlConstraints	& controlConstraints	= gui.Controls.Constraints[iControl];
	rectu1_c		ncSizes					= controlNCRect(controlPlacement);
	rectf3_c		ncSizesScaled			= {(ncSizes.Left * scale.x), (ncSizes.Top * scale.y), (ncSizes.Right * scale.x), (ncSizes.Bottom * scale.y)};
	n2f3_c			ncTotalSize				= {(f3_t)ncSizes.Left + ncSizes.Right, (f3_t)ncSizes.Top + ncSizes.Bottom};
	n2f3_c			ncTotalSizeScaled		= ncTotalSize.GetScaled(scale.x, scale.y);
	if(controlConstraints.AttachSizeToText.x) scaledSize.x	= rectText.Size.x + ncTotalSizeScaled.x;
	if(controlConstraints.AttachSizeToText.y) scaledSize.y	= rectText.Size.y + ncTotalSizeScaled.y;
	b8_c			isValidParent			= 0 == controlInvalid(gui, controlState.Parent);
	n2u1_c			targetSize				= isValidParent ? gui.Controls.Area[controlState.Parent].Client.Global.Size.u1() : _targetSize.u1();

	if(controlConstraints.AttachSizeToControl.x == iControl) { if(controlConstraints.DockToControl.Left	!= -1) {} else { scaledPosition.x = 0; } scaledSize.x = targetSize.x; } else if(false == controlInvalid(gui, controlConstraints.AttachSizeToControl.x)) { if(controlConstraints.DockToControl.Left != -1) {} else { scaledPosition.x = 0;} scaledSize.x = gui.Controls.Area[controlConstraints.AttachSizeToControl.x].Total.Global.Size.x; }
	if(controlConstraints.AttachSizeToControl.y == iControl) { if(controlConstraints.DockToControl.Top	!= -1) {} else { scaledPosition.y = 0; } scaledSize.y = targetSize.y; } else if(false == controlInvalid(gui, controlConstraints.AttachSizeToControl.y)) { if(controlConstraints.DockToControl.Top  != -1) {} else { scaledPosition.y = 0;} scaledSize.y = gui.Controls.Area[controlConstraints.AttachSizeToControl.y].Total.Global.Size.y; }

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

	const rectcid		& dockToControl			= controlConstraints.DockToControl;
	if(dockToControl.Right != -1) {
		if_fail_fef(gpk::controlInvalid(gui, dockToControl.Right), "Invalid control id: %i.", dockToControl.Right);
		controlUpdateMetricsTopToDown(gui, dockToControl.Right, targetSize.u1(), true);
		const SControlPlacement	& other				= gui.Controls.Placement[dockToControl.Right];
		const SControlArea		& otherMetrics		= gui.Controls.Area	[dockToControl.Right];
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
	if(dockToControl.Bottom != -1) {
		if_fail_fef(gpk::controlInvalid(gui, dockToControl.Bottom), "Invalid control id: %i.", dockToControl.Bottom);
		const SControlPlacement	& other					= gui.Controls.Placement[dockToControl.Bottom];
		const SControlArea		& otherMetrics			= gui.Controls.Area[dockToControl.Bottom	];
		if(bit_true(other.Align, ALIGN_BOTTOM	) && bit_false(other.Align, ALIGN_VCENTER)) {
			controlMetrics.Total.Global.Offset.y	= otherMetrics.Total.Global.Offset.y - controlMetrics.Total.Global.Size.y;
			controlMetrics.Client.Global.Offset.y	= (s1_t)(controlMetrics.Total.Global.Offset.y + ncSizesScaled.Top);
		} else {
			controlMetrics.Total.Global.Offset.y	= otherMetrics.Total.Global.Offset.y + otherMetrics.Total.Global.Size.y;
			controlMetrics.Client.Global.Offset.y	= (s1_t)(controlMetrics.Total.Global.Offset.y + ncSizesScaled.Top);
		}
	}
	if(dockToControl.Left != -1) {
		if_fail_fef(gpk::controlInvalid(gui, dockToControl.Left), "Invalid control id: %i.", dockToControl.Left);
		controlUpdateMetricsTopToDown(gui, dockToControl.Left, targetSize, true);
		const SControlPlacement	& other			= gui.Controls.Placement[dockToControl.Left];
		const SControlArea		& otherMetrics	= gui.Controls.Area	[dockToControl.Left];
		if(bit_true(other.Align, ALIGN_RIGHT) && bit_false(other.Align, ALIGN_HCENTER)) {
			controlMetrics.Total	.Global.Offset.x	= otherMetrics				.Total.Global.Offset.x + otherMetrics.Total.Global.Size.x - controlMetrics.Total.Global.Size.x;
			controlMetrics.Client	.Global.Offset.x	= s1_t(controlMetrics	.Total.Global.Offset.x + ncSizesScaled.Left);
		}
		else {
			controlMetrics.Total	.Global.Offset.x	= otherMetrics				.Total.Global.Offset.x;
			controlMetrics.Client	.Global.Offset.x	= s1_t(controlMetrics	.Total.Global.Offset.x + ncSizesScaled.Left);
		}
	}
	if(dockToControl.Top	!= -1) {
		if_fail_fef(gpk::controlInvalid(gui, dockToControl.Top), "Invalid control id: %i.", dockToControl.Top);
		const SControlPlacement	& other					= gui.Controls.Placement[dockToControl.Top ];
		const SControlArea		& otherMetrics			= gui.Controls.Area	[dockToControl.Top ];
		if(bit_true(other.Align, ALIGN_BOTTOM	) && bit_false(other.Align, ALIGN_VCENTER)) {
			controlMetrics.Total.Global.Offset.y	= otherMetrics.Total.Global.Offset.y - controlMetrics.Total.Global.Size.y;
			controlMetrics.Client.Global.Offset.y	= controlMetrics.Total.Global.Offset.y + ncSizes.Top ;
		}
		else {
			controlMetrics.Total.Global.Offset.y	= otherMetrics.Total.Global.Offset.y + controlMetrics.Total.Global.Offset.y;
			controlMetrics.Client.Global.Offset.y	= controlMetrics.Total.Global.Offset.y + ncSizes.Top ;
		}
	}
	{ // calculate text rectangle
		const rect2s1_t		& targetRect			= controlMetrics.Client.Global;
		controlMetrics.Text.Offset	= {};
		controlMetrics.Text.Size	= rectText.Size.s1();
		realignRectangle(targetRect.Size.u2(), controlMetrics.Text, controlMetrics.Text, controlText.Align);
		controlMetrics.Text.Offset	+= controlMetrics.Client.Global.Offset.s1();
	}
	::buildControlGeometry(controlPlacement, controlMetrics, gui.Zoom, controlMetrics.Rectangles, controlMetrics.Triangles);
	gui.Controls.SetUpdated(iControl, true);
	
	return 0;
}

gpk::err_t 		gpk::controlUpdateMetricsTopToDown							(SGUI & gui, cid_t iControl, const n2<u1_t> & targetSize, bool forceUpdate)				{
	if(false == gui.Controls.IsUpdated(iControl) || forceUpdate)
		if_fail_fef(::controlUpdateMetrics(gui, iControl, targetSize), "Failed to update metrics for control %i", iControl);

	vcid					& children												= gui.Controls.Children[iControl];
	for(u2_t iChild = 0; iChild < children.size(); ++iChild)
		if_fail_fef(gpk::controlUpdateMetricsTopToDown(gui, children[iChild], targetSize, forceUpdate), "Failed to update metrics for control %i", children[iChild]);
	return 0;
}

gpk::err_t 		gpk::guiUpdateMetrics									(SGUI & gui, const n2<u1_t> & targetSize, bool forceUpdate)								{
	for(u2_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl)
		if(controlInvalid(gui, gui.Controls.States[iControl].Parent) && false == controlInvalid(gui, (cid_t)iControl))
			if_fail_fef(gpk::controlUpdateMetricsTopToDown(gui, (cid_t)iControl, targetSize, forceUpdate), "Failed to update metrics for control %i", iControl);
	return 0;
}

sttc	cid_t	updateGUIControlHovered	(SGUI & gui, cid_t iControl, SControlState & controlState, const SInput & inputSystem, bool disabled)	nxpt	{
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

sttc	cid_t	controlProcessInput		(SGUI & gui, const SInput & input, cid_t iControl)														{
	//--------------------
	SControlState		& controlState			= gui.Controls.States[iControl];
	cid_t				controlHovered			= -1;
	{
		vcid					& children				= gui.Controls.Children[iControl];
		for(u2_t iChild = 0, countChild = children.size(); iChild < countChild; ++iChild) {
			if(controlHidden(gui, children[iChild]))
				continue;
			cid_t				controlPressed			= ::controlProcessInput(gui, input, children[iChild]);
			if(gui.Controls.States.size() > (u2_t)controlPressed) {
				//controlState.Hover		= false;
				gui.Controls.SetPressed(iControl, false);
				controlHovered			= controlPressed;
			}
		}
	}
	if(controlHovered == -1 && in_range(gui.CursorPos.s1(), gui.Controls.Area[iControl].Total.Global)) {
		if(controlState.IsPressed() && input.MouseCurrent.ButtonState[0] && input.MouseCurrent.Position != input.MousePrevious.Position)
			gui.Controls.SetDragging(iControl, true);
		controlHovered			= ::updateGUIControlHovered(gui, iControl, controlState, input, controlDisabled(gui, iControl) || controlHidden(gui, iControl));
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

gpk::err_t 		gpk::guiProcessInput	(SGUI & gui, const SInput & input, vpobj<SEventSystem> sysEvents)	{
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
		const SControlState	& controlState			= gui.Controls.States[iControl];
		if(controlState.IsUnused())
			continue;

		gui.Controls.SetAction((cid_t)iControl, false);

		if(controlState.IsDisabled())
			continue;

		if(controlHidden(gui, (cid_t)iControl))
			continue;

		if(false == controlInvalid(gui, controlState.Parent))
			continue;

		rootControlsToProcess.push_back((cid_t)iControl);
	}

	gui.CursorPos			= {(f2_t)input.MouseCurrent.Position.x, (f2_t)input.MouseCurrent.Position.y}; //+= {(f2_t)input.MouseCurrent.Deltas.x, (f2_t)input.MouseCurrent.Deltas.y};
	for(u2_t iControl = 0, countControls = rootControlsToProcess.size(); iControl < countControls; ++iControl) {
		if(controlHidden(gui, rootControlsToProcess[iControl]))
			continue;

		cid_t				controlPressed		= ::controlProcessInput(gui, input, rootControlsToProcess[iControl]);
		if(gui.Controls.States.size() > (u2_t)controlPressed)
			controlHovered			= controlPressed;
	}
	for(u2_t iControl = 0, countControls = gui.Controls.States.size(); iControl < countControls; ++iControl) {
		if(iControl != (u2_t)controlHovered) {
			gui.Controls.SetHovered((cid_t)iControl, false);;
			if(0 == input.MouseCurrent.ButtonState[0]) {
				gui.Controls.SetPressed((cid_t)iControl, false);
				gui.Controls.SetDragging((cid_t)iControl, false);
			}
		}
	}

	//for(u2_t iControl = 0, countControls = gui.Controls.States.size(); iControl < countControls; ++iControl) {
	//	const GUI_CONTROL_FLAG	statePrevious		= gui.Controls.StatesPrevious[iControl].Flag;
	//	const GUI_CONTROL_FLAG	stateCurrent		= gui.Controls.States[iControl].Flag;
	//	if(statePrevious != stateCurrent)
	//		if_fail_fe(eventEnqueueGuiControlStateChange(gui.Controls.EventQueue, (cid_t)iControl, statePrevious, stateCurrent));
	//}

	return controlHovered;
}

gpk::err_t 		gpk::guiDeselect		(SGUI & gui)	{
	for(u2_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl)
		gui.Controls.SetSelected((cid_t)iControl, false);

	return 0;
}

gpk::err_t 		gpk::guiSelect			(SGUI & gui, cid_t controlToSelect)	{
	if_true_fe(controlInvalid(gui, controlToSelect));
	cnst cid_t parent = gui.Controls.States[controlToSelect].Parent;
	for(u2_t iControl = 0; iControl < gui.Controls.States.size(); ++iControl)
		if(gui.Controls.States[iControl].Parent == parent)
			gui.Controls.SetSelected((cid_t)iControl, controlToSelect == (cid_t)iControl);

	return 0;
}

gpk::err_t 		gpk::controlHidden		(const SGUI & gui, cid_t iControl)	{
	bool						imHidden				= controlInvalid(gui, iControl) || gui.Controls.States[iControl].IsHidden();
	return imHidden ? imHidden : (0 == controlInvalid(gui, gui.Controls.States[iControl].Parent) && controlHidden(gui, gui.Controls.States[iControl].Parent));
}

gpk::err_t 		gpk::controlDisabled	(const SGUI & gui, cid_t iControl)	{
	bool						imDisabled				= controlInvalid(gui, iControl) || gui.Controls.States[iControl].IsDisabled();
	return imDisabled ? imDisabled : (false == controlInvalid(gui, gui.Controls.States[iControl].Parent) && controlDisabled(gui, gui.Controls.States[iControl].Parent));
}

gpk::err_t 		gpk::controlMetricsInvalidate	(SGUI & gui, cid_t iControl)	{
	gui.Controls.SetUpdated(iControl, false);
	const vcid			& controlChildren		= gui.Controls.Children[iControl];
	for(u2_t iChild = 0, countChild = controlChildren.size(); iChild < countChild; ++iChild)
		if_fail_fef(gpk::controlMetricsInvalidate(gui, controlChildren[iChild]), "%s", "Invalid child?");

	return 0;
}

gpk::err_t 		gpk::guiGetProcessableControls	(const SGUI & gui, acid & controlIndices)													{
	for(u2_t iControl = 0, countControls = gui.Controls.States.size(); iControl < countControls; ++iControl) {	// Only process root parents
		const SControlState	& controlState					= gui.Controls.States[iControl];
		if(controlState.IsUnused() || controlDisabled(gui, (cid_t)iControl) || controlHidden(gui, (cid_t)iControl))
			continue;
		if_fail_fe(controlIndices.push_back((cid_t)iControl));
	}
	return controlIndices.size();
}

gpk::err_t 		gpk::guiProcessControls		(const SGUI & gui, const function<err_t(cid_t iControl)> & funcOnExecute) {
	acid					controlsToProcess			= {};
	if_fail_fe(gpk::guiGetProcessableControls(gui, controlsToProcess));
	return guiProcessControls(gui, controlsToProcess, funcOnExecute);
}

gpk::err_t 		gpk::guiProcessControls		(const SGUI & gui, vcid controlsToProcess, const function<err_t(cid_t iControl)> & funcOnExecute) {
	for(u2_t iControl = 0, countControls = controlsToProcess.size(); iControl < countControls; ++iControl) {
		const cid_t			idControl					= controlsToProcess[iControl];
		const SControlState	& controlState				= gui.Controls.States[idControl];
		if(controlState.IsAction()) {
			info_printf("Executed control %i (0x%x).", idControl, idControl);
			return funcOnExecute(idControl);
		}
	}
	return 0;
}

gpk::err_t 		gpk::controlTextSet		(SGUI & gui, cid_t iControl, vcst_t text)				{
	vcst_t					& oldText				= gui.Controls.Text[iControl].Text;
	if(text != oldText) {
		oldText					= text;
		gui.Controls.SetUpdated(iControl, false);
	}
	return 0;
}

gpk::err_t 		gpk::controlFontSet		(SGUI & gui, cid_t iControl, s1_t iFont)				{
	s1_t						& oldText				= gui.Controls.Text[iControl].FontSelected;
	if(iFont != oldText) {
		oldText					= iFont;
		gui.Controls.SetUpdated(iControl, false);
	}
	return 0;
}

sttc	gpk::err_t controlInstanceReset	(SControlTable & controlTable, cid_t iControl, vs1_t defaultColors)										{
	controlTable.States			[iControl]	= {};
	controlTable.Previous		[iControl]	= {};
	controlTable.Placement		[iControl]	= {};
	controlTable.Area			[iControl]	= {};
	controlTable.Constraints	[iControl]	= {};
	controlTable.Draw			[iControl]	= {};
	controlTable.Text			[iControl]	= {};
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
			if_fail_fef(::controlInstanceReset(controlTable, (cid_t)iControl, defaultColors), "iControl: %i", iControl);
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
		, controlTable.Images
		, controlTable.Children
		) - 1);
	if_fail_fe(::controlInstanceReset(controlTable, (cid_t)iControl, defaultColors));
	return (cid_t)iControl;
}
