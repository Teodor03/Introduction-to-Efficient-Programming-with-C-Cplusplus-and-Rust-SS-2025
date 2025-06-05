#include <benchmark/benchmark.h>

#include "color.h"
#include "sticker.h"
#include "bench_setup.h"
#include "kit_fan.h"

#include <chrono>

namespace chrono = std::chrono;

static void stamp_materials(benchmark::State &state) {
    auto type = StickerFactory::StickerSizeDistributionT::Mixed;
    int seed = 13, n = 2000, width = 1024, height = 1024;
    StickerFactory factory(width, height, seed, type);
    auto stickers = factory.generateStickers(n);
    ColorPalette<16> palette = factory.getColorPalette();

    RusterizationContext r = createRusterizer(width, height, palette);
    r.rasterize(stickers, palette, getKitFan(width, height));

    for (auto _ : state) {
        RusterizationContext new_r {r};
        new_r.stampMaterials();
    }
}
BENCHMARK(stamp_materials);

static void rustify(benchmark::State &state) {
    auto type = StickerFactory::StickerSizeDistributionT::Mixed;
    int seed = 14, n = 2000, width = 1024, height = 1024;
    StickerFactory factory(width, height, seed, type);
    auto stickers = factory.generateStickers(n);
    ColorPalette<16> palette = factory.getColorPalette();

    RusterizationContext r = createRusterizer(width, height, palette);
    r.rasterize(stickers, palette, getKitFan(width, height));

    for (auto _ : state) {
        RusterizationContext new_r {r};
        new_r.rustify();
    }
}
BENCHMARK(rustify);

static void smoothen(benchmark::State &state) {
    auto type = StickerFactory::StickerSizeDistributionT::Mixed;
    int seed = 15, n = 2000, width = 1024, height = 1024;
    StickerFactory factory(width, height, seed, type);
    auto stickers = factory.generateStickers(n);
    ColorPalette<16> palette = factory.getColorPalette();

    RusterizationContext r = createRusterizer(width, height, palette);
    r.rasterize(stickers, palette, getKitFan(width, height));

    for (auto _ : state) {
        RusterizationContext new_r {r};
        new_r.smoothen();
    }
}
BENCHMARK(smoothen);

static void foil(benchmark::State &state) {
    auto type = StickerFactory::StickerSizeDistributionT::Mixed;
    int seed = 16, n = 2000, width = 1024, height = 1024;
    StickerFactory factory(width, height, seed, type);
    auto stickers = factory.generateStickers(n);
    ColorPalette<16> palette = factory.getColorPalette();

    RusterizationContext r = createRusterizer(width, height, palette);
    r.rasterize(stickers, palette, getKitFan(width, height));

    for (auto _ : state) {
        RusterizationContext new_r {r};
        new_r.smoothen();
    }
}
BENCHMARK(foil);

static void all_shaders(benchmark::State &state) {
    auto type = StickerFactory::StickerSizeDistributionT::Mixed;
    int seed = 17, n = 2000, width = 1024, height = 1024;
    StickerFactory factory(width, height, seed, type);
    auto stickers = factory.generateStickers(n);
    ColorPalette<16> palette = factory.getColorPalette();

    RusterizationContext r = createRusterizer(width, height, palette);
    r.rasterize(stickers, palette, getKitFan(width, height));

    for (auto _ : state) {
        RusterizationContext new_r {r};
        new_r.allShaders();
    }
}
BENCHMARK(all_shaders);


BENCHMARK_MAIN();
