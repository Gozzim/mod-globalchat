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

void WorldChat::LoadConfig(bool reload)
{
    WorldChatEnabled = sConfigMgr->GetOption<bool>("WorldChat.Enable", true);
    Announce = sConfigMgr->GetOption<bool>("WorldChat.Announce", true);
    ChannelName = sConfigMgr->GetOption<std::string>("WorldChat.ChannelName", "World");
    FactionSpecific = sConfigMgr->GetOption<bool>("WorldChat.FactionSpecific", false);
    EnableOnLogin = sConfigMgr->GetOption<bool>("WorldChat.OnFirstLogin", true);
    MinPlayTime = sConfigMgr->GetOption<uint32>("WorldChat.PlayTimeToChat", 300);
    SwearMute = sConfigMgr->GetOption<uint32>("WorldChat.Swearing.MuteTime", 30);
    URLMute = sConfigMgr->GetOption<uint32>("WorldChat.URL.MuteTime", 120);
    CoolDown = sConfigMgr->GetOption<uint32>("WorldChat.CoolDown", 2);
    JoinChannelAllowed = sConfigMgr->GetOption<bool>("WorldChat.JoinChannelAllowed", false);

    std::string configSwear = sConfigMgr->GetOption<std::string>("WorldChat.Swearing.Blacklist", "");
    SwearBlacklist.clear();
    std::string phrase;
    std::istringstream swearPhrases(configSwear);
    while (std::getline(swearPhrases, phrase, ';'))
    {
        SwearBlacklist.push_back(phrase);
    }

    std::string configUrl = sConfigMgr->GetOption<std::string>("WorldChat.URL.Whitelist", "");
    URLWhitelist.clear();
    std::string url;
    std::istringstream urls(configUrl);
    while (std::getline(urls, url, ';'))
    {
        URLWhitelist.push_back(url);
    }
}

bool WorldChat::HasForbiddenPhrase(std::string message)
{
    for (const auto& phrase: SwearBlacklist)
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

const char* RACE_ICON;

std::string WorldChat::GetNameLink(Player* player)
{
    std::string name = player->GetName();
    std::string color;
    std::string icon;

    switch (player->getClass())
    {
        case CLASS_DEATH_KNIGHT:
            color = "|cffC41F3B";
            icon = "|TInterface\\icons\\Spell_Deathknight_ClassIcon:12:12|t|r";
            break;
        case CLASS_DRUID:
            color = "|cffFF7D0A";
            icon = "|TInterface\\icons\\Ability_Druid_Maul:12:12|t|r";
            break;
        case CLASS_HUNTER:
            color = "|cffABD473";
            icon = "|TInterface\\icons\\INV_Weapon_Bow_07:12:12|t|r";
            break;
        case CLASS_MAGE:
            color = "|cff69CCF0";
            icon = "|TInterface\\icons\\INV_Staff_13:12:12|t|r";
            break;
        case CLASS_PALADIN:
            color = "|cffF58CBA";
            icon = "|TInterface\\icons\\INV_Hammer_01:12:12|t|r";
            break;
        case CLASS_PRIEST:
            color = "|cffFFFFFF";
            icon = "|TInterface\\icons\\INV_Staff_30:12:12|t|r";
            break;
        case CLASS_ROGUE:
            color = "|cffFFF569";
            icon = "|TInterface\\icons\\INV_ThrowingKnife_04:12:12|t|r";
            break;
        case CLASS_SHAMAN:
            color = "|cff0070DE";
            icon = "|TInterface\\icons\\Spell_Nature_BloodLust:12:12|t|r";
            break;
        case CLASS_WARLOCK:
            color = "|cff9482C9";
            icon = "|TInterface\\icons\\Spell_Nature_FaerieFire:12:12|t|r";
            break;
        case CLASS_WARRIOR:
            color = "|cffC79C6E";
            icon = "|TInterface\\icons\\INV_Sword_27.png:15|t|r";
            break;
    }
    return "|Hplayer:" + name + "|h" + color + "[" + name + "]|h|r";
}

void WorldChat::SendWorldChat(Player* player, std::string message)
{
    if (player->GetSession()->GetSecurity() == 0 && !sWorldChat->WorldChatEnabled)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000World Chat is currently disabled.|r");
        return;
    }
    if (!player->CanSpeak())
    {
        ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000You can't use the World Chat while muted.|r");
        return;
    }
    if (!sWorldChat->WorldChatMap[player->GetGUID().GetCounter()].enabled)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("|cffff0000World Chat is currently hidden. Type |r.showworld|cffff0000 to display the World Chat.|r");
        return;
    }
    if (sWorldChat->WorldChatMap[player->GetGUID().GetCounter()].last_msg + sWorldChat->CoolDown >= GameTime::GetGameTime().count() && player->GetSession()->GetSecurity() == 0)
    {
        return;
    }
    size_t stringpos;
    if (message.find("|TInterface") != std::string::npos)
        return;
    if (message.find("\n") != std::string::npos)
        return;
    if ((stringpos = message.find("|H")) != std::string::npos && (stringpos = message.find("|h")) != std::string::npos && (stringpos = message.find("|c")) != std::string::npos)
    {
        stringpos = 0;
        while ((stringpos = message.find("|h|r", stringpos)) != std::string::npos)
        {
            stringpos = message.find("|r", stringpos);
            message.replace(stringpos, 2, "|cffededed");
            stringpos += 9;
        }
    }

    if (player->GetSession()->GetSecurity() == 0)
    {
        if (HasForbiddenPhrase(message))
        {
            sWorld->SendGMText(17000, player->GetName().c_str(), message.c_str()); // send passive report to gm
            ChatHandler(player->GetSession()).PSendSysMessage("Your message contains a forbidden phrase. You have been muted for %us.", SwearMute);
            LoginDatabasePreparedStatement* mt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME); // << ?
            int64 muteTime = time(NULL) + sWorldChat->SwearMute; // muted player
            player->GetSession()->m_muteTime = muteTime; // << ?
            mt->SetData(0, muteTime); // << ?
            return;
        }

        if (HasForbiddenURL(message))
        {
            sWorld->SendGMText(17001, player->GetName().c_str(), message.c_str()); // send passive report to gm
            ChatHandler(player->GetSession()).PSendSysMessage("Urls are not allowed. You have been muted for %us.", URLMute);
            LoginDatabasePreparedStatement* mt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME); // << ?
            int64 muteTime = time(NULL) + sWorldChat->URLMute; // muted player
            player->GetSession()->m_muteTime = muteTime; // << ?
            mt->SetData(0, muteTime); // << ?
            return;
        }
    }

    std::string msg;
    std::ostringstream chat_string;
    if (player->GetTotalPlayedTime() <= sWorldChat->MinPlayTime && player->GetSession()->GetSecurity() == 0) // New If - Played Time Need For Use This Cmd
    {
        std::string adStr = secsToTimeString(sWorldChat->MinPlayTime - player->GetTotalPlayedTime());
        std::string minTime = secsToTimeString(sWorldChat->MinPlayTime);
        player->GetSession()->SendNotification("You must have played at least %s to use the World Chat. %s remaining.", minTime.c_str(), adStr.c_str());
        return;
    }

    switch (player->GetSession()->GetSecurity())
    {
        case 0:
            msg += "|cffffd500[";
            msg += sWorldChat->ChannelName;
            msg += "] ";
            msg += GetNameLink(player);
            msg += ":|cffededed";
            break;
        case 1:
            msg += "|cffffd500[";
            msg += sWorldChat->ChannelName;
            msg += "] ";
            msg += "|TINTERFACE/CHATFRAME/UI-CHATICON-BLIZZ:12:22:0:-3|t|r";
            msg += "|Hplayer:" + player->GetName() + "|h" + "|cff091fe0[" + player->GetName() + "|cff091fe0]|h|r";
            msg += ":|cffededed";
            break;
        case 2:
            msg += "|cffffd500[";
            msg += sWorldChat->ChannelName;
            msg += "] ";
            msg += "|TINTERFACE/CHATFRAME/UI-CHATICON-BLIZZ:12:22:0:-3|t|r";
            msg += "|Hplayer:" + player->GetName() + "|h" + "|cff091fe0[" + player->GetName() + "|cff091fe0]|h|r";
            msg += ":|cffededed";
            break;
        case 3:
            msg += "|cffffd500[";
            msg += sWorldChat->ChannelName;
            msg += "] ";
            msg += "|TINTERFACE/CHATFRAME/UI-CHATICON-BLIZZ:12:22:0:-3|t|r";
            msg += "|Hplayer:" + player->GetName() + "|h" + "|cff9d14ff[" + player->GetName() + "|cff9d14ff]|h|r";
            msg += ":|cffededed";
            break;
        case 4:
            msg += "|cffffd500[";
            msg += sWorldChat->ChannelName;
            msg += "] ";
            msg += "|TINTERFACE/CHATFRAME/UI-CHATICON-BLIZZ:12:22:0:-3|t|r";
            msg += "|Hplayer:" + player->GetName() + "|h" + "|cff9d14ff[" + player->GetName() + "|cff9d14ff]|h|r";
            msg += ":|cffededed";
            break;
        case 5:
            msg += "|cffffd500[";
            msg += sWorldChat->ChannelName;
            msg += "] ";
            msg += "|TINTERFACE/CHATFRAME/UI-CHATICON-BLIZZ:12:22:0:-3|t|r";
            msg += "|Hplayer:" + player->GetName() + "|h" + "|cffff0000[" + player->GetName() + "|cffff0000]|h|r";
            msg += ":|cffededed";
            break;
        default:
            break;
    }
    chat_string << msg << " " << message;

    SessionMap sessions = sWorld->GetAllSessions();

    for (SessionMap::iterator itr = sessions.begin(); itr != sessions.end(); ++itr)
    {
        if (!itr->second)
            continue;
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

        if (sWorldChat->WorldChatMap[guid2].enabled == 1)
        {
            if (!sWorldChat->FactionSpecific || (player->GetTeamId() == target->GetTeamId()))
            {
                sWorld->SendServerMessage(SERVER_MSG_STRING, chat_string.str().c_str(), target);
                sWorldChat->WorldChatMap[player->GetGUID().GetCounter()].last_msg = GameTime::GetGameTime().count();
            }
        }
    }
}
