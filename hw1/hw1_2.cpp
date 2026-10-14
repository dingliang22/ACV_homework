#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
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

void rotateBMP(const fs::path& inputPath, const fs::path& outputPath) {
    ifstream in(inputPath, ios::binary);
    if (!in) {
        cerr << "Cannot open: " << inputPath << '\n';
        return;
    }

    BMPHeader h;
    in.read((char*)&h, sizeof(h));
    if (!in || h.type != 0x4D42 || h.bitCount != 24 || h.compression != 0 || h.width <= 0 || h.height <= 0) {
        cerr << "Unsupported BMP: " << inputPath << '\n';
        return;
    }

    int W = h.width, H = h.height;
    int inStride = (W * 3 + 3) & ~3;
    int outStride = (H * 3 + 3) & ~3;

    vector<uint8_t> src(inStride * H), dst(outStride * W, 0);
    in.seekg(h.offset);
    in.read((char*)src.data(), src.size());
    if (!in) {
        cerr << "Read failed\n";
        return;
    }
    in.close();

    auto start = chrono::high_resolution_clock::now();

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            int sx = y * inStride + x * 3;
            int dx = x * outStride + (H - 1 - y) * 3;
            for (int c = 0; c < 3; c++) {
                dst[dx + c] = src[sx + c];
            }
        }
    }

    auto end = chrono::high_resolution_clock::now();

    swap(h.width, h.height);
    h.imageSize = dst.size();
    h.fileSize = h.offset + h.imageSize;

    ofstream out(outputPath, ios::binary);
    out.write((char*)&h, sizeof(h));
    if (h.offset > sizeof(h)) {
        out.write(string(h.offset - sizeof(h), '\0').data(), h.offset - sizeof(h));
    }
    out.write((char*)dst.data(), dst.size());
    out.close();

    double ms = chrono::duration<double, milli>(end - start).count();
    cout << inputPath.filename().string() << " | " << W << "x" << H << " -> " << h.width << "x" << h.height << " | "
         << ms << " ms\n";
}

int main() {
    fs::path folder = fs::path(__FILE__).parent_path() / "hw1_img";
    vector<string> files = {"lena512.bmp", "lena1024.bmp", "lena2048.bmp", "lena4096.bmp"};

    for (const auto& name : files) {
        fs::path input = folder / name;
        fs::path output = folder.parent_path() / "hw1_img_result" / (fs::path(name).stem().string() + "_270.bmp");
        rotateBMP(input, output);
    }
    return 0;
}
