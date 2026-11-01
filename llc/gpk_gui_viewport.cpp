#include "gpk_gui_viewport.h"
#include "gpk_label.h"

gpk::err_t 		gpk::viewportInitialize	(::gpk::SGUI & gui, ::gpk::SViewport & viewport)			{
	const ::gpk::n2u0_t			fontCharSize			= gui.Fonts[gui.SelectedFont]->CharSize;
	s1_t						heightTitleBar			= fontCharSize.y + 4;
	u2_t					widthTarget				= 800;
	const ::gpk::n2f3_t			targetSize				= {(f3_t)widthTarget, widthTarget * (9 / 16.0)};

	//if(-1 == viewport.IdControl)
	{
		viewport.IdControl		= ::gpk::controlCreate(gui);
		//::gpk::SControlMode			& controlMode			= gui.Controls.Modes		[viewport.IdControl];
		//controlMode.Design		= true;
		::gpk::SControlPlacement	& control				= gui.Controls.Placement	[viewport.IdControl];
		control.Align			= ::gpk::ALIGN_TOP_LEFT;
		control.Border = control.Margin	= {1, 1, 1, 1};

		const u2_t				widthViewport			= (u2_t)(targetSize.x + ::gpk::controlNCSpacing(control).x);
		const u2_t				heightViewport			= (u2_t)(targetSize.y + ((s3_t)heightTitleBar + ::gpk::controlNCSpacing(control).y));
		control.Area.Size		= {(s1_t)widthViewport, (s1_t)heightViewport};
		for(u2_t iElement = 0; iElement < viewport.IdControls.size(); ++iElement) {
			if_fail_fe(viewport.IdControls[iElement] = ::gpk::controlCreate(gui));
			::gpk::SControlConstraints	& controlConstraints	= gui.Controls.Constraints	[viewport.IdControls[iElement]];
			controlConstraints.AttachSizeToControl.x	= viewport.IdControls[iElement];
			if_fail_fef(gpk::controlSetParent(gui, viewport.IdControls[iElement], viewport.IdControl), "iElement: %i, viewport.IdControl: %i", iElement, viewport.IdControl);
		}
		for(u2_t iElement = 0; iElement < VIEWPORT_CONTROL_TITLE; ++iElement) {
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
		control.Area.Size		= {(s1_t)heightTitleBar, (s1_t)heightTitleBar};
		control.Align			= ::gpk::ALIGN_TOP_RIGHT;
		controlText.Text		= {"X", (u2_t)1};
		controlText.Align		= ::gpk::ALIGN_CENTER;
		::gpk::SControlConstraints	& controlConstraints	= gui.Controls.Constraints[viewport.IdControls[VIEWPORT_CONTROL_CLOSE]];
		controlConstraints.AttachSizeToControl	= {-1, -1};
	}
	{
		::gpk::SControlPlacement	& control				= gui.Controls.Placement[viewport.IdControls[VIEWPORT_CONTROL_TITLE]];
		::gpk::SControlText			& controlText			= gui.Controls.Text		[viewport.IdControls[VIEWPORT_CONTROL_TITLE]];
		control.Area.Size.y		= heightTitleBar;
		static	s2_t				counterVPs				= 0;
		++counterVPs;
		sc_t						buffer [256];
		const s2_t				titleLen				= snprintf(buffer, ::gpk::size(buffer) - 2, "Viewport #%i", counterVPs);
		controlText.Text		= {::gpk::label(buffer).begin(), (u2_t)titleLen};
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
		control.Area.Size			= targetSize.s1();
		//control.Area.Offset.y		= heightTitleBar;
		controlDraw.ColorTheme			= 60;
		::gpk::SControlConstraints		& controlConstraints	= gui.Controls.Constraints[viewport.IdControls[VIEWPORT_CONTROL_TARGET]];
		controlConstraints.DockToControl	= {-1, -1, -1, viewport.IdControls[VIEWPORT_CONTROL_TITLE]};
		//controlConstraints.AttachSizeToControl	= {-1, viewport.IdControl};
	}
	//for(u2_t i = 0; i < 2; ++i)
	//	::gpk::controlUpdateMetricsTopToDown(gui, viewport.IdControl, gui.LastSize, true);
	return 0;
}
