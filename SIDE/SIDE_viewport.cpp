#include "SIDE_viewport.h"
#include "gpk_label.h"

usns gpk;

stxp vcst_c viewportControlIdentifiers[VIEWPORT_CONTROL_COUNT] =
	{ "resize_left"
	, "resize_top"
	, "resize_right"
	, "resize_bottom"
	, "resize_top_left"
	, "resize_top_right"
	, "resize_bottom_left"
	, "resize_bottom_right"
	, "title"
	, "close"
	, "target"
	};

gpk::cid_t gpk::viewportControl(cnst SGUI & gui, cid_t idViewport, VIEWPORT_CONTROL control) {
	if_true_vef(CID_INVALID, controlInvalid(gui, idViewport), "Invalid viewport control: %i.", idViewport);
	if_true_vef(CID_INVALID, control < 0 || control >= VIEWPORT_CONTROL_COUNT, "Invalid viewport part: %i.", control);
	if_true_vef(CID_INVALID, gui.Controls.Children[idViewport].size() < VIEWPORT_CONTROL_COUNT, "Control %i is not a viewport.", idViewport);
	rtrn gui.Controls.Children[idViewport][control];
}

gpk::cid_t gpk::viewportCreate(SGUI & gui, cid_t idParent) {
	if_true_vef(CID_INVALID, gui.Fonts.size() <= gui.SelectedFont, "Invalid GUI font index: %u.", gui.SelectedFont);
	cnst n2u0_t fontCharSize = gui.Fonts[gui.SelectedFont]->CharSize;
	s1_c heightTitleBar = fontCharSize.y + 4;
	u2_c widthTarget = 800;
	cnst n2f3_t targetSize = {(f3_t)widthTarget, widthTarget * (9 / 16.0)};
	sttc s2_t counterVPs = 0;
	s2_c viewportSequence = ++counterVPs;
	cid_t idViewport = -1;
	if_fail_fe(idViewport = controlCreate(gui));
	string viewportIdentifier = {};
	if_fail_fe(append_strings(viewportIdentifier, "viewport_", str(viewportSequence)));
	gui.Controls.Identifier[idViewport] = label(viewportIdentifier);
	if(-1 != idParent)
		if_fail_fe(controlSetParent(gui, idViewport, idParent));
	SControlPlacement & viewport = gui.Controls.Placement[idViewport];
	viewport.Align = ALIGN_TOP_LEFT;
	viewport.Border = viewport.Margin = {1, 1, 1, 1};
	viewport.Area.Size = {
		(s1_t)(targetSize.x + controlNCSpacing(viewport).x),
		(s1_t)(targetSize.y + heightTitleBar + controlNCSpacing(viewport).y),
	};
	for(u2_t iElement = 0; iElement < VIEWPORT_CONTROL_COUNT; ++iElement) {
		cid_t idControl = -1;
		if_fail_fe(idControl = controlCreateChild(gui, idViewport));
		gui.Controls.Identifier[idControl] = viewportControlIdentifiers[iElement];
		gui.Controls.Constraints[idControl].AttachSizeToControl.x = idControl;
	}
	for(u2_t iElement = 0; iElement < VIEWPORT_CONTROL_TITLE; ++iElement) {
		cid_t idControl = -1;
		if_fail_fe(idControl = viewportControl(gui, idViewport, (VIEWPORT_CONTROL)iElement));
		gui.Controls.Placement[idControl].Area.Size = {16, 16};
		gui.Controls.Placement[idControl].Area.Offset = {-2, -2};
		gui.Controls.Constraints[idControl].AttachSizeToControl = {-1, -1};
	}
	gui.Controls.Placement[viewportControl(gui, idViewport, VIEWPORT_CONTROL_RESIZE_LEFT		)].Align = ALIGN_CENTER_LEFT;
	gui.Controls.Placement[viewportControl(gui, idViewport, VIEWPORT_CONTROL_RESIZE_TOP		)].Align = ALIGN_CENTER_TOP;
	gui.Controls.Placement[viewportControl(gui, idViewport, VIEWPORT_CONTROL_RESIZE_RIGHT		)].Align = ALIGN_CENTER_RIGHT;
	gui.Controls.Placement[viewportControl(gui, idViewport, VIEWPORT_CONTROL_RESIZE_BOTTOM		)].Align = ALIGN_CENTER_BOTTOM;
	gui.Controls.Placement[viewportControl(gui, idViewport, VIEWPORT_CONTROL_RESIZE_TOP_LEFT	)].Align = ALIGN_TOP_LEFT;
	gui.Controls.Placement[viewportControl(gui, idViewport, VIEWPORT_CONTROL_RESIZE_TOP_RIGHT	)].Align = ALIGN_TOP_RIGHT;
	gui.Controls.Placement[viewportControl(gui, idViewport, VIEWPORT_CONTROL_RESIZE_BOTTOM_LEFT	)].Align = ALIGN_BOTTOM_LEFT;
	gui.Controls.Placement[viewportControl(gui, idViewport, VIEWPORT_CONTROL_RESIZE_BOTTOM_RIGHT)].Align = ALIGN_BOTTOM_RIGHT;
	cid_t idClose = viewportControl(gui, idViewport, VIEWPORT_CONTROL_CLOSE);
	gui.Controls.Placement[idClose].Area.Size = {heightTitleBar, heightTitleBar};
	gui.Controls.Placement[idClose].Align = ALIGN_TOP_RIGHT;
	gui.Controls.Text[idClose].Text = "X";
	gui.Controls.Text[idClose].Align = ALIGN_CENTER;
	gui.Controls.Constraints[idClose].AttachSizeToControl = {-1, -1};
	if_fail_fe(append_strings(gui.Controls.Script[idClose], "{\"", gui.Controls.Identifier[idViewport], "\":{\"State\":{\"Flag\":1}}}"));
	cid_t idTitle = viewportControl(gui, idViewport, VIEWPORT_CONTROL_TITLE);
	gui.Controls.Placement[idTitle].Area.Size.y = heightTitleBar;
	sc_t buffer[256] = {};
	cnst s2_t titleLen = snprintf(buffer, size(buffer) - 2, "Viewport #%i", viewportSequence);
	gui.Controls.Text[idTitle].Text = {label(buffer).begin(), (u2_t)titleLen};
	gui.Controls.Text[idTitle].Align = ALIGN_CENTER;
	gui.Controls.Placement[idTitle].Area.Offset.x = heightTitleBar;
	gui.Controls.Placement[idTitle].Align = ALIGN_TOP_RIGHT;
	gui.Controls.Constraints[idTitle].DockToControl = {-1, -1, idClose, -1};
	cid_t idTarget = viewportControl(gui, idViewport, VIEWPORT_CONTROL_TARGET);
	gui.Controls.Text[idTarget].Align = ALIGN_CENTER_TOP;
	gui.Controls.Placement[idTarget].Border = {};
	gui.Controls.Placement[idTarget].Margin = {};
	gui.Controls.Placement[idTarget].Area.Size = targetSize.s1();
	gui.Controls.Draw[idTarget].ColorTheme = 60;
	gui.Controls.Constraints[idTarget].DockToControl = {-1, -1, -1, idTitle};
	rtrn idViewport;
}

sttc gpk::err_t viewportResize(SGUI & gui, cnst SInput & input, cid_t idViewport, gpk::VIEWPORT_CONTROL resizeControl) {
	cid_t idTarget = -1;
	if_fail_fe(idTarget = viewportControl(gui, idViewport, VIEWPORT_CONTROL_TARGET));
	n2s1_t mouseDeltas = input.MouseCurrent.Position - input.MousePrevious.Position;
	if(0 == mouseDeltas.x && 0 == mouseDeltas.y)
		rtrn 0;
	mouseDeltas.InPlaceScale(1 / gui.Zoom.DPI.x, 1 / gui.Zoom.DPI.y);
	mouseDeltas.InPlaceScale(1 / gui.Zoom.ZoomLevel);
	b8_c resizeLeft = VIEWPORT_CONTROL_RESIZE_LEFT == resizeControl || VIEWPORT_CONTROL_RESIZE_TOP_LEFT == resizeControl || VIEWPORT_CONTROL_RESIZE_BOTTOM_LEFT == resizeControl;
	b8_c resizeRight = VIEWPORT_CONTROL_RESIZE_RIGHT == resizeControl || VIEWPORT_CONTROL_RESIZE_TOP_RIGHT == resizeControl || VIEWPORT_CONTROL_RESIZE_BOTTOM_RIGHT == resizeControl;
	b8_c resizeTop = VIEWPORT_CONTROL_RESIZE_TOP == resizeControl || VIEWPORT_CONTROL_RESIZE_TOP_LEFT == resizeControl || VIEWPORT_CONTROL_RESIZE_TOP_RIGHT == resizeControl;
	b8_c resizeBottom = VIEWPORT_CONTROL_RESIZE_BOTTOM == resizeControl || VIEWPORT_CONTROL_RESIZE_BOTTOM_LEFT == resizeControl || VIEWPORT_CONTROL_RESIZE_BOTTOM_RIGHT == resizeControl;
	SControlPlacement & placementViewport = gui.Controls.Placement[idViewport];
	SControlPlacement & placementTarget = gui.Controls.Placement[idTarget];
	if(resizeLeft) {
		mouseDeltas.x = min(mouseDeltas.x, placementTarget.Area.Size.x);
		placementViewport.Area.Offset.x += mouseDeltas.x;
		placementViewport.Area.Size.x -= mouseDeltas.x;
		placementTarget.Area.Size.x -= mouseDeltas.x;
	}
	else if(resizeRight) {
		mouseDeltas.x = max(mouseDeltas.x, (s1_t)-placementTarget.Area.Size.x);
		placementViewport.Area.Size.x += mouseDeltas.x;
		placementTarget.Area.Size.x += mouseDeltas.x;
	}
	if(resizeTop) {
		mouseDeltas.y = min(mouseDeltas.y, placementTarget.Area.Size.y);
		placementViewport.Area.Offset.y += mouseDeltas.y;
		placementViewport.Area.Size.y -= mouseDeltas.y;
		placementTarget.Area.Size.y -= mouseDeltas.y;
	}
	else if(resizeBottom) {
		mouseDeltas.y = max(mouseDeltas.y, (s1_t)-placementTarget.Area.Size.y);
		placementViewport.Area.Size.y += mouseDeltas.y;
		placementTarget.Area.Size.y += mouseDeltas.y;
	}
	rtrn controlMetricsInvalidate(gui, idViewport);
}

gpk::err_t gpk::viewportUpdate(SGUI & gui, cnst SInput & input, cid_t idViewport) {
	if_true_vef(-1, controlInvalid(gui, idViewport), "Invalid viewport control: %i.", idViewport);
	if(gui.Controls.IsHidden(idViewport))
		rtrn 0;
	cid_t idTitle = -1;
	cid_t idTarget = -1;
	if_fail_fe(idTitle = viewportControl(gui, idViewport, VIEWPORT_CONTROL_TITLE));
	if_fail_fe(idTarget = viewportControl(gui, idViewport, VIEWPORT_CONTROL_TARGET));
	cnst SControlState & titleState = gui.Controls.States[idTitle];
	if(input.ButtonDown(0) && titleState.IsPressed())
		if_fail_fe(controlToFront(gui, idViewport));
	if(titleState.IsDragging()) {
		n2s1_t mouseDeltas = (input.MouseCurrent.Position - input.MousePrevious.Position).s1();
		if(mouseDeltas.x || mouseDeltas.y) {
			mouseDeltas.InPlaceScale(1 / gui.Zoom.DPI.x, 1 / gui.Zoom.DPI.y);
			mouseDeltas.InPlaceScale(1 / gui.Zoom.ZoomLevel);
			SControlPlacement & placement = gui.Controls.Placement[idViewport];
			placement.Area.Offset.x += ((placement.Align & ALIGN_HCENTER) == ALIGN_RIGHT) ? -mouseDeltas.x : mouseDeltas.x;
			placement.Area.Offset.y += ((placement.Align & ALIGN_VCENTER) == ALIGN_BOTTOM) ? -mouseDeltas.y : mouseDeltas.y;
			if_fail_fe(controlMetricsInvalidate(gui, idViewport));
		}
		rtrn 0;
	}
	if(false == gui.Controls.IsHidden(idTarget))
		for(u2_t iResize = 0; iResize < VIEWPORT_CONTROL_TITLE; ++iResize) {
			VIEWPORT_CONTROL resizeControl = (VIEWPORT_CONTROL)iResize;
			cid_t idResize = -1;
			if_fail_fe(idResize = viewportControl(gui, idViewport, resizeControl));
			if(gui.Controls.States[idResize].IsDragging())
				rtrn ::viewportResize(gui, input, idViewport, resizeControl);
		}
	rtrn 0;
}

