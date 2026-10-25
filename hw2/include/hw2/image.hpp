#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>

struct Image {
    int width = 0;
    int height = 0;
    int channels = 0;

    std::vector<uint8_t> pixels;

    // 建構函式
    Image() = default;
    Image(int w, int h, int c = 3);

    // 圖片讀寫
    bool load(const std::filesystem::path& path);
    bool save(const std::filesystem::path& path) const;

    // 基本檢查
    bool empty() const;
    bool inBounds(int x, int y) const;

    // 像素存取
    uint8_t& at(int x, int y, int c = 0);
    const uint8_t& at(int x, int y, int c = 0) const;
};
