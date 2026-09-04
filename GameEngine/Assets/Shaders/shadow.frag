#version 450 core
// ─────────────────────────────────────────────────────────────────────────────
// shadow.frag — depth-only pass; color output is required so the color
// attachment stays defined, but only the depth is ever sampled.
// ─────────────────────────────────────────────────────────────────────────────
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0);
}