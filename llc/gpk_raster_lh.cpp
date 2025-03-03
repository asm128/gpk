#include "gpk_raster_lh.h"

#ifndef GPK_ATMEL

#include <algorithm>

::gpk::error_t			gpk::pixelBlend			(::gpk::g8bgra pixels, ::gpk::n2i16 position, ::gpk::bgra color)	{
	if( (position.x >= 0 && position.x < (int32_t)pixels.metrics().x)
	 && (position.y >= 0 && position.y < (int32_t)pixels.metrics().y)
	) {
		::gpk::bgra					& targetPixel	= pixels[position.y][position.x];
		::gpk::rgbaf				srcPixel		= targetPixel;
		targetPixel				= ::gpk::interpolate_linear(srcPixel, ::gpk::rgbaf(color), color.a / 255.0f);
		return 1;
	}
	return 0;
}

::gpk::error_t			gpk::setPixel			(::gpk::g8bgra pixels, ::gpk::n2i16 position, ::gpk::bgra color)	{
	if( (position.x >= 0 && position.x < (int32_t)pixels.metrics().x)
	 && (position.y >= 0 && position.y < (int32_t)pixels.metrics().y)
	) {
		::gpk::bgra					& targetPixel = pixels[position.y][position.x];
		targetPixel				= {color.b, color.g, color.r, (uint8_t)::gpk::clamped(targetPixel.a + (uint32_t)color.a, 0U, 255U)};
		return 1;
	}
	return 0;
}

::gpk::error_t			gpk::drawRectangle		(::gpk::g8bgra pixels, ::gpk::rect2<int16_t> rectangle, ::gpk::bgra color)	{
	int32_t						countPixels				= 0;
	for(int16_t y = 0; y < (int16_t)rectangle.Size.y; ++y)
	for(int16_t x = 0; x < (int16_t)rectangle.Size.x; ++x)
		countPixels += ::gpk::setPixel(pixels, {(int16_t)(rectangle.Offset.x + x), (int16_t)(rectangle.Offset.y + y)}, color);
	return countPixels;
}

::gpk::error_t			gpk::drawCircle			(::gpk::g8bgra pixels, ::gpk::circle<int16_t> circle, ::gpk::bgra color)	{
	int32_t						countPixels				= 0;
	for(int16_t y = (int16_t)-circle.Radius; y < (int16_t)circle.Radius; ++y)
	for(int16_t x = (int16_t)-circle.Radius; x < (int16_t)circle.Radius; ++x) {
		::gpk::n2i16				position			= {x, y};
		if(position.Length() <= circle.Radius)
			countPixels += ::gpk::setPixel(pixels, {int16_t(circle.Center.x + x), int16_t(circle.Center.y + y)}, color);
	}
	return countPixels;
}

// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
::gpk::error_t			gpk::drawLine       	(::gpk::g8bgra pixels, ::gpk::line2i16 line, ::gpk::bgra color)	{
	int32_t						countPixels				= 0;
	int32_t						dx						= (int32_t)fabs(line.B.x - line.A.x);
	int32_t						sx						= (int32_t)line.A.x < line.B.x ? 1 : -1;
	int32_t						dy						= (int32_t)-fabs(line.B.y - line.A.y);
	int32_t						sy						= (int32_t)line.A.y < line.B.y ? 1 : -1;
	int32_t						err						= dx + dy;  /* error value e_xy */
	while (true) {   /* loop */
		if (line.A.x == line.B.x && line.A.y == line.B.y)
			break;
		int32_t						e2						= 2 * err;
		if (e2 >= dy) {
			err						+= dy; /* e_xy+e_x > 0 */
			line.A.x				+= (int16_t)sx;
			countPixels += setPixel(pixels, {line.A.x, line.A.y}, color);
		}
		if (e2 <= dx) { /* e_xy+e_y < 0 */
			err						+= dx;
			line.A.y				+= (int16_t)sy;
			countPixels += setPixel(pixels, {line.A.x, line.A.y}, color);
		}

	}
	return countPixels;
}


// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
::gpk::error_t			gpk::drawLine       	(const ::gpk::n2u16	offscreenMetrics, ::gpk::line2i16 line, ::gpk::apod<::gpk::n2i16> & pixelCoords)	{
	int32_t						dx						= (int32_t)fabs(line.B.x - line.A.x);
	int32_t						sx						= (int32_t)line.A.x < line.B.x ? 1 : -1;
	int32_t						dy						= (int32_t)-fabs(line.B.y - line.A.y );
	int32_t						sy						= (int32_t)line.A.y < line.B.y ? 1 : -1;
	int32_t						err						= dx + dy;  /* error value e_xy */
	pixelCoords.push_back({line.A.x, line.A.y});
	while (true) {   /* loop */
		if (line.A.x == line.B.x && line.A.y == line.B.y)
			break;
		int32_t						e2						= 2 * err;
		if (e2 >= dy) {
			err						+= dy; /* e_xy+e_x > 0 */
			line.A.x				+= (int16_t)sx;
			if( line.A.x >= 0 && line.A.x < (int32_t)offscreenMetrics.x
			 && line.A.y >= 0 && line.A.y < (int32_t)offscreenMetrics.y
			)
				pixelCoords.push_back({line.A.x, line.A.y});

		}
		if (e2 <= dx) { /* e_xy+e_y < 0 */
			err						+= dx;
			line.A.y				+= (int16_t)sy;
			if( line.A.x >= 0 && line.A.x < (int32_t)offscreenMetrics.x
			 && line.A.y >= 0 && line.A.y < (int32_t)offscreenMetrics.y
			)
				pixelCoords.push_back({line.A.x, line.A.y});
		}

	}
	return 0;
}


// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
::gpk::error_t			gpk::drawLine
	( const ::gpk::n2u16		offscreenMetrics
	, const ::gpk::line3f32		& lineFloat
	, ::gpk::apod<::gpk::n3f32>	& pixelCoords
	, ::gpk::grid<uint32_t>	depthBuffer
	) {
	::gpk::line2i32				line					= {{(int32_t)lineFloat.A.x, (int32_t)lineFloat.A.y}, {(int32_t)lineFloat.B.x, (int32_t)lineFloat.B.y}};
	int32_t						xDiff					= (int32_t)fabs(line.B.x - line.A.x);
	int32_t						yDiff					= (int32_t)-fabs(line.B.y - line.A.y);
	int32_t						sx						= (int32_t)line.A.x < line.B.x ? 1 : -1;
	int32_t						sy						= (int32_t)line.A.y < line.B.y ? 1 : -1;
	int32_t						err						= xDiff + yDiff;  /* error value e_xy */

	bool						yAxis					= fabs(yDiff) > xDiff;
	::gpk::vu32					depthBufferRow			= {};
	if( line.A.x >= 0 && line.A.x < (int32_t)offscreenMetrics.x
	 && line.A.y >= 0 && line.A.y < (int32_t)offscreenMetrics.y
	 && lineFloat.A.z >= 0 && lineFloat.A.z <= 1
	) {
		depthBufferRow			= depthBuffer[line.A.y];
		uint32_t					& depthCell				= depthBufferRow[line.A.x];
		uint32_t					intZ					= uint32_t((double)0xFFFFFFFU * lineFloat.A.z);
		if( depthCell > intZ ) {
			depthCell				= intZ;
			//pixelCoords.push_back({(int32_t)line.A.x, (int32_t)line.A.y});
			pixelCoords.push_back(lineFloat.A);
		}
	}

	//bool								outside				= true;
	const double				factorUnit				= 1.0 / (yAxis ? fabs(yDiff) : xDiff);
	while (true) {   /* loop */
		if (line.A.x == line.B.x && line.A.y == line.B.y)
			break;
		int32_t						e2						= 2 * err;
		if (e2 >= yDiff) {
			err						+= yDiff; /* e_xy+e_x > 0 */
			line.A.x				+= sx;
			if( line.A.x >= 0 && line.A.x < (int32_t)offscreenMetrics.x
			 && line.A.y >= 0 && line.A.y < (int32_t)offscreenMetrics.y
			) {
				depthBufferRow			= depthBuffer[line.A.y];
				const double				factor					= 1.0 / (yAxis ? factorUnit * line.A.y : factorUnit * line.A.x);
				const double				finalZ					= ::gpk::interpolate_linear(lineFloat.A.z, lineFloat.B.z, factor);// lineFloat.B.z * factor + (lineFloat.A.z * (1.0 - factor));
				if (finalZ <= 0 || finalZ >= 1)
					continue;
				const uint32_t				intZ					= uint32_t(0xFFFFFFFU * finalZ);
				uint32_t					& depthCell				= depthBufferRow[line.A.x];
				if(depthCell <= intZ)
					continue;

				depthCell				= intZ;
				pixelCoords.push_back({(float)line.A.x, (float)line.A.y, (float)finalZ});
			}
			//else
			//	outside							= true;
		}
		if (e2 <= xDiff) { /* e_xy+e_y < 0 */
			err						+= xDiff;
			line.A.y				+= sy;
			if( line.A.x >= 0 && line.A.x < (int32_t)offscreenMetrics.x
			 && line.A.y >= 0 && line.A.y < (int32_t)offscreenMetrics.y
			) {
				depthBufferRow					= depthBuffer[line.A.y];
				const double				factor					= 1.0 / (yAxis ? factorUnit * line.A.y : factorUnit * line.A.x);
				const double				finalZ					= ::gpk::interpolate_linear(lineFloat.A.z, lineFloat.B.z, factor);// lineFloat.B.z * factor + (lineFloat.A.z * (1.0 - factor));
				if (finalZ <= 0 || finalZ >= 1)
					continue;
				const uint32_t				intZ					= uint32_t(0xFFFFFFFU * finalZ);
				uint32_t					& depthCell				= depthBufferRow[line.A.x];
				if(depthCell <= intZ)
					continue;

				depthCell				= intZ;
				pixelCoords.push_back({(float)line.A.x, (float)line.A.y, (float)finalZ});
			}
			//else
			//	outside					= true;
		}

	}
	return 0;
}

//https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/
static	double		orient2d		(const ::gpk::line2i16 & segment, const ::gpk::n2i16 & point)	{ return (segment.B.x - segment.A.x) * (point.y - (double)segment.A.y) - (segment.B.y - segment.A.y) * (point.x - (double)segment.A.x); }
//static	double		orient2d		(const ::gpk::line3i16 & segment, const ::gpk::n2i16 & point)	{ return (segment.B.x - segment.A.x) * (point.y - (double)segment.A.y) - (segment.B.y - segment.A.y) * (point.x - (double)segment.A.x); }
static	double		orient2d		(const ::gpk::line3f32 & segment, const ::gpk::n2i16 & point)	{ return (segment.B.x - segment.A.x) * (point.y - (double)segment.A.y) - (segment.B.y - segment.A.y) * (point.x - (double)segment.A.x); }
static	double		orient2d		(const ::gpk::line2f32 & segment, const ::gpk::n2i16 & point)	{ return (segment.B.x - segment.A.x) * (point.y - (double)segment.A.y) - (segment.B.y - segment.A.y) * (point.x - (double)segment.A.x); }

tplt <tpnm _tValue>	_tValue	max3		(_tValue & a, _tValue & b, _tValue & c)			{ return ::std::max(::std::max(a, b), c); }
tplt <tpnm _tValue>	_tValue	min3		(_tValue & a, _tValue & b, _tValue & c)			{ return ::std::min(::std::min(a, b), c); }

::gpk::error_t			gpk::drawTriangle		(::gpk::g8bgra pixels, const ::gpk::tri2<int16_t> & triangle, ::gpk::bgra color){
	// Compute triangle bounding box
	int16_t						minX					= ::min3(triangle.A.x, triangle.B.x, triangle.C.x);
	int16_t						minY					= ::min3(triangle.A.y, triangle.B.y, triangle.C.y);
	int16_t						maxX					= ::max3(triangle.A.x, triangle.B.x, triangle.C.x);
	int16_t						maxY					= ::max3(triangle.A.y, triangle.B.y, triangle.C.y);

	// Clip against screen bounds
	minX							= ::std::max(minX, (int16_t)0);
	minY							= ::std::max(minY, (int16_t)0);
	maxX							= ::std::min(maxX, int16_t(int16_t(pixels.metrics().x)	- 1));
	maxY							= ::std::min(maxY, int16_t(int16_t(pixels.metrics().y)	- 1));

	// Rasterize
	int32_t						countPixels				= 0;
	::gpk::n2i16				p;
	for (p.y = minY; p.y <= maxY; ++p.y) 
	for (p.x = minX; p.x <= maxX; ++p.x) {
		// Determine barycentric coordinates
		double						w0						= ::orient2d({triangle.B, triangle.C}, p);
		double						w1						= ::orient2d({triangle.C, triangle.A}, p);
		double						w2						= ::orient2d({triangle.A, triangle.B}, p);
		// If p is on or inside all edges, render pixel.
		if (w0 < 0 || w1 < 0 || w2 < 0)
			continue;
		countPixels += ::gpk::setPixel(pixels, p, color);
	}
	return countPixels;
}

::gpk::error_t			gpk::drawTriangle
	( const ::gpk::n2u32			targetSize
	, const ::gpk::tri3f32			& triangle
	, ::gpk::apod<::gpk::n2i16>		& pixelCoords
	, ::gpk::apod<::gpk::trif32>	& proportions
	, ::gpk::grid<uint32_t>		depthBuffer
	)	{
	// Compute triangle bounding box
	int16_t						minX					= (int16_t)::min3(triangle.A.x, triangle.B.x, triangle.C.x);
	int16_t						minY					= (int16_t)::min3(triangle.A.y, triangle.B.y, triangle.C.y);
	int16_t						maxX					= (int16_t)::max3(triangle.A.x, triangle.B.x, triangle.C.x);
	int16_t						maxY					= (int16_t)::max3(triangle.A.y, triangle.B.y, triangle.C.y);

	// Clip against screen bounds
	minX					= ::std::max(minX, (int16_t)0);
	minY					= ::std::max(minY, (int16_t)0);
	maxX					= ::std::min(maxX, (int16_t)((int32_t)targetSize.x - 1));
	maxY					= ::std::min(maxY, (int16_t)((int32_t)targetSize.y - 1));

	// Rasterize
	::gpk::n2i16				p;
	for (p.y = minY; p.y <= maxY; ++p.y)
	for (p.x = minX; p.x <= maxX; ++p.x) {
		// Determine barycentric coordinates
		double						w0						= ::orient2d({triangle.B, triangle.C}, p);
		double						w1						= ::orient2d({triangle.C, triangle.A}, p);
		double						w2						= ::orient2d({triangle.A, triangle.B}, p);
		// If p is on or inside all edges, render pixel.
		if (w0 < 0 || w1 < 0 || w2 < 0)
			continue;

		double						proportionABC			= w0 + w1 + w2;
		if(proportionABC == 0)
			continue;
		double						proportionA				= w0 / proportionABC;
		double						proportionB				= w1 / proportionABC;
		double						proportionC				= 1.0 - (proportionA + proportionB);

		// Calculate interpolated depth
		double						finalZ					= triangle.A.z * proportionA;
		finalZ					+= triangle.B.z * proportionB;
		finalZ					+= triangle.C.z * proportionC;
		if(finalZ >= 1.0 || finalZ <= 0)
			continue;

		uint32_t					intZ					= uint32_t(0xFFFFFFFU * finalZ);
		uint32_t					& currentDepth			= depthBuffer[p.y][p.x];
		if(currentDepth <= intZ)
			continue;

		currentDepth			= intZ;
		pixelCoords.push_back(p.s1_t());
		proportions.push_back({(float)proportionA, (float)proportionB, (float)proportionC});

	}
	return 0;
}

::gpk::error_t			gpk::drawTriangle
	( const ::gpk::n2u32			targetSize
	, const ::gpk::tri2f32			& triangle
	, ::gpk::apod<::gpk::n2i16>		& pixelCoords
	, ::gpk::apod<::gpk::trif32>	& proportions
	)	{
	// Compute triangle bounding box
	int16_t						minX					= (int16_t)::min3(triangle.A.x, triangle.B.x, triangle.C.x);
	int16_t						minY					= (int16_t)::min3(triangle.A.y, triangle.B.y, triangle.C.y);
	int16_t						maxX					= (int16_t)::max3(triangle.A.x, triangle.B.x, triangle.C.x);
	int16_t						maxY					= (int16_t)::max3(triangle.A.y, triangle.B.y, triangle.C.y);

	// Clip against screen bounds
	minX					= ::std::max(minX, (int16_t)0);
	minY					= ::std::max(minY, (int16_t)0);
	maxX					= ::std::min(maxX, (int16_t)((int32_t)targetSize.x - 1));
	maxY					= ::std::min(maxY, (int16_t)((int32_t)targetSize.y - 1));

	// Rasterize
	::gpk::n2i16 p;
	for (p.y = minY; p.y <= maxY; ++p.y)
	for (p.x = minX; p.x <= maxX; ++p.x) {
		// Determine barycentric coordinates
		double						w0						= ::orient2d({triangle.B, triangle.C}, p);
		double						w1						= ::orient2d({triangle.C, triangle.A}, p);
		double						w2						= ::orient2d({triangle.A, triangle.B}, p);
		// If p is on or inside all edges, render pixel.
		if (w0 < 0 || w1 < 0 || w2 < 0)
			continue;

		double						proportionABC			= w0 + w1 + w2;
		if(proportionABC == 0)
			continue;
		double						proportionA				= w0 / proportionABC;
		double						proportionB				= w1 / proportionABC;
		double						proportionC				= 1.0 - (proportionA + proportionB);

		pixelCoords.push_back(p.s1_t());
		proportions.push_back({(float)proportionA, (float)proportionB, (float)proportionC});

	}
	return 0;
}

::gpk::error_t			gpk::drawPixels
	( ::gpk::g8bgra		targetPixels
	, const ::gpk::tri3f32				& triangleWorld
	, const ::gpk::n3f32				& normal
	, const ::gpk::tri2f32				& triangleTexCoords
	, const ::gpk::n3f32				& lightVector
	, ::gpk::apod<::gpk::n2i16>			& pixelCoords
	, ::gpk::apod<::gpk::trif32>		& pixelVertexWeights
	, ::gpk::gc8bgra					textureImage
	, ::gpk::apod<::gpk::n3f32>			& lightPoints
	, ::gpk::a8bgra			& lightColors
	, const ::gpk::function<::gpk::error_t(::gpk::g8bgra targetPixels, const ::gpk::n2i16 & pixelCoord, const ::gpk::bgra & color)> & funcSetPixel
	) {
	const ::gpk::n2f32			imageUnit				= {textureImage.metrics().x - 0.000001f, textureImage.metrics().y - 0.000001f};
	double						lightFactorDirectional	= normal.Dot(lightVector);
	(void)lightFactorDirectional;
	int32_t						countPixels				= 0;
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::gpk::n2i16				pixelCoord				= pixelCoords		[iPixelCoord];
		const ::gpk::trif32			& vertexWeights			= pixelVertexWeights[iPixelCoord];
		const ::gpk::n2f32			texCoord				= ::gpk::triangleWeight(vertexWeights, triangleTexCoords);
		const ::gpk::n3f32			position				= ::gpk::triangleWeight(vertexWeights, triangleWorld);
		const ::gpk::rgbaf			texelColor				= textureImage.size() ? textureImage[(uint32_t)(texCoord.y * imageUnit.y) % textureImage.metrics().y][(uint32_t)(texCoord.x * imageUnit.x) % textureImage.metrics().x] : ::gpk::bgra{::gpk::GRAY};
		::gpk::rgbaf				fragmentColor			= {};
		stacxpr	double				rangeLight				= 10.0;
		stacxpr	double				rangeLightSquared		= rangeLight * rangeLight;
		stacxpr	double				rangeUnit				= 1.0 / rangeLightSquared;
		for(uint32_t iLight = 0; iLight < lightPoints.size(); ++iLight) {
			const ::gpk::n3f32			lightToPoint			= lightPoints[iLight] - position;
			const ::gpk::n3f32			vectorToLight			= lightToPoint;
			const double				lightFactor				= vectorToLight.Dot(normal);
			const double				distanceToLight			= lightToPoint.LengthSquared();
			if(distanceToLight > rangeLightSquared)
				continue;
			fragmentColor			+= texelColor / (10.0 * lightPoints.size());
			if(lightFactor <= 0)
				continue;
			const double				invAttenuation			= ::std::max(0.0, 1.0 - (distanceToLight * rangeUnit));
			fragmentColor			+= (texelColor * lightColors[iLight]) * (invAttenuation);
		}
		countPixels += funcSetPixel(targetPixels, pixelCoord, (texelColor * .2 + (texelColor * lightFactorDirectional).Clamp() + fragmentColor.Clamp()).Clamp());
	}
	return countPixels;
}

::gpk::error_t			gpk::drawPixels
	( ::gpk::g8bgra	targetPixels
	, const ::gpk::tri3f32			& triangleWorld
	, const ::gpk::n3f32			& normal
	, const ::gpk::tri2f32			& triangleTexCoords
	, const ::gpk::n3f32			& lightVector
	, ::gpk::apod<::gpk::n2i16>		& pixelCoords
	, ::gpk::apod<::gpk::trif32>	& pixelVertexWeights
	, ::gpk::gc8bgra	textureImage
	, ::gpk::apod<::gpk::n3f32>		& lightPoints
	, ::gpk::a8bgra		& lightColors
	) {

	return gpk::drawPixels(targetPixels, triangleWorld, normal, triangleTexCoords, lightVector, pixelCoords, pixelVertexWeights, textureImage, lightPoints, lightColors, setPixel);
}

::gpk::error_t			gpk::drawQuadTriangle
	( const ::gpk::n2u32			& targetSize
	, const ::gpk::tri3f32		& triangleScreen
	, ::gpk::apod<::gpk::n2i16>		& pixelCoords
	, ::gpk::apod<::gpk::trif32>	& pixelVertexWeights
	, ::gpk::grid<uint32_t>		depthBuffer
	) {
	return ::gpk::drawTriangle(targetSize, triangleScreen, pixelCoords, pixelVertexWeights, depthBuffer);
}

::gpk::error_t			gpk::drawQuadTriangle
	( const ::gpk::n2u32			& targetSize
	, ::gpk::tri3f32				triangle
	, const ::gpk::m4f32			& matrixTransformVP
	, ::gpk::apod<::gpk::n2i16>		& pixelCoords
	, ::gpk::apod<::gpk::trif32>	& pixelVertexWeights
	, ::gpk::grid<uint32_t>		depthBuffer
	) {
	::gpk::transform(triangle, matrixTransformVP);
	if( triangle.A.z <= 0 || triangle.A.z >= 1
	 || triangle.B.z <= 0 || triangle.B.z >= 1
	 || triangle.C.z <= 0 || triangle.C.z >= 1
	)
		return 0;
	return ::gpk::drawQuadTriangle(targetSize, triangle, pixelCoords, pixelVertexWeights, depthBuffer);
}

::gpk::error_t			gpk::drawQuadTriangle
	( ::gpk::g8bgra	targetPixels
	, const ::gpk::SGeometryQuads	& geometry
	, const int						iTriangle
	, const ::gpk::m4f32			& matrixTransform
	, const ::gpk::m4f32			& matrixTransformVP
	, const ::gpk::n3f32			& lightVector
	, ::gpk::apod<::gpk::n2i16>		& pixelCoords
	, ::gpk::apod<::gpk::trif32>	& pixelVertexWeights
	, ::gpk::gc8bgra	textureImage
	, ::gpk::apod<::gpk::n3f32>		& lightPoints
	, ::gpk::a8bgra		& lightColors
	, ::gpk::grid<uint32_t>		depthBuffer
	) {
	return ::gpk::drawQuadTriangle(targetPixels, geometry, iTriangle, matrixTransform, matrixTransformVP, lightVector, pixelCoords, pixelVertexWeights, textureImage, lightPoints, lightColors, depthBuffer, setPixel);
}

::gpk::error_t			gpk::drawQuadTriangle
	( ::gpk::g8bgra	targetPixels
	, const ::gpk::SGeometryQuads	& geometry
	, const int						iTriangle
	, const ::gpk::m4f32			& matrixTransform
	, const ::gpk::m4f32			& matrixTransformVP
	, const ::gpk::n3f32			& lightVector
	, ::gpk::apod<::gpk::n2i16>		& pixelCoords
	, ::gpk::apod<::gpk::trif32>	& pixelVertexWeights
	, ::gpk::gc8bgra	textureImage
	, ::gpk::apod<::gpk::n3f32>		& lightPoints
	, ::gpk::a8bgra		& lightColors
	, ::gpk::grid<uint32_t>		depthBuffer
	, const ::gpk::function<::gpk::error_t(::gpk::g8bgra targetPixels, const ::gpk::n2i16 & pixelCoord, const ::gpk::bgra & color)> & funcSetPixel
	) {
	const ::gpk::tri3f32		& triangle			= geometry.Triangles	[iTriangle];;
	const ::gpk::tri2f32		& triangleTexCoords	= geometry.TextureCoords[iTriangle];
	const ::gpk::n3f32			& normal			= geometry.Normals		[iTriangle / 2];
	::gpk::drawQuadTriangle(targetPixels.metrics(), triangle, matrixTransformVP, pixelCoords, pixelVertexWeights, depthBuffer);
	const ::gpk::n3f32			xnormal				= matrixTransform.TransformDirection(normal).Normalize();
	::gpk::tri3f32				triangleWorld		= triangle;
	::gpk::transform(triangleWorld, matrixTransform);
	return ::gpk::drawPixels(targetPixels, triangleWorld, xnormal, triangleTexCoords, lightVector, pixelCoords, pixelVertexWeights, textureImage, lightPoints, lightColors, funcSetPixel);
}

::gpk::error_t			gpk::drawQuadTriangle
	( ::gpk::g8bgra	targetPixels
	, const ::gpk::SGeometryQuads	& geometry
	, const int						iTriangle
	, const ::gpk::m4f32			& matrixTransform
	, const ::gpk::m4f32			& matrixTransformView
	, const ::gpk::n3f32			& lightVector
	, const ::gpk::bgra				color
	, ::gpk::apod<::gpk::n2i16>		& pixelCoords
	, ::gpk::apod<::gpk::trif32>	& pixelVertexWeights
	, ::gpk::grid<uint32_t>		depthBuffer
	) {
	::gpk::tri3f32				triangle			= geometry.Triangles	[iTriangle];
	::gpk::n3f32				normal				= geometry.Normals		[iTriangle / 2];
	normal					= matrixTransform.TransformDirection(normal).Normalize();
	::gpk::transform(triangle, matrixTransformView);
	if( triangle.A.z <= 0 || triangle.A.z >= 1
	 || triangle.B.z <= 0 || triangle.B.z >= 1
	 || triangle.C.z <= 0 || triangle.C.z >= 1
	)
		return 0;

	double						lightFactor			= normal.Dot(lightVector);

	int32_t						countPixels			= 0;
	::gpk::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::gpk::n2i16				pixelCoord			= pixelCoords[iPixelCoord];
		countPixels += ::gpk::setPixel(targetPixels, pixelCoord, (color * 0.1) + color * lightFactor);
	}
	return countPixels;
}

::gpk::error_t			gpk::drawTriangle
	( ::gpk::g8bgra	targetPixels
	, const ::gpk::SGeometryTriangles	& geometry
	, const int						iTriangle
	, const ::gpk::m4f32			& matrixTransform
	, const ::gpk::m4f32			& matrixTransformView
	, const ::gpk::n3f32			& lightVector
	, const ::gpk::bgra				color
	, ::gpk::apod<::gpk::n2i16>		& pixelCoords
	, ::gpk::apod<::gpk::trif32>	& pixelVertexWeights
	, ::gpk::grid<uint32_t>		depthBuffer
	) {
	::gpk::tri3f32				triangle			= geometry.Triangles	[iTriangle];
	const ::gpk::tri3f32		& triangleNormals	= geometry.Normals		[iTriangle];
	::gpk::transform(triangle, matrixTransformView);
	if( triangle.A.z <= 0 || triangle.A.z >= 1
	 || triangle.B.z <= 0 || triangle.B.z >= 1
	 || triangle.C.z <= 0 || triangle.C.z >= 1
	)
		return 0;

	::gpk::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);
	int32_t						countPixels				= 0;
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		const ::gpk::n2i16			pixelCoord				= pixelCoords		[iPixelCoord];
		const ::gpk::trif32			& vertexWeights			= pixelVertexWeights[iPixelCoord];
		::gpk::n3f32				normal					= ::gpk::triangleWeight(vertexWeights, triangleNormals);
		normal					= matrixTransform.TransformDirection(normal).Normalize();
		double						lightFactor				= normal.Dot(lightVector);
		countPixels += ::gpk::setPixel(targetPixels, pixelCoord, color * lightFactor);
	}
	return countPixels;
}

::gpk::error_t			gpk::drawTriangle
	( const ::gpk::g8bgra	targetPixels
	, const ::gpk::SGeometryTriangles	& geometry
	, const int							iTriangle
	, const ::gpk::m4f32				& matrixTransform
	, const ::gpk::m4f32				& matrixTransformView
	, const ::gpk::n3f32				& lightVector
	, ::gpk::apod<::gpk::n2i16>			& pixelCoords
	, ::gpk::apod<::gpk::trif32>		& pixelVertexWeights
	, ::gpk::gc8bgra	textureImage
	, ::gpk::grid<uint32_t>			depthBuffer
	) {
	::gpk::tri3f32				triangle			= geometry.Triangles		[iTriangle];
	const ::gpk::tri3f32		& triangleNormals	= geometry.Normals			[iTriangle];
	const ::gpk::tri2f32		& triangleTexCoords	= geometry.TextureCoords	[iTriangle];
	::gpk::transform(triangle, matrixTransformView);
	if( triangle.A.z <= 0 || triangle.A.z >= 1
	 || triangle.B.z <= 0 || triangle.B.z >= 1
	 || triangle.C.z <= 0 || triangle.C.z >= 1
	)
		return 0;

	int32_t						countPixels				= 0;
	::gpk::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);
	const ::gpk::n2f32			imageUnit				= {textureImage.metrics().x - 0.000001f, textureImage.metrics().y - 0.000001f};
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		const ::gpk::n2i16			pixelCoord				= pixelCoords		[iPixelCoord];
		const ::gpk::trif32			& vertexWeights			= pixelVertexWeights[iPixelCoord];
		::gpk::n3f32				normal					= ::gpk::triangleWeight(vertexWeights, triangleNormals);
		normal					= matrixTransform.TransformDirection(normal).Normalize();

		double						lightFactor				= normal.Dot(lightVector);

		::gpk::n2f32				texCoord				= triangleTexCoords.A * vertexWeights.A;
		texCoord				+= triangleTexCoords.B * vertexWeights.B;
		texCoord				+= triangleTexCoords.C * vertexWeights.C;
		::gpk::bgra					texelColor				= textureImage[(uint32_t)(texCoord.y * imageUnit.y)][(uint32_t)(texCoord.x * imageUnit.x)];
		countPixels += ::gpk::setPixel(targetPixels, pixelCoord, (::gpk::rgbaf(texelColor) * .3 + ::gpk::rgbaf(texelColor) * lightFactor).Clamp());
	}
	return countPixels;
}

::gpk::error_t			gpk::drawTriangle
	( const ::gpk::g8bgra	targetPixels
	, const ::gpk::SGeometryTriangles	& geometry
	, const int							iTriangle
	, const ::gpk::m4f32				& matrixTransform
	, const ::gpk::m4f32				& matrixTransformView
	, const ::gpk::n3f32				& lightVector
	, const ::gpk::rgbaf				& lightColor
	, ::gpk::apod<::gpk::n2i16>			& pixelCoords
	, ::gpk::apod<::gpk::trif32>		& pixelVertexWeights
	, ::gpk::gc8bgra	textureImage
	, ::gpk::apod<::gpk::SLight3>		& lightPoints
	, ::gpk::a8bgra			& lightColors
	, ::gpk::grid<uint32_t>			depthBuffer
	) {
	::gpk::tri3f32				triangleWorld			= geometry.Triangles		[iTriangle];
	::gpk::tri3f32				triangle				= triangleWorld;
	::gpk::transform(triangle, matrixTransformView);
	if( triangle.A.z < 0 || triangle.A.z >= 1
	 || triangle.B.z < 0 || triangle.B.z >= 1
	 || triangle.C.z < 0 || triangle.C.z >= 1
	)
		return 0;

	int32_t						countPixels				= 0;
	::gpk::transform(triangleWorld, matrixTransform);
	::gpk::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);

	const ::gpk::tri3f32		& triangleNormals		= geometry.Normals			[iTriangle];
	const ::gpk::tri2f32		& triangleTexCoords		= geometry.TextureCoords	[iTriangle];
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
			fragmentColor			+= ::gpk::rgbaf{texelColor * lightColors[iLight] * invAttenuation * .5}.Clamp();
		}
		 (void)lightColor;
		 (void)lightFactorDirectional;

		countPixels				+= ::gpk::setPixel(targetPixels, pixelCoord, ((texelColor * .1) + texelColor * lightColor * lightFactorDirectional + fragmentColor).Clamp());
	}
	return countPixels;
}

#endif