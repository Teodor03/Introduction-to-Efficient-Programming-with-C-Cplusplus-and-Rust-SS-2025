#pragma once

#include "bmp.h"
#include "buffer.h"
#include "grid.h"
#include "shader.h"
#include "sticker.h"

#include <array>
#include <memory>
#include <random>

class Rusterizer {
public:

	Rusterizer(const Rusterizer& other):
		mWidth(other.mWidth),
		mHeight(other.mHeight),
		mFB(std::make_unique<FrameBuffer>(*other.mFB)),
		mMat(std::make_unique<MaterialBuffer>(*other.mMat)),
		mStamps(other.mStamps),
		mColorPalette(other.mColorPalette) { }

	struct RusterData {
		std::vector<Point> points;
		int paletteIndex;
		int materialIndex;
	};

	Rusterizer(int width, int height, ColorRGB backgroundColor, ColorPalette<16> const & colorPalette) : mWidth(width), mHeight(height), mColorPalette(colorPalette) {
		mFB = std::make_unique<FrameBuffer>(width, height, backgroundColor);
		mMat = std::make_unique<MaterialBuffer>(width, height, -1);

		initStamps();
	}

	/* TODO: implement */
	void rasterize(const std::vector<RusterData> & rds) {
		for (auto const & rd : rds) {
			rasterizePolygon(rd);
		}
	}

	/* TODO: implement */
	void stampMaterials() {
		return;
	}

	/* TODO: optimize */
	void rustify() {
		RustShader rustShader = RustShader(mFB.get(), mMat->values);
		rustShader.shade();
	}

	/* TODO: optimize */
	void smoothen() {
		SmoothShader smoothShader = SmoothShader(mFB.get(), mMat->values);
		smoothShader.shade();
	}

	/* TODO: optimize */
	void foil() {
		FoilShader foilShader = FoilShader(mFB.get(), mMat->values);
		foilShader.shade();
	}

	void allShaders() {
		stampMaterials();
		rustify();
		smoothen();
		foil();
	}

	/* TODO: optimize */
	void rasterizeStickersWithCutout(std::vector<Sticker>& stickers, const ColorPalette<16>& palette, const std::vector<ConvexPolygon>& cutout) {
		std::sort(stickers.begin(), stickers.end(), [](const Sticker& lhs, const Sticker& rhs) {
			return lhs.timestamp < rhs.timestamp;
		});

		assert((mWidth % 16) == 0 && (mHeight % 16) == 0);
		int const GRIDCOLS = 16;
		int const GRIDROWS = 16;

		// build grid
		Grid<Sticker> stickerGrid(mWidth, mHeight, GRIDROWS, GRIDCOLS);
		stickerGrid.insert(stickers);

		for (const auto& cutPolygon : cutout) {
			std::vector<Sticker*> overlapping = stickerGrid.getObjectsInsideOrOverlapping(cutPolygon);
			for (auto& sticker : overlapping) {
				sticker->materialID = -1; // mark as cutout
			}
		}

		// set up polygons
		std::vector<Rusterizer::RusterData> polys;
		for (const auto& sticker : stickers) {
			int const paletteIndex = palette.findClosestPaletteIndex(sticker.baseColor);
			int const materialIndex = sticker.materialID;
			Rusterizer::RusterData rd {sticker.getPoints(), paletteIndex, materialIndex};
			polys.emplace_back(rd);
		}

		// let the rusterizer rasterize
		rasterize(polys);
	}

	// output
	BMP writeFrameBuffer() const {
		BMP bmp = { (unsigned)mFB->width , (unsigned)mFB->height };
		for (size_t i = 0; i < mFB->values.size(); ++i) {
			int x = i % mFB->width;
			int y = i / mFB->width;
			bmp.setPixel(x, y, mFB->values[i][0], mFB->values[i][1], mFB->values[i][2]);
		}
		return bmp;
	}

	BMP writeMaterialBuffer() const {
		BMP bmp = { (unsigned)mMat->width , (unsigned)mMat->height };
		for (size_t i = 0; i < mMat->values.size(); ++i) {
			size_t x = i % mMat->width;
			size_t y = i / mMat->width;

			int const matIndex = mMat->values[i]; 
			int const r = (matIndex + 1) * 15;

			ColorRGB materialColor = matIndex == -1 ? ColorRGB({ 0, 0, 0 }) : ColorRGB({ r, 255 - r, 0 });
			bmp.setPixel(x, y, materialColor[0], materialColor[1], materialColor[2]);
		}
		return bmp;
	}

	const std::array<PointStamp, 16>& getStamps() const {
		return mStamps;
	}

	std::vector<BMP> writeStamps() {
		std::vector<BMP> result;
		for (auto const& stamp : mStamps) {
			result.push_back(stamp.write());
		}
		return result;
	}

private:
	int mWidth;
	int mHeight;
	std::unique_ptr<FrameBuffer> mFB;
	std::unique_ptr<MaterialBuffer> mMat;
	std::array<PointStamp, 16> mStamps;

	ColorPalette<16> mColorPalette;

	/* TODO */
	void rasterizePolygon(const RusterData & rd) {
		(void)rd;
		return;
	}

	void initStamps() {
		int arr[16] = { 1 };
		for (int i = 1; i < 16; ++i) arr[i] = 0;
		for (size_t i = 0; i < mStamps.size(); ++i) {
			for (int j = 0; j < 16; ++j) {
				mStamps[i].pointSet[j] = arr[j];
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
							mStamps[s - 1].set({ x, y });
						}
					}
				}
			}
		}

		for (auto & stamp : mStamps) {
			for (int i = 0; i < 16; ++i) {
				char stampFact = static_cast<char>(stampede);
				stamp.pointSet[i] = stamp.pointSet[i] + (stampFact - stampede);
			}
		}
	}
};
