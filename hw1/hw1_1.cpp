#include <chrono>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
using namespace std;
namespace fs = std::filesystem;

#pragma pack(push, 1)

struct BMPHeader {
    uint16_t type;
    uint32_t fileSize;
    uint16_t reserved1, reserved2;
    uint32_t offset;
    uint32_t headerSize;
    int32_t width, height;
    uint16_t planes, bitCount;
    uint32_t compression, imageSize;
    int32_t xppm, yppm;
    uint32_t clrUsed, clrImportant;
};

#pragma pack(pop)

void readWriteBMP(const fs::path& inputPath, const fs::path& outputPath) {
    auto start = chrono::high_resolution_clock::now();

    ifstream in(inputPath, ios::binary);
    if (!in) {
        cerr << "Cannot open: " << inputPath << '\n';
        return;
    }

    vector<uint8_t> data((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    in.close();

    if (data.size() < sizeof(BMPHeader)) {
        cerr << "Invalid BMP: " << inputPath << '\n';
        return;
    }

    BMPHeader h;
    memcpy(&h, data.data(), sizeof(h));
    if (h.type != 0x4D42 || h.width <= 0 || h.height == 0 || h.planes != 1 || h.bitCount == 0) {
        cerr << "Unsupported BMP: " << inputPath << '\n';
        return;
    }

    uint64_t rowSize = (uint64_t)h.width * h.bitCount;
    rowSize = (rowSize + 7) / 8;
    uint64_t padding = (4 - rowSize % 4) % 4;
    uint64_t actualRow = rowSize + padding;
    uint64_t bufferSize = actualRow * (h.height < 0 ? -(int64_t)h.height : h.height);

    cout << "\nFile: " << inputPath.filename().string() << '\n';
    cout << "Image width              : " << h.width << '\n';
    cout << "Image height             : " << h.height << '\n';
    cout << "Bits per pixel           : " << h.bitCount << '\n';
    cout << "Row size before padding  : " << rowSize << " bytes\n";
    cout << "Padding bytes per row    : " << padding << " bytes\n";
    cout << "Actual row size          : " << actualRow << " bytes\n";
    cout << "Total image buffer size  : " << bufferSize << " bytes\n";

    ofstream out(outputPath, ios::binary);
    if (!out) {
        cerr << "Cannot write: " << outputPath << '\n';
        return;
    }
    out.write((char*)data.data(), data.size());
    out.close();

    auto end = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(end - start).count();
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
