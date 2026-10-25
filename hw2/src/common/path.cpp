#include "hw2/common.hpp"
namespace fs = std::filesystem;

fs::path findFolder(fs::path current_path, std::string target) {
    current_path = fs::absolute(current_path).lexically_normal();
    std::string parent_name = target;
    std::string image_name = target;

    // 支援 hw2 或 hw2_img
    if (target.size() >= 4 && target.substr(target.size() - 4) == "_img") {
        parent_name = target.substr(0, target.size() - 4);
    } else {
        image_name = target + "_img";
    }

    while (true) {
        // 1. current_path/hw2_img
        fs::path direct = current_path / image_name;

        if (fs::is_directory(direct)) {
            return direct;
        }

        // 2. current_path/hw2/hw2_img
        fs::path nested = current_path / parent_name / image_name;

        if (fs::is_directory(nested)) {
            return nested;
        }

        // 3. 已到根目錄
        if (current_path == current_path.root_path()) {
            break;
        }

        // 4. 往上一層
        current_path = current_path.parent_path();
    }

    return {};
}
