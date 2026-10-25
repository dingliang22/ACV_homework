#include "hw2/common.hpp"
#include "hw2/hw2_1.hpp"

#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

int main() {
    // 尋找圖片資料夾
    fs::path imageDir = findFolder(fs::current_path(), "hw2");

    if (imageDir.empty()) {
        std::cerr << "Cannot find hw2_img folder\n";
        return 1;
    }

    // 輸出資料夾
    fs::path outputDir = imageDir.parent_path() / "hw2_img_result";

    while (true) {
        std::cout << "\n===== ACV HW2 =====\n";
        std::cout << "1. HW2_1\n";
        std::cout << "2. HW2_2\n";
        std::cout << "0. Exit\n";
        std::cout << "Select: ";

        std::string choice = "1";
        // if (!std::getline(std::cin, choice)) {
        //     break;
        // }

        try {
            if (choice == "1") {
                Hw2_1Config config;

                config.roadThreshold = {
                    {150, 250}, // Red
                    {140, 180}, // Green
                    {0, 150}    // Blue
                };
                config.minComponentArea = 50;
                fs::path inputPath = imageDir / "forest_junction_sat.jpg";

                hw2_1(inputPath, outputDir / "hw2_1", config);
            } else if (choice == "2") {
                // TODO: hw2_2()
            } else if (choice == "0") {
                break;
            } else {
                std::cout << "Invalid selection\n";
            }
            return 0;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << '\n';
        }
    }

    return 0;
}
