#include "gpk_gui_viewport.h"
#include "gpk_gui_widgets.h"
#include "gpk_label.h"

::gpk::cid_t gpk::viewportCreate(::gpk::SGUI & gui, ::gpk::SViewport & viewport) {
	::gpk::SControlTable & controlTable = gui.Controls;
	gpk_necs(viewport.IdControl = ::gpk::controlCreate(gui));
	controlTable.Draw[viewport.IdControl].UseNewPalettes = true;
	controlTable.Draw[viewport.IdControl].FrameOut = true;
	controlTable.Placement[viewport.IdControl].Margin = {};
	{
		::gpk::cid_t idControl;
		gpk_necs(viewport.IdControls[VIEWPORT_CONTROL_TARGET] = idControl = ::gpk::controlCreateChild(gui, viewport.IdControl));
		::gpk::SControlPlacement & control = controlTable.Placement[idControl];
		control.Area.Offset = {0, 18 + 1};
		control.Area.Size = {2, 2};
		control.Margin = {};
		controlTable.Constraints[idControl].AttachSizeToControl.x = idControl;
	}
	{
		::gpk::cid_t idControl;
		gpk_necs(viewport.IdControls[VIEWPORT_CONTROL_TITLE] = idControl = ::gpk::controlCreateChild(gui, viewport.IdControl));
		::gpk::SControlPlacement & control = controlTable.Placement[idControl];
		control.Area.Offset = {};
		control.Area.Size = {2, 18};
		control.Align = ::gpk::ALIGN_TOP_LEFT;
		control.Margin.Left = 4;
		controlTable.Constraints[idControl].AttachSizeToControl.x = idControl;
		controlTable.Text[idControl].Text = "Viewport";
	}
	viewport.Settings.Unfolded = true;
	return viewport.IdControl;
}

gpk::err_t			gpk::viewportInitialize	(::gpk::SGUI & gui, ::gpk::SViewport & viewport)			{
	const ::gpk::n2u0_t			fontCharSize			= gui.Fonts[gui.SelectedFont]->CharSize;
	int16_t						heightTitleBar			= fontCharSize.y + 4;
	uint32_t					widthTarget				= 800;
	const ::gpk::n2f3_t			targetSize				= {(double)widthTarget, widthTarget * (9 / 16.0)};

	//if(-1 == viewport.IdControl)
	{
		viewport.IdControl		= ::gpk::controlCreate(gui);
		//::gpk::SControlMode			& controlMode			= gui.Controls.Modes		[viewport.IdControl];
		//controlMode.Design		= true;
		::gpk::SControlPlacement	& control				= gui.Controls.Placement	[viewport.IdControl];
		control.Align			= ::gpk::ALIGN_TOP_LEFT;
		control.Border = control.Margin	= {1, 1, 1, 1};

		const uint32_t				widthViewport			= (uint32_t)(targetSize.x + ::gpk::controlNCSpacing(control).x);
		const uint32_t				heightViewport			= (uint32_t)(targetSize.y + ((int64_t)heightTitleBar + ::gpk::controlNCSpacing(control).y));
		control.Area.Size		= {(int16_t)widthViewport, (int16_t)heightViewport};
		for(uint32_t iElement = 0; iElement < viewport.IdControls.size(); ++iElement) {
			gpk_necs(viewport.IdControls[iElement] = ::gpk::controlCreate(gui));
			::gpk::SControlConstraints	& controlConstraints	= gui.Controls.Constraints	[viewport.IdControls[iElement]];
			controlConstraints.AttachSizeToControl.x	= viewport.IdControls[iElement];
			gpk_necall(gpk::controlSetParent(gui, viewport.IdControls[iElement], viewport.IdControl), "iElement: %i, viewport.IdControl: %i", iElement, viewport.IdControl);
		}
		for(uint32_t iElement = 0; iElement < VIEWPORT_CONTROL_TITLE; ++iElement) {
			gui.Controls.Placement  [viewport.IdControls[iElement]].Area.Size			= {16, 16};
			gui.Controls.Placement  [viewport.IdControls[iElement]].Area.Offset			= {-2, -2};
			gui.Controls.Constraints[viewport.IdControls[iElement]].AttachSizeToControl	= {-1, -1};
		}
	}
	{
		gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_LEFT			]].Align = ::gpk::ALIGN_CENTER_LEFT		;
		gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_TOP			]].Align = ::gpk::ALIGN_CENTER_TOP		;
		gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_RIGHT			]].Align = ::gpk::ALIGN_CENTER_RIGHT	;
		gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_BOTTOM		]].Align = ::gpk::ALIGN_CENTER_BOTTOM	;
		gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_TOP_LEFT		]].Align = ::gpk::ALIGN_TOP_LEFT		;
		gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_TOP_RIGHT		]].Align = ::gpk::ALIGN_TOP_RIGHT		;
		gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_BOTTOM_LEFT	]].Align = ::gpk::ALIGN_BOTTOM_LEFT		;
		gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_BOTTOM_RIGHT	]].Align = ::gpk::ALIGN_BOTTOM_RIGHT	;

	}
	{
		::gpk::SControlPlacement	& control				= gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_CLOSE]];
		::gpk::SControlText			& controlText			= gui.Controls.Text		[viewport.IdControls[VIEWPORT_CONTROL_CLOSE]];
		control.Area.Size		= {(int16_t)heightTitleBar, (int16_t)heightTitleBar};
		control.Align			= ::gpk::ALIGN_TOP_RIGHT;
		controlText.Text		= {"X", (uint32_t)1};
		controlText.Align		= ::gpk::ALIGN_CENTER;
		::gpk::SControlConstraints	& controlConstraints	= gui.Controls.Constraints[viewport.IdControls[VIEWPORT_CONTROL_CLOSE]];
		controlConstraints.AttachSizeToControl	= {-1, -1};
	}
	{
		::gpk::SControlPlacement	& control				= gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_TITLE]];
		::gpk::SControlText			& controlText			= gui.Controls.Text		[viewport.IdControls[VIEWPORT_CONTROL_TITLE]];
		control.Area.Size.y		= heightTitleBar;
		static	int32_t				counterVPs				= 0;
		++counterVPs;
		sc_t						buffer [256];
		const int32_t				titleLen				= snprintf(buffer, ::gpk::size(buffer) - 2, "Viewport #%i", counterVPs);
		controlText.Text		= {::gpk::label(buffer).begin(), (uint32_t)titleLen};
		controlText.Align		= ::gpk::ALIGN_CENTER;
		control.Area.Offset.x	= heightTitleBar;
		control.Align			= ::gpk::ALIGN_TOP_RIGHT;
		::gpk::SControlConstraints	& controlConstraints	= gui.Controls.Constraints[viewport.IdControls[VIEWPORT_CONTROL_TITLE]];
		controlConstraints.DockToControl	= {-1, -1, viewport.IdControls[VIEWPORT_CONTROL_CLOSE], -1};
	}
	{
		::gpk::SControlDraw				& controlDraw		= gui.Controls.Draw		[viewport.IdControls[VIEWPORT_CONTROL_TARGET]];
		::gpk::SControlPlacement		& control			= gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_TARGET]];
		::gpk::SControlText				& controlText		= gui.Controls.Text		[viewport.IdControls[VIEWPORT_CONTROL_TARGET]];
		controlText.Align			= ::gpk::ALIGN_CENTER_TOP;
		control.Border				= {};
		control.Margin				= {};
		control.Area.Size			= targetSize.s1_t();
		//control.Area.Offset.y		= heightTitleBar;
		controlDraw.ColorTheme			= 60;
		::gpk::SControlConstraints		& controlConstraints	= gui.Controls.Constraints[viewport.IdControls[VIEWPORT_CONTROL_TARGET]];
		controlConstraints.DockToControl	= {-1, -1, -1, viewport.IdControls[VIEWPORT_CONTROL_TITLE]};
		//controlConstraints.AttachSizeToControl	= {-1, viewport.IdControl};
	}
	//for(uint32_t i = 0; i < 2; ++i)
	//	::gpk::controlUpdateMetricsTopToDown(gui, viewport.IdControl, gui.LastSize, true);
	return 0;
}

gpk::err_t gpk::viewportFold(::gpk::SGUI & gui, ::gpk::SViewport & control, bool fold) {
	if((fold && control.Settings.Unfolded) || (false == control.Settings.Unfolded && false == fold)) {
		::gpk::SControlTable & controlTable = gui.Controls;
		::gpk::SControlPlacement & placementMain = controlTable.Placement[control.IdControl];
		::gpk::SControlPlacement & placementTitle = controlTable.Placement[control.IdControls[VIEWPORT_CONTROL_TITLE]];
		if(fold && control.Settings.Unfolded)
			placementMain.Area.Size.y = placementTitle.Area.Size.y + ::gpk::controlNCSpacing(placementMain).y;
		else if(false == control.Settings.Unfolded && false == fold) {
			::gpk::SControlPlacement & controlClient = controlTable.Placement[control.IdControls[VIEWPORT_CONTROL_TARGET]];
			placementMain.Area.Size.y = controlClient.Area.Size.y + placementTitle.Area.Size.y + ::gpk::controlNCSpacing(placementMain).y + 1;
		}
		::gpk::SControlState & controlMain = controlTable.States[control.IdControl];
		if(-1 != controlMain.Parent)
			gpk_necs(gpk::controlMetricsInvalidate(gui, controlMain.Parent));
		else
			gpk_necs(gpk::controlMetricsInvalidate(gui, control.IdControl));
		::gpk::SControlState & stateClient = controlTable.States[control.IdControls[VIEWPORT_CONTROL_TARGET]];
		stateClient.SetHidden(control.IdControls[VIEWPORT_CONTROL_TARGET], controlTable.EventQueue, fold);
		control.Settings.Unfolded = !fold;
	}
	return one_if(false == control.Settings.Unfolded);
}

static gpk::err_t viewportDrag(::gpk::SViewport & control, ::gpk::SControlPlacement & controlMain, ::gpk::SGUI & gui, ::gpk::n2<bool> locked, ::gpk::n2s1_t mouseDeltas) {
	mouseDeltas.InPlaceScale(1 / gui.Zoom.DPI.x, 1 / gui.Zoom.DPI.y);
	mouseDeltas.InPlaceScale(1 / gui.Zoom.ZoomLevel);
	control.Settings.Dragging = true;
	if(false == locked.x)
		controlMain.Area.Offset.x += ((controlMain.Align & ::gpk::ALIGN_HCENTER) == ::gpk::ALIGN_RIGHT) ? -mouseDeltas.x : mouseDeltas.x;
	if(false == locked.y)
		controlMain.Area.Offset.y += ((controlMain.Align & ::gpk::ALIGN_VCENTER) == ::gpk::ALIGN_BOTTOM) ? -mouseDeltas.y : mouseDeltas.y;
	::gpk::controlMetricsInvalidate(gui, control.IdControl);
	return 0;
}

gpk::err_t gpk::viewportUpdate(::gpk::SGUI & gui, ::gpk::SInput & input, ::gpk::SViewport & control) {
	::gpk::SControlTable & controlTable = gui.Controls;
	::gpk::SControlPlacement & controlMain = controlTable.Placement[control.IdControl];
	::gpk::SControlState & controlTitleState = controlTable.States[control.IdControls[VIEWPORT_CONTROL_TITLE]];
	if(input.ButtonDown(0) && controlTitleState.IsPressed())
		gpk_necs(::gpk::controlToFront(gui, control.IdControl));
	if(controlTitleState.IsPressed()) {
		const ::gpk::n2<bool> locked = {control.Settings.DisplacementLockX, control.Settings.DisplacementLockY};
		if(false == locked.x || false == locked.y) {
			::gpk::n2s1_t mouseDeltas = (input.MouseCurrent.Position - input.MousePrevious.Position).s1_t();
			if(mouseDeltas.x || mouseDeltas.y)
				gpk_necs(::viewportDrag(control, controlMain, gui, locked, mouseDeltas));
		}
	}
	else {
		if(controlTitleState.IsAction() && false == control.Settings.Dragging)
			gpk_necs(gpk::viewportFold(gui, control, control.Settings.Unfolded));
		control.Settings.Dragging = false;
	}
	if(control.Settings.Unfolded != control.SettingsOld.Unfolded)
		if(true == control.Settings.Unfolded) {
			::gpk::n2s1_t clientFinalSize = {controlMain.Area.Size};
			clientFinalSize -= ::gpk::controlNCSpacing(controlMain).s1_t();
			clientFinalSize.y -= controlTable.Placement[control.IdControls[VIEWPORT_CONTROL_TITLE]].Area.Size.y + 1;
			::gpk::SControlPlacement & controlClient = controlTable.Placement[control.IdControls[VIEWPORT_CONTROL_TARGET]];
			controlClient.Area.Size = clientFinalSize;
		}
	control.SettingsOld = control.Settings;
	return 0;
}
