#include "renderer.h"

Renderer::Renderer() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        throw std::runtime_error("Unable to initialize SDL");
    }
}
Renderer::Renderer(Renderer&& other) {
    m_Context = other.m_Context;
    m_Window = other.m_Window;


    other.m_Context = nullptr;
    other.m_Window = nullptr;
}

Renderer::~Renderer(){
    if(m_Window){
        SDL_GL_DeleteContext(m_Context);
        SDL_DestroyWindow(m_Window);

        m_Context = nullptr;
        m_Window = nullptr;

        SDL_Quit();
    }
}

Renderer& Renderer::operator=(Renderer&& other) {
    if(m_Window){
        SDL_GL_DeleteContext(m_Context);
        SDL_DestroyWindow(m_Window);
    }

    m_Window = other.m_Window;
    m_Context = other.m_Context;

    other.m_Window = nullptr;
    other.m_Context = nullptr;

    return *this;
}

RendererError Renderer::create_window(const char* title, int width, int height, bool fullscreen) noexcept {
    if (!!m_Window) {
        return RendererError::DoubleInit;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    uint32_t const flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | (SDL_WINDOW_FULLSCREEN * fullscreen);
    m_Window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);

    if (!m_Window) {
        return RendererError::CouldNotOpenWindow;
    }

    if (RendererError const err = initialize_opengl(); err != RendererError::None) {
        SDL_DestroyWindow(m_Window);
        m_Window = nullptr;
        return err;
    }

    SDL_ShowWindow(m_Window);

    glViewport(0, 0, width, height);

    return RendererError::None;
}

RendererError Renderer::initialize_opengl() noexcept {
    m_Context = SDL_GL_CreateContext(m_Window);

    if (!m_Context) {
        return RendererError::OpenGLError;
    }

    SDL_GL_MakeCurrent(m_Window, m_Context);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        return RendererError::OpenGLError;
    }

    return RendererError::None;
}

void Renderer::swap_buffers() const noexcept {
    SDL_GL_SwapWindow(m_Window);
}

void Renderer::set_clear_color(vec4 color) noexcept {
    m_ClearColor = color;
    glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer::clear() const noexcept {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

