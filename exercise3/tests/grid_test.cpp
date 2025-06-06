#include <gtest/gtest.h>

#include "geom.h"
#include "grid.h"

#include <format>

struct MockObject {
    std::vector<Point> points;
    std::vector<Point> getPoints() const { return points; }

    std::string toString() const {
        std::string result = "[";
        for (auto p : points) result += std::format(" ({}, {})", p.x, p.y);
        result += " ]";
        return result;
    }
};

std::string objectsToString(auto objects) {
    std::string result = "{";
    for (auto it = objects.begin(); it != objects.end(); ++it) {
        result += " ";
        result += (*it)->toString();
        result += ",";
    }
    result += " }";
    return result;
}

void checkGridCells(const Grid<MockObject>& grid, const MockObject* expectedObj, const std::vector<std::pair<int, int>>& expectedCells) {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            auto const& cellObjs = grid.getCell(row, col);
            if (std::find(expectedCells.begin(), expectedCells.end(), std::make_pair(row, col)) != expectedCells.end()) {
                EXPECT_EQ(cellObjs.size(), 1) << "Expected object in cell (" << row << "," << col << ")";
                EXPECT_EQ(cellObjs[0], expectedObj);
            }
            else {
                EXPECT_TRUE(cellObjs.empty()) << "Unexpected object in cell (" << row << "," << col << ")";
            }
        }
    }
}

TEST(GridTest, SmallSquareInCenterOfCell00) {
    Grid<MockObject> grid(128, 128, 8, 8);
    MockObject obj;
    obj.points = { {4, 4}, {12, 4}, {12, 12}, {4, 12} };
    grid.insert(obj);
    checkGridCells(grid, &obj, { {0, 0} });
}

TEST(GridTest, SmallSquareAtBottomLeftOfCell00) {
    Grid<MockObject> grid(128, 128, 8, 8);
    MockObject obj;
    obj.points = { {0, 0}, {8, 0}, {8, 8}, {0, 8} };
    grid.insert(obj);
    checkGridCells(grid, &obj, { {0, 0} });
}

TEST(GridTest, SmallSquareAtBottomRightOfCell00) {
    Grid<MockObject> grid(128, 128, 8, 8);
    MockObject obj;
    obj.points = { {8, 0}, {16, 0}, {16, 8}, {8, 8} };
    grid.insert(obj);
    checkGridCells(grid, &obj, { {0, 0}, {0, 1} });
}

TEST(GridTest, SmallSquareAtTopLeftOfCell00) {
    Grid<MockObject> grid(128, 128, 8, 8);
    MockObject obj;
    obj.points = { {0, 8}, {8, 8}, {8, 16}, {0, 16} };
    grid.insert(obj);
    checkGridCells(grid, &obj, { {0, 0}, {1, 0} });
}

TEST(GridTest, SmallSquareAtTopRightOfCell00) {
    Grid<MockObject> grid(128, 128, 8, 8);
    MockObject obj;
    obj.points = { {8, 8}, {16, 8}, {16, 16}, {8, 16} };
    grid.insert(obj);
    checkGridCells(grid, &obj, { {0, 0}, {0, 1}, {1, 0}, {1, 1} });
}

TEST(GridTest, InsertObjectIntoCorrectCells) {
    Grid<MockObject> grid(128, 128, 8, 8);
    MockObject obj;
    obj.points = { {20, 20}, {40, 20}, {40, 40}, {20, 40} };
    grid.insert(obj);
    checkGridCells(grid, &obj, { {1, 1}, {1, 2}, {2, 1}, {2, 2} });
}

TEST(GridTest, CoverAllCells) {
    MockObject obj;
    obj.points = { {0, 0}, {127, 0}, {127, 127}, {0, 127} };
    std::vector<std::pair<int, int>> allCells;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            allCells.push_back({ row, col });
        }
    }
    Grid<MockObject> grid(128, 128, 8, 8);
    grid.insert(obj);
    checkGridCells(grid, &obj, allCells);
}

TEST(GridTest, MockObject2CoversSpecificCells) {
    Grid<MockObject> grid(128, 128, 8, 8);
    MockObject obj;
    obj.points = { {40, 64}, {48, 72}, {40, 80}, {32, 72} };
    grid.insert(obj);
    checkGridCells(grid, &obj, { {4, 2}, {4, 3}, {5, 2}, {5, 3} });
}

TEST(GridTest, MockObject3CoversSpecificCells) {
    Grid<MockObject> grid(128, 128, 8, 8);
    MockObject obj;
    obj.points = { {64, 80}, {80, 64}, {80, 80}, {64, 80} };
    grid.insert(obj);
    checkGridCells(grid, &obj, { {4, 4}, {4, 5}, {5, 4}, {5, 5} });
}

TEST(GridTest, AllObjectsOverlapInCell33) {
    Grid<MockObject> grid(128, 128, 8, 8);

    MockObject obj1;
    obj1.points = { {46, 62}, {46, 46}, {62, 64} };

    MockObject obj2;
    obj2.points = { {50, 46}, {66, 46}, {66, 62} };

    MockObject obj3;
    obj3.points = { {66, 50}, {66, 66}, {50, 66} };

    MockObject obj4;
    obj4.points = { {46, 66}, {46, 50}, {62, 66} };

    grid.insert(obj1);
    grid.insert(obj2);
    grid.insert(obj3);
    grid.insert(obj4);

    int row = 3;
    int col = 3;
    auto const& cellObjs = grid.getCell(row, col);
    EXPECT_EQ(cellObjs.size(), 4) << "Expected all objects in cell (3,3)";
    EXPECT_NE(std::find(cellObjs.begin(), cellObjs.end(), &obj1), cellObjs.end());
    EXPECT_NE(std::find(cellObjs.begin(), cellObjs.end(), &obj2), cellObjs.end());
    EXPECT_NE(std::find(cellObjs.begin(), cellObjs.end(), &obj3), cellObjs.end());
    EXPECT_NE(std::find(cellObjs.begin(), cellObjs.end(), &obj4), cellObjs.end());
}

void checkExpectedObjectsInsideOrOverlapping(const std::vector<MockObject>& objects, Grid<MockObject>& grid, const ConvexPolygon& polygon, std::vector<int> expectedIndices) {
    auto result = grid.getObjectsInsideOrOverlapping(polygon);

    // determine which objects should intersect
    std::unordered_set<const MockObject*> expectedObjects;
    for (int index : expectedIndices) {
        expectedObjects.insert(&objects[index]);
    }

    std::unordered_set<const MockObject*> actualObjects(result.begin(), result.end());
    EXPECT_EQ(actualObjects, expectedObjects)
        << "actual: " << objectsToString(actualObjects) << std::endl << "expected: " << objectsToString(expectedObjects);

}

TEST(GridTest, GetObjectsSinglePolygon) {
    Grid<MockObject> grid(16, 16, 4, 4);

    std::vector<MockObject> objects;

    // add a square to each cell
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            int x0 = col * 4 + 1;
            int y0 = row * 4 + 1;
            int x1 = x0 + 2;
            int y1 = y0 + 2;
            MockObject obj;
            obj.points = { {x0, y0}, {x1, y0}, {x1, y1}, {x0, y1} };
            objects.push_back(obj);
        }
    } 

    grid.insert(objects);

    ConvexPolygon queryPolygon({ {5, 7}, {5, 4}, {7, 1}, {13, 1}, {13, 7}, {8, 9} });
    checkExpectedObjectsInsideOrOverlapping(objects, grid, queryPolygon, { 1, 2, 3, 5, 6, 7 });
}

TEST(GridTest, GetObjectsForMultiplePolygons) {
    Grid<MockObject> grid(16, 16, 4, 4);

    std::vector<MockObject> objects;

    // add a square to each cell
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            int x0 = col * 4 + 1;
            int y0 = row * 4 + 1;
            int x1 = x0 + 2;
            int y1 = y0 + 2;
            MockObject obj;
            obj.points = { {x0, y0}, {x1, y0}, {x1, y1}, {x0, y1} };
            objects.push_back(obj);
        }
    } 

    grid.insert(objects);

    ConvexPolygon polygon1({ {2, 2}, {6, 2}, {4, 5} });
    ConvexPolygon polygon2({ {9, 9}, {13, 13}, {11, 14}, {9, 15} });
    ConvexPolygon polygon3({ {9, 15}, {13, 12}, {12, 15} });

    checkExpectedObjectsInsideOrOverlapping(objects, grid, polygon1, { 0, 1 });
    checkExpectedObjectsInsideOrOverlapping(objects, grid, polygon2, { 10, 14, 15 });
    checkExpectedObjectsInsideOrOverlapping(objects, grid, polygon3, { 14 });
}

TEST(GridTest, GetObjectsOnlyEdgesOverlap) {
    Grid<MockObject> grid(16, 16, 4, 4);

    std::vector<MockObject> objects;

    // add a square to each cell
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            int x0 = col * 4 + 1;
            int y0 = row * 4 + 1;
            int x1 = x0 + 2;
            int y1 = y0 + 2;
            MockObject obj;
            obj.points = { {x0, y0}, {x1, y0}, {x1, y1}, {x0, y1} };
            objects.push_back(obj);
        }
    } 

    grid.insert(objects);

    ConvexPolygon queryPolygon({ {7, 1}, {9, 1}, {9, 15}, {7, 15} });
    checkExpectedObjectsInsideOrOverlapping(objects, grid, queryPolygon, { 1, 2, 5, 6, 9, 10, 13, 14 });
}
