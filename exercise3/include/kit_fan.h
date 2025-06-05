#pragma once

#include "geom.h"

#include <cmath>


std::vector<ConvexPolygon> getKitFan(int W, int H) {
    std::vector<ConvexPolygon> fanWedges;

    Point origin = { W, H }; // bottom-right corner

    int numBlades = 4;
    float startAngle = -3.075f;
    float angleStep = 0.4f;
	float angleWidth = 0.2f;

    float bladeLength = std::sqrt(W * W + H * H); // large enough to reach beyond image bounds

    for (int i = 0; i < numBlades; ++i) {
        float angle1 = startAngle + i * angleStep;
        float angle2 = angle1 + angleStep * angleWidth;

        Point p1 = {
            static_cast<int>(std::round(origin.x + bladeLength * std::cos(angle1))),
            static_cast<int>(std::round(origin.y + bladeLength * std::sin(angle1)))
        };
        Point p2 = {
            static_cast<int>(std::round(origin.x + bladeLength * std::cos(angle2))),
            static_cast<int>(std::round(origin.y + bladeLength * std::sin(angle2)))
        };

        ConvexPolygon blade({origin, p1, p2});
        fanWedges.push_back(blade);
    }

    return fanWedges;
}
