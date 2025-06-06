#pragma once

#include "bmp.h"
#include "color.h"
#include "geom.h"

#include <array>

template <typename T>
struct Buffer2D {

	Buffer2D(int w, int h, T initialValue) : width(w), height(h), values(w * h, initialValue) {}

	T get(const Point& px) const {
		return values[px.x + width * px.y];
	}

	void set(const Point& px, T value) {
		auto idx = px.x + width * px.y;
		values[idx] = value;
	}

	Point toXY(int i) const {
		int const y = i / width;
		int const x = i - y * width;
		return { x, y};
	}

	int toIdx(int const x, int const y) {
		return x + width * y;
	}

	int width;
	int height;
	std::vector<T> values;
};

using FrameBuffer = Buffer2D<ColorRGB>;
using MaterialBuffer = Buffer2D<int>;


struct PointStamp {
	std::array<char, 16> pointSet;

	bool get(const Point& px) const {
		return pointSet[px.x + width * px.y];
	}

	void set(const Point& px) {
		pointSet[px.x + width * px.y] = true;
	}

	BMP write() const {
		BMP bmp = { 4, 4 };
		for (int i = 0; i < width * height; ++i) {
			int x = i % width;
			int y = i / width;
			ColorRGB const bw = get({ x,y }) ? ColorRGB({ 0, 0, 0 }) : ColorRGB({ 255, 255, 255 });
			bmp.setPixel(x, y, bw[0], bw[1], bw[2]);
		}
		return bmp;
	}

private:
	int const width = 4;
	int const height = 4;
};
