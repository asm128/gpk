#include "application.h"
#include "gpk_png.h"
#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_encoding.h"
#include "gpk_json_expression.h"
#include "gpk_file.h"
#include "gpk_path.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "PNG Test");

::gpk::err_t			cleanup			(::gme::SApplication & app)	{ return ::gpk::mainWindowDestroy(app.Framework.RootWindow); }
::gpk::err_t			setup			(::gme::SApplication & app)	{
	::gpk::SFramework			& framework		= app.Framework;
	::gpk::SWindow				& mainWindow	= framework.RootWindow;
	mainWindow.Size			= {1280, 720};
	if_true_e(::gpk::failed(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, mainWindow.Input)));
	{ // Build the exit button
		::gpk::SGUI					& gui			= *framework.GUI;
		gui.ColorModeDefault	= ::gpk::GUI_COLOR_MODE_3D;
		gui.ThemeDefault		= ::gpk::ASCII_COLOR_Dark_green * 16 + 7;
		app.IdExit				= ::gpk::controlCreate(gui);
		::gpk::SControlPlacement	& controlExit	= gui.Controls.Placement[app.IdExit];
		controlExit.Area		= {{}, {64, 20}};
		controlExit.Border		= {10, 10, 10, 10};
		controlExit.Margin		= {1, 1, 1, 1};
		controlExit.Align		= ::gpk::ALIGN_BOTTOM_RIGHT;
		::gpk::SControlText			& controlText	= gui.Controls.Text[app.IdExit];
		controlText.Text		= "Exit";
		controlText.Align		= ::gpk::ALIGN_CENTER;
		::gpk::SControlConstraints	& controlConstraints	= gui.Controls.Constraints[app.IdExit];
		controlConstraints.AttachSizeToControl	= {app.IdExit, -1};
		::gpk::controlSetParent(gui, app.IdExit, -1);
	}

	{
		::gpk::SPNGData				pngDataCacheForFasterLoad;
		::gpk::view_const_string	pathPNGSuite		 {};
		{
			const ::gpk::SJSONReader	& jsonReader	= framework.JSONConfig.Reader;
			do { if_fail_fef(::gpk::jsonExpressionResolve(::gpk::vcst_t{"assets.pngsuite.path"}, jsonReader, 0, pathPNGSuite), "Failed to get path of PNG files! Last contents found: %s.", pathPNGSuite.begin()); } while(0);
			info_printf("Path to PNG test files: %.*s.", GPK_VIEW_ARGS(pathPNGSuite));
			::gpk::view_const_string	fileNamePNG							= {};
			const int32_t				indexJSONNodeArrayPNGFileNames		= ::gpk::jsonExpressionResolve(::gpk::vcst_t{"application.gpk_test_png.images"}, jsonReader, 0, fileNamePNG);
			const uint32_t				countFilesToLoad					= (uint32_t)::gpk::jsonArraySize(*jsonReader.Tree[indexJSONNodeArrayPNGFileNames]);
			do { if_fail_fef(app.PNGImages.resize(countFilesToLoad), "Failed to resize array for %u PNG files.", countFilesToLoad); } while(0);
			::gpk::apod<char>			expression							= {};
			::gpk::apod<char>			fullPathPNG							= {};
			char						subscriptExpression	[64]			= {};
			for(uint32_t iFile = 0; iFile < countFilesToLoad; ++iFile) {
				const uint32_t				lenExpression						= snprintf(subscriptExpression, 62, "['%u']", iFile);
				::gpk::jsonExpressionResolve({subscriptExpression, lenExpression}, jsonReader, indexJSONNodeArrayPNGFileNames, fileNamePNG);
				fullPathPNG.clear();
				::gpk::pathNameCompose(pathPNGSuite, fileNamePNG, fullPathPNG);
				if_true_ef(::gpk::failed(::gpk::pngLoadFile(pngDataCacheForFasterLoad, {fullPathPNG.begin(), fullPathPNG.size()}, app.PNGImages[iFile])), "Failed to load file: %s.", fullPathPNG.begin());
			}
		}
		{
			// ---- Test our recently developed RLE algorithm.
			::gpk::au2_t				sizesUncompressed;
			::gpk::au0_t					rleBuffer;
			::gpk::au2_t				sizesRLE;
			uint32_t					sizeTotalUncompressed				= 0;
			uint32_t					sizeTotalRLE						= 0;
			for(uint32_t iFile = 0, countFilesToLoad = app.PNGImages.size(); iFile < countFilesToLoad; ++iFile) {
				::gpk::view<::gpk::bgra>	viewToRLE{app.PNGImages[iFile].View.begin(), app.PNGImages[iFile].View.metrics().x * app.PNGImages[iFile].View.metrics().y};
				sizesUncompressed.push_back(viewToRLE.size());
				::gpk::rleEncode(viewToRLE, rleBuffer);
				sizesRLE.push_back(rleBuffer.size());
				const uint32_t				sizePNGInBytes					= viewToRLE.size() * sizeof(::gpk::bgra);
				//info_printf("--- RLE compression stats:"
				//	"\nsizePNGRLE          : %u"
				//	"\nsizePNGUncompressed : %u"
				//	"\nratio               : %f"
				//	, rleBuffer.size()
				//	, sizePNGInBytes
				//	, (float)rleBuffer.size() / sizePNGInBytes
				//	);
				sizeTotalRLE			+= rleBuffer.size();
				sizeTotalUncompressed	+= sizePNGInBytes;
				rleBuffer.clear();
			}
			//info_printf("--- RLE compression stats:"
			//	"\nsizeTotalRLE          : %u"
			//	"\nsizeTotalUncompressed : %u"
			//	"\nratio                 : %f"
			//	, sizeTotalRLE
			//	, sizeTotalUncompressed
			//	, (float)sizeTotalRLE / sizeTotalUncompressed
			//	);
		}
	}
	return 0;
}

::gpk::err_t			update		(::gme::SApplication & app, bool exitSignal)	{
	//::gpk::STimer															timer;
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, exitSignal);
	{
		::std::lock_guard														lock						(app.LockRender);
		app.Framework.RootWindow.BackBuffer									= app.Offscreen;
	}
	::gpk::SFramework				& framework					= app.Framework;
	if_true_vi(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework));

	::gpk::SGUI					& gui			= *framework.GUI;
	::gpk::acid					toProcess		= {};
	if(1 == ::gpk::guiProcessControls(gui, [&app](::gpk::cid_t iControl) { return iControl == app.IdExit ? 1 : 0; }))
		return 1;
	//timer.Frame();
	//warning_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}

::gpk::err_t			draw		(::gme::SApplication & app)							{
	//::gpk::STimer															timer;
	(void)app;
	::gpk::pobj<::gpk::rtbgra8d32>	target;
	target.create();
	target->resize(app.Framework.RootWindow.Size, {}, 0xFFFFFFFFU);
	for(uint32_t y = 0; y < target->Color.View.metrics().y; ++y)
	for(uint32_t x = 0; x < target->Color.View.metrics().x; ++x) {
		target->Color.View[y][x]											= rand();
		target->Color.View[y][x].a											= 255;
	}

	for(uint32_t iFile = 0; iFile < app.PNGImages.size(); ++iFile) {
		const uint32_t		offsetX			= (iFile * 64);
		::gpk::n2u2_t		position		= {offsetX % (target->Color.View.metrics().x - 64), offsetX / (target->Color.View.metrics().x - 64) * 64};
		::gpk::grid_copy_blend(target->Color.View, app.PNGImages[iFile].View, position);
		//::gpk::grid_scale_alpha(target->Color.View, app.PNGImages[iFile].View, position.Cast<int32_t>(), app.PNGImages[iFile].View.metrics().Cast<int32_t>() * (1 + (.01 * iFile)));
	}

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
		::std::lock_guard		lock					(app.LockGUI);

		::gpk::guiDraw(*app.Framework.GUI, target->Color.View);
	}
	{
		::std::lock_guard		lock					(app.LockRender);
		app.Offscreen		= target;
	}
	//timer.Frame();
	//warning_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	return 0;
}
