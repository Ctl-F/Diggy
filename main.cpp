#include <cstdio>
#include "renderer.h"
#include "util.h"
#include "input.h"


size_t load_shader(Renderer& renderer, const char* vertex_file, const char* fragment_file) noexcept;

void game_step(float delta_time) noexcept;
void game_render(Renderer& renderer, float delta_time) noexcept;

void update_player(float delta_time) noexcept;

static DiggyContext Context{ true };

size_t shader_id;
size_t mesh_id;
size_t texture_id;

int main() {
    Renderer renderer{};

    if (RendererError err = renderer.create_window("Diggy", 1280, 720, false); err != RendererError::None) {
        return 1;
    }

    renderer.set_clear_color({0.0f, 0.05f, 0.075f, 1.0f});

    Context.projection = glm::perspective(glm::radians(100.0f), 1280.0f / 720.0f, 0.01f, 1000.0f);
    Context.view = glm::lookAt(vec3{0.0f, 10.0f, 0.0f}, vec3{0.0f, 10.0f, 100.0f}, {0.0f, 1.0f, 0.0f});

    shader_id = load_shader(renderer, "vertex.glsl", "fragment.glsl");
    if (shader_id == -1) {
        return 1;
    }

    texture_id = renderer.upload_texture("texture_pack.png", true);

    MeshBuffer quad;
    {
        MeshBuilder builder{quad};

        builder.add_quad({-100.0, 0.0, -100.0},
                         { 100.0, 0.0, -100.0},
                         {-100.0,  0.0, 100.0},
                         { 100.0,  0.0, 100.0},
                         {1.0, 0.0, 0.0},
                         {0.0, 1.0, 0.0},
                         {0.0, 0.0, 1.0},
                         {1.0, 1.0, 1.0},
                         {0.0, 0.0}, {1.0, 0.0},
                         {0.0, 1.0}, {1.0, 1.0},
                         {0.0, 1.0, 0.0});
    }

    mesh_id = renderer.upload_mesh(quad);

    capture_mouse();

    uint32_t oldTime = SDL_GetTicks();

    while (Context.running) {
        uint32_t newTime = SDL_GetTicks();
        uint32_t delta = newTime - oldTime;
        oldTime = newTime;

        float delta_time = 1.0f / glm::max(static_cast<float>(delta), 0.1f);

        handle_events(Context);
        game_step(delta_time);

        renderer.clear();
        game_render(renderer, delta_time);
        renderer.swap_buffers();
    }

    release_mouse();

    return 0;
}

void update_player(float delta_time) noexcept {
    float sensitivity = 1.0f;
    float smoothFactor = 10.0f;

    Context.player.yaw -= get_cam_axis_horizontal() * sensitivity * delta_time;
    Context.player.pitch -= get_cam_axis_vertical() * sensitivity * delta_time;

    const float angle_limit = glm::radians(85.0f);
    Context.player.pitch = glm::clamp(Context.player.pitch, -angle_limit, angle_limit);

    glm::quat targetPitch = glm::angleAxis(Context.player.pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::quat targetYaw = glm::angleAxis(Context.player.yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat targetOrientation = glm::normalize(targetYaw * targetPitch);

    Context.player.orientation = glm::slerp(Context.player.orientation, targetOrientation, 1.0f - glm::exp(-smoothFactor * delta_time));
    glm::vec3 forward = Context.player.orientation * glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 right = Context.player.orientation * glm::vec3(1.0f, 0.0f, 0.0f);

    float temp_y = forward.y;
    forward.y = 0.0f;
    right.y = 0.0f;

    forward = glm::normalize(forward);
    right = glm::normalize(right);

    vec2 move_axis = get_move_axis_normalized();

    glm::vec3 velocity = (-forward * move_axis.y + right * move_axis.x) * Context.player_speed * delta_time;

    Context.player.position += velocity;

    forward.y = temp_y;

    Context.view = glm::lookAt(Context.player.position + Context.player.height,
        Context.player.position + Context.player.height + forward, glm::vec3(0.0f, 1.0f, 0.0f));
}

void game_step(float delta_time) noexcept {
    if (button_is_just_pressed(ActionButton::Menu)) {
        Context.running = false;
    }

    update_player(delta_time);
}

void game_render(Renderer &renderer, float delta_time) noexcept {
    renderer.use_shader(shader_id);

    renderer.set_sampler("texture_atlas", 0, texture_id);

    renderer.set_uniform("u_Projection", Context.projection);
    renderer.set_uniform("u_View", Context.view);
    renderer.set_uniform("u_Model", Context.model);

    renderer.render_mesh(mesh_id);
}



size_t load_shader(Renderer& renderer, const char* vertex_file, const char* fragment_file) noexcept {
    auto vSrc = util::read_file(vertex_file);
    auto fSrc = util::read_file(fragment_file);

    if (!vSrc.has_value() || !fSrc.has_value()) {
        return -1;
    }

    size_t shader_id;
    if (RendererError err = renderer.upload_shader(vSrc.value().c_str(),
        fSrc.value().c_str(), shader_id);
            err != RendererError::None) {
        return -1;
    }

    return shader_id;
}

