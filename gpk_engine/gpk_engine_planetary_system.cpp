#include "gpk_engine_planetary_system.h"
#include "gpk_engine_celestial_body.h"

#include "gpk_stdstring.h"
#include "gpk_json_expression.h"
#include "gpk_engine_shader.h"
#include "gpk_gui_text.h"

stacxpr	double			ASTRONOMICAL_UNIT_SCALE		= 1.0 / 149597870700;

//static	::gpk::error_t	setupPlanet		(::gpk::SSolarSystem & /*solarSystem*/, ::gpk::SEngine & /*engine*/, const ::gpk::SJSONReader & /*jsonData*/) { return 0; }
//static	::gpk::error_t	setupStar		(::gpk::SSolarSystem & /*solarSystem*/, ::gpk::SEngine & /*engine*/, const ::gpk::SJSONReader & /*jsonData*/) { return 0; }
::gpk::error_t			gpk::planetarySystemSetup	(const ::gpk::SPlanetarySystem & solarSystem, ::gpk::SPlanetarySystemEntityMap & entityMap, ::gpk::SEngine & engine) {
	stacxpr	uint8_t				MAX_COLORS					= 16;
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

	::gpk::SPNGData				pngCache				= {};
	for(uint32_t iOrbiter = 0; iOrbiter < solarSystem.Body.size(); ++iOrbiter) {
		solarSystem.Body.Keys;
		stacxpr char				fileFolder	[]			= "../gpk_data/images";
		char						fileName	[64]		= {};
		sprintf_s(fileName, "%s_color.png", ::gpk::toString(solarSystem.Body.Keys[0]).begin());
		es_if(engine.CreateImageFromFile(fileFolder, fileName));
	}

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

static	::gpk::error_t	initOrbit				(::gpk::SEngine & engine, int32_t iOrbiter, int32_t iEntity, float distanceFromCenter, float distanceScale) {
	gpk_necs(engine.SetPhysicsActive(iEntity, iOrbiter));
	gpk_necs(engine.SetCollides	(iEntity, false));

	::gpk::n3f32				scale					= {distanceFromCenter, distanceFromCenter, distanceFromCenter};
	if(iOrbiter)
		scale					*= distanceScale * 2;
	else
		scale					= {.1f, .1f, .1f};
	gpk_necs(engine.SetMeshScale(iEntity, scale, true));
	gpk_necs(engine.SetHidden	(iEntity, false));
	return 0;
}

stacxpr	::gpk::n3f32	SUN_SCALE				= {.00625f, .00625f, .00625f};

static	::gpk::error_t	initBody				(::gpk::SEngine & engine, int32_t iEntity, bool isStar, int32_t iParent, float diameter, float distanceScale, bool ringSystem) {
	gpk_necs(engine.SetPhysicsActive(iEntity, iParent >= 0 || isStar));
	gpk_necs(engine.SetCollides		(iEntity, false));

	::gpk::n3f32				scale					= {diameter, diameter, diameter};
	scale					= isStar ? SUN_SCALE : scale * distanceScale;
	gpk_necs(engine.SetMeshScale(iEntity, scale, true));
	::gpk::TFuncPixelShader		& ps					= isStar ? ::gpk::psSphereAxis	: ringSystem ? ::gpk::psSphereMeridian	: ::gpk::psSphereSolid		;
	const ::gpk::vcc			psName					= isStar ? ::gpk::vcs{"psSun"}	: ringSystem ? ::gpk::vcs{"psGasGiant"}	: ::gpk::vcs{"psSphereSolid"};
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
	solarSystem.Body.Values.max<float>(fFurthest, [](const ::gpk::SCelestialBody & body){ return body.DistanceFromParent; });
	const float					distanceScale			= 1.0f / fFurthest * 5;
	const float					rotationUnit			= 24;

	for(uint32_t iOrbiter = 0; iOrbiter < solarSystem.Body.size(); ++iOrbiter) {
		const ::gpk::SCelestialBody	& body					= solarSystem.Body.Values[iOrbiter];
		{
			const uint32_t				iEntity					= entityMap.Orbits[iOrbiter];
			gpk_necs(::gpk::initOrbiterOrbit(body, engine.Integrator, engine.GetRigidBody(iEntity)));
			gpk_necs(::initOrbit(engine, iOrbiter, iEntity, body.DistanceFromParent, distanceScale));
		}
		{
			const uint32_t				iEntity					= entityMap.Bodies[iOrbiter];
			gpk_necs(::gpk::initOrbiterBody(body, engine.Integrator, engine.GetRigidBody(iEntity), distanceScale, rotationUnit));
			gpk_necs(::initBody(engine, iEntity, ::gpk::CELESTIAL_BODY_Star == solarSystem.Type[iOrbiter], solarSystem.Parent[iOrbiter], (float)body.Diameter, distanceScale * .00012f, body.Detail.Planet.RingSystem));
		}
		::gpk::rgbaf				color					= colors[iOrbiter];
		const ::gpk::eid_t			entities[2]				= {entityMap.Orbits[iOrbiter], entityMap.Bodies[iOrbiter]};
		for(uint32_t i = 0; i < 2; ++i) {
			color.a					= i ? 1 : .05f;
			gpk_necs(::initSkin(engine, color, iOrbiter, entities[i]));
		}
	}

	engine.Update(365 * .5);
	return 0;
}
