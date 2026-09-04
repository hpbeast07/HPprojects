#version 450 core
// ─────────────────────────────────────────────────────────────────────────────
// shadow.vert — depth-only pass for directional shadow mapping
// ─────────────────────────────────────────────────────────────────────────────
layout(location = 0) in vec3 a_Position;

uniform mat4 u_Model;
uniform mat4 u_LightSpaceMat;

void main() {
    gl_Position = u_LightSpaceMat * u_Model * vec4(a_Position, 1.0);
}