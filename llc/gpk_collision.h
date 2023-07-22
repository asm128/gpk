// Tip: Best viewed with zoom level at 81%.
// Tip: Hold Left ALT + SHIFT while tapping or holding the arrow keys in order to select multiple columns and write on them at once.
//		Also useful for copy & paste operations in which you need to copy a bunch of variable or function names and you can't afford the time of copying them one by one.
#include "gpk_log.h"
#include "gpk_line2.h"
#include "gpk_sphere.h"

#ifndef GPK_COLLISION_H_23627
#define GPK_COLLISION_H_23627

namespace gpk
{
// Calculate intersection of two lines. return 1 if found, 0 if not found or -1 on error
	tplt<tpnm TAxis>
	::gpk::error_t		line_line_intersect
		( const ::gpk::line2<TAxis>	& line1
		, const ::gpk::line2<TAxis>	& line2
		, ::gpk::n2<TAxis>		& out_intersect_point
		)
	{
		double					detL1			= ::gpk::determinant(line1);
		double					detL2			= ::gpk::determinant(line2);

		double					x1mx2			= line1.A.x - line1.B.x;
		double					y1my2			= line1.A.y - line1.B.y;

		double					x3mx4			= line2.A.x - line2.B.x;
		double					y3my4			= line2.A.y - line2.B.y;

		double					xnom			= ::gpk::determinant(detL1, x1mx2, detL2, x3mx4);
		double					ynom			= ::gpk::determinant(detL1, y1my2, detL2, y3my4);
		double					denom			= ::gpk::determinant(x1mx2, y1my2, x3mx4, y3my4);
		if(denom == 0.0) { // Lines don't seem to cross
			out_intersect_point.x	= NAN;
			out_intersect_point.y	= NAN;
			return 0;
		}
		out_intersect_point.x	= (TAxis)(xnom / denom);
		out_intersect_point.y	= (TAxis)(ynom / denom);
		ree_if(!isfinite(out_intersect_point.x)
			|| !isfinite(out_intersect_point.y)
			, "%s", "Probably a numerical issue.");
		return 1; //All OK
	}

	// Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting,
	// returns t value of intersection and intersection point q
	tplt<tpnm TAxis>
	int32_t				intersectRaySphere
		( const ::gpk::n3<TAxis>		& position
		, const ::gpk::n3<TAxis>		& direction
		, const ::gpk::sphere<TAxis>	& sphere
		, float							& t
		, ::gpk::n3<TAxis>				& q
		) {
		const ::gpk::n3<TAxis>	m			= position - sphere.Center;
		double					b			= m.Dot(direction);
		double					c			= m.Dot(m) - sphere.Radius * sphere.Radius;

		if (c > 0.0f && b > 0.0f)	// Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
			return 0;
		double					discr		= b * b - c;

		if (discr < 0.0f)	// A negative discriminant corresponds to ray missing sphere
			return 0;

		t					= (float)(-b - sqrt(discr));	// Ray now found to intersect sphere, compute smallest t value of intersection
		if (t < 0.0f)	// If t is negative, ray started inside sphere so clamped t to zero
			t					= 0.0f;

		q					= position + direction * t;
		return 1;
	}

	tplt<tpnm TAxis>
	::gpk::error_t		point_in_segment
		( const ::gpk::line2<TAxis>	& segment
		, const ::gpk::n2<TAxis>	& point
		)
	{
		return (	point.x >=	::gpk::min(segment.A.x, segment.B.x)
				&&	point.y >=	::gpk::min(segment.A.y, segment.B.y)
				&&	point.x <=	::gpk::max(segment.A.x, segment.B.x)
				&&	point.y <=	::gpk::max(segment.A.y, segment.B.y)
				) ? 1 :  0;
	}
	// Calculate intersection of two lines. return 1 if found, 0 if not found or -1 on error
	tplt<tpnm TAxis>
	::gpk::error_t		line_segment_intersect
		( const ::gpk::line2<TAxis>	& line
		, const ::gpk::line2<TAxis>	& segment
		, ::gpk::n2<TAxis>			& out_intersect_point
		)
	{
		::gpk::error_t			collision			= line_line_intersect(line, segment, out_intersect_point);
		return (1 == collision) ? point_in_segment(segment, out_intersect_point) : collision;
	}

	// Calculate intersection of two lines. return 1 if found, 0 if not found or -1 on error
	tplt<tpnm TAxis>
	::gpk::error_t		segment_segment_intersect
		( const ::gpk::line2<TAxis>	& segment1
		, const ::gpk::line2<TAxis>	& segment2
		, ::gpk::n2<TAxis>		& out_intersect_point
		)
	{
		::gpk::error_t			collision			= line_segment_intersect(segment1, segment2, out_intersect_point);
		return (1 == collision) ? point_in_segment(segment1, out_intersect_point) : collision;
	}


	tplt<tpnm TAxis>
	::gpk::error_t		buildAABBSegments
		( const ::gpk::n2<TAxis>	& center
		, const TAxis				halfSizeBox
		, ::gpk::line2<TAxis>		& left
		, ::gpk::line2<TAxis>		& top
		, ::gpk::line2<TAxis>		& right
		, ::gpk::line2<TAxis>		& bottom
		)
	{
		typedef	::gpk::n2<TAxis>	TCoord2;
		left				= {center + TCoord2{-halfSizeBox, halfSizeBox}, center + TCoord2{-halfSizeBox,-halfSizeBox}};
		top					= {center + TCoord2{ halfSizeBox, halfSizeBox}, center + TCoord2{-halfSizeBox, halfSizeBox}};
		right				= {center + TCoord2{ halfSizeBox, halfSizeBox}, center + TCoord2{ halfSizeBox,-halfSizeBox}};
		bottom				= {center + TCoord2{ halfSizeBox,-halfSizeBox}, center + TCoord2{-halfSizeBox,-halfSizeBox}};
		return 0;
	}

	tplt<tpnm TAxis>
	::gpk::error_t		buildAABBSegments
		( const ::gpk::rect2<TAxis>	& rect
		, ::gpk::line2<TAxis>		& left
		, ::gpk::line2<TAxis>		& top
		, ::gpk::line2<TAxis>		& right
		, ::gpk::line2<TAxis>		& bottom
		)
	{
		typedef	::gpk::n2<TAxis>	TCoord2;
		left				= {rect.Offset										, rect.Offset + TCoord2{0			, rect.Size.y}};
		top					= {rect.Offset + TCoord2{0, rect.Size.y}			, rect.Offset + TCoord2{rect.Size.x	, rect.Size.y}};
		right				= {rect.Offset + TCoord2{rect.Size.x, rect.Size.y}	, rect.Offset + TCoord2{rect.Size.x, 0}};
		bottom				= {rect.Offset										, rect.Offset + TCoord2{rect.Size.x, 0}};
		return 0;
	}

}


#endif // GPK_COLLISION_H_23627
