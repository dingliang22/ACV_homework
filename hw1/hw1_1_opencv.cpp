#include <chrono>
#include <filesystem>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
using namespace std;
using namespace cv;
namespace fs = std::filesystem;

void readWriteBMP(const fs::path& inputPath, const fs::path& outputPath) {
    auto start = chrono::high_resolution_clock::now();

    Mat img = imread(inputPath.string(), IMREAD_UNCHANGED);
    if (img.empty()) {
        cerr << "Cannot open: " << inputPath << '\n';
        return;
    }

    int width = img.cols;
    int height = img.rows;
    int bpp = img.channels() * img.elemSize1() * 8;
    size_t rowSize = (size_t)width * bpp / 8;
    size_t padding = (4 - rowSize % 4) % 4;
    size_t actualRow = rowSize + padding;
    size_t bufferSize = actualRow * height;

    if (!imwrite(outputPath.string(), img)) {
        cerr << "Cannot write: " << outputPath << '\n';
        return;
    }

    auto end = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(end - start).count();

    cout << "\nFile: " << inputPath.filename().string() << '\n';
    cout << "Image width              : " << width << '\n';
    cout << "Image height             : " << height << '\n';
    cout << "Bits per pixel           : " << bpp << '\n';
    cout << "Row size before padding  : " << rowSize << " bytes\n";
    cout << "Padding bytes per row    : " << padding << " bytes\n";
    cout << "Actual row size          : " << actualRow << " bytes\n";
    cout << "Total image buffer size  : " << bufferSize << " bytes\n";
    cout << "Read/Write time          : " << ms << " ms\n";
}

int main() {
    fs::path folder = fs::path(__FILE__).parent_path() / "hw1_img";
    fs::create_directories(folder.parent_path() / "hw1_img_result");

    vector<string> files = {"lena512.bmp", "lena1024.bmp", "lena2048.bmp", "lena4096.bmp"};

    for (const auto& name : files) {
        fs::path input = folder / name;
        fs::path output = folder.parent_path() / "hw1_img_result" / (fs::path(name).stem().string() + "_copy.bmp");
        readWriteBMP(input, output);
    }
    return 0;
}
