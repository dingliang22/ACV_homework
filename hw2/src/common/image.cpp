#include "hw2/image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <algorithm>
#include <cctype>
#include <limits>
#include <stb/stb_image_write.h>
#include <stdexcept>
#include <string>
#include <utility>

// 建立指定尺寸的影像
Image::Image(int w, int h, int c) {
    if (w <= 0 || h <= 0 || c < 1 || c > 4) {
        throw std::invalid_argument("Invalid image dimensions");
    }

    const size_t sw = static_cast<size_t>(w);
    const size_t sh = static_cast<size_t>(h);
    const size_t sc = static_cast<size_t>(c);

    if (sw > std::numeric_limits<size_t>::max() / sh || sw * sh > std::numeric_limits<size_t>::max() / sc) {
        throw std::overflow_error("Image size overflow");
    }

    width = w;
    height = h;
    channels = c;
    pixels.resize(sw * sh * sc, 0);
}

// 判斷是否為空影像
bool Image::empty() const {
    return width <= 0 || height <= 0 || channels < 1 || channels > 4 || pixels.empty();
}

// 判斷座標是否在影像範圍內
bool Image::inBounds(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

// 可修改的像素存取
uint8_t& Image::at(int x, int y, int c) {
    if (!inBounds(x, y) || c < 0 || c >= channels) {
        throw std::out_of_range("Pixel index out of range");
    }

    size_t index = (static_cast<size_t>(y) * width + x) * channels + c;

    return pixels.at(index);
}

// 唯讀的像素存取
const uint8_t& Image::at(int x, int y, int c) const {
    if (!inBounds(x, y) || c < 0 || c >= channels) {
        throw std::out_of_range("Pixel index out of range");
    }

    size_t index = (static_cast<size_t>(y) * width + x) * channels + c;

    return pixels.at(index);
}

// 讀取影像
bool Image::load(const std::filesystem::path& path) {
    int w = 0;
    int h = 0;
    int c = 0;

    unsigned char* data = stbi_load(path.string().c_str(), &w, &h, &c, 0);

    if (!data) {
        return false;
    }

    // 先建立暫存物件，成功後才取代原圖
    try {
        Image temp(w, h, c);

        std::copy_n(data, temp.pixels.size(), temp.pixels.begin());

        stbi_image_free(data);

        *this = std::move(temp);
        return true;
    } catch (...) {
        stbi_image_free(data);
        throw;
    }
}

// 儲存影像
bool Image::save(const std::filesystem::path& path) const {
    if (empty()) {
        return false;
    }

    const size_t expected = static_cast<size_t>(width) * height * channels;

    if (pixels.size() != expected) {
        return false;
    }

    std::string ext = path.extension().string();

    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

    const std::string filename = path.string();

    if (ext == ".png") {
        return stbi_write_png(filename.c_str(), width, height, channels, pixels.data(), width * channels) != 0;
    }

    if (ext == ".jpg" || ext == ".jpeg") {
        // JPEG 不支援 Alpha，這裡只接受灰階或 RGB
        if (channels != 1 && channels != 3) {
            return false;
        }

        return stbi_write_jpg(filename.c_str(), width, height, channels, pixels.data(), 95) != 0;
    }

    return false;
}
