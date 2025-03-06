#include "renderer.h"

#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"

Renderer::Renderer() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        throw std::runtime_error("Unable to initialize SDL");
    }
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
    Mesh& mesh = m_Meshes.emplace_back(0);

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

RendererError Renderer::upload_shader(const char *vertex_source, const char *fragment_source, size_t &shader_id) noexcept {
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

    shader_id = m_Shaders.size();
    Shader& shader = m_Shaders.emplace_back();

    shader.program_id = prog_id;
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


void Renderer::set_uniform(const std::string_view name, const mat4 &value) {
    uint32_t loc;
    if (loc=get_loc(name); loc == -1) {
        return;
    }

    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

size_t Renderer::upload_texture(const char *file, bool v_flip) noexcept {
    Texture& texture = m_Textures.emplace_back();
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

