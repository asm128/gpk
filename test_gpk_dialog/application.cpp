#include "application.h"
#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

			::gpk::error_t											cleanup						(::gme::SApplication & app)						{ return ::gpk::mainWindowDestroy(app.Framework.MainDisplay); }
			::gpk::error_t											setup						(::gme::SApplication & app)						{ 
	::gpk::SFramework														& framework					= app.Framework;
	::gpk::SDisplay															& mainWindow				= framework.MainDisplay;
	framework.Input.create();
	app.DialogMain.Input = framework.Input;
	error_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window. %s.", "why?????!?!?!?!?");
	::gpk::SGUI																& gui						= framework.GUI;
	gui.ColorModeDefault												= ::gpk::GUI_COLOR_MODE_3D;
	gui.ThemeDefault													= ::gpk::ASCII_COLOR_DARKGREEN * 16 + 7;
	app.IdExit															= ::gpk::controlCreate(gui);
	::gpk::SControl															& controlExit				= gui.Controls.Controls[app.IdExit];
	controlExit.Area													= {{}, {64, 20}};
	controlExit.Border													= {10, 10, 10, 10};
	controlExit.Margin													= {1, 1, 1, 1};
	controlExit.Align													= ::gpk::ALIGN_BOTTOM_RIGHT;
	::gpk::SControlText														& controlText				= gui.Controls.Text[app.IdExit];
	controlText.Text													= "Exit";
	controlText.Align													= ::gpk::ALIGN_CENTER;
	::gpk::SControlConstraints												& controlConstraints		= gui.Controls.Constraints[app.IdExit];
	controlConstraints.AttachSizeToControl								= {app.IdExit, -1};
	::gpk::controlSetParent(gui, app.IdExit, -1);

	//app.PNGImages.resize(::gpk::size(filenames));
	//::gpk::SPNGData															pngDataCacheForFasterLoad;
	//for(uint32_t iFile = 0; iFile < app.PNGImages.size(); ++iFile) 
	//	error_if(errored(::gpk::pngFileLoad(pngDataCacheForFasterLoad, filenames[iFile], app.PNGImages[iFile])), "Failed to load file: %s.", filenames[iFile].begin());

	::gpk::ptr_obj<::gpk::SDialogTuner>										tuner						= {};
	app.NumericTuner													= ::gpk::tunerCreate(app.DialogMain, tuner);
	tuner->ValueLimits.Min												= 100;
	tuner->ValueLimits.Max												= 200;
	app.DialogMain.GUI.Controls.Controls[tuner->IdGUIControl].Area.Offset	= {128, 192};
	app.DialogMain.GUI.Controls.Controls[tuner->IdGUIControl].Area.Size.x	= 128;
	app.DialogMain.GUI.Controls.Controls[tuner->IdGUIControl].Area.Size.y	=  20;
	::gpk::tunerSetValue(*tuner, 0);

	::gpk::ptr_obj<::gpk::SDialogSlider>									slider						= {};
	app.Slider															= ::gpk::sliderCreate(app.DialogMain, slider);
	slider->ValueLimits.Min												= 0;
	slider->ValueLimits.Max												= 255;
	app.DialogMain.GUI.Controls.Controls[slider->IdGUIControl].Area.Offset	= {128, 128};
	app.DialogMain.GUI.Controls.Controls[slider->IdGUIControl].Area.Size.x	= 128;
	app.DialogMain.GUI.Controls.Controls[slider->IdGUIControl].Area.Size.y	= 8;

	::gpk::ptr_obj<::gpk::SDialogCheckBox>									checkbox					= {};
	app.CheckBox														= ::gpk::checkBoxCreate(app.DialogMain, checkbox);
	app.DialogMain.GUI.Controls.Controls[checkbox->IdGUIControl].Area.Offset	= {128, 256};
	return 0; 
}

			::gpk::error_t											update						(::gme::SApplication & app, bool exitSignal)	{ 
	//::gpk::STimer															timer;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "%s", "Exit requested by runtime.");
	{
		::gpk::mutex_guard														lock						(app.LockRender);
		app.Framework.MainDisplayOffscreen									= app.Offscreen;
	}
	::gpk::SFramework														& framework					= app.Framework;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "%s", "Exit requested by framework update.");

	::gpk::SGUI																& gui						= framework.GUI;
	{
		::gpk::mutex_guard														lock						(app.LockGUI);
		::gpk::guiProcessInput(gui, *app.Framework.Input);
	}
	if(app.Framework.Input->MouseCurrent.Deltas.z) {
		::gpk::mutex_guard														lock						(app.LockGUI);
		if(app.Framework.Input->MouseCurrent.Deltas.z > 0) {
			if(gui.Zoom.ZoomLevel > 1)
				++gui.Zoom.ZoomLevel;				
			else
				gui.Zoom.ZoomLevel			*= 2;

		}
		else {
			if(gui.Zoom.ZoomLevel > 1)
				--gui.Zoom.ZoomLevel;			
			else
				gui.Zoom.ZoomLevel			*= .5;
		}
		::gpk::guiUpdateMetrics(gui, framework.MainDisplay.Size, true);
	}
 
	for(uint32_t iControl = 0, countControls = gui.Controls.Controls.size(); iControl < countControls; ++iControl) {
		const ::gpk::SControlState												& controlState				= gui.Controls.States[iControl];
		if(controlState.Unused || controlState.Disabled)
			continue;
		if(controlState.Execute) {
			info_printf("Executed %u.", iControl);
			if(iControl == (uint32_t)app.IdExit)
				return 1;
		}
	}

	app.DialogMain.GUI.Zoom		= gui.Zoom;
	app.DialogMain.Update();

	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0; 
}

			::gpk::error_t											draw					(::gme::SApplication & app)							{ 
	//::gpk::STimer															timer;
	app;
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>		target;
	target.create();
	//target->resize(app.Framework.MainDisplay.Size, (::gpk::DARKBLUE + (::gpk::DARKRED * .5)) * 2.0, 0xFFFFFFFFU);
	target->resize(app.Framework.MainDisplay.Size, ::gpk::LIGHTGRAY, 0xFFFFFFFFU);
	//for(uint32_t y = 0; y < target->Color.View.metrics().y; ++y) 
	//for(uint32_t x = 0; x < target->Color.View.metrics().x; ++x) {
	//	target->Color.View[y][x]											= rand();
	//	target->Color.View[y][x].a											= 255;
	//}

	//for(uint32_t iFile = 0; iFile < app.PNGImages.size(); ++iFile) {
	//	::gpk::SCoord2<uint32_t>												position				= {(iFile * 64) % (target->Color.View.metrics().x - 64), (iFile * 64) / (target->Color.View.metrics().x - 64) * 64};
	//	::gpk::grid_copy_blend(target->Color.View, app.PNGImages[iFile].View, position);
	//	//::gpk::grid_scale_alpha(target->Color.View, app.PNGImages[iFile].View, position.Cast<int32_t>(), app.PNGImages[iFile].View.metrics().Cast<int32_t>() * (1 + (.01 * iFile)));
	//}

	//::gpk::array_pod<ubyte_t>												bytesPNG				= 0;
	//::gpk::pngFileWrite(target->Color.View, bytesPNG);
	//FILE																	* fp					= 0;
	//fopen_s(&fp, "png_test_generated.png", "wb");
	//if(fp) {
	//	fwrite(bytesPNG.begin(), 1, bytesPNG.size(), fp);
	//	fclose(fp);
	//}

	//::gpk::clearTarget(*target);
	{
		::gpk::mutex_guard														lock					(app.LockGUI);
		::gpk::controlDrawHierarchy(app.Framework.GUI, 0, target->Color.View);
	}
	{
		::gpk::mutex_guard														lock					(app.LockGUI);
		::gpk::guiDraw(app.DialogMain.GUI, target->Color.View);
	}
	{
		::gpk::mutex_guard														lock					(app.LockRender);
		app.Offscreen														= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	return 0; 
}