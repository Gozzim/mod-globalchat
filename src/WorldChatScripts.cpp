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
#include "WorldChat.h"
#include "ScriptMgr.h"

class WorldChat_Config : public WorldScript
{
public: WorldChat_Config() : WorldScript("WorldChat_Config") { };

    void OnBeforeConfigLoad(bool reload) override
    {
        sWorldChat->WorldChatEnabled = sConfigMgr->GetOption<bool>("WorldChat.Enable", true);
        sWorldChat->Announce = sConfigMgr->GetOption<bool>("WorldChat.Announce", true);
        sWorldChat->ChatName = sConfigMgr->GetOption<std::string>("WorldChat.Chat.Name", "World");
        sWorldChat->ChatNameColor = sConfigMgr->GetOption<std::string>("WorldChat.Chat.NameColor", "FFFF00");
        sWorldChat->ChatTextColor = sConfigMgr->GetOption<std::string>("WorldChat.Chat.TextColor", "");
        sWorldChat->FactionSpecific = sConfigMgr->GetOption<bool>("WorldChat.FactionSpecific", false);
        sWorldChat->EnableOnLogin = sConfigMgr->GetOption<bool>("WorldChat.OnFirstLogin", true);
        sWorldChat->MinPlayTime = sConfigMgr->GetOption<uint32>("WorldChat.PlayTimeToChat", 300);
        sWorldChat->BlockProfanities = sConfigMgr->GetOption<int>("WorldChat.Profanity.Block", 0);
        sWorldChat->ProfanityMute = sConfigMgr->GetOption<uint32>("WorldChat.Profanity.MuteTime", 30);
        sWorldChat->BlockURLs = sConfigMgr->GetOption<int>("WorldChat.URL.Block", 0);
        sWorldChat->URLMute = sConfigMgr->GetOption<uint32>("WorldChat.URL.MuteTime", 120);
        sWorldChat->CoolDown = sConfigMgr->GetOption<uint32>("WorldChat.CoolDown", 2);
        sWorldChat->JoinChannelAllowed = sConfigMgr->GetOption<bool>("WorldChat.JoinChannelAllowed", false);

        std::string configColors = sConfigMgr->GetOption<std::string>("WorldChat.GM.Colors", "00FF00;091FE0;FF0000");
        sWorldChat->GMColors.clear();
        // Do not remove this
        sWorldChat->GMColors.push_back("808080");
        std::string color;
        std::istringstream colors(configColors);
        while (std::getline(colors, color, ';'))
        {
            sWorldChat->GMColors.push_back(color);
        }

        std::string configProfanity = sConfigMgr->GetOption<std::string>("WorldChat.Profanity.Blacklist", "");
        sWorldChat->ProfanityBlacklist.clear();
        std::string profanity;
        std::istringstream ProfanityPhrases(configProfanity);
        while (std::getline(ProfanityPhrases, profanity, ';'))
        {
            sWorldChat->ProfanityBlacklist.push_back(profanity);
        }

        std::string configUrl = sConfigMgr->GetOption<std::string>("WorldChat.URL.Whitelist", "");
        sWorldChat->URLWhitelist.clear();
        std::string url;
        std::istringstream urls(configUrl);
        while (std::getline(urls, url, ';'))
        {
            sWorldChat->URLWhitelist.push_back(url);
        }
    }
};

class WorldChat_Player : public PlayerScript
{
public:
    WorldChat_Player() : PlayerScript("WorldChat_Player") { }

    void OnLogin(Player* player)
    {
        if (sWorldChat->WorldChatEnabled)
        {
            if (sWorldChat->Announce)
            {
                ChatHandler(player->GetSession()).SendSysMessage("This server is running the |cff4CFF00WorldChat |rmodule. Use \".help chat\" to find out how to use it.");
            }

            if (sWorldChat->EnableOnLogin && sWorldChat->WorldChatMap.find(player->GetGUID().GetCounter()) == sWorldChat->WorldChatMap.end())
            {
                // TODO: Load from db
                sWorldChat->WorldChatMap[player->GetGUID().GetCounter()].enabled = sWorldChat->EnableOnLogin;
            }
        }
    }

    void OnChat(Player* player, uint32 /*type*/, uint32 lang, std::string& msg, Channel* channel)
    {
        if (sWorldChat->JoinChannelAllowed && sWorldChat->ChatName != "" && lang != LANG_ADDON && !strcmp(channel->GetName().c_str(), sWorldChat->ChatName.c_str()))
        {
            sWorldChat->SendWorldChat(player, msg.c_str());
            msg = -1;
        }
    }
};

void AddSC_WorldChat()
{
    new WorldChat_Config();
    new WorldChat_Player();
}
