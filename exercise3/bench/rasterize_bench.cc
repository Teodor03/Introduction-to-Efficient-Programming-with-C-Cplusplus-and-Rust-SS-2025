#include <benchmark/benchmark.h>

#include "color.h"
#include "sticker.h"
#include "kit_fan.h"
#include "bench_setup.h"


static void initialize_rusterizer(benchmark::State &state) {
    auto type = StickerFactory::StickerSizeDistributionT::Default;
    int seed = 0, width = 1024, height = 1024;
    StickerFactory factory(width, height, seed, type);
    auto stickers = factory.generateStickers(1);
    ColorPalette<16> palette = factory.getColorPalette();

    for (auto _ : state) {
        RusterizationContext r = createRusterizer(width, height, palette);
    }
}
BENCHMARK(initialize_rusterizer);


static void rasterize_default_size(benchmark::State &state) {
    auto type = StickerFactory::StickerSizeDistributionT::Default;
    int seed = 1, n = 5000, width = 1024, height = 1024;
    StickerFactory factory(width, height, seed, type);
    auto stickers = factory.generateStickers(n);
    ColorPalette<16> palette = factory.getColorPalette();

    for (auto _ : state) {
        RusterizationContext r = createRusterizer(width, height, palette);
        r.rasterize(stickers, palette, {});
    }
}
BENCHMARK(rasterize_default_size);


static void rasterize_small_size(benchmark::State &state) {
    auto type = StickerFactory::StickerSizeDistributionT::Small;
    int seed = 2, n = 100000, width = 1024, height = 1024;
    StickerFactory factory(width, height, seed, type);
    auto stickers = factory.generateStickers(n);
    ColorPalette<16> palette = factory.getColorPalette();

    for (auto _ : state) {
        RusterizationContext r = createRusterizer(width, height, palette);
        r.rasterize(stickers, palette, {});
    }
}
BENCHMARK(rasterize_small_size);


static void rasterize_large_size(benchmark::State &state) {
    auto type = StickerFactory::StickerSizeDistributionT::Large;
    int seed = 3, n = 500, width = 2048, height = 2048;
    StickerFactory factory(width, height, seed, type);
    auto stickers = factory.generateStickers(n);
    ColorPalette<16> palette = factory.getColorPalette();

    for (auto _ : state) {
        RusterizationContext r = createRusterizer(width, height, palette);
        r.rasterize(stickers, palette, {});
    }
}
BENCHMARK(rasterize_large_size);


static void rasterize_mixed(benchmark::State &state) {
    auto type = StickerFactory::StickerSizeDistributionT::Mixed;
    int seed = 4, n = 2000, width = 1024, height = 1024;
    StickerFactory factory(width, height, seed, type);
    auto stickers = factory.generateStickers(n);
    ColorPalette<16> palette = factory.getColorPalette();

    for (auto _ : state) {
        RusterizationContext r = createRusterizer(width, height, palette);
        r.rasterize(stickers, palette, {});
    }
}
BENCHMARK(rasterize_mixed);


static void kit_fan(benchmark::State &state) {
    auto type = StickerFactory::StickerSizeDistributionT::Default;
    int seed = 7, n = 10000, width = 1024, height = 1024;
    StickerFactory factory(width, height, seed, type);
    auto stickers = factory.generateStickers(n);
    ColorPalette<16> palette = factory.getColorPalette();

    for (auto _ : state) {
        RusterizationContext r = createRusterizer(width, height, palette);
        r.rasterize(stickers, palette, getKitFan(width, height));
        r.allShaders();
    }
}
BENCHMARK(kit_fan);


BENCHMARK_MAIN();
