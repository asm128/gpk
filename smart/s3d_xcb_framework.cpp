#include "s3d_xcb_framework.h"

#if !defined(GPK_WINDOWS)
static ::gpk::error_t		create_window	(xcb_connection_t * connection, xcb_screen_t * screen, ::gpk::SDisplay & window) {
	::std::shared_ptr<xcb_get_geometry_reply_t>
		geometry	(xcb_get_geometry_reply(connection, xcb_get_geometry(connection, screen->root), nullptr), free);
	window.Size									= {geometry->width, geometry->height};
	window.PlatformDetail.Connection			= connection;
	window.PlatformDetail.IdDrawableBackPixmap	= xcb_generate_id(connection);
	xcb_create_pixmap(connection, screen->root_depth, window.PlatformDetail.IdDrawableBackPixmap, screen->root, window.Size.x, window.Size.y);
	{	// create graphics context
		window.PlatformDetail.GC		= xcb_generate_id(connection);
		const uint32_t						mask		= XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_GRAPHICS_EXPOSURES;
		const uint32_t						values[3]	= {screen->black_pixel, screen->white_pixel, 0};
		xcb_create_gc(connection, window.PlatformDetail.GC, screen->root, mask, values);
	}

	{	// create the window
		window.PlatformDetail.IdDrawable			= xcb_generate_id(connection);
		const uint32_t					mask		= XCB_CW_BACK_PIXMAP | XCB_CW_EVENT_MASK;
		const uint32_t					values[2]	= { window.PlatformDetail.IdDrawableBackPixmap
			, XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_STRUCTURE_NOTIFY
			| XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_KEY_PRESS  | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_BUTTON_MOTION | XCB_EVENT_MASK_POINTER_MOTION
		};
		xcb_create_window
			( connection						// connection
			, screen->root_depth				// depth
			, window.PlatformDetail.IdDrawable	// window Id
			, screen->root						// parent window
			, 0, 0								// x, y
			, window.Size.x, window.Size.y		// width, height
			, 0									// border_width
			, XCB_WINDOW_CLASS_INPUT_OUTPUT		// class
			, screen->root_visual				// visual
			, mask, values						// masks
			);
		xcb_map_window(connection, window.PlatformDetail.IdDrawable);
	}
	{	// context for filling with white
		xcb_gcontext_t				fill		= xcb_generate_id(connection);
		const uint32_t				mask		= XCB_GC_FOREGROUND;
		const uint32_t				values[2]	= {0xFF00FF,};
		xcb_create_gc(connection, fill, window.PlatformDetail.IdDrawableBackPixmap, mask, values);

		const xcb_rectangle_t		r1			= {0, 0, (uint16_t)window.Size.x, (uint16_t)window.Size.y};
		xcb_poly_fill_rectangle(connection, window.PlatformDetail.IdDrawableBackPixmap, fill, 1, &r1);
		xcb_free_gc(connection, fill);
	}
	return 0;
}
::gpk::error_t				s3d::mainWindowCreate				(::s3d::SXCBFramework & framework) {
	if(0 == framework.PlatformDetail.XCBConnection) {
		framework.PlatformDetail.XCBConnection	= xcb_connect(NULL, NULL);
		framework.PlatformDetail.XCBScreen		= xcb_setup_roots_iterator(xcb_get_setup(framework.PlatformDetail.XCBConnection)).data;
	}
	return ::create_window(framework.PlatformDetail.XCBConnection, framework.PlatformDetail.XCBScreen, framework.MainDisplay);
}
#else
::gpk::error_t				s3d::mainWindowCreate				(::s3d::SXCBFramework & framework) { (void)framework; return 0; }
#endif

::gpk::error_t				s3d::frameworkUpdate				(::s3d::SXCBFramework & framework) {
#ifndef GPK_WINDOWS
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>
										& guiRenderTarget			= framework.MainDisplayOffscreen;
	const ::gpk::SCoord2<uint32_t>		& guiRenderTargetMetrics	= guiRenderTarget->Color.View.metrics();
	::gpk::SInput						& input						= *framework.Input;
	if(framework.MainDisplay.Repaint) {
		const xcb_image_format_t			format						= XCB_IMAGE_FORMAT_Z_PIXMAP;
		const ::gpk::SCoord2<uint32_t>		& guiRenderTargetMetrics	= guiRenderTarget->Color.View.metrics();
		xcb_image_t							* image						= xcb_image_create_native
			( framework.PlatformDetail.XCBConnection
			, guiRenderTargetMetrics.x
			, guiRenderTargetMetrics.y
			, format
			, framework.PlatformDetail.XCBScreen->root_depth
			, (uint8_t*)guiRenderTarget->Color.View.begin()
			, guiRenderTarget->Color.View.size() * 4
			, (uint8_t*)guiRenderTarget->Color.View.begin()
			);
		xcb_image_put(framework.PlatformDetail.XCBConnection, framework.MainDisplay.PlatformDetail.IdDrawable, framework.MainDisplay.PlatformDetail.GC, image, 0, 0, 0);
		image->base						= 0;
		xcb_image_destroy(image);
		xcb_flush(framework.PlatformDetail.XCBConnection);
		framework.MainDisplay.Repaint	= false;
	}
	input.KeyboardPrevious			= input.KeyboardCurrent;
	input.MousePrevious				= input.MouseCurrent;
	input.MouseCurrent.Deltas		= {};
	while (xcb_generic_event_t * ev = xcb_poll_for_event(framework.PlatformDetail.XCBConnection)) {
		switch (ev->response_type & ~0x80) {
		default					: break;
		case XCB_EXPOSE			: {
			//xcb_expose_event_t												* x					= (xcb_expose_event_t *)ev;
			framework.MainDisplay.Repaint	= true;
			break;
		}
		case XCB_KEY_PRESS		: { framework.MainDisplay.Repaint = true; const xcb_key_press_event_t * iev = (xcb_key_press_event_t *)ev; info_printf("Key %i Down"	, iev->detail); input.KeyboardCurrent.KeyState	  [iev->detail] = 1; break;		}
		case XCB_KEY_RELEASE	: { framework.MainDisplay.Repaint = true; const xcb_key_press_event_t * iev = (xcb_key_press_event_t *)ev; info_printf("Key %i Up"		, iev->detail); input.KeyboardCurrent.KeyState	  [iev->detail] = 0; break;		}
		case XCB_BUTTON_PRESS	: { framework.MainDisplay.Repaint = true; const xcb_key_press_event_t * iev = (xcb_key_press_event_t *)ev; info_printf("Button %i Down"	, iev->detail); input.MouseCurrent   .ButtonState[iev->detail - 1] = 1; break;	}
		case XCB_BUTTON_RELEASE	: { framework.MainDisplay.Repaint = true; const xcb_key_press_event_t * iev = (xcb_key_press_event_t *)ev; info_printf("Button %i Up"	, iev->detail); input.MouseCurrent   .ButtonState[iev->detail - 1] = 0; break;	}
		case XCB_MOTION_NOTIFY	: {
			xcb_motion_notify_event_t			* mev				= (xcb_motion_notify_event_t*)ev;
			int32_t								xPos				= mev->same_screen ? mev->event_x : mev->root_x;
			int32_t								yPos				= mev->same_screen ? mev->event_y : mev->root_y;
			input.MouseCurrent.Position.x	= ::gpk::clamp(xPos, 0, (int32_t)guiRenderTargetMetrics.x);
			input.MouseCurrent.Position.y	= ::gpk::clamp(yPos, 0, (int32_t)guiRenderTargetMetrics.y);
			input.MouseCurrent.Deltas.x		= input.MouseCurrent.Position.x - input.MousePrevious.Position.x;
			input.MouseCurrent.Deltas.y		= input.MouseCurrent.Position.y - input.MousePrevious.Position.y;
			info_printf("Mouse position: %i, %i", input.MouseCurrent.Position.x, input.MouseCurrent.Position.y);
			framework.MainDisplay.Repaint	= true;
			break;
		}
		}
	}
	::gpk::STimer							& timer					= framework.Timer;
	timer.Frame();

	::gpk::SGUI								& gui					= *framework.GUI;
	::gpk::guiProcessInput(gui, input);
#else
	(void)framework;
#endif
	return 0;
}
