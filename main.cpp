#include <cstdio>

#include "renderer.h"

int main() {
    Renderer renderer{};

    renderer.create_window("Diggy", 1280, 720, false);

    renderer.set_clear_color({0.0f, 0.05f, 0.075f, 1.0f});

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                default: break;
            }
        }

        renderer.clear();

        renderer.swap_buffers();
    }

    return 0;
}