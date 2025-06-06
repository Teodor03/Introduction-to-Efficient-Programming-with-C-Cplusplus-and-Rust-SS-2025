#include "bench_setup.h"

#include "bmp.h"
#include "color.h"
#include "rusterizer.h"
#include "sticker.h"

RusterizationContext::RusterizationContext(const RusterizationContext& other): inner(std::make_unique<Rusterizer>(*other.inner)) {}

RusterizationContext::RusterizationContext(std::unique_ptr<Rusterizer>&& inner): inner(std::move(inner)) {}

RusterizationContext::~RusterizationContext() = default;

void RusterizationContext::rasterize(std::vector<Sticker>& stickers, const ColorPalette<16>& palette, const std::vector<ConvexPolygon>& cutout) {
    inner->rasterizeStickersWithCutout(stickers, palette, cutout);
}

void RusterizationContext::stampMaterials() {
    inner->stampMaterials();
}

void RusterizationContext::rustify() {
    inner->rustify();
}

void RusterizationContext::smoothen() {
    inner->smoothen();
}

void RusterizationContext::foil() {
    inner->foil();
}

void RusterizationContext::allShaders() {
    inner->allShaders();
}

void RusterizationContext::saveImage(const std::string& filename) {
    inner->writeFrameBuffer().save(filename);
}

RusterizationContext createRusterizer(int width, int height, ColorPalette<16> const & palette) {
    return { std::make_unique<Rusterizer>(Rusterizer {width, height, { 255, 255, 255 }, palette}) };
}
