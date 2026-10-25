#include "hw2/threshold.hpp"

#include <cstddef>
#include <stdexcept>

// 檢查數值是否落在指定範圍內（包含上下限）
static bool inRange(uint8_t value, const ChannelRange& range) {
    return value >= range.min && value <= range.max;
}

// 根據 RGB 閾值產生二值遮罩
Image binarize(const Image& image, const ColorThreshold& threshold) {
    if (image.empty()) {
        throw std::invalid_argument("Input image is empty");
    }

    if (image.channels < 1 || image.channels > 4) {
        throw std::invalid_argument("Unsupported channel count");
    }

    // 確認閾值範圍有效
    if (threshold.red.min > threshold.red.max || threshold.green.min > threshold.green.max ||
        threshold.blue.min > threshold.blue.max) {
        throw std::invalid_argument("Invalid threshold range");
    }

    Image result(image.width, image.height, 1);

    const size_t pixelCount = static_cast<size_t>(image.width) * image.height;

    for (size_t i = 0; i < pixelCount; ++i) {
        const size_t index = i * image.channels;

        uint8_t r, g, b;

        if (image.channels == 1 || image.channels == 2) {
            // Gray 或 Gray + Alpha
            r = g = b = image.pixels[index];
        } else {
            // RGB 或 RGBA
            r = image.pixels[index];
            g = image.pixels[index + 1];
            b = image.pixels[index + 2];
        }

        // 三個通道必須全部符合條件
        const bool isForeground =
            inRange(r, threshold.red) && inRange(g, threshold.green) && inRange(b, threshold.blue);

        result.pixels[i] = isForeground ? 255 : 0;
    }

    return result;
}
