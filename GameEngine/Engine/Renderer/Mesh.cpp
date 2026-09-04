#include "Mesh.h"
#include "../Core/Logger.h"
#include "../Core/Assert.h"

#include <cmath>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <limits>

namespace GE {

// ─── Construction ────────────────────────────────────────────────────────────
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
    : m_Vertices(std::move(vertices)), m_Indices(std::move(indices))
{
    GE_ASSERT(!m_Vertices.empty(), "Mesh constructed with no vertices");

    ComputeBounds();

    m_VBO = std::make_shared<VertexBuffer>(m_Vertices.data(),
                                           static_cast<uint32_t>(m_Vertices.size() * sizeof(Vertex)));
    m_VBO->SetLayout(Vertex::GetLayout());
    m_VAO = std::make_shared<VertexArray>();
    m_VAO->AddVertexBuffer(m_VBO);

    if (!m_Indices.empty()) {
        m_IBO = std::make_shared<IndexBuffer>(m_Indices.data(),
                                              static_cast<uint32_t>(m_Indices.size()));
        m_VAO->SetIndexBuffer(m_IBO);
    }
}

void Mesh::ComputeBounds() {
    m_Bounds.min = Vec3(std::numeric_limits<float>::max());
    m_Bounds.max = Vec3(std::numeric_limits<float>::lowest());
    for (const Vertex& v : m_Vertices) {
        m_Bounds.min = glm::min(m_Bounds.min, v.position);
        m_Bounds.max = glm::max(m_Bounds.max, v.position);
    }
}

void Mesh::Bind()   const { if (m_VAO) m_VAO->Bind(); }
void Mesh::Unbind() const { if (m_VAO) m_VAO->Unbind(); }

std::shared_ptr<Mesh> MeshLibrary::BuildMesh(std::vector<Vertex> vertices,
                                             std::vector<uint32_t> indices) {
    return std::make_shared<Mesh>(std::move(vertices), std::move(indices));
}

// ─── Small helpers for procedural generation ─────────────────────────────────
namespace {

Vec3 SafeNormalize(Vec3 v) {
    float len = glm::length(v);
    return len > 1e-6f ? v / len : Vec3(0.0f, 1.0f, 0.0f);
}

// Tangent from an arbitrary non-parallel reference vector.
Vec3 ComputeTangent(const Vec3& normal, const Vec3& reference = Vec3(0.0f, 1.0f, 0.0f)) {
    Vec3 b = glm::abs(glm::dot(normal, reference)) > 0.99f
        ? Vec3(0.0f, 0.0f, 1.0f)
        : reference;
    return SafeNormalize(glm::cross(b, normal));
}

} // namespace

// ─── Cube ────────────────────────────────────────────────────────────────────
std::shared_ptr<Mesh> MeshLibrary::Cube() {
    if (m_Cube) return m_Cube;

    // 6 faces × 4 corners — unique vertices so normals stay sharp.
    static const struct Face {
        Vec3 normal;
        Vec3 uAxis, vAxis;   // tangent axes in local space
    } faces[6] = {
        { { 0.0f, 0.0f, 1.0f },  { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },  // +Z
        { { 0.0f, 0.0f,-1.0f },  {-1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },  // -Z
        { { 1.0f, 0.0f, 0.0f },  { 0.0f, 0.0f,-1.0f }, { 0.0f, 1.0f, 0.0f } },  // +X
        { {-1.0f, 0.0f, 0.0f },  { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },  // -X
        { { 0.0f, 1.0f, 0.0f },  { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f,-1.0f } },  // +Y
        { { 0.0f,-1.0f, 0.0f },  { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },  // -Y
    };

    std::vector<Vertex> verts;
    verts.reserve(24);
    std::vector<uint32_t> idx;
    idx.reserve(36);

    for (uint32_t f = 0; f < 6; f++) {
        const Vec3& n = faces[f].normal;
        const Vec3& u = faces[f].uAxis;
        const Vec3& v = faces[f].vAxis;
        const uint32_t base = static_cast<uint32_t>(verts.size());

        const Vec3 corners[4] = {
            (-u - v) * 0.5f, (u - v) * 0.5f, (u + v) * 0.5f, (-u + v) * 0.5f,
        };
        for (uint32_t c = 0; c < 4; c++) {
            Vertex vertex;
            vertex.position = corners[c];
            vertex.normal   = n;
            vertex.tangent  = u;
            vertex.uv       = Vec2((c == 1 || c == 2) ? 1.0f : 0.0f,
                                   (c == 2 || c == 3) ? 1.0f : 0.0f);
            verts.push_back(vertex);
        }
        idx.insert(idx.end(), { base + 0, base + 1, base + 2, base + 0, base + 2, base + 3 });
    }

    m_Cube = BuildMesh(std::move(verts), std::move(idx));
    return m_Cube;
}
// ─── Plane (1×1 unit, lies on XZ, 32×32 subdivisions) ────────────────────────
std::shared_ptr<Mesh> MeshLibrary::Plane() {
    if (m_Plane) return m_Plane;

    constexpr uint32_t SEG = 32;
    std::vector<Vertex> verts;
    verts.reserve((SEG + 1) * (SEG + 1));
    std::vector<uint32_t> idx;
    idx.reserve(SEG * SEG * 6);

    for (uint32_t z = 0; z <= SEG; z++) {
        for (uint32_t x = 0; x <= SEG; x++) {
            Vertex vertex;
            vertex.position = Vec3(static_cast<float>(x) / SEG - 0.5f, 0.0f,
                                   static_cast<float>(z) / SEG - 0.5f);
            vertex.normal   = Vec3(0.0f, 1.0f, 0.0f);
            vertex.tangent  = Vec3(1.0f, 0.0f, 0.0f);
            vertex.uv       = Vec2(static_cast<float>(x) / SEG * 8.0f,
                                   static_cast<float>(z) / SEG * 8.0f);
            verts.push_back(vertex);
        }
    }

    for (uint32_t z = 0; z < SEG; z++) {
        for (uint32_t x = 0; x < SEG; x++) {
            const uint32_t a = z * (SEG + 1) + x;
            const uint32_t b = a + 1;
            const uint32_t c = a + (SEG + 1);
            const uint32_t d = c + 1;
            idx.insert(idx.end(), { a, c, d, a, d, b });
        }
    }

    m_Plane = BuildMesh(std::move(verts), std::move(idx));
    return m_Plane;
}

// ─── UV Sphere (smooth normals, radius 0.5) ─────────────────────────────────
std::shared_ptr<Mesh> MeshLibrary::Sphere() {
    if (m_Sphere) return m_Sphere;

    constexpr uint32_t STACKS = 24;
    constexpr uint32_t SECTS  = 32;
    constexpr float    RADIUS = 0.5f;

    std::vector<Vertex> verts;
    verts.reserve((STACKS + 1) * (SECTS + 1));
    std::vector<uint32_t> idx;
    idx.reserve(STACKS * SECTS * 6);

    for (uint32_t s = 0; s <= STACKS; s++) {
        const float theta = static_cast<float>(s) / STACKS * PI;   // 0..pi
        const float sinT  = std::sin(theta);
        const float cosT  = std::cos(theta);

        for (uint32_t q = 0; q <= SECTS; q++) {
            const float phi   = static_cast<float>(q) / SECTS * TWO_PI;  // 0..2pi
            const float sinP  = std::sin(phi);
            const float cosP  = std::cos(phi);

            Vertex vertex;
            vertex.position = Vec3(RADIUS * cosP * sinT, RADIUS * cosT, RADIUS * sinP * sinT);
            vertex.normal   = SafeNormalize(vertex.position);
            vertex.tangent  = ComputeTangent(vertex.normal);
            vertex.uv       = Vec2(static_cast<float>(q) / SECTS, 1.0f - static_cast<float>(s) / STACKS);
            verts.push_back(vertex);
        }
    }

    for (uint32_t s = 0; s < STACKS; s++) {
        for (uint32_t q = 0; q < SECTS; q++) {
            const uint32_t a = s * (SECTS + 1) + q;
            const uint32_t b = a + 1;
            const uint32_t c = a + (SECTS + 1);
            const uint32_t d = c + 1;
            idx.insert(idx.end(), { a, c, d, a, d, b });
        }
    }

    m_Sphere = BuildMesh(std::move(verts), std::move(idx));
    return m_Sphere;
}

// ─── Icosphere (unit sphere from subdivided Icosahedron) ─────────────────────
std::shared_ptr<Mesh> MeshLibrary::Icosphere() {
    if (m_Icosphere) return m_Icosphere;

    constexpr float I = 0.5257311121191336f;
    constexpr float J = 0.8506508083520399f;

    std::vector<Vec3> base = {
        { -I,  0.0f,  J }, {  I,  0.0f,  J }, { -I,  0.0f, -J }, {  I,  0.0f, -J },
        {  0.0f,  J,  I }, {  0.0f,  J, -I }, {  0.0f, -J,  I }, {  0.0f, -J, -I },
        {  J,  I,  0.0f }, { -J,  I,  0.0f }, {  J, -I,  0.0f }, { -J, -I,  0.0f },
    };
    std::vector<uint32_t> lines = {
        0,1,4,  1,9,4,  4,9,5,  8,5,4,  1,8,4,   // 5 faces around vertex 4
        1,10,8, 10,3,8, 8,3,5,  3,2,5, 3,7,2,    // 5 faces adjacent to first
        3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,   // 5 faces around vertex 0
        10,1,6, 11,0,9, 2,11,9, 5,2,9, 11,2,7,   // 5 faces around vertex 11
    };

    std::unordered_map<uint64_t, uint32_t> midCache;
    auto getMid = [&](uint32_t a, uint32_t b) -> uint32_t {
        if (a > b) std::swap(a, b);
        const uint64_t key = (static_cast<uint64_t>(a) << 32) | b;
        if (auto it = midCache.find(key); it != midCache.end()) return it->second;
        const size_t index = base.size();
        base.push_back(SafeNormalize(base[a] + base[b]));
        midCache[key] = static_cast<uint32_t>(index);
        return static_cast<uint32_t>(index);
    };

    std::vector<uint32_t> tris;
    tris.reserve(lines.size() + 6 * lines.size());
    tris = lines;

    for (uint32_t subdiv = 0; subdiv < 2; subdiv++) {
        std::vector<uint32_t> next;
        next.reserve(tris.size() * 4);
        for (size_t t = 0; t < tris.size(); t += 3) {
            const uint32_t v0 = tris[t + 0], v1 = tris[t + 1], v2 = tris[t + 2];
            const uint32_t m01 = getMid(v0, v1);
            const uint32_t m12 = getMid(v1, v2);
            const uint32_t m20 = getMid(v2, v0);
            next.insert(next.end(), { v0, m01, m20,  m01, v1, m12,  m20, m12, v2,  m01, m12, m20 });
        }
        tris.swap(next);
    }

    std::vector<Vertex> verts;
    verts.reserve(base.size());
    for (const Vec3& p : base) {
        Vertex vertex;
        vertex.position = p * 0.5f;
        vertex.normal   = p;
        vertex.tangent  = ComputeTangent(p);
        vertex.uv       = Vec2(0.5f + std::atan2(p.z, p.x) / TWO_PI,
                               0.5f - std::asin(glm::clamp(p.y, -1.0f, 1.0f)) / PI);
        verts.push_back(vertex);
    }

    m_Icosphere = BuildMesh(std::move(verts), std::move(tris));
    return m_Icosphere;
}
// ─── Cylinder (height 1, radius 0.5, capped) ────────────────────────────────
std::shared_ptr<Mesh> MeshLibrary::Cylinder() {
    if (m_Cylinder) return m_Cylinder;

    constexpr uint32_t SEG      = 32;
    constexpr float    RADIUS   = 0.5f;
    constexpr float    HALF_H   = 0.5f;

    std::vector<Vertex> verts;
    std::vector<uint32_t> idx;

    // ── Side ──
    for (uint32_t i = 0; i <= SEG; i++) {
        const float a = static_cast<float>(i) / SEG * TWO_PI;
        Vertex v;
        v.position = Vec3(std::cos(a) * RADIUS, -HALF_H, std::sin(a) * RADIUS);
        v.normal   = Vec3(std::cos(a), 0.0f, std::sin(a));
        v.tangent  = Vec3(-std::sin(a), 0.0f, std::cos(a));
        v.uv       = Vec2(static_cast<float>(i) / SEG, 0.0f);
        verts.push_back(v);
    }
    const uint32_t ringBottom = static_cast<uint32_t>(verts.size());
    for (uint32_t i = 0; i <= SEG; i++) {
        const float a = static_cast<float>(i) / SEG * TWO_PI;
        Vertex v;
        v.position = Vec3(std::cos(a) * RADIUS, HALF_H, std::sin(a) * RADIUS);
        v.normal   = Vec3(std::cos(a), 0.0f, std::sin(a));
        v.tangent  = Vec3(-std::sin(a), 0.0f, std::cos(a));
        v.uv       = Vec2(static_cast<float>(i) / SEG, 1.0f);
        verts.push_back(v);
    }
    for (uint32_t i = 0; i < SEG; i++) {
        const uint32_t b0 = i;
        const uint32_t b1 = i + 1;
        const uint32_t t0 = ringBottom + i;
        const uint32_t t1 = ringBottom + i + 1;
        idx.insert(idx.end(), { b0, t0, t1, b0, t1, b1 });
    }

    // ── Bottom cap ──
    const uint32_t bottomCenter = static_cast<uint32_t>(verts.size());
    {
        Vertex v;
        v.position = Vec3(0.0f, -HALF_H, 0.0f);
        v.normal   = Vec3(0.0f, -1.0f, 0.0f);
        v.tangent  = Vec3(1.0f, 0.0f, 0.0f);
        v.uv       = Vec2(0.5f);
        verts.push_back(v);
    }
    for (uint32_t i = 0; i <= SEG; i++) {
        const float a = -static_cast<float>(i) / SEG * TWO_PI;
        Vertex v;
        v.position = Vec3(std::cos(a) * RADIUS, -HALF_H, std::sin(a) * RADIUS);
        v.normal   = Vec3(0.0f, -1.0f, 0.0f);
        v.tangent  = Vec3(1.0f, 0.0f, 0.0f);
        v.uv       = Vec2(std::cos(a) * 0.5f + 0.5f, std::sin(a) * 0.5f + 0.5f);
        verts.push_back(v);
    }
    for (uint32_t i = 0; i < SEG; i++) {
        const uint32_t first  = bottomCenter + 1 + i + 1;
        const uint32_t second = bottomCenter + 1 + i;
        idx.insert(idx.end(), { bottomCenter, first, second });
    }

    // ── Top cap ──
    const uint32_t topCenter = static_cast<uint32_t>(verts.size());
    {
        Vertex v;
        v.position = Vec3(0.0f, HALF_H, 0.0f);
        v.normal   = Vec3(0.0f, 1.0f, 0.0f);
        v.tangent  = Vec3(1.0f, 0.0f, 0.0f);
        v.uv       = Vec2(0.5f);
        verts.push_back(v);
    }
    for (uint32_t i = 0; i <= SEG; i++) {
        const float a = static_cast<float>(i) / SEG * TWO_PI;
        Vertex v;
        v.position = Vec3(std::cos(a) * RADIUS, HALF_H, std::sin(a) * RADIUS);
        v.normal   = Vec3(0.0f, 1.0f, 0.0f);
        v.tangent  = Vec3(1.0f, 0.0f, 0.0f);
        v.uv       = Vec2(std::cos(a) * 0.5f + 0.5f, std::sin(a) * 0.5f + 0.5f);
        verts.push_back(v);
    }
    for (uint32_t i = 0; i < SEG; i++) {
        const uint32_t first  = topCenter + 1 + i;
        const uint32_t second = topCenter + 1 + i + 1;
        idx.insert(idx.end(), { topCenter, first, second });
    }

    m_Cylinder = BuildMesh(std::move(verts), std::move(idx));
    return m_Cylinder;
}
// ─── Cone (height 1, base radius 0.5) ────────────────────────────────────────
std::shared_ptr<Mesh> MeshLibrary::Cone() {
    if (m_Cone) return m_Cone;

    constexpr uint32_t SEG    = 32;
    constexpr float    RADIUS = 0.5f;
    constexpr float    HALF_H = 0.5f;

    std::vector<Vertex> verts;
    std::vector<uint32_t> idx;

    // Side: apex + base ring (independent apex vertex for a sharp tip).
    const uint32_t apex = static_cast<uint32_t>(verts.size());
    {
        Vertex v;
        v.position = Vec3(0.0f, HALF_H, 0.0f);
        v.normal   = Vec3(0.0f, 1.0f, 0.0f);
        v.tangent  = Vec3(1.0f, 0.0f, 0.0f);
        v.uv       = Vec2(0.5f, 1.0f);
        verts.push_back(v);
    }
    for (uint32_t i = 0; i <= SEG; i++) {
        const float a = static_cast<float>(i) / SEG * TWO_PI;
        Vertex v;
        v.position = Vec3(std::cos(a) * RADIUS, -HALF_H, std::sin(a) * RADIUS);
        v.normal   = SafeNormalize(Vec3(std::cos(a), 0.8f, std::sin(a)));
        v.tangent  = Vec3(-std::sin(a), 0.0f, std::cos(a));
        v.uv       = Vec2(static_cast<float>(i) / SEG, 0.0f);
        verts.push_back(v);
    }
    for (uint32_t i = 0; i < SEG; i++) {
        idx.insert(idx.end(), { apex, i + 1, i + 2 });
    }

    // Bottom cap.
    const uint32_t baseCenter = static_cast<uint32_t>(verts.size());
    {
        Vertex v;
        v.position = Vec3(0.0f, -HALF_H, 0.0f);
        v.normal   = Vec3(0.0f, -1.0f, 0.0f);
        v.tangent  = Vec3(1.0f, 0.0f, 0.0f);
        v.uv       = Vec2(0.5f);
        verts.push_back(v);
    }
    for (uint32_t i = 0; i <= SEG; i++) {
        const float a = -static_cast<float>(i) / SEG * TWO_PI;
        Vertex v;
        v.position = Vec3(std::cos(a) * RADIUS, -HALF_H, std::sin(a) * RADIUS);
        v.normal   = Vec3(0.0f, -1.0f, 0.0f);
        v.tangent  = Vec3(1.0f, 0.0f, 0.0f);
        v.uv       = Vec2(std::cos(a) * 0.5f + 0.5f, std::sin(a) * 0.5f + 0.5f);
        verts.push_back(v);
    }
    for (uint32_t i = 0; i < SEG; i++) {
        const uint32_t first  = baseCenter + 1 + i + 1;
        const uint32_t second = baseCenter + 1 + i;
        idx.insert(idx.end(), { baseCenter, first, second });
    }

    m_Cone = BuildMesh(std::move(verts), std::move(idx));
    return m_Cone;
}
// ─── Torus (quads, tangents aligned to wrap direction) ───────────────────────
std::shared_ptr<Mesh> MeshLibrary::Torus() {
    if (m_Torus) return m_Torus;

    constexpr uint32_t SEG  = 40;   // around the ring
    constexpr uint32_t SIDE = 16;   // around the tube
    constexpr float    MAJOR = 0.35f;
    constexpr float    MINOR = 0.14f;

    std::vector<Vertex> verts;
    verts.reserve((SEG + 1) * (SIDE + 1));
    std::vector<uint32_t> idx;
    idx.reserve(SEG * SIDE * 6);

    for (uint32_t s = 0; s <= SEG; s++) {
        const float a = static_cast<float>(s) / SEG * TWO_PI;
        const float cosA = std::cos(a), sinA = std::sin(a);

        for (uint32_t q = 0; q <= SIDE; q++) {
            const float b       = static_cast<float>(q) / SIDE * TWO_PI;
            const float cosB    = std::cos(b), sinB = std::sin(b);

            const Vec3 center(cosA * MAJOR, sinA * MAJOR, 0.0f);
            const Vec3 radial(cosA * cosB, sinA * cosB, sinB);

            Vertex vertex;
            vertex.position = center + radial * MINOR;
            vertex.normal   = radial;
            vertex.tangent  = Vec3(-sinA, cosA, 0.0f);
            vertex.uv       = Vec2(static_cast<float>(s) / SEG, static_cast<float>(q) / SIDE);
            verts.push_back(vertex);
        }
    }

    for (uint32_t s = 0; s < SEG; s++) {
        for (uint32_t q = 0; q < SIDE; q++) {
            const uint32_t a = s * (SIDE + 1) + q;
            const uint32_t b = a + 1;
            const uint32_t c = a + (SIDE + 1);
            const uint32_t d = c + 1;
            idx.insert(idx.end(), { a, b, d, a, d, c });
        }
    }

    m_Torus = BuildMesh(std::move(verts), std::move(idx));
    return m_Torus;
}

// ─── Grid (unit grid lying on XZ for editor gizmos) ──────────────────────────
std::shared_ptr<Mesh> MeshLibrary::Grid() {
    if (m_Grid) return m_Grid;

    constexpr uint32_t SEG = 8;
    std::vector<Vertex> verts((SEG + 1) * (SEG + 1));
    std::vector<uint32_t> idx;

    for (uint32_t z = 0; z <= SEG; z++) {
        for (uint32_t x = 0; x <= SEG; x++) {
            Vertex& v = verts[z * (SEG + 1) + x];
            v.position = Vec3(static_cast<float>(x) / SEG - 0.5f, 0.0f,
                              static_cast<float>(z) / SEG - 0.5f);
            v.normal   = Vec3(0.0f, 1.0f, 0.0f);
            v.tangent  = Vec3(1.0f, 0.0f, 0.0f);
            v.uv       = Vec2(static_cast<float>(x) / SEG, static_cast<float>(z) / SEG);
        }
    }
    for (uint32_t z = 0; z < SEG; z++) {
        for (uint32_t x = 0; x < SEG; x++) {
            const uint32_t a = z * (SEG + 1) + x;
            const uint32_t b = a + 1;
            const uint32_t c = a + (SEG + 1);
            const uint32_t d = c + 1;
            idx.insert(idx.end(), { a, c, d, a, d, b });
        }
    }

    m_Grid = BuildMesh(std::move(verts), std::move(idx));
    return m_Grid;
}
// ─── Wavefront OBJ importer ─────────────────────────────────────────────────
// Supports: v, vn, vt, f (tris + quads, with or without indices),
// ignores everything else. Smooths normals when missing, dedupes by
// (position, normal, uv) triple.
std::shared_ptr<Mesh> MeshLibrary::LoadOBJ(const std::string& path) {
    if (auto it = m_LoadedFiles.find(path); it != m_LoadedFiles.end())
        return it->second;

    std::ifstream file(path);
    if (!file.is_open()) {
        GE_ERROR("Failed to open OBJ file '{}'", path);
        return nullptr;
    }

    std::vector<Vec3> positions;
    std::vector<Vec3> normals;
    std::vector<Vec2> uvs;

    struct FaceIndex {
        int32_t pos = 0;   // 1-based as parsed; 0 = missing
        int32_t uv  = 0;
        int32_t nrm = 0;
    };
    std::vector<std::vector<FaceIndex>> faces;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string token;
        ss >> token;

        if (token == "v") {
            Vec3 p;
            ss >> p.x >> p.y >> p.z;
            positions.push_back(p);
        } else if (token == "vn") {
            Vec3 n;
            ss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        } else if (token == "vt") {
            Vec2 uv;
            ss >> uv.x >> uv.y;
            uvs.push_back(uv);
        } else if (token == "f") {
            std::vector<FaceIndex> face;
            std::string raw;
            while (ss >> raw) {
                FaceIndex fi{};
                char slash1 = '\0', slash2 = '\0';
                std::istringstream rs(raw);
                rs >> fi.pos;
                if (rs.peek() == '/') {
                    rs >> slash1;
                    if (rs.peek() != '/') rs >> fi.uv;
                    if (rs.peek() == '/') { rs >> slash2; rs >> fi.nrm; }
                } else {
                    rs >> fi.uv;
                }
                face.push_back(fi);
            }
            if (!face.empty()) faces.push_back(std::move(face));
        }
    }

    GE_INFO("Loaded OBJ '{}' — {} vertices, {} faces", path, positions.size(), faces.size());

    // Generate flat normals for faces lacking indexed normals.
    std::unordered_map<uint64_t, uint32_t> vertexMap;
    std::vector<Vertex> verts;
    std::vector<uint32_t> indices;

    auto indexOf = [&](const FaceIndex& f, const Vec3& flatNormal, size_t facePos) -> uint32_t {
        // OBJ indices are 1-based; negative indices are relative.
        auto resolve = [&](int32_t idx, size_t count) -> int32_t {
            if (idx > 0) return idx - 1;
            if (idx < 0) return static_cast<int32_t>(count) + idx;
            return -1;
        };

        const int32_t p = resolve(f.pos, positions.size());
        const int32_t t = resolve(f.uv, uvs.size());
        const int32_t n = resolve(f.nrm, normals.size());

        Vec3 position = (p >= 0 && p < static_cast<int32_t>(positions.size())) ? positions[p] : Vec3(0.0f);
        Vec2 uv       = (t >= 0 && t < static_cast<int32_t>(uvs.size()))        ? uvs[t]      : Vec2(0.0f);
        Vec3 normal   = (n >= 0 && n < static_cast<int32_t>(normals.size()))    ? normals[n]   : flatNormal;

        // OBJ dedupe key: (pos, uv, normal) indices. Faces whose OBJ records do
        // NOT provide a normal index (n < 0) use the generated per-face flat
        // normal, so they must never be welded across faces — encode facePos
        // into the key for those. Faces with real per-vertex normals CAN be
        // shared, so facePos is omitted there.
        const uint64_t key = (static_cast<uint64_t>(p) & 0xFFFF) |
                             ((static_cast<uint64_t>(t) & 0xFFFF) << 16) |
                             ((static_cast<uint64_t>(n) & 0xFFFF) << 32) |
                             (static_cast<uint64_t>(n < 0 ? facePos : 0) << 48);

        if (auto it = vertexMap.find(key); it != vertexMap.end())
            return it->second;

        Vertex v;
        v.position = position;
        v.normal   = SafeNormalize(normal);
        v.tangent  = ComputeTangent(v.normal);
        v.uv       = uv;
        vertexMap[key] = static_cast<uint32_t>(verts.size());
        verts.push_back(v);
        return static_cast<uint32_t>(verts.size() - 1);
    };

    for (size_t f = 0; f < faces.size(); f++) {
        const auto& face = faces[f];
        const Vec3 a = positions[std::max(0, face[0].pos - 1)];
        const Vec3 b = positions[std::max(0, face[1].pos - 1)];
        const Vec3 c = positions[std::max(0, face[2].pos - 1)];
        const Vec3 flatNormal = SafeNormalize(glm::cross(b - a, c - a));

        if (face.size() == 4) {
            const uint32_t i0 = indexOf(face[0], flatNormal, f);
            const uint32_t i1 = indexOf(face[1], flatNormal, f);
            const uint32_t i2 = indexOf(face[2], flatNormal, f);
            const uint32_t i3 = indexOf(face[3], flatNormal, f);
            indices.insert(indices.end(), { i0, i1, i2, i0, i2, i3 });
        } else {
            for (size_t v = 0; v < face.size(); v++)
                indices.push_back(indexOf(face[v], flatNormal, f));
        }
    }

    auto mesh = BuildMesh(std::move(verts), std::move(indices));
    m_LoadedFiles[path] = mesh;
    return mesh;
}

MeshLibrary& MeshLibrary::Get() {
    static MeshLibrary instance;
    return instance;
}

} // namespace GE