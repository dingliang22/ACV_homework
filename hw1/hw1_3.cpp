
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

void interchangeBMP(const fs::path& inputPath, const fs::path& outputPath) {
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
    int stride = (W * 3 + 3) & ~3;
    vector<uint8_t> src(stride * H), dst(stride * H, 0);

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
            int i = y * stride + x * 3;
            dst[i] = src[i + 1];     // G -> B
            dst[i + 1] = src[i + 2]; // R -> G
            dst[i + 2] = src[i];     // B -> R
        }
    }

    auto end = chrono::high_resolution_clock::now();

    ofstream out(outputPath, ios::binary);
    if (!out) {
        cerr << "Cannot write: " << outputPath << '\n';
        return;
    }
    out.write((char*)&h, sizeof(h));
    if (h.offset > sizeof(h)) {
        out.write(string(h.offset - sizeof(h), '\0').data(), h.offset - sizeof(h));
    }
    out.write((char*)dst.data(), dst.size());
    out.close();

    double ms = chrono::duration<double, milli>(end - start).count();
    cout << inputPath.filename().string() << " | " << W << "x" << H << " | Channel Interchange: " << ms << " ms\n";
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
