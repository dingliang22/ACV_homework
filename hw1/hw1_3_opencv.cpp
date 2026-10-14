
#include <chrono>
#include <filesystem>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
using namespace std;
using namespace cv;
namespace fs = std::filesystem;

void interchangeBMP(const fs::path& inputPath, const fs::path& outputPath) {
    Mat src = imread(inputPath.string(), IMREAD_COLOR);
    if (src.empty()) {
        cerr << "Cannot open: " << inputPath << '\n';
        return;
    }

    Mat dst(src.size(), src.type());

    auto start = chrono::high_resolution_clock::now();

    int fromTo[] = {1, 0, 2, 1, 0, 2};
    mixChannels(&src, 1, &dst, 1, fromTo, 3);

    auto end = chrono::high_resolution_clock::now();

    if (!imwrite(outputPath.string(), dst)) {
        cerr << "Cannot write: " << outputPath << '\n';
        return;
    }

    double ms = chrono::duration<double, milli>(end - start).count();
    cout << inputPath.filename().string() << " | " << src.cols << "x" << src.rows << " | Channel Interchange: " << ms
         << " ms\n";
}

int main() {
    fs::path folder = fs::path(__FILE__).parent_path() / "hw1_img_result";

    vector<string> files = {"lena512_270.bmp", "lena1024_270.bmp", "lena2048_270.bmp", "lena4096_270.bmp"};

    for (const auto& name : files) {
        fs::path input = folder / name;
        fs::path output = folder.parent_path() / "hw1_img_result" / (fs::path(name).stem().string() + "_channel.bmp");
        interchangeBMP(input, output);
    }
    return 0;
}
