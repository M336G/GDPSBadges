# v1.1.1
- Switched Alphalaneous' Badgify soft-dependency to Dasshu's Badgified
    - From now on, using Badgify to display badges will simply not work and the mod will prompt you to use Badgified instead

# v1.1.0
- Made the mod independent of any other mod due to Badgify's recent deprecation;
    - Using Badgify will still work as before
    - Using Badges API will work and allow any amount of badges to be displayed
    - Not using any of the above will only display one badge per profile

# v1.0.0
- Made the maximum cache time for icons configurable (can also be disabled completely)
- Automatically delete locally cached icons if the badge is no longer available (or the cache is disabled)
- Fixed logic issue for `minRank` & `maxRank`
- Fixed various issues in the codebase
- Replaced `badgeIconPath.string()` by `utils::string::pathToString(badgeIconPath)` to prevent issues with UTF-16 users (mostly for the russian & ukrainian userbase using the cyrillic alphabet)