//
// Created by ctlf on 3/5/25.
//

#include "util.h"

#include <fstream>
#include <filesystem>
#include <sstream>

namespace util {

    size_t get_file_size(const char *filename) noexcept {
        std::ifstream stream(filename, std::ios::binary | std::ios::ate);

        if (!stream) {
            return 0;
        }
        return static_cast<size_t>(stream.tellg());
    }


    std::optional<std::string> read_file(const char *filename) noexcept {
        std::ifstream stream(filename, std::ios::binary | std::ios::ate);

        if (!stream) {
            return {};
        }

        std::streamsize const size = stream.tellg();
        stream.seekg(0, std::ios::beg);

        std::string content(size, '\0');
        stream.read(content.data(), size);

        return content;
    }


}
