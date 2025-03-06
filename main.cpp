#include <cstdio>
#include "renderer.h"
#include "util.h"
#include "input.h"

size_t load_shader(Renderer& renderer, const char* vertex_file, const char* fragment_file) noexcept;

void update_player(float delta_time) noexcept;

static DiggyContext Context{ true };

int main() {
    Renderer renderer{};

    if (RendererError err = renderer.create_window("Diggy", 1280, 720, false); err != RendererError::None) {
        return 1;
    }

    renderer.set_clear_color({0.0f, 0.05f, 0.075f, 1.0f});

    Context.projection = glm::perspective(glm::radians(100.0f), 1280.0f / 720.0f, 0.01f, 1000.0f);
    Context.view = glm::lookAt(vec3{0.0f, 10.0f, 0.0f}, vec3{0.0f, 10.0f, 100.0f}, {0.0f, 1.0f, 0.0f});

    size_t shader_id = load_shader(renderer, "vertex.glsl", "fragment.glsl");
    if (shader_id == -1) {
        return 1;
    }

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

    size_t mesh_id = renderer.upload_mesh(quad);

    capture_mouse();

    uint32_t oldTime = SDL_GetTicks();

    while (Context.running) {
        uint32_t newTime = SDL_GetTicks();
        uint32_t delta = newTime - oldTime;
        oldTime = newTime;

        float delta_time = 1.0f / glm::max(static_cast<float>(delta), 0.1f);

        handle_events(Context);

        Context.camera_direction.x -= get_cam_axis_horizontal() * delta_time * 1.0f;
        Context.camera_direction.y += get_cam_axis_vertical() * delta_time * 1.0f;
        Context.camera_direction.y = glm::clamp(Context.camera_direction.y, glm::radians(-85.0f), glm::radians(85.0f));

        vec3 forward{
            glm::cos(Context.camera_direction.y) * glm::sin(Context.camera_direction.x),
            -glm::sin(Context.camera_direction.y),
            glm::cos(Context.camera_direction.y) * glm::cos(Context.camera_direction.x)
        };

        Context.view = glm::lookAt({0.0f, 10.0f, 0.0f},
            100.0f * forward,
            {0.0f, 1.0f, 0.0f});

        if (button_is_just_pressed(ActionButton::Menu)) {
            Context.running = false;
        }

        renderer.clear();

        renderer.use_shader(shader_id);
        renderer.set_uniform("u_Projection", Context.projection);
        renderer.set_uniform("u_View", Context.view);
        renderer.set_uniform("u_Model", Context.model);

        renderer.render_mesh(mesh_id);

        renderer.swap_buffers();
    }

    release_mouse();

    return 0;
}

void update_player(float delta_time) noexcept {

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

