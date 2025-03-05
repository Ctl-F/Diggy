#include <cstdio>

#include "renderer.h"

int main() {
    Renderer renderer{};

    renderer.create_window("Diggy", 1280, 720, false);

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

        glClearColor(0.0f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        renderer.swap_buffers();
    }

    return 0;
}