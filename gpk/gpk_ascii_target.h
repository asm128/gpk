#include "gpk_ascii_color.h"
#include "gpk_image.h"
#include "gpk_color.h"
#include <memory> // this is required for ::std::swap()

#ifndef GPK_ASCII_TARGET_H_29874982734
#define GPK_ASCII_TARGET_H_29874982734

namespace gpk
{
	struct SASCIITarget {
								::gpk::SImage<uint8_t>						Characters									= {};
								::gpk::SImage<uint16_t>						Colors										= {};

		inline constexpr		::gpk::SCoord2<uint32_t>					metrics										()																	const	noexcept	{ return Characters.metrics(); }
	};

							::gpk::error_t									asciiTargetClear							(::gpk::SASCIITarget& target, uint8_t character = ' ', uint16_t color = ASCII_COLOR_WHITE);

	// ------------------------------------------------------
#pragma pack( push, 1 )
	struct SASCIICell {
								uint8_t											Character	;
								uint16_t										Color		;
	};
#pragma pack( pop )

	static inline			::gpk::error_t									drawRectangle								(::gpk::SASCIITarget& asciiTarget, const ::gpk::SASCIICell& value, const ::gpk::SRectangle2D<int32_t>& rectangle)	{
		for(int32_t y = (int32_t)::gpk::max(0, rectangle.Offset.y), yStop = ::gpk::min((int32_t)(rectangle.Offset.y + rectangle.Size.y), (int32_t)asciiTarget.metrics().y); y < yStop; ++y)
		for(int32_t x = (int32_t)::gpk::max(0, rectangle.Offset.x), xStop = ::gpk::min((int32_t)(rectangle.Offset.x + rectangle.Size.x), (int32_t)asciiTarget.metrics().x); x < xStop; ++x) {
			asciiTarget.Characters	[y][x]											= value.Character;
			asciiTarget.Colors		[y][x]											= value.Color;
		}
		return 0;
	}

	static inline			::gpk::error_t									drawCircle									(::gpk::SASCIITarget& asciiTarget, const ::gpk::SASCIICell& value, const ::gpk::SCircle2D<int32_t>& circle)			{
		for(int32_t y = ::gpk::max(0, (int32_t)(circle.Center.y - circle.Radius)), yStop = ::gpk::min((int32_t)(circle.Center.y + circle.Radius), (int32_t)asciiTarget.metrics().y); y < yStop; ++y)
		for(int32_t x = ::gpk::max(0, (int32_t)(circle.Center.x - circle.Radius)), xStop = ::gpk::min((int32_t)(circle.Center.x + circle.Radius), (int32_t)asciiTarget.metrics().x); x < xStop; ++x) {
			::gpk::SCoord2<int32_t>														cellCurrent									= {x, y};
			double																		distance									= (cellCurrent - circle.Center).Length();
			if(distance < circle.Radius) {
				asciiTarget.Characters	[y][x]											= value.Character;
				asciiTarget.Colors		[y][x]											= value.Color;
			}
		}
		return 0;
	}

	// A good article on this kind of triangle rasterization: https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/
	template<typename _tCoord>
	static inline			::gpk::error_t									drawTriangle								(::gpk::SASCIITarget& asciiTarget, const ::gpk::SASCIICell& value, const ::gpk::STriangle2D<_tCoord>& triangle)		{
		::gpk::SCoord2		<int32_t>												areaMin										= {(int32_t)::gpk::min(::gpk::min(triangle.A.x, triangle.B.x), triangle.C.x), (int32_t)::gpk::min(::gpk::min(triangle.A.y, triangle.B.y), triangle.C.y)};
		::gpk::SCoord2		<int32_t>												areaMax										= {(int32_t)::gpk::max(::gpk::max(triangle.A.x, triangle.B.x), triangle.C.x), (int32_t)::gpk::max(::gpk::max(triangle.A.y, triangle.B.y), triangle.C.y)};
		for(int32_t y = ::gpk::max(areaMin.y, 0), yStop = ::gpk::min(areaMax.y, (int32_t)asciiTarget.metrics().y); y < yStop; ++y)
		for(int32_t x = ::gpk::max(areaMin.x, 0), xStop = ::gpk::min(areaMax.x, (int32_t)asciiTarget.metrics().x); x < xStop; ++x) {
			const ::gpk::SCoord2<int32_t>												cellCurrent									= {x, y};
			// Determine barycentric coordinates
			int																			w0											= ::gpk::orient2d({triangle.A, triangle.B}, cellCurrent);
			int																			w1											= ::gpk::orient2d({triangle.B, triangle.C}, cellCurrent);
			int																			w2											= ::gpk::orient2d({triangle.C, triangle.A}, cellCurrent);
			if (w0 >= 0 && w1 >= 0 && w2 >= 0) { // If p is on or inside all edges, render pixel.
				asciiTarget.Characters	[y][x]											= value.Character;
				asciiTarget.Colors		[y][x]											= value.Color;
			}
		}
		return 0;
	}

	// Bresenham's line algorithm
	template<typename _tCoord>
	static					::gpk::error_t									drawLine									(::gpk::SASCIITarget& asciiTarget, const ::gpk::SASCIICell& value, const ::gpk::SLine2D<_tCoord>& line)				{
		float																		x1											= (float)line.A.x
			,																		y1											= (float)line.A.y
			,																		x2											= (float)line.B.x
			,																		y2											= (float)line.B.y
			;
		const bool																	steep										= (fabs(y2 - y1) > fabs(x2 - x1));
		if(steep){
			::std::swap(x1, y1);
			::std::swap(x2, y2);
		}
		if(x1 > x2) {
			::std::swap(x1, x2);
			::std::swap(y1, y2);
		}
		const float																	dx											= x2 - x1;
		const float																	dy											= fabs(y2 - y1);
		float																		error										= dx / 2.0f;
		const int32_t																ystep										= (y1 < y2) ? 1 : -1;
		int32_t																		y											= (int32_t)y1;
		if(steep) {
			for(int32_t x = (int32_t)x1, xStop = (int32_t)x2; x < xStop; ++x) {
				if( false == ::gpk::in_range(x, 0, (int32_t)asciiTarget.metrics().y)
				 || false == ::gpk::in_range(y, 0, (int32_t)asciiTarget.metrics().x)
				 )
					continue;
				asciiTarget.Characters	[x][y]											= value.Character;
				asciiTarget.Colors		[x][y]											= value.Color;
				error																	-= dy;
				if(error < 0) {
					y																		+= ystep;
					error																	+= dx;
				}
			}
		}
		else {
			for(int32_t x = (int32_t)x1, xStop = (int32_t)x2; x < xStop; ++x) {
				if( false == ::gpk::in_range(y, 0, (int32_t)asciiTarget.metrics().y)
				 || false == ::gpk::in_range(x, 0, (int32_t)asciiTarget.metrics().x)
				 )
					continue;
				asciiTarget.Characters	[y][x]											= value.Character;
				asciiTarget.Colors		[y][x]											= value.Color;
				error																	-= dy;
				if(error < 0) {
					y																		+= ystep;
					error																	+= dx;
				}
			}
		}
		return 0;
	}
}

#endif // GPK_ASCII_TARGET_H_29874982734
