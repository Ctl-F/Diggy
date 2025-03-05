#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <SDL2/SDL_opengl.h>
#include <stdexcept>

enum class RendererError {
    None = 0,
    DoubleInit,
    OpenGLError,
    CouldNotOpenWindow,
};

class Renderer {
public:
    Renderer();
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&);
    ~Renderer();

    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&);

    RendererError create_window(const char* title, int width, int height, bool fullscreen) noexcept;
    void swap_buffers() const noexcept;

private:
    RendererError initialize_opengl() noexcept;

private:
    SDL_GLContext m_Context = nullptr;
    SDL_Window* m_Window = nullptr;
};


#endif