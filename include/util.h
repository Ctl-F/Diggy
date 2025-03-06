//
// Created by ctlf on 3/5/25.
//

#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <optional>

#include "common.h"

namespace util {
    size_t get_file_size(const char* filename) noexcept;
    std::optional<std::string> read_file(const char* filename) noexcept;

}


#endif //UTIL_H
