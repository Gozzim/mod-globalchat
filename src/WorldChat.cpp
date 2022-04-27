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

#include "Channel.h"
#include "Config.h"
#include "DatabaseEnv.h"
#include "GameTime.h"
#include "World.h"
#include "WorldChat.h"
#include "WorldSession.h"

WorldChat* WorldChat::instance()
{
    static WorldChat instance;
    return &instance;
}

void WorldChat::LoadConfig(bool reload)
{
    WorldChatEnabled = sConfigMgr->GetOption<bool>("WorldChat.Enable", true);
    Announce = sConfigMgr->GetOption<bool>("WorldChat.Announce", true);
    ChatName = sConfigMgr->GetOption<std::string>("WorldChat.Chat.Name", "World");
    ChatNameColor = sConfigMgr->GetOption<std::string>("WorldChat.Chat.NameColor", "FFFF00");
    ChatTextColor = sConfigMgr->GetOption<std::string>("WorldChat.Chat.TextColor", "");
    PlayerColor = sConfigMgr->GetOption<uint32>("WorldChat.Player.NameColor", 1);
    FactionIcon = sConfigMgr->GetOption<bool>("WorldChat.Player.FactionIcon", false);
    RaceIcon = sConfigMgr->GetOption<bool>("WorldChat.Player.RaceIcon", true);
    ClassIcon = sConfigMgr->GetOption<bool>("WorldChat.Player.ClassIcon", false);
    GMBadge = sConfigMgr->GetOption<uint32>("WorldChat.GM.Badge", 1);
    FactionSpecific = sConfigMgr->GetOption<bool>("WorldChat.FactionSpecific", false);
    EnableOnLogin = sConfigMgr->GetOption<bool>("WorldChat.OnFirstLogin", true);
    MinPlayTime = sConfigMgr->GetOption<uint32>("WorldChat.PlayTimeToChat", 300);
    CoolDown = sConfigMgr->GetOption<uint32>("WorldChat.CoolDown", 2);
    JoinChannel = sConfigMgr->GetOption<bool>("WorldChat.JoinChannelAllowed", false);
    AnnounceMutes = sConfigMgr->GetOption<bool>("WorldChat.AnnounceMutes", false);
    BlockProfanities = sConfigMgr->GetOption<int>("WorldChat.Profanity.Block", 0);
    ProfanityMuteType = sConfigMgr->GetOption<uint32>("WorldChat.Profanity.MuteType", 1);
    ProfanityMute = sConfigMgr->GetOption<uint32>("WorldChat.Profanity.MuteTime", 30);
    BlockURLs = sConfigMgr->GetOption<int>("WorldChat.URL.Block", 0);
    URLMuteType = sConfigMgr->GetOption<uint32>("WorldChat.URL.MuteType", 1);
    URLMute = sConfigMgr->GetOption<uint32>("WorldChat.URL.MuteTime", 120);

    if (reload)
    {
        GMColors.clear();
        ProfanityBlacklist.clear();
        URLWhitelist.clear();
    }

    std::string configColors = sConfigMgr->GetOption<std::string>("WorldChat.GM.Colors", "00FF00;091FE0;FF0000");
    // Do not remove this
    GMColors.push_back("808080");
    std::string color;
    std::istringstream colors(configColors);
    while (std::getline(colors, color, ';'))
    {
        GMColors.push_back(color);
    }
    // Do not remove this
    GMColors.push_back("000000");

    std::string configProfanity = sConfigMgr->GetOption<std::string>("WorldChat.Profanity.Blacklist", "");
    std::string profanity;
    std::istringstream ProfanityPhrases(configProfanity);
    while (std::getline(ProfanityPhrases, profanity, ';'))
    {
        ProfanityBlacklist.push_back(profanity);
    }

    std::string configUrl = sConfigMgr->GetOption<std::string>("WorldChat.URL.Whitelist", "");
    std::string url;
    std::istringstream urls(configUrl);
    while (std::getline(urls, url, ';'))
    {
        URLWhitelist.push_back(url);
    }
}

bool WorldChat::HasForbiddenPhrase(std::string message)
{
    for (const auto& phrase: ProfanityBlacklist)
    {
        if (message.find(phrase) != std::string::npos)
        {
            return true;
        }
    }

    return false;
}

bool WorldChat::HasForbiddenURL(std::string message)
{
    auto words_begin = std::sregex_iterator(message.begin(), message.end(), urlRegex);
    auto words_end = std::sregex_iterator();

    for (std::sregex_iterator i = words_begin; i != words_end; ++i)
    {
        std::smatch match = *i;

        if (!(std::find(URLWhitelist.begin(), URLWhitelist.end(), match[4].str()) != URLWhitelist.end()))
            return true;
    }

    return false;
}

std::string WorldChat::GetFactionIcon(Player* player)
{
    switch (player->GetTeamId())
    {
        case TEAM_ALLIANCE:
            return "|TInterface\\ICONS\\Achievement_PVP_A_A:13:13:0:-3|t";
        case TEAM_HORDE:
            return "|TInterface\\ICONS\\Achievement_PVP_H_H:13:13:0:-3|t";
        default:
            return "";
    }
}

std::string WorldChat::GetFactionColor(Player* player)
{
    switch (player->GetTeamId())
    {
        case TEAM_ALLIANCE:
            return "3399FF";
        case TEAM_HORDE:
            return "CC0000";
        default:
            return "FFFFFF";
    }
}

std::string WorldChat::GetClassIcon(Player* player)
{
    std::ostringstream icon;

    uint8 iconSize = 32;
    uint8 row = 0;
    uint8 column = 0;
    switch (player->getClass())
    {
        case CLASS_WARRIOR:
            row = 0;
            column = 0;
            break;
        case CLASS_MAGE:
            row = 0;
            column = 1;
            break;
        case CLASS_ROGUE:
            row = 0;
            column = 2;
            break;
        case CLASS_DRUID:
            row = 0;
            column = 3;
            break;
        case CLASS_HUNTER:
            row = 1;
            column = 0;
            break;
        case CLASS_SHAMAN:
            row = 1;
            column = 1;
            break;
        case CLASS_PRIEST:
            row = 1;
            column = 2;
            break;
        case CLASS_WARLOCK:
            row = 1;
            column = 3;
            break;
        case CLASS_PALADIN:
            row = 2;
            column = 0;
            break;
        case CLASS_DEATH_KNIGHT:
            row = 2;
            column = 1;
            break;
    }

    icon << "|TInterface\\Glues\\CharacterCreate\\UI-CharacterCreate-Classes:13:13:0:-3:128:128:";
    icon << column * iconSize << ":" << (column + 1) * iconSize << ":";
    icon << row * iconSize << ":" << (row + 1) * iconSize << "|t";

    return icon.str();
}

std::string WorldChat::GetClassColor(Player* player)
{
    switch (player->getClass())
    {
        case CLASS_DEATH_KNIGHT:
            return "C41F3B";
        case CLASS_DRUID:
            return "FF7D0A";
        case CLASS_HUNTER:
            return "ABD473";
        case CLASS_MAGE:
            return "69CCF0";
        case CLASS_PALADIN:
            return "F58CBA";
        case CLASS_PRIEST:
            return "FFFFFF";
        case CLASS_ROGUE:
            return "FFF569";
        case CLASS_SHAMAN:
            return "0070DE";
        case CLASS_WARLOCK:
            return "9482C9";
        case CLASS_WARRIOR:
            return "C79C6E";
        default:
            return GMColors[0];
    }
}

std::string WorldChat::GetRaceIcon(Player* player)
{
    std::ostringstream icon;

    uint8 iconSize = 32;
    uint8 row = 0;
    uint8 column = 0;

    switch (player->getRace())
    {
        case RACE_HUMAN:
            row = 0;
            column = 0;
            break;
        case RACE_DWARF:
            row = 0;
            column = 1;
            break;
        case RACE_GNOME:
            row = 0;
            column = 2;
            break;
        case RACE_NIGHTELF:
            row = 0;
            column = 3;
            break;
        case RACE_DRAENEI:
            row = 0;
            column = 4;
            break;
        case RACE_TAUREN:
            row = 1;
            column = 0;
            break;
        case RACE_UNDEAD_PLAYER:
            row = 1;
            column = 1;
            break;
        case RACE_TROLL:
            row = 1;
            column = 2;
            break;
        case RACE_ORC:
            row = 1;
            column = 3;
            break;
        case RACE_BLOODELF:
            row = 1;
            column = 4;
            break;
    }

    if (player->getGender() == GENDER_FEMALE)
    {
        row += 2;
    }

    icon << "|TInterface\\Glues\\CharacterCreate\\UI-CharacterCreate-Races:13:13:0:-3:256:128:";
    icon << column * iconSize << ":" << (column + 1) * iconSize << ":";
    icon << row * iconSize << ":" << (row + 1) * iconSize << "|t";

    return icon.str();
}

std::string WorldChat::GetChatPrefix()
{
    std::ostringstream chatPrefix;

    if (!ChatName.empty())
    {
        chatPrefix << "|Hchannel:";
        if (JoinChannel)
        {
            chatPrefix << "c " << ChatName;
        }
        else
        {
            chatPrefix << "s .world ";
        }
        chatPrefix << "|h|cff" << (ChatNameColor.empty() ? "FFFF00" : ChatNameColor);
        chatPrefix << "[" << ChatName << "]|h";
    }

    return chatPrefix.str();
}

std::string WorldChat::GetNameLink(Player* player)
{
    std::ostringstream nameLink;

    if (!player)
    {
        nameLink << "[|cff000000Console";
        nameLink << "|cff" << (ChatNameColor.empty() ? "FFFF00" : ChatNameColor) << "]";
        return nameLink.str();
    }

    std::string playerName = player->GetName();
    AccountTypes playerSecurity = player->GetSession()->GetSecurity();

    std::string color;
    std::string icons;

    if (FactionIcon)
        icons += GetFactionIcon(player);

    if (RaceIcon)
        icons += GetRaceIcon(player);

    if (ClassIcon)
        icons += GetClassIcon(player);

    switch (PlayerColor)
    {
        case 1:
            color = GetClassColor(player);
            break;
        case 2:
            color = GetFactionColor(player);
            break;
        default:
            color = GMColors[0];
            break;
    }

    if (playerSecurity > 0 && (GMBadge == 1 || (GMBadge == 2 && (player->IsDeveloper() || player->IsGameMaster())) || (GMBadge == 3 && player->isGMChat())))
    {
        icons = "|TINTERFACE\\CHATFRAME\\UI-CHATICON-BLIZZ:12:22:0:-3|t";

        if (GMColors.size() > 2 && playerSecurity < GMColors.size())
        {
            color = GMColors[playerSecurity];
        }
    }

    nameLink << icons;
    nameLink << "[|Hplayer:" << playerName << "|h";
    nameLink << "|cff" << color << playerName << "|h";
    nameLink << "|cff" << (ChatNameColor.empty() ? "FFFF00" : ChatNameColor) << "]";

    return nameLink.str();
}

std::string WorldChat::BuildChatContent(const char* text)
{
    std::string content = text;
    std::string color = ChatTextColor.empty() ? "|cffFFFFFF" : "|cff" + ChatTextColor;

    // Find and replace any resets of the color (e.g. from linking an Item) and set the color to ChatTextColor
    if (content.find("|H") != std::string::npos && content.find("|h") != std::string::npos && content.find("|cff") != std::string::npos)
    {
        size_t pos = 0;
        while ((pos = content.find("|h|r", pos)) != std::string::npos)
        {
            pos = content.find("|r", pos);
            content.replace(pos, 2, color);
            pos += 9;
        }
    }

    return content;
}

void WorldChat::SendToPlayers(std::string chatMessage, TeamId teamId)
{
    SessionMap sessions = sWorld->GetAllSessions();
    for (SessionMap::iterator itr = sessions.begin(); itr != sessions.end(); ++itr)
    {
        if (!itr->second || !itr->second->GetPlayer() || !itr->second->GetPlayer()->IsInWorld())
        {
            continue;
        }

        Player* target = itr->second->GetPlayer();
        uint64 guid2 = target->GetGUID().GetCounter();

        if (WorldChatMap[guid2].enabled == 1)
        {
            if (!FactionSpecific || teamId == TEAM_NEUTRAL || teamId == target->GetTeamId())
            {
                sWorld->SendServerMessage(SERVER_MSG_STRING, chatMessage.c_str(), target);
            }
        }
    }
}

void WorldChat::SendWorldChat(WorldSession* session, const char* message)
{
    Player* player;

    std::string nameLink;
    std::string chatPrefix = GetChatPrefix();
    std::string chatContent = BuildChatContent(message);
    std::string chatMessage;

    std::string chatColor = ChatTextColor.empty() ? "FFFFFF" : ChatTextColor;
    std::ostringstream chat_stream;

    if (!session)
    {
        nameLink = GetNameLink(nullptr);
        chat_stream << chatPrefix << " " << nameLink << ": ";
        chat_stream << "|cff" << chatColor;
        chat_stream << chatContent;

        SendToPlayers(chat_stream.str());
        return;
    }

    player = session->GetPlayer();
    nameLink = GetNameLink(player);

    uint64 guid = player->GetGUID().GetCounter();
    const char* playerName = player->GetName().c_str();
    AccountTypes playerSecurity = session->GetSecurity();

    // Prevent Spamming first to avoid sending massive amounts of SysMessages as well
    if (WorldChatMap[guid].last_msg + CoolDown >= GameTime::GetGameTime().count() && playerSecurity == 0)
    {
        return;
    }

    if (playerSecurity == 0 && !WorldChatEnabled)
    {
        ChatHandler(session).PSendSysMessage("|cffff0000WorldChat is currently disabled.|r");
        return;
    }

    if (WorldChatMap[guid].banned)
    {
        ChatHandler(session).PSendSysMessage("|cffff0000You are currently banned from the WorldChat.|r");
        return;
    }

    if (!player->CanSpeak() || WorldChatMap[guid].mute_time > GameTime::GetGameTime().count())
    {
        uint32 muteLeft = session->m_muteTime - GameTime::GetGameTime().count();
        if (WorldChatMap[guid].mute_time > session->m_muteTime)
        {
            muteLeft = WorldChatMap[guid].mute_time - GameTime::GetGameTime().count();
        }

        ChatHandler(session).PSendSysMessage("|cffff0000You can't use the WorldChat while muted.|r You need to wait another %s.", secsToTimeString(muteLeft));
        return;
    }

    if (!WorldChatMap[guid].enabled)
    {
        ChatHandler(session).PSendSysMessage("|cffff0000You have not joined the WorldChat. Type |r.joinworld|cffff0000 to join the WorldChat.|r");
        return;
    }

    if (chatContent.empty())
    {
        ChatHandler(session).PSendSysMessage("Your message cannot be empty.");
        return;
    }

    if (BlockProfanities >= 0 && playerSecurity <= BlockProfanities && HasForbiddenPhrase(message))
    {
        if (playerSecurity > 0)
        {
            ChatHandler(session).PSendSysMessage("Your message contains a forbidden phrase.");
            return;
        }

        if (ProfanityMute > 0)
        {
            sWorld->SendGMText(17000, playerName, message); // send report to GMs
            ChatHandler(session).PSendSysMessage("Your message contains a forbidden phrase. You have been muted for %s.", secsToTimeString(ProfanityMute));
            int64 muteTime = GameTime::GetGameTime().count() + ProfanityMute;

            if (ProfanityMuteType >= 1)
            {
                WorldChatMap[guid].mute_time = muteTime;
            }

            if (ProfanityMuteType >= 2)
            {
                LoginDatabasePreparedStatement* mt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME);
                session->m_muteTime = muteTime;
                mt->SetData(0, muteTime);
            }

        }
        else
        {
            sWorld->SendGMText(17000, playerName, message); // send report to GMs
            ChatHandler(session).PSendSysMessage("Your message contains a forbidden phrase.");
        }

        return;
    }

    if (BlockURLs >= 0 && playerSecurity <= BlockURLs && HasForbiddenURL(message))
    {
        if (playerSecurity > 0)
        {
            ChatHandler(session).PSendSysMessage("Urls are not allowed.");
            return;
        }

        if (URLMute > 0)
        {
            sWorld->SendGMText(17001, playerName, message); // send passive report to GMs
            ChatHandler(session).PSendSysMessage("Urls are not allowed. You have been muted for %s.", secsToTimeString(URLMute));
            int64 muteTime = GameTime::GetGameTime().count() + URLMute; // muted player

            if (URLMuteType >= 1)
            {
                WorldChatMap[guid].mute_time = muteTime;
            }

            if (URLMuteType >= 2)
            {
                LoginDatabasePreparedStatement* mt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME);
                session->m_muteTime = muteTime;
                mt->SetData(0, muteTime);
            }
        }
        else
        {
            sWorld->SendGMText(17001, playerName, message); // send passive report to GMs
            ChatHandler(session).PSendSysMessage("Urls are not allowed.");
        }

        return;
    }

    if (player->GetTotalPlayedTime() <= MinPlayTime && session->GetSecurity() == 0)
    {
        std::string adStr = secsToTimeString(MinPlayTime - player->GetTotalPlayedTime());
        std::string minTime = secsToTimeString(MinPlayTime);
        session->SendNotification("You must have played at least %s to use the WorldChat. %s remaining.", minTime.c_str(), adStr.c_str());
        return;
    }

    // Update last message to avoid sending massive amounts of SysMessages as well
    WorldChatMap[player->GetGUID().GetCounter()].last_msg = GameTime::GetGameTime().count();

    chat_stream << chatPrefix << " " << nameLink << ": ";
    chat_stream << "|cff" << chatColor;
    chat_stream << chatContent;

    SendToPlayers(chat_stream.str(), player->GetTeamId());
}
