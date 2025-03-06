//
// Created by ctlf on 3/4/25.
//

#ifndef COMMON_H
#define COMMON_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdint>

typedef glm::vec4 vec4;
typedef glm::vec3 vec3;
typedef glm::vec2 vec2;
typedef glm::mat4 mat4;

struct DiggyContext {
    bool running;

    vec2 camera_direction;

    mat4 projection{1.0f};
    mat4 view{1.0f};
    mat4 model{1.0f};
};

#endif //COMMON_H
