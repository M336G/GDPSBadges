#include <Geode/Geode.hpp>
#include "../utils/Utils.hpp"

using namespace geode::prelude;

static bool g_shownGdpsWarning = false;

#include <Geode/modify/MenuLayer.hpp>
class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;

        if (!Utils::isOnGdps() && !g_shownGdpsWarning) {
            auto *alert = FLAlertLayer::create(
                "GDPS Badges Disabled",
                "<cj>GDPS Badges</c> has been <cr>disabled</c> as you are <co>not on a GDPS</c>.",
                "OK"
            );
            alert->m_scene = this;
            alert->show();

            g_shownGdpsWarning = true;
        }

        return true;
    }
};