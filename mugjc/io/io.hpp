//
// Created by moamen on 11/4/26.
//

#ifndef MUGJC_IO_HPP
#define MUGJC_IO_HPP
#include <optional>
#include <string>

namespace mug::io {
// Returns the whole file, or std::nullopt if it cannot be read.
std::optional<std::string> ReadFile(std::string const& path);

// Overwrites the file. Returns false on failure.
[[nodiscard]] bool WriteFile(std::string const& path,
                             std::string const& content);
}  // namespace mug::io

#endif  // MUGJC_IO_HPP
