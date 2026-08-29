#include <Geode/Geode.hpp>

#include "../managers/SessionManager.hpp"
#include "../utils/Utils.hpp"

using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;

        if (!Utils::isOnGdps() && !SessionManager::shownGdpsWarning) {
            auto *alert = FLAlertLayer::create(
                "GDPS Badges Disabled",
                "<cj>GDPS Badges</c> has been <cr>disabled</c> as you are <co>not on a GDPS</c>.",
                "OK"
            );
            alert->m_scene = this;
            alert->show();

            SessionManager::shownGdpsWarning = true;
        }

        if (Utils::isBadgifyLoaded() && !SessionManager::shownBadgifyWarning) {
            Notification::create(
                "GDPS Badges will not work with Badgify. Use Badgified instead",
                NotificationIcon::Warning,
                3.f
            )->show();

            SessionManager::shownBadgifyWarning = true;
        }

        return true;
    }
};