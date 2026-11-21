#include "gpk_gui_viewport.h"
#include "gpk_label.h"

gpk::err_t 		gpk::viewportInitialize	(SGUI & gui, SViewport & viewport)			{
	cnst n2u0_t			fontCharSize			= gui.Fonts[gui.SelectedFont]->CharSize;
	s1_t						heightTitleBar			= fontCharSize.y + 4;
	u2_t					widthTarget				= 800;
	cnst n2f3_t			targetSize				= {(f3_t)widthTarget, widthTarget * (9 / 16.0)};

	//if(-1 == viewport.IdControl)
	{
		viewport.IdControl		= controlCreate(gui);
		//SControlMode			& controlMode			= gui.Controls.Modes		[viewport.IdControl];
		//controlMode.Design		= true;
		SControlPlacement	& control				= gui.Controls.Placement	[viewport.IdControl];
		control.Align			= ALIGN_TOP_LEFT;
		control.Border = control.Margin	= {1, 1, 1, 1};

		u2_c				widthViewport			= (u2_t)(targetSize.x + controlNCSpacing(control).x);
		u2_c				heightViewport			= (u2_t)(targetSize.y + ((s3_t)heightTitleBar + controlNCSpacing(control).y));
		control.Area.Size		= {(s1_t)widthViewport, (s1_t)heightViewport};
		for(u2_t iElement = 0; iElement < viewport.IdControls.size(); ++iElement) {
			if_fail_fe(viewport.IdControls[iElement] = controlCreate(gui));
			SControlConstraints	& controlConstraints	= gui.Controls.Constraints	[viewport.IdControls[iElement]];
			controlConstraints.AttachSizeToControl.x	= viewport.IdControls[iElement];
			if_fail_fef(controlSetParent(gui.Controls, viewport.IdControls[iElement], viewport.IdControl), "iElement: %i, viewport.IdControl: %i", iElement, viewport.IdControl);
		}
		for(u2_t iElement = 0; iElement < VIEWPORT_CONTROL_TITLE; ++iElement) {
			gui.Controls.Placement  [viewport.IdControls[iElement]].Area.Size			= {16, 16};
			gui.Controls.Placement  [viewport.IdControls[iElement]].Area.Offset			= {-2, -2};
			gui.Controls.Constraints[viewport.IdControls[iElement]].AttachSizeToControl	= {-1, -1};
		}
	}
	{
		gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_LEFT			]].Align = ALIGN_CENTER_LEFT		;
		gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_TOP			]].Align = ALIGN_CENTER_TOP		;
		gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_RIGHT			]].Align = ALIGN_CENTER_RIGHT	;
		gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_BOTTOM		]].Align = ALIGN_CENTER_BOTTOM	;
		gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_TOP_LEFT		]].Align = ALIGN_TOP_LEFT		;
		gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_TOP_RIGHT		]].Align = ALIGN_TOP_RIGHT		;
		gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_BOTTOM_LEFT	]].Align = ALIGN_BOTTOM_LEFT		;
		gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_RESIZE_BOTTOM_RIGHT	]].Align = ALIGN_BOTTOM_RIGHT	;

	}
	{
		SControlPlacement	& control				= gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_CLOSE]];
		SControlText			& controlText			= gui.Controls.Text		[viewport.IdControls[VIEWPORT_CONTROL_CLOSE]];
		control.Area.Size		= {(s1_t)heightTitleBar, (s1_t)heightTitleBar};
		control.Align			= ALIGN_TOP_RIGHT;
		controlText.Text		= {"X", (u2_t)1};
		controlText.Align		= ALIGN_CENTER;
		SControlConstraints	& controlConstraints	= gui.Controls.Constraints[viewport.IdControls[VIEWPORT_CONTROL_CLOSE]];
		controlConstraints.AttachSizeToControl	= {-1, -1};
	}
	{
		SControlPlacement	& control				= gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_TITLE]];
		SControlText			& controlText			= gui.Controls.Text		[viewport.IdControls[VIEWPORT_CONTROL_TITLE]];
		control.Area.Size.y		= heightTitleBar;
		sttc	s2_t				counterVPs				= 0;
		++counterVPs;
		sc_t						buffer [256];
		cnst s2_t				titleLen				= snprintf(buffer, size(buffer) - 2, "Viewport #%i", counterVPs);
		controlText.Text		= {label(buffer).begin(), (u2_t)titleLen};
		controlText.Align		= ALIGN_CENTER;
		control.Area.Offset.x	= heightTitleBar;
		control.Align			= ALIGN_TOP_RIGHT;
		SControlConstraints	& controlConstraints	= gui.Controls.Constraints[viewport.IdControls[VIEWPORT_CONTROL_TITLE]];
		controlConstraints.DockToControl	= {-1, -1, viewport.IdControls[VIEWPORT_CONTROL_CLOSE], -1};
	}
	{
		SControlDraw				& controlDraw		= gui.Controls.Draw		[viewport.IdControls[VIEWPORT_CONTROL_TARGET]];
		SControlPlacement		& control			= gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_TARGET]];
		SControlText				& controlText		= gui.Controls.Text		[viewport.IdControls[VIEWPORT_CONTROL_TARGET]];
		controlText.Align			= ALIGN_CENTER_TOP;
		control.Border				= control.Margin				= {};
		control.Area.Size			= targetSize.s1();
		//control.Area.Offset.y		= heightTitleBar;
		controlDraw.ColorTheme			= 60;
		SControlConstraints		& controlConstraints	= gui.Controls.Constraints[viewport.IdControls[VIEWPORT_CONTROL_TARGET]];
		controlConstraints.DockToControl	= {-1, -1, -1, viewport.IdControls[VIEWPORT_CONTROL_TITLE]};
		//controlConstraints.AttachSizeToControl	= {-1, viewport.IdControl};
	}
	//for(u2_t i = 0; i < 2; ++i)
	//	controlUpdateMetricsTopToDown(gui, viewport.IdControl, gui.Controls.LastSize, true);
	return 0;
}
