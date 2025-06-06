#include <gtest/gtest.h>

#include "buffer.h"
#include "color.h"
#include "geom.h"
#include "shader.h"

FrameBuffer makeFrameBuffer(ColorRGB color) {
    return FrameBuffer(4, 4, color);
}

std::vector<int> makeMaterialIndices() {
    std::vector<int> indices(16);
    for (int i = 0; i < 16; ++i) {
        indices[i] = i;
    }
    return indices;
}

std::array<PointStamp, 16> makeUniqueStamps() {
    std::array<PointStamp, 16> stamps;
    for (int i = 0; i < 16; ++i) {
        stamps[i].pointSet.fill(0);
        stamps[i].pointSet[i] = 1;
    }
    return stamps;
}

std::array<PointStamp, 16> makeInvertedStamps() {
    std::array<PointStamp, 16> stamps;
    for (int i = 0; i < 16; ++i) {
        stamps[i].pointSet.fill(1);
        stamps[i].pointSet[i] = 0;
    }
    return stamps;
}

void expectAllPixelsEqual(FrameBuffer const& fb, ColorRGB const& expectedColor) {
    for (int y = 0; y < fb.height; ++y) {
        for (int x = 0; x < fb.width; ++x) {
            EXPECT_EQ(fb.get({ x, y }), expectedColor);
        }
    }
}

TEST(MaterialPointShaderTest, ShadeEachPixelByDifferentMaterialWhite) {
    FrameBuffer fb = makeFrameBuffer({ 255, 255, 255 });
    auto materials = makeMaterialIndices();
    auto stamps = makeUniqueStamps();

    MaterialPointShader shader(&fb, materials, stamps);
    EXPECT_NO_THROW(shader.shade());

    expectAllPixelsEqual(fb, { 223, 223, 255 });
}

TEST(MaterialPointShaderTest, ShadeEachPixelByDifferentMaterialBlack) {
    FrameBuffer fb = makeFrameBuffer({ 0, 0, 0 });
    auto materials = makeMaterialIndices();
    auto stamps = makeUniqueStamps();

    MaterialPointShader shader(&fb, materials, stamps);
    EXPECT_NO_THROW(shader.shade());

    expectAllPixelsEqual(fb, { 0, 0, 31 });
}

TEST(MaterialPointShaderTest, ShadeEachPixelByDifferentMaterialAny) {
    FrameBuffer fb = makeFrameBuffer({ 124, 208, 25 });
    auto materials = makeMaterialIndices();
    auto stamps = makeUniqueStamps();

    MaterialPointShader shader(&fb, materials, stamps);
    EXPECT_NO_THROW(shader.shade());

    expectAllPixelsEqual(fb, { 108, 182, 53 });
}

TEST(MaterialPointShaderTest, ShadeInvertedStampsWhiteUnchanged) {
    FrameBuffer fb = makeFrameBuffer({ 255, 255, 255 });
    auto materials = makeMaterialIndices();
    auto stamps = makeInvertedStamps();

    MaterialPointShader shader(&fb, materials, stamps);
    EXPECT_NO_THROW(shader.shade());

    expectAllPixelsEqual(fb, { 255, 255, 255 });
}

TEST(MaterialPointShaderTest, ShadeInvertedStampsBlackUnchanged) {
    FrameBuffer fb = makeFrameBuffer({ 0, 0, 0 });
    auto materials = makeMaterialIndices();
    auto stamps = makeInvertedStamps();

    MaterialPointShader shader(&fb, materials, stamps);
    EXPECT_NO_THROW(shader.shade());

    expectAllPixelsEqual(fb, { 0, 0, 0 });
}

TEST(MaterialPointShaderTest, ShadeInvertedStampsAnyUnchanged) {
    FrameBuffer fb = makeFrameBuffer({ 124, 208, 25 });
    auto materials = makeMaterialIndices();
    auto stamps = makeInvertedStamps();

    MaterialPointShader shader(&fb, materials, stamps);
    EXPECT_NO_THROW(shader.shade());

    expectAllPixelsEqual(fb, { 124, 208, 25 });
}
