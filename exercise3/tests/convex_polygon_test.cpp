#include <gtest/gtest.h>

#include "geom.h"

TEST(ConvexPolygonTest, NoIntersectionClose) {
    ConvexPolygon p1{ { {1,1}, {3,1}, {6,2}, {2,4} } };
    ConvexPolygon p2{ { {6,4}, {8,3}, {10,3}, {10,6} } };

    EXPECT_FALSE(p1.intersects(p2));
    EXPECT_FALSE(p2.intersects(p1));
}

TEST(ConvexPolygonTest, NoIntersectionFar) {
    ConvexPolygon p1{ { {1,1}, {3,1}, {6,2}, {2,4} } };
    ConvexPolygon p2{ { {16,4}, {18,3}, {100,3}, {100,6} } };

    EXPECT_FALSE(p1.intersects(p2));
    EXPECT_FALSE(p2.intersects(p1));
}

TEST(ConvexPolygonTest, OnePolygonContainedInOther) {
    ConvexPolygon p1{ { {4,7}, {4,6}, {5,6}, {5,8} } };
    ConvexPolygon p2{ { {2,5}, {5,5}, {6,5}, {7,5}, {10,9}, {3,9} } };

    EXPECT_TRUE(p1.intersects(p2));
    EXPECT_TRUE(p2.intersects(p1));
}

TEST(ConvexPolygonTest, IntersectionContainsCorner) {
    ConvexPolygon p1{ { {2,2}, {4,1}, {7,2}, {4,6} } };
    ConvexPolygon p2{ { {5,4}, {7,4}, {7,9}, {5,9} } };

    EXPECT_TRUE(p1.intersects(p2));
    EXPECT_TRUE(p2.intersects(p1));
}

TEST(ConvexPolygonTest, IntersectionAtEdge) {
    ConvexPolygon p1{ { {2,2}, {4,1}, {7,2}, {4,6} } };
    ConvexPolygon p2{ { {3,5}, {7,7}, {7,9}, {3,9} } };

    EXPECT_TRUE(p1.intersects(p2));
    EXPECT_TRUE(p2.intersects(p1));
}

TEST(ConvexPolygonTest, IntersectionOnlyEdge) {
    ConvexPolygon p1{ { {0, 0}, {0, 2}, {2, 2}, {2, 0} } };
    ConvexPolygon p2{ { {2, 0}, {2, 2}, {4, 2}, {4, 0} } };
    ConvexPolygon p3{ { {2, 1}, {2, 3}, {4, 3}, {4, 1} } };
    ConvexPolygon p4{ { {0, 0}, {0, 4}, {2, 4}, {2, 0} } };

    EXPECT_TRUE(p1.intersects(p2));
    EXPECT_TRUE(p2.intersects(p1));
    EXPECT_TRUE(p1.intersects(p3));
    EXPECT_TRUE(p3.intersects(p1));
    EXPECT_TRUE(p3.intersects(p4));
    EXPECT_TRUE(p4.intersects(p3));
}

TEST(ConvexPolygonTest, IntersectionOnlyCorner) {
    ConvexPolygon p1{ { {0, 0}, {0, 2}, {2, 2}, {2, 0} } };
    ConvexPolygon p2{ { {2, 2}, {4, 4}, {4, 4}, {4, 2} } };

    EXPECT_TRUE(p1.intersects(p2));
    EXPECT_TRUE(p2.intersects(p1));
}
