//
// Created by 15813 on 2026/8/21.
//

#include "IO.h"
#include <filesystem>
#include <fstream>

namespace CLOX_V1::IO{
    std::string IOSystem::readAllBytes(const char *filename) {
        if (!std::filesystem::exists(filename)) {
            throw std::runtime_error(std::string("File not found: ") + filename);
        }
        if (!std::filesystem::is_regular_file(filename)) {
            throw std::runtime_error(std::string("Path is not a regular file: ") + filename);
        }

        auto fileSize = std::filesystem::file_size(filename);

        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error(std::string("Cannot open file: ") + filename);
        }

        std::string buffer;
        buffer.resize(static_cast<size_t>(fileSize));

        file.read(buffer.data(), buffer.size());

        if (file.gcount() != static_cast<std::streamsize>(buffer.size())) {
            throw std::runtime_error("Failed to read all bytes from file");
        }
        return buffer;
    }
}