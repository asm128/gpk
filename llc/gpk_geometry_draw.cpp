#include "gpk_geometry_draw.h"

::gpk::error_t			gpk::drawTriangle
	( const ::gpk::g8bgra	targetPixels
	, const ::gpk::STrianglesIndexed	& geometry
	, const int							iTriangle
	, const ::gpk::m4f32				& matrixTransform
	, const ::gpk::m4f32				& matrixTransformView
	, const ::gpk::n3f32				& lightVector
	, const ::gpk::rgbaf				& lightColor
	, ::gpk::apod<::gpk::n2i16>			& pixelCoords
	, ::gpk::apod<::gpk::trif32>		& pixelVertexWeights
	, ::gpk::gc8bgra					textureImage
	, ::gpk::apod<::gpk::SLight3>		& lightPoints
	, ::gpk::apod<::gpk::rgbaf>			& lightColors
	, ::gpk::grid<uint32_t>			depthBuffer
	) {
	::gpk::triu32				triangleIndices		= *(const ::gpk::tri<uint32_t>*)&geometry.PositionIndices[iTriangle * 3];
	::gpk::tri3f32				triangleWorld		= {geometry.Positions[triangleIndices.A], geometry.Positions[triangleIndices.B], geometry.Positions[triangleIndices.C]};
	::gpk::tri3f32				triangle			= triangleWorld;
	::gpk::transform(triangle, matrixTransformView);
	if( triangle.A.z < 0 || triangle.A.z >= 1
	 || triangle.B.z < 0 || triangle.B.z >= 1
	 || triangle.C.z < 0 || triangle.C.z >= 1
	)
		return 0;

	if( (triangle.A.x < 0 && triangle.B.x < 0 && triangle.C.x < 0)
	 || (triangle.A.y < 0 && triangle.B.y < 0 && triangle.C.y < 0)
	)
		return 0;

	if( (triangle.A.x >= targetPixels.metrics().x && triangle.B.x >= targetPixels.metrics().x && triangle.C.x >= targetPixels.metrics().x)
	 || (triangle.A.y >= targetPixels.metrics().y && triangle.B.y >= targetPixels.metrics().y && triangle.C.y >= targetPixels.metrics().y)
	)
		return 0;

	int32_t						countPixels			= 0;
	::gpk::transform(triangleWorld, matrixTransform);
	::gpk::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);

	const ::gpk::tri3f32		triangleNormals			= {geometry.Normals[triangleIndices.A], geometry.Normals[triangleIndices.B], geometry.Normals[triangleIndices.C]};
	const ::gpk::tri2f32		triangleTexCoords		= {geometry.TextureCoords[triangleIndices.A], geometry.TextureCoords[triangleIndices.B], geometry.TextureCoords[triangleIndices.C]};
	const ::gpk::n2f32			imageUnit				= {textureImage.metrics().x - 0.000001f, textureImage.metrics().y - 0.000001f};
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		const ::gpk::n2i16			pixelCoord				= pixelCoords		[iPixelCoord];
		const ::gpk::trif32			& vertexWeights			= pixelVertexWeights[iPixelCoord];

		::gpk::n3f32				normal					= ::gpk::triangleWeight(vertexWeights, triangleNormals);
		normal					= matrixTransform.TransformDirection(normal).Normalize();
		double						lightFactorDirectional	= normal.Dot(lightVector);
		::gpk::n3f32				position				= ::gpk::triangleWeight(vertexWeights, triangleWorld);
		::gpk::n2f32				texCoord				= ::gpk::triangleWeight(vertexWeights, triangleTexCoords);
		::gpk::rgbaf				texelColor				= textureImage[(uint32_t)(texCoord.y * imageUnit.y) % textureImage.metrics().y][(uint32_t)(texCoord.x * imageUnit.x) % textureImage.metrics().x];
		::gpk::rgbaf				fragmentColor			= {};

		for(uint32_t iLight = 0; iLight < lightPoints.size(); ++iLight) {
			const ::gpk::SLight3		& light					= lightPoints[iLight];

			::gpk::n3f32				lightToPoint			= light.Position - position;
			::gpk::n3f32				vectorToLight			= lightToPoint;
			vectorToLight.Normalize();
			double						lightFactorPoint		= vectorToLight.Dot(normal);
			if(lightToPoint.Length() > light.Range || lightFactorPoint <= 0)
				continue;
			double						invAttenuation			= ::std::max(0.0, 1.0 - (lightToPoint.Length() / light.Range));
			fragmentColor			+= (texelColor * lightColors[iLight] * invAttenuation).Clamp();
		}
		 (void)lightColor;
		 (void)lightFactorDirectional;

		countPixels				+= ::gpk::setPixel(targetPixels, pixelCoord, ((texelColor * .1) + texelColor * lightColor * lightFactorDirectional + fragmentColor).Clamp());
	}
	return countPixels;
}

