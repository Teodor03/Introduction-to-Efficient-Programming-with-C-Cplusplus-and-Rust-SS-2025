#include <benchmark/benchmark.h>

#include "color.h"
#include "sticker.h"
#include "bench_setup.h"


std::vector<ConvexPolygon> random_cutout(int seed, int n, int width, int height, StickerFactory::StickerSizeDistributionT type) {
    StickerFactory factory(width, height, seed, type);
    auto stickers = factory.generateStickers(n);
    std::vector<ConvexPolygon> cutout;
    for (const auto& s : stickers) {
        cutout.emplace_back(s.polygon);
    }
    return cutout;
}


static void cutout_multiple_large(benchmark::State &state) {
    auto type = StickerFactory::StickerSizeDistributionT::Default;
    int seed = 17, n = 10000, width = 512, height = 512;
    StickerFactory factory(width, height, seed, type);
    auto stickers = factory.generateStickers(n);
    ColorPalette<16> palette = factory.getColorPalette();

    auto cutout = random_cutout(seed, 100, width, height, StickerFactory::StickerSizeDistributionT::Default);

    for (auto _ : state) {
        RusterizationContext r = createRusterizer(width, height, palette);
        r.rasterize(stickers, palette, cutout);
    }
}
BENCHMARK(cutout_multiple_large);


static void cutout_many_small(benchmark::State &state) {
    auto type = StickerFactory::StickerSizeDistributionT::Default;
    int seed = 18, n = 10000, width = 512, height = 512;
    StickerFactory factory(width, height, seed, type);
    auto stickers = factory.generateStickers(n);
    ColorPalette<16> palette = factory.getColorPalette();

    auto cutout = random_cutout(seed, 500, width, height, StickerFactory::StickerSizeDistributionT::Small);

    for (auto _ : state) {
        RusterizationContext r = createRusterizer(width, height, palette);
        r.rasterize(stickers, palette, cutout);
    }
}
BENCHMARK(cutout_many_small);


BENCHMARK_MAIN();
