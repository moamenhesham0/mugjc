//
// Created by moamen on 11/4/26.
//

#include "io.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>

namespace mug::io {
static std::optional<std::string> ReadFile(std::string const& path) {
    // Validate location
    if (!std::filesystem::exists(path)) {
        return std::nullopt;
    }

    // Open file
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return std::nullopt;
    }

    // Allocate string size for file's content
    const size_t file_size = file.tellg();
    std::string content;
    content.resize(file_size);

    // Read whole file
    file.seekg(0, std::ios::beg);
    if (!file.read(content.data(), file_size)) {
        return std::nullopt;
    }

    return content;
}
[[nodiscard]] static bool WriteFile(std::string const& path) {
    // TODO
    return true;
}
}  // namespace mug::io
