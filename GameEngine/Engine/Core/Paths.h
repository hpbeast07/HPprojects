#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// Paths.h — asset path resolution and common path helpers
// ─────────────────────────────────────────────────────────────────────────────
#include <filesystem>

namespace GE {

// Resolves a project-relative asset path ("Shaders/lit.vert") to a real
// filesystem location. The engine binaries may be running from:
//   • build/bin/                      (Assets copied next to the exe)
//   • the source tree root            (Assets lives at <root>/Assets)
//   • any other working directory     (fallback candidates)
std::filesystem::path ResolveAssetPath(const std::filesystem::path& relativePath);

// Same as above but namespaced with the conventional "Assets/" prefix.
std::filesystem::path ResolveAssetFile(const std::filesystem::path& relativePath);

} // namespace GE