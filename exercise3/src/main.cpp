#include "bmp.h"
#include "color.h"
#include "geom.h"
#include "grid.h"
#include "rusterizer.h"
#include "sticker.h"
#include "kit_fan.h"

#include <cassert>
#include <iostream>
#include <span>


int main(int argc, char* argv[]) {

    // Default
    int SEED = 7;
    int W = 1024;
    int H = 1024;
    int N = 10000;
    int TYPE = 0;
    int CUTOUT = 1;

    if (argc > 3) SEED = std::stoi(argv[1]);
    if (argc > 1) W = std::stoi(argv[2]);
    if (argc > 2) H = std::stoi(argv[3]);
    if (argc > 4) N = std::stoi(argv[4]);
    if (argc > 5) TYPE = std::stoi(argv[5]);
    if (argc > 6) CUTOUT = std::stoi(argv[6]);

    if ((W % 16) != 0 || (H % 16) != 0) {
        abort();
    }

    // create stickers
    StickerFactory factory(W, H, SEED, StickerFactory::StickerSizeDistributionT{ TYPE });
    auto stickers = factory.generateStickers(N);
    ColorPalette<16> palette = factory.getColorPalette();

    Rusterizer rusterizer(W, H, { 255, 255, 255 }, palette);

    // initial image
    rusterizer.writeFrameBuffer().save("00_initial.bmp");

    // rasterize stickers
    std::vector<ConvexPolygon> cutout;
    if (CUTOUT == 1) {
        cutout = getKitFan(W, H);
    }

    rusterizer.rasterizeStickersWithCutout(stickers, palette, cutout);

    // create images
    rusterizer.writeFrameBuffer().save("01_raster.bmp");
    rusterizer.stampMaterials();
    rusterizer.writeFrameBuffer().save("02_stamp.bmp");
    rusterizer.rustify();
    rusterizer.writeFrameBuffer().save("03_rustified.bmp");
    rusterizer.smoothen();
    rusterizer.writeFrameBuffer().save("04_smoothened.bmp");
    rusterizer.foil();
    rusterizer.writeFrameBuffer().save("05_final.bmp");

    auto stamps = rusterizer.writeStamps();
    for (size_t i = 0; i < stamps.size(); ++i) {
        auto const& stamp = stamps[i];
        stamp.save("stamp_" + std::to_string(i));
    }
    rusterizer.writeMaterialBuffer().save("06_materials.bmp");

    return 0;
}
