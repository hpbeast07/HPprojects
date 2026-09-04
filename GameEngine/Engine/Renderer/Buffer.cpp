#include "Buffer.h"
#include "../Core/Assert.h"
#include "../Core/Logger.h"

namespace GE {

// ─── VertexBuffer ─────────────────────────────────────────────────────────────
VertexBuffer::VertexBuffer(uint32_t size) {
    glCreateBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

VertexBuffer::VertexBuffer(const void* data, uint32_t size) {
    glCreateBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
    if (m_RendererID) glDeleteBuffers(1, &m_RendererID);
}

void VertexBuffer::Bind()   const { glBindBuffer(GL_ARRAY_BUFFER, m_RendererID); }
void VertexBuffer::Unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

void VertexBuffer::SetData(const void* data, uint32_t size, uint32_t offset) {
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

// ─── IndexBuffer ──────────────────────────────────────────────────────────────
IndexBuffer::IndexBuffer(const uint32_t* indices, uint32_t count) : m_Count(count) {
    glCreateBuffers(1, &m_RendererID);
    // Must bind to ELEMENT_ARRAY_BUFFER here (VAO captures this binding)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() {
    if (m_RendererID) glDeleteBuffers(1, &m_RendererID);
}

void IndexBuffer::Bind()   const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID); }
void IndexBuffer::Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

// ─── VertexArray ──────────────────────────────────────────────────────────────
VertexArray::VertexArray() {
    glCreateVertexArrays(1, &m_RendererID);
}

VertexArray::~VertexArray() {
    if (m_RendererID) glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::Bind()   const { glBindVertexArray(m_RendererID); }
void VertexArray::Unbind() const { glBindVertexArray(0); }

void VertexArray::AddVertexBuffer(std::shared_ptr<VertexBuffer> vb) {
    GE_ASSERT(!vb->GetLayout().GetElements().empty(), "VertexBuffer has no layout set!");

    glBindVertexArray(m_RendererID);
    vb->Bind();

    const auto& layout = vb->GetLayout();
    for (const auto& element : layout) {
        switch (element.type) {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4: {
                glEnableVertexAttribArray(m_AttribIndex);
                glVertexAttribPointer(
                    m_AttribIndex,
                    element.GetComponentCount(),
                    GL_FLOAT,
                    element.normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    reinterpret_cast<const void*>(static_cast<uintptr_t>(element.offset)));
                m_AttribIndex++;
                break;
            }
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
            case ShaderDataType::Bool: {
                glEnableVertexAttribArray(m_AttribIndex);
                glVertexAttribIPointer(
                    m_AttribIndex,
                    element.GetComponentCount(),
                    GL_INT,
                    layout.GetStride(),
                    reinterpret_cast<const void*>(static_cast<uintptr_t>(element.offset)));
                m_AttribIndex++;
                break;
            }
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4: {
                uint32_t count = (element.type == ShaderDataType::Mat4) ? 4 : 3;
                for (uint32_t i = 0; i < count; i++) {
                    glEnableVertexAttribArray(m_AttribIndex);
                    glVertexAttribPointer(
                        m_AttribIndex, count, GL_FLOAT,
                        element.normalized ? GL_TRUE : GL_FALSE,
                        layout.GetStride(),
                        reinterpret_cast<const void*>(static_cast<uintptr_t>(
                            element.offset + sizeof(float) * count * i)));
                    glVertexAttribDivisor(m_AttribIndex, 1);
                    m_AttribIndex++;
                }
                break;
            }
            default:
                GE_ASSERT(false, "Unknown ShaderDataType!");
        }
    }
    m_VertexBuffers.push_back(vb);
}

void VertexArray::SetIndexBuffer(std::shared_ptr<IndexBuffer> ib) {
    glBindVertexArray(m_RendererID);
    ib->Bind();
    m_IndexBuffer = ib;
}

} // namespace GE
