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

#include "GameTime.h"
#include "ScriptMgr.h"
#include "WorldChat.h"

using namespace Acore::ChatCommands;

class worldchat_commandscript : public CommandScript
{
public:
    worldchat_commandscript() : CommandScript("worldchat_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable commandTable =
                {
                        { "chat",       HandleWorldChatCommand,        SEC_PLAYER,    Console::Yes },
                        { "world",      HandleWorldChatCommand,        SEC_PLAYER,    Console::Yes },
                        { "w",          HandleWorldChatCommand,        SEC_PLAYER,    Console::Yes },
                        { "c",          HandleWorldChatCommand,        SEC_PLAYER,    Console::Yes },
                        { "joinworld",  HandleWorldChatJoinCommand,    SEC_PLAYER,    Console::No },
                        { "leaveworld", HandleWorldChatLeaveCommand,   SEC_PLAYER,    Console::No },
                        { "wdisable",   HandleWorldChatDisableCommand, SEC_MODERATOR, Console::Yes },
                        { "wenable",    HandleWorldChatEnableCommand,  SEC_MODERATOR, Console::Yes },
                        { "wmute",      HandleMuteWorldChat,           SEC_MODERATOR, Console::Yes },
                        { "wunmute",    HandleUnmuteWorldChat,         SEC_MODERATOR, Console::Yes },
                };

        return commandTable;
    }

    static bool HandleWorldChatCommand(ChatHandler* handler, Tail message)
    {
        if (message.empty())
            return false;

        WorldSession* session = handler->GetSession();
        sWorldChat->SendWorldChat(session, message.data());
        return true;
    }

    static bool HandleWorldChatEnableCommand(ChatHandler* handler)
    {
        std::string playerName = "Console";

        if (handler->GetSession())
        {
            playerName = handler->GetSession()->GetPlayer()->GetName();
        }

        if (sWorldChat->WorldChatEnabled)
        {
            handler->PSendSysMessage("The WorldChat is already enabled.");
            return false;
        }

        sWorldChat->WorldChatEnabled = true;
        sWorld->SendWorldText(17002, playerName.c_str(), "enabled");
        return true;
    };

    static bool HandleWorldChatDisableCommand(ChatHandler* handler)
    {
        std::string playerName = "Console";
        if (handler->GetSession())
        {
            playerName = handler->GetSession()->GetPlayer()->GetName();
        }

        if (!sWorldChat->WorldChatEnabled)
        {
            handler->PSendSysMessage("The WorldChat is already disabled.");
            return false;
        }

        sWorldChat->WorldChatEnabled = false;
        sWorld->SendWorldText(17002, playerName.c_str(), "disabled");
        return true;
    };

    static bool HandleWorldChatJoinCommand(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();
        uint64 guid = player->GetGUID().GetCounter();

        if (!sWorldChat->WorldChatEnabled)
        {
            handler->PSendSysMessage("The WorldChat is currently disabled.");
            return true;
        }

        if (sWorldChat->WorldChatMap[guid].enabled)
        {
            handler->PSendSysMessage("You already joined the WorldChat.");
            return true;
        }

        sWorldChat->WorldChatMap[guid].enabled = 1;

        handler->PSendSysMessage("You have joined the WorldChat.");
        return true;
    };

    static bool HandleWorldChatLeaveCommand(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();
        uint64 guid = player->GetGUID().GetCounter();

        if (!sWorldChat->WorldChatMap[guid].enabled)
        {
            handler->PSendSysMessage("You already left the WorldChat.");
            return true;
        }

        sWorldChat->WorldChatMap[guid].enabled = 0;

        handler->PSendSysMessage("You have left the WorldChat.");

        return true;
    };

    static bool HandleMuteWorldChat(ChatHandler* handler, PlayerIdentifier player, std::string duration, Tail muteReason)
    {
        std::string playerName = "Console";
        Player* target = player.GetConnectedPlayer();

        if (!target || duration.empty())
            return false;

        if (handler->GetSession() && handler->GetSession()->GetSecurity() <= target->GetSession()->GetSecurity())
            return false;

        if (handler->GetSession())
            playerName = handler->GetSession()->GetPlayer()->GetName();

        std::string muteReasonStr{ muteReason };
        uint64 guid = target->GetGUID().GetCounter();

        if (atoi(duration.c_str()) < 0)
        {
            sWorldChat->WorldChatMap[guid].banned = true;
            if (sWorldChat->AnnounceMutes)
            {
                sWorld->SendWorldText(17004, playerName.c_str(), target->GetName().c_str(), muteReasonStr.c_str());
            }
            else
            {
                ChatHandler(target->GetSession()).PSendSysMessage(17006, muteReasonStr.c_str());
                sWorld->SendGMText(17004, playerName.c_str(), target->GetName().c_str(), muteReasonStr.c_str());
            }

            return true;
        }

        uint32 durationSecs = TimeStringToSecs(duration);
        int64 muteTime = GameTime::GetGameTime().count() + durationSecs;
        sWorldChat->WorldChatMap[guid].mute_time = muteTime;
        if (sWorldChat->AnnounceMutes)
        {
            sWorld->SendWorldText(17003, playerName.c_str(), target->GetName().c_str(), secsToTimeString(durationSecs, true).c_str(), muteReasonStr.c_str());
        }
        else
        {
            ChatHandler(target->GetSession()).PSendSysMessage(17005, secsToTimeString(durationSecs, true).c_str(), muteReasonStr.c_str());
            sWorld->SendGMText(17003, playerName.c_str(), target->GetName().c_str(), secsToTimeString(durationSecs, true).c_str(), muteReasonStr.c_str());
        }

        return true;
    };

    static bool HandleUnmuteWorldChat(ChatHandler* /*handler*/, PlayerIdentifier player)
    {
        Player* target = player.GetConnectedPlayer();

        if (!target)
            return false;

        uint64 guid = target->GetGUID().GetCounter();
        sWorldChat->WorldChatMap[guid].banned = false;
        sWorldChat->WorldChatMap[guid].mute_time = 0;
        return true;
    };
};

void AddSC_worldchat_commandscript()
{
    new worldchat_commandscript();
}
