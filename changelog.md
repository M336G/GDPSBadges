# v1.0.0
- Made the maximum cache time for icons configurable (can also be disabled completely)
- Automatically delete locally cached icons if the badge is no longer available (or the cache is disabled)
- Fixed logic issue for `minRank` & `maxRank`
- Fixed various issues in the codebase
- Replaced `badgeIconPath.string()` by `utils::string::pathToString(badgeIconPath)` to prevent issues with UTF-16 users (mostly for the russian & ukrainian userbase using the cyrillic alphabet)