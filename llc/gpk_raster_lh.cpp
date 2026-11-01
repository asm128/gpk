#include "gpk_raster_lh.h"

#ifndef GPK_ATMEL

GPK_USING_TYPEINT();
GPK_USING_VIEW();
GPK_USING_APOD();


gpk::err_t 		gpk::pixelBlend			(::gpk::g8bgra pixels, ::gpk::n2s1_t position, ::gpk::bgra color)	{
	if( (position.x >= 0 && position.x < (s2_t)pixels.metrics().x)
	 && (position.y >= 0 && position.y < (s2_t)pixels.metrics().y)
	) {
		::gpk::bgra					& targetPixel	= pixels[position.y][position.x];
		::gpk::rgbaf				srcPixel		= targetPixel;
		targetPixel				= ::gpk::interpolate_linear(srcPixel, ::gpk::rgbaf(color), color.a / 255.0f);
		return 1;
	}
	return 0;
}

gpk::err_t 		gpk::setPixel			(::gpk::g8bgra pixels, ::gpk::n2s1_t position, ::gpk::bgra color)	{
	if( (position.x >= 0 && position.x < (s2_t)pixels.metrics().x)
	 && (position.y >= 0 && position.y < (s2_t)pixels.metrics().y)
	) {
		::gpk::bgra					& targetPixel = pixels[position.y][position.x];
		targetPixel				= {color.b, color.g, color.r, (u0_t)::gpk::clamped(targetPixel.a + (u2_t)color.a, 0U, 255U)};
		return 1;
	}
	return 0;
}

gpk::err_t 		gpk::drawRectangle		(::gpk::g8bgra pixels, ::gpk::rect2<s1_t> rectangle, ::gpk::bgra color)	{
	s2_t						countPixels				= 0;
	for(s1_t y = 0; y < (s1_t)rectangle.Size.y; ++y)
	for(s1_t x = 0; x < (s1_t)rectangle.Size.x; ++x)
		countPixels += ::gpk::setPixel(pixels, {(s1_t)(rectangle.Offset.x + x), (s1_t)(rectangle.Offset.y + y)}, color);
	return countPixels;
}

gpk::err_t 		gpk::drawCircle			(::gpk::g8bgra pixels, ::gpk::circle<s1_t> circle, ::gpk::bgra color)	{
	s2_t						countPixels				= 0;
	for(s1_t y = (s1_t)-circle.Radius; y < (s1_t)circle.Radius; ++y)
	for(s1_t x = (s1_t)-circle.Radius; x < (s1_t)circle.Radius; ++x) {
		::gpk::n2s1_t				position			= {x, y};
		if(position.Length() <= circle.Radius)
			countPixels += ::gpk::setPixel(pixels, {s1_t(circle.Center.x + x), s1_t(circle.Center.y + y)}, color);
	}
	return countPixels;
}

// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
gpk::err_t 		gpk::drawLine       	(::gpk::g8bgra pixels, ::gpk::line2s1_t line, ::gpk::bgra color)	{
	s2_t						countPixels				= 0;
	s2_t						dx						= (s2_t)fabs(line.B.x - line.A.x);
	s2_t						sx						= (s2_t)line.A.x < line.B.x ? 1 : -1;
	s2_t						dy						= (s2_t)-fabs(line.B.y - line.A.y);
	s2_t						sy						= (s2_t)line.A.y < line.B.y ? 1 : -1;
	s2_t						err						= dx + dy;  /* error value e_xy */
	while (true) {   /* loop */
		if (line.A.x == line.B.x && line.A.y == line.B.y)
			break;
		s2_t						e2						= 2 * err;
		if (e2 >= dy) {
			err						+= dy; /* e_xy+e_x > 0 */
			line.A.x				+= (s1_t)sx;
			countPixels += setPixel(pixels, {line.A.x, line.A.y}, color);
		}
		if (e2 <= dx) { /* e_xy+e_y < 0 */
			err						+= dx;
			line.A.y				+= (s1_t)sy;
			countPixels += setPixel(pixels, {line.A.x, line.A.y}, color);
		}

	}
	return countPixels;
}


// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
gpk::err_t 		gpk::drawLine       	(const ::gpk::n2u1_t offscreenMetrics, ::gpk::line2s1_t line, ::gpk::apod<::gpk::n2s1_t> & pixelCoords)	{
	s2_t						dx						= (s2_t)fabs(line.B.x - line.A.x);
	s2_t						sx						= (s2_t)line.A.x < line.B.x ? 1 : -1;
	s2_t						dy						= (s2_t)-fabs(line.B.y - line.A.y );
	s2_t						sy						= (s2_t)line.A.y < line.B.y ? 1 : -1;
	s2_t						err						= dx + dy;  /* error value e_xy */
	pixelCoords.push_back({line.A.x, line.A.y});
	while (true) {   /* loop */
		if (line.A.x == line.B.x && line.A.y == line.B.y)
			break;
		s2_t						e2						= 2 * err;
		if (e2 >= dy) {
			err						+= dy; /* e_xy+e_x > 0 */
			line.A.x				+= (s1_t)sx;
			if( line.A.x >= 0 && line.A.x < (s2_t)offscreenMetrics.x
			 && line.A.y >= 0 && line.A.y < (s2_t)offscreenMetrics.y
			)
				pixelCoords.push_back({line.A.x, line.A.y});

		}
		if (e2 <= dx) { /* e_xy+e_y < 0 */
			err						+= dx;
			line.A.y				+= (s1_t)sy;
			if( line.A.x >= 0 && line.A.x < (s2_t)offscreenMetrics.x
			 && line.A.y >= 0 && line.A.y < (s2_t)offscreenMetrics.y
			)
				pixelCoords.push_back({line.A.x, line.A.y});
		}

	}
	return 0;
}


// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
gpk::err_t 		gpk::drawLine
	( const ::gpk::n2u1_t		offscreenMetrics
	, const ::gpk::line3f2_t		& lineFloat
	, ::gpk::apod<::gpk::n3f2_t>	& pixelCoords
	, ::gpk::grid<u2_t>	depthBuffer
	) {
	::gpk::line2i32				line					= {{(s2_t)lineFloat.A.x, (s2_t)lineFloat.A.y}, {(s2_t)lineFloat.B.x, (s2_t)lineFloat.B.y}};
	s2_t						xDiff					= (s2_t)fabs(line.B.x - line.A.x);
	s2_t						yDiff					= (s2_t)-fabs(line.B.y - line.A.y);
	s2_t						sx						= (s2_t)line.A.x < line.B.x ? 1 : -1;
	s2_t						sy						= (s2_t)line.A.y < line.B.y ? 1 : -1;
	s2_t						err						= xDiff + yDiff;  /* error value e_xy */

	bool						yAxis					= fabs(yDiff) > xDiff;
	::gpk::vu2_t					depthBufferRow			= {};
	if( line.A.x >= 0 && line.A.x < (s2_t)offscreenMetrics.x
	 && line.A.y >= 0 && line.A.y < (s2_t)offscreenMetrics.y
	 && lineFloat.A.z >= 0 && lineFloat.A.z <= 1
	) {
		depthBufferRow			= depthBuffer[line.A.y];
		u2_t					& depthCell				= depthBufferRow[line.A.x];
		u2_t					intZ					= u2_t((f3_t)0xFFFFFFFU * lineFloat.A.z);
		if( depthCell > intZ ) {
			depthCell				= intZ;
			//pixelCoords.push_back({(s2_t)line.A.x, (s2_t)line.A.y});
			pixelCoords.push_back(lineFloat.A);
		}
	}

	//bool								outside				= true;
	const f3_t				factorUnit				= 1.0 / (yAxis ? fabs(yDiff) : xDiff);
	while (true) {   /* loop */
		if (line.A.x == line.B.x && line.A.y == line.B.y)
			break;
		s2_t						e2						= 2 * err;
		if (e2 >= yDiff) {
			err						+= yDiff; /* e_xy+e_x > 0 */
			line.A.x				+= sx;
			if( line.A.x >= 0 && line.A.x < (s2_t)offscreenMetrics.x
			 && line.A.y >= 0 && line.A.y < (s2_t)offscreenMetrics.y
			) {
				depthBufferRow			= depthBuffer[line.A.y];
				const f3_t				factor					= 1.0 / (yAxis ? factorUnit * line.A.y : factorUnit * line.A.x);
				const f3_t				finalZ					= ::gpk::interpolate_linear(lineFloat.A.z, lineFloat.B.z, factor);// lineFloat.B.z * factor + (lineFloat.A.z * (1.0 - factor));
				if (finalZ <= 0 || finalZ >= 1)
					continue;
				const u2_t				intZ					= u2_t(0xFFFFFFFU * finalZ);
				u2_t					& depthCell				= depthBufferRow[line.A.x];
				if(depthCell <= intZ)
					continue;

				depthCell				= intZ;
				pixelCoords.push_back({(f2_t)line.A.x, (f2_t)line.A.y, (f2_t)finalZ});
			}
			//else
			//	outside							= true;
		}
		if (e2 <= xDiff) { /* e_xy+e_y < 0 */
			err						+= xDiff;
			line.A.y				+= sy;
			if( line.A.x >= 0 && line.A.x < (s2_t)offscreenMetrics.x
			 && line.A.y >= 0 && line.A.y < (s2_t)offscreenMetrics.y
			) {
				depthBufferRow					= depthBuffer[line.A.y];
				const f3_t				factor					= 1.0 / (yAxis ? factorUnit * line.A.y : factorUnit * line.A.x);
				const f3_t				finalZ					= ::gpk::interpolate_linear(lineFloat.A.z, lineFloat.B.z, factor);// lineFloat.B.z * factor + (lineFloat.A.z * (1.0 - factor));
				if (finalZ <= 0 || finalZ >= 1)
					continue;
				const u2_t				intZ					= u2_t(0xFFFFFFFU * finalZ);
				u2_t					& depthCell				= depthBufferRow[line.A.x];
				if(depthCell <= intZ)
					continue;

				depthCell				= intZ;
				pixelCoords.push_back({(f2_t)line.A.x, (f2_t)line.A.y, (f2_t)finalZ});
			}
			//else
			//	outside					= true;
		}

	}
	return 0;
}

//https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/
static	f3_t		orient2d		(const ::gpk::line2s1_t & segment, const ::gpk::n2s1_t & point)	{ return (segment.B.x - segment.A.x) * (point.y - (f3_t)segment.A.y) - (segment.B.y - segment.A.y) * (point.x - (f3_t)segment.A.x); }
//static	f3_t		orient2d		(const ::gpk::line3i16 & segment, const ::gpk::n2s1_t & point)	{ return (segment.B.x - segment.A.x) * (point.y - (f3_t)segment.A.y) - (segment.B.y - segment.A.y) * (point.x - (f3_t)segment.A.x); }
//static	f3_t		orient2d		(const ::gpk::line3f2_t & segment, const ::gpk::n2s1_t & point)	{ return (segment.B.x - segment.A.x) * (point.y - (f3_t)segment.A.y) - (segment.B.y - segment.A.y) * (point.x - (f3_t)segment.A.x); }
static	f3_t		orient2d		(const ::gpk::line2f2_t & segment, const ::gpk::n2s1_t & point)	{ return (segment.B.x - segment.A.x) * (point.y - (f3_t)segment.A.y) - (segment.B.y - segment.A.y) * (point.x - (f3_t)segment.A.x); }
static	f3_t		orient2d		(const ::gpk::n3f2_t & start, const ::gpk::n2f2_t & edge, const ::gpk::n2s1_t & point)	{ return edge.x * (point.y - (f3_t)start.y) - edge.y * (point.x - (f3_t)start.x); }

tplt <tpnm _tValue>	_tValue	max3		(_tValue & a, _tValue & b, _tValue & c)			{ return ::std::max(::std::max(a, b), c); }
tplt <tpnm _tValue>	_tValue	min3		(_tValue & a, _tValue & b, _tValue & c)			{ return ::std::min(::std::min(a, b), c); }

gpk::err_t 		gpk::drawTriangle		(::gpk::g8bgra pixels, const ::gpk::tri2<s1_t> & triangle, ::gpk::bgra color){
	// Compute triangle bounding box
	s1_t						minX					= ::min3(triangle.A.x, triangle.B.x, triangle.C.x);
	s1_t						minY					= ::min3(triangle.A.y, triangle.B.y, triangle.C.y);
	s1_t						maxX					= ::max3(triangle.A.x, triangle.B.x, triangle.C.x);
	s1_t						maxY					= ::max3(triangle.A.y, triangle.B.y, triangle.C.y);

	// Clip against screen bounds
	minX							= ::std::max(minX, (s1_t)0);
	minY							= ::std::max(minY, (s1_t)0);
	maxX							= ::std::min(maxX, s1_t(s1_t(pixels.metrics().x)	- 1));
	maxY							= ::std::min(maxY, s1_t(s1_t(pixels.metrics().y)	- 1));

	// Rasterize
	s2_t						countPixels				= 0;
	::gpk::n2s1_t				p;
	for (p.y = minY; p.y <= maxY; ++p.y) 
	for (p.x = minX; p.x <= maxX; ++p.x) {
		// Determine barycentric coordinates
		f3_t						w0						= ::orient2d({triangle.B, triangle.C}, p);
		f3_t						w1						= ::orient2d({triangle.C, triangle.A}, p);
		f3_t						w2						= ::orient2d({triangle.A, triangle.B}, p);
		// If p is on or inside all edges, render pixel.
		if (w0 < 0 || w1 < 0 || w2 < 0)
			continue;
		countPixels += ::gpk::setPixel(pixels, p, color);
	}
	return countPixels;
}

gpk::err_t 		gpk::drawTriangle
	( const ::gpk::n2u2_t			targetSize
	, const ::gpk::tri3f2_t			& triangle
	, ::gpk::apod<::gpk::n2s1_t>		& pixelCoords
	, ::gpk::apod<::gpk::trif2_t>	& proportions
	, ::gpk::grid<u2_t>		depthBuffer
	)	{
	// Compute triangle bounding box
	s1_t						minX					= (s1_t)::min3(triangle.A.x, triangle.B.x, triangle.C.x);
	s1_t						minY					= (s1_t)::min3(triangle.A.y, triangle.B.y, triangle.C.y);
	s1_t						maxX					= (s1_t)::max3(triangle.A.x, triangle.B.x, triangle.C.x);
	s1_t						maxY					= (s1_t)::max3(triangle.A.y, triangle.B.y, triangle.C.y);

	// Clip against screen bounds
	minX					= ::std::max(minX, (s1_t)0);
	minY					= ::std::max(minY, (s1_t)0);
	maxX					= ::std::min(maxX, (s1_t)((s2_t)targetSize.x - 1));
	maxY					= ::std::min(maxY, (s1_t)((s2_t)targetSize.y - 1));

	// Rasterize
	const ::gpk::n2f2_t			edgeBC					= {triangle.C.x - triangle.B.x, triangle.C.y - triangle.B.y};
	const ::gpk::n2f2_t			edgeCA					= {triangle.A.x - triangle.C.x, triangle.A.y - triangle.C.y};
	const ::gpk::n2f2_t			edgeAB					= {triangle.B.x - triangle.A.x, triangle.B.y - triangle.A.y};
	::gpk::n2s1_t				p;
	for (p.y = minY; p.y <= maxY; ++p.y)
	for (p.x = minX; p.x <= maxX; ++p.x) {
		// Determine barycentric coordinates
		f3_t						w0						= ::orient2d(triangle.B, edgeBC, p);
		f3_t						w1						= ::orient2d(triangle.C, edgeCA, p);
		f3_t						w2						= ::orient2d(triangle.A, edgeAB, p);
		// If p is on or inside all edges, render pixel.
		if (w0 < 0 || w1 < 0 || w2 < 0)
			continue;

		f3_t						proportionABC			= w0 + w1 + w2;
		if(proportionABC == 0)
			continue;
		f3_t						proportionA				= w0 / proportionABC;
		f3_t						proportionB				= w1 / proportionABC;
		f3_t						proportionC				= 1.0 - (proportionA + proportionB);

		// Calculate interpolated depth
		f3_t						finalZ					= triangle.A.z * proportionA;
		finalZ					+= triangle.B.z * proportionB;
		finalZ					+= triangle.C.z * proportionC;
		if(finalZ >= 1.0 || finalZ <= 0)
			continue;

		u2_t					intZ					= u2_t(0xFFFFFFFU * finalZ);
		u2_t					& currentDepth			= depthBuffer[p.y][p.x];
		if(currentDepth <= intZ)
			continue;

		currentDepth			= intZ;
		pixelCoords.push_back(p.s1());
		proportions.push_back({(f2_t)proportionA, (f2_t)proportionB, (f2_t)proportionC});

	}
	return 0;
}

gpk::err_t 		gpk::drawTriangle
	( const ::gpk::n2u2_t			targetSize
	, const ::gpk::tri2f2_t			& triangle
	, ::gpk::apod<::gpk::n2s1_t>		& pixelCoords
	, ::gpk::apod<::gpk::trif2_t>	& proportions
	)	{
	// Compute triangle bounding box
	s1_t						minX					= (s1_t)::min3(triangle.A.x, triangle.B.x, triangle.C.x);
	s1_t						minY					= (s1_t)::min3(triangle.A.y, triangle.B.y, triangle.C.y);
	s1_t						maxX					= (s1_t)::max3(triangle.A.x, triangle.B.x, triangle.C.x);
	s1_t						maxY					= (s1_t)::max3(triangle.A.y, triangle.B.y, triangle.C.y);

	// Clip against screen bounds
	minX					= ::std::max(minX, (s1_t)0);
	minY					= ::std::max(minY, (s1_t)0);
	maxX					= ::std::min(maxX, (s1_t)((s2_t)targetSize.x - 1));
	maxY					= ::std::min(maxY, (s1_t)((s2_t)targetSize.y - 1));

	// Rasterize
	::gpk::n2s1_t p;
	for (p.y = minY; p.y <= maxY; ++p.y)
	for (p.x = minX; p.x <= maxX; ++p.x) {
		// Determine barycentric coordinates
		f3_t						w0						= ::orient2d({triangle.B, triangle.C}, p);
		f3_t						w1						= ::orient2d({triangle.C, triangle.A}, p);
		f3_t						w2						= ::orient2d({triangle.A, triangle.B}, p);
		// If p is on or inside all edges, render pixel.
		if (w0 < 0 || w1 < 0 || w2 < 0)
			continue;

		f3_t						proportionABC			= w0 + w1 + w2;
		if(proportionABC == 0)
			continue;
		f3_t						proportionA				= w0 / proportionABC;
		f3_t						proportionB				= w1 / proportionABC;
		f3_t						proportionC				= 1.0 - (proportionA + proportionB);

		pixelCoords.push_back(p.s1());
		proportions.push_back({(f2_t)proportionA, (f2_t)proportionB, (f2_t)proportionC});

	}
	return 0;
}

gpk::err_t 		gpk::drawPixels
	( ::gpk::g8bgra		targetPixels
	, const ::gpk::tri3f2_t				& triangleWorld
	, const ::gpk::n3f2_t				& normal
	, const ::gpk::tri2f2_t				& triangleTexCoords
	, const ::gpk::n3f2_t				& lightVector
	, ::gpk::apod<::gpk::n2s1_t>			& pixelCoords
	, ::gpk::apod<::gpk::trif2_t>		& pixelVertexWeights
	, ::gpk::gc8bgra					textureImage
	, ::gpk::apod<::gpk::n3f2_t>			& lightPoints
	, ::gpk::a8bgra			& lightColors
	, const ::gpk::function<::gpk::err_t(::gpk::g8bgra targetPixels, const ::gpk::n2s1_t & pixelCoord, const ::gpk::bgra & color)> & funcSetPixel
	) {
	const ::gpk::n2f2_t			imageUnit				= {textureImage.metrics().x - 0.000001f, textureImage.metrics().y - 0.000001f};
	f3_t						lightFactorDirectional	= normal.Dot(lightVector);
	(void)lightFactorDirectional;
	s2_t						countPixels				= 0;
	for(u2_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::gpk::n2s1_t				pixelCoord				= pixelCoords		[iPixelCoord];
		const ::gpk::trif2_t			& vertexWeights			= pixelVertexWeights[iPixelCoord];
		const ::gpk::n2f2_t			texCoord				= ::gpk::triangleWeight(vertexWeights, triangleTexCoords);
		const ::gpk::n3f2_t			position				= ::gpk::triangleWeight(vertexWeights, triangleWorld);
		const ::gpk::rgbaf			texelColor				= textureImage.size() ? textureImage[(u2_t)(texCoord.y * imageUnit.y) % textureImage.metrics().y][(u2_t)(texCoord.x * imageUnit.x) % textureImage.metrics().x] : ::gpk::bgra{::gpk::GRAY};
		const u2_t				countLights				= lightPoints.size();
		const ::gpk::rgbaf			ambientPerLight			= countLights ? texelColor / (10.0 * countLights) : ::gpk::rgbaf{};
		::gpk::rgbaf				fragmentColor			= {};
		stxp	f3_t				rangeLight				= 10.0;
		stxp	f3_t				rangeLightSquared		= rangeLight * rangeLight;
		stxp	f3_t				rangeUnit				= 1.0 / rangeLightSquared;
		for(u2_t iLight = 0; iLight < countLights; ++iLight) {
			const ::gpk::n3f2_t			lightToPoint			= lightPoints[iLight] - position;
			const f3_t				distanceToLight			= lightToPoint.LengthSquared();
			if(distanceToLight > rangeLightSquared)
				continue;
			fragmentColor			+= ambientPerLight;
			const f3_t				lightFactor				= lightToPoint.Dot(normal);
			if(lightFactor <= 0)
				continue;
			const f3_t				invAttenuation			= ::std::max(0.0, 1.0 - (distanceToLight * rangeUnit));
			fragmentColor			+= (texelColor * lightColors[iLight]) * (invAttenuation);
		}
		countPixels += funcSetPixel(targetPixels, pixelCoord, (texelColor * .2 + (texelColor * lightFactorDirectional).Clamp() + fragmentColor.Clamp()).Clamp());
	}
	return countPixels;
}

gpk::err_t 		gpk::drawPixels
	( ::gpk::g8bgra	targetPixels
	, const ::gpk::tri3f2_t			& triangleWorld
	, const ::gpk::n3f2_t			& normal
	, const ::gpk::tri2f2_t			& triangleTexCoords
	, const ::gpk::n3f2_t			& lightVector
	, ::gpk::apod<::gpk::n2s1_t>		& pixelCoords
	, ::gpk::apod<::gpk::trif2_t>	& pixelVertexWeights
	, ::gpk::gc8bgra	textureImage
	, ::gpk::apod<::gpk::n3f2_t>		& lightPoints
	, ::gpk::a8bgra		& lightColors
	) {

	return gpk::drawPixels(targetPixels, triangleWorld, normal, triangleTexCoords, lightVector, pixelCoords, pixelVertexWeights, textureImage, lightPoints, lightColors, setPixel);
}

gpk::err_t 		gpk::drawQuadTriangle
	( const ::gpk::n2u2_t			& targetSize
	, const ::gpk::tri3f2_t		& triangleScreen
	, ::gpk::apod<::gpk::n2s1_t>		& pixelCoords
	, ::gpk::apod<::gpk::trif2_t>	& pixelVertexWeights
	, ::gpk::grid<u2_t>		depthBuffer
	) {
	return ::gpk::drawTriangle(targetSize, triangleScreen, pixelCoords, pixelVertexWeights, depthBuffer);
}

gpk::err_t 		gpk::drawQuadTriangle
	( const ::gpk::n2u2_t			& targetSize
	, ::gpk::tri3f2_t				triangle
	, const ::gpk::m4f2_t			& matrixTransformVP
	, ::gpk::apod<::gpk::n2s1_t>		& pixelCoords
	, ::gpk::apod<::gpk::trif2_t>	& pixelVertexWeights
	, ::gpk::grid<u2_t>		depthBuffer
	) {
	::gpk::transform(triangle, matrixTransformVP);
	if( triangle.A.z <= 0 || triangle.A.z >= 1
	 || triangle.B.z <= 0 || triangle.B.z >= 1
	 || triangle.C.z <= 0 || triangle.C.z >= 1
	)
		return 0;
	return ::gpk::drawQuadTriangle(targetSize, triangle, pixelCoords, pixelVertexWeights, depthBuffer);
}

gpk::err_t 		gpk::drawQuadTriangle
	( ::gpk::g8bgra	targetPixels
	, const ::gpk::SGeometryQuads	& geometry
	, const int						iTriangle
	, const ::gpk::m4f2_t			& matrixTransform
	, const ::gpk::m4f2_t			& matrixTransformVP
	, const ::gpk::n3f2_t			& lightVector
	, ::gpk::apod<::gpk::n2s1_t>		& pixelCoords
	, ::gpk::apod<::gpk::trif2_t>	& pixelVertexWeights
	, ::gpk::gc8bgra	textureImage
	, ::gpk::apod<::gpk::n3f2_t>		& lightPoints
	, ::gpk::a8bgra		& lightColors
	, ::gpk::grid<u2_t>		depthBuffer
	) {
	return ::gpk::drawQuadTriangle(targetPixels, geometry, iTriangle, matrixTransform, matrixTransformVP, lightVector, pixelCoords, pixelVertexWeights, textureImage, lightPoints, lightColors, depthBuffer, setPixel);
}

gpk::err_t 		gpk::drawQuadTriangle
	( ::gpk::g8bgra	targetPixels
	, const ::gpk::SGeometryQuads	& geometry
	, const int						iTriangle
	, const ::gpk::m4f2_t			& matrixTransform
	, const ::gpk::m4f2_t			& matrixTransformVP
	, const ::gpk::n3f2_t			& lightVector
	, ::gpk::apod<::gpk::n2s1_t>		& pixelCoords
	, ::gpk::apod<::gpk::trif2_t>	& pixelVertexWeights
	, ::gpk::gc8bgra	textureImage
	, ::gpk::apod<::gpk::n3f2_t>		& lightPoints
	, ::gpk::a8bgra		& lightColors
	, ::gpk::grid<u2_t>		depthBuffer
	, const ::gpk::function<::gpk::err_t(::gpk::g8bgra targetPixels, const ::gpk::n2s1_t & pixelCoord, const ::gpk::bgra & color)> & funcSetPixel
	) {
	const ::gpk::tri3f2_t		& triangle			= geometry.Triangles	[iTriangle];;
	const ::gpk::tri2f2_t		& triangleTexCoords	= geometry.TextureCoords[iTriangle];
	const ::gpk::n3f2_t			& normal			= geometry.Normals		[iTriangle / 2];
	::gpk::drawQuadTriangle(targetPixels.metrics(), triangle, matrixTransformVP, pixelCoords, pixelVertexWeights, depthBuffer);
	const ::gpk::n3f2_t			xnormal				= matrixTransform.TransformDirection(normal).Normalize();
	::gpk::tri3f2_t				triangleWorld		= triangle;
	::gpk::transform(triangleWorld, matrixTransform);
	return ::gpk::drawPixels(targetPixels, triangleWorld, xnormal, triangleTexCoords, lightVector, pixelCoords, pixelVertexWeights, textureImage, lightPoints, lightColors, funcSetPixel);
}

gpk::err_t 		gpk::drawQuadTriangle
	( ::gpk::g8bgra	targetPixels
	, const ::gpk::SGeometryQuads	& geometry
	, const int						iTriangle
	, const ::gpk::m4f2_t			& matrixTransform
	, const ::gpk::m4f2_t			& matrixTransformView
	, const ::gpk::n3f2_t			& lightVector
	, const ::gpk::bgra				color
	, ::gpk::apod<::gpk::n2s1_t>		& pixelCoords
	, ::gpk::apod<::gpk::trif2_t>	& pixelVertexWeights
	, ::gpk::grid<u2_t>		depthBuffer
	) {
	::gpk::tri3f2_t				triangle			= geometry.Triangles	[iTriangle];
	::gpk::n3f2_t				normal				= geometry.Normals		[iTriangle / 2];
	normal					= matrixTransform.TransformDirection(normal).Normalize();
	::gpk::transform(triangle, matrixTransformView);
	if( triangle.A.z <= 0 || triangle.A.z >= 1
	 || triangle.B.z <= 0 || triangle.B.z >= 1
	 || triangle.C.z <= 0 || triangle.C.z >= 1
	)
		return 0;

	f3_t						lightFactor			= normal.Dot(lightVector);

	s2_t						countPixels			= 0;
	::gpk::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);
	for(u2_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::gpk::n2s1_t				pixelCoord			= pixelCoords[iPixelCoord];
		countPixels += ::gpk::setPixel(targetPixels, pixelCoord, (color * 0.1) + color * lightFactor);
	}
	return countPixels;
}

gpk::err_t 		gpk::drawTriangle
	( ::gpk::g8bgra	targetPixels
	, const ::gpk::SGeometryTriangles	& geometry
	, const int						iTriangle
	, const ::gpk::m4f2_t			& matrixTransform
	, const ::gpk::m4f2_t			& matrixTransformView
	, const ::gpk::n3f2_t			& lightVector
	, const ::gpk::bgra				color
	, ::gpk::apod<::gpk::n2s1_t>		& pixelCoords
	, ::gpk::apod<::gpk::trif2_t>	& pixelVertexWeights
	, ::gpk::grid<u2_t>		depthBuffer
	) {
	::gpk::tri3f2_t				triangle			= geometry.Triangles	[iTriangle];
	const ::gpk::tri3f2_t		& triangleNormals	= geometry.Normals		[iTriangle];
	::gpk::transform(triangle, matrixTransformView);
	if( triangle.A.z <= 0 || triangle.A.z >= 1
	 || triangle.B.z <= 0 || triangle.B.z >= 1
	 || triangle.C.z <= 0 || triangle.C.z >= 1
	)
		return 0;

	::gpk::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);
	s2_t						countPixels				= 0;
	for(u2_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		const ::gpk::n2s1_t			pixelCoord				= pixelCoords		[iPixelCoord];
		const ::gpk::trif2_t			& vertexWeights			= pixelVertexWeights[iPixelCoord];
		::gpk::n3f2_t				normal					= ::gpk::triangleWeight(vertexWeights, triangleNormals);
		normal					= matrixTransform.TransformDirection(normal).Normalize();
		f3_t						lightFactor				= normal.Dot(lightVector);
		countPixels += ::gpk::setPixel(targetPixels, pixelCoord, color * lightFactor);
	}
	return countPixels;
}

gpk::err_t 		gpk::drawTriangle
	( const ::gpk::g8bgra	targetPixels
	, const ::gpk::SGeometryTriangles	& geometry
	, const int							iTriangle
	, const ::gpk::m4f2_t				& matrixTransform
	, const ::gpk::m4f2_t				& matrixTransformView
	, const ::gpk::n3f2_t				& lightVector
	, ::gpk::apod<::gpk::n2s1_t>			& pixelCoords
	, ::gpk::apod<::gpk::trif2_t>		& pixelVertexWeights
	, ::gpk::gc8bgra	textureImage
	, ::gpk::grid<u2_t>			depthBuffer
	) {
	::gpk::tri3f2_t				triangle			= geometry.Triangles		[iTriangle];
	const ::gpk::tri3f2_t		& triangleNormals	= geometry.Normals			[iTriangle];
	const ::gpk::tri2f2_t		& triangleTexCoords	= geometry.TextureCoords	[iTriangle];
	::gpk::transform(triangle, matrixTransformView);
	if( triangle.A.z <= 0 || triangle.A.z >= 1
	 || triangle.B.z <= 0 || triangle.B.z >= 1
	 || triangle.C.z <= 0 || triangle.C.z >= 1
	)
		return 0;

	s2_t						countPixels				= 0;
	::gpk::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);
	const ::gpk::n2f2_t			imageUnit				= {textureImage.metrics().x - 0.000001f, textureImage.metrics().y - 0.000001f};
	for(u2_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		const ::gpk::n2s1_t			pixelCoord				= pixelCoords		[iPixelCoord];
		const ::gpk::trif2_t			& vertexWeights			= pixelVertexWeights[iPixelCoord];
		::gpk::n3f2_t				normal					= ::gpk::triangleWeight(vertexWeights, triangleNormals);
		normal					= matrixTransform.TransformDirection(normal).Normalize();

		f3_t						lightFactor				= normal.Dot(lightVector);

		::gpk::n2f2_t				texCoord				= triangleTexCoords.A * vertexWeights.A;
		texCoord				+= triangleTexCoords.B * vertexWeights.B;
		texCoord				+= triangleTexCoords.C * vertexWeights.C;
		::gpk::bgra					texelColor				= textureImage[(u2_t)(texCoord.y * imageUnit.y)][(u2_t)(texCoord.x * imageUnit.x)];
		countPixels += ::gpk::setPixel(targetPixels, pixelCoord, (::gpk::rgbaf(texelColor) * .3 + ::gpk::rgbaf(texelColor) * lightFactor).Clamp());
	}
	return countPixels;
}

gpk::err_t 		gpk::drawTriangle
	( const ::gpk::g8bgra	targetPixels
	, const ::gpk::SGeometryTriangles	& geometry
	, const int							iTriangle
	, const ::gpk::m4f2_t				& matrixTransform
	, const ::gpk::m4f2_t				& matrixTransformView
	, const ::gpk::n3f2_t				& lightVector
	, const ::gpk::rgbaf				& lightColor
	, ::gpk::apod<::gpk::n2s1_t>			& pixelCoords
	, ::gpk::apod<::gpk::trif2_t>		& pixelVertexWeights
	, ::gpk::gc8bgra	textureImage
	, ::gpk::apod<::gpk::SLight3>		& lightPoints
	, ::gpk::a8bgra			& lightColors
	, ::gpk::grid<u2_t>			depthBuffer
	) {
	::gpk::tri3f2_t				triangleWorld			= geometry.Triangles		[iTriangle];
	::gpk::tri3f2_t				triangle				= triangleWorld;
	::gpk::transform(triangle, matrixTransformView);
	if( triangle.A.z < 0 || triangle.A.z >= 1
	 || triangle.B.z < 0 || triangle.B.z >= 1
	 || triangle.C.z < 0 || triangle.C.z >= 1
	)
		return 0;

	s2_t						countPixels				= 0;
	::gpk::transform(triangleWorld, matrixTransform);
	::gpk::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);

	const ::gpk::tri3f2_t		& triangleNormals		= geometry.Normals			[iTriangle];
	const ::gpk::tri2f2_t		& triangleTexCoords		= geometry.TextureCoords	[iTriangle];
	const ::gpk::n2f2_t			imageUnit				= {textureImage.metrics().x - 0.000001f, textureImage.metrics().y - 0.000001f};
	for(u2_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		const ::gpk::n2s1_t			pixelCoord				= pixelCoords		[iPixelCoord];
		const ::gpk::trif2_t			& vertexWeights			= pixelVertexWeights[iPixelCoord];

		::gpk::n3f2_t				normal					= ::gpk::triangleWeight(vertexWeights, triangleNormals);
		normal					= matrixTransform.TransformDirection(normal).Normalize();
		f3_t						lightFactorDirectional	= normal.Dot(lightVector);
		::gpk::n3f2_t				position				= ::gpk::triangleWeight(vertexWeights, triangleWorld);
		::gpk::n2f2_t				texCoord				= ::gpk::triangleWeight(vertexWeights, triangleTexCoords);
		::gpk::rgbaf				texelColor				= textureImage[(u2_t)(texCoord.y * imageUnit.y) % textureImage.metrics().y][(u2_t)(texCoord.x * imageUnit.x) % textureImage.metrics().x];
		::gpk::rgbaf				fragmentColor			= {};

		for(u2_t iLight = 0; iLight < lightPoints.size(); ++iLight) {
			const ::gpk::SLight3		& light					= lightPoints[iLight];

			::gpk::n3f2_t				lightToPoint			= light.Position - position;
			::gpk::n3f2_t				vectorToLight			= lightToPoint;
			vectorToLight.Normalize();
			f3_t						lightFactorPoint		= vectorToLight.Dot(normal);
			if(lightToPoint.Length() > light.Range || lightFactorPoint <= 0)
				continue;
			f3_t						invAttenuation			= ::std::max(0.0, 1.0 - (lightToPoint.Length() / light.Range));
			fragmentColor			+= ::gpk::rgbaf{texelColor * lightColors[iLight] * invAttenuation * .5}.Clamp();
		}
		 (void)lightColor;
		 (void)lightFactorDirectional;

		countPixels				+= ::gpk::setPixel(targetPixels, pixelCoord, ((texelColor * .1) + texelColor * lightColor * lightFactorDirectional + fragmentColor).Clamp());
	}
	return countPixels;
}

#endif
