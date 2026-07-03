#include "Utils.hpp"
#include <km7dev.server_api/include/ServerAPIEvents.hpp>

using namespace geode::prelude;

std::string Utils::getBaseUrl() {
    static std::string url = [] {
        if (Loader::get()->isModLoaded("km7dev.server_api")) {
            auto url = std::move(ServerAPIEvents::getCurrentServer().url);

            if (!url.empty() && url != "NONE_REGISTERED") {
                while (url.ends_with('/')) url = url.substr(0, url.size() - 1);
                return url;
            }
        }

        static_assert(GEODE_COMP_GD_VERSION == 22081, "Incompatible GD version for GDPS check");

        return std::string(reinterpret_cast<const char *>(base::get() +
            GEODE_WINDOWS(0x558b70)
            GEODE_ARM_MAC(0x77d709)
            GEODE_INTEL_MAC(0x868df0)
            GEODE_ANDROID64(0xeccf90)
            GEODE_ANDROID32(0x96c0db)
            GEODE_IOS(0x6b8cc2)
        ), 0, 34);
    }();
    return url;
}

bool Utils::isOnGdps() {
    static bool isActive = getBaseUrl().rfind("://www.boomlings.com/database") == std::string::npos;
    return isActive;
}

bool Utils::isValidId(std::string_view id) {
    return std::all_of(id.begin(), id.end(), [](auto character) {
        return (character >= 'a' && character <= 'z') || character == '-';
    });
}