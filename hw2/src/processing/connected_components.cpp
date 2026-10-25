
#include "hw2/connected_components.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

ConnectedComponentsResult labelComponents(const Image& binary, uint8_t foreground) {
    // 1. 檢查輸入影像
    if (binary.empty()) {
        throw std::invalid_argument("Input image is empty");
    }

    if (binary.channels != 1) {
        throw std::invalid_argument("Connected components requires a single-channel image");
    }

    const int width = binary.width;
    const int height = binary.height;

    const std::size_t pixelCount = static_cast<std::size_t>(width) * height;

    if (binary.pixels.size() != pixelCount) {
        throw std::invalid_argument("Invalid image data size");
    }

    // 2. 初始化結果
    ConnectedComponentsResult result;

    result.width = width;
    result.height = height;
    result.labels.assign(pixelCount, 0);

    // 4-connectivity：右、左、下、上
    constexpr int dx[4] = {1, -1, 0, 0};
    constexpr int dy[4] = {0, 0, 1, -1};

    int currentLabel = 0;

    // BFS Queue：儲存像素的一維索引
    std::vector<std::size_t> queue;

    // 3. 掃描整張影像
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const std::size_t startIndex = static_cast<std::size_t>(y) * width + x;

            // 非前景像素：跳過
            if (binary.pixels[startIndex] != foreground) {
                continue;
            }

            // 已經屬於某個 Component：跳過
            if (result.labels[startIndex] != 0) {
                continue;
            }

            // 4. 發現新的 Component
            if (currentLabel == std::numeric_limits<int>::max()) {
                throw std::overflow_error("Too many components");
            }

            ++currentLabel;

            Component component;
            component.label = currentLabel;

            component.minX = x;
            component.maxX = x;
            component.minY = y;
            component.maxY = y;

            // 用於計算重心
            double sumX = 0.0;
            double sumY = 0.0;

            // 初始化 BFS
            queue.clear();

            // 發現時立即標記，避免重複加入
            result.labels[startIndex] = currentLabel;
            queue.push_back(startIndex);

            std::size_t head = 0;

            // 5. BFS：找出整個連通區域
            while (head < queue.size()) {
                const std::size_t index = queue[head++];

                const int cx = static_cast<int>(index % width);

                const int cy = static_cast<int>(index / width);

                // 計算面積
                ++component.area;

                // 累加座標（用於重心）
                sumX += cx;
                sumY += cy;

                // 更新 Bounding Box
                component.minX = std::min(component.minX, cx);
                component.maxX = std::max(component.maxX, cx);
                component.minY = std::min(component.minY, cy);
                component.maxY = std::max(component.maxY, cy);

                // 6. 檢查四個相鄰像素
                for (int k = 0; k < 4; ++k) {
                    const int nx = cx + dx[k];
                    const int ny = cy + dy[k];

                    // 檢查影像邊界
                    if (!binary.inBounds(nx, ny)) {
                        continue;
                    }

                    const std::size_t neighborIndex = static_cast<std::size_t>(ny) * width + nx;

                    // 必須是前景
                    if (binary.pixels[neighborIndex] != foreground) {
                        continue;
                    }

                    // 必須尚未被標記
                    if (result.labels[neighborIndex] != 0) {
                        continue;
                    }

                    // 先標記，再加入 Queue
                    result.labels[neighborIndex] = currentLabel;
                    queue.push_back(neighborIndex);
                }
            }

            // 7. BFS 結束：計算重心
            component.centroidX = sumX / static_cast<double>(component.area);

            component.centroidY = sumY / static_cast<double>(component.area);

            // 儲存完成的 Component
            result.components.push_back(component);
        }
    }

    return result;
}
