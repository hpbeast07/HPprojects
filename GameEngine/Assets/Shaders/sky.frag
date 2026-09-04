#version 450 core
// ─────────────────────────────────────────────────────────────────────────────
// sky.frag — procedural gradient sky dome with a sun disc
// ─────────────────────────────────────────────────────────────────────────────
in vec2 v_TexCoord;
out vec4 FragColor;

uniform mat4 u_InvViewProj;
uniform vec3 u_SunDirection;
uniform vec3 u_HorizonColor;
uniform vec3 u_ZenithColor;
uniform vec3 u_GroundColor;

void main() {
    vec4 worldPos = u_InvViewProj * vec4(v_TexCoord * 2.0 - 1.0, 1.0, 1.0);
    vec3 dir      = normalize(worldPos.xyz / worldPos.w);

    // Vertical gradient: ground → horizon → zenith
    vec3 color;
    if (dir.y < 0.0) {
        color = mix(u_HorizonColor, u_GroundColor, clamp(-dir.y * 4.0, 0.0, 1.0));
    } else {
        float t = pow(dir.y, 0.6);
        color   = mix(u_HorizonColor, u_ZenithColor, t);
    }

    // Sun + soft halo
    vec3  sunDir = normalize(u_SunDirection);
    float sun    = pow(max(dot(dir, sunDir), 0.0), 384.0);
    float halo   = pow(max(dot(dir, sunDir), 0.0), 12.0) * 0.30;
    color += u_ZenithColor * 1.6 * sun + u_HorizonColor * halo;

    FragColor = vec4(color, 1.0);
}