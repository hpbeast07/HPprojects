#include "Paths.h"
#include <vector>

namespace GE {

std::filesystem::path ResolveAssetPath(const std::filesystem::path& relativePath) {
    if (relativePath.empty()) return relativePath;

    const std::vector<std::filesystem::path> candidates = {
        relativePath,
        std::filesystem::path("Assets") / relativePath,
        std::filesystem::path("..") / "Assets" / relativePath,
        std::filesystem::path("../..") / "Assets" / relativePath,
        std::filesystem::path("../../..") / "Assets" / relativePath,
    };

    for (const auto& candidate : candidates) {
        if (std::filesystem::exists(candidate))
            return candidate;
    }
    return std::filesystem::path("Assets") / relativePath;
}

std::filesystem::path ResolveAssetFile(const std::filesystem::path& relativePath) {
    return ResolveAssetPath(relativePath);
}

} // namespace GE