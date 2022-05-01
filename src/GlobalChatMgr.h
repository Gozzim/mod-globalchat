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
#include "GlobalChatData.h"
#include "Player.h"
#include <regex>
#include <unordered_map>

class GlobalChatData;

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
    bool JoinChannel;
    bool AnnounceMutes;
    uint32 ProfanityBlockType;
    uint32 ProfanityBlockLevel;
    uint32 ProfanityMuteType;
    uint32 ProfanityMute;
    uint32 URLBlockType;
    uint32 URLBlockLevel;
    uint32 URLMuteType;
    uint32 URLMute;

    const std::regex urlRegex = std::regex{ "((?:http|ftp)s?://)?([\\w]*(?::[\\w]*)?@)?((?:(www\\.)?(([a-zA-Z0-9-\\.]{1,256})(\\.[a-zA-Z]{2,63})))|((?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)))(:\\d+)?(/[\\w\\.()-/\\\\]*)?((\\?([^#\\s&]+=[^#\\s&]+)(?:&([^#\\s&]+=[^#\\s&]+))*)?(#\\S*)?)?" };

    std::vector <std::string> GMColors;
    std::vector <std::string> ProfanityBlacklist;
    std::vector <std::string> URLWhitelist;

    void LoadConfig(bool reload);

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

    void SendGlobalChat(WorldSession* session, const char* message);

    void PlayerJoinCommand(ChatHandler* handler);
    void PlayerLeaveCommand(ChatHandler* handler);

private:
    std::string GetChatPrefix();
    std::string GetNameLink(Player* player);
    std::string BuildChatContent(std::string text);
    void SendToPlayers(std::string chatMessage, TeamId teamId = TEAM_NEUTRAL);

    typedef std::map<ObjectGuid, GlobalChatData> GlobalChatPlayersDataMap;
    GlobalChatPlayersDataMap playersChatData;
};

#define sGlobalChatMgr GlobalChatMgr::instance()

#endif
