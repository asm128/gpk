#include "solarsystem.h"
#include "gpk_geometry_draw.h"
#include "gpk_png.h"
#include "gpk_array_ptr.h"
#include "gpk_stl.h"
#include "gpk_geometry_buffers.h"

#include <cstring>
#include <cstdint>
#include <algorithm>

#if defined(GPK_WINDOWS)
#	include <Windows.h>
#endif
#include "gpk_raster_lh.h"

//static	::gpk::err_t	geometryBuildFromSTL			(const ::gpk::SSTLFile & stlFile, ::gpk::SGeometryTriangles & geometry) {
//	geometry.Triangles		.resize(stlFile.Triangles.size());
//	geometry.Normals		.resize(stlFile.Triangles.size());
//	geometry.TextureCoords	.resize(stlFile.Triangles.size());
//	for(uint32_t iTriangle = 0; iTriangle < stlFile.Triangles.size(); ++iTriangle) {
//		geometry.Normals		[iTriangle] = {stlFile.Triangles[iTriangle].Normal, stlFile.Triangles[iTriangle].Normal, stlFile.Triangles[iTriangle].Normal};
//		geometry.Triangles		[iTriangle] = stlFile.Triangles[iTriangle].Triangle;
//		geometry.TextureCoords	[iTriangle] = {};
//	}
//	return 0;
//}

static	::gpk::err_t	drawDebris			(::gpk::g8bgra targetPixels, ::ssg::SDebris & debris, const ::gpk::m4<float> & matrixVPV, ::gpk::gu2_t depthBuffer)	{
	::gpk::apod<::gpk::n2s2_t>	pixelCoords;
	for(uint32_t iParticle = 0; iParticle < debris.Brightness.size(); ++iParticle) {
		::gpk::rgbaf				colorShot			= debris.Colors[iParticle % ::gpk::size(debris.Colors)];
		::gpk::n3f2_t				starPos				= debris.Particles.Position[iParticle];
		starPos					= matrixVPV.Transform(starPos);
		const ::gpk::n2s2_t			pixelCoord			= {(int32_t)starPos.x, (int32_t)starPos.y};
		if( pixelCoord.y < 0 || pixelCoord.y >= (int32_t)targetPixels.metrics().y
		 || pixelCoord.x < 0 || pixelCoord.x >= (int32_t)targetPixels.metrics().x
		)
			continue;
		if(starPos.z > 1 || starPos.z < 0)
			continue;
		uint32_t					depth				= uint32_t(starPos.z * 0xFFFFFFFFU);
		if(depth > depthBuffer[pixelCoord.y][pixelCoord.x])
			continue;

		::gpk::rgbaf				starFinalColor		= colorShot * debris.Brightness[iParticle];
		starFinalColor.g		= ::gpk::max(0.0f, starFinalColor.g - (1.0f - ::gpk::min(1.0f, debris.Brightness[iParticle] * 2.5f * (1.0f / debris.Brightness.size() * iParticle * 2))));
		starFinalColor.b		= ::gpk::max(0.0f, starFinalColor.b - (1.0f - ::gpk::min(1.0f, debris.Brightness[iParticle] * 2.5f * (1.0f / debris.Brightness.size() * iParticle * 1))));
		//::gpk::setPixel(targetPixels, pixelCoord, starFinalColor);
		const double				brightRadius		= 2.0;
		const double				brightRadiusSquared	= brightRadius * brightRadius;
		double						brightUnit			= 1.0 / brightRadiusSquared;
		for(int32_t y = (int32_t)-brightRadius - 1; y < (int32_t)brightRadius + 1; ++y)
		for(int32_t x = (int32_t)-brightRadius - 1; x < (int32_t)brightRadius + 1; ++x) {
			::gpk::n2f2_t				brightPos			= {(float)x, (float)y};
			const double				brightDistance		= brightPos.LengthSquared();
			if(brightDistance <= brightRadiusSquared) {
				::gpk::n2s2_t				blendPos			= pixelCoord + (brightPos).Cast<int32_t>();
				if( blendPos.y < 0 || blendPos.y >= (int32_t)targetPixels.metrics().y
				 || blendPos.x < 0 || blendPos.x >= (int32_t)targetPixels.metrics().x
				)
					continue;
				uint32_t					& blendVal			= depthBuffer[blendPos.y][blendPos.x];
				//if(depth > blendVal)
				//	continue;
				blendVal				= depth;
				double						finalBrightness		= 1.0 - (brightDistance * brightUnit);
				::gpk::bgra					& pixelVal			= targetPixels[blendPos.y][blendPos.x];
				::gpk::rgbaf				pixelColor			= starFinalColor * finalBrightness + pixelVal;
				pixelVal				= pixelColor.Clamp();
			}
		}
	}
	return 0;
}

static	::gpk::err_t	updateEntityTransforms		(uint32_t iEntity, ::gpk::array_obj<::ssg::SEntity> & entities, ssg::SScene & scene, ::gpk::SRigidBodyIntegrator & bodies)	{
	const ssg::SEntity			& entity					= entities[iEntity];
	if(-1 == entity.Body)
		scene.Transform[iEntity]							= (-1 == entity.Parent) ? bodies.MatrixIdentity4 : scene.Transform[entity.Parent];
	else {
		if(-1 == entity.Parent)
			bodies.GetTransform(entity.Body, scene.Transform[iEntity]);
		else {
			::gpk::m4<float>									matrixBody					= {};
			bodies.GetTransform(entity.Body, matrixBody);
			scene.Transform[iEntity]							= matrixBody * scene.Transform[entity.Parent];
		}
	}
	for(uint32_t iChild = 0; iChild < entity.Children.size(); ++iChild) {
		const uint32_t											iChildEntity				= entity.Children[iChild];
		::updateEntityTransforms(iChildEntity, entities, scene, bodies);
	}
	return 0;
}

int													ssg::solarSystemUpdate			(ssg::SSolarSystemGame & solarSystem, double secondsLastFrame, ::gpk::pobj<::gpk::rtbgra8d32> & target)	{
	// ------------------------------------------- Handle input
	::gpk::SRigidBodyIntegrator								& bodies						= solarSystem.Engine.Integrator;
	::ssg::SScene											& scene							= solarSystem.Scene;

	scene.Transform.resize(solarSystem.Entities.size());
	for(uint32_t iEntity = 0; iEntity < solarSystem.Entities.size(); ++iEntity) {
		const ::ssg::SEntity								& entity					= solarSystem.Entities[iEntity];
		if(-1 != entity.Parent)	// process root entities
			::updateEntityTransforms(iEntity, solarSystem.Entities, scene, bodies);
		else
			solarSystem.Scene.Transform[iEntity].Identity();
	}

	static float timeScale = 1;
	// ------------------------------------------- Handle input
	::gpk::SCameraPoints									& camera					= solarSystem.Scene.Camera;
#if defined(GPK_WINDOWS)
	if(GetAsyncKeyState('Q')) camera.Position.z				-= (float)secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 100 : 10);
	if(GetAsyncKeyState('E')) camera.Position.z				+= (float)secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 100 : 10);
	if(GetAsyncKeyState('S')) camera.Position				+= camera.Position / camera.Position.Length() * (GetAsyncKeyState(VK_SHIFT) ? 100 : 2) * secondsLastFrame;
	if(GetAsyncKeyState('W')) camera.Position				-= camera.Position / camera.Position.Length() * (GetAsyncKeyState(VK_SHIFT) ? 100 : 2) * secondsLastFrame;
	if(GetAsyncKeyState('A')) camera.Position.RotateY( (GetAsyncKeyState(VK_SHIFT) ? 100 : 2) * secondsLastFrame);
	if(GetAsyncKeyState('D')) camera.Position.RotateY(-(GetAsyncKeyState(VK_SHIFT) ? 100 : 2) * secondsLastFrame);
	if(GetAsyncKeyState('0')) { ; camera.Target = scene.Transform[0 * 2].GetTranslation(); camera.Position = camera.Target + ::gpk::n3f2_t{scene.Pivot[solarSystem.Entities[0 * 2].Model].Scale.x * 10, 0, 0}; }
	if(GetAsyncKeyState('1')) { ; camera.Target = scene.Transform[1 * 2].GetTranslation(); camera.Position = camera.Target + ::gpk::n3f2_t{scene.Pivot[solarSystem.Entities[1 * 2].Model].Scale.x * 10, 0, 0}; }
	if(GetAsyncKeyState('2')) { ; camera.Target = scene.Transform[2 * 2].GetTranslation(); camera.Position = camera.Target + ::gpk::n3f2_t{scene.Pivot[solarSystem.Entities[2 * 2].Model].Scale.x * 10, 0, 0}; }
	if(GetAsyncKeyState('3')) { ; camera.Target = scene.Transform[3 * 2].GetTranslation(); camera.Position = camera.Target + ::gpk::n3f2_t{scene.Pivot[solarSystem.Entities[3 * 2].Model].Scale.x * 10, 0, 0}; }
	if(GetAsyncKeyState('4')) { ; camera.Target = scene.Transform[4 * 2].GetTranslation(); camera.Position = camera.Target + ::gpk::n3f2_t{scene.Pivot[solarSystem.Entities[4 * 2].Model].Scale.x * 10, 0, 0}; }
	if(GetAsyncKeyState('5')) { ; camera.Target = scene.Transform[5 * 2].GetTranslation(); camera.Position = camera.Target + ::gpk::n3f2_t{scene.Pivot[solarSystem.Entities[5 * 2].Model].Scale.x * 10, 0, 0}; }
	if(GetAsyncKeyState('6')) { ; camera.Target = scene.Transform[6 * 2].GetTranslation(); camera.Position = camera.Target + ::gpk::n3f2_t{scene.Pivot[solarSystem.Entities[6 * 2].Model].Scale.x * 10, 0, 0}; }
	if(GetAsyncKeyState('7')) { ; camera.Target = scene.Transform[7 * 2].GetTranslation(); camera.Position = camera.Target + ::gpk::n3f2_t{scene.Pivot[solarSystem.Entities[7 * 2].Model].Scale.x * 10, 0, 0}; }
	if(GetAsyncKeyState('8')) { ; camera.Target = scene.Transform[8 * 2].GetTranslation(); camera.Position = camera.Target + ::gpk::n3f2_t{scene.Pivot[solarSystem.Entities[8 * 2].Model].Scale.x * 10, 0, 0}; }
	if(GetAsyncKeyState('9')) { ; camera.Target = scene.Transform[9 * 2].GetTranslation(); camera.Position = camera.Target + ::gpk::n3f2_t{scene.Pivot[solarSystem.Entities[9 * 2].Model].Scale.x * 10, 0, 0}; }
	if(GetAsyncKeyState(VK_SUBTRACT))	{ timeScale *= .5f * (float)secondsLastFrame; }
	if(GetAsyncKeyState(VK_ADD))		{ timeScale += .1f * (float)secondsLastFrame + timeScale * .25f; }
#endif
	solarSystem.SunFire.SpawnSpherical(100, {}, 5, 1, 10);

	// Update physics
	solarSystem.SunFire.Update(secondsLastFrame * 2);
	bodies.Integrate(secondsLastFrame * timeScale);

	//------------------------------------------- Transform and Draw
	::gpk::g8bgra		targetPixels				= target->Color.View;
	memset((void*)targetPixels.begin(), 0, sizeof(::gpk::bgra) * targetPixels.size());
	::gpk::bgra							colorBackground		= {0x20, 0x8, 0x4};
	//colorBackground				+= (colorBackground * (0.5 + (0.5 / 65535 * rand())) * ((rand() % 2) ? -1 : 1)) ;
	for(uint32_t y = 0; y < targetPixels.metrics().y; ++y) // Generate noise color for planet texture
	for(uint32_t x = 0; x < targetPixels.metrics().x; ++x)
		targetPixels.begin()[y * targetPixels.metrics().x + x]	= colorBackground;

	::gpk::n3f2_t					lightVector					= camera.Position;
	lightVector.Normalize();

	::gpk::m4f2_t						matrixView					= {};
	::gpk::m4f2_t						matrixProjection			= {};
	::gpk::m4f2_t						matrixViewport				= {};
	stxp	::gpk::n3f2_t				cameraUp		= {0, 1};
	matrixView.LookAt(camera.Position, camera.Target, cameraUp);
	matrixProjection.FieldOfView(::gpk::math_pi * .25, targetPixels.metrics().x / (double)targetPixels.metrics().y, ::gpk::minmaxf2_t{0.1f, 10000.0f});
	matrixViewport.ViewportLH(targetPixels.metrics().Cast<uint16_t>());
	matrixView											*= matrixProjection;
	matrixView											*= matrixViewport;

	::gpk::apod<::gpk::n2s1_t>		pixelCoords					= {};
	::gpk::apod<::gpk::trif2_t>		pixelVertexWeights			= {};
	::gpk::SModelMatrices			matrices					= {};
	::gpk::gu2_t						depthBuffer					= target->DepthStencil.View;
	memset(depthBuffer.begin(), -1, sizeof(uint32_t) * depthBuffer.size());
	::gpk::apod<::gpk::SLight3>						lightPoints;
	::gpk::apod<::gpk::rgbaf>					lightColors;
	lightPoints.push_back({{0,0,0}, 10000});
	lightColors.push_back(::gpk::WHITE);

	for(uint32_t iEntity = 0; iEntity < solarSystem.Entities.size(); ++iEntity) {
		::gpk::m4<float>									matrixTransform				= scene.Transform[iEntity];
		const ::ssg::SEntity									& entity					= solarSystem.Entities[iEntity];
		if(-1 == entity.Model)
			continue;
		if(-1 == entity.Images)
			continue;

		matrices.Scale		.Scale			(scene.Pivot[entity.Model].Scale	, true);
		matrices.Position	.SetTranslation	(scene.Pivot[entity.Model].Position	, true);
		::gpk::gc8bgra						entityImage					= solarSystem.Images[entity.Images];
		::gpk::SGeometryBuffers				& entityGeometry			= solarSystem.Geometries[entity.Geometry];
		matrixTransform										= matrices.Scale * matrices.Position * matrixTransform;
		::gpk::m4<float>									matrixTransformView			= matrixTransform * matrixView;
		for(uint32_t iTriangle = 0; iTriangle < entityGeometry.PositionIndices.size() / 3; ++iTriangle) {
			pixelCoords			.clear();
			pixelVertexWeights	.clear();
			::gpk::drawTriangle(targetPixels, entityGeometry, iTriangle, matrixTransform, matrixTransformView, lightVector, iEntity ? ::gpk::BLACK : ::gpk::WHITE, pixelCoords, pixelVertexWeights, entityImage, lightPoints, lightColors, depthBuffer);
		}
	}


	return ::drawDebris(targetPixels, solarSystem.SunFire, matrixView, depthBuffer);
}

