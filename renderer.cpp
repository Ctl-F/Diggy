#include "renderer.h"

#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "util.h"

Renderer::Renderer() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        throw std::runtime_error("Unable to initialize SDL");
    }
    if (TTF_Init() < 0) {
        throw std::runtime_error("Unable to initialize TTF");
    }

    SDL_GL_SetSwapInterval(0);
}
Renderer::Renderer(Renderer&& other) noexcept {
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

        TTF_Quit();
        SDL_Quit();
    }
}

Renderer& Renderer::operator=(Renderer&& other) noexcept {
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

    m_GuiProjection = glm::ortho(0.0f, (float)width, (float)height, 0.0f);

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

size_t Renderer::upload_mesh(const MeshBuffer& buffer) noexcept {
    Mesh* mesh_ptr;

    if (m_DeadMeshes.empty()) {
        mesh_ptr = &m_Meshes.emplace_back();
    }
    else {
        mesh_ptr = &m_Meshes[m_DeadMeshes.top()];
        m_DeadMeshes.pop();
    }

    Mesh& mesh = *mesh_ptr;

    glGenVertexArrays(1, &mesh.array_buffer_id);
    glBindVertexArray(mesh.array_buffer_id);

    glGenBuffers(2, mesh.buffers);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.buffers[VERTEX_BUFFER]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.buffers[INDEX_BUFFER]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*buffer.vertices.size(), buffer.vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)*buffer.indices.size(), buffer.indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(float), (void*)OFFSET_OF_POSITION);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(float), (void*)OFFSET_OF_NORMAL);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(float), (void*)OFFSET_OF_COLOR);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(float), (void*)OFFSET_OF_UV);
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    mesh.index_count = static_cast<uint32_t>(buffer.indices.size());

    return m_Meshes.size() - 1;
}


MeshBuilder::MeshBuilder(MeshBuffer &target) : m_Buffer{target} {

}

MeshBuilder::~MeshBuilder() {

}

void MeshBuilder::clear() noexcept {
    m_Buffer.vertices.clear();
    m_Buffer.indices.clear();
    m_VertexCount = 0;
}

void MeshBuilder::add_vertex(vec3 position, vec3 normal, vec3 color, vec2 uv) noexcept {
    m_Buffer.vertices.push_back(position.x);
    m_Buffer.vertices.push_back(position.y);
    m_Buffer.vertices.push_back(position.z);
    m_Buffer.vertices.push_back(normal.x);
    m_Buffer.vertices.push_back(normal.y);
    m_Buffer.vertices.push_back(normal.z);
    m_Buffer.vertices.push_back(color.x);
    m_Buffer.vertices.push_back(color.y);
    m_Buffer.vertices.push_back(color.z);
    m_Buffer.vertices.push_back(uv.x);
    m_Buffer.vertices.push_back(uv.y);
    m_VertexCount++;
}

void MeshBuilder::add_index(uint32_t index) noexcept {
    m_Buffer.indices.push_back(index);
}

void MeshBuilder::add_triangle(vec3 position0, vec3 position1, vec3 position2, vec3 color0, vec3 color1, vec3 color2, vec2 uv0, vec2 uv1, vec2 uv2, vec3 normal) noexcept {
    add_vertex(position0, normal, color0, uv0);
    add_vertex(position1, normal, color1, uv1);
    add_vertex(position2, normal, color2, uv2);

    add_index(m_VertexCount-3);
    add_index(m_VertexCount-2);
    add_index(m_VertexCount-1);
}

void MeshBuilder::add_quad(vec3 position0, vec3 position1, vec3 position2, vec3 position3, vec3 color0, vec3 color1, vec3 color2, vec3 color3, vec2 uv0, vec2 uv1, vec2 uv2, vec2 uv3, vec3 normal) noexcept {
    add_vertex(position0, normal, color0, uv0);
    add_vertex(position1, normal, color1, uv1);
    add_vertex(position2, normal, color2, uv2);
    add_vertex(position3, normal, color3, uv3);

    add_index(m_VertexCount-4);
    add_index(m_VertexCount-3);
    add_index(m_VertexCount-2);

    add_index(m_VertexCount-2);
    add_index(m_VertexCount-3);
    add_index(m_VertexCount-1);
}

RendererError Renderer::create_text_shader(const char *vertex_source, const char *fragment_source) noexcept {
    if (RendererError err=compile_shader(vertex_source, fragment_source, m_FontShader); err != RendererError::None) {
        return err;
    }

    return RendererError::None;
}


RendererError Renderer::upload_shader(const char *vertex_source, const char *fragment_source, size_t &shader_id) noexcept {
    Shader shader;
    if (RendererError err=compile_shader(vertex_source, fragment_source, shader); err != RendererError::None) {
        return err;
    }

    if (m_DeadShaders.empty()) {
        shader_id = m_Shaders.size();
        m_Shaders.push_back(shader);
    }
    else {
        shader_id = m_DeadShaders.top();
        m_DeadShaders.pop();
        m_Shaders[shader_id] = shader;
    }
    return RendererError::None;
}


RendererError Renderer::compile_shader(const char* vertex_source, const char* fragment_source, Shader& out) noexcept {
    uint32_t vert, frag;
    int success;
    char infoLog[512]{0};

    vert = glCreateShader(GL_VERTEX_SHADER);
    frag = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vert, 1, &vertex_source, nullptr);
    glCompileShader(vert);

    glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vert, 512, nullptr, infoLog);
        fprintf(stderr, "%s\n", infoLog);

        glDeleteShader(vert);
        glDeleteShader(frag);

        return RendererError::ShaderVertexError;
    }

    glShaderSource(frag, 1, &fragment_source, nullptr);
    glCompileShader(frag);

    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frag, 512, nullptr, infoLog);
        fprintf(stderr, "%s\n", infoLog);

        glDeleteShader(vert);
        glDeleteShader(frag);

        return RendererError::ShaderFragmentError;
    }

    uint32_t prog_id = glCreateProgram();
    glAttachShader(prog_id, vert);
    glAttachShader(prog_id, frag);

    glLinkProgram(prog_id);

    glDetachShader(prog_id, vert);
    glDetachShader(prog_id, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);

    glGetProgramiv(prog_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(prog_id, 512, nullptr, infoLog);
        fprintf(stderr, "%s\n", infoLog);

        glDeleteProgram(prog_id);

        return RendererError::ShaderLinkerError;
    }

    out.program_id = prog_id;
    return RendererError::None;
}

void Renderer::use_shader(size_t shader_id) noexcept {
    if (shader_id == -1) {
        glUseProgram(0);
        m_CurrentShader = nullptr;
        return;
    }
    if (shader_id >= m_Shaders.size()) return;

    glUseProgram(m_Shaders[shader_id].program_id);
    m_CurrentShader = &m_Shaders[shader_id];
}

void Renderer::render_mesh(size_t mesh_id) noexcept {
    if (mesh_id >= m_Meshes.size()) return;

    Mesh& mesh = m_Meshes[mesh_id];

    glBindVertexArray(mesh.array_buffer_id);
    glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, nullptr);
}

uint32_t Renderer::get_loc(const std::string_view name) noexcept {
    if (m_CurrentShader == nullptr) {
        return -1;
    }

    const std::string sName{name.begin(), name.end()};

    if (auto where = m_CurrentShader->uniform_cache.find(sName); where != m_CurrentShader->uniform_cache.end()) {
        return where->second;
    }
    uint32_t loc = glGetUniformLocation(m_CurrentShader->program_id, name.begin());
    m_CurrentShader->uniform_cache[sName] = loc;

    return loc;
}

void Renderer::set_uniform(const std::string_view name, vec2 value) {
    uint32_t loc;
    if (loc=get_loc(name); loc == -1) {
        return;
    }

    glUniform2f(loc, value.x, value.y);
}

void Renderer::set_uniform(const std::string_view name, vec3 value) {
    uint32_t loc;
    if (loc=get_loc(name); loc == -1) {
        return;
    }

    glUniform3f(loc, value.x, value.y, value.z);
}

void Renderer::set_uniform(const std::string_view name, vec4 value) {
    uint32_t loc;
    if (loc=get_loc(name); loc == -1) {
        return;
    }

    glUniform4f(loc, value.x, value.y, value.z, value.w);
}

void Renderer::set_uniform(const std::string_view name, int value) {
    uint32_t loc;
    if (loc=get_loc(name); loc == -1) {
        return;
    }
    glUniform1i(loc, value);
}

void Renderer::set_uniform(const std::string_view name, const mat4 &value) {
    uint32_t loc;
    if (loc=get_loc(name); loc == -1) {
        return;
    }

    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

size_t Renderer::upload_texture(const char *file, bool v_flip) noexcept {
    Texture* texture_ptr;
    size_t texture_id;
    if (m_DeadTextures.empty()) {
        texture_id = m_Textures.size();
        texture_ptr = &m_Textures.emplace_back();
    }
    else {
        texture_id = m_DeadTextures.top();
        texture_ptr = &m_Textures[texture_id];
        m_DeadTextures.pop();
    }


    Texture& texture = *texture_ptr;
    stbi_set_flip_vertically_on_load(v_flip);
    unsigned char* data = stbi_load(file, &texture.width, &texture.height, &texture.channels, 0);

    if (!data) {
        m_Textures.pop_back();
        return -1;
    }

    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    auto fmt = GL_RGB;
    if (texture.channels == 4) {
        fmt = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, fmt, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    return m_Textures.size() - 1;
}

void Renderer::set_sampler(const std::string_view name, int slot, size_t texture_id) {
    if (texture_id == -1) {
        return;
    }

    uint32_t loc;
    if (loc=get_loc(name); loc == -1) {
        return;
    }
    glUniform1i(loc, slot);

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_Textures[texture_id].id);
}

size_t Renderer::upload_font(const char *filename, int size) noexcept {
    size_t font_id = m_Fonts.size();
    TTF_Font*& font = m_Fonts.emplace_back();

    font = TTF_OpenFont(filename, size);
    if (!font) {
        fprintf(stderr, "TTF_Font Error: %s\n", TTF_GetError());
        m_Fonts.pop_back();
        return -1;
    }

    const int atlas_size = estimate_atlas_size(font_id);
    size_t vfont_id = create_virtual_font(font_id, atlas_size, atlas_size);
    return vfont_id;
}

void Renderer::batch_render_text_begin(size_t font_id) noexcept {
    m_BatchTextVertices.clear();
    m_BatchFontID = font_id;
}

void Renderer::batch_render_text(const char* text, int x, int y, vec3 color) noexcept {
    if (m_BatchFontID == -1) {
        fprintf(stderr, "Batch text not started.\n");
        return;
    }

    Font& font = m_VirtualFonts[m_BatchFontID];
    float startX = static_cast<float>(x);
    while (*text) {
        char c = *text; text++;
        if (c == '\n') {
            x = static_cast<int>(startX);
            y += font.glyphs['|'].size_y;
            continue;
        }

        Glyph& glyph = font.glyphs[c];

        float x0 = x + glyph.bearing_x;
        float y0 = y + glyph.bearing_y;
        float x1 = x0 + glyph.size_x;
        float y1 = y0 + glyph.size_y;

        m_BatchTextVertices.push_back(x0); m_BatchTextVertices.push_back(y0); m_BatchTextVertices.push_back(glyph.uv0.x); m_BatchTextVertices.push_back(glyph.uv0.y);
        m_BatchTextVertices.push_back(color.r); m_BatchTextVertices.push_back(color.g); m_BatchTextVertices.push_back(color.b);
        m_BatchTextVertices.push_back(x1); m_BatchTextVertices.push_back(y0); m_BatchTextVertices.push_back(glyph.uv1.x); m_BatchTextVertices.push_back(glyph.uv0.y);
        m_BatchTextVertices.push_back(color.r); m_BatchTextVertices.push_back(color.g); m_BatchTextVertices.push_back(color.b);
        m_BatchTextVertices.push_back(x1); m_BatchTextVertices.push_back(y1); m_BatchTextVertices.push_back(glyph.uv1.x); m_BatchTextVertices.push_back(glyph.uv1.y);
        m_BatchTextVertices.push_back(color.r); m_BatchTextVertices.push_back(color.g); m_BatchTextVertices.push_back(color.b);

        m_BatchTextVertices.push_back(x0); m_BatchTextVertices.push_back(y0); m_BatchTextVertices.push_back(glyph.uv0.x); m_BatchTextVertices.push_back(glyph.uv0.y);
        m_BatchTextVertices.push_back(color.r); m_BatchTextVertices.push_back(color.g); m_BatchTextVertices.push_back(color.b);
        m_BatchTextVertices.push_back(x1); m_BatchTextVertices.push_back(y1); m_BatchTextVertices.push_back(glyph.uv1.x); m_BatchTextVertices.push_back(glyph.uv1.y);
        m_BatchTextVertices.push_back(color.r); m_BatchTextVertices.push_back(color.g); m_BatchTextVertices.push_back(color.b);
        m_BatchTextVertices.push_back(x0); m_BatchTextVertices.push_back(y1); m_BatchTextVertices.push_back(glyph.uv0.x); m_BatchTextVertices.push_back(glyph.uv1.y);
        m_BatchTextVertices.push_back(color.r); m_BatchTextVertices.push_back(color.g); m_BatchTextVertices.push_back(color.b);

        x += glyph.advance;
    }
}

void Renderer::batch_render_text_end() noexcept {
    constexpr size_t FLOATS_PER_VERTEX = 2 + 2 + 3;
    constexpr size_t VERTEX_STRIDE = FLOATS_PER_VERTEX * sizeof(float);
    constexpr size_t OFFSET_OF_POS = 0;
    constexpr size_t OFFSET_OF_UV  = 2 * sizeof(float);
    constexpr size_t OFFSET_OF_COL = 4 * sizeof(float);

    if (m_BatchFontID == -1) {
        fprintf(stderr, "Batch text render not started\n");
        return;
    }
    Font& font = m_VirtualFonts[m_BatchFontID];

    if (font.text_mesh.array_buffer_id == 0) {
        glGenVertexArrays(1, &font.text_mesh.array_buffer_id);
        glGenBuffers(1, font.text_mesh.buffers);
    }
    glBindVertexArray(font.text_mesh.array_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, font.text_mesh.buffers[0]);

    glBufferData(GL_ARRAY_BUFFER, m_BatchTextVertices.size() * sizeof(float), m_BatchTextVertices.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (void*)OFFSET_OF_POS);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (void*)OFFSET_OF_UV);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (void*)OFFSET_OF_COL);

    glUseProgram(m_FontShader.program_id);

    Shader* shader = m_CurrentShader;
    m_CurrentShader = &m_FontShader;

    set_uniform("u_Projection", m_GuiProjection);
    set_uniform("u_FontAtlas", 0);

    m_CurrentShader = shader;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Textures[font.texture_id].id);
    glDrawArrays(GL_TRIANGLES, 0, m_BatchTextVertices.size());

    glBindVertexArray(0);
}

void Renderer::render_text(size_t font_id, const char* text, int x, int y, vec3 color) noexcept {
    constexpr size_t VERTICES_PER_QUAD = 6;
    constexpr size_t DEFAULT_QUAD_COUNT = 50;

    constexpr size_t FLOATS_PER_VERTEX = 2 + 2 + 3;
    constexpr size_t VERTEX_STRIDE = FLOATS_PER_VERTEX * sizeof(float);
    constexpr size_t OFFSET_OF_POS = 0;
    constexpr size_t OFFSET_OF_UV  = 2 * sizeof(float);
    constexpr size_t OFFSET_OF_COL = 4 * sizeof(float);

    static bool has_init = false;
    static std::vector<float> vertices{};
    if (!has_init) {
        vertices.reserve(VERTICES_PER_QUAD * DEFAULT_QUAD_COUNT);
        has_init = true;
    }
    vertices.clear();

    Font& font = m_VirtualFonts[font_id];

    float startX = static_cast<float>(x);
    while (*text) {
        char c = *text; text++;
        if (c == '\n') {
            x = static_cast<int>(startX);
            y += font.glyphs['|'].size_y;
            continue;
        }

        Glyph& glyph = font.glyphs[c];

        float x0 = x + glyph.bearing_x;
        float y0 = y + glyph.bearing_y;
        float x1 = x0 + glyph.size_x;
        float y1 = y0 + glyph.size_y;

        vertices.push_back(x0); vertices.push_back(y0); vertices.push_back(glyph.uv0.x); vertices.push_back(glyph.uv0.y);
            vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);
        vertices.push_back(x1); vertices.push_back(y0); vertices.push_back(glyph.uv1.x); vertices.push_back(glyph.uv0.y);
            vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);
        vertices.push_back(x1); vertices.push_back(y1); vertices.push_back(glyph.uv1.x); vertices.push_back(glyph.uv1.y);
            vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);

        vertices.push_back(x0); vertices.push_back(y0); vertices.push_back(glyph.uv0.x); vertices.push_back(glyph.uv0.y);
            vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);
        vertices.push_back(x1); vertices.push_back(y1); vertices.push_back(glyph.uv1.x); vertices.push_back(glyph.uv1.y);
            vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);
        vertices.push_back(x0); vertices.push_back(y1); vertices.push_back(glyph.uv0.x); vertices.push_back(glyph.uv1.y);
            vertices.push_back(color.r); vertices.push_back(color.g); vertices.push_back(color.b);

        x += glyph.advance;
    }

    if (font.text_mesh.array_buffer_id == 0) {
        glGenVertexArrays(1, &font.text_mesh.array_buffer_id);
        glGenBuffers(1, font.text_mesh.buffers);
    }
    glBindVertexArray(font.text_mesh.array_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, font.text_mesh.buffers[0]);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (void*)OFFSET_OF_POS);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (void*)OFFSET_OF_UV);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (void*)OFFSET_OF_COL);

    glUseProgram(m_FontShader.program_id);

    Shader* shader = m_CurrentShader;
    m_CurrentShader = &m_FontShader;

    set_uniform("u_Projection", m_GuiProjection);
    set_uniform("u_FontAtlas", 0);

    m_CurrentShader = shader;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_Textures[font.texture_id].id);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glBindVertexArray(0);
}

bool Renderer::mesh_is_dead(size_t index) const {
    return util::contains<size_t>(m_DeadMeshes.cbegin(), m_DeadMeshes.cend(), index);
}
bool Renderer::shader_is_dead(size_t index) const {
    return util::contains<size_t>(m_DeadShaders.cbegin(), m_DeadShaders.cend(), index);
}
bool Renderer::texture_is_dead(size_t index) const {
    return util::contains<size_t>(m_DeadTextures.cbegin(), m_DeadTextures.cend(), index);
}


void Renderer::delete_mesh(size_t index) noexcept {
    if (index >= m_Meshes.size() || mesh_is_dead(index)) {
        return;
    }
    Mesh& m = m_Meshes[index];
    glDeleteVertexArrays(1, &m.array_buffer_id);
    glDeleteBuffers(2, m.buffers);

    m.array_buffer_id = 0;
    m.buffers[0] = 0;
    m.buffers[1] = 0;

    m_DeadMeshes.push(index);

}
void Renderer::delete_shader(size_t index) noexcept {
    if (index >= m_Shaders.size() || shader_is_dead(index)) {
        return;
    }
    Shader& s = m_Shaders[index];
    s.uniform_cache.clear();
    glDeleteProgram(s.program_id);
    s.program_id = 0;

    m_DeadShaders.push(index);
}
void Renderer::delete_texture(size_t index) noexcept {
    if (index >= m_Textures.size() || texture_is_dead(index)) {
        return;
    }
    Texture& tex = m_Textures[index];
    glDeleteTextures(1, &tex.id);

    tex.id = 0;
    tex.width = 0;
    tex.height = 0;
    tex.channels = 0;

    m_DeadTextures.push(index);
}


int Renderer::estimate_atlas_size(size_t font_id, int padding) const noexcept{
    TTF_Font* font = m_Fonts.at(font_id);
    int width = 0, height = 0;
    int row_height = 0;
    int max_width = 0;
    int area = 0;

    for (char c = 32; c < 127; ++c) {
        int w, h;
        if (TTF_SizeText(font, std::string(1, c).c_str(), &w, &h) == 0) {
            width += w + padding;
            row_height = std::max(row_height, h);
            max_width = std::max(max_width, w);
            area += (w + padding) * h;
        }
    }
    int est_size = std::max((int)std::sqrt(area) * 2, 64);

    int size = 64;
    while (size < est_size) {
        size *= 2;
    }
    return size;
}

size_t Renderer::create_virtual_font(size_t font_id, int atlasWidth, int atlasHeight) {
    size_t vfont_id = m_VirtualFonts.size();
    Font& font = m_VirtualFonts.emplace_back();
    TTF_Font* ttf_font = m_Fonts.at(font_id);

    font.true_font_id = font_id;

    font.atlasWidth = atlasWidth;
    font.atlasHeight = atlasHeight;

    SDL_Surface* surf = SDL_CreateRGBSurface(0, atlasWidth, atlasHeight, 32,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    SDL_FillRect(surf, NULL, 0);

    int x = 0, y = 0, maxRowHeight = 0;
    for (char c = 32; c < 127; ++c) {
        SDL_Surface* glyphSurf = TTF_RenderGlyph_Blended(ttf_font, c, {255, 255, 255, 255});
        if (!glyphSurf) continue;

        if (x + glyphSurf->w >= atlasWidth) {
            x = 0;
            y += maxRowHeight;
            maxRowHeight = 0;
        }

        SDL_Rect dstRect{x, y, glyphSurf->w, glyphSurf->h};
        SDL_BlitSurface(glyphSurf, NULL, surf, &dstRect);

        Glyph glyph;
        glyph.uv0 = { x / (float)atlasWidth, y / (float)atlasHeight };
        glyph.uv1 = { (x + glyphSurf->w) / (float)atlasWidth, (y + glyphSurf->h) / (float)atlasHeight };
        glyph.size_x = glyphSurf->w;
        glyph.size_y = glyphSurf->h;
        TTF_GlyphMetrics(ttf_font, c, &glyph.bearing_x, NULL, &glyph.bearing_y, NULL, &glyph.advance);

        font.glyphs[c] = glyph;
        x += glyphSurf->w + 2;
        maxRowHeight = std::max(maxRowHeight, glyphSurf->h);

        SDL_FreeSurface(glyphSurf);
    }

    font.texture_id = m_Textures.size(); // todo: check for dead_texture slots
    Texture& texture = m_Textures.emplace_back();
    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlasWidth, atlasHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, surf->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    SDL_FreeSurface(surf);

    return vfont_id;
}
