#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// Math.h — GLM type aliases and engine-wide math utilities
// ─────────────────────────────────────────────────────────────────────────────
#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>

namespace GE {

// ── Scalar aliases ────────────────────────────────────────────────────────────
using f32  = float;
using f64  = double;
using i8   = int8_t;
using i16  = int16_t;
using i32  = int32_t;
using i64  = int64_t;
using u8   = uint8_t;
using u16  = uint16_t;
using u32  = uint32_t;
using u64  = uint64_t;

// ── Vector aliases ────────────────────────────────────────────────────────────
using Vec2  = glm::vec2;
using Vec3  = glm::vec3;
using Vec4  = glm::vec4;
using IVec2 = glm::ivec2;
using IVec3 = glm::ivec3;
using IVec4 = glm::ivec4;
using UVec2 = glm::uvec2;
using UVec3 = glm::uvec3;
using UVec4 = glm::uvec4;

// ── Matrix aliases ────────────────────────────────────────────────────────────
using Mat2  = glm::mat2;
using Mat3  = glm::mat3;
using Mat4  = glm::mat4;

// ── Quaternion alias ──────────────────────────────────────────────────────────
using Quat  = glm::quat;

// ── Constants ─────────────────────────────────────────────────────────────────
inline constexpr f32 PI      = glm::pi<f32>();
inline constexpr f32 TWO_PI  = 2.0f * PI;
inline constexpr f32 HALF_PI = 0.5f * PI;
inline constexpr f32 DEG2RAD = PI / 180.0f;
inline constexpr f32 RAD2DEG = 180.0f / PI;
inline constexpr f32 EPSILON = 1e-6f;

// ── Utility functions ─────────────────────────────────────────────────────────
inline f32  Radians(f32 deg) { return glm::radians(deg); }
inline f32  Degrees(f32 rad) { return glm::degrees(rad); }
inline f32  Lerp(f32 a, f32 b, f32 t) { return glm::mix(a, b, t); }
inline Vec3 Lerp(const Vec3& a, const Vec3& b, f32 t) { return glm::mix(a, b, t); }
inline Quat Slerp(const Quat& a, const Quat& b, f32 t) { return glm::slerp(a, b, t); }
inline f32  Clamp(f32 v, f32 lo, f32 hi) { return glm::clamp(v, lo, hi); }
inline f32  Saturate(f32 v) { return Clamp(v, 0.0f, 1.0f); }

// ── Transform helpers ─────────────────────────────────────────────────────────
inline Mat4 Translate(const Vec3& pos) {
    return glm::translate(Mat4(1.0f), pos);
}
inline Mat4 Rotate(f32 angleDeg, const Vec3& axis) {
    return glm::rotate(Mat4(1.0f), Radians(angleDeg), axis);
}
inline Mat4 Scale(const Vec3& s) {
    return glm::scale(Mat4(1.0f), s);
}
inline Mat4 TRS(const Vec3& pos, const Quat& rot, const Vec3& scl) {
    return Translate(pos) * glm::toMat4(rot) * Scale(scl);
}

// ── Frustum AABB/sphere checks (stubs for Phase 2) ───────────────────────────
struct AABB {
    Vec3 min{-0.5f}, max{0.5f};
    Vec3 Center() const { return (min + max) * 0.5f; }
    Vec3 Extents() const { return (max - min) * 0.5f; }
};

} // namespace GE
