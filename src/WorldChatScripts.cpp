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

#include "ChannelMgr.h"
#include "WorldChat.h"
#include "ScriptMgr.h"

class WorldChat_Config : public WorldScript
{
public: WorldChat_Config() : WorldScript("WorldChat_Config") { };

    void OnAfterConfigLoad(bool reload) override
    {
        sWorldChat->LoadConfig(reload);
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
        if (sWorldChat->JoinChannel && sWorldChat->ChatName != "" && lang != LANG_ADDON && !strcmp(channel->GetName().c_str(), sWorldChat->ChatName.c_str()))
        {
            sWorldChat->SendWorldChat(player->GetSession(), msg.c_str());
            msg = -1;
        }
    }
};

void AddSC_WorldChat()
{
    new WorldChat_Config();
    new WorldChat_Player();
}
