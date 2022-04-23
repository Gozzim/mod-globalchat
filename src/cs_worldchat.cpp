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
                        { "chat",         HandleWorldChatCommand,     SEC_PLAYER,    Console::No },
                        { "world",        HandleWorldChatCommand,     SEC_PLAYER,    Console::No },
                        { "w",            HandleWorldChatCommand,     SEC_PLAYER,    Console::No },
                        { "c",            HandleWorldChatCommand,     SEC_PLAYER,    Console::No },
                        { "showworld",    HandleWorldChatShowCommand, SEC_PLAYER,    Console::No },
                        { "hideworld",    HandleWorldChatHideCommand, SEC_PLAYER,    Console::No },
                        { "disableworld", HandleWorldChatOffCommand,  SEC_MODERATOR, Console::Yes },
                        { "enableworld",  HandleWorldChatOnCommand,   SEC_MODERATOR, Console::Yes },
                };

        return commandTable;
    }

    static bool HandleWorldChatCommand(ChatHandler* handler, Tail message)
    {
        if (message.empty())
            return false;

        Player* player = handler->GetSession()->GetPlayer();
        sWorldChat->SendWorldChat(player, message.data());
        return true;
    }

    static bool HandleWorldChatOnCommand(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!sWorldChat->WorldChatEnabled)
        {
            sWorldChat->WorldChatEnabled = true;
            sWorld->SendGMText(17002, player->GetName().c_str(), "enabled");
        }
        else
        {
            ChatHandler(player->GetSession()).PSendSysMessage("|cffffd500World Chat is already enabled.|r");
        }
        return true;
    };

    static bool HandleWorldChatOffCommand(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (sWorldChat->WorldChatEnabled)
        {
            sWorldChat->WorldChatEnabled = false;
            sWorld->SendGMText(17002, player->GetName().c_str(), "disabled");
        }
        else
        {
            ChatHandler(player->GetSession()).PSendSysMessage("|cffffd500World Chat is already disabled.|r");
        }
        return true;
    };

    static bool HandleWorldChatShowCommand(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();
        uint64 guid = player->GetGUID().GetCounter();

        if (!sWorldChat->WorldChatEnabled)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("|cffffd500World Chat is currently disabled.|r");
            return true;
        }

        if (sWorldChat->WorldChatMap[guid].enabled)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("|cffffd500World Chat is already visible.|r");
            return true;
        }

        sWorldChat->WorldChatMap[guid].enabled = 1;

        ChatHandler(player->GetSession()).PSendSysMessage("|cffffd500World Chat is now visible.|r");

        return true;
    };

    static bool HandleWorldChatHideCommand(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();
        uint64 guid = player->GetGUID().GetCounter();

        if (!sWorldChat->WorldChatMap[guid].enabled)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("|cffffd500World Chat is already hidden.|r");
            return true;
        }

        sWorldChat->WorldChatMap[guid].enabled = 0;

        ChatHandler(player->GetSession()).PSendSysMessage("|cffffd500World Chat is now hidden.|r");

        return true;
    };
};

void AddSC_worldchat_commandscript()
{
    new worldchat_commandscript();
}
