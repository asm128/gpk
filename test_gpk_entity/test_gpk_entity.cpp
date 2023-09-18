// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "test_gpk_entity.h"
#include "gpk_sun.h"

#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_view_bit.h"
#include "gpk_matrix.h"
#include "gpk_png.h"

#include "gpk_app_impl.h"
#include "gpk_bitmap_target.h"

#include <DirectXColors.h>

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "GPK Entity Test v0.1");

// --- Cleanup application resources.
::gpk::error_t			cleanup					(::SApplication & app)											{
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::SWindow				& mainWindow			= framework.RootWindow;
#if !defined(DISABLE_D3D11)
	app.D3DApp.Shutdown();
#endif

	gpk_necs(gpk::mainWindowDestroy(mainWindow));
	return 0;
}

static	::gpk::error_t	updateSizeDependentResources(::SApplication & app)											{
	::gpk::SWindow				& mainWindow			= app.Framework.RootWindow;
	const ::gpk::n2u16			newSize					= mainWindow.Size;
#if !defined(DISABLE_D3D11)
	mainWindow.BackBuffer	= {};
	gpk_necs(app.D3DApp.SetWindowSize(newSize));
#else
	gpk_necs(mainWindow.BackBuffer->resize(newSize, ::gpk::bgra{0, 0, 0, 0}, 0xFFFFFFFF));
#endif
	mainWindow.Resized		= false;
	return 0;
}

static	::gpk::error_t	processScreenEvent		(::SApplication & app, const ::gpk::SEventView<::gpk::EVENT_SCREEN> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_SCREEN_Create:
#if !defined(DISABLE_D3D11)
		gpk_necs(app.D3DApp.Initialize(app.Framework.RootWindow.PlatformDetail.WindowHandle, app.Engine.Scene->Graphics));
#endif
	case ::gpk::EVENT_SCREEN_Resize: 
		gpk_necs(::updateSizeDependentResources(app));
		break;
	}
	return 0;
}

static	::gpk::error_t	processGUIEvent			(::SApplication & /*app*/, const ::gpk::SEventView<::gpk::EVENT_GUI_CONTROL> & screenEvent) { 
	switch(screenEvent.Type) {
	default: break;
	case ::gpk::EVENT_GUI_CONTROL_StateChange: {
		const ::gpk::SChangeControlState	stateChange	= *(const ::gpk::SChangeControlState*)screenEvent.Data.begin();
		if(0 == (stateChange.Set & ::gpk::GUI_CONTROL_FLAG_Action))
			return 0;
	}
	}
	return 0;
}

static	::gpk::error_t	processSystemEvent		(::SApplication & app, const ::gpk::SEventSystem & sysEvent) { 
	switch(sysEvent.Type) {
	default: break;
	case ::gpk::SYSTEM_EVENT_Screen	: return ::gpk::eventExtractAndHandle<::gpk::EVENT_SCREEN		>(sysEvent, [&app](const ::gpk::SEventView<::gpk::EVENT_SCREEN		> & screenEvent) { return processScreenEvent(app, screenEvent); }); 
	case ::gpk::SYSTEM_EVENT_GUI	: return ::gpk::eventExtractAndHandle<::gpk::EVENT_GUI_CONTROL	>(sysEvent, [&app](const ::gpk::SEventView<::gpk::EVENT_GUI_CONTROL	> & screenEvent) { return processGUIEvent	(app, screenEvent); }); 
	}
	return 0;
}

::gpk::error_t			setup					(::SApplication& app)											{
#if !defined(DISABLE_D3D11)
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
#endif
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::SWindow				& mainWindow			= framework.RootWindow;
	mainWindow.Size			= {1280, 720};
	gpk_necs(gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input));

	return 0;
}

::gpk::error_t			update					(::SApplication& app, bool systemRequestedExit)					{
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::SWindow				& mainWindow			= app.Framework.RootWindow;
	::gpk::SGUI					& gui					= *framework.GUI;

	bool						systemExit				= false;

	::gpk::apobj<::gpk::SEventSystem>	eventsToProcess	= mainWindow.EventQueue;
	gpk_necs(eventsToProcess.append(gui.Controls.EventQueue));
	gpk_necs(eventsToProcess.for_each([&app, &systemExit](const ::gpk::pobj<::gpk::SEventSystem> & sysEvent) { 
		::gpk::error_t				result; 
		gpk_necs(result = ::processSystemEvent(app, *sysEvent)); 
		if(result == 1) 
			systemExit				= true; 
		return result;
	}));

	rvi_if(::gpk::APPLICATION_STATE_EXIT, systemExit || systemRequestedExit, "%s || %s", ::gpk::bool2char(systemExit), ::gpk::bool2char(systemRequestedExit));

	::gpk::SFrameInfo			& frameInfo				= framework.FrameInfo;
	{
		::gpk::STimer				timer					= {};
		gpk_necs(app.Engine.Update(frameInfo.Seconds.LastFrame));
		timer.Frame();
		//info_printf("Update engine in %f seconds", timer.LastTimeSeconds);
	}

#if !defined(DISABLE_D3D11)
	if(app.D3DApp.Scene.IndexBuffer.size() < app.Engine.Scene->Graphics->Meshes.size() || !app.D3DApp.GUIStuff.IndexBuffer) {
		gpk_necs(app.D3DApp.CreateDeviceResources(*app.Engine.Scene->Graphics));
	}
	app.D3DApp.Text.Update(frameInfo.Seconds.LastFrame, frameInfo.Seconds.Total, (uint32_t)frameInfo.FrameMeter.FramesPerSecond);
#endif

	//-----------------------------
	::gpk::STimer				& timer					= app.Framework.Timer;
	char						buffer	[256]			= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND						windowHandle			= mainWindow.PlatformDetail.WindowHandle;
	SetWindowTextA(windowHandle, buffer);
	
	return ::gpk::updateFramework(framework);
}

#ifdef DISABLE_D3D11
static	::gpk::error_t	sceneDraw				
	( const ::gpk::SEngineScene	& engineScene
	, ::gpk::SEngineRenderCache	& renderCache
	, ::gpk::rtbgra8d32		& backBuffer
	, const ::gpk::n3f32	& cameraPosition
	, const ::gpk::n3f32	& cameraTarget
	, const ::gpk::n3f32	& cameraUp
	, double				totalSeconds
	) {

	const ::gpk::n2u16			offscreenMetrics	= backBuffer.Color.View.metrics16();

	::gpk::SEngineSceneConstants	constants		= {};
	constants.CameraPosition	= cameraPosition;
	constants.CameraFront		= (cameraTarget - cameraPosition).Normalized();
	constants.LightPosition		= {0, 10, 0};
	constants.LightDirection	= {0, -1, 0};

	::gpk::minmax				nearFar 			= {.0001f, 10.0f}; 

	constants.View.LookAt(cameraPosition, cameraTarget, cameraUp);
	constants.Perspective.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar.Min, nearFar.Max);
	constants.Screen.ViewportLH(offscreenMetrics);
	constants.VP			= constants.View * constants.Perspective;
	constants.VPS			= constants.VP * constants.Screen;

	::gpk::STimer				timer;
	

	::gpk::an3f32				& wireframePixelCoords	= renderCache.VertexShaderCache.WireframePixelCoords;
	for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord) {
		::gpk::n3i16				coord		= wireframePixelCoords[iCoord].i16();
		::gpk::rgbaf				color		= 
			{ (float)(totalSeconds - iCoord / 1.0f / totalSeconds)
			, (float)(totalSeconds - iCoord / 2.0f / totalSeconds)
			, (float)(totalSeconds - iCoord / 3.0f / totalSeconds)
			};
		backBuffer.Color.View[coord.y][coord.x]	= color;
	}

	::gpk::drawScene(backBuffer.Color.View, backBuffer.DepthStencil.View, renderCache, engineScene, constants);
	timer.Frame();
	info_printf("Render scene in %f seconds", timer.LastTimeSeconds);
	return 0;
}
#endif
::gpk::error_t			draw					(::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	const ::gpk::n3f32			sunlightPos				= ::gpk::calcSunPosition();
	//const double				sunlightFactor			= ::gpk::calcSunlightFactor(app.SSiegeApp.World.WorldState.DaylightRatioExtra, app.SSiegeApp.World.WorldState.DaylightOffsetMinutes);
	const ::gpk::rgbaf			clearColor				= ::gpk::interpolate_linear(::gpk::DARKBLUE * .25, ::gpk::LIGHTBLUE * 1.1, 1/*sunlightFactor*/);

	::gpk::SCameraPoints		cameraSelected			= {};
	cameraSelected.Position	= {50, 10, 50};
	cameraSelected.Target	= {};

	const ::gpk::SEngineScene	& engineScene			= *app.Engine.Scene;

#if !defined(DISABLE_D3D11) 
	memset(app.D3DApp.GUIStuff.RenderTarget.begin(), 0, app.D3DApp.GUIStuff.RenderTarget.byte_count());
	
	gpk_necs(gpk::guiDraw(*app.Framework.GUI, app.D3DApp.GUIStuff.RenderTarget));

	gpk_necs(gpk::d3dAppDraw(app.D3DApp, engineScene, clearColor, sunlightPos, cameraSelected.Position, cameraSelected.Target, {.01f, 1000.0f}));
#else 
	::gpk::SFramework			& framework				= app.Framework;
	::gpk::prtbgra8d32			backBuffer				= framework.RootWindow.BackBuffer;
	backBuffer->resize(framework.RootWindow.BackBuffer->Color.metrics(), clearColor, (uint32_t)-1);
	::sceneDraw(engineScene, app.Engine.Scene->RenderCache, *backBuffer, cameraSelected.Position, cameraSelected.Target, {0, 1}, 0);
	gpk_necs(gpk::guiDraw(*app.Framework.GUI, backBuffer->Color));
	memcpy(framework.RootWindow.BackBuffer->Color.View.begin(), backBuffer->Color.View.begin(), backBuffer->Color.View.byte_count());
	//::gpk::grid_mirror_y(framework.RootWindow.BackBuffer->Color.View, backBuffer->Color.View);
	//framework.RootWindow.BackBuffer		= backBuffer;
#endif
	return 0;
}
