#include "solarsystem.h"
#include "gpk_engine_orbit.h"

#include "gpk_png.h"

struct SPlanet {
	float	Mass				;
	float	Scale				;
	float	Day					;
	float	AxialTilt			;
	float	Distance			;
	float	OrbitalPeriod		;
	float	OrbitalVelocity		;
	float	OrbitalInclination	;
	float	OrbitalEccentricity	;
	float	Images				;
};
																				//	- Mercury			- Venus				- Earth				- Mars				- Jupiter				- Saturn				- Uranus				- Neptune				- Pluto
stxp double			PLANET_MASSES				[::ssg::PLANET_COUNT]	=	{	0.330f				, 4.87f				, 5.97f				, 0.642f			, 1899					, 568					, 86.8f					, 102					, 0.0125f			};
stxp double			PLANET_SCALES				[::ssg::PLANET_COUNT]	=	{	4879				, 12104				, 12756				, 6792				, 142984				, 120536				, 51118					, 49528					, 2390				};
stxp double			PLANET_DAY					[::ssg::PLANET_COUNT]	=	{	4222.6f				, 2802.0f			, 23.9f				, 24.7f				, 9.9f					, 10.7f					, 17.2f					, 16.1f					, 153.3f			};
stxp double			PLANET_AXIALTILT			[::ssg::PLANET_COUNT]	=	{	0.01f				, 177.4f			, 23.4f				, 25.2f				, 3.1f					, 26.7f					, 97.8f					, 28.3f					, 122.5f			};
stxp double			PLANET_DISTANCE				[::ssg::PLANET_COUNT]	=	{	57.9f				, 108.2f			, 149.6f			, 227.9f			, 778.6f				, 1433.5f				, 2872.5f				, 4495.1f				, 5870.0f			};

stxp double			PLANET_ORBITALPERIOD		[::ssg::PLANET_COUNT]	=	{	88.0f				, 224.7f			, 365.2f			, 687.0f			, 4331					, 10747					, 30589					, 59800					, 90588				};
stxp double			PLANET_ORBITALVELOCITY		[::ssg::PLANET_COUNT]	=	{	47.9f				, 35.0f				, 29.8f				, 24.1f				, 13.1f					, 9.7f					, 6.8f					, 5.4f					, 4.7f				};
stxp double			PLANET_ORBITALINCLINATION	[::ssg::PLANET_COUNT]	=	{	7.0					, 3.4f				, 0.0f				, 1.9f				, 1.3f					, 2.5f					, 0.8f					, 1.8f					, 17.2f				};
stxp double			PLANET_ORBITALECCENTRICITY	[::ssg::PLANET_COUNT]	=	{	0.205f				, 0.007f			, 0.017f			, 0.094f			, 0.049f				, 0.057f				, 0.046f				, 0.011f				, 0.244f			};
stxp const char*		PLANET_IMAGE				[::ssg::PLANET_COUNT]	=	{	"mercury_color.png"	, "venus_color.png"	, "earth_color.png"	, "mars_color.png"	, "jupiter_color.png"	, "saturn_color.png"	, "uranus_color.png"	, "neptune_color.png"	, "pluto_color.png"	};

::gpk::err_t			ssg::solarSystemSetup	(::ssg::SSolarSystemGame & solarSystem) {
	solarSystem.Geometries.resize(1);
	::gpk::SParamsSphere		params					= {};
	params.CellCount		= {16, 16};
	params.Radius			= .5f;
	::gpk::geometryBuildSphere(solarSystem.Geometries[0], params);

	const ::gpk::vcst_t			ssFileFolder			= ".";
	const ::gpk::vcst_t			ssFileName				= "gpk_solar_system.json";
	::gpk::astsc_t<4096>		filePath				= {};
	sprintf_s(filePath.Storage, "%s/%s", ssFileFolder.begin(), ssFileName.begin());

	if_fail_fe(::gpk::planetarySystemSetup(solarSystem, ::gpk::vcst_t{filePath.Storage}));

//	::gpk::geometryBuildFromSTL();

	SPlanet						planet					= {};
	
	::gpk::SRigidBodyIntegrator	& bodies				= solarSystem.Engine.Integrator;
	::ssg::SScene				& scene					= solarSystem.Scene;
	solarSystem.Scene.Pivot.resize(::ssg::PLANET_COUNT + 1); //
	for(uint32_t iModel = 0; iModel < solarSystem.Scene.Pivot.size(); ++iModel) {
		int32_t						iPlanet					= iModel - 1;
		{ // Set up rigid body
			::ssg::SModelPivot			& model					= scene.Pivot[iModel];
			const float					scale					= (0 == iModel) ? 10.0f : float(1.0 / PLANET_SCALES[ssg::PLANET_EARTH] * PLANET_SCALES[iPlanet]);
			model.Scale				= {scale, scale, scale};
			model.Position			= {0, 0};
		}
		if(iModel) { // Set up rigid body
			/*int32_t iBody = */::gpk::createOrbiter(bodies
				, ::gpk::AXIS_X_POSITIVE
				, ::gpk::AXIS_Y_NEGATIVE
				, PLANET_ORBITALINCLINATION	[iPlanet]
				, PLANET_ORBITALPERIOD		[iPlanet]
				, PLANET_DAY				[ssg::PLANET_EARTH]
				, PLANET_AXIALTILT			[iPlanet]
				, PLANET_DISTANCE			[iPlanet]
				, 1.0 / PLANET_DISTANCE		[ssg::PLANET_COUNT - 1] * 2500
				, PLANET_DAY				[iPlanet]
				, PLANET_DAY				[ssg::PLANET_EARTH]
				);
		}
	}
	solarSystem.Entities.push_back({-1, 0, 0, 0, -1});
	for(uint32_t iPlanet = 0; iPlanet < ssg::PLANET_COUNT; ++iPlanet) {
		const uint32_t				iBodyOrbit				= iPlanet * 2;
		const uint32_t				iBodyPlanet				= iBodyOrbit + 1;
		int32_t						iEntityOrbit			= solarSystem.Entities.push_back({0, -1, -1, -1, (int32_t)iBodyOrbit});
		solarSystem.Entities[0].Children.push_back(iEntityOrbit);
		int32_t						iEntityPlanet			= solarSystem.Entities.push_back({iEntityOrbit, 0, (int32_t)iPlanet + 1, (int32_t)iPlanet + 1, (int32_t)iBodyPlanet});
		solarSystem.Entities[iEntityOrbit].Children.push_back(iEntityPlanet);
	}
	for(uint32_t iRigidBody = 0; iRigidBody < solarSystem.Engine.Integrator.Flags.size(); ++iRigidBody)
		solarSystem.Engine.Integrator.Flags[iRigidBody].Active	= true;

	::gpk::bgra					colors []				=
		{ ::gpk::YELLOW
		, ::gpk::DARKRED
		, ::gpk::ORANGE
		, ::gpk::BLUE
		, ::gpk::LIGHTRED
		, ::gpk::LIGHTGRAY
		, ::gpk::LIGHTYELLOW
		, ::gpk::GREEN
		, ::gpk::DARKGRAY
		, ::gpk::YELLOW
		};

	::gpk::SSurfaceManager		& surfaces				= solarSystem.Engine.Scene->Graphics->Surfaces;


	solarSystem.Images.resize(ssg::PLANET_COUNT + 1);


	::gpk::SPNGData				pngCache;
	::gpk::img8bgra				loaded;

	if_fail_e(::gpk::pngLoadFile(pngCache, "../gpk_data/images/sun_color.png", loaded));

	int32_t						iSurface				= surfaces.Create(::gpk::vcst_t{"sun_color.png"});

	surfaces[iSurface]->Data					= loaded.Texels.bytes();
	surfaces[iSurface]->Desc.BitDepth			= 8;
	surfaces[iSurface]->Desc.ColorType			= ::gpk::COLOR_TYPE_BGRA;
	surfaces[iSurface]->Desc.Dimensions			= loaded.metrics().u1();
	surfaces[iSurface]->Desc.MethodFilter		= 0;
	surfaces[iSurface]->Desc.MethodInterlace	= 0;
	surfaces[iSurface]->Desc.MethodCompression	= 0;
	solarSystem.Images[0] = ::gpk::gc8bgra{(const gpk::bgra*)surfaces[iSurface]->Data.begin(), surfaces[iSurface]->Desc.Dimensions.u2()};

	for(uint32_t iPlanet = 0; iPlanet < ::ssg::PLANET_COUNT; ++iPlanet) {
		iSurface				= surfaces.Create(::gpk::vcst_t{PLANET_IMAGE[iPlanet], (uint32_t)-1});

		char						finalPath[256]		= {};
		sprintf_s(finalPath, "../gpk_data/images/%s", PLANET_IMAGE[iPlanet]);
		::gpk::pngLoadFile(pngCache, finalPath, loaded);

		surfaces[iSurface]->Data					= loaded.Texels.bytes();
		surfaces[iSurface]->Desc.BitDepth			= 8;
		surfaces[iSurface]->Desc.ColorType			= ::gpk::COLOR_TYPE_BGRA;
		surfaces[iSurface]->Desc.Dimensions			= loaded.metrics().u1();
		surfaces[iSurface]->Desc.MethodFilter		= 0;
		surfaces[iSurface]->Desc.MethodInterlace	= 0;
		surfaces[iSurface]->Desc.MethodCompression	= 0;
		solarSystem.Images[iPlanet + 1] = ::gpk::gc8bgra{(const gpk::bgra*)surfaces[iSurface]->Data.begin(), surfaces[iSurface]->Desc.Dimensions.u2()};
	}

	//for(uint32_t iImage = 0; iImage < solarSystem.Images.size(); ++iImage) {
	//	if(solarSystem.Images[iImage].Texels.size())
	//		continue;
	//	solarSystem.Images[iImage].resize(::gpk::n2u1_t{512, 512});
	//	for(uint32_t y = 0; y < solarSystem.Images[iImage].metrics().y; ++y) { // Generate noise color for planet texture
	//		const double															ecuatorialShade			= cos(y * (1.0 / solarSystem.Images[iImage].metrics().y * ::gpk::math_2pi)) + 1.5;
	//		uint32_t																rowOffset				= y * solarSystem.Images[iImage].metrics().x;
	//		for(uint32_t x = 0; x < solarSystem.Images[iImage].metrics().x; ++x) {
	//			solarSystem.Images[iImage].Texels[rowOffset + x]	= (::gpk::SColorFloat(colors[iImage % ::gpk::size(colors)] * ecuatorialShade * (1.0 - (rand() / 3.0 / (double)RAND_MAX)))).Clamp();
	//		}
	//	}
	//}
	bodies.Integrate((365 * 4 + 1) * 10);	// Update physics

	solarSystem.Scene.Camera.Target		= {};
	solarSystem.Scene.Camera.Position	= {-0.000001f, 500, -1000};
	//solarSystem.Scene.Camera.Up			= {0, 1, 0};
	return 0;
}
