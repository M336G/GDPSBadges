#include <Geode/Geode.hpp>
#include <alphalaneous.badgify/include/Badgify.hpp>
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

            queueInMainThread([id, name, description, requirements, path = utils::string::pathToString(badgeIconPath)] {
                alpha::badgify::registerBadge(
                    fmt::format("{}"_spr, id),
                    name,
                    !description.empty() ? description : "This is a <cj>custom badge</c> added by this <cl>GDPS</c>!",
                    [requirements, path](alpha::badgify::Badge const& badge) {
                        if (!badge.user)
                            return;

                        if (requirements["players"].isArray()) {
                            std::vector<matjson::Value> const players = requirements["players"].asArray().unwrap();
                            
                            auto hasPlayer = false;
                            for (matjson::Value const& player : players) {
                                if (player.isNumber() && player.asInt().unwrap() == badge.user->m_accountID) {
                                    hasPlayer = true;
                                    break;
                                }
                            }

                            if (!hasPlayer) return;
                        }

                        if (requirements["modBadge"].isString()) {
                            std::string const modBadge = requirements["modBadge"].asString().unwrapOrDefault();

                            bool isValid = false;
                            switch (badge.modStatus) {
                                case alpha::badgify::ModStatus::Leaderboard:
                                    isValid = (modBadge == "leaderboard");
                                    break;
                                case alpha::badgify::ModStatus::Elder:
                                    isValid = (modBadge == "elder");
                                    break;
                                case alpha::badgify::ModStatus::Regular:
                                    isValid = (modBadge == "regular");
                                    break;
                                default:
                                    isValid = false;
                                    break;
                            }
                            
                            if (!isValid) return;
                        }

                        if (requirements["minRank"].isNumber()) {
                            auto const minRank = requirements["minRank"].asInt().unwrap();
                            if (badge.user->m_globalRank == 0 || badge.user->m_globalRank > minRank) return;
                        }
                        if (requirements["maxRank"].isNumber()) {
                            auto const maxRank = requirements["maxRank"].asInt().unwrap();
                            if (badge.user->m_globalRank != 0 && badge.user->m_globalRank < maxRank) return;
                        }
                        
                        if (requirements["minStars"].isNumber()) {
                            auto const minStars = requirements["minStars"].asInt().unwrap();
                            if (badge.user->m_stars < minStars) return;
                        }
                        if (requirements["maxStars"].isNumber()) {
                            auto const maxStars = requirements["maxStars"].asInt().unwrap();
                            if (badge.user->m_stars > maxStars) return;
                        }

                        if (requirements["minMoons"].isNumber()) {
                            auto const minMoons = requirements["minMoons"].asInt().unwrap();
                            if (badge.user->m_moons < minMoons) return;
                        }
                        if (requirements["maxMoons"].isNumber()) {
                            auto const maxMoons = requirements["maxMoons"].asInt().unwrap();
                            if (badge.user->m_moons > maxMoons) return;
                        }

                        if (requirements["minGoldCoins"].isNumber()) {
                            auto const minGoldCoins = requirements["minGoldCoins"].asInt().unwrap();
                            if (badge.user->m_secretCoins < minGoldCoins) return;
                        }
                        if (requirements["maxGoldCoins"].isNumber()) {
                            auto const maxGoldCoins = requirements["maxGoldCoins"].asInt().unwrap();
                            if (badge.user->m_secretCoins > maxGoldCoins) return;
                        }
                        
                        if (requirements["minSilverCoins"].isNumber()) {
                            auto const minSilverCoins = requirements["minSilverCoins"].asInt().unwrap();
                            if (badge.user->m_userCoins < minSilverCoins) return;
                        }
                        if (requirements["maxSilverCoins"].isNumber()) {
                            auto const maxSilverCoins = requirements["maxSilverCoins"].asInt().unwrap();
                            if (badge.user->m_userCoins > maxSilverCoins) return;
                        }

                        if (requirements["minDemons"].isNumber()) {
                            auto const minDemons = requirements["minDemons"].asInt().unwrap();
                            if (badge.user->m_demons < minDemons) return;
                        }
                        if (requirements["maxDemons"].isNumber()) {
                            auto const maxDemons = requirements["maxDemons"].asInt().unwrap();
                            if (badge.user->m_demons > maxDemons) return;
                        }

                        alpha::badgify::showBadge(
                            badge,
                            CCSprite::create(path.c_str())
                        );
                    }
                );
            });

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