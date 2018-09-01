#include "application.h"
#include "gpk_bitmap_file.h"
#include "gpk_grid_copy.h"
#include "gpk_label.h"
#include "gpk_view_layered.h"
#include "gpk_png.h"

//#define GPK_AVOID_LOCAL_APPLICATION_MODULE_MODEL_EXECUTABLE_RUNTIME
#include "gpk_app_impl.h"

GPK_DEFINE_APPLICATION_ENTRY_POINT(::gme::SApplication, "Module Explorer");

			::gpk::error_t												optionListInitialize		(::gpk::SGUI& gui, ::gme::SOptionList& menu)													{
	gpk_necall(menu.IdControl = ::gpk::controlCreate(gui), "%s", "Failed to create menu control!");
	return 0;
}

			::gpk::error_t												gme::optionListPush			(::gpk::SGUI& gui, ::gme::SOptionList& menu, const ::gpk::view_const_string& text)				{
	if(menu.IdControl == -1) 
		gpk_necall(::optionListInitialize(gui, menu), "");
	const int32_t																idControl					= ::gpk::controlCreate(gui);
	gpk_necall(idControl, "%s", "Failed to create control! Out of memory?");
	::gpk::controlSetParent(gui, idControl, menu.IdControl);
	::gpk::SControl																& control					= gui.Controls.Controls		[idControl];
	::gpk::SControlText															& controlText				= gui.Controls.Text			[idControl];
	::gpk::SControlConstraints													& controlConstraints		= gui.Controls.Constraints	[idControl];
	control				.Margin												= {4, 4, 4, 4};
	controlText			.Text												= {text.begin(), text.size()};
	controlText			.Align												= ::gpk::ALIGN_CENTER_LEFT;
	controlConstraints	.AttachSizeToText									= {true, true};
	switch(menu.Orientation) {	
	case ::gme::MENU_ORIENTATION_HORIZONTAL	: controlConstraints.AttachSizeToControl.y = menu.IdControl; if(menu.IdControls.size()) controlConstraints.DockToControl.Right	 = menu.IdControls[menu.IdControls.size() - 1]; break;
	case ::gme::MENU_ORIENTATION_VERTICAL	: controlConstraints.AttachSizeToControl.x = menu.IdControl; if(menu.IdControls.size()) controlConstraints.DockToControl.Bottom	 = menu.IdControls[menu.IdControls.size() - 1]; break;
	}
	return menu.IdControls.push_back(idControl);			
}

	enum ANIMATION
		{ ANIMATION_GHOST		= 0
		, ANIMATION_EYES
		, ANIMATION_PLAYER
		, FRIGHTENED_FACE
		, TEXTURE_PAC_MAP
		, TEXTURE_PELLET
		, TEXTURE_ENERGYZER
		, TEXTURE_FRUIT_0
		, ANIMATION_COUNT
		};

			uint32_t													tileSize					= 8;

struct SSprite {
	uint8_t			Frames	;
	char			Name	[15];
};

			::gpk::error_t												cleanup						(::gme::SApplication & app)						{ return ::gpk::mainWindowDestroy(app.Framework.MainDisplay); }
			::gpk::error_t												setup						(::gme::SApplication & app)						{ 
	::gpk::SFramework															& framework					= app.Framework;
	::gpk::SDisplay																& mainWindow				= framework.MainDisplay;
	ree_if(0 == framework.Input.create(), "%s", "Out of memory?");
	error_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "%s", "Failed to create main window why?????!?!?!?!?");
	::gpk::SImage<ubyte_t>			tileMap;
	SSprite							spriteNames[ANIMATION_COUNT];
	{
		static constexpr	const char	filename[]	= "..\\gpk_data\\scripts\\pacman.txt";
		FILE							* fp		= 0;
		fopen_s(&fp, filename, "rb");
		error_if(0 == fp, "Failed to open file: %s.", filename)
		else {
			fseek(fp, 0, SEEK_END);
			int32_t							fileSize		= ftell(fp);
			fseek(fp, 0, SEEK_SET);
			::gpk::array_pod<ubyte_t>		mapFile;
			mapFile.resize(fileSize);
			fread(mapFile.begin(), 1, fileSize, fp);
			fclose(fp);
			tileMap.resize({AXIS_X, AXIS_Y}, ' ');
			for(uint32_t y = 0; y < tileMap.metrics().y; ++y) {
				memcpy(tileMap[y].begin(), &mapFile[y * (tileMap.metrics().x + 2)], tileMap.metrics().x);
			}
		}
	}
	{
		FILE							* fp		= 0;
		static constexpr	const char	filename[]	= "..\\gpk_data\\scripts\\pacman.bin";
		fopen_s(&fp, filename, "rb");
		error_if(0 == fp, "Failed to open '%s'.", filename)
		else {
			fseek(fp, 0, SEEK_END);
			int32_t							fileSize		= ftell(fp);
			fseek(fp, 0, SEEK_SET);
			::gpk::array_pod<ubyte_t>		mapFile;
			mapFile.resize(fileSize);
			fread(mapFile.begin(), 1, fileSize, fp);
			fclose(fp);
			for(uint32_t y = 0; y < 8; ++y) {
				memcpy(&spriteNames[y], &mapFile[y * sizeof(SSprite)], sizeof(SSprite));
			}
		}
	}

	::wak::setup(app.GameInstance, tileMap);
	app.GameInstance.Map.posToDraw.x = (app.Framework.MainDisplay.Size.x / 2) - (app.GameInstance.Map.Size.x / 2);
	app.GameInstance.Map.posToDraw.y = app.Framework.MainDisplay.Size.y / 4;

	app.CharacterAnimationImages.resize(ANIMATION_COUNT);
	app.CharacterAnimationLayers.resize(ANIMATION_COUNT);
	for(uint32_t iAnimation = 0; iAnimation < app.CharacterAnimationImages.size(); ++iAnimation) {
		char						fileName[1024];
		sprintf_s(fileName, "..\\gpk_data\\images\\%s", spriteNames[iAnimation].Name);
		error_if(errored(::gpk::pngFileLoad(fileName, app.CharacterAnimationImages[iAnimation])), "Failed to load file: %s", fileName);
	}

	for(uint32_t iAnimation = 0; iAnimation < app.CharacterAnimationImages.size(); ++iAnimation)
		// --- Build Ghost animation layered views
		app.CharacterAnimationLayers[iAnimation]								= 
			{ app.CharacterAnimationImages[iAnimation].Texels.begin()
			, app.CharacterAnimationImages[iAnimation].View.metrics().x
			, app.CharacterAnimationImages[iAnimation].View.metrics().y / spriteNames[iAnimation].Frames
			, spriteNames[iAnimation].Frames
			};

	return 0; 
}

::gpk::error_t												drawAnimated
				(::gme::SApplication					& app
				, const ::gpk::SCoord2<int32_t>			& posToDraw
				, const ::wak::SAnimatedObject			& animated
				, ::gpk::view_grid<::gpk::SColorBGRA>	& target
				, ::wak::MODE							mode
				, ::gpk::view_array<::gpk::view_layered<::gpk::SColorBGRA>>	animations
				, int32_t iGhost
			) 
{
	static constexpr const ::gpk::SColorBGRA									magenta				= { 0xFF, 0x00, 0xFF, 0xFF };
	const ::gpk::SColorBGRA														ghostColors	[4]		= 
		{ {0x00, 0x00, 0xFF, 0xFF}
		, {0xFF, 0x00, 0xFF, 0xFF}
		, {0xFF, 0xFF, 0x00, 0xFF}
		, ::gpk::ORANGE
		};

	if (mode == ::wak::MODE_ESCAPE) {
		const bool																swapColors			= ((app.GameInstance.CounterFrightened > 4) && (app.GameInstance.CounterAnimation / 2 % 2));
		::gpk::SColorBGRA														colorFrightened		= swapColors ? ::gpk::SColorBGRA{0xFF, 0xFF, 0xFF, 0xFF} : ::gpk::SColorBGRA{0xFF, 0, 0, 0xFF};
		::gpk::SColorBGRA														colorFrightenedFace	= swapColors ? ::gpk::SColorBGRA{0, 0, 0xFF, 0xFF} : ::gpk::SColorBGRA{0xFF, 0xFF, 0xFF, 0xFF};
		::gpk::grid_copy_alpha(target, animations[ANIMATION_GHOST][app.GameInstance.CounterAnimation % 2]	, posToDraw, magenta, colorFrightened);
		::gpk::grid_copy_alpha(target, animations[FRIGHTENED_FACE][0]										, posToDraw, magenta, colorFrightenedFace);
		
	}
	else if(app.GameInstance.Enemies[iGhost].Dead == true)
		::gpk::grid_copy_alpha(target, animations[ANIMATION_EYES][animated.CurrentDirection], posToDraw, magenta);
	else {
		::gpk::grid_copy_alpha(target, animations[ANIMATION_GHOST][app.GameInstance.CounterAnimation % 2], posToDraw, magenta, ghostColors[iGhost]);
		::gpk::grid_copy_alpha(target, animations[ANIMATION_EYES][animated.CurrentDirection], posToDraw, magenta);
	}
	return 0;
}

	::gpk::SCoord2<int32_t>										getPosToDraw				(const ::wak::SAnimatedObject & animated, const ::gpk::SCoord2<int32_t> & mapPosition) {
		return	{ (int32_t)(mapPosition.x + ((animated.Position.x + animated.PositionDeltas.x) * tileSize) - 8)
				, (int32_t)(mapPosition.y + ((animated.Position.y + animated.PositionDeltas.y) * tileSize) - 8)
		};
	}

				::gpk::error_t												draw						(::gme::SApplication & app)						{ 
	::gpk::STimer																timer;
	app;
	::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>			target;
	target.create();
	target->Color		.resize(app.Framework.MainDisplay.Size);
	target->DepthStencil.resize(target->Color.View.metrics());

	static constexpr const ::gpk::SColorBGRA									magenta							= { 0xFF, 0x00, 0xFF, 0xFF };

	for(uint32_t iAnim = 0; iAnim < TEXTURE_PAC_MAP; ++iAnim)
		::gpk::grid_copy_alpha										(target->Color.View, app.CharacterAnimationImages[iAnim].View, ::gpk::SCoord2<int32_t>{0, 128 + 128 * (int32_t)iAnim}, magenta);

	{
		::gme::mutex_guard															lock						(app.LockGame);
		::gpk::SCoord2<int32_t>														& mapPosition				= app.GameInstance.Map.posToDraw;
		::gpk::view_array<::wak::SEnemy>											& enemies					= app.GameInstance.Enemies;

		::gpk::SCoord2<int32_t>						playerPosToDraw				= { (int32_t)(mapPosition.x + ((app.GameInstance.Player.Position.x + app.GameInstance.Player.PositionDeltas.x) * tileSize - tileSize))	,
																					(int32_t)(mapPosition.y + ((app.GameInstance.Player.Position.y + app.GameInstance.Player.PositionDeltas.y) * tileSize - tileSize))	};

		::gpk::SCoord2<int32_t>														posToDraw[::wak::GHOST_COUNT] = {};

		for (uint32_t iGhost = 0; iGhost < ::wak::GHOST_COUNT; ++iGhost) {
			posToDraw[iGhost]							= getPosToDraw(enemies[iGhost], mapPosition);
		}
	
		//--- Dibujar sobrne el target
		for (int32_t y = 0; y < app.GameInstance.Map.Size.y; ++y) {
			for (int32_t x = 0; x < app.GameInstance.Map.Size.x; ++x) {
				::gpk::SCoord2<uint32_t>								pointToDraw = { mapPosition.x + (x * tileSize) , mapPosition.y + (y * tileSize) };
				::gpk::grid_copy_alpha(target->Color.View, app.CharacterAnimationLayers[TEXTURE_PAC_MAP][app.GameInstance.Map.TilesMap[y][x]], pointToDraw, magenta);
			}
		}
		if (app.GameInstance.Fruit) {
			::gpk::SCoord2<uint32_t>								pointToDraw = { mapPosition.x + (15 * tileSize - (tileSize)) , mapPosition.y + (20 * tileSize - (tileSize / 2)) };
			::gpk::grid_copy_alpha(target->Color.View, app.CharacterAnimationLayers[TEXTURE_FRUIT_0][0], pointToDraw, magenta);
		}

		if (app.GameInstance.Player.Position.x > 0 && app.GameInstance.Player.Position.x < 29) 
			::gpk::grid_copy_alpha(target->Color.View, app.CharacterAnimationLayers[ANIMATION_PLAYER][app.GameInstance.CounterAnimationPlayer % 4], playerPosToDraw, magenta, ::gpk::SColorBGRA{0x00, 0xFF, 0xFF, 0xFF});

		for (uint32_t iGhost = 0; iGhost < ::wak::GHOST_COUNT; ++iGhost)
			if (enemies[iGhost].Position.x > 0 && enemies[iGhost].Position.x < 29)
				drawAnimated(app, posToDraw[iGhost], enemies[iGhost], target->Color.View, enemies[iGhost].CurrentMode, app.CharacterAnimationLayers, iGhost);
	}

	{
		::gme::mutex_guard															lock						(app.LockGUI);
		::gpk::guiDraw(app.Framework.GUI, target->Color.View);
	}
	{
		::gme::mutex_guard															lock						(app.LockRender);
		app.Offscreen															= target;
	}
	//timer.Frame();
	//info_printf("Draw time: %f.", (float)timer.LastTimeSeconds);
	return 0; 
}

			::gpk::error_t												update						(::gme::SApplication & app, bool exitSignal)	{ 
	::gpk::STimer																timer;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, exitSignal, "%s", "Exit requested by runtime.");
	{
		::gme::mutex_guard															lock						(app.LockRender);
		app.Framework.MainDisplayOffscreen										= app.Offscreen;
	}
	::gpk::SFramework															& framework					= app.Framework;
	retval_info_if(::gpk::APPLICATION_STATE_EXIT, ::gpk::APPLICATION_STATE_EXIT == ::gpk::updateFramework(app.Framework), "%s", "Exit requested by framework update.");

	::gpk::SGUI																	& gui						= framework.GUI;
	{
		::gme::mutex_guard															lock						(app.LockGUI);
		::gpk::guiProcessInput(gui, *app.Framework.Input);
	}
	if(app.Framework.Input->MouseCurrent.Deltas.z) {
		gui.Zoom.ZoomLevel														+= app.Framework.Input->MouseCurrent.Deltas.z * (1.0 / (120 * 4));
		::gpk::guiUpdateMetrics(gui, framework.MainDisplay.Size, true);
	}
 
	for(uint32_t iControl = 0, countControls = gui.Controls.Controls.size(); iControl < countControls; ++iControl) {
		const ::gpk::SControlState													& controlState				= gui.Controls.States[iControl];
		if(controlState.Unused || controlState.Disabled)
			continue;
		if(controlState.Execute) {
			info_printf("Executed %u.", iControl);
			if(iControl == (uint32_t)app.IdExit)
				return ::gpk::APPLICATION_STATE_EXIT;
		}
	}

	app.GameInstance.NextDirection = (::wak::DIRECTION)-1;
		 if (GetAsyncKeyState('D')) app.GameInstance.NextDirection = ::wak::RIGHT	;
	else if (GetAsyncKeyState('A')) app.GameInstance.NextDirection = ::wak::LEFT	;
	else if (GetAsyncKeyState('W')) app.GameInstance.NextDirection = ::wak::UP		;
	else if (GetAsyncKeyState('S')) app.GameInstance.NextDirection = ::wak::DOWN	;

	{
		::gme::mutex_guard															lock						(app.LockGame);
		::wak::update(app.GameInstance, (float)framework.FrameInfo.Seconds.LastFrame);
	}
	//timer.Frame();
	//info_printf("Update time: %f.", (float)timer.LastTimeSeconds);
	return 0; 
}
