#include "SessionManager.hpp"

std::unordered_map<std::string, SessionManager::BadgeInfo> SessionManager::badges = {};
bool SessionManager::shownGdpsWarning = false;