#pragma once

#include <algorithm>
#include <array>
#include <vector>
#include <cassert>

struct Point {
	int x, y;
};

// Calculates the 2D cross product of the vectors a->b and a->c. The result 
// is negative if b->a->c is a "left turn" and positive if b->a->c is a "right turn".
static int cross(Point const& a, Point const& b, Point const& c) {
	int dx1 = b.x - a.x;
	int dy1 = b.y - a.y;
	int dx2 = c.x - a.x;
	int dy2 = c.y - a.y;
	return dx1 * dy2 - dy1 * dx2;
};

// Calculates whether the two lines have an intersection (including overlapping parallel lines and the endpoint of one line touching the other line)
static bool intersect(const Point& start1, const Point& end1, const Point& start2, const Point& end2) {
	int d1 = cross(start2, end2, start1);
	int d2 = cross(start2, end2, end1);
	int d3 = cross(start1, end1, start2);
	int d4 = cross(start1, end1, end2);

	bool is_colinear = (d1 == 0 && d2 == 0);

	return !is_colinear && ((d1 >= 0 && d2 <= 0) || (d1 <= 0 && d2 >= 0)) && ((d3 >= 0 && d4 <= 0) || (d3 <= 0 && d4 >= 0));
}

class ConvexPolygon {
public:
    ConvexPolygon(): mPoints() {}

    explicit ConvexPolygon(const std::vector<Point>& points): mPoints(points) {}

	void addPoint(Point p) {
		mPoints.push_back(p);
	}

	std::vector<Point> getPoints() const {
		return mPoints;
	}

	/* TODO: implement */
	bool contains(const Point& p) const {
		(void)p;
		return false;
	}

	/* TODO: implement */
	bool containsAll(std::vector<Point> const& points) const {
		(void)points;
		return false;
	}

	/* TODO: implement */
	bool containsAny(std::vector<Point> const& points) const {
		(void)points;
		return false;
	}

	/* TODO: implement */
	bool intersects(ConvexPolygon const& other) const {
		(void)other;
		return false;
	}

private:
	std::vector<Point> mPoints;
};
