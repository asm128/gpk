#include "gpk_array_pod.h"
#include "gpk_circle.h"
#include "gpk_color.h"
#include "gpk_grid.h"
#include "gpk_line2.h"
#include "gpk_line3.h"
#include "gpk_rect2.h"
#include "gpk_tri2.h"

#ifndef GPK_BITMAP_TARGET_H_23627
#define GPK_BITMAP_TARGET_H_23627

namespace gpk
{
	tplt<tpnm TCoord, tpnm TCell>
	err_t	drawPixelBrightness	(grid<TCell>& target, n2<TCoord> const& sourcePosition, TCell const& lightColor, f2_t factor, f3_t range) {
		if(range <= 0)
			rtrn 0;
		f3_c rangeUnit = 1.0 / n2f3_t{range, range}.Length();
		s3_c rangeCells = s3_t(range) + 1;
		err_t pixelsDrawn = 0;
		for(s3_t y = -rangeCells; y <= rangeCells; ++y)
		for(s3_t x = -rangeCells; x <= rangeCells; ++x) {
			if(0 == x && 0 == y)
				continue;
			f3_c targetX = f3_t(sourcePosition.x) + x;
			f3_c targetY = f3_t(sourcePosition.y) + y;
			if(targetX < 0 || targetY < 0 || targetX >= target.metrics().x || targetY >= target.metrics().y)
				continue;
			f3_c distanceFactor = sqrt(f3_t(x * x + y * y)) * rangeUnit;
			if(distanceFactor > 1.0)
				continue;
			n2u2_c cell = {u2_t(targetX), u2_t(targetY)};
			target[cell] = interpolate_linear(target[cell], lightColor, factor * (1.0 - distanceFactor));
			++pixelsDrawn;
		}
		rtrn pixelsDrawn;
	}

	tplt<tpnm TCoord, tpnm TCell>
	err_t	drawPixelLight		(grid<TCell>& target, n2<TCoord> const& sourcePosition, TCell const& lightColor, f2_t factor, f3_t range) {
		err_t pixelsDrawn = 0;
		f3_c targetX = sourcePosition.x;
		f3_c targetY = sourcePosition.y;
		if(targetX >= 0 && targetY >= 0 && targetX < target.metrics().x && targetY < target.metrics().y) {
			target[{u2_t(targetX), u2_t(targetY)}] = lightColor;
			++pixelsDrawn;
		}
		err_t brightnessResult = drawPixelBrightness(target, sourcePosition, lightColor, factor, range);
		if_fail_fe(brightnessResult);
		rtrn pixelsDrawn + brightnessResult;
	}

	tplt<tpnm TCell>
	err_t	drawLineVertical	(grid<TCell>& target, TCell const& value, s2_t x, s2_t y1, s2_t y2) {
		if(x < 0 || x >= s3_t(target.metrics().x) || 0 == target.metrics().y)
			rtrn 0;
		s3_t yStart = y1 < y2 ? y1 : y2;
		s3_t yStop = y1 > y2 ? y1 : y2;
		yStart = yStart < 0 ? 0 : yStart;
		yStop = yStop >= s3_t(target.metrics().y) ? s3_t(target.metrics().y) - 1 : yStop;
		if(yStart > yStop)
			rtrn 0;
		for(s3_t y = yStart; y <= yStop; ++y)
			target[u2_t(y)][u2_t(x)] = value;
		rtrn err_t(yStop - yStart + 1);
	}

	tplt<tpnm TCell>
	err_t	drawLineHorizontal	(grid<TCell>& target, TCell const& value, s2_t y, s2_t x1, s2_t x2) {
		if(y < 0 || y >= s3_t(target.metrics().y) || 0 == target.metrics().x)
			rtrn 0;
		s3_t xStart = x1 < x2 ? x1 : x2;
		s3_t xStop = x1 > x2 ? x1 : x2;
		xStart = xStart < 0 ? 0 : xStart;
		xStop = xStop >= s3_t(target.metrics().x) ? s3_t(target.metrics().x) - 1 : xStop;
		if(xStart > xStop)
			rtrn 0;
		for(s3_t x = xStart; x <= xStop; ++x)
			target[u2_t(y)][u2_t(x)] = value;
		rtrn err_t(xStop - xStart + 1);
	}

	tplt<tpnm TCoord, tpnm TCell>
	err_t	drawRectangleBorder	(grid<TCell>& target, TCell const& value, rect2<TCoord> const& rectangle) {
		if(0 == target.metrics().x || 0 == target.metrics().y)
			rtrn 0;
		s3_c left = s3_t(rectangle.Offset.x);
		s3_c top = s3_t(rectangle.Offset.y);
		s3_c width = s3_t(rectangle.Size.x);
		s3_c height = s3_t(rectangle.Size.y);
		if(width <= 0 || height <= 0)
			rtrn 0;
		s3_c right = left + width - 1;
		s3_c bottom = top + height - 1;
		s3_c xStart = left < 0 ? 0 : left;
		s3_c yStart = top < 0 ? 0 : top;
		s3_c xStop = right >= s3_t(target.metrics().x) ? s3_t(target.metrics().x) - 1 : right;
		s3_c yStop = bottom >= s3_t(target.metrics().y) ? s3_t(target.metrics().y) - 1 : bottom;
		if(xStart > xStop || yStart > yStop)
			rtrn 0;
		err_t pixelsDrawn = 0;
		if(top >= 0 && top < s3_t(target.metrics().y))
			for(s3_t x = xStart; x <= xStop; ++x) { target[u2_t(top)][u2_t(x)] = value; ++pixelsDrawn; }
		if(bottom != top && bottom >= 0 && bottom < s3_t(target.metrics().y))
			for(s3_t x = xStart; x <= xStop; ++x) { target[u2_t(bottom)][u2_t(x)] = value; ++pixelsDrawn; }
		if(left >= 0 && left < s3_t(target.metrics().x))
			for(s3_t y = yStart; y <= yStop; ++y) {
				if(y == top || y == bottom)
					continue;
				target[u2_t(y)][u2_t(left)] = value;
				++pixelsDrawn;
			}
		if(right != left && right >= 0 && right < s3_t(target.metrics().x))
			for(s3_t y = yStart; y <= yStop; ++y) {
				if(y == top || y == bottom)
					continue;
				target[u2_t(y)][u2_t(right)] = value;
				++pixelsDrawn;
			}
		rtrn pixelsDrawn;
	}

	tplt<tpnm TCoord, tpnm TCallback>
	err_t	rasterCircle		(n2u2_c& targetMetrics, circle<TCoord> const& circleToRaster, TCallback&& callback) {
		if(circleToRaster.Radius < 0 || 0 == targetMetrics.x || 0 == targetMetrics.y)
			rtrn 0;
		f3_c centerX = circleToRaster.Center.x;
		f3_c centerY = circleToRaster.Center.y;
		f3_c radiusSquared = circleToRaster.Radius * circleToRaster.Radius;
		s3_t xStart = s3_t(floor(centerX - circleToRaster.Radius));
		s3_t yStart = s3_t(floor(centerY - circleToRaster.Radius));
		s3_t xStop = s3_t(ceil(centerX + circleToRaster.Radius));
		s3_t yStop = s3_t(ceil(centerY + circleToRaster.Radius));
		xStart = xStart < 0 ? 0 : xStart;
		yStart = yStart < 0 ? 0 : yStart;
		xStop = xStop >= s3_t(targetMetrics.x) ? s3_t(targetMetrics.x) - 1 : xStop;
		yStop = yStop >= s3_t(targetMetrics.y) ? s3_t(targetMetrics.y) - 1 : yStop;
		if(xStart > xStop || yStart > yStop)
			rtrn 0;
		err_t pixelsDrawn = 0;
		for(s3_t y = yStart; y <= yStop; ++y)
		for(s3_t x = xStart; x <= xStop; ++x) {
			f3_c distanceX = x - centerX;
			f3_c distanceY = y - centerY;
			if(distanceX * distanceX + distanceY * distanceY > radiusSquared)
				continue;
			n2u2_c cell = {u2_t(x), u2_t(y)};
			err_t callbackResult = callback(cell);
			if_fail_fe(callbackResult);
			pixelsDrawn += callbackResult;
		}
		rtrn pixelsDrawn;
	}

	tplt<tpnm TCoord, tpnm TCell>
	err_t	drawCircle			(grid<TCell>& target, TCell const& value, circle<TCoord> const& circleToDraw) {
		rtrn rasterCircle(target.metrics(), circleToDraw, [&target, &value](n2u2_c& cell) -> err_t { target[cell] = value; rtrn 1; });
	}

	tplt<tpnm TCoord>
	err_t	drawCircle			(n2u2_c& targetMetrics, circle<TCoord> const& circleToDraw, apod<n2s2_t>& output) {
		if_true_fef(targetMetrics.x > 0x80000000U || targetMetrics.y > 0x80000000U, "Circle target cannot be represented by 32-bit signed coordinates: %u x %u.", targetMetrics.x, targetMetrics.y);
		apod<n2s2_t> points = {};
		err_t rasterResult = rasterCircle(targetMetrics, circleToDraw, [&points](n2u2_c& cell) -> err_t { if_fail_fe(points.push_back(cell.s2())); rtrn 1; });
		if_fail_fe(rasterResult);
		if_fail_fe(output.append(points));
		rtrn rasterResult;
	}

	tplt<tpnm TCoord, tpnm TCallback>
	err_t	rasterTriangle		(n2u2_c& targetMetrics, tri2<TCoord> const& triangle, TCallback&& callback) {
		if(0 == targetMetrics.x || 0 == targetMetrics.y)
			rtrn 0;
		n2f3_c pointA = triangle.A.f3();
		n2f3_c pointB = triangle.B.f3();
		n2f3_c pointC = triangle.C.f3();
		auto orient = [](n2f3_c& a, n2f3_c& b, n2f3_c& point) { rtrn (b.x - a.x) * (point.y - a.y) - (b.y - a.y) * (point.x - a.x); };
		f3_c area = orient(pointA, pointB, pointC);
		if(0 == area)
			rtrn 0;
		s3_t xStart = s3_t(floor(min(pointA.x, min(pointB.x, pointC.x))));
		s3_t yStart = s3_t(floor(min(pointA.y, min(pointB.y, pointC.y))));
		s3_t xStop = s3_t(ceil(max(pointA.x, max(pointB.x, pointC.x))));
		s3_t yStop = s3_t(ceil(max(pointA.y, max(pointB.y, pointC.y))));
		xStart = xStart < 0 ? 0 : xStart;
		yStart = yStart < 0 ? 0 : yStart;
		xStop = xStop >= s3_t(targetMetrics.x) ? s3_t(targetMetrics.x) - 1 : xStop;
		yStop = yStop >= s3_t(targetMetrics.y) ? s3_t(targetMetrics.y) - 1 : yStop;
		if(xStart > xStop || yStart > yStop)
			rtrn 0;
		err_t pixelsDrawn = 0;
		for(s3_t y = yStart; y <= yStop; ++y)
		for(s3_t x = xStart; x <= xStop; ++x) {
			n2f3_c point = {f3_t(x), f3_t(y)};
			f3_c weightA = orient(pointB, pointC, point);
			f3_c weightB = orient(pointC, pointA, point);
			f3_c weightC = orient(pointA, pointB, point);
			if(area > 0 ? (weightA < 0 || weightB < 0 || weightC < 0) : (weightA > 0 || weightB > 0 || weightC > 0))
				continue;
			n2u2_c cell = {u2_t(x), u2_t(y)};
			err_t callbackResult = callback(cell);
			if_fail_fe(callbackResult);
			pixelsDrawn += callbackResult;
		}
		rtrn pixelsDrawn;
	}

	tplt<tpnm TCoord, tpnm TCell>
	err_t	drawTriangle		(grid<TCell>& target, TCell const& value, tri2<TCoord> const& triangle) {
		rtrn rasterTriangle(target.metrics(), triangle, [&target, &value](n2u2_c& cell) -> err_t { target[cell] = value; rtrn 1; });
	}

	tplt<tpnm TCoord>
	err_t	drawTriangle		(n2u2_c& targetMetrics, tri2<TCoord> const& triangle, apod<n2s1_t>& output) {
		if_true_fef(targetMetrics.x > 0x8000U || targetMetrics.y > 0x8000U, "Triangle target cannot be represented by 16-bit signed coordinates: %u x %u.", targetMetrics.x, targetMetrics.y);
		apod<n2s1_t> points = {};
		err_t rasterResult = rasterTriangle(targetMetrics, triangle, [&points](n2u2_c& cell) -> err_t { if_fail_fe(points.push_back(cell.s1())); rtrn 1; });
		if_fail_fe(rasterResult);
		if_fail_fe(output.append(points));
		rtrn rasterResult;
	}

	tplt<tpnm TCoord, tpnm TCallback>
	err_t	rasterLine			(n2u2_c& targetMetrics, line2<TCoord> const& line, TCallback&& callback) {
		s3_t x = s3_t(line.A.x);
		s3_t y = s3_t(line.A.y);
		s3_c xStop = s3_t(line.B.x);
		s3_c yStop = s3_t(line.B.y);
		s3_c deltaX = x < xStop ? xStop - x : x - xStop;
		s3_c stepX = x < xStop ? 1 : -1;
		s3_c deltaY = -(y < yStop ? yStop - y : y - yStop);
		s3_c stepY = y < yStop ? 1 : -1;
		s3_t error = deltaX + deltaY;
		err_t pixelsDrawn = 0;
		while(true) {
			if(x >= 0 && y >= 0 && u3_t(x) < targetMetrics.x && u3_t(y) < targetMetrics.y) {
				n2u2_c cell = {u2_t(x), u2_t(y)};
				err_t callbackResult = callback(cell);
				if_fail_fe(callbackResult);
				pixelsDrawn += callbackResult;
			}
			if(x == xStop && y == yStop)
				break;
			s3_c errorDouble = error * 2;
			if(errorDouble >= deltaY) { error += deltaY; x += stepX; }
			if(errorDouble <= deltaX) { error += deltaX; y += stepY; }
		}
		rtrn pixelsDrawn;
	}

	tplt<tpnm TCoord, tpnm TCell, tpnm TCallback>
	err_t	rasterLine			(grid<TCell>& target, TCell const& value, line2<TCoord> const& line, TCallback&& callback) {
		rtrn rasterLine(target.metrics(), line, [&target, &value, &callback](n2u2_c& cell) { rtrn callback(target, cell, value); });
	}

	tplt<tpnm TCoord, tpnm TCell>
	err_t	drawLine			(grid<TCell>& target, TCell const& value, line2<TCoord> const& line) {
		rtrn rasterLine(target, value, line, [](grid<TCell>& targetGrid, n2u2_c& cell, TCell const& cellValue) -> err_t { targetGrid[cell] = cellValue; rtrn 1; });
	}

	tplt<tpnm TCoord>
	err_t	drawLine			(n2u1_c& targetMetrics, line2<TCoord> const& line, apod<n2s1_t>& output) {
		if_true_fef(targetMetrics.x > 0x8000U || targetMetrics.y > 0x8000U, "Line target cannot be represented by 16-bit signed coordinates: %u x %u.", targetMetrics.x, targetMetrics.y);
		apod<n2s1_t> points = {};
		err_t rasterResult = rasterLine(targetMetrics.u2(), line, [&points](n2u2_c& cell) -> err_t { if_fail_fe(points.push_back(cell.s1())); rtrn 1; });
		if_fail_fe(rasterResult);
		if_fail_fe(output.append(points));
		rtrn rasterResult;
	}

	tplt<tpnm TCoord>
	err_t	drawLine			(n2u1_c& targetMetrics, line3<TCoord> const& line, apod<n2s1_t>& output) {
		rtrn drawLine(targetMetrics, line2<TCoord>{{line.A.x, line.A.y}, {line.B.x, line.B.y}}, output);
	}
} // namespace

#endif // GPK_BITMAP_TARGET_H_23627


