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
#include "GlobalChatMgr.h"

using namespace Acore::ChatCommands;

class globalchat_commandscript : public CommandScript
{
public:
    globalchat_commandscript() : CommandScript("globalchat_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable commandTable =
                {
                        { "chat",        HandleGlobalChatCommand,        SEC_PLAYER,    Console::Yes },
                        { "world",      HandleGlobalChatCommand,        SEC_PLAYER,    Console::Yes },
                        { "w",           HandleGlobalChatCommand,        SEC_PLAYER,    Console::Yes },
                        { "c",           HandleGlobalChatCommand,        SEC_PLAYER,    Console::Yes },
                        { "joinworld",  HandleGlobalChatJoinCommand,    SEC_PLAYER,    Console::No },
                        { "leaveworld", HandleGlobalChatLeaveCommand,   SEC_PLAYER,    Console::No },
                        { "wdisable",    HandleGlobalChatDisableCommand, SEC_MODERATOR, Console::Yes },
                        { "wenable",     HandleGlobalChatEnableCommand,  SEC_MODERATOR, Console::Yes },
                        { "wmute",       HandleMuteGlobalChat,           SEC_MODERATOR, Console::Yes },
                        { "wunmute",     HandleUnmuteGlobalChat,         SEC_MODERATOR, Console::Yes },
                        { "winfo",       HandlePlayerInfoGlobalChat,     SEC_MODERATOR, Console::Yes },
                };

        return commandTable;
    }

    static bool HandleGlobalChatCommand(ChatHandler* handler, Tail message)
    {
        if (message.empty())
            return false;

        WorldSession* session = handler->GetSession();
        sGlobalChatMgr->SendGlobalChat(session, message.data());
        return true;
    }

    static bool HandleGlobalChatEnableCommand(ChatHandler* handler)
    {
        std::string playerName = "Console";

        if (handler->GetSession())
        {
            playerName = handler->GetSession()->GetPlayer()->GetName();
        }

        if (sGlobalChatMgr->GlobalChatEnabled)
        {
            handler->PSendSysMessage("The WorldChat is already enabled.");
            return false;
        }

        sGlobalChatMgr->GlobalChatEnabled = true;
        sWorld->SendWorldText(17002, playerName.c_str(), "enabled");
        LOG_INFO("module", "GlobalChat: Player {} enabled the GlobalChat.", playerName);

        return true;
    };

    static bool HandleGlobalChatDisableCommand(ChatHandler* handler)
    {
        std::string playerName = "Console";
        if (handler->GetSession())
        {
            playerName = handler->GetSession()->GetPlayer()->GetName();
        }

        if (!sGlobalChatMgr->GlobalChatEnabled)
        {
            handler->PSendSysMessage("The WorldChat is already disabled.");
            return false;
        }

        sGlobalChatMgr->GlobalChatEnabled = false;
        sWorld->SendWorldText(17002, playerName.c_str(), "disabled");
        LOG_INFO("module", "GlobalChat: Player {} disabled the GlobalChat.", playerName);

        return true;
    };

    static bool HandleGlobalChatJoinCommand(ChatHandler* handler)
    {
        sGlobalChatMgr->PlayerJoinCommand(handler);
        return true;
    };

    static bool HandleGlobalChatLeaveCommand(ChatHandler* handler)
    {
        sGlobalChatMgr->PlayerLeaveCommand(handler);
        return true;
    };

    static bool HandleMuteGlobalChat(ChatHandler* handler, PlayerIdentifier player, std::string duration, Tail muteReason)
    {
        std::string playerName = "Console";
        Player* target = player.GetConnectedPlayer();

        if (!target)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (duration.empty())
            return false;

        if (handler->GetSession() && handler->GetSession()->GetSecurity() <= target->GetSession()->GetSecurity())
            return false;

        if (handler->GetSession())
            playerName = handler->GetSession()->GetPlayer()->GetName();

        std::string muteReasonStr{ muteReason };
        ObjectGuid guid = target->GetGUID();

        if (atoi(duration.c_str()) < 0)
        {
            sGlobalChatMgr->Ban(guid);
            LOG_INFO("module", "GlobalChat: Player {} banned {} in GlobalChat.", playerName, target->GetName());

            if (sGlobalChatMgr->AnnounceMutes)
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
        sGlobalChatMgr->Mute(guid, durationSecs);
        LOG_INFO("module", "GlobalChat: Player {} muted {} for {} in GlobalChat.", playerName, target->GetName(), secsToTimeString(durationSecs, true));

        if (sGlobalChatMgr->AnnounceMutes)
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

    static bool HandleUnmuteGlobalChat(ChatHandler* handler, PlayerIdentifier player)
    {
        Player* target = player.GetConnectedPlayer();

        if (!target)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        ObjectGuid guid = target->GetGUID();
        sGlobalChatMgr->Unmute(guid);
        LOG_INFO("module", "GlobalChat: Player {} was unmuted in GlobalChat.", target->GetName());

        return true;
    };

    static bool HandlePlayerInfoGlobalChat(ChatHandler* handler, Optional<PlayerIdentifier> player)
    {
        if (!player)
        {
            player = PlayerIdentifier::FromTarget(handler);
        }

        if (!player || !player->IsConnected())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = player->GetConnectedPlayer();
        sGlobalChatMgr->PlayerInfoCommand(handler, target);

        return true;
    };
};

void AddSC_globalchat_commandscript()
{
    new globalchat_commandscript();
}
