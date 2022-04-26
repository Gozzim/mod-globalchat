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

                /*
                if (sWorldChat->JoinChannel && sWorldChat->ChatName != "")
                {
                    ChannelMgr* cMgr;
                    if (sWorldChat->FactionSpecific)
                    {
                        cMgr = ChannelMgr::forTeam(player->GetTeamId());
                    }
                    else
                    {
                        cMgr = ChannelMgr::forTeam(TEAM_ALLIANCE);
                    }

                    cMgr->GetJoinChannel(sWorldChat->ChatName, 0);

                    Channel* channel = cMgr->GetJoinChannel(sWorldChat->ChatName, 0);
                    if (!channel)
                        LOG_DEBUG("module", "WorldChat::NoChannel");
                    channel->JoinChannel(player, channel->GetPassword());
                }
                 */
            }

            //ChannelMgr* mgr = ChannelMgr::forTeam(player->GetTeamId());
            //mgr->GetJoinChannel(sWorldChat->ChatName, 0);
        }
    }

    void OnChat(Player* player, uint32 /*type*/, uint32 lang, std::string& msg, Channel* channel)
    {
        if (sWorldChat->JoinChannel && sWorldChat->ChatName != "" && lang != LANG_ADDON && !strcmp(channel->GetName().c_str(), sWorldChat->ChatName.c_str()))
        {
            /*
            const ChannelRights& channelRights = ChannelMgr::GetChannelRightsFor(channel->GetName());
            if (!channelRights.flags & CHANNEL_RIGHT_NO_OWNERSHIP)
                channelRights.flags |= CHANNEL_RIGHT_NO_OWNERSHIP

            if (!channelRights.flags & CHANNEL_RIGHT_FORCE_NO_ANNOUNCEMENTS)
                channelRights.flags |= CHANNEL_RIGHT_FORCE_NO_ANNOUNCEMENTS

            if (!channelRights.flags & CHANNEL_RIGHT_CANT_CHANGE_PASSWORD)
                channelRights.flags |= CHANNEL_RIGHT_CANT_CHANGE_PASSWORD

            if (!channelRights.flags & CHANNEL_RIGHT_CANT_BAN)
                channelRights.flags |= CHANNEL_RIGHT_CANT_BAN

            if (!channelRights.flags & CHANNEL_RIGHT_CANT_KICK)
                channelRights.flags |= CHANNEL_RIGHT_CANT_KICK

            if (!channelRights.flags & CHANNEL_RIGHT_CANT_MUTE)
                channelRights.flags |= CHANNEL_RIGHT_CANT_MUTE
                */
            /*
            const std::set<uint32> mods;
            uint32 newFlags = 0;
            newFlags |= CHANNEL_RIGHT_NO_OWNERSHIP;
            newFlags |= CHANNEL_RIGHT_FORCE_NO_ANNOUNCEMENTS;
            newFlags |= CHANNEL_RIGHT_CANT_CHANGE_PASSWORD;
            newFlags |= CHANNEL_RIGHT_CANT_BAN;
            newFlags |= CHANNEL_RIGHT_CANT_KICK;
            newFlags |= CHANNEL_RIGHT_CANT_MUTE;
            ChannelMgr::SetChannelRightsFor(sWorldChat->ChatName, newFlags, 0, "", "", mods);
             */
            // static const ChannelRights& GetChannelRightsFor(const std::string& name);
            // SetChannelRightsFor(const std::string& name, const uint32& flags, const uint32& speakDelay, const std::string& joinmessage, const std::string& speakmessage, const std::set<uint32>& moderators);

            sWorldChat->SendWorldChat(player->GetSession(), msg.c_str());
            //channel->RemoveWatching(player);
            msg = -1;
        }
    }
};

class WorldChat_Database : public DatabaseScript
{
public:
    WorldChat_Database() : DatabaseScript("WorldChat_Database") { }

    void OnAfterDatabasesLoaded(uint32 /*updateFlags*/)
    {
        /*
        ChannelMgr* mgr = ChannelMgr::forTeam(TEAM_NEUTRAL);
        ChannelMgr* mgrAlliance = ChannelMgr::forTeam(TEAM_ALLIANCE);
        ChannelMgr* mgrHorde = ChannelMgr::forTeam(TEAM_HORDE);

        mgr->GetJoinChannel(sWorldChat->ChatName, 0);
        mgrAlliance->GetJoinChannel(sWorldChat->ChatName, 0);
        mgrHorde->GetJoinChannel(sWorldChat->ChatName, 0);
         */
    }
};

void AddSC_WorldChat()
{
    new WorldChat_Config();
    new WorldChat_Player();
    new WorldChat_Database();
}
