#pragma once

#include "image.hpp"

#include <cstdint>

// 單一通道的閾值範圍
struct ChannelRange {
    uint8_t min = 0;
    uint8_t max = 255;
};

// RGB 三個通道的閾值
struct ColorThreshold {
    ChannelRange red;
    ChannelRange green;
    ChannelRange blue;
};

Image binarize(const Image& image, const ColorThreshold& threshold);
