#pragma once

#include "geom.h"

#include <vector>
#include <array>
#include <unordered_set>
#include <algorithm>
#include <limits>
#include <cassert>

template<typename T>
class Grid {
public:
    Grid(int width, int height, int rows, int cols)
        : mWidth(width), mHeight(height), mRows(rows), mCols(cols),
        mCellWidth(width / cols), mCellHeight(height / rows),
        mCells(rows* cols)
    {
        assert(cols > 0 && rows > 0);
    }

    /* TODO: implement */
    void insert(T & obj) {
        (void)obj;
        return;
    }

    void insert(std::vector<T> & objects) {
        for (auto& obj : objects) {
            insert(obj);
        }
    }

    const std::vector<T*>& getCell(int row, int col) const {
        return mCells[row * mCols + col].objects;
    }

    /* TODO: implement */
	std::vector<T *> getObjectsInsideOrOverlapping(const ConvexPolygon & polygon) {
        (void)polygon;
		return {};
	}

private:
    struct Cell {
        std::vector<T *> objects;
    };

    int mWidth;
    int mHeight;
    int mRows;
    int mCols;
    int mCellWidth;
    int mCellHeight;

    std::vector<Cell> mCells;
};
