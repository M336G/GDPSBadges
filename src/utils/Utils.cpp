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

bool Utils::badgeMeetsRequirements(GJUserScore *score, matjson::Value const& requirements) {
    if (!score) return false;

    if (requirements.contains("players") && requirements["players"].isArray()) {
        bool hasPlayer = false;
        for (auto& player : requirements["players"].asArray().unwrap()) {
            if (player.isNumber() && player.asInt().unwrap() == score->m_accountID) {
                hasPlayer = true;
                break;
            }
        }

        if (!hasPlayer) return false;
    }

    if (requirements["modBadge"].isString()) {
        std::string modBadge = requirements["modBadge"].asString().unwrap();
        bool valid = false;

        if (modBadge == "regular") valid = (score->m_modBadge == 1);
        else if (modBadge == "elder") valid = (score->m_modBadge == 2);
        else if (modBadge == "leaderboard") valid = (score->m_modBadge == 3);

        if (!valid) return false;
    }

    if (requirements["minRank"].isNumber()) {
        int rank = requirements["minRank"].asInt().unwrap();
        if (score->m_globalRank == 0 || score->m_globalRank > rank) return false;
    }
    if (requirements["maxRank"].isNumber()) {
        int rank = requirements["maxRank"].asInt().unwrap();
        if (score->m_globalRank != 0 && score->m_globalRank < rank) return false;
    }

    if (requirements["minStars"].isNumber() && score->m_stars < requirements["minStars"].asInt().unwrap()) return false;
    if (requirements["maxStars"].isNumber() && score->m_stars > requirements["maxStars"].asInt().unwrap()) return false;

    if (requirements["minMoons"].isNumber() && score->m_moons < requirements["minMoons"].asInt().unwrap()) return false;
    if (requirements["maxMoons"].isNumber() && score->m_moons > requirements["maxMoons"].asInt().unwrap()) return false;

    if (requirements["minGoldCoins"].isNumber() && score->m_secretCoins < requirements["minGoldCoins"].asInt().unwrap()) return false;
    if (requirements["maxGoldCoins"].isNumber() && score->m_secretCoins > requirements["maxGoldCoins"].asInt().unwrap()) return false;

    if (requirements["minSilverCoins"].isNumber() && score->m_userCoins < requirements["minSilverCoins"].asInt().unwrap()) return false;
    if (requirements["maxSilverCoins"].isNumber() && score->m_userCoins > requirements["maxSilverCoins"].asInt().unwrap()) return false;

    if (requirements["minDemons"].isNumber() && score->m_demons < requirements["minDemons"].asInt().unwrap()) return false;
    if (requirements["maxDemons"].isNumber() && score->m_demons > requirements["maxDemons"].asInt().unwrap()) return false;

    return true;
}

bool Utils::isBadgifyLoaded() {
    static bool isBadgifyLoaded = Loader::get()->isModLoaded("alphalaneous.badgify");
    return isBadgifyLoaded;
}

bool Utils::isBadgifiedLoaded() {
    static bool isBadgifiedLoaded = Loader::get()->isModLoaded("dasshu.badgified");
    return isBadgifiedLoaded;
}

bool Utils::isBadgesApiLoaded() {
    static bool isBadgesApiLoaded = Loader::get()->isModLoaded("jouca.badgesapi");
    return isBadgesApiLoaded;
}