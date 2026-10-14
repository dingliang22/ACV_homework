#include <chrono>
#include <filesystem>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
using namespace std;
using namespace cv;
namespace fs = std::filesystem;

void rotateBMP(const fs::path& inputPath, const fs::path& outputPath) {
    Mat src = imread(inputPath.string(), IMREAD_UNCHANGED);
    if (src.empty()) {
        cerr << "Cannot open: " << inputPath << '\n';
        return;
    }

    int W = src.cols, H = src.rows;
    Mat dst;

    auto start = chrono::high_resolution_clock::now();

    rotate(src, dst, ROTATE_90_COUNTERCLOCKWISE);

    auto end = chrono::high_resolution_clock::now();

    if (!imwrite(outputPath.string(), dst)) {
        cerr << "Cannot write: " << outputPath << '\n';
        return;
    }

    double ms = chrono::duration<double, milli>(end - start).count();
    cout << inputPath.filename().string() << " | " << W << "x" << H << " -> " << dst.cols << "x" << dst.rows << " | "
         << ms << " ms\n";
}

int main() {
    fs::path folder = fs::path(__FILE__).parent_path() / "hw1_img";
    fs::create_directories(folder.parent_path() / "hw1_img_result");

    vector<string> files = {"lena512.bmp", "lena1024.bmp", "lena2048.bmp", "lena4096.bmp"};

    for (const auto& name : files) {
        fs::path input = folder / name;
        fs::path output = folder.parent_path() / "hw1_img_result" / (fs::path(name).stem().string() + "_270.bmp");
        rotateBMP(input, output);
    }
    return 0;
}
