#pragma once

#include "threshold.hpp"

#include <cstddef>
#include <filesystem>

struct Hw2_1Config {
    ColorThreshold roadThreshold;
    std::size_t minComponentArea = 50;
};

// inputPath:
//   原始影像資
//
// outputDir:
//   結果輸出資料夾
//
// config:
//   RGB Threshold、面積篩選等參數
void hw2_1(const std::filesystem::path& inputPath, const std::filesystem::path& outputDir, const Hw2_1Config& config);
