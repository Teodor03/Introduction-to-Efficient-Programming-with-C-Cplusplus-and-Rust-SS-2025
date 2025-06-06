#pragma once

#include "sticker.h"
#include "color.h"

#include <memory>

class Rusterizer;

class RusterizationContext {
public:
    RusterizationContext(const RusterizationContext& other);

    RusterizationContext(std::unique_ptr<Rusterizer>&& inner);

    ~RusterizationContext();

    void rasterize(std::vector<Sticker>& stickers, const ColorPalette<16>& palette, const std::vector<ConvexPolygon>& cutout);

	void stampMaterials();

	void rustify();

	void smoothen();

	void foil();

	void allShaders();

    void saveImage(const std::string& filename);

private:
    std::unique_ptr<Rusterizer> inner;
};

RusterizationContext createRusterizer(int width, int height, ColorPalette<16> const & colorPalette);
