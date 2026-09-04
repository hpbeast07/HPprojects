#version 450 core
// ─────────────────────────────────────────────────────────────────────────────
// line.vert — vertex-color lines (grid, gizmos, selection boxes, debug)
// ─────────────────────────────────────────────────────────────────────────────
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;

uniform mat4 u_ViewProj;

out vec3 v_Color;

void main() {
    v_Color     = a_Color;
    gl_Position = u_ViewProj * vec4(a_Position, 1.0);
}