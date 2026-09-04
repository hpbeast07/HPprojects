#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// Mesh.h — indexed mesh with GPU buffers, procedural primitive factory and
//          Wavefront OBJ importer
// ─────────────────────────────────────────────────────────────────────────────
#include "../Math/Math.h"
#include "Buffer.h"
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace GE {

// ─────────────────────────────────────────────────────────────────────────────
// Vertex — matches the vertex buffer layout in Buffer.h/index buffers:
//   0: vec3 position   1: vec3 normal   2: vec3 tangent   3: vec2 uv
// ─────────────────────────────────────────────────────────────────────────────
struct Vertex {
    Vec3 position;
    Vec3 normal   { 0.0f, 1.0f, 0.0f };
    Vec3 tangent  { 1.0f, 0.0f, 0.0f };
    Vec2 uv       { 0.0f };

    static BufferLayout GetLayout() {
        return {
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Normal"   },
            { ShaderDataType::Float3, "a_Tangent"  },
            { ShaderDataType::Float2, "a_TexCoord" },
        };
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// Mesh — owns its GPU vertex/index storage
// ─────────────────────────────────────────────────────────────────────────────
class Mesh {
public:
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices);
    ~Mesh() = default;

    Mesh(const Mesh&)            = delete;
    Mesh& operator=(const Mesh&) = delete;

    void Bind() const;
    void Unbind() const;

    [[nodiscard]] uint32_t GetVertexCount() const { return m_Indices.empty() ? static_cast<uint32_t>(m_Vertices.size()) : static_cast<uint32_t>(m_Indices.size()); }
    [[nodiscard]] uint32_t GetIndexCount()  const { return static_cast<uint32_t>(m_Indices.size()); }
    [[nodiscard]] bool     IsIndexed()       const { return !m_Indices.empty(); }
    [[nodiscard]] AABB     GetBounds()       const { return m_Bounds; }

    const std::vector<Vertex>&   GetVertices() const { return m_Vertices; }
    const std::vector<uint32_t>& GetIndices()  const { return m_Indices;  }

private:
    void ComputeBounds();

    std::shared_ptr<VertexArray>  m_VAO;
    std::shared_ptr<VertexBuffer> m_VBO;
    std::shared_ptr<IndexBuffer>  m_IBO;
    std::vector<Vertex>           m_Vertices;
    std::vector<uint32_t>         m_Indices;
    AABB                          m_Bounds;
};

// ─────────────────────────────────────────────────────────────────────────────
// MeshLibrary — cached procedural primitives + disk meshes
// ─────────────────────────────────────────────────────────────────────────────
class MeshLibrary {
public:
    static MeshLibrary& Get();

    std::shared_ptr<Mesh> Cube();
    std::shared_ptr<Mesh> Plane();          // 1x1 unit quad lying on XZ
    std::shared_ptr<Mesh> Sphere();         // unit UV sphere, smooth normals
    std::shared_ptr<Mesh> Icosphere();      // unit icosphere (for fun)
    std::shared_ptr<Mesh> Cylinder();       // unit cylinder (h=1, r=0.5)
    std::shared_ptr<Mesh> Cone();           // unit cone
    std::shared_ptr<Mesh> Torus();          // torus r=0.35, tube=0.14
    std::shared_ptr<Mesh> Grid();           // 1x1 grid plane (tangents ready)

    // Loads an OBJ file (positions, normals, texture coordinates, quads/tris).
    std::shared_ptr<Mesh> LoadOBJ(const std::string& path);

    // Lower-level construction utilities (used by apps to make bespoke meshes).
    static std::shared_ptr<Mesh> BuildMesh(std::vector<Vertex> vertices,
                                           std::vector<uint32_t> indices);

private:
    std::shared_ptr<Mesh> m_Cube;
    std::shared_ptr<Mesh> m_Plane;
    std::shared_ptr<Mesh> m_Sphere;
    std::shared_ptr<Mesh> m_Icosphere;
    std::shared_ptr<Mesh> m_Cylinder;
    std::shared_ptr<Mesh> m_Cone;
    std::shared_ptr<Mesh> m_Torus;
    std::shared_ptr<Mesh> m_Grid;

    std::unordered_map<std::string, std::shared_ptr<Mesh>> m_LoadedFiles;
};

} // namespace GE