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

#ifndef _WORLDCHAT_H_
#define _WORLDCHAT_H_

#include "Common.h"
#include "Chat.h"
#include "Player.h"
#include <regex>
#include <unordered_map>

typedef struct
{
    bool enabled;
    time_t last_msg;
} WorldChatVars;

class WorldChat
{
public:
    static WorldChat* instance();

    bool WorldChatEnabled;
    bool Announce;
    std::string ChatName;
    std::string ChatNameColor;
    std::string ChatTextColor;
    bool FactionSpecific;
    bool EnableOnLogin;
    uint32 MinPlayTime;
    int BlockProfanities;
    uint32 ProfanityMute;
    int BlockURLs;
    uint32 URLMute;
    uint32 CoolDown;
    bool JoinChannelAllowed;

    const std::regex urlRegex = std::regex{ "((http|ftp)s?://)?([\\w]*(?::[\\w]*)?@)?((?:(www\\.)?(?:[a-zA-Z0-9-\\.]{1,256}\\.[a-zA-Z]{2,63}))|(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?))(:\\d+)?(/[\\w\\.()-/\\\\]*)?((\\?([^#\\s&]+=[^#\\s&]+)(?:&([^#\\s&]+=[^#\\s&]+))*)?(#\\S*)?)?" };

    std::vector <std::string> GMColors;
    std::vector <std::string> ProfanityBlacklist;
    std::vector <std::string> URLWhitelist;

    std::unordered_map <uint32, WorldChatVars> WorldChatMap;

    void LoadConfig(bool reload);

    bool HasForbiddenPhrase(std::string message);
    bool HasForbiddenURL(std::string message);

    void SendWorldChat(WorldSession* session, const char* message);

private:
    std::string GetChatPrefix();
    std::string GetNameLink(Player* player);
    std::string BuildChatContent(const char* text);
    void SendToPlayers(std::string chatMessage, TeamId teamId = TEAM_NEUTRAL);
};

#define sWorldChat WorldChat::instance()

#endif
