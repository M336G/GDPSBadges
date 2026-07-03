#pragma once

namespace Utils {
    std::string getBaseUrl();
    bool isOnGdps();

    bool isValidId(std::string_view id);
}