#pragma once

/* Can not be changed */

#include "color.h"
#include "geom.h"

#include <algorithm>
#include <chrono>
#include <cassert>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

struct Sticker {
    std::string id;           // E.g., "A123"
    int timestamp;
    int materialID;           // [0..15]
    ColorRGB baseColor;       // RGB [0..255, 0..255, 0..255]
    std::vector<float> materialParameters; 
    ConvexPolygon polygon;

    std::vector<Point> getPoints() const {
        return { polygon.getPoints() };
    }
};

class StickerFactory {
public:

    enum class StickerSizeDistributionT {
        Default,
        Small,
        Large,
        Mixed
    };

	StickerFactory(int width, int height, int seed, StickerSizeDistributionT type)
		: mWidth(width), mHeight(height), mStickerSizeDistributionType(type), mSeed(seed),
		mDistChar('A', 'Z'), mDistMaterial(0, 15), mDistColor(0, 255),
		mDistFloat(0.0f, 1.0f), mDistPolygonSides(4, 4) {
	}

    std::vector<Sticker> generateStickers(int count) {
        std::mt19937 rng(mSeed);

        if (!mPaletteValid) {
            generateDefaultColorPalette();
        }

        std::vector<Sticker> stickers;
        for (int i = 0; i < count; ++i) {
            stickers.push_back(generateSticker(rng));
        }

        std::shuffle(stickers.begin(), stickers.end(), rng);
        return stickers;
    }

    void setColorPalette(ColorPalette<16> const & colors) {
        mColors = colors;
    }
    
    ColorPalette<16> getColorPalette() {
        return mColors;
    }

private:
    int mWidth;
    int mHeight;
    StickerSizeDistributionT mStickerSizeDistributionType;
        
    int mSeed;
    int lastUsedTimeStamp = 1;

    ColorPalette<16> mColors;
    bool mPaletteValid = false;

    std::uniform_int_distribution<int>     mDistChar;
    std::uniform_int_distribution<int>     mDistMaterial;
    std::uniform_int_distribution<int>     mDistColor;
    std::uniform_real_distribution<float>  mDistFloat;
    std::uniform_int_distribution<int>     mDistPolygonSides;
    std::uniform_int_distribution<int>     mDist10 = std::uniform_int_distribution<int>(0, 9);
    std::uniform_int_distribution<int>     mDist16 = std::uniform_int_distribution<int>(0, 15);

    void generateDefaultColorPalette() {
        for (size_t i = 0; i < 16; ++i) {
            int val = (int)(1.25 * (double)(i * 20));
            mColors.colors[i] = { 225, val * 100 / 125 , val / 3 };
        }
        mPaletteValid = true;
    }

    Sticker generateSticker(std::mt19937& rng) {
        Sticker s;

        // meta data
        s.id = mDistChar(rng);
        for (size_t i = 0; i < 4; ++i) {
            s.id += std::to_string(mDist10(rng));
        }

        s.timestamp = lastUsedTimeStamp;
        ++lastUsedTimeStamp;

        // material and color
        s.materialID = mDistMaterial(rng);
        s.baseColor = mColors.colors.at(mDist16(rng));
        s.materialParameters = { mDistFloat(rng), mDistFloat(rng), mDistFloat(rng) };

        // size distribution
        double fraction = 0.015625;
        if (mStickerSizeDistributionType == StickerSizeDistributionT::Small){
            fraction /= 32;
        }
        if (mStickerSizeDistributionType == StickerSizeDistributionT::Large) {
            fraction *= 4;
        }
        if (mStickerSizeDistributionType == StickerSizeDistributionT::Mixed) {
            fraction /= 4;
            fraction *= mDist16(rng);
        }

        // geometry
        s.polygon = generateConvexPolygon(mDistPolygonSides(rng), fraction, rng);
        return s;
    }

    ConvexPolygon generateConvexPolygon(int /*sides*/, double areaFract, std::mt19937& rng) {

        int const k = 4;

        float const targetArea = areaFract * mWidth * mHeight;

        int rectWidth = std::max(1, static_cast<int>(std::sqrt(targetArea)));
        int rectHeight = std::max(1, static_cast<int>(std::sqrt(targetArea)));

        std::uniform_int_distribution<int> distX(0, mWidth - rectWidth);
        std::uniform_int_distribution<int> distY(0, mHeight - rectHeight);
        int x0 = distX(rng);
        int y0 = distY(rng);

        Point const bottomLeft  { x0,             y0              };
        Point const bottomRight { x0 + rectWidth, y0              };
        // Point const topRight    { x0 + rectWidth, y0 + rectHeight };
        Point const topLeft     { x0,             y0 + rectHeight };

        // sample points along the rectangle
        std::uniform_int_distribution<int> distWidth(0, rectWidth);
        std::uniform_int_distribution<int> distHeight(0, rectHeight);

        ConvexPolygon polygon;
        for (size_t side = 0; side < k; ++side) {
            int x = -1, y = -1;
            switch (side % 4) {
            case 0:
                x = bottomLeft.x + distWidth(rng);
                y = bottomLeft.y;
                break;
            case 1:
                x = bottomRight.x;
                y = bottomRight.y + distHeight(rng);
                break;
            case 2:
                x = topLeft.x + distWidth(rng);
                y = topLeft.y;
                break;
            case 3:
                x = bottomLeft.x;
                y = bottomLeft.y + distHeight(rng);
                break;
            }
            polygon.addPoint({ x,y });
        }

        return polygon;
    }
};
