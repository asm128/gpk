#include "gpk_line2.h"
#include "gpk_rect2.h"
#include "gpk_sphere.h"

#include <limits>

#ifndef GPK_COLLISION_H_23627
#define GPK_COLLISION_H_23627

namespace gpk
{
	tplt<tpnm T>
	bool	collisionCoordinateCast	(f3_t value, T& output) {
		if(false == isfinite(value) || value < f3_t(::std::numeric_limits<T>::lowest()) || value > f3_t(::std::numeric_limits<T>::max()))
			rtrn false;
		if(::std::numeric_limits<T>::is_integer) {
			f3_c nearest = value < 0 ? ceil(value - 0.5) : floor(value + 0.5);
			if(abs(value - nearest) > math_epsilon)
				rtrn false;
			value = nearest;
		}
		output = T(value);
		rtrn true;
	}

	// Returns one for a unique intersection, zero for parallel or coincident lines and -1 when the result cannot be represented by TAxis.
	tplt<tpnm TAxis>
	err_t	line_line_intersect	(cnst line2<TAxis>& line1, cnst line2<TAxis>& line2, n2<TAxis>& intersection) {
		f3_c determinant1 = determinant(line1);
		f3_c determinant2 = determinant(line2);
		f3_c x1mx2 = f3_t(line1.A.x) - line1.B.x;
		f3_c y1my2 = f3_t(line1.A.y) - line1.B.y;
		f3_c x3mx4 = f3_t(line2.A.x) - line2.B.x;
		f3_c y3my4 = f3_t(line2.A.y) - line2.B.y;
		f3_c denominator = determinant(x1mx2, y1my2, x3mx4, y3my4);
		if(0 == denominator)
			rtrn 0;
		n2<TAxis> result = {};
		if(false == collisionCoordinateCast(determinant(determinant1, x1mx2, determinant2, x3mx4) / denominator, result.x))
			rtrn -1;
		if(false == collisionCoordinateCast(determinant(determinant1, y1my2, determinant2, y3my4) / denominator, result.y))
			rtrn -1;
		intersection = result;
		rtrn 1;
	}

	// A ray starts inside its sphere at t == 0. Direction length changes the parameter scale but not the intersection point.
	tplt<tpnm TAxis>
	err_t	intersectRaySphere	(cnst n3<TAxis>& position, cnst n3<TAxis>& direction, cnst sphere<TAxis>& sphereToIntersect, f2_t& t, n3<TAxis>& intersection) {
		if(sphereToIntersect.Radius < 0)
			rtrn -1;
		n3f3_c positionF3 = position.f3();
		n3f3_c directionF3 = direction.f3();
		n3f3_c distance = positionF3 - sphereToIntersect.Center.f3();
		f3_c directionLengthSquared = directionF3.Dot(directionF3);
		f3_c projection = distance.Dot(directionF3);
		f3_c distanceFromSurface = distance.Dot(distance) - sphereToIntersect.Radius * sphereToIntersect.Radius;
		if(distanceFromSurface > 0 && projection >= 0)
			rtrn 0;
		f3_t parameter = 0;
		if(distanceFromSurface > 0) {
			if(0 == directionLengthSquared)
				rtrn 0;
			f3_c discriminant = projection * projection - directionLengthSquared * distanceFromSurface;
			if(discriminant < 0)
				rtrn 0;
			parameter = (-projection - sqrt(discriminant)) / directionLengthSquared;
			if(parameter < 0)
				rtrn 0;
		}
		if(false == isfinite(parameter) || parameter > ::std::numeric_limits<f2_t>::max())
			rtrn -1;
		n3f3_c point = positionF3 + directionF3 * parameter;
		n3<TAxis> result = {};
		if(false == collisionCoordinateCast(point.x, result.x) || false == collisionCoordinateCast(point.y, result.y) || false == collisionCoordinateCast(point.z, result.z))
			rtrn -1;
		t = f2_t(parameter);
		intersection = result;
		rtrn 1;
	}

	tplt<tpnm TAxis>
	err_t	point_in_segment	(cnst line2<TAxis>& segment, cnst n2<TAxis>& point) {
		f3_c segmentX = f3_t(segment.B.x) - segment.A.x;
		f3_c segmentY = f3_t(segment.B.y) - segment.A.y;
		f3_c pointX = f3_t(point.x) - segment.A.x;
		f3_c pointY = f3_t(point.y) - segment.A.y;
		f3_c crossProduct = segmentX * pointY - segmentY * pointX;
		f3_c crossScale = abs(segmentX * pointY) + abs(segmentY * pointX) + 1;
		if(abs(crossProduct) > math_epsilon * crossScale)
			rtrn 0;
		f3_c minimumX = min(f3_t(segment.A.x), f3_t(segment.B.x));
		f3_c minimumY = min(f3_t(segment.A.y), f3_t(segment.B.y));
		f3_c maximumX = max(f3_t(segment.A.x), f3_t(segment.B.x));
		f3_c maximumY = max(f3_t(segment.A.y), f3_t(segment.B.y));
		rtrn point.x >= minimumX - math_epsilon && point.y >= minimumY - math_epsilon && point.x <= maximumX + math_epsilon && point.y <= maximumY + math_epsilon;
	}

	tplt<tpnm TAxis>
	err_t	line_segment_intersect	(cnst line2<TAxis>& line, cnst line2<TAxis>& segment, n2<TAxis>& intersection) {
		n2<TAxis> result = {};
		err_c collision = line_line_intersect(line, segment, result);
		if(1 != collision)
			rtrn collision;
		if(0 == point_in_segment(segment, result))
			rtrn 0;
		intersection = result;
		rtrn 1;
	}

	tplt<tpnm TAxis>
	err_t	segment_segment_intersect	(cnst line2<TAxis>& segment1, cnst line2<TAxis>& segment2, n2<TAxis>& intersection) {
		n2<TAxis> result = {};
		err_c collision = line_segment_intersect(segment1, segment2, result);
		if(1 != collision)
			rtrn collision;
		if(0 == point_in_segment(segment1, result))
			rtrn 0;
		intersection = result;
		rtrn 1;
	}

	tplt<tpnm TAxis>
	err_t	buildAABBSegments	(cnst n2<TAxis>& center, cnst TAxis halfSize, line2<TAxis>& left, line2<TAxis>& top, line2<TAxis>& right, line2<TAxis>& bottom) {
		if(halfSize < 0)
			rtrn -1;
		n2<TAxis> bottomLeft = {}, topLeft = {}, topRight = {}, bottomRight = {};
		if(false == collisionCoordinateCast(f3_t(center.x) - halfSize, bottomLeft.x) || false == collisionCoordinateCast(f3_t(center.y) - halfSize, bottomLeft.y))
			rtrn -1;
		if(false == collisionCoordinateCast(f3_t(center.x) - halfSize, topLeft.x) || false == collisionCoordinateCast(f3_t(center.y) + halfSize, topLeft.y))
			rtrn -1;
		if(false == collisionCoordinateCast(f3_t(center.x) + halfSize, topRight.x) || false == collisionCoordinateCast(f3_t(center.y) + halfSize, topRight.y))
			rtrn -1;
		if(false == collisionCoordinateCast(f3_t(center.x) + halfSize, bottomRight.x) || false == collisionCoordinateCast(f3_t(center.y) - halfSize, bottomRight.y))
			rtrn -1;
		left = {bottomLeft, topLeft};
		top = {topLeft, topRight};
		right = {topRight, bottomRight};
		bottom = {bottomRight, bottomLeft};
		rtrn 0;
	}

	tplt<tpnm TAxis>
	err_t	buildAABBSegments	(cnst rect2<TAxis>& rectangle, line2<TAxis>& left, line2<TAxis>& top, line2<TAxis>& right, line2<TAxis>& bottom) {
		if(rectangle.Size.x < 0 || rectangle.Size.y < 0)
			rtrn -1;
		n2<TAxis> bottomLeft = rectangle.Offset, topLeft = {}, topRight = {}, bottomRight = {};
		if(false == collisionCoordinateCast(f3_t(rectangle.Offset.x), topLeft.x) || false == collisionCoordinateCast(f3_t(rectangle.Offset.y) + rectangle.Size.y, topLeft.y))
			rtrn -1;
		if(false == collisionCoordinateCast(f3_t(rectangle.Offset.x) + rectangle.Size.x, topRight.x) || false == collisionCoordinateCast(f3_t(rectangle.Offset.y) + rectangle.Size.y, topRight.y))
			rtrn -1;
		if(false == collisionCoordinateCast(f3_t(rectangle.Offset.x) + rectangle.Size.x, bottomRight.x) || false == collisionCoordinateCast(f3_t(rectangle.Offset.y), bottomRight.y))
			rtrn -1;
		left = {bottomLeft, topLeft};
		top = {topLeft, topRight};
		right = {topRight, bottomRight};
		bottom = {bottomRight, bottomLeft};
		rtrn 0;
	}
}

#endif // GPK_COLLISION_H_23627


