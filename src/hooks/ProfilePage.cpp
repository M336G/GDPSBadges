#include "../managers/SessionManager.hpp"
#include "../utils/Utils.hpp"

using namespace geode::prelude;

#include <Geode/modify/ProfilePage.hpp>
class $modify(GDPSBadgesProfilePage, ProfilePage) {
    struct Fields {
        std::vector<std::string> m_badgeKeys;
    };

    void loadPageFromUserInfo(GJUserScore *score) {
        ProfilePage::loadPageFromUserInfo(score);

        if (Utils::isBadgifiedLoaded() || Utils::isBadgifyLoaded()) return;

        auto *usernameMenu = static_cast<CCMenu *>(m_mainLayer->getChildByIDRecursive("username-menu"));
        if (!usernameMenu) return;

        if (static_cast<CCSprite *>(usernameMenu->getChildByIDRecursive("mod-badge"))) {
            if (!Utils::isBadgesApiLoaded()) return;
        }

        m_fields->m_badgeKeys.clear();
        int badgesAdded = 0;

        for (auto const& [id, info] : SessionManager::badges) {
            if (!Utils::isBadgesApiLoaded() && badgesAdded >= 1) break;
            if (!Utils::badgeMeetsRequirements(score, info.requirements)) continue;
            
            auto badgeId = fmt::format("{}"_spr, id);
            if (auto *badge = static_cast<CCMenuItemSpriteExtra *>(usernameMenu->getChildByIDRecursive(badgeId))) {
                badge->removeFromParentAndCleanup(true);
            }

            auto *sprite = CCSprite::create(info.icon.c_str());
            if (info.scale != 1.f) sprite->setScale(info.scale);

            auto *badge = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(GDPSBadgesProfilePage::onGdpsBadge));
            badge->setID(badgeId);

            auto tag = m_fields->m_badgeKeys.size();
            m_fields->m_badgeKeys.push_back(id);
            badge->setTag(tag);

            usernameMenu->addChild(badge);
            badgesAdded++;
        }

        usernameMenu->updateLayout();
    }

    void onGdpsBadge(CCObject *sender) {
        int tag = sender->getTag();
        if (tag < 0 || tag >= m_fields->m_badgeKeys.size()) return;

        std::string const& id = m_fields->m_badgeKeys[tag];
        auto info = SessionManager::badges.at(id);

        FLAlertLayer::create(info.name.c_str(), info.description.c_str(), "OK")->show();
    }
};