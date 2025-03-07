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
#include <random>

#include "renderer.h"

typedef uint16_t block_t;

enum class Material {
    Void,
    Dirt,
    Stone,
    Grass,
    Wood,
    Iron,
    Copper,
    INVALID
};

class VoxelEntity {
public:
    VoxelEntity(int seed = 0);
    ~VoxelEntity();

    void render(float player_x, float player_y, float player_z);

public:
    static constexpr size_t VOXEL_SIZE = 2;
    static constexpr size_t CHUNK_SIZE_X = 32;
    static constexpr size_t CHUNK_SIZE_Y = 32;
    static constexpr size_t CHUNK_SIZE_Z = 32;
    static constexpr size_t WORLD_CHUNKS_COUNT_X = 64;
    static constexpr size_t WORLD_CHUNKS_COUNT_Y = 64;
    static constexpr size_t WORLD_CHUNKS_COUNT_Z = 64;
    static constexpr size_t WORLD_QUADRANT_COUNT = (WORLD_CHUNKS_COUNT_X * WORLD_CHUNKS_COUNT_Y * WORLD_CHUNKS_COUNT_Z) / 2;
private:
    struct Chunk {
        Material voxels[CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z];
        size_t visual_mesh_id;
    };

    Chunk& get_chunk(float x, float y, float z);

    void generate_chunk(float x, float y, float z);
    void generate_chunk_mesh(float x, float y, float z);
private:
    Chunk Quadrant1[WORLD_QUADRANT_COUNT]; // +x +y +z
    Chunk Quadrant2[WORLD_QUADRANT_COUNT]; // -x +y +z
    Chunk Quadrant3[WORLD_QUADRANT_COUNT]; // -x +y -z
    Chunk Quadrant4[WORLD_QUADRANT_COUNT]; // +x +y -z
    Chunk Quadrant5[WORLD_QUADRANT_COUNT]; // +x -y +z
    Chunk Quadrant6[WORLD_QUADRANT_COUNT]; // -x -y +z
    Chunk Quadrant7[WORLD_QUADRANT_COUNT]; // -x -y -z
    Chunk Quadrant8[WORLD_QUADRANT_COUNT]; // +x -y -z
};


#endif //TERRAIN_H
