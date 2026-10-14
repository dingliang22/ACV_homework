
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
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

void scaleBMP(const fs::path& inputPath, const fs::path& outputPath, double scale) {
    ifstream in(inputPath, ios::binary);
    if (!in) {
        cerr << "Cannot open: " << inputPath << '\n';
        return;
    }

    BMPHeader h;
    in.read((char*)&h, sizeof(h));
    if (!in || h.type != 0x4D42 || h.bitCount != 24 || h.compression != 0 || h.width <= 0 || h.height <= 0 ||
        h.offset < sizeof(h) || scale <= 0) {
        cerr << "Unsupported BMP: " << inputPath << '\n';
        return;
    }

    int W = h.width, H = h.height;
    int newW = (int)round(W * scale);
    int newH = (int)round(H * scale);
    if (newW <= 0 || newH <= 0) {
        return;
    }

    int inStride = (W * 3 + 3) & ~3;
    int outStride = (newW * 3 + 3) & ~3;

    vector<uint8_t> header(h.offset);
    vector<uint8_t> src((size_t)inStride * H);
    vector<uint8_t> dst((size_t)outStride * newH, 0);

    in.seekg(0);
    in.read((char*)header.data(), header.size());
    in.read((char*)src.data(), src.size());
    if (!in) {
        cerr << "Read failed\n";
        return;
    }
    in.close();

    auto start = chrono::high_resolution_clock::now();

    for (int y = 0; y < newH; y++) {
        for (int x = 0; x < newW; x++) {
            int srcX = (int)(x / scale);
            int srcY = (int)(y / scale);
            if (srcX >= W) {
                srcX = W - 1;
            }
            if (srcY >= H) {
                srcY = H - 1;
            }

            int si = srcY * inStride + srcX * 3;
            int di = y * outStride + x * 3;
            for (int c = 0; c < 3; c++) {
                dst[di + c] = src[si + c];
            }
        }
    }

    auto end = chrono::high_resolution_clock::now();

    h.width = newW;
    h.height = newH;
    h.imageSize = (uint32_t)dst.size();
    h.fileSize = h.offset + h.imageSize;
    memcpy(header.data(), &h, sizeof(h));

    ofstream out(outputPath, ios::binary);
    if (!out) {
        cerr << "Cannot write: " << outputPath << '\n';
        return;
    }
    out.write((char*)header.data(), header.size());
    out.write((char*)dst.data(), dst.size());
    out.close();

    double ms = chrono::duration<double, milli>(end - start).count();
    cout << inputPath.filename().string() << " | Scale: " << scale << " | " << W << "x" << H << " -> " << newW << "x"
         << newH << " | " << ms << " ms\n";
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
