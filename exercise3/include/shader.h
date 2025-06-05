#pragma once

#include "buffer.h"

#include <array>
#include <cassert>
#include <cstdint>
#include <optional>
#include <random>
#include <tuple>

template <typename BufferOutT>
class IShader {
public:
	explicit IShader(BufferOutT* outBuffer)
		: mOutBuffer(outBuffer) {}
	virtual ~IShader() = default;
	virtual void shade() = 0;

protected:
	BufferOutT* mOutBuffer;
};

// -----------------------------------------------------------------------------------------------------------

template <typename BufferOutT>
class Shader2D : public IShader<BufferOutT> {
public:
	explicit Shader2D(BufferOutT* outBuffer)
		: IShader<BufferOutT>(outBuffer) {}

	void shade() override {
		int const width = this->mOutBuffer->width;
		int const height = this->mOutBuffer->height;
		for (int x = 0; x < width; ++x) {
			for (int y = 0; y < height; ++y) {
				ColorRGB const& inColor = this->mOutBuffer->get({ x, y });
				shadeInt(x, y, inColor);
			}
		}
	}

protected:
	virtual void shadeInt(int const x, int const y, ColorRGB const& inColor) = 0;
	void setColor(int x, int y, ColorRGB const& color) {
		this->mOutBuffer->set({ x, y }, color);
	}
};

// -----------------------------------------------------------------------------------------------------------

class MaterialShader : public Shader2D<FrameBuffer> {
public:
	MaterialShader(FrameBuffer* fb, std::vector<int> const& materialIndices)
		: Shader2D(fb), mMaterialIndices(materialIndices) {}

protected:
	void shadeInt(int const x, int const y, ColorRGB const& inColor) override {
		int const matIndex = mMaterialIndices[this->mOutBuffer->toIdx(x, y)];
		shadeInt(x, y, inColor, matIndex);
	}

	virtual void shadeInt(int const x, int const y, ColorRGB const& inColor, int const matIndex) = 0;
	std::vector<int> const& mMaterialIndices;
};

// -----------------------------------------------------------------------------------------------------------


class MaterialPointShader : public MaterialShader {
public:
	MaterialPointShader(FrameBuffer* fb, std::vector<int> const& materialIndices, std::array<PointStamp, 16> const& stamps)
		: MaterialShader(fb, materialIndices), mStamps(stamps) {}

protected:
	/* TODO: implement */
	void shadeInt(int x, int y, ColorRGB const& inColor, int const matIndex) override {
		(void)x;
		(void)y;
		(void)inColor;
		(void)matIndex;
		return;
	}

private:
	std::array<PointStamp, 16> mStamps;
};

// -----------------------------------------------------------------------------------------------------------

class RustShader : public MaterialShader {
public:
	RustShader(FrameBuffer* fb, std::vector<int> const& materialIndices)
		: MaterialShader(fb, materialIndices) {}


	void shadeInt(int const x, int const y, ColorRGB const& inColor, int const matIndex) override {
		ColorRGB outColor = inColor;
		auto [rMin, rMax, gMin, gMax, bMin, bMax, rustProbability] = getRustParams(matIndex);
		std::vector<std::pair<int, uint32_t>> hashes;

		hashes.push_back({16, hashPixel(x, y, matIndex, 16)});
		hashes.push_back({8, hashPixel(x, y, matIndex, 8)});
		hashes.push_back({4, hashPixel(x, y, matIndex, 4)});
		hashes.push_back({2, hashPixel(x, y, matIndex, 2)});
		hashes.push_back({1, hashPixel(x, y, matIndex, 1) });

		for (auto [bs, hash] : hashes) {
			if ((hash % 100) < static_cast<uint32_t>(rustProbability) && matIndex >= 0) {
				int r = rMin + (hash % (rMax - rMin + 1));
				int g = gMin + ((hash >> 8) % (gMax - gMin + 1));
				int b = bMin + ((hash >> 16) % (bMax - bMin + 1));

				r = std::min(45, r),
				g = std::min(15, g),
				b = std::max(b, 30);

				outColor[0] = (2 * bs * inColor[0] + r) / (2 * bs + 1);
				outColor[1] = (2 * bs * inColor[1] + g) / (2 * bs + 1);
				outColor[2] = (2 * bs * inColor[2] + b) / (2 * bs + 1);
			}
		}
		setColor(x, y, outColor);
	}

private:
	static uint32_t hashPixel(int x, int y, int mat, int blockSize) {
		x /= blockSize;
		y /= blockSize;

		uint32_t h = static_cast<uint32_t>(x) * 73856093u ^
			static_cast<uint32_t>(y) * 19349663u ^
			static_cast<uint32_t>(mat) * 83492791u;

		h ^= (h >> 13); h *= 0x85ebca6bu;
		h ^= (h >> 16); h *= 0xc2b2ae35u;
		h ^= (h >> 16);
		return h;
	}

	std::tuple<int, int, int, int, int, int, int> getRustParams(int matId) {
		switch (matId) {
		case 0:  return { 120, 140, 70, 90, 60, 80, 10 };  
		case 1:  return { 120, 150, 70, 90, 60, 80, 30 };  
		case 2:  return { 130, 160, 80, 100, 70, 90, 40 }; 
		case 3:  return { 90, 120, 60, 80, 50, 70, 5 };    
		case 4:  return { 140, 170, 80, 100, 70, 90, 60 }; 
		case 5:  return { 120, 150, 80, 100, 60, 80, 25 }; 
		case 6:  return { 110, 140, 70, 90, 50, 70, 15 };  
		case 7:  return { 110, 140, 70, 90, 50, 70, 10 };  
		case 8:  return { 100, 130, 60, 80, 50, 70, 5 };   
		case 9:  return { 90, 120, 80, 100, 70, 90, 30 };  
		case 10: return { 120, 150, 80, 100, 60, 80, 30 }; 
		case 11: return { 120, 150, 80, 100, 60, 80, 30 }; 
		case 12: return { 110, 130, 70, 90, 60, 80, 3 };   
		case 13: return { 100, 120, 70, 90, 50, 70, 2 };   
		case 14: return { 120, 150, 80, 100, 60, 80, 40 }; 
		case 15: return { 110, 140, 70, 90, 60, 80, 20 };  
		default: return { 100, 130, 70, 90, 60, 80, 10 };
		}
	}
};

// -----------------------------------------------------------------------------------------------------------

class FoilShader : public MaterialShader {
public:
	FoilShader(FrameBuffer* fb, std::vector<int> const& materialIndices)
		: MaterialShader(fb, materialIndices) {}


	void shadeInt(int const x, int const y, ColorRGB const& inColor, int const matIndex) override {

		const float pi = 4.0f * std::atan(1.0f);
		float const time = 0.1f;
		float const waveAmplitude = 50.0f;

		float const waveFrequency = getMaterialFrequency(matIndex);
		float const sineValue = sinf((x + y + time) * waveFrequency * pi);

		ColorRGB outColor = inColor;

		outColor[0] += static_cast<int>(waveAmplitude * sineValue);
		outColor[1] += static_cast<int>(waveAmplitude * sineValue);
		outColor[2] += static_cast<int>(waveAmplitude * sineValue);

		outColor[0] = std::min(255, std::max(0, outColor[0]));
		outColor[1] = std::min(255, std::max(0, outColor[1]));
		outColor[2] = std::min(255, std::max(0, outColor[2]));

		setColor(x, y, outColor);
	}

private:
	float getMaterialFrequency(int matIndex) {
		switch (matIndex) {
		case 0:  return 0.125f;
		case 1:  return 0.25f;
		case 2:  return 0.25f;
		case 3:  return 0.25f;
		case 4:  return 0.0625f;
		case 5:  return 0.25f;
		case 6:  return 0.25f;
		case 7:  return 0.25f;
		case 8:  return 0.375f;
		case 9:  return 0.125f;
		case 10: return 0.25f;
		case 11: return 0.0625f;
		case 12: return 0.5f;
		case 13: return 0.25f;
		case 14: return 0.25f;
		case 15: return 0.25f;
		default: return 0.0f;
		}
	}
};

// -----------------------------------------------------------------------------------------------------------

class SmoothShader : public MaterialShader {
public:
	SmoothShader(FrameBuffer* fb, std::vector<int> const& materialIndices)
		: MaterialShader(fb, materialIndices) {}


	void shadeInt(int const x, int const y, ColorRGB const& inColor, int const matIndex) override {
		setColor(x, y, applySmoothing(x, y, matIndex, inColor));
	}

private:

	ColorRGB applySmoothing(int const x, int const y, int const matIndex, ColorRGB const inColor) {

		int windowSize = 1;
		int sumR = 0, sumG = 0, sumB = 0, count = 0;
		ColorRGB outColor = inColor;

		for (int dx = -windowSize; dx <= windowSize; ++dx) {
			for (int dy = -windowSize; dy <= windowSize; ++dy) {
				int nx = x + dx;
				int ny = y + dy;

				if (nx >= 0 && nx < mOutBuffer->width && ny >= 0 && ny < mOutBuffer->height) {
					int neighborMatIndex = mMaterialIndices.at(mOutBuffer->toIdx(nx, ny));

					if (neighborMatIndex == matIndex) {
						ColorRGB neighborColor = mOutBuffer->get({ nx, ny });
						sumR += neighborColor[0];
						sumG += neighborColor[1];
						sumB += neighborColor[2];
						++count;
					}
				}
			}
		}

		if (count > 0) {
			outColor[0] = sumR / count;
			outColor[1] = sumG / count;
			outColor[2] = sumB / count;
		}

		return outColor;
	}
};

