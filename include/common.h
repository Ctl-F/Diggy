//
// Created by ctlf on 3/4/25.
//

#ifndef COMMON_H
#define COMMON_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <cstdint>
#include <cstdio>

typedef glm::vec4 vec4;
typedef glm::vec3 vec3;
typedef glm::vec2 vec2;
typedef glm::mat4 mat4;
typedef glm::quat quat;

struct Camera {
    vec3 position{0.0f};
    quat orientation{1.0f, 0.0f, 0.0f, 0.0f};
    float pitch{0.0f};
    float yaw{0.0f};
    vec3 height{0.0f, 10.0f, 0.0f};
};

struct DiggyContext {
    bool running;

    Camera player{};

    float player_speed = 50.0f;

    mat4 projection{1.0f};
    mat4 view{1.0f};
    mat4 model{1.0f};
};

#ifndef EXCLUDE_LOGGING
#define LOG(a, ...) fprintf(stderr, "%s:%d - %s\n", __FILE__, __LINE__, a)
#else
#define LOG(a, ...)
#endif

#endif //COMMON_H
