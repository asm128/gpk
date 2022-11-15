#include "gpk_raster_lh.h"
#include <algorithm>

int								gpk::pixelBlend			(::gpk::view_grid<::gpk::SColorBGRA> pixels, ::gpk::SCoord2<int16_t> position, ::gpk::SColorBGRA color)	{
	if( (position.x >= 0 && position.x < (int32_t)pixels.metrics().x)
	 && (position.y >= 0 && position.y < (int32_t)pixels.metrics().y)
	) {
		::gpk::SColorBGRA					& targetPixel	= pixels[position.y][position.x];
		::gpk::SColorFloat					srcPixel		= targetPixel;
		targetPixel						= ::gpk::interpolate_linear(srcPixel, ::gpk::SColorFloat(color), color.a / 255.0f);
		return 1;
	}
	return 0;
}

int								gpk::setPixel			(::gpk::view_grid<::gpk::SColorBGRA> pixels, ::gpk::SCoord2<int16_t> position, ::gpk::SColorBGRA color)	{
	if( (position.x >= 0 && position.x < (int32_t)pixels.metrics().x)
	 && (position.y >= 0 && position.y < (int32_t)pixels.metrics().y)
	) {
		::gpk::SColorBGRA					& targetPixel = pixels[position.y][position.x];
		targetPixel						= {color.b, color.g, color.r, (uint8_t)::gpk::clamp(targetPixel.a + (uint32_t)color.a, 0U, 255U)};
		return 1;
	}
	return 0;
}

int								gpk::drawRectangle		(::gpk::view_grid<::gpk::SColorBGRA> pixels, ::gpk::SRectangle2<int16_t> rectangle, ::gpk::SColorBGRA color)	{
	int32_t								countPixels				= 0;
	for(int16_t y = 0; y < (int16_t)rectangle.Size.y; ++y)
	for(int16_t x = 0; x < (int16_t)rectangle.Size.x; ++x)
		countPixels += ::gpk::setPixel(pixels, {(int16_t)(rectangle.Offset.x + x), (int16_t)(rectangle.Offset.y + y)}, color);
	return countPixels;
}

int								gpk::drawCircle			(::gpk::view_grid<::gpk::SColorBGRA> pixels, ::gpk::SCircle<int16_t> circle, ::gpk::SColorBGRA color)	{
	int32_t								countPixels				= 0;
	for(int16_t y = (int16_t)-circle.Radius; y < (int16_t)circle.Radius; ++y)
	for(int16_t x = (int16_t)-circle.Radius; x < (int16_t)circle.Radius; ++x) {
		::gpk::SCoord2<int16_t>						position			= {x, y};
		if(position.Length() <= circle.Radius)
			countPixels += ::gpk::setPixel(pixels, {int16_t(circle.Center.x + x), int16_t(circle.Center.y + y)}, color);
	}
	return countPixels;
}

// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
int								gpk::drawLine       	(::gpk::view_grid<::gpk::SColorBGRA> pixels, ::gpk::SLine2<int16_t> line, ::gpk::SColorBGRA color)	{
	int32_t								countPixels				= 0;
	int32_t								dx						= (int32_t)fabs(line.B.x - line.A.x);
	int32_t								sx						= (int32_t)line.A.x < line.B.x ? 1 : -1;
	int32_t								dy						= (int32_t)-fabs(line.B.y - line.A.y);
	int32_t								sy						= (int32_t)line.A.y < line.B.y ? 1 : -1;
	int32_t								err						= dx + dy;  /* error value e_xy */
	while (true) {   /* loop */
		if (line.A.x == line.B.x && line.A.y == line.B.y)
			break;
		int32_t								e2						= 2 * err;
		if (e2 >= dy) {
			err								+= dy; /* e_xy+e_x > 0 */
			line.A.x						+= (int16_t)sx;
			countPixels += setPixel(pixels, {line.A.x, line.A.y}, color);
		}
		if (e2 <= dx) { /* e_xy+e_y < 0 */
			err								+= dx;
			line.A.y						+= (int16_t)sy;
			countPixels += setPixel(pixels, {line.A.x, line.A.y}, color);
		}

	}
	return countPixels;
}


// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
int								gpk::drawLine       	(::gpk::view_grid<::gpk::SColorBGRA> pixels, ::gpk::SLine2<int16_t> line, ::gpk::array_pod<::gpk::SCoord2<int16_t>> & pixelCoords)	{
	int32_t								dx						= (int32_t)fabs(line.B.x - line.A.x);
	int32_t								sx						= (int32_t)line.A.x < line.B.x ? 1 : -1;
	int32_t								dy						= (int32_t)-fabs(line.B.y - line.A.y );
	int32_t								sy						= (int32_t)line.A.y < line.B.y ? 1 : -1;
	int32_t								err						= dx + dy;  /* error value e_xy */
	pixelCoords.push_back({line.A.x, line.A.y});
	while (true) {   /* loop */
		if (line.A.x == line.B.x && line.A.y == line.B.y)
			break;
		int32_t								e2						= 2 * err;
		if (e2 >= dy) {
			err								+= dy; /* e_xy+e_x > 0 */
			line.A.x						+= (int16_t)sx;
			if( line.A.x >= 0 && line.A.x < (int32_t)pixels.metrics().x
			 && line.A.y >= 0 && line.A.y < (int32_t)pixels.metrics().y
			)
				pixelCoords.push_back({line.A.x, line.A.y});

		}
		if (e2 <= dx) { /* e_xy+e_y < 0 */
			err								+= dx;
			line.A.y						+= (int16_t)sy;
			if( line.A.x >= 0 && line.A.x < (int32_t)pixels.metrics().x
			 && line.A.y >= 0 && line.A.y < (int32_t)pixels.metrics().y
			)
				pixelCoords.push_back({line.A.x, line.A.y});
		}

	}
	return 0;
}


// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
int								gpk::drawLine
	( ::gpk::view_grid<::gpk::SColorBGRA>			pixels
	, const ::gpk::SLine3<float>					& lineFloat
	, ::gpk::array_pod<::gpk::SCoord3<float>>		& pixelCoords
	, ::gpk::view_grid<uint32_t>					depthBuffer
	) {
	::gpk::SLine2<int32_t>				line					= {{(int32_t)lineFloat.A.x, (int32_t)lineFloat.A.y}, {(int32_t)lineFloat.B.x, (int32_t)lineFloat.B.y}};
	int32_t								xDiff					= (int32_t)fabs(line.B.x - line.A.x);
	int32_t								yDiff					= (int32_t)-fabs(line.B.y - line.A.y);
	int32_t								sx						= (int32_t)line.A.x < line.B.x ? 1 : -1;
	int32_t								sy						= (int32_t)line.A.y < line.B.y ? 1 : -1;
	int32_t								err						= xDiff + yDiff;  /* error value e_xy */

	bool								yAxis					= fabs(yDiff) > xDiff;
	::gpk::view_array<uint32_t>			depthBufferRow			= {};
	if( line.A.x >= 0 && line.A.x < (int32_t)pixels.metrics().x
	 && line.A.y >= 0 && line.A.y < (int32_t)pixels.metrics().y
	 && lineFloat.A.z >= 0 && lineFloat.A.z <= 1
	) {
		depthBufferRow					= depthBuffer[line.A.y];
		uint32_t							& depthCell				= depthBufferRow[line.A.x];
		uint32_t							intZ					= uint32_t(0xFFFFFFFFU * lineFloat.A.z);
		if( depthCell > intZ ) {
			depthCell						= intZ;
			//pixelCoords.push_back({(int32_t)line.A.x, (int32_t)line.A.y});
			pixelCoords.push_back(lineFloat.A);
		}
	}

	//bool								outside				= true;
	const double						factorUnit			= 1.0 / (yAxis ? fabs(yDiff) : xDiff);
	while (true) {   /* loop */
		if (line.A.x == line.B.x && line.A.y == line.B.y)
			break;
		int32_t								e2						= 2 * err;
		if (e2 >= yDiff) {
			err								+= yDiff; /* e_xy+e_x > 0 */
			line.A.x						+= sx;
			if( line.A.x >= 0 && line.A.x < (int32_t)pixels.metrics().x
			 && line.A.y >= 0 && line.A.y < (int32_t)pixels.metrics().y
			) {
				depthBufferRow				= depthBuffer[line.A.y];
				const double						factor					= 1.0 / (yAxis ? factorUnit * line.A.y : factorUnit * line.A.x);
				const double						finalZ					= ::gpk::interpolate_linear(lineFloat.A.z, lineFloat.B.z, factor);// lineFloat.B.z * factor + (lineFloat.A.z * (1.0 - factor));
				if (finalZ <= 0 || finalZ >= 1)
					continue;
				const uint32_t						intZ					= uint32_t(0xFFFFFFFFU * finalZ);
				uint32_t							& depthCell				= depthBufferRow[line.A.x];
				if(depthCell <= intZ)
					continue;

				depthCell						= intZ;
				pixelCoords.push_back({(float)line.A.x, (float)line.A.y, (float)finalZ});
			}
			//else
			//	outside							= true;
		}
		if (e2 <= xDiff) { /* e_xy+e_y < 0 */
			err								+= xDiff;
			line.A.y						+= sy;
			if( line.A.x >= 0 && line.A.x < (int32_t)pixels.metrics().x
			 && line.A.y >= 0 && line.A.y < (int32_t)pixels.metrics().y
			) {
				depthBufferRow					= depthBuffer[line.A.y];
				const double						factor					= 1.0 / (yAxis ? factorUnit * line.A.y : factorUnit * line.A.x);
				const double						finalZ					= ::gpk::interpolate_linear(lineFloat.A.z, lineFloat.B.z, factor);// lineFloat.B.z * factor + (lineFloat.A.z * (1.0 - factor));
				if (finalZ <= 0 || finalZ >= 1)
					continue;
				const uint32_t						intZ					= uint32_t(0xFFFFFFFFU * finalZ);
				uint32_t							& depthCell				= depthBufferRow[line.A.x];
				if(depthCell <= intZ)
					continue;

				depthCell						= intZ;
				pixelCoords.push_back({(float)line.A.x, (float)line.A.y, (float)finalZ});
			}
			//else
			//	outside							= true;
		}

	}
	return 0;
}

//https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/
double									orient2d				(const ::gpk::SLine2<int16_t>	& segment, const ::gpk::SCoord2<int16_t>& point)		{ return (segment.B.x - segment.A.x) * (point.y - (double)segment.A.y) - (segment.B.y - segment.A.y) * (point.x - (double)segment.A.x); }
double									orient2d				(const ::gpk::SLine3<int16_t>	& segment, const ::gpk::SCoord2<int16_t>& point)		{ return (segment.B.x - segment.A.x) * (point.y - (double)segment.A.y) - (segment.B.y - segment.A.y) * (point.x - (double)segment.A.x); }
double									orient2d				(const ::gpk::SLine3<float>		& segment, const ::gpk::SCoord2<int16_t>& point)		{ return (segment.B.x - segment.A.x) * (point.y - (double)segment.A.y) - (segment.B.y - segment.A.y) * (point.x - (double)segment.A.x); }
double									orient2d				(const ::gpk::SLine2<float>		& segment, const ::gpk::SCoord2<int16_t>& point)		{ return (segment.B.x - segment.A.x) * (point.y - (double)segment.A.y) - (segment.B.y - segment.A.y) * (point.x - (double)segment.A.x); }

template <typename _tValue>	_tValue 	max3					(_tValue & a, _tValue & b, _tValue & c)			{ return ::std::max(::std::max(a, b), c); }
template <typename _tValue>	_tValue 	min3					(_tValue & a, _tValue & b, _tValue & c)			{ return ::std::min(::std::min(a, b), c); }

int								gpk::drawTriangle		(::gpk::view_grid<::gpk::SColorBGRA> pixels, const ::gpk::STriangle2<int16_t> & triangle, ::gpk::SColorBGRA color){
	// Compute triangle bounding box
	int16_t								minX					= ::min3(triangle.A.x, triangle.B.x, triangle.C.x);
	int16_t								minY					= ::min3(triangle.A.y, triangle.B.y, triangle.C.y);
	int16_t								maxX					= ::max3(triangle.A.x, triangle.B.x, triangle.C.x);
	int16_t								maxY					= ::max3(triangle.A.y, triangle.B.y, triangle.C.y);

	// Clip against screen bounds
	minX							= ::std::max(minX, (int16_t)0);
	minY							= ::std::max(minY, (int16_t)0);
	maxX							= ::std::min(maxX, int16_t(int16_t(pixels.metrics().x)	- 1));
	maxY							= ::std::min(maxY, int16_t(int16_t(pixels.metrics().y)	- 1));

	// Rasterize
	int32_t								countPixels				= 0;
	::gpk::SCoord2<int16_t>				p;
	for (p.y = minY; p.y <= maxY; ++p.y) {
		for (p.x = minX; p.x <= maxX; ++p.x) {
			// Determine barycentric coordinates
			double									w0						= ::orient2d({triangle.B, triangle.C}, p);
			double									w1						= ::orient2d({triangle.C, triangle.A}, p);
			double									w2						= ::orient2d({triangle.A, triangle.B}, p);
			// If p is on or inside all edges, render pixel.
			if (w0 < 0 || w1 < 0 || w2 < 0)
				continue;
			countPixels += ::gpk::setPixel(pixels, p, color);
		}
	}
	return countPixels;
}

int								gpk::drawTriangle
	( const ::gpk::SCoord2<uint32_t>					targetSize
	, const ::gpk::STriangle3<float>					& triangle
	, ::gpk::array_pod<::gpk::SCoord2<int16_t>>			& pixelCoords
	, ::gpk::array_pod<::gpk::STriangleWeights<float>> & proportions
	, ::gpk::view_grid<uint32_t>						depthBuffer
	)	{
	// Compute triangle bounding box
	int16_t								minX					= (int16_t)::min3(triangle.A.x, triangle.B.x, triangle.C.x);
	int16_t								minY					= (int16_t)::min3(triangle.A.y, triangle.B.y, triangle.C.y);
	int16_t								maxX					= (int16_t)::max3(triangle.A.x, triangle.B.x, triangle.C.x);
	int16_t								maxY					= (int16_t)::max3(triangle.A.y, triangle.B.y, triangle.C.y);

	// Clip against screen bounds
	minX							= ::std::max(minX, (int16_t)0);
	minY							= ::std::max(minY, (int16_t)0);
	maxX							= ::std::min(maxX, (int16_t)((int32_t)targetSize.x - 1));
	maxY							= ::std::min(maxY, (int16_t)((int32_t)targetSize.y - 1));

	// Rasterize
	::gpk::SCoord2<int16_t> p;
	for (p.y = minY; p.y <= maxY; ++p.y)
	for (p.x = minX; p.x <= maxX; ++p.x) {
		// Determine barycentric coordinates
		double								w0						= ::orient2d({triangle.B, triangle.C}, p);
		double								w1						= ::orient2d({triangle.C, triangle.A}, p);
		double								w2						= ::orient2d({triangle.A, triangle.B}, p);
		// If p is on or inside all edges, render pixel.
		if (w0 < 0 || w1 < 0 || w2 < 0)
			continue;

		double								proportionABC			= w0 + w1 + w2;
		if(proportionABC == 0)
			continue;
		double								proportionA				= w0 / proportionABC;
		double								proportionB				= w1 / proportionABC;
		double								proportionC				= 1.0 - (proportionA + proportionB);

		// Calculate interpolated depth
		double								finalZ					= triangle.A.z * proportionA;
		finalZ							+= triangle.B.z * proportionB;
		finalZ							+= triangle.C.z * proportionC;
		if(finalZ >= 1.0 || finalZ <= 0)
			continue;

		uint32_t							intZ					= uint32_t(0xFFFFFFFFU * finalZ);
		uint32_t							& currentDepth			= depthBuffer[p.y][p.x];
		if(currentDepth <= intZ)
			continue;

		currentDepth					= intZ;
		pixelCoords.push_back(p.Cast<int16_t>());
		proportions.push_back({(float)proportionA, (float)proportionB, (float)proportionC});

	}
	return 0;
}

int								gpk::drawTriangle
	( const ::gpk::SCoord2<uint32_t>					targetSize
	, const ::gpk::STriangle2<float>					& triangle
	, ::gpk::array_pod<::gpk::SCoord2<int16_t>>			& pixelCoords
	, ::gpk::array_pod<::gpk::STriangleWeights<float>> & proportions
	)	{
	// Compute triangle bounding box
	int16_t								minX					= (int16_t)::min3(triangle.A.x, triangle.B.x, triangle.C.x);
	int16_t								minY					= (int16_t)::min3(triangle.A.y, triangle.B.y, triangle.C.y);
	int16_t								maxX					= (int16_t)::max3(triangle.A.x, triangle.B.x, triangle.C.x);
	int16_t								maxY					= (int16_t)::max3(triangle.A.y, triangle.B.y, triangle.C.y);

	// Clip against screen bounds
	minX							= ::std::max(minX, (int16_t)0);
	minY							= ::std::max(minY, (int16_t)0);
	maxX							= ::std::min(maxX, (int16_t)((int32_t)targetSize.x - 1));
	maxY							= ::std::min(maxY, (int16_t)((int32_t)targetSize.y - 1));

	// Rasterize
	::gpk::SCoord2<int16_t> p;
	for (p.y = minY; p.y <= maxY; ++p.y)
	for (p.x = minX; p.x <= maxX; ++p.x) {
		// Determine barycentric coordinates
		double								w0						= ::orient2d({triangle.B, triangle.C}, p);
		double								w1						= ::orient2d({triangle.C, triangle.A}, p);
		double								w2						= ::orient2d({triangle.A, triangle.B}, p);
		// If p is on or inside all edges, render pixel.
		if (w0 < 0 || w1 < 0 || w2 < 0)
			continue;

		double								proportionABC			= w0 + w1 + w2;
		if(proportionABC == 0)
			continue;
		double								proportionA				= w0 / proportionABC;
		double								proportionB				= w1 / proportionABC;
		double								proportionC				= 1.0 - (proportionA + proportionB);

		pixelCoords.push_back(p.Cast<int16_t>());
		proportions.push_back({(float)proportionA, (float)proportionB, (float)proportionC});

	}
	return 0;
}

int													gpk::drawPixels
	( ::gpk::view_grid<::gpk::SColorBGRA>				targetPixels
	, const ::gpk::STriangle3	<float>					& triangleWorld
	, const ::gpk::SCoord3		<float>					& normal
	, const ::gpk::STriangle2	<float>					& triangleTexCoords
	, const ::gpk::SCoord3		<float>					& lightVector
	, ::gpk::array_pod<::gpk::SCoord2<int16_t>>			& pixelCoords
	, ::gpk::array_pod<::gpk::STriangleWeights<float>>	& pixelVertexWeights
	, ::gpk::view_grid<const ::gpk::SColorBGRA>			textureImage
	, ::gpk::array_pod<::gpk::SCoord3<float>>			& lightPoints
	, ::gpk::array_pod<::gpk::SColorBGRA>				& lightColors
	, const ::std::function<::gpk::error_t(::gpk::view_grid<::gpk::SColorBGRA> targetPixels, const ::gpk::SCoord2<int16_t> & pixelCoord, const ::gpk::SColorBGRA & color)> & funcSetPixel
	) {
	const ::gpk::SCoord2<float>								imageUnit				= {textureImage.metrics().x - 0.000001f, textureImage.metrics().y - 0.000001f};
	double													lightFactorDirectional	= normal.Dot(lightVector);
	(void)lightFactorDirectional;
	int32_t								countPixels				= 0;
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::gpk::SCoord2<int16_t>									pixelCoord				= pixelCoords		[iPixelCoord];
		const ::gpk::STriangleWeights<float>					& vertexWeights			= pixelVertexWeights[iPixelCoord];
		const ::gpk::SCoord2<float>								texCoord				= ::gpk::triangleWeight(vertexWeights, triangleTexCoords);
		const ::gpk::SCoord3<float>								position				= ::gpk::triangleWeight(vertexWeights, triangleWorld);
		const ::gpk::SColorFloat								texelColor				= textureImage.size() ? textureImage[(uint32_t)(texCoord.y * imageUnit.y) % textureImage.metrics().y][(uint32_t)(texCoord.x * imageUnit.x) % textureImage.metrics().x] : ::gpk::SColorBGRA{::gpk::GRAY};
		::gpk::SColorFloat										fragmentColor			= {};
		static constexpr	const double						rangeLight				= 10.0;
		static constexpr	const double						rangeLightSquared		= rangeLight * rangeLight;
		static constexpr	const double						rangeUnit				= 1.0 / rangeLightSquared;
		for(uint32_t iLight = 0; iLight < lightPoints.size(); ++iLight) {
			const ::gpk::SCoord3<float>								lightToPoint			= lightPoints[iLight] - position;
			const ::gpk::SCoord3<float>								vectorToLight			= lightToPoint;
			const double											lightFactor				= vectorToLight.Dot(normal);
			const double											distanceToLight			= lightToPoint.LengthSquared();
			if(distanceToLight > rangeLightSquared)
				continue;
			fragmentColor										+= texelColor / (10.0 * lightPoints.size());
			if(lightFactor <= 0)
				continue;
			const double											invAttenuation			= ::std::max(0.0, 1.0 - (distanceToLight * rangeUnit));
			fragmentColor										+= (texelColor * lightColors[iLight]) * (invAttenuation);
		}
		countPixels += funcSetPixel(targetPixels, pixelCoord, (texelColor * .2 + (texelColor * lightFactorDirectional).Clamp() + fragmentColor.Clamp()).Clamp());
	}
	return countPixels;
}

int													gpk::drawPixels
	( ::gpk::view_grid<::gpk::SColorBGRA>				targetPixels
	, const ::gpk::STriangle3	<float>					& triangleWorld
	, const ::gpk::SCoord3		<float>					& normal
	, const ::gpk::STriangle2	<float>					& triangleTexCoords
	, const ::gpk::SCoord3		<float>					& lightVector
	, ::gpk::array_pod<::gpk::SCoord2<int16_t>>			& pixelCoords
	, ::gpk::array_pod<::gpk::STriangleWeights<float>>	& pixelVertexWeights
	, ::gpk::view_grid<const ::gpk::SColorBGRA>			textureImage
	, ::gpk::array_pod<::gpk::SCoord3<float>>			& lightPoints
	, ::gpk::array_pod<::gpk::SColorBGRA>				& lightColors
	) {

	return gpk::drawPixels(targetPixels, triangleWorld, normal, triangleTexCoords, lightVector, pixelCoords, pixelVertexWeights, textureImage, lightPoints, lightColors, setPixel);
}

int													gpk::drawQuadTriangle
	( const ::gpk::SCoord2<uint32_t>					& targetSize
	, const ::gpk::STriangle3	<float>					& triangleScreen
	, ::gpk::array_pod<::gpk::SCoord2<int16_t>>			& pixelCoords
	, ::gpk::array_pod<::gpk::STriangleWeights<float>>	& pixelVertexWeights
	, ::gpk::view_grid<uint32_t>						depthBuffer
	) {
	return ::gpk::drawTriangle(targetSize, triangleScreen, pixelCoords, pixelVertexWeights, depthBuffer);
}

int													gpk::drawQuadTriangle
	( const ::gpk::SCoord2<uint32_t>					& targetSize
	, ::gpk::STriangle3			<float>					triangle
	, const ::gpk::SMatrix4<float>						& matrixTransformVP
	, ::gpk::array_pod<::gpk::SCoord2<int16_t>>			& pixelCoords
	, ::gpk::array_pod<::gpk::STriangleWeights<float>>	& pixelVertexWeights
	, ::gpk::view_grid<uint32_t>						depthBuffer
	) {
	::gpk::transform(triangle, matrixTransformVP);
	if( triangle.A.z <= 0 || triangle.A.z >= 1
	 || triangle.B.z <= 0 || triangle.B.z >= 1
	 || triangle.C.z <= 0 || triangle.C.z >= 1
	)
		return 0;
	return ::gpk::drawQuadTriangle(targetSize, triangle, pixelCoords, pixelVertexWeights, depthBuffer);
}

int													gpk::drawQuadTriangle
	( ::gpk::view_grid<::gpk::SColorBGRA>				targetPixels
	, const ::gpk::SGeometryQuads						& geometry
	, const int											iTriangle
	, const ::gpk::SMatrix4<float>						& matrixTransform
	, const ::gpk::SMatrix4<float>						& matrixTransformVP
	, const ::gpk::SCoord3<float>						& lightVector
	, ::gpk::array_pod<::gpk::SCoord2<int16_t>>			& pixelCoords
	, ::gpk::array_pod<::gpk::STriangleWeights<float>>	& pixelVertexWeights
	, ::gpk::view_grid<const ::gpk::SColorBGRA>			textureImage
	, ::gpk::array_pod<::gpk::SCoord3<float>>			& lightPoints
	, ::gpk::array_pod<::gpk::SColorBGRA>				& lightColors
	, ::gpk::view_grid<uint32_t>						depthBuffer
	) {
	return ::gpk::drawQuadTriangle(targetPixels, geometry, iTriangle, matrixTransform, matrixTransformVP, lightVector, pixelCoords, pixelVertexWeights, textureImage, lightPoints, lightColors, depthBuffer, setPixel);
}

int													gpk::drawQuadTriangle
	( ::gpk::view_grid<::gpk::SColorBGRA>				targetPixels
	, const ::gpk::SGeometryQuads						& geometry
	, const int											iTriangle
	, const ::gpk::SMatrix4<float>						& matrixTransform
	, const ::gpk::SMatrix4<float>						& matrixTransformVP
	, const ::gpk::SCoord3<float>						& lightVector
	, ::gpk::array_pod<::gpk::SCoord2<int16_t>>			& pixelCoords
	, ::gpk::array_pod<::gpk::STriangleWeights<float>>	& pixelVertexWeights
	, ::gpk::view_grid<const ::gpk::SColorBGRA>			textureImage
	, ::gpk::array_pod<::gpk::SCoord3<float>>			& lightPoints
	, ::gpk::array_pod<::gpk::SColorBGRA>				& lightColors
	, ::gpk::view_grid<uint32_t>						depthBuffer
	, const ::std::function<::gpk::error_t(::gpk::view_grid<::gpk::SColorBGRA> targetPixels, const ::gpk::SCoord2<int16_t> & pixelCoord, const ::gpk::SColorBGRA & color)> & funcSetPixel
	) {
	const ::gpk::STriangle3	<float>							& triangle			= geometry.Triangles	[iTriangle];;
	const ::gpk::STriangle2	<float>							& triangleTexCoords	= geometry.TextureCoords[iTriangle];
	const ::gpk::SCoord3	<float>							& normal			= geometry.Normals		[iTriangle / 2];
	::gpk::drawQuadTriangle(targetPixels.metrics(), triangle, matrixTransformVP, pixelCoords, pixelVertexWeights, depthBuffer);
	const ::gpk::SCoord3	<float>							xnormal				= matrixTransform.TransformDirection(normal).Normalize();
	::gpk::STriangle3		<float>							triangleWorld		= triangle;
	::gpk::transform(triangleWorld, matrixTransform);
	return ::gpk::drawPixels(targetPixels, triangleWorld, xnormal, triangleTexCoords, lightVector, pixelCoords, pixelVertexWeights, textureImage, lightPoints, lightColors, funcSetPixel);
}

int													gpk::drawQuadTriangle
	( ::gpk::view_grid<::gpk::SColorBGRA>				targetPixels
	, const ::gpk::SGeometryQuads						& geometry
	, const int											iTriangle
	, const ::gpk::SMatrix4<float>						& matrixTransform
	, const ::gpk::SMatrix4<float>						& matrixTransformView
	, const ::gpk::SCoord3<float>						& lightVector
	, const ::gpk::SColorBGRA							color
	, ::gpk::array_pod<::gpk::SCoord2<int16_t>>			& pixelCoords
	, ::gpk::array_pod<::gpk::STriangleWeights<float>>	& pixelVertexWeights
	, ::gpk::view_grid<uint32_t>						depthBuffer
	) {
	::gpk::STriangle3	<float>								triangle			= geometry.Triangles	[iTriangle];
	::gpk::SCoord3		<float>								normal				= geometry.Normals		[iTriangle / 2];
	normal												= matrixTransform.TransformDirection(normal).Normalize();
	::gpk::transform(triangle, matrixTransformView);
	if( triangle.A.z <= 0 || triangle.A.z >= 1
	 || triangle.B.z <= 0 || triangle.B.z >= 1
	 || triangle.C.z <= 0 || triangle.C.z >= 1
	)
		return 0;

	double													lightFactor			= normal.Dot(lightVector);

	int32_t								countPixels				= 0;
	::gpk::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::gpk::SCoord2<int16_t>									pixelCoord			= pixelCoords[iPixelCoord];
		countPixels += ::gpk::setPixel(targetPixels, pixelCoord, (color * 0.1) + color * lightFactor);
	}
	return countPixels;
}

int													gpk::drawTriangle
	( ::gpk::view_grid<::gpk::SColorBGRA>				targetPixels
	, const ::gpk::SGeometryTriangles					& geometry
	, const int											iTriangle
	, const ::gpk::SMatrix4<float>						& matrixTransform
	, const ::gpk::SMatrix4<float>						& matrixTransformView
	, const ::gpk::SCoord3<float>						& lightVector
	, const ::gpk::SColorBGRA							color
	, ::gpk::array_pod<::gpk::SCoord2<int16_t>>			& pixelCoords
	, ::gpk::array_pod<::gpk::STriangleWeights<float>>	& pixelVertexWeights
	, ::gpk::view_grid<uint32_t>						depthBuffer
	) {
	::gpk::STriangle3		<float>								triangle			= geometry.Triangles	[iTriangle];
	const ::gpk::STriangle3	<float>								& triangleNormals	= geometry.Normals		[iTriangle];
	::gpk::transform(triangle, matrixTransformView);
	if( triangle.A.z <= 0 || triangle.A.z >= 1
	 || triangle.B.z <= 0 || triangle.B.z >= 1
	 || triangle.C.z <= 0 || triangle.C.z >= 1
	)
		return 0;

	::gpk::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);
	int32_t								countPixels				= 0;
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		const ::gpk::SCoord2<int16_t>							pixelCoord				= pixelCoords		[iPixelCoord];
		const ::gpk::STriangleWeights<float>					& vertexWeights			= pixelVertexWeights[iPixelCoord];
		::gpk::SCoord3<float>									normal				= ::gpk::triangleWeight(vertexWeights, triangleNormals);
		normal												= matrixTransform.TransformDirection(normal).Normalize();
		double													lightFactor			= normal.Dot(lightVector);
		countPixels += ::gpk::setPixel(targetPixels, pixelCoord, color * lightFactor);
	}
	return countPixels;
}

int													gpk::drawTriangle
	( const ::gpk::view_grid<::gpk::SColorBGRA>			targetPixels
	, const ::gpk::SGeometryTriangles					& geometry
	, const int											iTriangle
	, const ::gpk::SMatrix4<float>						& matrixTransform
	, const ::gpk::SMatrix4<float>						& matrixTransformView
	, const ::gpk::SCoord3<float>						& lightVector
	, ::gpk::array_pod<::gpk::SCoord2<int16_t>>			& pixelCoords
	, ::gpk::array_pod<::gpk::STriangleWeights<float>>	& pixelVertexWeights
	, ::gpk::view_grid<const ::gpk::SColorBGRA>			textureImage
	, ::gpk::view_grid<uint32_t>						depthBuffer
	) {
	::gpk::STriangle3		<float>								triangle			= geometry.Triangles		[iTriangle];
	const ::gpk::STriangle3	<float>								& triangleNormals	= geometry.Normals			[iTriangle];
	const ::gpk::STriangle2	<float>								& triangleTexCoords	= geometry.TextureCoords	[iTriangle];
	::gpk::transform(triangle, matrixTransformView);
	if( triangle.A.z <= 0 || triangle.A.z >= 1
	 || triangle.B.z <= 0 || triangle.B.z >= 1
	 || triangle.C.z <= 0 || triangle.C.z >= 1
	)
		return 0;

	int32_t								countPixels				= 0;
	::gpk::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);
	const ::gpk::SCoord2<float>									imageUnit				= {textureImage.metrics().x - 0.000001f, textureImage.metrics().y - 0.000001f};
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		const ::gpk::SCoord2<int16_t>								pixelCoord				= pixelCoords		[iPixelCoord];
		const ::gpk::STriangleWeights<float>						& vertexWeights			= pixelVertexWeights[iPixelCoord];
		::gpk::SCoord3<float>										normal				= ::gpk::triangleWeight(vertexWeights, triangleNormals);
		normal													= matrixTransform.TransformDirection(normal).Normalize();

		double														lightFactor				= normal.Dot(lightVector);

		::gpk::SCoord2<float>										texCoord				= triangleTexCoords.A * vertexWeights.A;
		texCoord												+= triangleTexCoords.B * vertexWeights.B;
		texCoord												+= triangleTexCoords.C * vertexWeights.C;
		::gpk::SColorBGRA											texelColor				= textureImage[(uint32_t)(texCoord.y * imageUnit.y)][(uint32_t)(texCoord.x * imageUnit.x)];
		countPixels += ::gpk::setPixel(targetPixels, pixelCoord, (::gpk::SColorFloat(texelColor) * .3 + ::gpk::SColorFloat(texelColor) * lightFactor).Clamp());
	}
	return countPixels;
}

int													gpk::drawTriangle
	( const ::gpk::view_grid<::gpk::SColorBGRA>			targetPixels
	, const ::gpk::SGeometryTriangles					& geometry
	, const int											iTriangle
	, const ::gpk::SMatrix4<float>						& matrixTransform
	, const ::gpk::SMatrix4<float>						& matrixTransformView
	, const ::gpk::SCoord3<float>						& lightVector
	, const ::gpk::SColorFloat							& lightColor
	, ::gpk::array_pod<::gpk::SCoord2<int16_t>>			& pixelCoords
	, ::gpk::array_pod<::gpk::STriangleWeights<float>>	& pixelVertexWeights
	, ::gpk::view_grid<const ::gpk::SColorBGRA>			textureImage
	, ::gpk::array_pod<::gpk::SLight3>					& lightPoints
	, ::gpk::array_pod<::gpk::SColorBGRA>				& lightColors
	, ::gpk::view_grid<uint32_t>						depthBuffer
	) {
	::gpk::STriangle3		<float>								triangleWorld			= geometry.Triangles		[iTriangle];
	::gpk::STriangle3		<float>								triangle				= triangleWorld;
	::gpk::transform(triangle, matrixTransformView);
	if( triangle.A.z < 0 || triangle.A.z >= 1
	 || triangle.B.z < 0 || triangle.B.z >= 1
	 || triangle.C.z < 0 || triangle.C.z >= 1
	)
		return 0;

	int32_t								countPixels				= 0;
	::gpk::transform(triangleWorld, matrixTransform);
	::gpk::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);

	const ::gpk::STriangle3	<float>								& triangleNormals		= geometry.Normals			[iTriangle];
	const ::gpk::STriangle2	<float>								& triangleTexCoords		= geometry.TextureCoords	[iTriangle];
	const ::gpk::SCoord2<float>									imageUnit				= {textureImage.metrics().x - 0.000001f, textureImage.metrics().y - 0.000001f};
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		const ::gpk::SCoord2<int16_t>								pixelCoord				= pixelCoords		[iPixelCoord];
		const ::gpk::STriangleWeights<float>						& vertexWeights			= pixelVertexWeights[iPixelCoord];

		::gpk::SCoord3<float>										normal					= ::gpk::triangleWeight(vertexWeights, triangleNormals);
		normal													= matrixTransform.TransformDirection(normal).Normalize();
		double														lightFactorDirectional	= normal.Dot(lightVector);
		::gpk::SCoord3<float>										position				= ::gpk::triangleWeight(vertexWeights, triangleWorld);
		::gpk::SCoord2<float>										texCoord				= ::gpk::triangleWeight(vertexWeights, triangleTexCoords);
		::gpk::SColorFloat											texelColor				= textureImage[(uint32_t)(texCoord.y * imageUnit.y) % textureImage.metrics().y][(uint32_t)(texCoord.x * imageUnit.x) % textureImage.metrics().x];
		::gpk::SColorFloat											fragmentColor			= {};

		for(uint32_t iLight = 0; iLight < lightPoints.size(); ++iLight) {
			const ::gpk::SLight3										& light					= lightPoints[iLight];

			::gpk::SCoord3<float>										lightToPoint			= light.Position - position;
			::gpk::SCoord3<float>										vectorToLight			= lightToPoint;
			vectorToLight.Normalize();
			double														lightFactorPoint		= vectorToLight.Dot(normal);
			if(lightToPoint.Length() > light.Range || lightFactorPoint <= 0)
				continue;
			double														invAttenuation			= ::std::max(0.0, 1.0 - (lightToPoint.Length() / light.Range));
			fragmentColor											+= ::gpk::SColorFloat{texelColor * lightColors[iLight] * invAttenuation * .5}.Clamp();
		}
		 (void)lightColor;
		 (void)lightFactorDirectional ;

		countPixels += ::gpk::setPixel(targetPixels, pixelCoord, ((texelColor * .1) + texelColor * lightColor * lightFactorDirectional + fragmentColor).Clamp());
	}
	return countPixels;
}


int													gpk::drawTriangle
	( const ::gpk::view_grid<::gpk::SColorBGRA>			targetPixels
	, const ::gpk::SGeometryIndexedTriangles			& geometry
	, const int											iTriangle
	, const ::gpk::SMatrix4<float>						& matrixTransform
	, const ::gpk::SMatrix4<float>						& matrixTransformView
	, const ::gpk::SCoord3<float>						& lightVector
	, const ::gpk::SColorFloat							& lightColor
	, ::gpk::array_pod<::gpk::SCoord2<int16_t>>			& pixelCoords
	, ::gpk::array_pod<::gpk::STriangleWeights<float>>	& pixelVertexWeights
	, ::gpk::view_grid<const ::gpk::SColorBGRA>			textureImage
	, ::gpk::array_pod<::gpk::SLight3>					& lightPoints
	, ::gpk::array_pod<::gpk::SColorFloat>				& lightColors
	, ::gpk::view_grid<uint32_t>						depthBuffer
	) {
	::gpk::STriangle		<uint16_t>							triangleIndices			= geometry.PositionIndices		[iTriangle];
	::gpk::STriangle3		<float>								triangleWorld			= {geometry.Positions[triangleIndices.A], geometry.Positions[triangleIndices.B], geometry.Positions[triangleIndices.C]};
	::gpk::STriangle3		<float>								triangle				= triangleWorld;
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

	int32_t								countPixels				= 0;
	::gpk::transform(triangleWorld, matrixTransform);
	::gpk::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);

	const ::gpk::STriangle3	<float>								triangleNormals			= {geometry.Normals[triangleIndices.A], geometry.Normals[triangleIndices.B], geometry.Normals[triangleIndices.C]};
	const ::gpk::STriangle2	<float>								triangleTexCoords		= {geometry.TextureCoords[triangleIndices.A], geometry.TextureCoords[triangleIndices.B], geometry.TextureCoords[triangleIndices.C]};
	const ::gpk::SCoord2<float>									imageUnit				= {textureImage.metrics().x - 0.000001f, textureImage.metrics().y - 0.000001f};
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		const ::gpk::SCoord2<int16_t>								pixelCoord				= pixelCoords		[iPixelCoord];
		const ::gpk::STriangleWeights<float>						& vertexWeights			= pixelVertexWeights[iPixelCoord];

		::gpk::SCoord3<float>										normal					= ::gpk::triangleWeight(vertexWeights, triangleNormals);
		normal													= matrixTransform.TransformDirection(normal).Normalize();
		double														lightFactorDirectional	= normal.Dot(lightVector);
		::gpk::SCoord3<float>										position				= ::gpk::triangleWeight(vertexWeights, triangleWorld);
		::gpk::SCoord2<float>										texCoord				= ::gpk::triangleWeight(vertexWeights, triangleTexCoords);
		::gpk::SColorFloat											texelColor				= textureImage[(uint32_t)(texCoord.y * imageUnit.y) % textureImage.metrics().y][(uint32_t)(texCoord.x * imageUnit.x) % textureImage.metrics().x];
		::gpk::SColorFloat											fragmentColor			= {};

		for(uint32_t iLight = 0; iLight < lightPoints.size(); ++iLight) {
			const ::gpk::SLight3										& light					= lightPoints[iLight];

			::gpk::SCoord3<float>										lightToPoint			= light.Position - position;
			::gpk::SCoord3<float>										vectorToLight			= lightToPoint;
			vectorToLight.Normalize();
			double														lightFactorPoint		= vectorToLight.Dot(normal);
			if(lightToPoint.Length() > light.Range || lightFactorPoint <= 0)
				continue;
			double														invAttenuation			= ::std::max(0.0, 1.0 - (lightToPoint.Length() / light.Range));
			fragmentColor											+= (texelColor * lightColors[iLight] * invAttenuation).Clamp();
		}
		 (void)lightColor;
		 (void)lightFactorDirectional ;

		countPixels += ::gpk::setPixel(targetPixels, pixelCoord, ((texelColor * .1) + texelColor * lightColor * lightFactorDirectional + fragmentColor).Clamp());
	}
	return countPixels;
}

