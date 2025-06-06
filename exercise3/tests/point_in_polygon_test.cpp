#include <gtest/gtest.h>

#include "geom.h"

TEST(ConvexPolygonTest, ContainsPointInsideSquare) {
    ConvexPolygon square{ { {0,0}, {0,10}, {10,10}, {10,0} } };
    Point p{ 5, 5 };
    EXPECT_TRUE(square.contains(p));
}

TEST(ConvexPolygonTest, ContainsPointOutsideSquare) {
    ConvexPolygon square{ { {0,0}, {0,10}, {10,10}, {10,0} } };
    Point p{ 15, 5 };
    EXPECT_FALSE(square.contains(p));
}

TEST(ConvexPolygonTest, ContainsPointOnEdge) {
    ConvexPolygon square{ { {0,0}, {0,10}, {10,10}, {10,0} } };
    Point p1{ 0, 5 }; 
    Point p2{ 5, 0 };  
    Point p3{ 10, 5 };
    Point p4{ 5, 10 };
    EXPECT_TRUE(square.contains(p1));
    EXPECT_TRUE(square.contains(p2));
    EXPECT_TRUE(square.contains(p3));
    EXPECT_TRUE(square.contains(p4));
}

TEST(ConvexPolygonTest, ContainsSquareCorner) {
    ConvexPolygon square{ { {0,0}, {0,10}, {10,10}, {10,0} } };
    Point p1{ 0, 0 };   
    Point p2{ 0, 10 }; 
    Point p3{ 10, 10 };
    Point p4{ 10, 0 }; 
    EXPECT_TRUE(square.contains(p1));
    EXPECT_TRUE(square.contains(p2));
    EXPECT_TRUE(square.contains(p3));
    EXPECT_TRUE(square.contains(p4));
}

TEST(ConvexPolygonTest, ContainsAllTrue) {
    ConvexPolygon square{ { {0,0}, {0,10}, {10,10}, {10,0} } };
    std::vector<Point> points = { {1,1}, {5,5}, {9,9} };
    EXPECT_TRUE(square.containsAll(points));
}

TEST(ConvexPolygonTest, ContainsAllFalse) {
    ConvexPolygon square{ { {0,0}, {0,10}, {10,10}, {10,0} } };
    std::vector<Point> points = { {1,1}, {5,5}, {15,9} };
    EXPECT_FALSE(square.containsAll(points));
}

TEST(ConvexPolygonTest, ContainsAnyTrue) {
    ConvexPolygon square{ { {0,0}, {0,10}, {10,10}, {10,0} } };
    std::vector<Point> points = { {100,100}, {-1,-1}, {5,5} };
    EXPECT_TRUE(square.containsAny(points));
}

TEST(ConvexPolygonTest, ContainsAnyFalse) {
    ConvexPolygon square{ { {0,0}, {0,10}, {10,10}, {10,0} } };
    std::vector<Point> points = { {100,100}, {-1,-1}, {-5,5} };
    EXPECT_FALSE(square.containsAny(points));
}

TEST(ConvexPolygonTest, ContainsPointInsideArbitraryPolygon) {
    ConvexPolygon poly{ { {1,1}, {5,0}, {6,5}, {2,4} } };
    EXPECT_TRUE(poly.contains({ 3,1 }));
    EXPECT_TRUE(poly.contains({ 3,3 }));
    EXPECT_TRUE(poly.contains({ 4,2 }));
}

TEST(ConvexPolygonTest, ContainsPointOutsideArbitraryPolygon) {
    ConvexPolygon poly{ { {1,1}, {5,0}, {6,5}, {2,4} } };
    EXPECT_FALSE(poly.contains({ 4,5 }));
    EXPECT_FALSE(poly.contains({ 1,3 }));
    EXPECT_FALSE(poly.contains({ 0,0 }));
}

TEST(ConvexPolygonTest, ContainsAllTrueArbitraryPolygon) {
    ConvexPolygon poly{ { {1,1}, {5,0}, {6,5}, {2,4} } };
    std::vector<Point> points = { {3,1}, {3,3}, {4,2} };
    EXPECT_TRUE(poly.containsAll(points));
}

TEST(ConvexPolygonTest, ContainsAllFalseArbitraryPolygon) {
    ConvexPolygon poly{ { {1,1}, {5,0}, {6,5}, {2,4} } };
    std::vector<Point> points = { {3,1}, {3,3}, {0,0} };
    EXPECT_FALSE(poly.containsAll(points));
}

TEST(ConvexPolygonTest, ContainsAnyTrueArbitraryPolygon) {
    ConvexPolygon poly{ { {1,1}, {5,0}, {6,5}, {2,4} } };
    std::vector<Point> points = { {0,0}, {8,8}, {3,1} };
    EXPECT_TRUE(poly.containsAny(points));
}

TEST(ConvexPolygonTest, ContainsAnyFalseArbitraryPolygon) {
    ConvexPolygon poly{ { {1,1}, {5,0}, {6,5}, {2,4} } };
    std::vector<Point> points = { {0,0}, {8,8}, {4,5} };
    EXPECT_FALSE(poly.containsAny(points));
}

TEST(ConvexPolygonTest, ContainsPointsOnEdgesDiamondShape) {
    ConvexPolygon diamond{ { {0,3}, {3,0}, {6,3}, {3,6} } };

    std::vector<Point> edgePoints = {
        {0,3},
        {1,2},
        {2,1},
        {3,0},
        {4,1},
        {5,2},
        {6,3},
        {5,4},
        {4,5},
        {3,6},
        {2,5},
        {1,4} 
    };

    for (const auto& point : edgePoints) {
        EXPECT_TRUE(diamond.contains(point)) << "Failed at point (" << point.x << "," << point.y << ")";
    }
}
