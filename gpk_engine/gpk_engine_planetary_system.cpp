#include "gpk_engine_planetary_system.h"
#include "gpk_engine_orbit.h"

#include "gpk_stdstring.h"
#include "gpk_json_expression.h"
#include "gpk_engine_shader.h"
#include "gpk_gui_text.h"

GPK_USING_TYPEINT();

stacxpr	double			ASTRONOMICAL_UNIT_SCALE		= 1.0 / 149597870700;

::gpk::error_t			gpk::planetarySystemSetup(::gpk::SEnginePlanetarySystem & planetarySystem, ::gpk::SEngine & engine, const ::gpk::SJSONReader & jsonData)	{ 
	gpk_necall(gpk::planetarySystemSetup(planetarySystem.PlanetarySystem, jsonData), "%s", ::gpk::toString(jsonData.View[0]).begin());	
	return ::gpk::planetarySystemSetup(planetarySystem.PlanetarySystem, planetarySystem.EntityMap, engine); 
}
::gpk::error_t			gpk::planetarySystemSetup(::gpk::SEnginePlanetarySystem & planetarySystem, ::gpk::SEngine & engine, ::gpk::vcc jsonFilePath)				{ 
	gpk_necall(gpk::planetarySystemSetup(planetarySystem.PlanetarySystem, jsonFilePath), "%s", jsonFilePath.begin());						
	return ::gpk::planetarySystemSetup(planetarySystem.PlanetarySystem, planetarySystem.EntityMap, engine); 
}

::gpk::error_t			gpk::planetarySystemSetup(const ::gpk::SPlanetarySystem & solarSystem, ::gpk::SPlanetarySystemEntityMap & entityMap, ::gpk::SEngine & engine) {
	stacxpr	uint8_t				MAX_COLORS				= 16;
	stacxpr	::gpk::astatic<::gpk::bgra, MAX_COLORS>	PLANET_COLORS	=
		{ ::gpk::YELLOW
		, ::gpk::ORANGE
		, ::gpk::DARKRED * .5f
		, ::gpk::GREEN
		, ::gpk::WHITE
		, ::gpk::RED
		, ::gpk::PURPLE
		, ::gpk::BLUE
		, ::gpk::BLACK
		, ::gpk::ORANGE
		, ::gpk::BLUE
		, ::gpk::RED
		, ::gpk::PURPLE
		, ::gpk::DARKRED * .5f
		, ::gpk::GREEN
		, ::gpk::YELLOW
		};
	gpk_necs(gpk::planetarySystemCreateEntities(solarSystem, entityMap, engine));
	gpk_necs(gpk::planetarySystemReset(solarSystem, entityMap, engine, PLANET_COLORS));
	return 0;
}

static	::gpk::error_t	createOrbiterOrbit	(::gpk::SEngine & engine, const ::gpk::vcs shaderName = ::gpk::vcc(5, "psOrbit")) {	// Gravity cemters
	::gpk::SParamsRingSide		params					= {{}, 32};
	int32_t						iEntity;
	gpk_necs(iEntity = engine.CreateRingFlat(params));
	gpk_necs(engine.SetColorDiffuse	(iEntity, {1.0f, 1.0f, 1.0f, .5}));
	gpk_necs(engine.SetShader		(iEntity, ::gpk::psGridRuler, shaderName));
	return iEntity;
}

static	::gpk::error_t	createGravityCenter		(::gpk::SEngine & engine, const ::gpk::vcs shaderName = ::gpk::vcc(15, "psGravityCenter")) {	// Gravity cemters
	::gpk::SParamsSphere		params					= {{}, {8, 8}};
	int32_t						iEntity;
	gpk_necs(iEntity = engine.CreateSphere(params));
	gpk_necs(engine.SetColorDiffuse	(iEntity, {.0f, 0.0f, 1.0f, .5f}));
	gpk_necs(engine.SetShader		(iEntity, ::gpk::psSphereSolid, shaderName));
	return iEntity;
}

static	::gpk::error_t	createOrbiterBody		(::gpk::SEngine & engine, const ::gpk::vcs shaderName = ::gpk::vcc(5, "psSun")) {	// Gravity cemters
	::gpk::SParamsSphere		params					= {{}, {32, 32}};
	int32_t						iEntity;
	gpk_necs(iEntity = engine.CreateSphere(params));
	gpk_necs(engine.SetColorDiffuse	(iEntity, {1.0f, 1.0f, 1.0f, 1.0f}));
	gpk_necs(engine.SetShader		(iEntity, ::gpk::psSphereSolid, shaderName));
	return iEntity;
}

::gpk::error_t			gpk::planetarySystemCreateEntities(const ::gpk::SPlanetarySystem & solarSystem, ::gpk::SPlanetarySystemEntityMap & entityMap, ::gpk::SEngine & engine) {
	::gpk::SPNGData				pngCache				= {};
	stacxpr sc_t				fileFolder	[]			= "../gpk_data/images";
	sc_t						fileName	[1024]		= {};
	int32_t						iBackground				= 0;

	{	// Background sphere map
		::gpk::SParamsSphere		params					= {{}, {32, 32}, true, 1, 2};
		gpk_necs(iBackground = engine.CreateSphere(params));
		gpk_necs(engine.SetColorDiffuse(iBackground, {1.0f, 1.0f, 1.0f, 1.0f}));
		gpk_necs(engine.SetShader(iBackground, ::gpk::psSphereAxis, "psBackgroundMilkyWay"));
		sprintf_s(fileName, "%s.png", "STScI-H-Whale_galaxy-NASA.Hubble_Space_Telescope.ESA-h-4467x1217");
		int32_t						iImage;
		gpk_necs(iImage = engine.CreateImageFromFile(fileFolder, fileName));
		::gpk::SRenderNode			& renderNode			= engine.Scene->RenderNodes[engine.GetRenderNode(iBackground)];
		gpk_necs(engine.Scene->Graphics->Skins[renderNode.Skin]->Textures.insert(0, iImage));
	}

	gpk_necs(entityMap.GravityCenters.push_back(::createGravityCenter(engine)));
	for(uint32_t iOrbiter = 1; iOrbiter < solarSystem.Body.size(); ++iOrbiter)
		gpk_necs(entityMap.GravityCenters.push_back(engine.Clone(entityMap.GravityCenters[0], true, true, true)));

	gpk_necs(engine.SetShader(entityMap.GravityCenters[0], ::gpk::psSphereAxis, "psSunfire"));
	
	gpk_necs(entityMap.Bodies.push_back(::createOrbiterBody(engine)));
	for(uint32_t iOrbiter = 1; iOrbiter < solarSystem.Body.size(); ++iOrbiter)
		gpk_necs(entityMap.Bodies.push_back(engine.Clone(entityMap.Bodies[0], true, true, true)));

	for(uint32_t iOrbiter = 0; iOrbiter < solarSystem.Body.size(); ++iOrbiter) {
		sprintf_s(fileName, "%s_color.png", ::gpk::toString(solarSystem.Body.Keys[iOrbiter]).begin());
		int32_t						iImage;
		gpk_necs(iImage = engine.CreateImageFromFile(fileFolder, fileName));
		::gpk::SRenderNode			& renderNode			= engine.Scene->RenderNodes[engine.GetRenderNode(entityMap.Bodies[iOrbiter])];
		gpk_necs(engine.Scene->Graphics->Skins[renderNode.Skin]->Textures.insert(0, iImage));
	}

	gpk_necs(entityMap.Orbits.push_back(::createOrbiterOrbit(engine)));
	for(uint32_t iOrbiter = 1; iOrbiter < solarSystem.Body.size(); ++iOrbiter)
		gpk_necs(entityMap.Orbits.push_back(engine.Clone(entityMap.Orbits[0], true, false, false)));

	//{	// Rings circles
	//	::gpk::SParamsRing			params					= {{}, 32};
	//	gpk_necs(entityMap.Rings.push_back(engine.CreateRingSide(params)));
	//	gpk_necs(engine.SetColorDiffuse(entityMap.Rings[0], {.0f, 1.0f, 0.0f, .05f}));
	//	gpk_necs(engine.SetShader(entityMap.Rings[0], ::gpk::psGridRuler, "psOrbit"));
	//}
	//for(uint32_t iOrbiter = 1; iOrbiter < solarSystem.Body.size(); ++iOrbiter)
	//	gpk_necs(entityMap.Rings.push_back(engine.Clone(entityMap.Rings[0], true, false, false)));

	for(uint32_t iOrbiter = 0; iOrbiter < solarSystem.Body.size(); ++iOrbiter) {
		if(-1 != solarSystem.Parent[iOrbiter])
			engine.Entities.SetParent(entityMap.Orbits[iOrbiter], entityMap.GravityCenters[solarSystem.Parent[iOrbiter]]);

		engine.Entities.SetParent(entityMap.GravityCenters[iOrbiter], entityMap.Orbits[iOrbiter]);
		engine.Entities.SetParent(entityMap.Bodies[iOrbiter], entityMap.GravityCenters[iOrbiter]);
		//engine.Entities.SetParent(entityMap.Rings[iOrbiter], entityMap.GravityCenters[iOrbiter]);
	}
	return iBackground;
}

static	::gpk::error_t	initOrbit				(::gpk::SEngine & engine, int32_t iOrbiter, int32_t iEntity, double radius, double distanceScale) {
	gpk_necs(engine.SetPhysicsActive(iEntity, iOrbiter));
	gpk_necs(engine.SetCollides	(iEntity, false));

	::gpk::n3f64				scale					= {radius, radius, radius};
	if(iOrbiter)
		scale					*= distanceScale * 2;
	else
		scale					= {.01f, .01f, .01f};
	gpk_necs(engine.SetMeshScale(iEntity, ::gpk::n3f64{scale.x * 1.001, scale.y, scale.z * 1.001}.f2_t(), true));
	gpk_necs(engine.SetHidden	(iEntity, false));
	return 0;
}

stacxpr	::gpk::n3f64	SUN_SCALE				= {.00625f, .00625f, .00625f};

static	::gpk::error_t	initGravityCenter		(::gpk::SEngine & engine, int32_t iEntity, bool isStar, double diameter, double distanceScale, bool ) {
	gpk_necs(engine.SetPhysicsActive(iEntity, true));
	gpk_necs(engine.SetCollides		(iEntity, false));

	::gpk::n3f64				scale					= {diameter, diameter, diameter};
	scale					= isStar ? SUN_SCALE : scale * distanceScale;
	scale					*= 2;
	gpk_necs(engine.SetMeshScale(iEntity, scale.f2_t(), true));
	gpk_necs(engine.SetHidden	(iEntity, false));
	return 0;
}

static	::gpk::error_t	initBody				(::gpk::SEngine & engine, int32_t iEntity, bool isStar, double diameter, double distanceScale, bool ringSystem) {
	gpk_necs(engine.SetPhysicsActive(iEntity, false == isStar));
	gpk_necs(engine.SetCollides		(iEntity, false));

	::gpk::n3f64				scale					= {diameter, diameter, diameter};
	scale					= isStar ? SUN_SCALE : scale * distanceScale;
	gpk_necs(engine.SetMeshScale(iEntity, scale.f2_t(), true));
	::gpk::TFuncPixelShader		& ps					= isStar ? ::gpk::psSphereAxis	: ringSystem ? ::gpk::psSphereMeridian	: ::gpk::psSphereSolid		;
	const ::gpk::vcc			psName					= isStar ? ::gpk::vcs{"psSun"}	: ringSystem ? ::gpk::vcs{"psGasGiant"}	: ::gpk::vcs{"psTerrestrial"};
	gpk_necs(engine.SetShader	(iEntity, ps, psName));
	gpk_necs(engine.SetHidden	(iEntity, false));
	return 0;
}

static	::gpk::error_t	textureNumber			(::gpk::g8bgra view, uint32_t number, const ::gpk::SRasterFont & font) { 
	sc_t						strNumber[4]			= {};
	sprintf_s(strNumber, "%i", number);
	const ::gpk::rect2i16		targetRect				= 
		{ {int16_t(view.metrics().x / 2 - (font.CharSize.x * strlen(strNumber)) / 2), int16_t(view.metrics().y / 2 - font.CharSize.y / 2)}
		, font.CharSize.s1_t()
		};
	::gpk::apod<::gpk::n2u16>	coords;
	::gpk::textLineRaster(view.metrics(), font.CharSize, targetRect, font.Texture, strNumber, coords);
	for(uint32_t iCoord = 0; iCoord < coords.size(); ++iCoord) {
		const ::gpk::n2u16			coord						= coords[iCoord];
		view[coord.y][coord.x]	= ::gpk::BLACK;
	}
	return 0; 
}

static	::gpk::error_t	initSkin				(::gpk::SEngine & engine, ::gpk::rgbaf color, int32_t iOrbiter, int32_t iEntity) {
	const ::gpk::SEntity	& entity				= engine.Entities[iEntity];
	const ::gpk::SRenderNode	& renderNode			= engine.Scene->RenderNodes[entity.RenderNode];
	::gpk::SSkin				& skin					= *engine.Scene->Graphics->Skins[renderNode.Skin];
	::gpk::SSurface				& surface				= *engine.Scene->Graphics->Surfaces[skin.Textures[0]];
	::gpk::SRenderMaterial		& material				= skin.Material;
	material.Color.Specular	= ::gpk::GRAY;
	material.Color.Diffuse	= color;
  	material.Color.Ambient	= color * .1f;
	::gpk::g8bgra				view					= {(::gpk::bgra*)surface.Data.begin(), surface.Desc.Dimensions.u2_t()};
	const ::gpk::SRasterFont	& font					= *engine.Scene->Graphics->Fonts.Fonts[8];
	textureNumber(view, iOrbiter, font);
	return 0;
}

::gpk::error_t			gpk::planetarySystemReset
	( const ::gpk::SPlanetarySystem				& solarSystem	
	, const ::gpk::SPlanetarySystemEntityMap	& entityMap
	, ::gpk::SEngine							& engine		
	, const ::gpk::vc8bgra						& colors
	) {
	float						fFurthest				= 0;
	solarSystem.Body.Values.max<float>(fFurthest, [](const ::gpk::SCelestialBody & body){ return body.Orbit.Radius; });
	const double				distanceScale			= 1.0 / fFurthest * 10;
	const double				rotationUnit			= 23.9;

	for(uint32_t iOrbiter = 0; iOrbiter < solarSystem.Body.size(); ++iOrbiter) {
		const ::gpk::SCelestialBody	& body					= solarSystem.Body.Values[iOrbiter];
		const bool					isStar					= solarSystem.Type[iOrbiter] == ::gpk::CELESTIAL_BODY_Star;
		const bool					isMoon					= solarSystem.Type[iOrbiter] == ::gpk::CELESTIAL_BODY_Moon;
		stacxpr	float				moonScale				= 25.f;
		{
			const uint32_t				iEntity					= entityMap.Orbits[iOrbiter];
			gpk_necs(gpk::initOrbiterOrbit(body, engine.Integrator, engine.GetRigidBody(iEntity), 1));

			double						orbitRadius				= body.Orbit.Radius;// + body.Diameter * .0005;
			if(isMoon)
				orbitRadius				*= moonScale;
			gpk_necs(::initOrbit(engine, iOrbiter, iEntity, orbitRadius, distanceScale));
		}
		{
			const uint32_t				iEntity					= entityMap.GravityCenters[iOrbiter];
			gpk_necs(gpk::initOrbiterCenter(body, engine.Integrator, engine.GetRigidBody(iEntity), distanceScale * (isMoon ? moonScale : 1)));
			gpk_necs(::initGravityCenter(engine, iEntity, isStar, body.Diameter, distanceScale * .0001, body.Detail.Planet.RingSystem));
		}
		{
			const uint32_t				iEntity					= entityMap.Bodies[iOrbiter];
			gpk_necs(gpk::initOrbiterBody(body, engine.Integrator, engine.GetRigidBody(iEntity), rotationUnit));
			gpk_necs(::initBody(engine, iEntity, isStar, body.Diameter, distanceScale * .0001 * (isMoon ? moonScale * .05 : 1), body.Detail.Planet.RingSystem));
		}
		::gpk::rgbaf				color					= colors[iOrbiter];
		const ::gpk::eid_t			entities[3]				= {entityMap.Orbits[iOrbiter], entityMap.GravityCenters[iOrbiter], entityMap.Bodies[iOrbiter]};
		for(uint32_t i = 0; i < 3; ++i) {
			color.a					= i ? (i == 2) ? 1 : .25f : .125f;
			gpk_necs(::initSkin(engine, color, iOrbiter, entities[i]));
		}
	}

	engine.Update(365);
	return 0;
}
