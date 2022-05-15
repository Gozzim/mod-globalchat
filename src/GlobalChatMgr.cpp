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
#include "GlobalChatMgr.h"
#include "WorldSession.h"

DBCStorage<ChatProfanityEntry> sChatProfanityStore(ChatProfanityEntryfmt);

GlobalChatMgr* GlobalChatMgr::instance()
{
    static GlobalChatMgr instance;
    return &instance;
}

void GlobalChatMgr::LoadConfig(bool reload)
{
    GlobalChatEnabled = sConfigMgr->GetOption<bool>("GlobalChat.Enable", true);
    Announce = sConfigMgr->GetOption<bool>("GlobalChat.Announce", true);
    ChatName = sConfigMgr->GetOption<std::string>("GlobalChat.Chat.Name", "Global");
    ChatNameColor = sConfigMgr->GetOption<std::string>("GlobalChat.Chat.NameColor", "FFFF00");
    ChatTextColor = sConfigMgr->GetOption<std::string>("GlobalChat.Chat.TextColor", "FFFFFF");
    PlayerColor = sConfigMgr->GetOption<uint32>("GlobalChat.Player.NameColor", 1);
    FactionIcon = sConfigMgr->GetOption<bool>("GlobalChat.Player.FactionIcon", false);
    RaceIcon = sConfigMgr->GetOption<bool>("GlobalChat.Player.RaceIcon", true);
    ClassIcon = sConfigMgr->GetOption<bool>("GlobalChat.Player.ClassIcon", false);
    GMBadge = sConfigMgr->GetOption<uint32>("GlobalChat.GM.Badge", 1);
    FactionSpecific = sConfigMgr->GetOption<bool>("GlobalChat.FactionSpecific", false);
    EnableOnLogin = sConfigMgr->GetOption<bool>("GlobalChat.OnFirstLogin", true);
    MinPlayTime = sConfigMgr->GetOption<uint32>("GlobalChat.PlayTimeToChat", 300);
    CoolDown = sConfigMgr->GetOption<uint32>("GlobalChat.CoolDown", 2);
    SendIgnored = sConfigMgr->GetOption<bool>("GlobalChat.SendToIgnored", false);
    JoinChannel = sConfigMgr->GetOption<bool>("GlobalChat.JoinChannelAllowed", false);
    AnnounceMutes = sConfigMgr->GetOption<bool>("GlobalChat.AnnounceMutes", false);
    ProfanityBlockType = sConfigMgr->GetOption<uint32>("GlobalChat.Profanity.BlockType", 1);
    ProfanityBlockLevel = sConfigMgr->GetOption<uint32>("GlobalChat.Profanity.BlockLevel", 0);
    ProfanityMuteType = sConfigMgr->GetOption<uint32>("GlobalChat.Profanity.MuteType", 1);
    ProfanityMute = sConfigMgr->GetOption<uint32>("GlobalChat.Profanity.MuteTime", 30);
    ProfanityFromDBC = sConfigMgr->GetOption<bool>("GlobalChat.Profanity.FromDBC", false);
    URLBlockType = sConfigMgr->GetOption<uint32>("GlobalChat.URL.BlockType", 1);
    URLBlockLevel = sConfigMgr->GetOption<uint32>("GlobalChat.URL.BlockLevel", 0);
    URLMuteType = sConfigMgr->GetOption<uint32>("GlobalChat.URL.MuteType", 1);
    URLMute = sConfigMgr->GetOption<uint32>("GlobalChat.URL.MuteTime", 120);

    // Checking Hex ColorCodes for validity
    if (!isValidHexColorCode(ChatNameColor))
    {
        LOG_ERROR("module", "GlobalChat: ChatNameColor is not a valid HexColorCode - Falling back to default");
        ChatNameColor = "FFFF00";
    }

    if (!isValidHexColorCode(ChatTextColor))
    {
        LOG_ERROR("module", "GlobalChat: ChatTextColor is not a valid HexColorCode - Falling back to default");
        ChatTextColor = "FFFFFF";
    }

    if (reload)
    {
        GMColors.clear();
        ProfanityBlacklist.clear();
        URLWhitelist.clear();
    }

    std::string configColors = sConfigMgr->GetOption<std::string>("GlobalChat.GM.Colors", "00FF00;091FE0;FF0000");
    std::string color;
    std::istringstream colors(configColors);
    while (std::getline(colors, color, ';'))
    {
        LOG_DEBUG("module", "GlobalChat: GMColor at position '{}' has length {} and value '{}' - isEmpty: {}", GMColors.size(), color.size(), color, color.empty());
        if (color.empty())
        {
            LOG_WARN("module", "GlobalChat: GMColor at position '{}' is empty - Falling back to configured PlayerNameColor", GMColors.size());
            GMColors.push_back(color);
            continue;
        }

        if (!isValidHexColorCode(color))
        {
            LOG_ERROR("module", "GlobalChat: GMColor at position '{}' is not a valid HexColorCode - Falling back to default GM Colors", GMColors.size());
            switch (GMColors.size())
            {
                case 0:
                    GMColors.push_back("00FF00");
                    break;
                case 1:
                    GMColors.push_back("091FE0");
                    break;
                default:
                    GMColors.push_back("FF0000");
                    break;
            }
            continue;
        }

        GMColors.push_back(color);
    }

    // Do not remove these lines
    GMColors.insert(GMColors.begin(), ChatNameColor);
    GMColors.push_back("808080");

    LoadBlacklistDB();
    if (ProfanityFromDBC)
        LoadProfanityDBC();

    std::string configUrl = sConfigMgr->GetOption<std::string>("GlobalChat.URL.Whitelist", "");
    std::string url;
    std::istringstream urls(configUrl);
    while (std::getline(urls, url, ';'))
    {
        URLWhitelist.push_back(url);
    }
}

void GlobalChatMgr::LoadPlayerData(Player* player)
{
    ObjectGuid guid = player->GetGUID();

    QueryResult result = CharacterDatabase.Query("SELECT enabled,last_msg,mute_time,total_mutes,banned FROM player_globalchat_status WHERE guid={};", guid.GetCounter());

    if (!result)
        return;

    if (result->GetRowCount() == 0)
        return;

    Field* fields   = result->Fetch();

    bool enabled = fields[0].Get<bool>();
    time_t lastMessage = time_t(fields[1].Get<uint32>());
    time_t muteTime = time_t(fields[2].Get<uint32>());
    uint32 totalMutes = fields[3].Get<uint32>();
    bool banned = fields[4].Get<bool>();

    playersChatData[guid].SetInChat(enabled);
    playersChatData[guid].SetLastMessage(lastMessage);
    playersChatData[guid].SetMuteTime(muteTime);
    playersChatData[guid].SetTotalMutes(totalMutes);
    playersChatData[guid].SetBanned(banned);
}

void GlobalChatMgr::SavePlayerData(Player* player)
{
    LOG_DEBUG("module", "GlobalChat: Saving PlayerData for {}", player->GetName());
    ObjectGuid guid = player->GetGUID();
    CharacterDatabase.Execute("REPLACE INTO player_globalchat_status (guid,enabled,last_msg,mute_time,total_mutes,banned) VALUES ({},{},{},{},{},{});", guid.GetCounter(), playersChatData[guid].IsInChat(), playersChatData[guid].GetLastMessage(), playersChatData[guid].GetMuteTime(), playersChatData[guid].GetTotalMutes(), playersChatData[guid].IsBanned());
}

void GlobalChatMgr::LoadBlacklistDB()
{
    QueryResult blacklist = CharacterDatabase.Query("SELECT phrase FROM `globalchat_blacklist`");

    if (!blacklist)
        return;

    do
    {
        Field* field = blacklist->Fetch();
        std::string phrase = field[0].Get<std::string>();
        ProfanityBlacklist[phrase] = std::regex{phrase, std::regex::icase | std::regex::optimize};
    } while (blacklist->NextRow());
}

void GlobalChatMgr::LoadProfanityDBC()
{
    LOG_DEBUG("module", "GlobalChat: Loading ProfanityDBC");

    uint32 availableDbcLocales = 0xFFFFFFFF;

    std::string filename = "ChatProfanity.dbc";
    std::string dbcPath = sWorld->GetDataPath() + "dbc/";
    std::string dbcFilename = dbcPath + filename;

    if (sChatProfanityStore.Load(dbcFilename.c_str()))
    {
        for (uint8 i = 0; i < TOTAL_LOCALES; ++i)
        {
            if (!(availableDbcLocales & (1 << i)))
                continue;

            std::string localizedName(dbcPath);
            localizedName.append(localeNames[i]);
            localizedName.push_back('/');
            localizedName.append(filename);

            if (!sChatProfanityStore.LoadStringsFrom(localizedName.c_str()))
                availableDbcLocales &= ~(1 << i);             // mark as not available for speedup next checks
        }
    }

    for (ChatProfanityEntry const* chatProfanity : sChatProfanityStore)
    {
        std::string text = chatProfanity->Text;

        text.erase(remove(text.begin(), text.end(), '{'), text.end());
        text.erase(remove(text.begin(), text.end(), '}'), text.end());
        text.erase(remove(text.begin(), text.end(), '\\'), text.end());
        text.erase(remove(text.begin(), text.end(), '<'), text.end());
        text.erase(remove(text.begin(), text.end(), '>'), text.end());

        ProfanityBlacklist[text] = std::regex{text, std::regex::icase | std::regex::optimize};
    }
}

bool GlobalChatMgr::IsInChat(ObjectGuid guid)
{
    return playersChatData[guid].IsInChat();
}

bool GlobalChatMgr::isValidHexColorCode(std::string color)
{
    if (color.length() != 6 )
        return false;

    for (const char& c : color)
    {
        if (!isxdigit(c))
            return false;
    }

    return true;
}

void GlobalChatMgr::Mute(ObjectGuid guid, uint32 duration)
{
    int64 muteTime = GameTime::GetGameTime().count() + duration;
    playersChatData[guid].SetMuteTime(muteTime);
    uint32 totalMutes = playersChatData[guid].GetTotalMutes();
    playersChatData[guid].SetTotalMutes(totalMutes + 1);
}

void GlobalChatMgr::Ban(ObjectGuid guid)
{
    playersChatData[guid].SetBanned(true);
    uint32 totalMutes = playersChatData[guid].GetTotalMutes();
    playersChatData[guid].SetTotalMutes(totalMutes++);
}

void GlobalChatMgr::Unmute(ObjectGuid guid)
{
    playersChatData[guid].SetBanned(false);
    playersChatData[guid].SetMuteTime(0);
}

bool GlobalChatMgr::HasForbiddenPhrase(std::string message)
{
    for (auto const& regex : ProfanityBlacklist)
        if (std::regex_search(message, regex.second))
            return true;

    return false;
}

bool GlobalChatMgr::HasForbiddenURL(std::string message)
{
    auto words_begin = std::sregex_iterator(message.begin(), message.end(), urlRegex);
    auto words_end = std::sregex_iterator();

    for (std::sregex_iterator i = words_begin; i != words_end; ++i)
    {
        std::smatch match = *i;

        if (std::find(URLWhitelist.begin(), URLWhitelist.end(), match[3].str()) != URLWhitelist.end())
            continue;

        return true;
    }

    return false;
}

std::string GlobalChatMgr::CensorForbiddenPhrase(std::string message)
{
    std::ostringstream result;
    std::smatch match;

    for (auto const& regex : ProfanityBlacklist)
    {
        if (std::regex_search(message, match, regex.second))
        {
            result << match.prefix();

            if (match.str().size() > 0)
            {
                result << std::string(match.str().size(), '*');
            }

            return result.str() + CensorForbiddenPhrase(match.suffix());
        }
    }

    return message;
}

std::string GlobalChatMgr::CensorForbiddenURL(std::string message)
{
    std::ostringstream result;
    std::smatch match;

    if (std::regex_search(message, match, urlRegex)) {
        result << match.prefix();

        if (std::find(URLWhitelist.begin(), URLWhitelist.end(), match[5].str()) != URLWhitelist.end())
        {
            result << match.str();
        }
        else
        {
            if (match[8].str().size() > 0)
                result << std::string(match[8].str().size(), '*');

            if (match[6].str().size() > 0)
            {
                result << match[1].str() << match[4].str();
                result << std::string(match[6].str().size(), '*');
                result << match[7].str();
            }
        }

        return result.str() + CensorForbiddenURL(match.suffix());
    }

    return message;
}

std::string GlobalChatMgr::GetFactionIcon(Player* player)
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

std::string GlobalChatMgr::GetFactionColor(Player* player)
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

std::string GlobalChatMgr::GetClassIcon(Player* player)
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

std::string GlobalChatMgr::GetClassColor(Player* player)
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

std::string GlobalChatMgr::GetRaceIcon(Player* player)
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

std::string GlobalChatMgr::GetChatPrefix()
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
            chatPrefix << "s .global ";
        }
        chatPrefix << "|h|cff" << (ChatNameColor.empty() ? "FFFF00" : ChatNameColor);
        chatPrefix << "[" << ChatName << "]|h";
    }

    return chatPrefix.str();
}

std::string GlobalChatMgr::GetGMChatPrefix(TeamId teamId)
{
    std::ostringstream chatPrefix;
    std::string factionColor = teamId == TEAM_ALLIANCE ? "3399FF" : "CC0000";
    std::string factionName = teamId == TEAM_ALLIANCE ? "Alliance" : "Horde";
    std::string factionCommand = teamId == TEAM_ALLIANCE ? ".galliance" : ".ghorde";

    chatPrefix << "|Hchannel:";
    chatPrefix << "s " << factionCommand << " ";
    chatPrefix << "|h|cff" << factionColor;
    chatPrefix << "[" << factionName << "]|h";
    chatPrefix << "|cff" << (ChatNameColor.empty() ? "FFFF00" : ChatNameColor);

    return chatPrefix.str();
}

std::string GlobalChatMgr::GetNameLink(Player* player)
{
    std::ostringstream nameLink;

    if (!player)
    {
        nameLink << "[|cff" << (GMColors.size() > 0 ? GMColors.back() : "808080");
        nameLink << "Console";
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
            std::string gmColor = GMColors[playerSecurity];
            if (isValidHexColorCode(gmColor))
            {
                color = gmColor;
            }
        }
    }

    nameLink << icons;
    nameLink << "[|Hplayer:" << playerName << "|h";
    nameLink << "|cff" << color << playerName << "|h";
    nameLink << "|cff" << (ChatNameColor.empty() ? "FFFF00" : ChatNameColor) << "]";

    return nameLink.str();
}

std::string GlobalChatMgr::BuildChatContent(std::string text)
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

void GlobalChatMgr::SendToPlayers(std::string chatMessage, Player* player, TeamId teamId)
{
    LOG_DEBUG("module", "GlobalChat: Sending Message to Players.");
    std::string chatPrefix = GetChatPrefix();
    std::string gmChatPrefix = GetGMChatPrefix(teamId);
    SessionMap sessions = sWorld->GetAllSessions();
    for (SessionMap::iterator itr = sessions.begin(); itr != sessions.end(); ++itr)
    {
        if (!itr->second || !itr->second->GetPlayer() || !itr->second->GetPlayer()->IsInWorld())
        {
            continue;
        }

        Player* target = itr->second->GetPlayer();
        ObjectGuid guid2 = target->GetGUID();
        std::string message;

        if (IsInChat(guid2))
        {
            if (FactionSpecific && teamId != TEAM_NEUTRAL && itr->second->GetSecurity() > 0)
            {
                message = gmChatPrefix + " " + chatMessage;
                sWorld->SendServerMessage(SERVER_MSG_STRING, message.c_str(), target);
                continue;
            }

            // Skip if receiver has sender on ignore
            if (player && !SendIgnored && target->GetSocial()->HasIgnore(player->GetGUID()) && player->GetSession()->GetSecurity() == 0)
                continue;

            if (!FactionSpecific || teamId == TEAM_NEUTRAL || teamId == target->GetTeamId())
            {
                message = chatPrefix + " " + chatMessage;
                sWorld->SendServerMessage(SERVER_MSG_STRING, message.c_str(), target);
            }
        }
    }
}

void GlobalChatMgr::SendGlobalChat(WorldSession* session, const char* message, TeamId toTeam)
{
    Player* player;

    std::string nameLink;
    std::string chatText = message;
    std::string chatContent;

    std::string chatColor = ChatTextColor.empty() ? "FFFFFF" : ChatTextColor;
    std::ostringstream chat_stream;

    if (!session)
    {
        nameLink = GetNameLink(nullptr);
        chatContent = BuildChatContent(chatText);

        chat_stream << nameLink << ": ";
        chat_stream << "|cff" << chatColor;
        chat_stream << chatContent;

        SendToPlayers(chat_stream.str(), nullptr, TEAM_NEUTRAL);
        return;
    }

    player = session->GetPlayer();
    nameLink = GetNameLink(player);

    ObjectGuid guid = player->GetGUID();
    const char* playerName = player->GetName().c_str();
    AccountTypes playerSecurity = session->GetSecurity();

    // Prevent Spamming first to avoid sending massive amounts of SysMessages as well
    if (playersChatData[guid].GetLastMessage() + CoolDown >= GameTime::GetGameTime().count() && playerSecurity == 0)
    {
        return;
    }

    if (playerSecurity == 0 && !GlobalChatEnabled)
    {
        ChatHandler(session).PSendSysMessage("|cffff0000GlobalChat is currently disabled.|r");
        return;
    }

    if (playersChatData[guid].IsBanned())
    {
        ChatHandler(session).PSendSysMessage("|cffff0000You are currently banned from the GlobalChat.|r");
        return;
    }

    if (!player->CanSpeak() || playersChatData[guid].GetMuteTime() > GameTime::GetGameTime().count())
    {
        uint32 muteLeft = session->m_muteTime - GameTime::GetGameTime().count();
        if (playersChatData[guid].GetMuteTime() > session->m_muteTime)
        {
            muteLeft = playersChatData[guid].GetMuteTime() - GameTime::GetGameTime().count();
        }

        ChatHandler(session).PSendSysMessage("|cffff0000You can't use the GlobalChat while muted.|r You need to wait another %s.", secsToTimeString(muteLeft));
        return;
    }

    if (!IsInChat(guid))
    {
        ChatHandler(session).PSendSysMessage("|cffff0000You have not joined the GlobalChat. Type |r.joinglobal|cffff0000 to join the GlobalChat.|r");
        return;
    }

    if (chatText.empty())
    {
        ChatHandler(session).PSendSysMessage("Your message cannot be empty.");
        return;
    }

    if (ProfanityBlockType > 0 && playerSecurity <= ProfanityBlockLevel && HasForbiddenPhrase(message))
    {
        if (ProfanityBlockType == 1)
        {
            chatText = CensorForbiddenPhrase(chatText);
        }

        if ((playerSecurity > 0 && ProfanityBlockType != 1) || ProfanityBlockType == 2)
        {
            ChatHandler(session).PSendSysMessage("Your message contains a forbidden phrase.");
            LOG_INFO("module", "GlobalChat: Player {} tried posting a forbidden message.", player->GetName());
            return;
        }

        if (ProfanityBlockType == 3)
        {
            if (ProfanityMute > 0)
            {
                sWorld->SendGMText(LANG_FORBIDDEN_PHRASE_ANNOUNCE_GM, playerName, message); // send report to GMs
                LOG_INFO("module", "GlobalChat: Player {} got muted for {} for posting a forbidden message.", player->GetName(), secsToTimeString(ProfanityMute));
                ChatHandler(session).PSendSysMessage("Your message contains a forbidden phrase. You have been muted for %s.", secsToTimeString(ProfanityMute));

                if (ProfanityMuteType >= 1)
                {
                    Mute(guid, ProfanityMute);
                }

                if (ProfanityMuteType >= 2)
                {
                    int64 muteTime = GameTime::GetGameTime().count() + ProfanityMute;
                    LoginDatabasePreparedStatement* mt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME);
                    session->m_muteTime = muteTime;
                    mt->SetData(0, muteTime);
                }
            }
            else
            {
                sWorld->SendGMText(LANG_FORBIDDEN_PHRASE_ANNOUNCE_GM, playerName, message); // send report to GMs
                LOG_INFO("module", "GlobalChat: Player {} tried posting a forbidden message.", player->GetName());
                ChatHandler(session).PSendSysMessage("Your message contains a forbidden phrase.");
            }

            return;
        }
    }

    if (URLBlockType > 0 && playerSecurity <= URLBlockLevel && HasForbiddenURL(message))
    {
        if (URLBlockType == 1)
        {
            chatText = CensorForbiddenURL(chatText);
        }

        if ((playerSecurity > 0 && URLBlockType != 1) || URLBlockType == 2)
        {
            ChatHandler(session).PSendSysMessage("Urls are not allowed.");
            LOG_INFO("module", "GlobalChat: Player {} tried posting a forbidden URL.", player->GetName());
            return;
        }

        if (URLBlockType == 3)
        {
            if (URLMute > 0)
            {
                sWorld->SendGMText(LANG_FORBIDDEN_URL_ANNOUNCE_GM, playerName, message); // send passive report to GMs
                LOG_INFO("module", "GlobalChat: Player {} got muted for {} for posting a forbidden URL.", player->GetName(), secsToTimeString(URLMute));
                ChatHandler(session).PSendSysMessage("Urls are not allowed. You have been muted for %s.", secsToTimeString(URLMute));

                if (URLMuteType >= 1)
                {
                    Mute(guid, URLMute);
                }

                if (URLMuteType >= 2)
                {
                    int64 muteTime = GameTime::GetGameTime().count() + URLMute;
                    LoginDatabasePreparedStatement* mt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME);
                    session->m_muteTime = muteTime;
                    mt->SetData(0, muteTime);
                }
            }
            else
            {
                sWorld->SendGMText(LANG_FORBIDDEN_URL_ANNOUNCE_GM, playerName, message); // send passive report to GMs
                LOG_INFO("module", "GlobalChat: Player {} tried posting a forbidden URL.", player->GetName());
                ChatHandler(session).PSendSysMessage("Urls are not allowed.");
                return;
            }

            return;
        }
    }

    if (player->GetTotalPlayedTime() <= MinPlayTime && session->GetSecurity() == 0)
    {
        std::string adStr = secsToTimeString(MinPlayTime - player->GetTotalPlayedTime());
        std::string minTime = secsToTimeString(MinPlayTime);
        session->SendNotification("You must have played at least %s to use the GlobalChat. %s remaining.", minTime.c_str(), adStr.c_str());
        return;
    }

    // Build Chat Content from text
    chatContent = BuildChatContent(chatText);

    // Update last message to avoid sending massive amounts of SysMessages as well
    playersChatData[guid].SetLastMessage(GameTime::GetGameTime().count());

    chat_stream << nameLink << ": ";
    chat_stream << "|cff" << chatColor;
    chat_stream << chatContent;

    LOG_INFO("module", "GlobalChat: Player {}: {}", player->GetName(), chatText);
    if (toTeam != TEAM_NEUTRAL)
        SendToPlayers(chat_stream.str(), player, toTeam);
    else
        SendToPlayers(chat_stream.str(), player, player->GetTeamId());
}

void GlobalChatMgr::PlayerJoinCommand(ChatHandler* handler)
{
    Player* player = handler->GetSession()->GetPlayer();
    ObjectGuid guid = player->GetGUID();

    if (!GlobalChatEnabled)
    {
        handler->PSendSysMessage("The GlobalChat is currently disabled.");
        return;
    }

    if (IsInChat(guid))
    {
        handler->PSendSysMessage("You already joined the GlobalChat.");
        return;
    }

    playersChatData[guid].SetInChat(true);

    handler->PSendSysMessage("You have joined the GlobalChat.");
    LOG_INFO("module", "GlobalChat: Player {} joined GlobalChat.", player->GetName());
}

void GlobalChatMgr::PlayerLeaveCommand(ChatHandler* handler)
{
    Player* player = handler->GetSession()->GetPlayer();
    ObjectGuid guid = player->GetGUID();

    if (!IsInChat(guid))
    {
        handler->PSendSysMessage("You already left the GlobalChat.");
        return;
    }

    playersChatData[guid].SetInChat(false);
    handler->PSendSysMessage("You have left the GlobalChat.");
    LOG_INFO("module", "GlobalChat: Player {} left GlobalChat.", player->GetName());
}

void GlobalChatMgr::PlayerInfoCommand(ChatHandler* handler, Player* player)
{
    ObjectGuid guid = player->GetGUID();

    bool inChat = IsInChat(guid);
    time_t lastMessage = playersChatData[guid].GetLastMessage();
    time_t muteTime = playersChatData[guid].GetMuteTime();
    uint32 totalMutes = playersChatData[guid].GetTotalMutes();
    bool isBanned = playersChatData[guid].IsBanned();

    bool isMuted = muteTime > GameTime::GetGameTime().count();
    std::string lastMsgStr = Acore::Time::TimeToTimestampStr(Seconds(lastMessage));

    handler->PSendSysMessage("GlobalChat information about player |cff4CFF00%s|r", player->GetName().c_str());
    handler->PSendSysMessage("> In Chat: %s || Last Message: %s ", inChat ? "|cff4CFF00Yes|r" : "|cffFF0000No|r", lastMessage ? ("|cff4CFF00" + lastMsgStr + "|r") : "|cffFF0000Never|r");
    handler->PSendSysMessage("> Muted: %s || Mute Time: %s", isMuted ? "|cffFF0000Yes|r" : "|cff4CFF00No|r", isMuted ? ("|cffFF0000" + secsToTimeString(muteTime - GameTime::GetGameTime().count(), true) + "|r") : "|cff4CFF000s|r");
    handler->PSendSysMessage("> Total Mutes: %s%u|r || Banned: %s", totalMutes > 0 ? "|cffFF0000" : "|cff4CFF00", totalMutes, isBanned ? "|cffFF0000Yes|r" : "|cff4CFF00No|r");
}
