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
                        { "chat",         HandleWorldChatCommand,        SEC_PLAYER,    Console::Yes },
                        { "world",        HandleWorldChatCommand,        SEC_PLAYER,    Console::Yes },
                        { "w",            HandleWorldChatCommand,        SEC_PLAYER,    Console::Yes },
                        { "c",            HandleWorldChatCommand,        SEC_PLAYER,    Console::Yes },
                        { "joinworld",    HandleWorldChatJoinCommand,    SEC_PLAYER,    Console::No },
                        { "leaveworld",   HandleWorldChatLeaveCommand,   SEC_PLAYER,    Console::No },
                        { "disableworld", HandleWorldChatDisableCommand, SEC_MODERATOR, Console::Yes },
                        { "enableworld",  HandleWorldChatEnableCommand,  SEC_MODERATOR, Console::Yes },
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
        Player* player = handler->GetSession()->GetPlayer();

        if (!sWorldChat->WorldChatEnabled)
        {
            sWorldChat->WorldChatEnabled = true;
            sWorld->SendGMText(17002, player->GetName().c_str(), "enabled");
        }
        else
        {
            ChatHandler(player->GetSession()).PSendSysMessage("|cffffd500The WorldChat is already enabled.|r");
        }
        return true;
    };

    static bool HandleWorldChatDisableCommand(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (sWorldChat->WorldChatEnabled)
        {
            sWorldChat->WorldChatEnabled = false;
            sWorld->SendGMText(17002, player->GetName().c_str(), "disabled");
        }
        else
        {
            ChatHandler(player->GetSession()).PSendSysMessage("|cffffd500The WorldChat is already disabled.|r");
        }
        return true;
    };

    static bool HandleWorldChatJoinCommand(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();
        uint64 guid = player->GetGUID().GetCounter();

        if (!sWorldChat->WorldChatEnabled)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("|cffffd500The WorldChat is currently disabled.|r");
            return true;
        }

        if (sWorldChat->WorldChatMap[guid].enabled)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("|cffffd500You already joined the WorldChat.|r");
            return true;
        }

        sWorldChat->WorldChatMap[guid].enabled = 1;

        ChatHandler(player->GetSession()).PSendSysMessage("|cffffd500Joined the WorldChat.|r");

        return true;
    };

    static bool HandleWorldChatLeaveCommand(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();
        uint64 guid = player->GetGUID().GetCounter();

        if (!sWorldChat->WorldChatMap[guid].enabled)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("|cffffd500You already left the WorldChat.|r");
            return true;
        }

        sWorldChat->WorldChatMap[guid].enabled = 0;

        ChatHandler(player->GetSession()).PSendSysMessage("|cffffd500Left the WorldChat.|r");

        return true;
    };
};

void AddSC_worldchat_commandscript()
{
    new worldchat_commandscript();
}
