#include "hw2/drawing.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace {
    // 檢查影像是否適合 RGB 繪圖
    void requireRGB(const Image& image) {
        if (image.empty() || image.channels != 3) {
            throw std::invalid_argument("Drawing requires a non-empty RGB image");
        }

        const std::size_t expected = static_cast<std::size_t>(image.width) * image.height * 3;

        if (image.pixels.size() != expected) {
            throw std::invalid_argument("Invalid RGB image data size");
        }
    }

    // 填滿矩形區域，並自動裁切至影像範圍
    void fillRectangle(Image& image, int64_t x1, int64_t y1, int64_t x2, int64_t y2, Color color) {
        x1 = std::max<int64_t>(0, x1);
        y1 = std::max<int64_t>(0, y1);

        x2 = std::min<int64_t>(image.width - 1, x2);
        y2 = std::min<int64_t>(image.height - 1, y2);

        if (x1 > x2 || y1 > y2) {
            return;
        }

        for (int64_t y = y1; y <= y2; ++y) {
            for (int64_t x = x1; x <= x2; ++x) {
                const std::size_t index = (static_cast<std::size_t>(y) * image.width + x) * 3;

                image.pixels[index] = color.r;
                image.pixels[index + 1] = color.g;
                image.pixels[index + 2] = color.b;
            }
        }
    }

    // 在單一像素附近繪製方形筆刷
    void paintBrush(Image& image, int64_t x, int64_t y, Color color, int thickness) {
        const int64_t lower = (thickness - 1) / 2;
        const int64_t upper = thickness / 2;

        fillRectangle(image, x - lower, y - lower, x + upper, y + upper, color);
    }

    // 線段裁切：Liang-Barsky Algorithm
    // 裁切至影像範圍，避免在畫面外進行大量迴圈
    bool clipLine(const Image& image, double& x1, double& y1, double& x2, double& y2) {
        const double dx = x2 - x1;
        const double dy = y2 - y1;

        double t0 = 0.0;
        double t1 = 1.0;

        auto clip = [&](double p, double q) -> bool {
            if (p == 0.0) {
                return q >= 0.0;
            }

            const double r = q / p;

            if (p < 0.0) {
                t0 = std::max(t0, r);
            } else {
                t1 = std::min(t1, r);
            }

            return t0 <= t1;
        };

        if (!clip(-dx, x1) || !clip(dx, image.width - 1.0 - x1) || !clip(-dy, y1) ||
            !clip(dy, image.height - 1.0 - y1)) {
            return false;
        }

        const double originalX = x1;
        const double originalY = y1;

        x1 = originalX + t0 * dx;
        y1 = originalY + t0 * dy;

        x2 = originalX + t1 * dx;
        y2 = originalY + t1 * dy;

        return true;
    }

    // 供 drawLine 和 drawCross 共用
    void drawLineInternal(Image& image, int64_t startX, int64_t startY, int64_t endX, int64_t endY, Color color,
                          int thickness) {
        double x1 = static_cast<double>(startX);
        double y1 = static_cast<double>(startY);
        double x2 = static_cast<double>(endX);
        double y2 = static_cast<double>(endY);

        if (!clipLine(image, x1, y1, x2, y2)) {
            return;
        }

        auto roundX = [&](double x) -> int64_t {
            return std::clamp<int64_t>(static_cast<int64_t>(std::llround(x)), 0, image.width - 1);
        };

        auto roundY = [&](double y) -> int64_t {
            return std::clamp<int64_t>(static_cast<int64_t>(std::llround(y)), 0, image.height - 1);
        };

        int64_t x = roundX(x1);
        int64_t y = roundY(y1);

        const int64_t tx = roundX(x2);
        const int64_t ty = roundY(y2);

        // Bresenham Line Algorithm
        const int64_t dx = std::abs(tx - x);
        const int64_t dy = std::abs(ty - y);

        const int64_t sx = (x < tx) ? 1 : -1;
        const int64_t sy = (y < ty) ? 1 : -1;

        int64_t error = dx - dy;

        while (true) {
            paintBrush(image, x, y, color, thickness);

            if (x == tx && y == ty) {
                break;
            }

            const int64_t e2 = 2 * error;

            if (e2 > -dy) {
                error -= dy;
                x += sx;
            }

            if (e2 < dx) {
                error += dx;
                y += sy;
            }
        }
    }
} // namespace

Image toRGB(const Image& image) {
    if (image.empty()) {
        throw std::invalid_argument("Input image is empty");
    }

    const std::size_t pixelCount = static_cast<std::size_t>(image.width) * image.height;

    if (image.pixels.size() != pixelCount * image.channels) {
        throw std::invalid_argument("Invalid image data size");
    }

    if (image.channels == 3) {
        return image;
    }

    Image result(image.width, image.height, 3);

    for (std::size_t i = 0; i < pixelCount; ++i) {
        const std::size_t src = i * image.channels;
        const std::size_t dst = i * 3;

        if (image.channels == 1 || image.channels == 2) {
            // Gray / Gray+Alpha → RGB
            const uint8_t gray = image.pixels[src];

            result.pixels[dst] = gray;
            result.pixels[dst + 1] = gray;
            result.pixels[dst + 2] = gray;
        } else {
            // RGBA → RGB，忽略 Alpha
            result.pixels[dst] = image.pixels[src];
            result.pixels[dst + 1] = image.pixels[src + 1];
            result.pixels[dst + 2] = image.pixels[src + 2];
        }
    }

    return result;
}

void drawRectangle(Image& image, int x, int y, int width, int height, Color color, int thickness) {
    requireRGB(image);

    if (width <= 0 || height <= 0 || thickness <= 0) {
        throw std::invalid_argument("Invalid rectangle dimensions or thickness");
    }

    const int64_t left = x;
    const int64_t top = y;

    const int64_t right = left + static_cast<int64_t>(width) - 1;

    const int64_t bottom = top + static_cast<int64_t>(height) - 1;

    const int64_t t = thickness;

    // 四條邊的線寬向矩形內部延伸
    fillRectangle(image, left, top, right, top + t - 1, color);

    fillRectangle(image, left, bottom - t + 1, right, bottom, color);

    fillRectangle(image, left, top, left + t - 1, bottom, color);

    fillRectangle(image, right - t + 1, top, right, bottom, color);
}

// ============================================================
// 繪製直線
// ============================================================

void drawLine(Image& image, int x1, int y1, int x2, int y2, Color color, int thickness) {
    requireRGB(image);

    if (thickness <= 0) {
        throw std::invalid_argument("Line thickness must be positive");
    }

    drawLineInternal(image, x1, y1, x2, y2, color, thickness);
}

// ============================================================
// 繪製十字標記
// ============================================================

void drawCross(Image& image, int x, int y, int size, Color color, int thickness) {
    requireRGB(image);

    if (size < 0 || thickness <= 0) {
        throw std::invalid_argument("Invalid cross size or thickness");
    }

    // 水平線
    drawLineInternal(image, static_cast<int64_t>(x) - size, y, static_cast<int64_t>(x) + size, y, color, thickness);

    // 垂直線
    drawLineInternal(image, x, static_cast<int64_t>(y) - size, x, static_cast<int64_t>(y) + size, color, thickness);
}
