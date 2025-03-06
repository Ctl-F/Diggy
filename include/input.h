//
// Created by ctlf on 3/5/25.
//

#ifndef INPUT_H
#define INPUT_H

#include "common.h"


enum class ActionButton {
    Menu,
    InteractPrimary,
    InteractSecondary,
    Jump,
    BUTTON_END
};

void initialize_input() noexcept;

void capture_mouse() noexcept;
void release_mouse() noexcept;

void handle_events(DiggyContext& context);

float get_move_axis_vertical() noexcept;
float get_move_axis_horizontal() noexcept;
vec2 get_move_axis_normalized() noexcept;

float get_cam_axis_vertical() noexcept;
float get_cam_axis_horizontal() noexcept;
vec2 get_cam_axis_normalized() noexcept;

float get_secondary_axis() noexcept;

bool button_is_pressed(ActionButton button) noexcept;
bool button_is_released(ActionButton button) noexcept;
bool button_is_just_pressed(ActionButton button) noexcept;
bool button_is_just_released(ActionButton button) noexcept;

#endif //INPUT_H
