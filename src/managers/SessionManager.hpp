#pragma once
#include <string>
#include <matjson.hpp>
#include <unordered_map>

namespace SessionManager {
    struct BadgeInfo {
        std::string name;
        std::string description;
        std::string icon;
        float scale; // 92.f (badge resolution) / <highest between width or height>
        matjson::Value requirements;
    };
    
    extern std::unordered_map<std::string, BadgeInfo> badges;
    extern bool shownGdpsWarning;
    extern bool shownBadgifyWarning;
};