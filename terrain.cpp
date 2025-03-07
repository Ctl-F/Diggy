//
// Created by ctlf on 3/6/25.
//
#include "terrain.h"

struct MaterialData {
    const char* name;
    int hardness;
    vec3 base_color;
};

#define RGB(r, g, b) vec3(r / 255.0f, g / 255.0f, b / 255.0f)

static MaterialData s_MaterialTable[static_cast<size_t>(Material::INVALID)] {
    { "air", 0, RGB(0, 0, 0)},
    { "dirt", 3, RGB(125, 90, 69)},
    { "stone", 10, RGB(73, 82, 81)},
    { "grass", 3, RGB(59, 140, 74)},
    { "wood", 7, RGB(163, 83, 59)},
    { "iron", 15, RGB(163, 123, 111)},
    { "copper", 12, RGB(237, 142, 52)},
};

VoxelEntity::VoxelEntity(int seed) {
    if (seed != 0) {
        std::default_random_engine::seed(seed);
    }
}
VoxelEntity::~VoxelEntity() {

}

VoxelEntity::Chunk& VoxelEntity::get_chunk(float x, float y, float z) {
    Chunk *quadrant = nullptr;
    // TODO
    // Maybe start simpler

    if (x >= 0 && y >= 0 && z >= 0) {
        quadrant = Quadrant1;
    }
    else if (x < 0 && y >= 0 && z >= 0) {
        quadrant = Quadrant2;
    }
    else if (x < 0 && y >= 0 && z < 0) {
        quadrant = Quadrant3;
    }
    else if (x >= 0 && y >= 0 && z < 0) {
        quadrant = Quadrant4;
    }
    else if (x >= 0 && y < 0 && z >= 0) {
        quadrant = Quadrant5;
    }
    else if (x < 0 && y < 0 && z >= 0) {
        quadrant = Quadrant6;
    }
    else if (x < 0 && y < 0 && z < 0) {
        quadrant = Quadrant7;
    }
    else {
        quadrant = Quadrant8;
    }

}

void VoxelEntity::generate_chunk(float x, float y, float z) {

}
