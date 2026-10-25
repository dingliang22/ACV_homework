#pragma once

#include "image.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

// 單一連通區域的資訊
struct Component {
    // 連通區域編號（從 1 開始）
    int label = 0;

    // 面積：前景像素數量
    std::size_t area = 0;

    // 重心座標
    double centroidX = 0.0;
    double centroidY = 0.0;

    // Bounding Box 邊界（包含端點）
    int minX = 0;
    int minY = 0;
    int maxX = 0;
    int maxY = 0;
};

// 整張影像的連通區域分析結果
struct ConnectedComponentsResult {
    int width = 0;
    int height = 0;

    std::vector<int> labels;

    std::vector<Component> components;
};

ConnectedComponentsResult labelComponents(const Image& binary, uint8_t foreground = 255);
