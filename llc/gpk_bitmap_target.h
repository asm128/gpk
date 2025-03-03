#include "gpk_image.h"
#include "gpk_color.h"
#include "gpk_camera.h"

#include "gpk_tri2.h"
#include "gpk_line2.h"
#include "gpk_line3.h"
#include "gpk_circle.h"

#include "gpk_swap.h" // this is required for ::gpk::swap()


#ifndef BITMAP_TARGET_H_23627
#define BITMAP_TARGET_H_23627

namespace gpk
{
	tplt<tpnm _tCoord, tpnm T>
	::gpk::error_t		drawPixelBrightness		(::gpk::grid<T> & viewOffscreen, const ::gpk::n2<_tCoord> & sourcePosition, const T & colorLight, float factor, double range)								{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		::gpk::n2f64			maxRange				= {range, range};
		double					rangeUnit				= 1.0 / maxRange.Length();
		for(int32_t y = -(int32_t)range - 1, blendCount = 1 + (int32_t)range + 1; y < blendCount; ++y)	// the + 1 - 1 is because we actually process more surrounding pixels in order to compensate for the flooring of the coordinates
		for(int32_t x = -(int32_t)range - 1; x < blendCount; ++x) {										// as it causes a visual effect of the light being cut to a rectangle and having sharp borders.
			if(x || y) {
				::gpk::n2<_tCoord>		blendPos				= sourcePosition + ::gpk::n2<_tCoord>{(_tCoord)x, (_tCoord)y};
				if( blendPos.x < (int32_t)viewOffscreen.metrics().x && blendPos.x >= 0
				 && blendPos.y < (int32_t)viewOffscreen.metrics().y && blendPos.y >= 0
				 ) {
					::gpk::n2<_tCoord>		brightDistance			= blendPos - sourcePosition;
					double					brightDistanceLength	= brightDistance.Length();
					double					distanceFactor			= brightDistanceLength * rangeUnit;
					if(distanceFactor <= 1.0) {
						viewOffscreen[(uint32_t)blendPos.y][(uint32_t)blendPos.x]	= ::gpk::interpolate_linear(viewOffscreen[(uint32_t)blendPos.y][(uint32_t)blendPos.x], colorLight, factor * (1.0f - distanceFactor));
					}
				}
			}
		}
		return 0;
	}

	tplt<tpnm _tCoord, tpnm T>
	::gpk::error_t		drawPixelLight				(::gpk::grid<T> & viewOffscreen, const ::gpk::n2<_tCoord> & sourcePosition, const T & colorLight, float maxFactor, double range)								{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
		if( ((uint32_t)sourcePosition.x) < viewOffscreen.metrics().x
		 && ((uint32_t)sourcePosition.y) < viewOffscreen.metrics().y
		 )
			viewOffscreen[(uint32_t)sourcePosition.y][(uint32_t)sourcePosition.x]	= colorLight;
		return drawPixelBrightness(viewOffscreen, sourcePosition, colorLight, maxFactor, range);
	}

	tplt<tpnm _tColor>
	::gpk::error_t			drawLineVertical		(::gpk::grid<_tColor> & target, const _tColor& value, int32_t x, int32_t y1, int32_t y2)	{
		if(x < 0 || x >= (int32_t)target.metrics().x)
			return 0;
		const int32_t				yMin					= ::gpk::min(y1, y2);
		const int32_t				yMax					= ::gpk::max(y1, y2);
		const int32_t				yStop					= ::gpk::min(yMax, (int32_t)target.metrics().y);
		int32_t						y						= ::gpk::max(yMin, (int32_t)0);
		for(; y < yStop; ++y)
			target[y][x]			= value;
		return y;
	}

	tplt<tpnm _tColor>
	::gpk::error_t			drawLineHorizontal		(::gpk::grid<_tColor> & target, const _tColor& value, int32_t y, int32_t x1, int32_t x2)	{
		if(y < 0 || y >= (int32_t)target.metrics().y)
			return 0;
		const int32_t				xMin					= ::gpk::min(x1, x2);
		const int32_t				xMax					= ::gpk::max(x1, x2);
		const int32_t				xStop					= ::gpk::min(xMax, (int32_t)target.metrics().x);
		int32_t						x						= ::gpk::max(xMin, (int32_t)0);
		for(; x < xStop; ++x)
			target[y][x]			= value;
		return x;
	}

	// This implementation is incorrect. The problem is that it draws borders even if it shuoldn't. I never tested it but I believe that's what the code says.
	tplt<tpnm _tCoord, tpnm _tColor>
	static	::gpk::error_t	drawRectangleBorder		(::gpk::grid<_tColor> & bitmapTarget, const _tColor & value, const ::gpk::rect2<_tCoord> & rectangle)		{
		int32_t						yStart					= (int32_t)::gpk::max((int32_t)0, (int32_t)rectangle.Offset.y);
		int32_t						yStop					= ::gpk::min((int32_t)rectangle.Offset.y + (int32_t)rectangle.Size.y, (int32_t)bitmapTarget.metrics().y);
		int32_t						xStart					= (int32_t)::gpk::max((int32_t)0, (int32_t)rectangle.Offset.x);
		int32_t						xStop					= ::gpk::min((int32_t)rectangle.Offset.x + (int32_t)rectangle.Size.x, (int32_t)bitmapTarget.metrics().x);
		if(yStart >= yStop || xStart >= xStop)
			return 0;
		for(int32_t x = xStart; x < xStop; ++x)
			bitmapTarget[yStart][x]	= value;
		memcpy(&bitmapTarget[yStop - 1][xStart], &bitmapTarget[yStart][xStart], sizeof(_tColor) * xStop - xStart);
		for(int32_t y = yStart + 1, yMax = (yStop - 1); y < yMax; ++y) {
			bitmapTarget[y][0]			= value;
			bitmapTarget[y][xStop - 1]	= value;
		}
		return (yStop - yStart) * 2 + (xStop - xStart) * 2;
	}

	tplt<tpnm _tCoord, tpnm _tColor>
	static	::gpk::error_t	drawCircle			(::gpk::grid<_tColor> & bitmapTarget, const _tColor & value, const ::gpk::circle<_tCoord> & circle)			{
		int32_t						xStop				= ::gpk::min((int32_t)(circle.Center.x + circle.Radius + 2), (int32_t)bitmapTarget.metrics().x);
		double						radiusSquared		= circle.Radius * circle.Radius;
		int32_t						pixelsDrawn			= 0;
		const int32_t				startY				= ::gpk::max((int32_t)0, (int32_t)(circle.Center.y - circle.Radius));
		const int32_t				startX				= ::gpk::max((int32_t)0, (int32_t)(circle.Center.x - circle.Radius));
		for(int32_t y = startY, yStop = ::gpk::min((int32_t)(circle.Center.y + circle.Radius + 2), (int32_t)bitmapTarget.metrics().y); y < yStop; ++y)
		for(int32_t x = startX; x < xStop; ++x) {
			::gpk::n2i32				cellCurrent			= {x, y};
			double						distanceSquared		= (cellCurrent - circle.Center).LengthSquared();
			if(distanceSquared < radiusSquared) {
				if(circle.Center.x - circle.Radius < 0)
					bitmapTarget[y][x]		= value;
				else {
				 	for(const int32_t xLimit = ::gpk::min((int32_t)circle.Center.x + (int32_t)(circle.Center.x - x), (int32_t)bitmapTarget.metrics().x); x < xLimit; ++x) {
						bitmapTarget[y][x]		= value;
						++pixelsDrawn;
					}
					break;
				}
			}
		}
		return pixelsDrawn;
	}

	tplt<tpnm _tCoord, tpnm _tColor>
	static	::gpk::error_t	drawCircle			(const ::gpk::n2u32 & targetMetrics, const ::gpk::circle<_tCoord> & circle, ::gpk::apod<::gpk::n2i32> & out_Points)			{
		int32_t						xStop				= ::gpk::min((int32_t)(circle.Center.x + circle.Radius), (int32_t)targetMetrics.x);
		double						radiusSquared		= circle.Radius * circle.Radius;
		int32_t						pixelsDrawn			= 0;
		for(int32_t y = ::gpk::max((int32_t)0, (int32_t)(circle.Center.y - circle.Radius)), yStop = ::gpk::min((int32_t)(circle.Center.y + circle.Radius), (int32_t)targetMetrics.y); y < yStop; ++y)
		for(int32_t x = ::gpk::max((int32_t)0, (int32_t)(circle.Center.x - circle.Radius)); x < xStop; ++x) {
			::gpk::n2i32				cellCurrent			= {x, y};
			double						distanceSquared		= (cellCurrent - circle.Center).LengthSquared();
			if(distanceSquared < radiusSquared) {
				if(circle.Center.x - circle.Radius < 0)
					out_Points.push_back({x, y});
				else {
			 		for(const int32_t xLimit = ::gpk::min((int32_t)circle.Center.x + ((int32_t)circle.Center.x - x), (int32_t)targetMetrics.x); x < xLimit; ++x) {
						out_Points.push_back({x, y});
						++pixelsDrawn;
					}
					break;
				}
			}
		}
		return pixelsDrawn;
	}

	// A good article on this kind of triangle rasterization: https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/
	tplt<tpnm _tCoord, tpnm _tColor>
	static	::gpk::error_t	drawTriangle		(::gpk::grid<_tColor>& bitmapTarget, const _tColor& value, const ::gpk::tri2<_tCoord>& triangle)										{
		::gpk::n2i32				areaMin				= {(int32_t)::gpk::min(::gpk::min(triangle.A.x, triangle.B.x), triangle.C.x), (int32_t)::gpk::min(::gpk::min(triangle.A.y, triangle.B.y), triangle.C.y)};
		::gpk::n2i32				areaMax				= {(int32_t)::gpk::max(::gpk::max(triangle.A.x, triangle.B.x), triangle.C.x), (int32_t)::gpk::max(::gpk::max(triangle.A.y, triangle.B.y), triangle.C.y)};
		const int32_t				xStop				= ::gpk::min(areaMax.x, (int32_t)bitmapTarget.metrics().x);
		int32_t						pixelsDrawn			= 0;
		for(int32_t y = ::gpk::max(areaMin.y, (int32_t)0), yStop = ::gpk::min(areaMax.y, (int32_t)bitmapTarget.metrics().y); y < yStop; ++y)
		for(int32_t x = ::gpk::max(areaMin.x, (int32_t)0); x < xStop; ++x) {
			const ::gpk::n2i16			cellCurrent			= {(int16_t)x, (int16_t)y};
			// Determine barycentric coordinates
			int							w0					= ::gpk::orient2d({triangle.B.s1_t(), triangle.A.s1_t()}, cellCurrent);	// ::gpk::orient2d({triangle.A, triangle.B}, cellCurrent);
			int							w1					= ::gpk::orient2d({triangle.C.s1_t(), triangle.B.s1_t()}, cellCurrent);	// ::gpk::orient2d({triangle.B, triangle.C}, cellCurrent);
			int							w2					= ::gpk::orient2d({triangle.A.s1_t(), triangle.C.s1_t()}, cellCurrent);	// ::gpk::orient2d({triangle.C, triangle.A}, cellCurrent);
			if(w0 < 0)
				continue;
			if(w1 < 0)
				continue;
			if(w2 < 0)
				continue;
			//if (w0 >= 0 && w1 >= 0 && w2 >= 0) { // If p is on or inside all edges, render pixel.
				bitmapTarget[y][x]		= value;
				++pixelsDrawn;
			//}
		}
		return pixelsDrawn;
	}

	tplt<tpnm _tCoord>
	static	::gpk::error_t	drawTriangle		(const ::gpk::n2u32 & targetMetrics, const ::gpk::tri2<_tCoord> & triangle, ::gpk::apod<::gpk::n2i16> & out_Points)		{
		::gpk::n2i32				areaMin				= {(int32_t)::gpk::min(::gpk::min(triangle.A.x, triangle.B.x), triangle.C.x), (int32_t)::gpk::min(::gpk::min(triangle.A.y, triangle.B.y), triangle.C.y)};
		::gpk::n2i32				areaMax				= {(int32_t)::gpk::max(::gpk::max(triangle.A.x, triangle.B.x), triangle.C.x), (int32_t)::gpk::max(::gpk::max(triangle.A.y, triangle.B.y), triangle.C.y)};
		const int32_t				xStop				= ::gpk::min(areaMax.x, (int32_t)targetMetrics.x);
		int32_t						pixelsDrawn			= 0;
		for(int32_t y = ::gpk::max(areaMin.y, (int32_t)0), yStop = ::gpk::min(areaMax.y, (int32_t)targetMetrics.y); y < yStop; ++y)
		for(int32_t x = ::gpk::max(areaMin.x, (int32_t)0); x < xStop; ++x) {
			const ::gpk::n2i32			cellCurrent			= {x, y};
			// Determine barycentric coordinates
			int32_t						w0					= ::gpk::orient2d({triangle.B, triangle.A}, cellCurrent);	// ::gpk::orient2d({triangle.A, triangle.B}, cellCurrent);
			int32_t						w1					= ::gpk::orient2d({triangle.C, triangle.B}, cellCurrent);	// ::gpk::orient2d({triangle.B, triangle.C}, cellCurrent);
			int32_t						w2					= ::gpk::orient2d({triangle.A, triangle.C}, cellCurrent);	// ::gpk::orient2d({triangle.C, triangle.A}, cellCurrent);
			if(w0 < 0)
				continue;
			if(w1 < 0)
				continue;
			if(w2 < 0)
				continue;
			//if (w0 >= 0 && w1 >= 0 && w2 >= 0) { // If p is on or inside all edges, render pixel.
			out_Points.push_back({(int16_t)x, (int16_t)y});
			++pixelsDrawn;
			//}
		}
		return pixelsDrawn;
	}

	typedef	::gpk::error_t	(*gpk_raster_callback)	(void* bitmapTarget, const ::gpk::n2u32 & bitmapMetrics, const ::gpk::n2u32 & cellPos, const void* value);


	// Bresenham's line algorithm
	tplt<tpnm _tCoord, tpnm _tColor>
	static	::gpk::error_t	rasterLine			(::gpk::grid<_tColor> & bitmapTarget, const _tColor & value, const ::gpk::line2<_tCoord> & line, gpk_raster_callback callback)				{
		::gpk::n2f32				A					= line.A.f2_t();
		::gpk::n2f32				B					= line.B.f2_t();
		const bool					steep				= (fabs(B.y - A.y) > fabs(B.x - A.x));
		if(steep){
			::gpk::swap(A.x, A.y);
			::gpk::swap(B.x, B.y);
		}
		if(A.x > B.x) {
			::gpk::swap(A.x, B.x);
			::gpk::swap(A.y, B.y);
		}
		const ::gpk::n2f32			d					= {B.x - A.x, (float)fabs(B.y - A.y)};
		float						error				= d.x / 2.0f;
		const int32_t				ystep				= (A.y < B.y) ? 1 : -1;
		int32_t						y					= (int32_t)A.y;
		int32_t						pixelsDrawn			= 0;
		if(steep) {
			for(int32_t x = (int32_t)A.x, xStop = (int32_t)B.x; x < xStop; ++x) {
				if(::gpk::in_range(x, (int32_t)0, (int32_t)bitmapTarget.metrics().y) && ::gpk::in_range(y, (int32_t)0, (int32_t)bitmapTarget.metrics().x)) {
					callback(bitmapTarget.begin(), bitmapTarget.metrics(), {(uint32_t)x, (uint32_t)y}, &value);
					++pixelsDrawn;
				}
				error					-= d.y;
				if(error < 0) {
					y						+= ystep;
					error					+= d.x;
				}
			}
		}
		else {
			for(int32_t x = (int32_t)A.x, xStop = (int32_t)B.x; x < xStop; ++x) {
				if(::gpk::in_range(y, (int32_t)0, (int32_t)bitmapTarget.metrics().y) && ::gpk::in_range(x, (int32_t)0, (int32_t)bitmapTarget.metrics().x)) {
					callback(bitmapTarget.begin(), bitmapTarget.metrics(), {(uint32_t)x, (uint32_t)y}, &value);
					++pixelsDrawn;
				}
				error					-= d.y;
				if(error < 0) {
					y						+= ystep;
					error					+= d.x;
				}
			}
		}
		return pixelsDrawn;
	}

	// Bresenham's line algorithm
	tplt<tpnm _tCoord, tpnm _tColor>
	static	::gpk::error_t	drawLine			(::gpk::grid<_tColor> & target, const _tColor & value, const ::gpk::line2<_tCoord> & line)				{
		::gpk::n2f32				A					= line.A.f2_t();
		::gpk::n2f32				B					= line.B.f2_t();
		if(line.A.x == line.B.x)
			return ::gpk::drawLineVertical(target, value, line.A.x, line.A.y, line.B.y);
		else if(line.A.y == line.B.y)
			return ::gpk::drawLineHorizontal(target, value, line.A.y, line.A.x, line.B.x);

		const bool					steep				= (fabs(B.y - A.y) > fabs(B.x - A.x));
		if(steep){
			::gpk::swap(A.x, A.y);
			::gpk::swap(B.x, B.y);
		}
		if(A.x > B.x) {
			::gpk::swap(A.x, B.x);
			::gpk::swap(A.y, B.y);
		}
		const ::gpk::n2f32			d					= {B.x - A.x, (float)fabs(B.y - A.y)};
		float						error				= d.x / 2.0f;
		const int32_t				ystep				= (A.y < B.y) ? 1 : -1;
		int32_t						y					= (int32_t)A.y;
		int32_t						pixelsDrawn			= 0;
		if(steep) {
			for(int32_t x = (int32_t)A.x, xStop = (int32_t)B.x; x < xStop; ++x) {
				if(::gpk::in_range(x, (int32_t)0, (int32_t)target.metrics().y) && ::gpk::in_range(y, (int32_t)0, (int32_t)target.metrics().x)) {
					target[x][y]				= value;
					++pixelsDrawn;
				}
				error					-= d.y;
				if(error < 0) {
					y						+= ystep;
					error					+= d.x;
				}
			}
		}
		else {
			for(int32_t x = (int32_t)A.x, xStop = (int32_t)B.x; x < xStop; ++x) {
				if(::gpk::in_range(y, (int32_t)0, (int32_t)target.metrics().y) && ::gpk::in_range(x, (int32_t)0, (int32_t)target.metrics().x)) {
					target[y][x]				= value;
					++pixelsDrawn;
				}
				error					-= d.y;
				if(error < 0) {
					y						+= ystep;
					error					+= d.x;
				}
			}
		}
		return pixelsDrawn;
	}

	// Bresenham's line algorithm
	tplt<tpnm _tCoord>
	static	::gpk::error_t	drawLine			(const ::gpk::n2u16 & targetMetrics, const ::gpk::line2<_tCoord> & line, ::gpk::apod<::gpk::n2i16> & out_Points)				{
		::gpk::n2f32				A					= line.A.f2_t();
		::gpk::n2f32				B					= line.B.f2_t();
		const bool					steep				= (fabs(B.y - A.y) > fabs(B.x - A.x));
		if(steep){
			::gpk::swap(A.x, A.y);
			::gpk::swap(B.x, B.y);
		}
		if(A.x > B.x) {
			::gpk::swap(A.x, B.x);
			::gpk::swap(A.y, B.y);
		}
		const ::gpk::n2f32			d					= {B.x - A.x, (float)fabs(B.y - A.y)};
		float						error				= d.x / 2.0f;
		const int16_t				ystep				= (A.y < B.y) ? 1 : -1;
		int16_t						y					= (int16_t)A.y;
		int32_t						pixelsDrawn			= 0;
		if(steep) {
			for(int16_t x = (int16_t)A.x, xStop = (int16_t)B.x; x < xStop; ++x) {
				if(::gpk::in_range(x, (int16_t)0, (int16_t)targetMetrics.y) && ::gpk::in_range(y, (int16_t)50, (int16_t)targetMetrics.x)) {
					out_Points.push_back({y, x});
					++pixelsDrawn;
				}
				error					-= d.y;
				if(error < 0) {
					y						+= ystep;
					error					+= d.x;
				}
			}
		}
		else {
			for(int16_t x = (int16_t)A.x, xStop = (int16_t)B.x; x < xStop; ++x) {
				if(::gpk::in_range(y, (int16_t)0, (int16_t)targetMetrics.y) && ::gpk::in_range(x, (int16_t)0, (int16_t)targetMetrics.x)) {
					out_Points.push_back({x, y});
					++pixelsDrawn;
				}
				error					-= d.y;
				if(error < 0) {
					y						+= ystep;
					error					+= d.x;
				}
			}
		}
		return pixelsDrawn;
	}

	// Bresenham's line algorithm
	tplt<tpnm _tCoord>
	static	::gpk::error_t	drawLine			(const ::gpk::n2u16 & targetMetrics, const ::gpk::line3<_tCoord> & line, ::gpk::apod<::gpk::n2i16> & out_Points)				{
		return drawLine(targetMetrics, ::gpk::line2<_tCoord>{{line.A.x, line.A.y}, {line.B.x, line.B.y}}, out_Points);
	}
} // namespace

#endif // BITMAP_TARGET_H_23627
