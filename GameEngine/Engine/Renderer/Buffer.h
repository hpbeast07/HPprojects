#pragma once
#include <glad/gl.h>
#include <cstdint>
#include <vector>
#include <memory>
#include <string>

namespace GE {

// ─────────────────────────────────────────────────────────────────────────────
// BufferLayout — describes how vertex data is laid out in a VBO
// ─────────────────────────────────────────────────────────────────────────────
enum class ShaderDataType : uint8_t {
    None = 0,
    Float, Float2, Float3, Float4,
    Mat3, Mat4,
    Int, Int2, Int3, Int4,
    Bool,
};

inline uint32_t ShaderDataTypeSize(ShaderDataType type) {
    switch (type) {
        case ShaderDataType::Float:  return 4;
        case ShaderDataType::Float2: return 4 * 2;
        case ShaderDataType::Float3: return 4 * 3;
        case ShaderDataType::Float4: return 4 * 4;
        case ShaderDataType::Mat3:   return 4 * 3 * 3;
        case ShaderDataType::Mat4:   return 4 * 4 * 4;
        case ShaderDataType::Int:    return 4;
        case ShaderDataType::Int2:   return 4 * 2;
        case ShaderDataType::Int3:   return 4 * 3;
        case ShaderDataType::Int4:   return 4 * 4;
        case ShaderDataType::Bool:   return 1;
        default: return 0;
    }
}

struct BufferElement {
    std::string    name;
    ShaderDataType type;
    uint32_t       size;
    uint32_t       offset;
    bool           normalized;

    BufferElement() = default;
    BufferElement(ShaderDataType type, std::string name, bool normalized = false)
        : name(std::move(name)), type(type),
          size(ShaderDataTypeSize(type)), offset(0), normalized(normalized) {}

    uint32_t GetComponentCount() const {
        switch (type) {
            case ShaderDataType::Float:  return 1;
            case ShaderDataType::Float2: return 2;
            case ShaderDataType::Float3: return 3;
            case ShaderDataType::Float4: return 4;
            case ShaderDataType::Mat3:   return 3 * 3;
            case ShaderDataType::Mat4:   return 4 * 4;
            case ShaderDataType::Int:    return 1;
            case ShaderDataType::Int2:   return 2;
            case ShaderDataType::Int3:   return 3;
            case ShaderDataType::Int4:   return 4;
            case ShaderDataType::Bool:   return 1;
            default: return 0;
        }
    }
};

class BufferLayout {
public:
    BufferLayout() = default;
    BufferLayout(std::initializer_list<BufferElement> elements) : m_Elements(elements) {
        CalculateOffsetsAndStride();
    }

    uint32_t GetStride()  const { return m_Stride; }
    const std::vector<BufferElement>& GetElements() const { return m_Elements; }
    auto begin() { return m_Elements.begin(); }
    auto end()   { return m_Elements.end(); }
    auto begin() const { return m_Elements.begin(); }
    auto end()   const { return m_Elements.end(); }

private:
    void CalculateOffsetsAndStride() {
        uint32_t offset = 0;
        m_Stride = 0;
        for (auto& e : m_Elements) {
            e.offset  = offset;
            offset   += e.size;
            m_Stride += e.size;
        }
    }
    std::vector<BufferElement> m_Elements;
    uint32_t m_Stride = 0;
};

// ─────────────────────────────────────────────────────────────────────────────
// VertexBuffer — RAII wrapper for OpenGL VBO
// ─────────────────────────────────────────────────────────────────────────────
class VertexBuffer {
public:
    explicit VertexBuffer(uint32_t size);                      // dynamic (empty)
    VertexBuffer(const void* data, uint32_t size);             // static
    ~VertexBuffer();

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

    void Bind()   const;
    void Unbind() const;
    void SetData(const void* data, uint32_t size, uint32_t offset = 0);

    const BufferLayout& GetLayout() const { return m_Layout; }
    void SetLayout(const BufferLayout& layout) { m_Layout = layout; }

    uint32_t GetID() const { return m_RendererID; }

private:
    GLuint       m_RendererID = 0;
    BufferLayout m_Layout;
};

// ─────────────────────────────────────────────────────────────────────────────
// IndexBuffer — RAII wrapper for OpenGL EBO
// ─────────────────────────────────────────────────────────────────────────────
class IndexBuffer {
public:
    IndexBuffer(const uint32_t* indices, uint32_t count);
    ~IndexBuffer();

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;

    void Bind()   const;
    void Unbind() const;
    uint32_t GetCount() const { return m_Count; }
    uint32_t GetID()    const { return m_RendererID; }

private:
    GLuint   m_RendererID = 0;
    uint32_t m_Count      = 0;
};

// ─────────────────────────────────────────────────────────────────────────────
// VertexArray — RAII wrapper for OpenGL VAO
// ─────────────────────────────────────────────────────────────────────────────
class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    void Bind()   const;
    void Unbind() const;

    void AddVertexBuffer(std::shared_ptr<VertexBuffer> vb);
    void SetIndexBuffer(std::shared_ptr<IndexBuffer>  ib);

    const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
    const std::shared_ptr<IndexBuffer>&               GetIndexBuffer()   const { return m_IndexBuffer; }
    uint32_t GetID() const { return m_RendererID; }

private:
    GLuint  m_RendererID = 0;
    uint32_t m_AttribIndex = 0;
    std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
    std::shared_ptr<IndexBuffer>               m_IndexBuffer;
};

} // namespace GE
