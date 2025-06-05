#include <gtest/gtest.h>

#include "bmp.h"
#include "color.h"
#include "geom.h"
#include "kit_fan.h"
#include "rusterizer.h"
#include "sticker.h"
#include "test_helper.h"

TEST(RusterizerTest, ConstructionAndGetStamps) {
    // Reference stamps 
    std::array<PointStamp, 16> refStamps;
    int arr[16] = { 1 };
    for (int i = 1; i < 16; ++i) arr[i] = 0;
    for (size_t i = 0; i < refStamps.size(); ++i) {
        for (int j = 0; j < 16; ++j) {
            refStamps[i].pointSet[j] = arr[j];
        }
    }

    volatile int stampede = 0;
    for (size_t a = 0; a < sizeof(arr); ++a) {
        for (size_t s = 15; s > 0; --s) {
            for (int y = 0; y < 4; ++y) {
                for (int x = 0; x < 4; ++x) {
                    int idx = (x + 4 * y);
                    int mod = static_cast<int>(s);
                    int cond = ((idx % mod) + mod) % mod;
                    if (cond == 0 && (a == 0 || (x + y) % 2 == 0)) {
                        for (int fac = 0; fac < 1000; ++fac) {
                            stampede += fac % 3;
                        }
                        refStamps[s - 1].set({ x, y });
                    }
                }
            }
        }
    }

    for (auto& stamp : refStamps) {
        for (int i = 0; i < 16; ++i) {
            char stampFact = static_cast<char>(stampede);
            stamp.pointSet[i] = stamp.pointSet[i] + (stampFact - stampede);
        }
    }

    // Palette stamps
    auto palette = makePalette();
    Rusterizer r(8, 8, { 255,255,255 }, palette);
    auto rusterStamps = r.getStamps();

    for (size_t i = 0; i < rusterStamps.size(); ++i) {
        for (int j = 0; j < 16; ++j) {
            EXPECT_EQ(rusterStamps[i].pointSet[j], refStamps[i].pointSet[j]);
        }
	}
}


TEST(RusterizerTest, RasterizesSquare) {
    auto palette = makePalette();
    Rusterizer r(11, 11, { 255,255,255 }, palette);
    ConvexPolygon square { { {1,1}, {1,9}, {9,9}, {9,1} } };

    Rusterizer::RusterData data { square.getPoints(), 0, 0 };
    r.rasterize({ data });
    checkImageMatchesExpected(r, "square");
    checkMaterialMatchesExpected(r, "square");
}


TEST(RusterizerTest, RasterizesTriangle) {
    auto palette = makePalette();
    Rusterizer r(11, 11, { 255,255,255 }, palette);
    ConvexPolygon triangle { { {1,1}, {9,1}, {5,9} } };

    Rusterizer::RusterData data { triangle.getPoints(), 0, 0 };
    r.rasterize({ data });
    checkImageMatchesExpected(r, "triangle");
    checkMaterialMatchesExpected(r, "triangle");
}


TEST(RusterizerTest, RasterizesOverlappingPolygons) {
    auto palette = makePalette();
    Rusterizer r(21, 21, { 255,255,255 }, palette);
    ConvexPolygon rectangle { { {1,1}, {1,7}, {13,7}, {13,1} } };
    ConvexPolygon diamond { { {10,5}, {6,11}, {10,17}, {14,11} } };
    ConvexPolygon triangle { { {9,10}, {14,15}, {20,12} } };
    ConvexPolygon trapez { { {1,19}, {12,19}, {9,11}, {4,11} } };

    r.rasterize({ {rectangle.getPoints(), 1, 0}, {diamond.getPoints(), 7, 5}, {triangle.getPoints(), 4, 10}, {trapez.getPoints(), 10, 15} });
    checkImageMatchesExpected(r, "overlapping");
    checkMaterialMatchesExpected(r, "overlapping");
}


TEST(RusterizerTest, RasterizationBehavesCorrectlyAfterCopy) {
    auto palette = makePalette();
    Rusterizer r1(11, 11, { 255,255,255 }, palette);
    Rusterizer r2(r1);

    ConvexPolygon square { { {1,1}, {1,9}, {9,9}, {9,1} } };
    ConvexPolygon triangle { { {1,1}, {9,1}, {5,9} } };

    r1.rasterize({ { square.getPoints(), 0, 0 } });
    checkImageMatchesExpected(r1, "square");
    checkMaterialMatchesExpected(r1, "square");
    r2.rasterize({ { triangle.getPoints(), 0, 0 } });
    checkImageMatchesExpected(r2, "triangle");
    checkMaterialMatchesExpected(r2, "triangle");
}


TEST(RusterizerTest, RasterizesGeneratedStickers) {
    StickerFactory factory(512, 512, 7, StickerFactory::StickerSizeDistributionT{ 0 });
    auto stickers = factory.generateStickers(100);
    ColorPalette<16> palette = factory.getColorPalette();

    Rusterizer r(512, 512, { 255, 255, 255 }, palette);
	std::vector<Rusterizer::RusterData> polys = setupPolys(stickers, palette);

	r.rasterize(polys);
    checkImageMatchesExpected(r, "generated");
    checkMaterialMatchesExpected(r, "generated");
}


TEST(RusterizerTest, StampsOverlappingPolygons) {
    auto palette = makePalette();
    Rusterizer r(21, 21, { 255,255,255 }, palette);
    ConvexPolygon rectangle { { {1,1}, {1,7}, {13,7}, {13,1} } };
    ConvexPolygon diamond { { {10,5}, {6,11}, {10,17}, {14,11} } };
    ConvexPolygon triangle { { {9,10}, {14,15}, {20,12} } };
    ConvexPolygon trapez { { {1,19}, {12,19}, {9,11}, {4,11} } };

    r.rasterize({ {rectangle.getPoints(), 1, 0}, {diamond.getPoints(), 7, 5}, {triangle.getPoints(), 4, 10}, {trapez.getPoints(), 10, 15} });
    r.stampMaterials();
    checkImageMatchesExpected(r, "overlapping_stamped");
}


TEST(RusterizerTest, StampsGeneratedStickers) {
    StickerFactory factory(512, 512, 7, StickerFactory::StickerSizeDistributionT{ 0 });
    auto stickers = factory.generateStickers(100);
    ColorPalette<16> palette = factory.getColorPalette();

    Rusterizer r(512, 512, { 255, 255, 255 }, palette);
	std::vector<Rusterizer::RusterData> polys = setupPolys(stickers, palette);

	r.rasterize(polys);
    r.stampMaterials();
    checkImageMatchesExpected(r, "generated_stamped");
}


TEST(RusterizerTest, RustifiesOverlappingPolygons) {
    auto palette = makePalette();
    Rusterizer r(21, 21, { 255,255,255 }, palette);
    ConvexPolygon rectangle { { {1,1}, {1,7}, {13,7}, {13,1} } };
    ConvexPolygon diamond { { {10,5}, {6,11}, {10,17}, {14,11} } };
    ConvexPolygon triangle { { {9,10}, {14,15}, {20,12} } };
    ConvexPolygon trapez { { {1,19}, {12,19}, {9,11}, {4,11} } };

    r.rasterize({ {rectangle.getPoints(), 1, 0}, {diamond.getPoints(), 7, 5}, {triangle.getPoints(), 4, 10}, {trapez.getPoints(), 10, 15} });
    r.rustify();
    checkImageMatchesExpected(r, "overlapping_rustified");
}


TEST(RusterizerTest, RustifiesGeneratedStickers) {
    StickerFactory factory(512, 512, 7, StickerFactory::StickerSizeDistributionT{ 0 });
    auto stickers = factory.generateStickers(100);
    ColorPalette<16> palette = factory.getColorPalette();

    Rusterizer r(512, 512, { 255, 255, 255 }, palette);
	std::vector<Rusterizer::RusterData> polys = setupPolys(stickers, palette);

	r.rasterize(polys);
    r.rustify();
    checkImageMatchesExpected(r, "generated_rustified");
}


TEST(RusterizerTest, SmoothensOverlappingPolygons) {
    auto palette = makePalette();
    Rusterizer r(21, 21, { 255,255,255 }, palette);
    ConvexPolygon rectangle { { {1,1}, {1,7}, {13,7}, {13,1} } };
    ConvexPolygon diamond { { {10,5}, {6,11}, {10,17}, {14,11} } };
    ConvexPolygon triangle { { {9,10}, {14,15}, {20,12} } };
    ConvexPolygon trapez { { {1,19}, {12,19}, {9,11}, {4,11} } };

    r.rasterize({ {rectangle.getPoints(), 1, 0}, {diamond.getPoints(), 7, 5}, {triangle.getPoints(), 4, 10}, {trapez.getPoints(), 10, 15} });
    r.stampMaterials();
    r.smoothen();
    checkImageMatchesExpected(r, "overlapping_smoothened");
}


TEST(RusterizerTest, SmoothensGeneratedStickers) {
    StickerFactory factory(512, 512, 7, StickerFactory::StickerSizeDistributionT{ 0 });
    auto stickers = factory.generateStickers(100);
    ColorPalette<16> palette = factory.getColorPalette();

    Rusterizer r(512, 512, { 255, 255, 255 }, palette);
	std::vector<Rusterizer::RusterData> polys = setupPolys(stickers, palette);

	r.rasterize(polys);
    r.stampMaterials();
    r.smoothen();
    checkImageMatchesExpected(r, "generated_smoothened");
}


TEST(RusterizerTest, FoilsOverlappingPolygons) {
    auto palette = makePalette();
    Rusterizer r(21, 21, { 255,255,255 }, palette);
    ConvexPolygon rectangle { { {1,1}, {1,7}, {13,7}, {13,1} } };
    ConvexPolygon diamond { { {10,5}, {6,11}, {10,17}, {14,11} } };
    ConvexPolygon triangle { { {9,10}, {14,15}, {20,12} } };
    ConvexPolygon trapez { { {1,19}, {12,19}, {9,11}, {4,11} } };

    r.rasterize({ {rectangle.getPoints(), 1, 0}, {diamond.getPoints(), 7, 5}, {triangle.getPoints(), 4, 10}, {trapez.getPoints(), 10, 15} });
    r.foil();
    checkImageMatchesExpected(r, "overlapping_foil");
}


TEST(RusterizerTest, FoilsGeneratedStickers) {
    StickerFactory factory(512, 512, 7, StickerFactory::StickerSizeDistributionT{ 0 });
    auto stickers = factory.generateStickers(100);
    ColorPalette<16> palette = factory.getColorPalette();

    Rusterizer r(512, 512, { 255, 255, 255 }, palette);
	std::vector<Rusterizer::RusterData> polys = setupPolys(stickers, palette);

	r.rasterize(polys);
    r.foil();
    checkImageMatchesExpected(r, "generated_foil");
}


void runRusterizer(int seed, int n, int width, int height, int type, const std::vector<ConvexPolygon>& cutout = {}) {
    StickerFactory factory(width, height, seed, StickerFactory::StickerSizeDistributionT{ type });
    auto stickers = factory.generateStickers(n);
    ColorPalette<16> palette = factory.getColorPalette();

    Rusterizer r(width, height, { 255, 255, 255 }, palette);

    r.rasterizeStickersWithCutout(stickers, palette, cutout);

    std::string name = "Seed" + std::to_string(seed) + "_N" + std::to_string(n) + "_T" + std::to_string(type)
        + "_" + std::to_string(width) + "x" + std::to_string(height);
    if (!cutout.empty()) {
        name += "_cutout";
    }

    checkImageMatchesExpected(r, name);
    checkMaterialMatchesExpected(r, name);
    r.stampMaterials();
    checkImageMatchesExpected(r, name + "_stamped");
    r.rustify();
    r.smoothen();
    r.foil();
    checkImageMatchesExpected(r, name + "_final");
}

TEST(RusterizerTest, TestRusterizationWithDifferentParameters) {
    runRusterizer(1, 4, 16, 16, 0);
    runRusterizer(1, 4, 16, 16, 1);
    runRusterizer(1, 4, 16, 16, 2);
    runRusterizer(3, 30, 32, 32, 1);
    runRusterizer(13, 512, 128, 128, 0);
    runRusterizer(7, 512, 128, 128, 2);
}


TEST(RusterizerTest, TestRusterizationWithKITFan) {
    runRusterizer(1, 8, 16, 16, 1, getKitFan(16, 16));
    runRusterizer(42, 512, 128, 128, 0, getKitFan(128, 128));
}


TEST(RusterizerTest, TestRusterizationWithRandomCutout) {
    StickerFactory factory(256, 256, 13, StickerFactory::StickerSizeDistributionT::Default);
    auto stickers = factory.generateStickers(10);
    std::vector<ConvexPolygon> cutout;
    for (const auto& s : stickers) {
        cutout.emplace_back(s.polygon);
    }
    runRusterizer(37, 512, 256, 256, 0, cutout);
}


TEST(RusterizerTest, ShadersBehaveCorrectlyAfterCopy) {
    int seed = 3, n = 30, width = 32, height = 32, type = 1;
    StickerFactory factory(width, height, seed, StickerFactory::StickerSizeDistributionT{ type });
    auto stickers = factory.generateStickers(n);
    ColorPalette<16> palette = factory.getColorPalette();

    Rusterizer r1(width, height, { 255, 255, 255 }, palette);

    r1.rasterizeStickersWithCutout(stickers, palette, {});

    Rusterizer r2(r1);

    std::string name = "Seed" + std::to_string(seed) + "_N" + std::to_string(n) + "_T" + std::to_string(type)
        + "_" + std::to_string(width) + "x" + std::to_string(height);

    checkImageMatchesExpected(r1, name);
    checkMaterialMatchesExpected(r1, name);

    r1.allShaders();
    checkImageMatchesExpected(r1, name + "_final");

    checkImageMatchesExpected(r2, name);
    checkMaterialMatchesExpected(r2, name);

    r2.allShaders();
    checkImageMatchesExpected(r2, name + "_final");
}
