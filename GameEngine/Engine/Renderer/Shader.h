#pragma once
#include "../Core/Logger.h"
#include <glad/gl.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <filesystem>
#include "../Math/Math.h"

namespace GE {

// ─────────────────────────────────────────────────────────────────────────────
// Shader — GLSL shader program with uniform cache and hot-reload support
// ─────────────────────────────────────────────────────────────────────────────
class Shader {
public:
    // Create from GLSL source strings directly
    static std::shared_ptr<Shader> FromSource(std::string_view name,
                                               std::string_view vertSrc,
                                               std::string_view fragSrc,
                                               std::string_view geomSrc = "");

    // Create from files — stores paths for hot-reload
    static std::shared_ptr<Shader> FromFiles(std::string_view name,
                                              const std::filesystem::path& vertPath,
                                              const std::filesystem::path& fragPath,
                                              const std::filesystem::path& geomPath = "");

    ~Shader();

    void Bind()   const;
    void Unbind() const;

    bool Reload();   // Recompile from file sources (hot-reload)
    bool IsValid() const { return m_ProgramID != 0; }

    const std::string& GetName() const { return m_Name; }
    uint32_t           GetID()   const { return m_ProgramID; }

    // ── Uniform setters ──────────────────────────────────────────────────────
    void SetBool (std::string_view name, bool  v)  const;
    void SetInt  (std::string_view name, int   v)  const;
    void SetUInt (std::string_view name, uint32_t v) const;
    void SetFloat(std::string_view name, float v)  const;
    void SetVec2 (std::string_view name, const Vec2& v) const;
    void SetVec3 (std::string_view name, const Vec3& v) const;
    void SetVec4 (std::string_view name, const Vec4& v) const;
    void SetMat3 (std::string_view name, const Mat3& m) const;
    void SetMat4 (std::string_view name, const Mat4& m) const;
    void SetIntArray(std::string_view name, const int* values, uint32_t count) const;

private:
    Shader() = default;
    bool Compile(std::string_view vertSrc, std::string_view fragSrc, std::string_view geomSrc);
    static GLuint CompileStage(GLenum type, std::string_view src, std::string_view name);
    int  GetUniformLocation(std::string_view name) const;

    std::string            m_Name;
    GLuint                 m_ProgramID = 0;
    std::filesystem::path  m_VertPath, m_FragPath, m_GeomPath;
    mutable std::unordered_map<std::string, int> m_UniformCache;

    friend class ShaderLibrary;
};

// ─────────────────────────────────────────────────────────────────────────────
// ShaderLibrary — named cache + hot-reload manager
// ─────────────────────────────────────────────────────────────────────────────
class ShaderLibrary {
public:
    static ShaderLibrary& Get();

    void Add(std::shared_ptr<Shader> shader);
    std::shared_ptr<Shader> Load(const std::filesystem::path& vertPath,
                                  const std::filesystem::path& fragPath,
                                  std::string_view name = "");
    std::shared_ptr<Shader> Get(std::string_view name);
    bool Exists(std::string_view name) const;

    // Reload all shaders whose source files changed on disk
    void HotReloadAll();

private:
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
    std::unordered_map<std::string, std::filesystem::file_time_type> m_FileTimes;
};

} // namespace GE
