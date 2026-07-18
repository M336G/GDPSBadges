#include <Geode/Geode.hpp>
#include <alphalaneous.badgify/include/Badgify.hpp>

#include "managers/SessionManager.hpp"
#include "utils/Utils.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
    std::filesystem::path const badgesSavePath = Mod::get()->getSaveDir() / "badges";
    std::filesystem::create_directories(badgesSavePath);

    std::chrono::hours const maxCacheTime = std::chrono::hours(Mod::get()->getSettingValue<int64_t>("max-cache-time"));

    if (!Utils::isOnGdps()) {
        log::warn("Mod disabled as it is not used on a GDPS");
        return;
    }

    async::spawn([badgesSavePath, maxCacheTime] -> arc::Future<> {
        auto res = co_await web::WebRequest().get(Utils::getBaseUrl() + "/badges/badges.json");

        auto const bodyRaw = res.json();
        if (!res.ok() || !bodyRaw.isOk()) {
            log::error("Failed loading badges: {}", res.errorMessage());
            co_return;
        }

        matjson::Value const& body = bodyRaw.unwrap();
        if (!body.isArray()) {
            log::error("Failed loading badges: badges.json is not an array");
            co_return;
        }

        std::vector<matjson::Value> const& badges = body.asArray().unwrap();
        if (badges.empty()) {
            log::error("Failed loading badges: none in badges.json");
            co_return;
        }

        log::info("Loading {} badges...", badges.size());

        int index = 0;
        for (matjson::Value const& badge : badges) {
            if (!badge["id"].isString() || !badge["name"].isString()) {
                log::error("Skipping badge unknown: invalid properties");
                continue;
            }

            std::string const id = badge["id"].asString().unwrapOrDefault();
            std::string const name = badge["name"].asString().unwrapOrDefault();
            std::string const description = badge["description"].asString().unwrapOrDefault();
            matjson::Value const requirements = badge["requirements"];

            // Also prevent path traversal LOL
            if (id.empty() || name.empty() || !Utils::isValidId(id)) {
                log::error("Skipping badge {}: empty or invalid properties", id);
                continue;
            }

            auto badgeIconPath = badgesSavePath / (id + ".png");
            if (!std::filesystem::exists(badgeIconPath) || maxCacheTime.count() == 0 || std::chrono::file_clock::now() - std::filesystem::last_write_time(badgeIconPath) > maxCacheTime) {
                auto res = co_await web::WebRequest().get(fmt::format("{}/badges/{}.png", Utils::getBaseUrl(), id));
                if (!res.ok()) {
                    log::error("Skipping {}: {}", id, res.errorMessage());
                    continue;
                }

                auto writeRes = utils::file::writeBinary(badgeIconPath, res.data());
                if (!writeRes.isOk()) {
                    log::error("Skipping {}: {}", id, writeRes.unwrapErr());
                    continue;
                }

                log::info("Downloaded & stored {} badge's texture", id);
            }

            auto const iconPath = utils::string::pathToString(badgeIconPath);

            if (Utils::isBadgifyLoaded()) {
                queueInMainThread([id, name, description, requirements, iconPath] {
                    alpha::badgify::registerBadge(
                        fmt::format("{}"_spr, id),
                        name,
                        !description.empty() ? description : "This is a <cj>custom badge</c> added by this <cl>GDPS</c>!",
                        [requirements, iconPath](alpha::badgify::Badge const& badge) {
                            if (!Utils::badgeMeetsRequirements(badge.user, requirements))
                                return;

                            alpha::badgify::showBadge(
                                badge,
                                CCSprite::create(iconPath.c_str())
                            );
                        }
                    );
                });
            } else {
                float scale = 1.f;

                auto image = new CCImage();
                if (image->initWithImageFile(iconPath.c_str())) {
                    int width = image->getWidth();
                    int height = image->getHeight();

                    if (width > 0 && height > 0)
                        scale = 92.f / std::max(width, height);
                }
                image->release();

                SessionManager::badges[Utils::isBadgesApiLoaded() ? fmt::format("{}-badge:{}", id, index + 1) : fmt::format("{}-badge", id)] = {
                    name,
                    !description.empty() ? description : "This is a <cj>custom badge</c> added by this <cl>GDPS</c>!",
                    iconPath,
                    scale,
                    requirements
                };
            }

            index++;
            log::info("Loaded {} badge", id);
        }

        log::info("Finished loading badges!");

        if (maxCacheTime.count() > 0) {
            // Delete cached badges' icon that aren't in the badges list
            for (std::filesystem::directory_entry const& file : std::filesystem::directory_iterator(badgesSavePath)) {
                std::filesystem::path const badgePath = file.path();
                std::string const badgeId = badgePath.stem().string();

                bool badgeExists = false;
                for (matjson::Value const& badge : badges) {
                    if (!badge["id"].isString())
                        continue;

                    if (badge["id"].asString().unwrap() == badgeId) {
                        badgeExists = true;
                        break;
                    }
                }

                if (!badgeExists)
                    std::filesystem::remove(badgePath);
            }
        } else {
            // If the cache is disabled then just delete the badges folder just in
            // case anything was there previously
            std::filesystem::remove_all(badgesSavePath);
        }
    });
}