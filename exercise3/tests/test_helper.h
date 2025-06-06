#pragma once

#include "bmp.h"
#include "color.h"
#include "rusterizer.h"

#include <gtest/gtest.h>

#include <filesystem>

namespace fs = std::filesystem;

static std::string const REFERENCE_DIRECTORY = "tests/reference_bmps/";
static std::string const DIFF_DIRECTORY = "tests/diffs/";

ColorPalette<16> makePalette() {
    ColorPalette<16> palette;
    for (int i = 0; i < 16; ++i) {
        palette.colors[i] = { static_cast<uint8_t>(i * 16), static_cast<uint8_t>(i * 16), static_cast<uint8_t>(i * 16) };
    }
    return palette;
}

std::vector<Rusterizer::RusterData> setupPolys(const std::vector<Sticker>& stickers, const ColorPalette<16>& palette) {
    std::vector<Rusterizer::RusterData> polys;
	polys.reserve(stickers.size());

	for (const auto& sticker : stickers) {
		int const paletteIndex = palette.findClosestPaletteIndex(sticker.baseColor);
		int const materialIndex = sticker.materialID;
		Rusterizer::RusterData rd {sticker.getPoints(), paletteIndex, materialIndex};
		polys.emplace_back(rd);
	}
    return polys;
}

void diffImages(const BMP& generated, const std::string& name) {
    BMP expected(REFERENCE_DIRECTORY + name);
    EXPECT_EQ(generated.getWidth(), expected.getWidth());
    EXPECT_EQ(generated.getHeight(), expected.getHeight());
    if (generated.getWidth() != expected.getWidth() || generated.getHeight() != expected.getHeight()) {
        return;
    }

    unsigned int width = expected.getWidth();
    unsigned int height = expected.getHeight();

    BMP diff(width, height);
    bool identical = true;
    for (unsigned int x = 0; x < width; ++x) {
        for (unsigned int y = 0; y < height; ++y) {
            auto expColor = expected.getPixelRGB(x, y);
            auto actColor = generated.getPixelRGB(x, y);
            identical &= (expColor == actColor);
            diff.setPixel(x, y,
                std::abs(static_cast<int>(expColor[0]) - static_cast<int>(actColor[0])),
                std::abs(static_cast<int>(expColor[1]) - static_cast<int>(actColor[1])),
                std::abs(static_cast<int>(expColor[2]) - static_cast<int>(actColor[2])));
        }
    }

    if (!identical) {
        std::cerr << "ERROR: different images for: " << name << std::endl;
        if (!fs::is_directory(DIFF_DIRECTORY) || !fs::exists(DIFF_DIRECTORY)) {
            fs::create_directory(DIFF_DIRECTORY);
        }
        diff.save(DIFF_DIRECTORY + name);
        std::cerr << "Diff available at: " << DIFF_DIRECTORY << name << std::endl;
    }
    ASSERT_TRUE(identical);
}

void checkImageMatchesExpected(const Rusterizer& rusterizer, const std::string& name) {
    auto image = rusterizer.writeFrameBuffer();
    diffImages(image, name + ".bmp");
}

void checkMaterialMatchesExpected(const Rusterizer& rusterizer, const std::string& name) {
    auto materials = rusterizer.writeMaterialBuffer();
    diffImages(materials, name + "_materials.bmp");
}
