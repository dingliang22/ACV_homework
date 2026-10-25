#include "hw2/hw2_1.hpp"

#include "hw2/connected_components.hpp"
#include "hw2/drawing.hpp"
#include "hw2/threshold.hpp"

#include <chrono>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

void hw2_1(const fs::path& inputPath, const fs::path& outputDir, const Hw2_1Config& config) {
    // --------------------------------------------------
    // 1. 確認輸入圖片存在
    // --------------------------------------------------

    if (!fs::is_regular_file(inputPath)) {
        throw std::runtime_error("Input image does not exist: " + inputPath.string());
    }

    // --------------------------------------------------
    // 2. 確認輸出資料夾存在，不存在則建立
    // --------------------------------------------------

    if (fs::exists(outputDir)) {
        if (!fs::is_directory(outputDir)) {
            throw std::runtime_error("Output path is not a directory: " + outputDir.string());
        }
    } else {
        if (!fs::create_directories(outputDir) && !fs::is_directory(outputDir)) {
            throw std::runtime_error("Failed to create output directory: " + outputDir.string());
        }
    }

    // --------------------------------------------------
    // 3. 讀取圖片
    // --------------------------------------------------

    Image original;

    if (!original.load(inputPath)) {
        throw std::runtime_error("Failed to load image: " + inputPath.string());
    }

    std::cout << "\n========== HW2_1 ==========\n";
    std::cout << "Image: " << inputPath.filename() << '\n';
    std::cout << "Size: " << original.width << " x " << original.height << '\n';

    // --------------------------------------------------
    // 4. 開始計算時間
    // --------------------------------------------------

    const auto start = std::chrono::steady_clock::now();

    // --------------------------------------------------
    // 5. RGB Thresholding
    // --------------------------------------------------

    Image binary = binarize(original, config.roadThreshold);

    // --------------------------------------------------
    // 6. Connected Components (4-connectivity)
    // --------------------------------------------------

    ConnectedComponentsResult cc = labelComponents(binary);

    // --------------------------------------------------
    // 7. 建立彩色結果圖
    // --------------------------------------------------

    Image annotated = toRGB(original);

    std::size_t significantCount = 0;

    // --------------------------------------------------
    // 8. 分析並繪製顯著區域
    // --------------------------------------------------

    for (const Component& c : cc.components) {
        if (c.area < config.minComponentArea) {
            continue;
        }

        ++significantCount;

        // 紅色 Bounding Box
        drawRectangle(annotated, c.minX, c.minY, c.maxX - c.minX + 1, c.maxY - c.minY + 1, {255, 0, 0}, 2);

        // 綠色 Centroid
        drawCross(annotated, static_cast<int>(std::lround(c.centroidX)), static_cast<int>(std::lround(c.centroidY)), 5,
                  {0, 255, 0}, 2);

        // Terminal 輸出統計資訊
        std::cout << "\nComponent " << c.label << '\n';

        std::cout << "  Area: " << c.area << '\n';

        std::cout << std::fixed << std::setprecision(2);

        std::cout << "  Centroid: (" << c.centroidX << ", " << c.centroidY << ")\n";

        std::cout << "  Bounding Box: (" << c.minX << ", " << c.minY << ") - (" << c.maxX << ", " << c.maxY << ")\n";
    }

    // --------------------------------------------------
    // 9. 結束演算法計時
    // --------------------------------------------------

    const auto end = std::chrono::steady_clock::now();

    const double elapsedMs = std::chrono::duration<double, std::milli>(end - start).count();

    // --------------------------------------------------
    // 10. 建立輸出檔案路徑
    // --------------------------------------------------

    const std::string name = inputPath.stem().string();

    const fs::path binaryPath = outputDir / (name + "_binary.png");

    const fs::path annotatedPath = outputDir / (name + "_annotated.png");

    // --------------------------------------------------
    // 11. 輸出圖片
    // --------------------------------------------------

    if (!binary.save(binaryPath)) {
        throw std::runtime_error("Failed to save: " + binaryPath.string());
    }

    if (!annotated.save(annotatedPath)) {
        throw std::runtime_error("Failed to save: " + annotatedPath.string());
    }

    // --------------------------------------------------
    // 12. Terminal 摘要
    // --------------------------------------------------

    std::cout << "\n---------- Summary ----------\n";

    std::cout << "Total Components: " << cc.components.size() << '\n';

    std::cout << "Significant Components: " << significantCount << '\n';

    std::cout << "Processing Time: " << elapsedMs << " ms\n";

    std::cout << "Binary: " << binaryPath << '\n';

    std::cout << "Annotated: " << annotatedPath << '\n';

    std::cout << "=============================\n";
}
