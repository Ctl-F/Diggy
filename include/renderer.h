#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <SDL2/SDL_opengl.h>
#include <SDL_ttf.h>
#include <stdexcept>
#include "common.h"
#include <vector>
#include <unordered_map>
#include <string_view>
#include "stack.h"

enum class RendererError {
    None = 0,
    DoubleInit,
    OpenGLError,
    CouldNotOpenWindow,
    ShaderVertexError,
    ShaderFragmentError,
    ShaderLinkerError,
};

// vec3: Position
// vec3: Normal
// vec3: ColorBlend
// vec2: UV
constexpr size_t FLOATS_PER_VERTEX = 11;

constexpr size_t OFFSET_OF_POSITION = 0;
constexpr size_t OFFSET_OF_NORMAL = 3 * sizeof(float);
constexpr size_t OFFSET_OF_COLOR = 6 * sizeof(float);
constexpr size_t OFFSET_OF_UV = 9 * sizeof(float);

struct MeshBuffer {
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
};

constexpr size_t VERTEX_BUFFER = 0;
constexpr size_t INDEX_BUFFER = 1;

struct Mesh {
    uint32_t array_buffer_id;
    uint32_t buffers[2];
    uint32_t index_count;
};

struct Shader {
    uint32_t program_id;
private:
    friend class Renderer;
    std::unordered_map<std::string, uint32_t> uniform_cache;
};

struct Texture {
    uint32_t id;
    int32_t width;
    int32_t height;
    int32_t channels;
};

class MeshBuilder {
public:
    MeshBuilder(MeshBuffer& target);
    ~MeshBuilder();

    void clear() noexcept;

    void add_vertex(vec3 position, vec3 normal, vec3 color, vec2 uv) noexcept;
    void add_index(uint32_t index) noexcept;

    void add_triangle(vec3 position0, vec3 position1, vec3 position2,
                    vec3 color0, vec3 color1, vec3 color2,
                     vec2 uv0, vec2 uv1, vec2 uv2, vec3 normal) noexcept;

    void add_quad(vec3 position0, vec3 position1, vec3 position2, vec3 position3,
        vec3 color0, vec3 color1, vec3 color2, vec3 color3,
        vec2 uv0, vec2 uv1, vec2 uv2, vec2 uv3,
        vec3 normal) noexcept;

private:
    MeshBuffer& m_Buffer;
    uint32_t m_VertexCount = 0;
};

struct RendererStringResult {
    bool ok;
    size_t mesh_id;
    size_t texture_id;
};

class Renderer {
public:
    Renderer();
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) noexcept;
    ~Renderer();

    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&) noexcept;

    size_t upload_texture(const char* file, bool v_flip) noexcept;

    RendererError create_window(const char* title, int width, int height, bool fullscreen) noexcept;
    void swap_buffers() const noexcept;

    void set_clear_color(vec4 color) noexcept;
    void clear() const noexcept;

    RendererError create_text_shader(const char* vertex_source, const char* fragment_source) noexcept;

    size_t upload_mesh(const MeshBuffer& buffer) noexcept;
    RendererError upload_shader(const char* vertex_source, const char* fragment_source, size_t& shader_id) noexcept;
    size_t upload_font(const char* filename, int size) noexcept;

    void delete_mesh(size_t index) noexcept;
    void delete_shader(size_t index) noexcept;
    void delete_texture(size_t index) noexcept;

    void use_shader(size_t shader_id) noexcept;
    void render_mesh(size_t mesh_id) noexcept;

    void render_text(size_t font_id, const char* text, int x, int y, vec3 color) noexcept;

    void batch_render_text_begin(size_t font_id) noexcept;
    void batch_render_text(const char* text, int x, int y, vec3 color) noexcept;
    void batch_render_text_end() noexcept;

    void set_uniform(const std::string_view name, vec2 value);
    void set_uniform(const std::string_view name, vec3 value);
    void set_uniform(const std::string_view name, vec4 value);
    void set_uniform(const std::string_view name, const mat4& value);
    void set_uniform(const std::string_view name, int value);
    void set_sampler(const std::string_view name, int slot, size_t texture_id);

    bool mesh_is_dead(size_t index) const;
    bool shader_is_dead(size_t index) const;
    bool texture_is_dead(size_t index) const;
private:
    RendererError initialize_opengl() noexcept;

    uint32_t get_loc(const std::string_view name) noexcept;

    size_t create_virtual_font(size_t font_id, int atlasWidth, int atlasHeight);
    int estimate_atlas_size(size_t font_id, int padding = 2) const noexcept;

    RendererError compile_shader(const char* vertex_source, const char* fragment_source, Shader& out) noexcept;

private:
    struct Glyph {
        vec2 uv0, uv1;
        int size_x, size_y;
        int advance;
        int bearing_x, bearing_y;
    };

    struct Font {
        size_t texture_id;
        std::unordered_map<char, Glyph> glyphs;
        int atlasWidth, atlasHeight;
        size_t true_font_id;
        Mesh text_mesh;
    };

private:
    SDL_GLContext m_Context = nullptr;
    SDL_Window* m_Window = nullptr;
    vec4 m_ClearColor = vec4{0.0f};
    Shader* m_CurrentShader = nullptr;

    std::vector<float> m_BatchTextVertices{600};
    size_t m_BatchFontID = -1;

    mat4 m_GuiProjection{1.0f};

    Shader m_FontShader;

    std::vector<Mesh> m_Meshes;
    std::vector<Shader> m_Shaders;
    std::vector<Texture> m_Textures;

    std::vector<TTF_Font*> m_Fonts;
    std::vector<Font> m_VirtualFonts;

    stack<size_t> m_DeadMeshes;
    stack<size_t> m_DeadShaders;
    stack<size_t> m_DeadTextures;
};


#endif