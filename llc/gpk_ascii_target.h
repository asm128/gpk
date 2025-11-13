#include "gpk_ascii_color.h"
#include "gpk_image.h"
#include "gpk_color.h"
#include "gpk_tri2.h"
#include "gpk_line2.h"
#include "gpk_circle.h"
#include "gpk_rect2.h"
#include "gpk_swap.h"

#ifndef GPK_ASCII_TARGET_H_23627
#define GPK_ASCII_TARGET_H_23627

namespace gpk
{
	struct SASCIITarget {
		::gpk::imgu8			Characters			= {};
		::gpk::imgu16			Colors				= {};

		inxp	::gpk::n2u2_t	metrics				()	const	noexcept	{ return Characters.metrics(); }
	};

	::gpk::error_t			asciiTargetClear	(::gpk::SASCIITarget & target, uint8_t character = ' ', uint16_t color = ASCII_COLOR_WHITE);

	// ------------------------------------------------------
#pragma pack(push, 1)
	struct SASCIICell {
		uint8_t					Character	;
		uint16_t				Color		;
	};
#pragma pack(pop)

	stin	::gpk::error_t	drawRectangle		(::gpk::SASCIITarget & asciiTarget, const ::gpk::SASCIICell & value, const ::gpk::rect2<int32_t>& rectangle)	{
		for(s2_t y = ::gpk::max((int32_t)0, (int32_t)rectangle.Offset.y), yStop = ::gpk::min((int32_t)(rectangle.Offset.y + rectangle.Size.y), (int32_t)asciiTarget.metrics().y); y < yStop; ++y)
		for(s2_t x = ::gpk::max((int32_t)0, (int32_t)rectangle.Offset.x), xStop = ::gpk::min((int32_t)(rectangle.Offset.x + rectangle.Size.x), (int32_t)asciiTarget.metrics().x); x < xStop; ++x) {
			asciiTarget.Characters	[y][x]											= value.Character;
			asciiTarget.Colors		[y][x]											= value.Color;
		}
		return 0;
	}

	stin	::gpk::error_t	drawCircle			(::gpk::SASCIITarget & asciiTarget, const ::gpk::SASCIICell & value, const ::gpk::circlei32 & circle)			{
		for(s2_t y = ::gpk::max((int32_t)0, (int32_t)(circle.Center.y - circle.Radius)), yStop = ::gpk::min((int32_t)(circle.Center.y + circle.Radius), (int32_t)asciiTarget.metrics().y); y < yStop; ++y)
		for(s2_t x = ::gpk::max((int32_t)0, (int32_t)(circle.Center.x - circle.Radius)), xStop = ::gpk::min((int32_t)(circle.Center.x + circle.Radius), (int32_t)asciiTarget.metrics().x); x < xStop; ++x) {
			::gpk::n2s2_c				cellCurrent			= {x, y};
			double						distance			= (cellCurrent - circle.Center).Length();
			if(distance < circle.Radius) {
				asciiTarget.Characters	[y][x]					= value.Character;
				asciiTarget.Colors		[y][x]					= value.Color;
			}
		}
		return 0;
	}

	// A good article on this kind of triangle rasterization: https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/
	tplt<tpnm _tCoord>
	stainli	::gpk::error_t	drawTriangle		(::gpk::SASCIITarget & asciiTarget, const ::gpk::SASCIICell & value, const ::gpk::tri2<_tCoord>& triangle)		{
		::gpk::n2s2_c			areaMin				= {(s2_t)::gpk::min(::gpk::min(triangle.A.x, triangle.B.x), triangle.C.x), (s2_t)::gpk::min(::gpk::min(triangle.A.y, triangle.B.y), triangle.C.y)};
		::gpk::n2s2_c			areaMax				= {(s2_t)::gpk::max(::gpk::max(triangle.A.x, triangle.B.x), triangle.C.x), (s2_t)::gpk::max(::gpk::max(triangle.A.y, triangle.B.y), triangle.C.y)};
		for(s2_t y = ::gpk::max((s2_t)areaMin.y, (s2_t)0), yStop = ::gpk::min(areaMax.y, (s2_t)asciiTarget.metrics().y); y < yStop; ++y)
		for(s2_t x = ::gpk::max((s2_t)areaMin.x, (s2_t)0), xStop = ::gpk::min(areaMax.x, (s2_t)asciiTarget.metrics().x); x < xStop; ++x) {
			::gpk::n2s2_c			cellCurrent			= {x, y};
			// Determine barycentric coordinates
			int						w0					= ::gpk::orient2d({triangle.A, triangle.B}, cellCurrent);
			int						w1					= ::gpk::orient2d({triangle.B, triangle.C}, cellCurrent);
			int						w2					= ::gpk::orient2d({triangle.C, triangle.A}, cellCurrent);
			if (w0 >= 0 && w1 >= 0 && w2 >= 0) { // If p is on or inside all edges, render pixel.
				asciiTarget.Characters	[y][x]	= value.Character;
				asciiTarget.Colors		[y][x]	= value.Color;
			}
		}
		return 0;
	}


	// Bresenham's line algorithm
	tplt<tpnm _tCoord>
	static	::gpk::error_t	drawLine			(::gpk::SASCIITarget & asciiTarget, const ::gpk::SASCIICell & value, const ::gpk::line2<_tCoord>& line)				{
		float						x1					= (float)line.A.x
			,						y1					= (float)line.A.y
			,						x2					= (float)line.B.x
			,						y2					= (float)line.B.y
			;
		const bool					steep				= (fabs(y2 - y1) > fabs(x2 - x1));
		if(steep){
			::gpk::swap(x1, y1);
			::gpk::swap(x2, y2);
		}
		if(x1 > x2) {
			::gpk::swap(x1, x2);
			::gpk::swap(y1, y2);
		}
		const float			dx					= x2 - x1;
		const float			dy					= fabs(y2 - y1);
		float				error				= dx / 2.0f;
		s2_c				ystep				= (y1 < y2) ? 1 : -1;
		s2_t				y					= (int32_t)y1;
		if(steep) {
			for(s2_t x = (s2_t)x1, xStop = (int32_t)x2; x < xStop; ++x) {
				if( false == ::gpk::in_range(x, (int32_t)0, (int32_t)asciiTarget.metrics().y)
				 || false == ::gpk::in_range(y, (int32_t)0, (int32_t)asciiTarget.metrics().x)
				 )
					continue;
				asciiTarget.Characters	[x][y]	= value.Character;
				asciiTarget.Colors		[x][y]	= value.Color;
				error					-= dy;
				if(error < 0) {
					y						+= ystep;
					error					+= dx;
				}
			}
		}
		else {
			for(s2_t x = (s2_t)x1, xStop = (s2_t)x2; x < xStop; ++x) {
				if( false == ::gpk::in_range(y, (s2_t)0, (s2_t)asciiTarget.metrics().y)
				 || false == ::gpk::in_range(x, (s2_t)0, (s2_t)asciiTarget.metrics().x)
				 )
					continue;
				asciiTarget.Characters	[y][x]	= value.Character;
				asciiTarget.Colors		[y][x]	= value.Color;
				error							-= dy;
				if(error < 0) {
					y								+= ystep;
					error							+= dx;
				}
			}
		}
		return 0;
	}
}

#endif // GPK_ASCII_TARGET_H_23627
