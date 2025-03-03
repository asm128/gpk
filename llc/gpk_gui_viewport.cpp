#include "gpk_gui_viewport.h"
#include "gpk_label.h"

::gpk::error_t			gpk::viewportInitialize	(::gpk::SGUI & gui, ::gpk::SViewport & viewport)			{
	const ::gpk::n2u8			fontCharSize			= gui.Fonts[gui.SelectedFont]->CharSize;
	int16_t						heightTitleBar			= fontCharSize.y + 4;
	uint32_t					widthTarget				= 800;
	const ::gpk::n2f64			targetSize				= {(double)widthTarget, widthTarget * (9 / 16.0)};

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
