#include "gpk_engine_planetary_system.h"
#include "gpk_engine_orbit.h"

#include "gpk_stdstring.h"
#include "gpk_json_expression.h"
#include "gpk_engine_shader.h"
#include "gpk_gui_text.h"
#include "gpk_string.h"
#include "gpk_string_compose.h"

GPK_USING_TYPEINT();

stxp	double			ASTRONOMICAL_UNIT_SCALE		= 1.0 / 149597870700;

::gpk::err_t			gpk::planetarySystemSetup(::gpk::SEnginePlanetarySystem & planetarySystem, ::gpk::SEngine & engine, const ::gpk::SJSONReader & jsonData)	{ 
	if_fail_fef(gpk::planetarySystemSetup(planetarySystem.PlanetarySystem, jsonData), "%s", GPK_VIEW_ARGS(jsonData.View[0]).begin());	
	return ::gpk::planetarySystemSetup(planetarySystem.PlanetarySystem, planetarySystem.EntityMap, engine); 
}
::gpk::err_t			gpk::planetarySystemSetup(::gpk::SEnginePlanetarySystem & planetarySystem, ::gpk::SEngine & engine, ::gpk::view<sc_t> jsonFilePath)				{ 
	if_fail_fef(gpk::planetarySystemSetup(planetarySystem.PlanetarySystem, jsonFilePath), "%s", jsonFilePath.begin());						
	return ::gpk::planetarySystemSetup(planetarySystem.PlanetarySystem, planetarySystem.EntityMap, engine); 
}

::gpk::err_t			gpk::planetarySystemSetup(const ::gpk::SPlanetarySystem & solarSystem, ::gpk::SPlanetarySystemEntityMap & entityMap, ::gpk::SEngine & engine) {
	stxp	u0_t				MAX_COLORS				= 16;
	stxp	::gpk::astatic<::gpk::bgra, MAX_COLORS>	PLANET_COLORS	=
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
	if_fail_fe(gpk::planetarySystemCreateEntities(solarSystem, entityMap, engine));
	if_fail_fe(gpk::planetarySystemReset(solarSystem, entityMap, engine, PLANET_COLORS));
	return 0;
}

static	::gpk::err_t	createOrbiterOrbit	(::gpk::SEngine & engine, const ::gpk::vcst_t shaderName = ::gpk::view<sc_t>(5, "psOrbit")) {	// Gravity cemters
	::gpk::SParamsRingSide		params					= {{}, 32};
	s2_t						iEntity;
	if_fail_fe(iEntity = engine.CreateRingFlat(params));
	if_fail_fe(engine.SetColorDiffuse	(iEntity, {1.0f, 1.0f, 1.0f, .5}));
	if_fail_fe(engine.SetShader		(iEntity, ::gpk::psGridRuler, shaderName));
	return iEntity;
}

static	::gpk::err_t	createGravityCenter		(::gpk::SEngine & engine, const ::gpk::vcst_t shaderName = ::gpk::view<sc_t>(15, "psGravityCenter")) {	// Gravity cemters
	::gpk::SParamsSphere		params					= {{}, {8, 8}};
	s2_t						iEntity;
	if_fail_fe(iEntity = engine.CreateSphere(params));
	if_fail_fe(engine.SetColorDiffuse	(iEntity, {.0f, 0.0f, 1.0f, .5f}));
	if_fail_fe(engine.SetShader		(iEntity, ::gpk::psSphereSolid, shaderName));
	return iEntity;
}

static	::gpk::err_t	createOrbiterBody		(::gpk::SEngine & engine, const ::gpk::vcst_t shaderName = ::gpk::view<sc_t>(5, "psSun")) {	// Gravity cemters
	::gpk::SParamsSphere		params					= {{}, {32, 32}};
	s2_t						iEntity;
	if_fail_fe(iEntity = engine.CreateSphere(params));
	if_fail_fe(engine.SetColorDiffuse	(iEntity, {1.0f, 1.0f, 1.0f, 1.0f}));
	if_fail_fe(engine.SetShader		(iEntity, ::gpk::psSphereSolid, shaderName));
	return iEntity;
}

::gpk::err_t			gpk::planetarySystemCreateEntities(const ::gpk::SPlanetarySystem & solarSystem, ::gpk::SPlanetarySystemEntityMap & entityMap, ::gpk::SEngine & engine) {
	::gpk::SPNGData				pngCache				= {};
	stxp vcst_t					fileFolder				= GPK_CXS("../gpk_data/images");
	::gpk::string				fileName				= {};
	s2_t						iBackground				= 0;

	{	// Background sphere map
		::gpk::SParamsSphere		params					= {{}, {32, 32}, true, 1, 2};
		if_fail_fe(iBackground = engine.CreateSphere(params));
		if_fail_fe(engine.SetColorDiffuse(iBackground, {1.0f, 1.0f, 1.0f, 1.0f}));
		if_fail_fe(engine.SetShader(iBackground, ::gpk::psSphereAxis, "psBackgroundMilkyWay"));
		::gpk::append_strings(fileName, "black", ".png");
		s2_t						iImage;
		if_fail_fe(iImage = engine.CreateImageFromFile(fileFolder, fileName));
		::gpk::SRenderNode			& renderNode			= engine.Scene->RenderNodes[engine.GetRenderNode(iBackground)];
		if_fail_fe(engine.Scene->Graphics->Skins[renderNode.Skin]->Textures.insert(0, iImage));
	}

	if_fail_fe(entityMap.GravityCenters.push_back(::createGravityCenter(engine)));
	for(u2_t iOrbiter = 1; iOrbiter < solarSystem.Body.size(); ++iOrbiter)
		if_fail_fe(entityMap.GravityCenters.push_back(engine.Clone(entityMap.GravityCenters[0], true, true, true)));

	if_fail_fe(engine.SetShader(entityMap.GravityCenters[0], ::gpk::psSphereAxis, "psSunfire"));
	
	if_fail_fe(entityMap.Bodies.push_back(::createOrbiterBody(engine)));
	for(u2_t iOrbiter = 1; iOrbiter < solarSystem.Body.size(); ++iOrbiter)
		if_fail_fe(entityMap.Bodies.push_back(engine.Clone(entityMap.Bodies[0], true, true, true)));

	for(u2_t iOrbiter = 0; iOrbiter < solarSystem.Body.size(); ++iOrbiter) {
		fileName.clear();
		::gpk::append_strings(fileName, solarSystem.Body.Keys[iOrbiter], "_color.png");

		s2_t						iImage;
		if_fail_fe(iImage = engine.CreateImageFromFile(fileFolder, fileName));
		
		::gpk::SRenderNode			& renderNode			= engine.Scene->RenderNodes[engine.GetRenderNode(entityMap.Bodies[iOrbiter])];
		if_fail_fe(engine.Scene->Graphics->Skins[renderNode.Skin]->Textures.insert(0, iImage));
	}

	if_fail_fe(entityMap.Orbits.push_back(::createOrbiterOrbit(engine)));
	for(u2_t iOrbiter = 1; iOrbiter < solarSystem.Body.size(); ++iOrbiter)
		if_fail_fe(entityMap.Orbits.push_back(engine.Clone(entityMap.Orbits[0], true, false, false)));

	//{	// Rings circles
	//	::gpk::SParamsRing			params					= {{}, 32};
	//	if_fail_fe(entityMap.Rings.push_back(engine.CreateRingSide(params)));
	//	if_fail_fe(engine.SetColorDiffuse(entityMap.Rings[0], {.0f, 1.0f, 0.0f, .05f}));
	//	if_fail_fe(engine.SetShader(entityMap.Rings[0], ::gpk::psGridRuler, "psOrbit"));
	//}
	//for(u2_t iOrbiter = 1; iOrbiter < solarSystem.Body.size(); ++iOrbiter)
	//	if_fail_fe(entityMap.Rings.push_back(engine.Clone(entityMap.Rings[0], true, false, false)));

	for(u2_t iOrbiter = 0; iOrbiter < solarSystem.Body.size(); ++iOrbiter) {
		if(-1 != solarSystem.Parent[iOrbiter])
			engine.Entities.SetParent(entityMap.Orbits[iOrbiter], entityMap.GravityCenters[solarSystem.Parent[iOrbiter]]);

		engine.Entities.SetParent(entityMap.GravityCenters[iOrbiter], entityMap.Orbits[iOrbiter]);
		engine.Entities.SetParent(entityMap.Bodies[iOrbiter], entityMap.GravityCenters[iOrbiter]);
		//engine.Entities.SetParent(entityMap.Rings[iOrbiter], entityMap.GravityCenters[iOrbiter]);
	}
	return iBackground;
}

static	::gpk::err_t	initOrbit				(::gpk::SEngine & engine, s2_t iOrbiter, s2_t iEntity, double radius, double distanceScale) {
	if_fail_fe(engine.SetPhysicsActive(iEntity, iOrbiter));
	if_fail_fe(engine.SetCollides	(iEntity, false));

	::gpk::n3f3_t				scale					= {radius, radius, radius};
	if(iOrbiter)
		scale					*= distanceScale * 2;
	else
		scale					= {.01f, .01f, .01f};
	if_fail_fe(engine.SetMeshScale(iEntity, ::gpk::n3f3_t{scale.x * 1.001, scale.y, scale.z * 1.001}.f2(), true));
	if_fail_fe(engine.SetHidden	(iEntity, false));
	return 0;
}

stxp	::gpk::n3f3_t	SUN_SCALE				= {.00625f, .00625f, .00625f};

static	::gpk::err_t	initGravityCenter		(::gpk::SEngine & engine, s2_t iEntity, bool isStar, double diameter, double distanceScale, bool ) {
	if_fail_fe(engine.SetPhysicsActive(iEntity, true));
	if_fail_fe(engine.SetCollides		(iEntity, false));

	::gpk::n3f3_t				scale					= {diameter, diameter, diameter};
	scale					= isStar ? SUN_SCALE : scale * distanceScale;
	scale					*= 2;
	if_fail_fe(engine.SetMeshScale(iEntity, scale.f2(), true));
	if_fail_fe(engine.SetHidden	(iEntity, false));
	return 0;
}

static	::gpk::err_t	initBody				(::gpk::SEngine & engine, s2_t iEntity, bool isStar, double diameter, double distanceScale, bool ringSystem) {
	if_fail_fe(engine.SetPhysicsActive(iEntity, false == isStar));
	if_fail_fe(engine.SetCollides		(iEntity, false));

	::gpk::n3f3_t				scale					= {diameter, diameter, diameter};
	scale					= isStar ? SUN_SCALE : scale * distanceScale;
	if_fail_fe(engine.SetMeshScale(iEntity, scale.f2(), true));
	::gpk::TFuncPixelShader		& ps					= isStar ? ::gpk::psSphereAxis	: ringSystem ? ::gpk::psSphereMeridian	: ::gpk::psSphereSolid		;
	const ::gpk::view<sc_t>			psName					= isStar ? "psSun"	: ringSystem ? "psGasGiant"	: "psTerrestrial";
	if_fail_fe(engine.SetShader	(iEntity, ps, psName));
	if_fail_fe(engine.SetHidden	(iEntity, false));
	return 0;
}

static	::gpk::err_t	textureNumber			(::gpk::g8bgra view, u2_t number, const ::gpk::SRasterFont & font) { 
	sc_t						strNumber[4]			= {};
	sprintf_s(strNumber, "%i", number);
	const ::gpk::rect2s1_t		targetRect				= 
		{ {s1_t(view.metrics().x / 2 - (font.CharSize.x * strlen(strNumber)) / 2), s1_t(view.metrics().y / 2 - font.CharSize.y / 2)}
		, font.CharSize.s1()
		};
	::gpk::apod<::gpk::n2u1_t>	coords;
	::gpk::textLineRaster(view.metrics(), font.CharSize, targetRect, font.Texture, strNumber, coords);
	for(u2_t iCoord = 0; iCoord < coords.size(); ++iCoord) {
		const ::gpk::n2u1_t			coord						= coords[iCoord];
		view[coord.y][coord.x]	= ::gpk::BLACK;
	}
	return 0; 
}

static	::gpk::err_t	initSkin				(::gpk::SEngine & engine, ::gpk::rgbaf color, s2_t iOrbiter, s2_t iEntity) {
	const ::gpk::SEntity	& entity				= engine.Entities[iEntity];
	const ::gpk::SRenderNode	& renderNode			= engine.Scene->RenderNodes[entity.RenderNode];
	::gpk::SSkin				& skin					= *engine.Scene->Graphics->Skins[renderNode.Skin];
	::gpk::SSurface				& surface				= *engine.Scene->Graphics->Surfaces[skin.Textures[0]];
	::gpk::SRenderMaterial		& material				= skin.Material;
	material.Color.Specular	= ::gpk::GRAY;
	material.Color.Diffuse	= color;
  	material.Color.Ambient	= color * .1f;
	::gpk::g8bgra				view					= {(::gpk::bgra*)surface.Data.begin(), surface.Desc.Dimensions.u2()};
	const ::gpk::SRasterFont	& font					= *engine.Scene->Graphics->Fonts.Fonts[8];
	textureNumber(view, iOrbiter, font);
	return 0;
}

::gpk::err_t			gpk::planetarySystemReset
	( const ::gpk::SPlanetarySystem				& solarSystem	
	, const ::gpk::SPlanetarySystemEntityMap	& entityMap
	, ::gpk::SEngine							& engine		
	, const ::gpk::vc8bgra						& colors
	) {
	float						fFurthest				= 0;
	solarSystem.Body.Values.max<float>(fFurthest, [](const ::gpk::SCelestialBody & body){ return body.Orbit.Radius; });
	const double				distanceScale			= 1.0 / fFurthest * 10;
	const double				rotationUnit			= 23.9;

	for(u2_t iOrbiter = 0; iOrbiter < solarSystem.Body.size(); ++iOrbiter) {
		const ::gpk::SCelestialBody	& body					= solarSystem.Body.Values[iOrbiter];
		const bool					isStar					= solarSystem.Type[iOrbiter] == ::gpk::CELESTIAL_BODY_Star;
		const bool					isMoon					= solarSystem.Type[iOrbiter] == ::gpk::CELESTIAL_BODY_Moon;
		stxp	float				moonScale				= 25.f;
		{
			const u2_t				iEntity					= entityMap.Orbits[iOrbiter];
			if_fail_fe(gpk::initOrbiterOrbit(body, engine.Integrator, engine.GetRigidBody(iEntity), 1));

			double						orbitRadius				= body.Orbit.Radius;// + body.Diameter * .0005;
			if(isMoon)
				orbitRadius				*= moonScale;
			if_fail_fe(::initOrbit(engine, iOrbiter, iEntity, orbitRadius, distanceScale));
		}
		{
			const u2_t				iEntity					= entityMap.GravityCenters[iOrbiter];
			if_fail_fe(gpk::initOrbiterCenter(body, engine.Integrator, engine.GetRigidBody(iEntity), distanceScale * (isMoon ? moonScale : 1)));
			if_fail_fe(::initGravityCenter(engine, iEntity, isStar, body.Diameter, distanceScale * .0001, body.Detail.Planet.RingSystem));
		}
		{
			const u2_t				iEntity					= entityMap.Bodies[iOrbiter];
			if_fail_fe(gpk::initOrbiterBody(body, engine.Integrator, engine.GetRigidBody(iEntity), rotationUnit));
			if_fail_fe(::initBody(engine, iEntity, isStar, body.Diameter, distanceScale * .0001 * (isMoon ? moonScale * .05 : 1), body.Detail.Planet.RingSystem));
		}
		::gpk::rgbaf				color					= colors[iOrbiter];
		const ::gpk::eid_t			entities[3]				= {entityMap.Orbits[iOrbiter], entityMap.GravityCenters[iOrbiter], entityMap.Bodies[iOrbiter]};
		for(u2_t i = 0; i < 3; ++i) {
			color.a					= i ? (i == 2) ? 1 : .25f : .125f;
			if_fail_fe(::initSkin(engine, color, iOrbiter, entities[i]));
		}
	}

	engine.Update(365);
	return 0;
}
