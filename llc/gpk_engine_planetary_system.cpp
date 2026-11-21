#include "gpk_engine_planetary_system.h"
#include "gpk_engine_orbit.h"

#include "gpk_stdstring.h"
#include "gpk_json_expression.h"
#include "gpk_engine_shader.h"
#include "gpk_gui_text.h"
#include "gpk_string.h"
#include "gpk_string_compose.h"

GPK_USING_TYPEINT();
GPK_USING_VIEW();
GPK_USING_APOD();
GPK_USING_ASTATIC();
GPK_USING_N2();
GPK_USING_N3();

stxp	f3_c		ASTRONOMICAL_UNIT_SCALE		= 1.0 / 149597870700;

gpk::err_t			gpk::planetarySystemSetup(SEnginePlanetarySystem & planetarySystem, SEngine & engine, cnst SJSONReader & jsonData)	{ 
	if_fail_fe(planetarySystemSetup(planetarySystem.PlanetarySystem, jsonData));	
	return planetarySystemSetup(planetarySystem.PlanetarySystem, planetarySystem.EntityMap, engine); 
}
gpk::err_t			gpk::planetarySystemSetup(SEnginePlanetarySystem & planetarySystem, SEngine & engine, vcst_t jsonFilePath)				{ 
	if_fail_fe(planetarySystemSetup(planetarySystem.PlanetarySystem, jsonFilePath));
	return planetarySystemSetup(planetarySystem.PlanetarySystem, planetarySystem.EntityMap, engine); 
}

gpk::err_t			gpk::planetarySystemSetup(cnst SPlanetarySystem & solarSystem, SPlanetarySystemEntityMap & entityMap, SEngine & engine) {
	stxp	u0_t				MAX_COLORS				= 16;
	stxp	astatic<bgra, MAX_COLORS>	PLANET_COLORS	=
		{ YELLOW
		, ORANGE
		, DARKRED * .5f
		, GREEN
		, WHITE
		, RED
		, PURPLE
		, BLUE
		, BLACK
		, ORANGE
		, BLUE
		, RED
		, PURPLE
		, DARKRED * .5f
		, GREEN
		, YELLOW
		};
	if_fail_fe(gpk::planetarySystemCreateEntities(solarSystem, entityMap, engine));
	if_fail_fe(gpk::planetarySystemReset(solarSystem, entityMap, engine, PLANET_COLORS));
	return 0;
}

sttc	gpk::err_t	createOrbiterOrbit		(gpk::SEngine & engine, vcst_t shaderName = "psOrbit") {	// Gravity cemters
	gpk::SParamsRingSide		params					= {{}, 32};
	s2_t						iEntity;
	if_fail_fe(iEntity = engine.CreateRingFlat(params));
	if_fail_fe(engine.SetColorDiffuse	(iEntity, {1.0f, 1.0f, 1.0f, .5}));
	if_fail_fe(engine.SetShader			(iEntity, gpk::psGridRuler, shaderName));
	return iEntity;
}

sttc	gpk::err_t	createGravityCenter		(gpk::SEngine & engine, vcst_t shaderName = "psGravityCenter") {	// Gravity cemters
	gpk::SParamsSphere		params					= {{}, {8, 8}};
	s2_t					iEntity;
	if_fail_fe(iEntity = engine.CreateSphere(params));
	if_fail_fe(engine.SetColorDiffuse	(iEntity, {.0f, 0.0f, 1.0f, .5f}));
	if_fail_fe(engine.SetShader		(iEntity, gpk::psSphereSolid, shaderName));
	return iEntity;
}

sttc	gpk::err_t	createOrbiterBody		(gpk::SEngine & engine, vcst_t shaderName = "psSun") {	// Gravity cemters
	gpk::SParamsSphere		params					= {{}, {32, 32}};
	s2_t					iEntity;
	if_fail_fe(iEntity = engine.CreateSphere(params));
	if_fail_fe(engine.SetColorDiffuse	(iEntity, {1.0f, 1.0f, 1.0f, 1.0f}));
	if_fail_fe(engine.SetShader			(iEntity, gpk::psSphereSolid, shaderName));
	return iEntity;
}

gpk::err_t			gpk::planetarySystemCreateEntities(cnst SPlanetarySystem & solarSystem, SPlanetarySystemEntityMap & entityMap, SEngine & engine) {
	SPNGData				pngCache				= {};
	stxp vcst_t				fileFolder				= "../gpk_data/images";
	string					fileName				= {};
	s2_t					iBackground				= 0;

	{	// Background sphere map
		SParamsSphere		params					= {{}, {32, 32}, true, 1, 2};
		if_fail_fe(iBackground = engine.CreateSphere(params));
		if_fail_fe(engine.SetColorDiffuse(iBackground, {1.0f, 1.0f, 1.0f, 1.0f}));
		if_fail_fe(engine.SetShader(iBackground, psSphereAxis, "psBackgroundMilkyWay"));
		append_strings(fileName, "black", ".png");
		s2_t						iImage;
		if_fail_fe(iImage = engine.CreateImageFromFile(fileFolder, fileName));
		SRenderNode			& renderNode			= engine.Scene->RenderNodes[engine.GetRenderNode(iBackground)];
		if_fail_fe(engine.Scene->Graphics->Skins[renderNode.Skin]->Textures.insert(0, iImage));
	}

	if_fail_fe(entityMap.GravityCenters.push_back(::createGravityCenter(engine)));
	for(u2_t iOrbiter = 1; iOrbiter < solarSystem.Body.size(); ++iOrbiter)
		if_fail_fe(entityMap.GravityCenters.push_back(engine.Clone(entityMap.GravityCenters[0], true, true, true)));

	if_fail_fe(engine.SetShader(entityMap.GravityCenters[0], psSphereAxis, "psSunfire"));
	
	if_fail_fe(entityMap.Bodies.push_back(::createOrbiterBody(engine)));
	for(u2_t iOrbiter = 1; iOrbiter < solarSystem.Body.size(); ++iOrbiter)
		if_fail_fe(entityMap.Bodies.push_back(engine.Clone(entityMap.Bodies[0], true, true, true)));

	for(u2_t iOrbiter = 0; iOrbiter < solarSystem.Body.size(); ++iOrbiter) {
		fileName.clear();
		if_fail_fe(append_strings(fileName, solarSystem.Body.Keys[iOrbiter], "_color.png"));

		s2_t						iImage;
		if_fail_fe(iImage = engine.CreateImageFromFile(fileFolder, fileName));
		
		SRenderNode			& renderNode			= engine.Scene->RenderNodes[engine.GetRenderNode(entityMap.Bodies[iOrbiter])];
		if_fail_fe(engine.Scene->Graphics->Skins[renderNode.Skin]->Textures.insert(0, iImage));
	}

	if_fail_fe(entityMap.Orbits.push_back(::createOrbiterOrbit(engine)));
	for(u2_t iOrbiter = 1; iOrbiter < solarSystem.Body.size(); ++iOrbiter)
		if_fail_fe(entityMap.Orbits.push_back(engine.Clone(entityMap.Orbits[0], true, false, false)));

	//{	// Rings circles
	//	SParamsRing			params					= {{}, 32};
	//	if_fail_fe(entityMap.Rings.push_back(engine.CreateRingSide(params)));
	//	if_fail_fe(engine.SetColorDiffuse(entityMap.Rings[0], {.0f, 1.0f, 0.0f, .05f}));
	//	if_fail_fe(engine.SetShader(entityMap.Rings[0], psGridRuler, "psOrbit"));
	//}
	//for(u2_t iOrbiter = 1; iOrbiter < solarSystem.Body.size(); ++iOrbiter)
	//	if_fail_fe(entityMap.Rings.push_back(engine.Clone(entityMap.Rings[0], true, false, false)));

	for(u2_t iOrbiter = 0; iOrbiter < solarSystem.Body.size(); ++iOrbiter) {
		if(-1 != solarSystem.Parent[iOrbiter])
			if_fail_fef(engine.Entities.SetParent(entityMap.Orbits[iOrbiter], entityMap.GravityCenters[solarSystem.Parent[iOrbiter]]), "iOrbiter:%" GPK_FMT_S2, iOrbiter);

		if_fail_fef(engine.Entities.SetParent(entityMap.GravityCenters[iOrbiter], entityMap.Orbits[iOrbiter]), "iOrbiter:%" GPK_FMT_S2, iOrbiter);
		if_fail_fef(engine.Entities.SetParent(entityMap.Bodies[iOrbiter], entityMap.GravityCenters[iOrbiter]), "iOrbiter:%" GPK_FMT_S2, iOrbiter);
		//engine.Entities.SetParent(entityMap.Rings[iOrbiter], entityMap.GravityCenters[iOrbiter]);
	}
	return iBackground;
}

sttc	gpk::err_t	initOrbit				(gpk::SEngine & engine, s2_t iOrbiter, s2_t iEntity, f3_t radius, f3_t distanceScale) {
	if_fail_fe(engine.SetPhysicsActive(iEntity, iOrbiter));
	if_fail_fe(engine.SetCollides	(iEntity, false));

	n3f3_t				scale					= {radius, radius, radius};
	if(iOrbiter)
		scale					*= distanceScale * 2;
	else
		scale					= {.01f, .01f, .01f};
	if_fail_fe(engine.SetMeshScale(iEntity, n3f3_t{scale.x * 1.001, scale.y, scale.z * 1.001}.f2(), true));
	if_fail_fe(engine.SetHidden	(iEntity, false));
	return 0;
}

stxp	n3f3_t	SUN_SCALE				= {.00625f, .00625f, .00625f};

sttc	gpk::err_t	initGravityCenter		(gpk::SEngine & engine, s2_t iEntity, b8_t isStar, f3_t diameter, f3_t distanceScale, b8_t ) {
	if_fail_fe(engine.SetPhysicsActive(iEntity, true));
	if_fail_fe(engine.SetCollides		(iEntity, false));

	n3f3_t				scale					= {diameter, diameter, diameter};
	scale					= isStar ? SUN_SCALE : scale * distanceScale;
	scale					*= 2;
	if_fail_fe(engine.SetMeshScale(iEntity, scale.f2(), true));
	if_fail_fe(engine.SetHidden	(iEntity, false));
	return 0;
}

sttc	gpk::err_t	initBody				(gpk::SEngine & engine, s2_t iEntity, b8_t isStar, f3_t diameter, f3_t distanceScale, b8_t ringSystem) {
	if_fail_fe(engine.SetPhysicsActive(iEntity, false == isStar));
	if_fail_fe(engine.SetCollides		(iEntity, false));

	n3f3_t					scale					= {diameter, diameter, diameter};
	scale				= isStar ? SUN_SCALE : scale * distanceScale;
	if_fail_fe(engine.SetMeshScale(iEntity, scale.f2(), true));
	gpk::TFuncPixelShader	& ps					= isStar ? gpk::psSphereAxis : ringSystem ? gpk::psSphereMeridian : gpk::psSphereSolid;
	vcst_t					psName					= isStar ? vcst_t{"psSun"}	: ringSystem ? vcst_t{"psGasGiant"}	: vcst_t{"psTerrestrial"};
	if_fail_fe(engine.SetShader	(iEntity, ps, psName));
	if_fail_fe(engine.SetHidden	(iEntity, false));
	return 0;
}

sttc	gpk::err_t	textureNumber			(gpk::g8bgra view, u2_t number, cnst gpk::SRasterFont & font) { 
	cnst auto			strNumberStorage		= ::gpk::str(number);
	gpk::vcst_c			strNumber				= strNumberStorage.Storage;
	gpk::rect2s1_c		targetRect				= 
		{ {s1_t(view.metrics().x / 2 - (font.CharSize.x * strNumber.size()) / 2), s1_t(view.metrics().y / 2 - font.CharSize.y / 2)}
		, font.CharSize.s1()
		};
	gpk::a2u1_t			coords;
	textLineRaster(view.metrics(), font.CharSize, targetRect, font.Texture, strNumber, coords);
	for(u2_t iCoord = 0; iCoord < coords.size(); ++iCoord) {
		n2u1_c			coord					= coords[iCoord];
		view[coord.y][coord.x]	= gpk::BLACK;
	}
	return 0; 
}

sttc	gpk::err_t	initSkin				(gpk::SEngine & engine, gpk::rgbaf color, s2_t iOrbiter, s2_t iEntity) {
	cnst gpk::SEntity		& entity				= engine.Entities[iEntity];
	cnst gpk::SRenderNode	& renderNode			= engine.Scene->RenderNodes[entity.RenderNode];
	gpk::SSkin				& skin					= *engine.Scene->Graphics->Skins[renderNode.Skin];
	gpk::SSurface			& surface				= *engine.Scene->Graphics->Surfaces[skin.Textures[0]];
	gpk::SRenderMaterial	& material				= skin.Material;
	material.Color.Specular	= gpk::GRAY;
	material.Color.Diffuse	= color;
  	material.Color.Ambient	= color * .1f;
	gpk::g8bgra				view					= {(gpk::bgra*)surface.Data.begin(), surface.Desc.Dimensions.u2()};
	cnst gpk::SRasterFont	& font					= *engine.Scene->Graphics->Fonts.Fonts[8];
	textureNumber(view, iOrbiter, font);
	return 0;
}

gpk::err_t			gpk::planetarySystemReset
	( cnst SPlanetarySystem			& solarSystem	
	, cnst SPlanetarySystemEntityMap	& entityMap
	, SEngine							& engine		
	, vc8bgra							colors
	) {
	f2_t						fFurthest				= 0;
	solarSystem.Body.Values.max<f2_t>(fFurthest, [](cnst SCelestialBody & body){ return body.Orbit.Radius; });
	cnst f3_t				distanceScale			= 1.0 / fFurthest * 10;
	cnst f3_t				rotationUnit			= 23.9;

	for(u2_t iOrbiter = 0; iOrbiter < solarSystem.Body.size(); ++iOrbiter) {
		cnst SCelestialBody	& body					= solarSystem.Body.Values[iOrbiter];
		b8_c					isStar					= solarSystem.Type[iOrbiter] == CELESTIAL_BODY_Star;
		b8_c					isMoon					= solarSystem.Type[iOrbiter] == CELESTIAL_BODY_Moon;
		stxp	f2_t				moonScale				= 25.f;
		{
			u2_c				iEntity					= entityMap.Orbits[iOrbiter];
			if_fail_fe(initOrbiterOrbit(body, engine.Integrator, engine.GetRigidBody(iEntity), 1));

			f3_t						orbitRadius				= body.Orbit.Radius;// + body.Diameter * .0005;
			if(isMoon)
				orbitRadius				*= moonScale;
			if_fail_fe(::initOrbit(engine, iOrbiter, iEntity, orbitRadius, distanceScale));
		}
		{
			u2_c				iEntity					= entityMap.GravityCenters[iOrbiter];
			if_fail_fe(initOrbiterCenter(body, engine.Integrator, engine.GetRigidBody(iEntity), distanceScale * (isMoon ? moonScale : 1)));
			if_fail_fe(::initGravityCenter(engine, iEntity, isStar, body.Diameter, distanceScale * .0001, body.Detail.Planet.RingSystem));
		}
		{
			u2_c				iEntity					= entityMap.Bodies[iOrbiter];
			if_fail_fe(initOrbiterBody(body, engine.Integrator, engine.GetRigidBody(iEntity), rotationUnit));
			if_fail_fe(::initBody(engine, iEntity, isStar, body.Diameter, distanceScale * .0001 * (isMoon ? moonScale * .05 : 1), body.Detail.Planet.RingSystem));
		}
		rgbaf				color					= colors[iOrbiter];
		cnst eid_t			entities[3]				= {entityMap.Orbits[iOrbiter], entityMap.GravityCenters[iOrbiter], entityMap.Bodies[iOrbiter]};
		for(u2_t i = 0; i < 3; ++i) {
			color.a					= i ? (i == 2) ? 1 : .25f : .125f;
			if_fail_fe(::initSkin(engine, color, iOrbiter, entities[i]));
		}
	}

	if_fail_fe(engine.Update(365));
	return 0;
}
