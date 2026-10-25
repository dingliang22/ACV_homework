#pragma once

#include "image.hpp"

#include <cstdint>

// RGB 顏色
struct Color {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
};

Image toRGB(const Image& image);

// 繪製矩形邊框
void drawRectangle(Image& image, int x, int y, int width, int height, Color color = {255, 0, 0}, int thickness = 2);

// 繪製十字標記
void drawCross(Image& image, int x, int y, int size = 5, Color color = {255, 0, 0}, int thickness = 1);

// 繪製線段
void drawLine(Image& image, int x1, int y1, int x2, int y2, Color color = {255, 0, 0}, int thickness = 1);
