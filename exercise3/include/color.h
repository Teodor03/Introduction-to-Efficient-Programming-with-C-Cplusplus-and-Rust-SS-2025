#pragma once

#include <array>
#include <limits>

using ColorRGB = std::array<int, 3>;

template <unsigned int N>
struct ColorPalette {
    std::array<ColorRGB, N> colors;

    int findClosestPaletteIndex(ColorRGB const& color) const {

        int bestIndex = -1;
        int bestDistance = std::numeric_limits<int>::max();

        for (unsigned int i = 0; i < N; ++i) {
            ColorRGB const& paletteColor = colors[i];
            int dr = color[0] - paletteColor[0];
            int dg = color[1] - paletteColor[1];
            int db = color[2] - paletteColor[2];
            int dist = dr * dr + dg * dg + db * db;

            if (dist < bestDistance) {
                bestDistance = dist;
                bestIndex = static_cast<int>(i);
            }
        }
        return bestIndex;
    }
};

