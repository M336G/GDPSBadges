# GDPS Badges
**ONLY useful on Geometry Dash Private Servers.**

> [!TIP]
> **Use [Badges API](https://geode-sdk.org/mods/jouca.badgesapi) or [Badgify](https://geode-sdk.org/mods/alphalaneous.badgify) to show more than one badge per profile!**

This mod allows Geometry Dash Private Server owners to set custom badges depending on various statistics of players using this mod (amount of stars, moons, coins obtained, demons beaten...).

## Setup (GDPS owners only)
1. On your web server's public root, create a `badges` folder
2. Inside it, create a `badges.json` file like so:
```json
[
    {
        "id": "your-badge-id-without-badge-at-the-end",
        "name": "Your Badge Name",
        "description": "An optional description for your badge",
        "requirements": {
            // ...
        }
    },
    {
        "id": "your-badge-id-without-badge-at-the-end2",
        "name": "Your Badge Name 2",
        "description": "Another optional description for your badge",
        "requirements": {
            // ...
        }
    }
]
```
3. Make sure to add textures for your badges in the same folder (for example `your-badge-id.png`)

### Customizable requirements
| Property | Description | Example |
| --- | --- | --- |
| `players` | A whitelist of account IDs that can have the badge | `"players": [ 72, 73, 74 ]` |
| `modBadge` | Add the custom badge if one of the mod badges is present. Can be either `leaderboard`, `elder` or `regular` | `"modBadge": "elder"` |
| `minRank` | Minimum global rank required to have the badge | `"minRank": 1` |
| `maxRank` | Maximum global rank allowed to have the badge | `"maxRank": 100` |
| `minStars` | Minimum number of stars required to have the badge | `"minStars": 1000` |
| `maxStars` | Maximum number of stars allowed to have the badge | `"maxStars": 5000` |
| `minMoons` | Minimum number of moons required to have the badge | `"minMoons": 500` |
| `maxMoons` | Maximum number of moons allowed to have the badge | `"maxMoons": 2000` |
| `minGoldCoins` | Minimum number of secret coins required to have the badge | `"minGoldCoins": 50` |
| `maxGoldCoins` | Maximum number of secret coins to have the badge | `"maxGoldCoins": 164` |
| `minSilverCoins` | Minimum number of user coins required to have the badge | `"minSilverCoins": 100` |
| `maxSilverCoins` | Maximum number of user coins allowed to have the badge | `"maxSilverCoins": 500` |
| `minDemons` | Minimum amount of demons beaten required to have the badge | `"minDemons": 10` |
| `maxDemons` | Maximum amount of demons beaten allowed to have the badge | `"maxDemons": 100` |

### Full example
```json
[
    {
        "id": "owner",
        "name": "Owner",
        "description": "This is the <cr>owner</c> of this GDPS!",
        "requirements": {
            "players": [ 72 ]
        }
    },
    {
        "id": "co-owner",
        "name": "Co-owner",
        "description": "This is the <co>co-owner</c> of this GDPS!",
        "requirements": {
            "players": [ 73 ]
        }
    },

    {
        "id": "demon-Grinder",
        "name": "Demon Grinder",
        "description": "This player has beaten over <cr>2,500</c> demon levels!",
        "requirements": {
            "minDemons": 2500,
            "maxDemons": 9999
        }
    }
]
```
*More information about [color tags here](https://docs.geode-sdk.org/geometrydash/tags); do note however that if you mess it up, the player's game will crash.*

**On the web server's public root:**
```
/
    /badges
        /badges.json
        /owner.png
        /co-owner.png
        /demon-grinder.png
```

## Want to help?
Feel free to open a [Pull Request](https://github.com/M336G/GDPSBadges/pulls) on GitHub!