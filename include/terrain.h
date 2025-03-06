//
// Created by ctlf on 3/6/25.
//

#ifndef TERRAIN_H
#define TERRAIN_H

#include "common.h"

#include <vector>
#include <string>
#include <any>
#include <unordered_map>

typedef uint16_t block_t;

struct TerrainInfo {
    struct {
        int width;
        int height;
        int padding_x;
        int padding_y;
        int format;
    } Format;
    std::vector<std::pair<block_t, std::string>> Blocks;
    std::vector<std::unordered_map<std::string, std::any>> MetaData;
};

bool load_terrain_info_from_json(const char* filename) noexcept;

#endif //TERRAIN_H
