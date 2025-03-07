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

    template<typename type_t>
    bool contains(const type_t* begin, const type_t* end, const type_t& value) noexcept {
        while (begin < end) {
            if (value == *begin) return true;
            ++begin;
        }
        return false;
    }

}


#endif //UTIL_H
