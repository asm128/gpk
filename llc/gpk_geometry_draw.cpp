#include "gpk_geometry_draw.h"

gpk::err_t 		gpk::drawTriangle
	( const g8bgra				targetPixels
	, const SGeometryBuffers		& geometry
	, const u2_t						iTriangle
	, const m4f2_t				& matrixTransform
	, const m4f2_t				& matrixTransformView
	, const n3f2_t				& lightVector
	, const rgbaf				& lightColor
	, apod<n2s1_t>		& pixelCoords
	, apod<trif2_t>		& pixelVertexWeights
	, gc8bgra					textureImage
	, apod<SLight3>		& lightPoints
	, apod<rgbaf>			& lightColors
	, grid<u2_t>			depthBuffer
	) {
	triu2_t				triangleIndices		= *(const tri<u2_t>*)&geometry.PositionIndices[iTriangle * 3];
	tri3f2_t				triangleWorld		= {geometry.Positions[triangleIndices.A], geometry.Positions[triangleIndices.B], geometry.Positions[triangleIndices.C]};
	tri3f2_t				triangle			= triangleWorld;
	transform(triangle, matrixTransformView);
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

	s2_t						countPixels			= 0;
	transform(triangleWorld, matrixTransform);
	drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);

	const tri3f2_t		triangleNormals			= {geometry.Normals[triangleIndices.A], geometry.Normals[triangleIndices.B], geometry.Normals[triangleIndices.C]};
	const tri2f2_t		triangleTexCoords		= {geometry.TextureCoords[triangleIndices.A], geometry.TextureCoords[triangleIndices.B], geometry.TextureCoords[triangleIndices.C]};
	const n2f2_t			imageUnit				= {textureImage.metrics().x - 0.000001f, textureImage.metrics().y - 0.000001f};
	for(u2_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		const n2s1_t			pixelCoord				= pixelCoords		[iPixelCoord];
		const trif2_t			& vertexWeights			= pixelVertexWeights[iPixelCoord];

		n3f2_t				normal					= triangleWeight(vertexWeights, triangleNormals);
		normal					= matrixTransform.TransformDirection(normal).Normalize();
		f3_t						lightFactorDirectional	= normal.Dot(lightVector);
		n3f2_t				position				= triangleWeight(vertexWeights, triangleWorld);
		n2f2_t				texCoord				= triangleWeight(vertexWeights, triangleTexCoords);
		rgbaf				texelColor				= textureImage[(u2_t)(texCoord.y * imageUnit.y) % textureImage.metrics().y][(u2_t)(texCoord.x * imageUnit.x) % textureImage.metrics().x];
		rgbaf				fragmentColor			= {};

		for(u2_t iLight = 0; iLight < lightPoints.size(); ++iLight) {
			const SLight3		& light					= lightPoints[iLight];

			n3f2_t				lightToPoint			= light.Position - position;
			n3f2_t				vectorToLight			= lightToPoint;
			vectorToLight.Normalize();
			f3_t						lightFactorPoint		= vectorToLight.Dot(normal);
			if(lightToPoint.Length() > light.Range || lightFactorPoint <= 0)
				continue;
			f3_t						invAttenuation			= max(0.0, 1.0 - (lightToPoint.Length() / light.Range));
			fragmentColor			+= (texelColor * lightColors[iLight] * invAttenuation).Clamp();
		}
		 (void)lightColor;
		 (void)lightFactorDirectional;

		countPixels				+= setPixel(targetPixels, pixelCoord, ((texelColor * .1) + texelColor * lightColor * lightFactorDirectional + fragmentColor).Clamp());
	}
	return countPixels;
}

