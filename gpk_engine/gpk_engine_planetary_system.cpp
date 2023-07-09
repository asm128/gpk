#include "gpk_engine_planetary_system.h"
#include "gpk_stdstring.h"
#include "gpk_json_expression.h"
#include "gpk_engine_shader.h"
#include "gpk_gui_text.h"

//static	::gpk::error_t	setupPlanet		(::gpk::SSolarSystem & /*solarSystem*/, ::gpk::SEngine & /*engine*/, const ::gpk::SJSONReader & /*jsonData*/) { return 0; }
//static	::gpk::error_t	setupStar		(::gpk::SSolarSystem & /*solarSystem*/, ::gpk::SEngine & /*engine*/, const ::gpk::SJSONReader & /*jsonData*/) { return 0; }
::gpk::error_t			gpk::planetarySystemSetup	(const ::gpk::SPlanetarySystem & solarSystem, ::gpk::SPlanetarySystemEntityMap & entityMap, ::gpk::SEngine & engine) {
	stacxpr	uint8_t				MAX_COLORS	= 16;
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
	gpk_necs(::gpk::planetarySystemCreateEntities(solarSystem, entityMap, engine));
	gpk_necs(::gpk::planetarySystemReset(solarSystem, entityMap, engine, PLANET_COLORS));
	return 0;
}

::gpk::error_t			gpk::planetarySystemSetup	(::gpk::SPlanetarySystem & planetarySystem, ::gpk::vcc jsonFilePath) { 
	::gpk::SJSONFile			jsonFile					= {};
	gpk_necs(::gpk::jsonFileRead(jsonFile, jsonFilePath), "%s", ::gpk::toString(jsonFilePath).begin());
	return ::gpk::planetarySystemSetup(planetarySystem, jsonFile.Reader);
}

::gpk::error_t			gpk::planetarySystemSetup	(::gpk::SPlanetarySystem & solarSystem, const ::gpk::SJSONReader & jsonData) {
	::gpk::avcc					stellarBodyNames			= {};
	::gpk::ai32					stellarBodyIndices;
	gpk_necs(::gpk::jsonObjectKeyList(jsonData, 0, stellarBodyIndices, stellarBodyNames));
	::gpk::vcvcc				jsonView					= jsonData.View;
	::gpk::avcc					bodyParentNames;

	int32_t						iFurthest					= 0;
	float						fFurthest					= 0;

	for(uint32_t iPlanet = 0; iPlanet < stellarBodyIndices.size(); ++iPlanet) {
		const int32_t				jsonIndexStellarBody		= stellarBodyIndices[iPlanet];
		int32_t						indexProperty;

		if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody + 2, "parent")) 
			gpk_necs(bodyParentNames.push_back({}));
		else
			gpk_necs(bodyParentNames.push_back(jsonView[indexProperty]));
		
		{ // Load planet properties.
			::gpk::CELESTIAL_BODY		bodyType					= ::gpk::CELESTIAL_BODY_Planet;
			ws_if_failed(indexProperty = ::gpk::jsonObjectValueGet(jsonData, jsonIndexStellarBody + 2, "type")) // ?? no type? shouldn't happen
			else {
				::gpk::vcc					strType						= jsonData.View[indexProperty]; 
				bodyType				= (::gpk::vcs("star") == strType) ? ::gpk::CELESTIAL_BODY_Star : ::gpk::CELESTIAL_BODY_Planet;
			}

#define CASE_LOAD(Token)					\
	case ::gpk::CELESTIAL_BODY_##Token:		\
		ws_if_failed(load##Token(bodyProperties.Detail.Token, jsonData, jsonIndexStellarBody + 2)); print##Token(bodyProperties.Detail.Token);	\
		break;

			::gpk::SCelestialBody		bodyProperties				= {};
			switch(bodyType) {
			CASE_LOAD(Star  ); 
			CASE_LOAD(Planet); 
			}
			if(fFurthest < bodyProperties.Detail.Planet.DistanceFromSun) {
				fFurthest				= bodyProperties.Detail.Planet.DistanceFromSun;
				iFurthest				= iPlanet;
			}
			gpk_necs(solarSystem.Type.push_back(bodyType));
			gpk_necs(solarSystem.Body.push_back(stellarBodyNames[iPlanet], bodyProperties));
		}
	}

	for(uint32_t iPlanet = 0; iPlanet < solarSystem.Body.Keys.size(); ++iPlanet)
		gpk_necs(solarSystem.Parent.push_back(solarSystem.Body.Keys.find(bodyParentNames[iPlanet])));

	return iFurthest;
}

static	::gpk::error_t	textureNumber				(::gpk::g8bgra view, uint32_t number, const ::gpk::SRasterFont & font) { 
	char						strNumber[4]				= {};
	sprintf_s(strNumber, "%i", number);
	const ::gpk::rect2i16		targetRect					= 
		{ {int16_t(view.metrics().x / 2 - (font.CharSize.x * strlen(strNumber)) / 2), int16_t(view.metrics().y / 2 - font.CharSize.y / 2)}
		, font.CharSize.i16()
		};
	::gpk::apod<::gpk::n2u16>	coords;
	::gpk::textLineRaster(view.metrics(), font.CharSize, targetRect, font.Texture, strNumber, coords);
	for(uint32_t iCoord = 0; iCoord < coords.size(); ++iCoord) {
		const ::gpk::n2u16			coord						= coords[iCoord];
		view[coord.y][coord.x]	= ::gpk::BLACK;
	}
	return 0; 
}

::gpk::error_t			gpk::planetarySystemCreateEntities	(const ::gpk::SPlanetarySystem & solarSystem, ::gpk::SPlanetarySystemEntityMap & entityMap, ::gpk::SEngine & engine) {
	{	// planets
		::gpk::SParamsSphere		params			= {{}, {24, 24}};
		gpk_necs(entityMap.Bodies.push_back(engine.CreateSphere(params)));
		gpk_necs(engine.SetColorDiffuse(entityMap.Bodies[0], {.0f, 0.0f, 1.0f, 1}));
	}

	gpk_necs(engine.SetShader(entityMap.Bodies[0], ::gpk::psSphereAxis, "psSun"));
	for(uint32_t iOrbiter = 1; iOrbiter < solarSystem.Body.size(); ++iOrbiter)
		gpk_necs(entityMap.Bodies.push_back(engine.Clone(entityMap.Bodies[0], true, true, true)));

	{	// orbits circles
		::gpk::SParamsCircle		params	= {{}, 24};
		gpk_necs(entityMap.Orbits.push_back(engine.CreateCircle(params)));
		gpk_necs(engine.SetColorDiffuse(entityMap.Orbits[0], {.0f, 1.0f, 0.0f, .05f}));
	}

	gpk_necs(engine.SetShader(entityMap.Orbits[0], ::gpk::psGridRuler, "psOrbit"));
	for(uint32_t iOrbiter = 1; iOrbiter < solarSystem.Body.size(); ++iOrbiter) {
		gpk_necs(entityMap.Orbits.push_back(engine.Clone(entityMap.Orbits[0], true, true, true)));
		engine.Entities.SetParent(entityMap.Bodies[iOrbiter], entityMap.Orbits[iOrbiter]);
		if(iOrbiter == 4)
			engine.Entities.SetParent(entityMap.Orbits[iOrbiter], entityMap.Bodies[3]);

	}

	return 0;
}

static	::gpk::error_t	initOrbit(::gpk::SEngine & engine, int32_t iOrbiter, int32_t iEntity, float distanceFromCenter, float distanceScale) {
	gpk_necs(engine.SetPhysicsActive(iEntity, iOrbiter));
	gpk_necs(engine.SetCollides	(iEntity, false));

	::gpk::n3f32				scale					= {distanceFromCenter, distanceFromCenter, distanceFromCenter};
	if(iOrbiter)
		scale					*= distanceScale * 2;
	else
		scale					= {.1f, .1f, .1f};
	gpk_necs(engine.SetMeshScale(iEntity, scale, true));
	//gpk_necs(engine.SetShader	(iEntity, ::gpk::psGridRuler, "psOrbit"));
	gpk_necs(engine.SetHidden	(iEntity, false));
	return 0;
}

stacxpr	::gpk::n3f32	SUN_SCALE			= {.00625f, .00625f, .00625f};


static	::gpk::error_t	initBody				(::gpk::SEngine & engine, int32_t iOrbiter, int32_t iEntity, float diameter, float distanceScale, bool ringSystem) {
	gpk_necs(engine.SetPhysicsActive(iEntity, iOrbiter));
	gpk_necs(engine.SetCollides	(iEntity, false));

	::gpk::n3f32				scale					= {diameter, diameter, diameter};
	//if(4 == iOrbiter) {}
	//	//scale					*= distanceScale * .000125f;
	//else 
	if(iOrbiter)
		scale					*= distanceScale * .000125f;
	else
		scale					= SUN_SCALE;
	gpk_necs(engine.SetMeshScale(iEntity, scale, true));
	::gpk::TFuncPixelShader		& ps					= (0 == iOrbiter) ? ::gpk::psSphereAxis	: ringSystem ? ::gpk::psSphereMeridian			: ::gpk::psSphereSolid		;
	const ::gpk::vcc			psName					= (0 == iOrbiter) ? ::gpk::vcs{"psSun"}	: ringSystem ? ::gpk::vcs{"psGasGiant"}	: ::gpk::vcs{"psSphereSolid"};
	gpk_necs(engine.SetShader	(iEntity, ps, psName));
	gpk_necs(engine.SetHidden	(iEntity, false));
	return 0;
}

static	::gpk::error_t	initSkin				(::gpk::SEngine & engine, ::gpk::rgbaf color, int32_t iOrbiter, int32_t iEntity) {
	const ::gpk::SVirtualEntity	& entity				= engine.Entities[iEntity];
	const ::gpk::SRenderNode	& renderNode			= engine.Scene->RenderNodes[entity.RenderNode];
	::gpk::SSkin				& skin					= *engine.Scene->Graphics->Skins[renderNode.Skin];
	::gpk::SSurface				& surface				= *engine.Scene->Graphics->Surfaces[skin.Textures[0]];
	::gpk::SRenderMaterial		& material				= skin.Material;
	material.Color.Specular	= ::gpk::GRAY;
	material.Color.Diffuse	= color;
  	material.Color.Ambient	= color * .1f;
	::gpk::g8bgra				view					= {(::gpk::bgra*)surface.Data.begin(), surface.Desc.Dimensions.u32()};
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
	solarSystem.Body.Values.max<float>(fFurthest, [](const ::gpk::SCelestialBody & body){ return body.Detail.Planet.DistanceFromSun; });
	const float					distanceScale			= 1.0f / fFurthest * 5;
	const float					rotationUnit			= 24;

	for(uint32_t iOrbiter = 0; iOrbiter < solarSystem.Body.size(); ++iOrbiter) {
		const ::gpk::SCelestialBody	& body					= solarSystem.Body.Values[iOrbiter];
		{
			const uint32_t				iEntityOrbit			= entityMap.Orbits[iOrbiter];
			gpk_necs(::gpk::initOrbiterOrbit(body, engine.Integrator, engine.GetRigidBody(iEntityOrbit)));
			gpk_necs(::initOrbit(engine, iOrbiter, iEntityOrbit, body.Detail.Planet.DistanceFromSun, distanceScale));
		}
		{
			const uint32_t				iEntityBody				= entityMap.Bodies[iOrbiter];
			gpk_necs(::gpk::initOrbiterBody(body, engine.Integrator, engine.GetRigidBody(iEntityBody), distanceScale, rotationUnit));
			gpk_necs(::initBody(engine, iOrbiter, iEntityBody, (float)body.Detail.Planet.Diameter, distanceScale, body.Detail.Planet.RingSystem));
		}
		::gpk::rgbaf				color					= colors[iOrbiter];
		const ::gpk::eid_t			entities[2]				= {entityMap.Orbits[iOrbiter], entityMap.Bodies[iOrbiter]};
		for(uint32_t i = 0; i < 2; ++i) {
			color.a					= i ? 1 : .05f;
			::initSkin(engine, color, iOrbiter, entities[i]);
		}
	}

	engine.Update(86400 * 365 * 4);
	return 0;
}
