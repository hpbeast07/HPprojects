#version 450 core
// ─────────────────────────────────────────────────────────────────────────────
// lit.vert — PBR direct-lighting vertex shader
// ─────────────────────────────────────────────────────────────────────────────
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec2 a_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_ViewProj;
uniform mat3 u_NormalMatrix;

out vec3 v_WorldPos;
out vec3 v_Normal;
out vec2 v_TexCoord;

void main() {
    v_WorldPos = (u_Model * vec4(a_Position, 1.0)).xyz;
    v_Normal   = normalize(u_NormalMatrix * a_Normal);
    v_TexCoord = a_TexCoord;
    gl_Position = u_ViewProj * vec4(v_WorldPos, 1.0);
}