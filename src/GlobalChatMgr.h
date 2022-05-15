/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GLOBALCHATMGR_H_
#define _GLOBALCHATMGR_H_

#include "Common.h"
#include "Chat.h"
#include "DBCFileLoader.h"
#include "DBCStores.h"
#include "GlobalChatData.h"
#include "Player.h"
#include "SocialMgr.h"
#include <regex>
#include <unordered_map>

class GlobalChatData;

struct ChatProfanityEntry
{
    //uint32      ID;
    char const* Text;
    int32       Language;
};

enum GlobalChatAcoreStrings
{
    LANG_FORBIDDEN_PHRASE_ANNOUNCE_GM = 17000, // Entry from sql acore_strings
    LANG_FORBIDDEN_URL_ANNOUNCE_GM,
    LANG_GLOBALCHAT_STATE_ANNOUNCE_WORLD,
    LANG_GLOBALCHAT_PLAYER_MUTED_ANNOUNCE_WORLD,
    LANG_GLOBALCHAT_PLAYER_BANNED_ANNOUNCE_WORLD,
    LANG_GLOBALCHAT_MUTED_ANNOUNCE_SELF,
    LANG_GLOBALCHAT_BANNED_ANNOUNCE_SELF,
};

char constexpr ChatProfanityEntryfmt[] = "dsi";

class GlobalChatMgr
{
public:
    static GlobalChatMgr* instance();

    bool GlobalChatEnabled;
    bool Announce;
    std::string ChatName;
    std::string ChatNameColor;
    std::string ChatTextColor;
    uint32 PlayerColor;
    bool FactionIcon;
    bool RaceIcon;
    bool ClassIcon;
    uint32 GMBadge;
    bool FactionSpecific;
    bool EnableOnLogin;
    uint32 MinPlayTime;
    uint32 CoolDown;
    bool SendIgnored;
    bool JoinChannel;
    bool AnnounceMutes;
    uint32 ProfanityBlockType;
    uint32 ProfanityBlockLevel;
    uint32 ProfanityMuteType;
    uint32 ProfanityMute;
    bool ProfanityFromDBC;
    uint32 URLBlockType;
    uint32 URLBlockLevel;
    uint32 URLMuteType;
    uint32 URLMute;

    const std::regex urlRegex = std::regex{ "((?:http|ftp)s?://)?([\\w]*(?::[\\w]*)?@)?((?:(www\\.)?(([a-zA-Z0-9-\\.]{1,256})(\\.[a-zA-Z]{2,63})))|((?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)))(:\\d+)?(/[\\w\\.()-/\\\\]*)?((\\?([^#\\s&]+=[^#\\s&]+)(?:&([^#\\s&]+=[^#\\s&]+))*)?(#\\S*)?)?" };

    typedef std::unordered_map<std::string, std::regex> ProfanityRegexMap;
    ProfanityRegexMap ProfanityBlacklist;
    std::vector <std::string> GMColors;
    std::vector <std::string> URLWhitelist;

    void LoadConfig(bool reload);
    void LoadPlayerData(Player* player);
    void SavePlayerData(Player* player);
    void LoadBlacklistDB();
    void LoadProfanityDBC();

    bool IsInChat(ObjectGuid guid);
    void Mute(ObjectGuid guid, uint32 duration);
    void Unmute(ObjectGuid guid);
    void Ban(ObjectGuid guid);

    bool HasForbiddenPhrase(std::string message);
    bool HasForbiddenURL(std::string message);
    std::string CensorForbiddenPhrase(std::string message);
    std::string CensorForbiddenURL(std::string message);

    std::string GetFactionIcon(Player* player);
    std::string GetFactionColor(Player* player);
    std::string GetClassIcon(Player* player);
    std::string GetClassColor(Player* player);
    std::string GetRaceIcon(Player* player);

    void SendGlobalChat(WorldSession* session, const char* message, TeamId toTeam = TEAM_NEUTRAL);

    void PlayerJoinCommand(ChatHandler* handler);
    void PlayerLeaveCommand(ChatHandler* handler);
    void PlayerInfoCommand(ChatHandler* handler, Player* player);

private:
    bool isValidHexColorCode(std::string color);
    std::string GetChatPrefix();
    std::string GetGMChatPrefix(TeamId teamId);
    std::string GetNameLink(Player* player);
    std::string BuildChatContent(std::string text);
    void SendToPlayers(std::string chatMessage, Player* player, TeamId teamId);

    typedef std::map<ObjectGuid, GlobalChatData> GlobalChatPlayersDataMap;
    GlobalChatPlayersDataMap playersChatData;
};

#define sGlobalChatMgr GlobalChatMgr::instance()

#endif
