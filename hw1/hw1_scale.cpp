#include <chrono>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
using namespace std;
using namespace cv;
namespace fs = std::filesystem;

void scaleBMP(const fs::path& inputPath, const fs::path& outputPath, double scale) {
    Mat src = imread(inputPath.string(), IMREAD_UNCHANGED);
    if (src.empty()) {
        cerr << "Cannot open: " << inputPath << '\n';
        return;
    }

    Mat dst;
    auto start = chrono::high_resolution_clock::now();

    resize(src, dst, Size(), scale, scale, INTER_NEAREST);

    auto end = chrono::high_resolution_clock::now();

    if (!imwrite(outputPath.string(), dst)) {
        cerr << "Cannot write: " << outputPath << '\n';
        return;
    }

    double ms = chrono::duration<double, milli>(end - start).count();
    cout << inputPath.filename().string() << " | Scale: " << scale << " | " << src.cols << "x" << src.rows << " -> "
         << dst.cols << "x" << dst.rows << " | " << ms << " ms\n";
}

int main() {
    fs::path folder = fs::path(__FILE__).parent_path() / "hw1_img";
    vector<string> files = {"lena1024.bmp"};
    vector<double> scales = {0.5, 2.0, 4.0};
    for (const auto& name : files) {
        fs::path input = folder / name;
        for (double scale : scales) {
            int newSize = (int)round(1024 * scale);
            fs::path output = folder.parent_path() / "hw1_img" / ("lena" + to_string(newSize) + ".bmp");
            scaleBMP(input, output, scale);
        }
    }
    return 0;
}
