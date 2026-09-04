#include "Shader.h"
#include "../Core/Assert.h"
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

namespace GE {

// ─── Factory: from source strings ────────────────────────────────────────────
std::shared_ptr<Shader> Shader::FromSource(std::string_view name,
                                            std::string_view vertSrc,
                                            std::string_view fragSrc,
                                            std::string_view geomSrc)
{
    auto shader = std::shared_ptr<Shader>(new Shader());
    shader->m_Name = std::string(name);
    if (!shader->Compile(vertSrc, fragSrc, geomSrc)) {
        GE_ERROR("Shader '{}' failed to compile", name);
        return nullptr;
    }
    GE_INFO("Shader '{}' compiled (id={})", name, shader->m_ProgramID);
    return shader;
}

// ─── Factory: from files ──────────────────────────────────────────────────────
std::shared_ptr<Shader> Shader::FromFiles(std::string_view name,
                                           const std::filesystem::path& vertPath,
                                           const std::filesystem::path& fragPath,
                                           const std::filesystem::path& geomPath)
{
    auto readFile = [](const std::filesystem::path& p) -> std::string {
        if (p.empty()) return {};
        std::ifstream f(p);
        if (!f.is_open()) {
            GE_ERROR("Cannot open shader file: {}", p.string());
            return {};
        }
        std::ostringstream ss;
        ss << f.rdbuf();
        return ss.str();
    };

    std::string vert = readFile(vertPath);
    std::string frag = readFile(fragPath);
    std::string geom = readFile(geomPath);
    if (vert.empty() || frag.empty()) return nullptr;

    std::string shaderName = name.empty() ? vertPath.stem().string() : std::string(name);
    auto shader = std::shared_ptr<Shader>(new Shader());
    shader->m_Name     = shaderName;
    shader->m_VertPath = vertPath;
    shader->m_FragPath = fragPath;
    shader->m_GeomPath = geomPath;

    if (!shader->Compile(vert, frag, geom)) {
        GE_ERROR("Shader '{}' failed to compile from files", shaderName);
        return nullptr;
    }
    GE_INFO("Shader '{}' loaded from files (id={})", shaderName, shader->m_ProgramID);
    return shader;
}

// ─── Destructor ───────────────────────────────────────────────────────────────
Shader::~Shader() {
    if (m_ProgramID) {
        glDeleteProgram(m_ProgramID);
        m_ProgramID = 0;
    }
}

// ─── Bind / Unbind ────────────────────────────────────────────────────────────
void Shader::Bind()   const { glUseProgram(m_ProgramID); }
void Shader::Unbind() const { glUseProgram(0); }

// ─── Hot reload ───────────────────────────────────────────────────────────────
bool Shader::Reload() {
    if (m_VertPath.empty()) {
        GE_WARN("Shader '{}': no file paths stored, cannot hot-reload", m_Name);
        return false;
    }
    auto readFile = [](const std::filesystem::path& p) -> std::string {
        if (p.empty()) return {};
        std::ifstream f(p);
        if (!f.is_open()) return {};
        std::ostringstream ss; ss << f.rdbuf(); return ss.str();
    };
    std::string vert = readFile(m_VertPath);
    std::string frag = readFile(m_FragPath);
    std::string geom = readFile(m_GeomPath);
    if (vert.empty() || frag.empty()) return false;

    GLuint oldProgram = m_ProgramID;
    m_ProgramID = 0;
    m_UniformCache.clear();

    if (Compile(vert, frag, geom)) {
        glDeleteProgram(oldProgram);
        GE_INFO("Shader '{}' hot-reloaded", m_Name);
        return true;
    }
    // Restore old program on failure
    m_ProgramID = oldProgram;
    GE_ERROR("Shader '{}' hot-reload failed, keeping old program", m_Name);
    return false;
}

// ─── Compile ─────────────────────────────────────────────────────────────────
bool Shader::Compile(std::string_view vertSrc, std::string_view fragSrc, std::string_view geomSrc) {
    GLuint vert = CompileStage(GL_VERTEX_SHADER,   vertSrc, m_Name);
    GLuint frag = CompileStage(GL_FRAGMENT_SHADER, fragSrc, m_Name);
    GLuint geom = 0;
    if (!geomSrc.empty()) geom = CompileStage(GL_GEOMETRY_SHADER, geomSrc, m_Name);

    if (!vert || !frag) {
        if (vert) glDeleteShader(vert);
        if (frag) glDeleteShader(frag);
        if (geom) glDeleteShader(geom);
        return false;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    if (geom) glAttachShader(program, geom);
    glLinkProgram(program);

    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetProgramInfoLog(program, len, &len, log.data());
        GE_ERROR("Shader '{}' link error:\n{}", m_Name, log);
        glDeleteProgram(program);
        glDeleteShader(vert);
        glDeleteShader(frag);
        if (geom) glDeleteShader(geom);
        return false;
    }

    // Shaders are no longer needed after linking
    glDetachShader(program, vert); glDeleteShader(vert);
    glDetachShader(program, frag); glDeleteShader(frag);
    if (geom) { glDetachShader(program, geom); glDeleteShader(geom); }

    m_ProgramID = program;
    return true;
}

GLuint Shader::CompileStage(GLenum type, std::string_view src, std::string_view name) {
    GLuint shader = glCreateShader(type);
    const char* srcPtr  = src.data();
    GLint       srcLen  = static_cast<GLint>(src.size());
    glShaderSource(shader, 1, &srcPtr, &srcLen);
    glCompileShader(shader);

    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetShaderInfoLog(shader, len, &len, log.data());
        const char* typeStr = (type == GL_VERTEX_SHADER) ? "Vertex"
                            : (type == GL_FRAGMENT_SHADER) ? "Fragment" : "Geometry";
        GE_ERROR("Shader '{}' {} compile error:\n{}", name, typeStr, log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

// ─── Uniform location (cached) ────────────────────────────────────────────────
int Shader::GetUniformLocation(std::string_view name) const {
    auto key = std::string(name);
    if (auto it = m_UniformCache.find(key); it != m_UniformCache.end())
        return it->second;
    int loc = glGetUniformLocation(m_ProgramID, key.c_str());
    if (loc == -1) {
        GE_WARN("Shader '{}': uniform '{}' not found", m_Name, name);
    }
    m_UniformCache[key] = loc;
    return loc;
}

// ─── Uniform setters ─────────────────────────────────────────────────────────
void Shader::SetBool(std::string_view n, bool v) const {
    if (const int location = GetUniformLocation(n); location >= 0) glUniform1i(location, v ? 1 : 0);
}
void Shader::SetInt(std::string_view n, int v) const {
    if (const int location = GetUniformLocation(n); location >= 0) glUniform1i(location, v);
}
void Shader::SetUInt(std::string_view n, uint32_t v) const {
    if (const int location = GetUniformLocation(n); location >= 0) glUniform1ui(location, v);
}
void Shader::SetFloat(std::string_view n, float v) const {
    if (const int location = GetUniformLocation(n); location >= 0) glUniform1f(location, v);
}
void Shader::SetVec2(std::string_view n, const Vec2& v) const {
    if (const int location = GetUniformLocation(n); location >= 0) glUniform2f(location, v.x, v.y);
}
void Shader::SetVec3(std::string_view n, const Vec3& v) const {
    if (const int location = GetUniformLocation(n); location >= 0) glUniform3f(location, v.x, v.y, v.z);
}
void Shader::SetVec4(std::string_view n, const Vec4& v) const {
    if (const int location = GetUniformLocation(n); location >= 0) glUniform4f(location, v.x, v.y, v.z, v.w);
}
void Shader::SetMat3(std::string_view n, const Mat3& m) const {
    if (const int location = GetUniformLocation(n); location >= 0) glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(m));
}
void Shader::SetMat4(std::string_view n, const Mat4& m) const {
    if (const int location = GetUniformLocation(n); location >= 0) glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m));
}
void Shader::SetIntArray(std::string_view n, const int* v, uint32_t c) const {
    if (const int location = GetUniformLocation(n); location >= 0) glUniform1iv(location, static_cast<GLsizei>(c), v);
}

// ─── ShaderLibrary ────────────────────────────────────────────────────────────
ShaderLibrary& ShaderLibrary::Get() {
    static ShaderLibrary instance;
    return instance;
}

void ShaderLibrary::Add(std::shared_ptr<Shader> shader) {
    GE_ASSERT(shader, "Cannot add null shader");
    m_Shaders[shader->GetName()] = shader;
}

std::shared_ptr<Shader> ShaderLibrary::Load(const std::filesystem::path& vertPath,
                                              const std::filesystem::path& fragPath,
                                              std::string_view name)
{
    std::string n = name.empty() ? vertPath.stem().string() : std::string(name);
    auto shader = Shader::FromFiles(n, vertPath, fragPath);
    if (shader) {
        m_Shaders[n] = shader;
        m_FileTimes[vertPath.string()] = std::filesystem::last_write_time(vertPath);
        m_FileTimes[fragPath.string()] = std::filesystem::last_write_time(fragPath);
    }
    return shader;
}

std::shared_ptr<Shader> ShaderLibrary::Get(std::string_view name) {
    auto it = m_Shaders.find(std::string(name));
    if (it == m_Shaders.end()) {
        GE_ERROR("Shader '{}' not found in library", name);
        return nullptr;
    }
    return it->second;
}

bool ShaderLibrary::Exists(std::string_view name) const {
    return m_Shaders.contains(std::string(name));
}

void ShaderLibrary::HotReloadAll() {
    for (auto& [name, shader] : m_Shaders) {
        // Check if vert or frag file changed
        bool changed = false;
        for (auto* path : { &shader->m_VertPath, &shader->m_FragPath, &shader->m_GeomPath }) {
            if (path->empty()) continue;
            auto key = path->string();
            if (!std::filesystem::exists(*path)) continue;
            auto newTime = std::filesystem::last_write_time(*path);
            if (m_FileTimes.contains(key) && m_FileTimes[key] != newTime) {
                changed = true;
                m_FileTimes[key] = newTime;
            }
        }
        if (changed) shader->Reload();
    }
}

} // namespace GE
