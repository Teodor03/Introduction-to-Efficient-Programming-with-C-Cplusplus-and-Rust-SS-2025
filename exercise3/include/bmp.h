#pragma once

/* Can not be changed */

#include <cstring>
#include <fstream>
#include <array>
#include <iostream>
#include <string>
#include <vector>

class BMP {
public:

    BMP(const std::string& filename) : m_width(0), m_height(0), m_rowIncrement(0), m_bytesPerPixel(0) {
        loadBMP(filename);
    }

    BMP(const unsigned int width, const unsigned int height) : m_width(width), m_height(height), m_rowIncrement(0), m_bytesPerPixel(3) {
        m_rowIncrement = m_width * m_bytesPerPixel;
        m_data.resize(m_height * m_rowIncrement);
    }

    inline void setPixel(const unsigned int x, const unsigned int y,
        const unsigned char red,
        const unsigned char green,
        const unsigned char blue)
    {
        const unsigned int y_offset = y * m_rowIncrement;
        const unsigned int x_offset = x * m_bytesPerPixel;
        const unsigned int offset = y_offset + x_offset;

        m_data[offset + 0] = blue;
        m_data[offset + 1] = green;
        m_data[offset + 2] = red;
    }

    void save(const std::string& fileName) const {
        std::ofstream stream(fileName.c_str(), std::ios::binary);

        if (!stream) {
            std::cerr << "Error: Could not open file " << fileName << " for writing!" << std::endl;
            std::exit(1);
        }

        InformationHeader bmpIH;

        bmpIH.width = m_width;
        bmpIH.height = m_height;
        bmpIH.bit_count = static_cast<unsigned short>(m_bytesPerPixel << 3);
        bmpIH.clr_important = 0;
        bmpIH.clr_used = 0;
        bmpIH.compression = 0;
        bmpIH.planes = 1;
        bmpIH.size = bmpIH.getSize();
        bmpIH.x_pels_per_meter = 0;
        bmpIH.y_pels_per_meter = 0;
        bmpIH.size_image = (((bmpIH.width * m_bytesPerPixel) + 3) & 0x0000FFFC) * bmpIH.height;

        FileHeader bmpFH;

        bmpFH.type = 19778;
        bmpFH.size = bmpFH.getSize() + bmpIH.getSize() + bmpIH.size_image;
        bmpFH.reserved1 = 0;
        bmpFH.reserved2 = 0;
        bmpFH.off_bits = bmpIH.getSize() + bmpFH.getSize();

        writeFileHeader(stream, bmpFH);
        writeInformationHeader(stream, bmpIH);

        unsigned int padding = (4 - ((3 * m_width) % 4)) % 4;
        char padding_data[4] = { 0x00, 0x00, 0x00, 0x00 };

        for (unsigned int i = 0; i < m_height; ++i) {
            const unsigned char* data_ptr = &m_data[(m_rowIncrement * (m_height - i - 1))];

            stream.write(reinterpret_cast<const char*>(data_ptr), sizeof(unsigned char) * m_bytesPerPixel * m_width);
            stream.write(padding_data, padding);
        }

        stream.close();
    }

    inline unsigned int getWidth() const {
        return m_width;
	}

    inline unsigned int getHeight() const {         
        return m_height;
    }

    std::array<unsigned int, 3> getPixelRGB(const unsigned int x, const unsigned int y) const {
        const unsigned int y_offset = y * m_rowIncrement;
        const unsigned int x_offset = x * m_bytesPerPixel;
        const unsigned int offset = y_offset + x_offset;
        return { m_data[offset + 2], m_data[offset + 1], m_data[offset + 0] }; 
	}

private:

    inline unsigned char* row(unsigned int row_index) const {
        return const_cast<unsigned char*>(&m_data[(row_index * m_rowIncrement)]);
    }

    struct FileHeader {
        unsigned short type;
        unsigned int   size;
        unsigned short reserved1;
        unsigned short reserved2;
        unsigned int   off_bits;

        unsigned int getSize() const {
            return sizeof(type) +
                sizeof(size) +
                sizeof(reserved1) +
                sizeof(reserved2) +
                sizeof(off_bits);
        }

        void clear() {
            std::memset(this, 0x00, sizeof(FileHeader));
        }
    };

    struct InformationHeader {
        unsigned int   size;
        unsigned int   width;
        unsigned int   height;
        unsigned short planes;
        unsigned short bit_count;
        unsigned int   compression;
        unsigned int   size_image;
        unsigned int   x_pels_per_meter;
        unsigned int   y_pels_per_meter;
        unsigned int   clr_used;
        unsigned int   clr_important;

        unsigned int getSize() const {
            return sizeof(size) +
                sizeof(width) +
                sizeof(height) +
                sizeof(planes) +
                sizeof(bit_count) +
                sizeof(compression) +
                sizeof(size_image) +
                sizeof(x_pels_per_meter) +
                sizeof(y_pels_per_meter) +
                sizeof(clr_used) +
                sizeof(clr_important);
        }

        void clear() {
            std::memset(this, 0x00, sizeof(InformationHeader));
        }
    };

    template <typename T>
    inline void readFrom(std::ifstream& stream, T& t) {
        stream.read(reinterpret_cast<char*>(&t), sizeof(T));
    }

    template <typename T>
    inline void writeTo(std::ofstream& stream, const T& t) const {
        stream.write(reinterpret_cast<const char*>(&t), sizeof(T));
    }

    inline void readFileHeader(std::ifstream& stream, FileHeader& bfh) {
        readFrom(stream, bfh.type);
        readFrom(stream, bfh.size);
        readFrom(stream, bfh.reserved1);
        readFrom(stream, bfh.reserved2);
        readFrom(stream, bfh.off_bits);
    }

    inline void writeFileHeader(std::ofstream& stream, const FileHeader& bfh) const {
        writeTo(stream, bfh.type);
        writeTo(stream, bfh.size);
        writeTo(stream, bfh.reserved1);
        writeTo(stream, bfh.reserved2);
        writeTo(stream, bfh.off_bits);
    }

    inline void readInformationHeader(std::ifstream& stream, InformationHeader& bih) {
        readFrom(stream, bih.size);
        readFrom(stream, bih.width);
        readFrom(stream, bih.height);
        readFrom(stream, bih.planes);
        readFrom(stream, bih.bit_count);
        readFrom(stream, bih.compression);
        readFrom(stream, bih.size_image);
        readFrom(stream, bih.x_pels_per_meter);
        readFrom(stream, bih.y_pels_per_meter);
        readFrom(stream, bih.clr_used);
        readFrom(stream, bih.clr_important);
    }

    inline void writeInformationHeader(std::ofstream& stream, const InformationHeader& bih) const {
        writeTo(stream, bih.size);
        writeTo(stream, bih.width);
        writeTo(stream, bih.height);
        writeTo(stream, bih.planes);
        writeTo(stream, bih.bit_count);
        writeTo(stream, bih.compression);
        writeTo(stream, bih.size_image);
        writeTo(stream, bih.x_pels_per_meter);
        writeTo(stream, bih.y_pels_per_meter);
        writeTo(stream, bih.clr_used);
        writeTo(stream, bih.clr_important);
    }

    void loadBMP(const std::string& filename) {
        std::ifstream stream(filename.c_str(), std::ios::binary);

        if (!stream) {
            std::cerr << "ERROR: file " << filename << " not found!" << std::endl;
            std::exit(1);
        }

        m_width = 0;
        m_height = 0;

        FileHeader bfh;
        InformationHeader bih;

        bfh.clear();
        bih.clear();

        readFileHeader(stream, bfh);
        readInformationHeader(stream, bih);

        if (bfh.type != 19778) {
            std::cerr << "ERROR: invalid type value " << bfh.type << ". Expected 19778." << std::endl;
            std::exit(1);
        }

        if (bih.bit_count != 24)
        {
            std::cerr << "bitmap_image::load_bitmap() ERROR: bitmap_image - "
                << "Invalid bit depth " << bih.bit_count << " expected 24." << std::endl;
            std::exit(1);
        }

        if (bih.size != bih.getSize())
        {
            std::cerr << "bitmap_image::load_bitmap() ERROR: bitmap_image - "
                << "Invalid BIH size " << bih.size
                << " expected " << bih.getSize() << std::endl;
            std::exit(1);
        }

        m_width = bih.width;
        m_height = bih.height;

        m_bytesPerPixel = bih.bit_count >> 3;

        unsigned int padding = (4 - ((3 * m_width) % 4)) % 4;
        char padding_data[4] = { 0x00, 0x00, 0x00, 0x00 };

        m_rowIncrement = m_width * m_bytesPerPixel;
        m_data.resize(m_height * m_rowIncrement);

        for (unsigned int i = 0; i < m_height; ++i) {
            unsigned char* data_ptr = row(m_height - i - 1); // read in inverted row order

            stream.read(reinterpret_cast<char*>(data_ptr), sizeof(char) * m_bytesPerPixel * m_width);
            stream.read(padding_data, padding);
        }
    }

    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_rowIncrement;
    unsigned int m_bytesPerPixel;
    std::vector<unsigned char> m_data;
};
