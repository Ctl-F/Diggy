//
// Created by ctlf on 3/5/25.
//
#include "input.h"
#include <SDL2/SDL.h>

constexpr int KEY_W = 0;
constexpr int KEY_A = 1;
constexpr int KEY_S = 2;
constexpr int KEY_D = 3;

static struct InputContext {
    bool button_last_state[static_cast<size_t>(ActionButton::BUTTON_END)]{false};
    bool button_state[static_cast<size_t>(ActionButton::BUTTON_END)]{false};
    vec2 camera_delta{0.0f};
    float secondary_axis = 0.0f;

    bool wasd[4]{false};
} s_InputContext;

void initialize_input() noexcept {

}

void capture_mouse() noexcept {
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void release_mouse() noexcept {
    SDL_SetRelativeMouseMode(SDL_FALSE);
}


static void handle_key_up(SDL_Event& event) noexcept {
    switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_W:
            s_InputContext.wasd[KEY_W] = false;
            break;
        case SDL_SCANCODE_A:
            s_InputContext.wasd[KEY_A] = false;
            break;
        case SDL_SCANCODE_S:
            s_InputContext.wasd[KEY_S] = false;
            break;
        case SDL_SCANCODE_D:
            s_InputContext.wasd[KEY_D] = false;
            break;
        case SDL_SCANCODE_SPACE:
            s_InputContext.button_state[static_cast<size_t>(ActionButton::Jump)] = false;
            break;
        case SDL_SCANCODE_ESCAPE:
            s_InputContext.button_state[static_cast<size_t>(ActionButton::Menu)] = false;
            break;
        default: break;
    }
}
static void handle_key_down(SDL_Event& event) noexcept {
    switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_W:
            s_InputContext.wasd[KEY_W] = true;
        break;
        case SDL_SCANCODE_A:
            s_InputContext.wasd[KEY_A] = true;
        break;
        case SDL_SCANCODE_S:
            s_InputContext.wasd[KEY_S] = true;
        break;
        case SDL_SCANCODE_D:
            s_InputContext.wasd[KEY_D] = true;
        break;
        case SDL_SCANCODE_SPACE:
            s_InputContext.button_state[static_cast<size_t>(ActionButton::Jump)] = true;
        break;
        case SDL_SCANCODE_ESCAPE:
            s_InputContext.button_state[static_cast<size_t>(ActionButton::Menu)] = true;
        break;
        default: break;
    }
}
static void handle_mouse_button_down(SDL_Event& event) noexcept {
    if (event.button.button == SDL_BUTTON_LEFT) {
        s_InputContext.button_state[static_cast<size_t>(ActionButton::InteractPrimary)] = true;
        return;
    }
    if (event.button.button == SDL_BUTTON_RIGHT) {
        s_InputContext.button_state[static_cast<size_t>(ActionButton::InteractSecondary)] = true;
        return;
    }
}
static void handle_mouse_button_up(SDL_Event& event) noexcept {
    if (event.button.button == SDL_BUTTON_LEFT) {
        s_InputContext.button_state[static_cast<size_t>(ActionButton::InteractPrimary)] = false;
        return;
    }
    if (event.button.button == SDL_BUTTON_RIGHT) {
        s_InputContext.button_state[static_cast<size_t>(ActionButton::InteractSecondary)] = false;
        return;
    }
}
static void handle_mouse_motion(SDL_Event& event) noexcept {
    s_InputContext.camera_delta.x = event.motion.xrel;
    s_InputContext.camera_delta.y = event.motion.yrel;
}
static void handle_mouse_wheel(SDL_Event& event) noexcept {
    s_InputContext.secondary_axis = static_cast<float>(event.wheel.y);
}

void handle_events(DiggyContext &context) {
    s_InputContext.camera_delta = vec2{0.0f};
    s_InputContext.secondary_axis = 0.0f;

    std::copy(s_InputContext.button_state,
        s_InputContext.button_state+static_cast<size_t>(ActionButton::BUTTON_END),
        s_InputContext.button_last_state);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                context.running = false;
                break;
            case SDL_KEYDOWN:
                handle_key_down(event);
                break;
            case SDL_KEYUP:
                handle_key_up(event);
                break;
            case SDL_MOUSEMOTION:
                handle_mouse_motion(event);
                break;
            case SDL_MOUSEBUTTONUP:
                handle_mouse_button_up(event);
                break;
            case SDL_MOUSEBUTTONDOWN:
                handle_mouse_button_down(event);
                break;
            case SDL_MOUSEWHEEL:
                handle_mouse_wheel(event);
                break;
            default: break;
        }
    }
}

float get_move_axis_vertical() noexcept {
    return s_InputContext.wasd[KEY_S] - s_InputContext.wasd[KEY_W];
}
float get_move_axis_horizontal() noexcept {
    return s_InputContext.wasd[KEY_D] - s_InputContext.wasd[KEY_A];
}
vec2 get_move_axis_normalized() noexcept {
    vec2 axis{get_move_axis_horizontal(), get_move_axis_vertical()};
    if (glm::dot(axis, axis) <= std::numeric_limits<float>::epsilon()) {
        return vec2{0.0f};
    }
    return glm::normalize(axis);
}

float get_cam_axis_vertical() noexcept {
    return s_InputContext.camera_delta.y;
}
float get_cam_axis_horizontal() noexcept {
    return s_InputContext.camera_delta.x;
}
vec2 get_cam_axis_normalized() noexcept {
    return glm::normalize(s_InputContext.camera_delta);
}

float get_secondary_axis() noexcept {
    return s_InputContext.secondary_axis;
}

bool button_is_pressed(ActionButton button) noexcept {
    return s_InputContext.button_state[static_cast<int>(button)];
}
bool button_is_released(ActionButton button) noexcept {
    return !s_InputContext.button_state[static_cast<int>(button)];
}
bool button_is_just_pressed(ActionButton button) noexcept {
    return s_InputContext.button_state[static_cast<int>(button)] &&
        !s_InputContext.button_last_state[static_cast<int>(button)];
}
bool button_is_just_released(ActionButton button) noexcept {
    return !s_InputContext.button_state[static_cast<int>(button)] &&
        s_InputContext.button_last_state[static_cast<int>(button)];
}
