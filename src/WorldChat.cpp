#include "WorldSession.h"
#include "Channel.h"
#include "GameTime.h"
#include "World.h"
#include "DatabaseEnv.h"
#include "Config.h"
#include "WorldChat.h"

WorldChat* WorldChat::instance()
{
    static WorldChat instance;
    return &instance;
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
        chatPrefix << "|cff";
        chatPrefix << (ChatNameColor.empty() ? "FFFF00" : ChatNameColor);
        chatPrefix << "[" << ChatName << "]|r";
    }

    return chatPrefix.str();
}

std::string WorldChat::GetNameLink(Player* player)
{
    std::string playerName = player->GetName();
    AccountTypes playerSecurity = player->GetSession()->GetSecurity();

    const char* classIcon;
    std::string color;
    std::string icons;
    std::ostringstream nameLink;

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

std::string WorldChat::BuildChatMessage(std::string prefix, std::string nameLink, std::string content)
{
    std::ostringstream chat_stream;
    std::string color = ChatTextColor.empty() ? "FFFFFF" : ChatTextColor;

    chat_stream << prefix << " " << nameLink;
    chat_stream << "|cff" << color;
    chat_stream << ": " << content;

    return chat_stream.str();
}

void WorldChat::SendWorldChat(Player* player, const char* message)
{
    if (!player)
    {
        return;
    }

    uint64 guid = player->GetGUID().GetCounter();
    const char* playerName = player->GetName().c_str();
    AccountTypes playerSecurity = player->GetSession()->GetSecurity();
    std::string chatPrefix = GetChatPrefix();
    std::string nameLink = GetNameLink(player);
    std::string chatContent = BuildChatContent(message);

    if (playerSecurity == 0 && !WorldChatEnabled)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000World Chat is currently disabled.|r");
        return;
    }

    if (!player->CanSpeak())
    {
        ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000You can't use the World Chat while muted.|r");
        return;
    }

    if (!WorldChatMap[guid].enabled)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000World Chat is currently hidden. Type |r.showworld|cffff0000 to display the World Chat.|r");
        return;
    }

    if (WorldChatMap[guid].last_msg + CoolDown >= GameTime::GetGameTime().count() && playerSecurity == 0)
    {
        return;
    }

    if (chatContent.empty())
    {
        return;
    }

    if (BlockProfanities >= 0 && playerSecurity <= BlockProfanities && HasForbiddenPhrase(message))
    {
        if (playerSecurity > 0)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Your message contains a forbidden phrase.");
            return;
        }

        if (ProfanityMute > 0)
        {
            sWorld->SendGMText(17000, playerName, message); // send report to GMs
            ChatHandler(player->GetSession()).PSendSysMessage("Your message contains a forbidden phrase. You have been muted for %us.", ProfanityMute);
            LoginDatabasePreparedStatement* mt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME); // << ?
            int64 muteTime = time(NULL) + ProfanityMute; // muted player
            player->GetSession()->m_muteTime = muteTime; // << ?
            mt->SetData(0, muteTime); // << ?
        }
        else
        {
            sWorld->SendGMText(17000, playerName, message); // send report to GMs
            ChatHandler(player->GetSession()).PSendSysMessage("Your message contains a forbidden phrase.");
        }

        return;
    }

    if (BlockURLs >= 0 && playerSecurity <= BlockURLs && HasForbiddenURL(message))
    {
        if (playerSecurity > 0)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("Urls are not allowed.");
            return;
        }

        if (URLMute > 0)
        {
            sWorld->SendGMText(17001, playerName, message); // send passive report to GMs
            ChatHandler(player->GetSession()).PSendSysMessage("Urls are not allowed. You have been muted for %us.", URLMute);
            LoginDatabasePreparedStatement* mt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME); // << ?
            int64 muteTime = time(NULL) + URLMute; // muted player
            player->GetSession()->m_muteTime = muteTime; // << ?
            mt->SetData(0, muteTime); // << ?
        }
        else
        {
            sWorld->SendGMText(17001, playerName, message); // send passive report to GMs
            ChatHandler(player->GetSession()).PSendSysMessage("Urls are not allowed.");
        }

        return;
    }

    if (player->GetTotalPlayedTime() <= MinPlayTime && player->GetSession()->GetSecurity() == 0)
    {
        std::string adStr = secsToTimeString(MinPlayTime - player->GetTotalPlayedTime());
        std::string minTime = secsToTimeString(MinPlayTime);
        player->GetSession()->SendNotification("You must have played at least %s to use the World Chat. %s remaining.", minTime.c_str(), adStr.c_str());
        return;
    }

    std::string chatMessage = BuildChatMessage(chatPrefix, nameLink, chatContent);

    SessionMap sessions = sWorld->GetAllSessions();
    for (SessionMap::iterator itr = sessions.begin(); itr != sessions.end(); ++itr)
    {
        if (!itr->second)
        {
            continue;
        }

        if (!itr->second->GetPlayer())
        {
            continue;
        }

        if (!itr->second->GetPlayer()->IsInWorld())
        {
            continue;
        }

        Player* target = itr->second->GetPlayer();
        uint64 guid2 = target->GetGUID().GetCounter();

        if (WorldChatMap[guid2].enabled == 1)
        {
            if (!FactionSpecific || (player->GetTeamId() == target->GetTeamId()))
            {
                sWorld->SendServerMessage(SERVER_MSG_STRING, chatMessage.c_str(), target);
                WorldChatMap[player->GetGUID().GetCounter()].last_msg = GameTime::GetGameTime().count();
            }
        }
    }
}
