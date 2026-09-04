#version 450 core
// ─────────────────────────────────────────────────────────────────────────────
// lit.frag — PBR (Cook-Torrance) forward fragment shader with:
//   • 1 directional light (optional PCF shadow map)
//   • up to 8 point lights
//   • optional albedo + normal maps
//   • hemispheric ambient, exponential fog, gamma correction
// ─────────────────────────────────────────────────────────────────────────────
in vec3 v_WorldPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

out vec4 FragColor;

uniform vec3 u_ViewPos;

struct MaterialParams {
    vec3  baseColor;
    float metallic;
    float roughness;
    vec3  emissiveColor;
    float emissiveIntensity;
};
uniform MaterialParams u_Material;

uniform int  u_UseAlbedo;
uniform int  u_UseNormal;
uniform sampler2D u_AlbedoTex;
uniform sampler2D u_NormalTex;

#define MAX_POINT_LIGHTS 8
struct PointLight {
    vec3  position;
    vec3  color;
    float intensity;
    float radius;
};
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform int u_PointLightCount;

uniform vec3  u_AmbientColor;
uniform vec3  u_FogColor;
uniform float u_FogDensity;

const float PI = 3.14159265359;

// ── Normal-map reconstruction via screen derivatives (no tangents needed) ───
vec3 ComputeNormal(vec3 N, vec2 uv, vec3 worldPos) {
    if (u_UseNormal == 0) return N;
    vec3 dpx  = dFdx(worldPos);
    vec3 dpy  = dFdy(worldPos);
    vec2 duvx = dFdx(uv);
    vec2 duvy = dFdy(uv);
    vec3 T    = normalize(dpx * duvy.y - dpy * duvx.y);
    vec3 B    = normalize(-dpx * duvy.x + dpy * duvx.x);
    vec3 n    = texture(u_NormalTex, uv).rgb * 2.0 - 1.0;
    return normalize(mat3(T, B, N) * n);
}

// ── BRDF helpers ─────────────────────────────────────────────────────────────
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a      = roughness * roughness;
    a            = max(a * a, 0.0001);
    float ndh    = max(dot(N, H), 0.0);
    float denom  = ndh * ndh * (a - 1.0) + 1.0;
    return a / (PI * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
struct DirectionalLight {
    vec3  direction;   // points FROM the light TOWARD the scene
    vec3  color;
    float intensity;
    int   castShadows;
};
uniform DirectionalLight u_DirLight;
uniform mat4 u_LightSpaceMat;
uniform sampler2D u_ShadowMap;
uniform vec2 u_ShadowMapSize;
uniform float u_ShadowStrength;

// ── Directional shadow (3×3 PCF, manual depth comparison) ───────────────────
float ShadowAttenuation(vec4 fragPosLightSpace, vec3 N) {
    vec3 proj = fragPosLightSpace.xyz / fragPosLightSpace.w;
    proj = proj * 0.5 + 0.5;
    if (proj.x < 0.0 || proj.x > 1.0 || proj.y < 0.0 || proj.y > 1.0 || proj.z > 1.0)
        return 1.0;
    float current = proj.z;
    float bias    = max(0.0015, 0.0003 * (1.0 - dot(N, normalize(u_DirLight.direction))));
    float texel   = 1.0 / u_ShadowMapSize.x;
    float shadow  = 0.0;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float depth = texture(u_ShadowMap, proj.xy + vec2(x, y) * texel).r;
            shadow += (current - bias > depth) ? 1.0 : 0.0;
        }
    }
    return 1.0 - (shadow / 9.0) * u_ShadowStrength;
}

void main() {
    vec3 baseColor = u_Material.baseColor;
    if (u_UseAlbedo != 0) baseColor *= texture(u_AlbedoTex, v_TexCoord).rgb;
    baseColor = max(baseColor, vec3(0.0));

    float metallic  = clamp(u_Material.metallic, 0.0, 1.0);
    float roughness = clamp(u_Material.roughness, 0.03, 1.0);

    vec3 N = normalize(v_Normal);
    N = ComputeNormal(N, v_TexCoord, v_WorldPos);
    vec3 V = normalize(u_ViewPos - v_WorldPos);

    vec3 F0 = mix(vec3(0.04), baseColor, metallic);

    vec3 Lo = vec3(0.0);

    // ── Directional light (with optional shadows) ─────────────────────────
    {
        vec3 L        = normalize(u_DirLight.direction);
        vec3 H        = normalize(V + L);
        vec3 radiance = u_DirLight.color * u_DirLight.intensity;

        float NdotL = max(dot(N, L), 0.0);
        float NDF   = DistributionGGX(N, H, roughness);
        float G     = GeometrySmith(N, V, L, roughness);
        vec3  F     = FresnelSchlick(max(dot(H, V), 0.0), F0);
        vec3  spec  = (NDF * G * F) / max(4.0 * max(dot(N, V), 0.0) * NdotL, 0.001);
        vec3  kD    = (vec3(1.0) - F) * (1.0 - metallic);

        float shadow = 1.0;
        if (u_DirLight.castShadows != 0)
            shadow = ShadowAttenuation(u_LightSpaceMat * vec4(v_WorldPos, 1.0), N);

        Lo += (kD * baseColor / PI + spec) * radiance * NdotL * shadow;
    }

    // ── Point lights ──────────────────────────────────────────────────────
    for (int i = 0; i < u_PointLightCount; i++) {
        vec3  toLight = u_PointLights[i].position - v_WorldPos;
        float dist    = length(toLight);
        vec3  L       = toLight / max(dist, 1e-4);
        float radius  = max(u_PointLights[i].radius, 0.01);

        float d     = dist / radius;
        float atten = clamp(1.0 - d * d, 0.0, 1.0);
        atten      *= atten;
        atten      /= max(1.0 + d * d, 1e-4);

        if (atten <= 0.0) continue;

        vec3  H    = normalize(V + L);
        float NdotL = max(dot(N, L), 0.0);

        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3  F   = FresnelSchlick(max(dot(H, V), 0.0), F0);
        vec3  spec = (NDF * G * F) / max(4.0 * max(dot(N, V), 0.0) * NdotL, 0.001);
        vec3  kD   = (vec3(1.0) - F) * (1.0 - metallic);

        Lo += (kD * baseColor / PI + spec) *
              u_PointLights[i].color * u_PointLights[i].intensity * atten * NdotL;
    }

    // ── Hemispheric ambient ───────────────────────────────────────────────
    vec3 ambient = u_AmbientColor * baseColor * (0.5 + 0.5 * N.y);
    ambient += u_AmbientColor * (0.5 + 0.5 * N.y) * vec3(0.15, 0.18, 0.25);

    vec3 emissive = u_Material.emissiveColor * u_Material.emissiveIntensity;

    vec3 color = ambient + Lo + emissive;

    // ── Exponential distance fog ──────────────────────────────────────────
    float dist = length(u_ViewPos - v_WorldPos);
    float fog  = 1.0 - exp(-u_FogDensity * u_FogDensity * dist * dist);
    color = mix(color, u_FogColor, clamp(fog, 0.0, 1.0));

    // ── Gamma correction ──────────────────────────────────────────────────
    color = pow(max(color, vec3(0.0)), vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}