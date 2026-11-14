//
// Created by moamen on 11/4/26.
//

#ifndef MUGJC_IO_HPP
#define MUGJC_IO_HPP
#include <optional>
#include <string>

namespace mugjc::io {
static std::optional<std::string> ReadFile(std::string const& path);

[[nodiscard]] static bool WriteFile(std::string const& path,
                                    std::string const& content);
}  // namespace mugjc::io

#endif  // MUGJC_IO_HPP
