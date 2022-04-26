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
    FactionSpecific = sConfigMgr->GetOption<bool>("WorldChat.FactionSpecific", false);
    EnableOnLogin = sConfigMgr->GetOption<bool>("WorldChat.OnFirstLogin", true);
    MinPlayTime = sConfigMgr->GetOption<uint32>("WorldChat.PlayTimeToChat", 300);
    BlockProfanities = sConfigMgr->GetOption<int>("WorldChat.Profanity.Block", 0);
    ProfanityMute = sConfigMgr->GetOption<uint32>("WorldChat.Profanity.MuteTime", 30);
    BlockURLs = sConfigMgr->GetOption<int>("WorldChat.URL.Block", 0);
    URLMute = sConfigMgr->GetOption<uint32>("WorldChat.URL.MuteTime", 120);
    CoolDown = sConfigMgr->GetOption<uint32>("WorldChat.CoolDown", 2);
    JoinChannelAllowed = sConfigMgr->GetOption<bool>("WorldChat.JoinChannelAllowed", false);

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

        if(!(std::find(URLWhitelist.begin(), URLWhitelist.end(), match[4].str()) != URLWhitelist.end()))
        {
            return true;
        }
    }

    return false;
}

std::string WorldChat::GetChatPrefix()
{
    std::ostringstream chatPrefix;

    if (!ChatName.empty())
    {
        chatPrefix << "|Hchannel:";
        if (JoinChannelAllowed)
        {
            chatPrefix << "c " << ChatName;
        }
        else
        {
            chatPrefix << "s .w ";
        }
        chatPrefix << "|h|cff" << (ChatNameColor.empty() ? "FFFF00" : ChatNameColor);
        chatPrefix << "[" << ChatName << "]|h|r";
    }

    return chatPrefix.str();
}

std::string WorldChat::GetNameLink(Player* player)
{
    std::ostringstream nameLink;

    if (!player)
    {
        nameLink << "|cff000000[Console]";
        nameLink << (ChatTextColor.empty() ? "|cffFFFFFF" : "|cff" + ChatTextColor);
        return nameLink.str();
    }

    std::string playerName = player->GetName();
    AccountTypes playerSecurity = player->GetSession()->GetSecurity();

    const char* classIcon;
    std::string color;
    std::string icons;

    switch (player->getClass())
    {
        case CLASS_DEATH_KNIGHT:
            color = "C41F3B";
            classIcon = "|TInterface\\icons\\Spell_Deathknight_ClassIcon:12:12|t|r";
            break;
        case CLASS_DRUID:
            color = "FF7D0A";
            classIcon = "|TInterface\\icons\\Ability_Druid_Maul:12:12|t|r";
            break;
        case CLASS_HUNTER:
            color = "ABD473";
            classIcon = "|TInterface\\icons\\INV_Weapon_Bow_07:12:12|t|r";
            break;
        case CLASS_MAGE:
            color = "69CCF0";
            classIcon = "|TInterface\\icons\\INV_Staff_13:12:12|t|r";
            break;
        case CLASS_PALADIN:
            color = "F58CBA";
            classIcon = "|TInterface\\icons\\INV_Hammer_01:12:12|t|r";
            break;
        case CLASS_PRIEST:
            color = "FFFFFF";
            classIcon = "|TInterface\\icons\\INV_Staff_30:12:12|t|r";
            break;
        case CLASS_ROGUE:
            color = "FFF569";
            classIcon = "|TInterface\\icons\\INV_ThrowingKnife_04:12:12|t|r";
            break;
        case CLASS_SHAMAN:
            color = "0070DE";
            classIcon = "|TInterface\\icons\\Spell_Nature_BloodLust:12:12|t|r";
            break;
        case CLASS_WARLOCK:
            color = "9482C9";
            classIcon = "|TInterface\\icons\\Spell_Nature_FaerieFire:12:12|t|r";
            break;
        case CLASS_WARRIOR:
            color = "C79C6E";
            classIcon = "|TInterface\\icons\\INV_Sword_27.png:15|t|r";
            break;
        default:
            color = GMColors[0];
            classIcon = "";
            break;
    }

    if (playerSecurity > 0)
    {
        icons = "|TINTERFACE\\CHATFRAME\\UI-CHATICON-BLIZZ:12:22:0:-3|t|r";

        if (playerSecurity < GMColors.size())
        {
            color = GMColors[playerSecurity];
        }
        else
        {
            color = GMColors[0];
        }
    }

    nameLink << icons;
    nameLink << "|Hplayer:" << playerName << "|h";
    nameLink << "|cff" << color << "[" << playerName << "]|h|r";

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
        chat_stream << chatPrefix << " " << nameLink;
        chat_stream << "|cff" << chatColor;
        chat_stream << ": " << chatContent;

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

    if (!player->CanSpeak())
    {
        ChatHandler(session).PSendSysMessage("|cffff0000You can't use the WorldChat while muted.|r");
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
            ChatHandler(session).PSendSysMessage("Your message contains a forbidden phrase. You have been muted for %us.", ProfanityMute);
            LoginDatabasePreparedStatement* mt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME); // << ?
            int64 muteTime = time(NULL) + ProfanityMute; // muted player
            session->m_muteTime = muteTime; // << ?
            mt->SetData(0, muteTime); // << ?
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
            ChatHandler(session).PSendSysMessage("Urls are not allowed. You have been muted for %us.", URLMute);
            LoginDatabasePreparedStatement* mt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME); // << ?
            int64 muteTime = time(NULL) + URLMute; // muted player
            session->m_muteTime = muteTime; // << ?
            mt->SetData(0, muteTime); // << ?
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

    chat_stream << chatPrefix << " " << nameLink;
    chat_stream << "|cff" << chatColor;
    chat_stream << ": " << chatContent;

    SendToPlayers(chat_stream.str(), player->GetTeamId());
}
