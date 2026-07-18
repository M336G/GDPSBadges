#pragma once

namespace Utils {
    std::string getBaseUrl();
    bool isOnGdps();

    bool isValidId(std::string_view id);

    bool badgeMeetsRequirements(GJUserScore *score, matjson::Value const& requirements);

    bool isBadgifyLoaded();
    bool isBadgesApiLoaded();
}