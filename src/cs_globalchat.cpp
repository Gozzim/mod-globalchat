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
#include <regex>

using namespace Acore::ChatCommands;

class globalchat_commandscript : public CommandScript
{
public:
    globalchat_commandscript() : CommandScript("globalchat_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable blacklistCommandTable =
                {
                        { "add",      HandleBlacklistAddCommand,         SEC_MODERATOR, Console::Yes },
                        { "remove",   HandleBlacklistRemoveCommand,      SEC_MODERATOR, Console::Yes },
                        { "reload",   HandleBlacklistReloadCommand,      SEC_MODERATOR, Console::Yes },
                };

        static ChatCommandTable commandTable =
                {
                        { "chat",        HandleGlobalChatCommand,        SEC_PLAYER,    Console::Yes },
                        { "global",      HandleGlobalChatCommand,        SEC_PLAYER,    Console::Yes },
                        { "g",           HandleGlobalChatCommand,        SEC_PLAYER,    Console::Yes },
                        { "c",           HandleGlobalChatCommand,        SEC_PLAYER,    Console::Yes },
                        { "joinglobal",  HandleGlobalChatJoinCommand,    SEC_PLAYER,    Console::No },
                        { "leaveglobal", HandleGlobalChatLeaveCommand,   SEC_PLAYER,    Console::No },
                        { "gdisable",    HandleGlobalChatDisableCommand, SEC_MODERATOR, Console::Yes },
                        { "genable",     HandleGlobalChatEnableCommand,  SEC_MODERATOR, Console::Yes },
                        { "gmute",       HandleMuteGlobalChat,           SEC_MODERATOR, Console::Yes },
                        { "gunmute",     HandleUnmuteGlobalChat,         SEC_MODERATOR, Console::Yes },
                        { "ginfo",       HandlePlayerInfoGlobalChat,     SEC_MODERATOR, Console::Yes },
                        { "galliance",   HandleGMAllianceChatCommand,    SEC_MODERATOR, Console::Yes },
                        { "ghorde",      HandleGMHordeChatCommand,       SEC_MODERATOR, Console::Yes },
                        { "gblacklist",  blacklistCommandTable },
                };

        return commandTable;
    }

    static bool HandleGlobalChatCommand(ChatHandler* handler, Tail message)
    {
        if (message.empty())
            return false;

        WorldSession* session = handler->GetSession();

        if (sGlobalChatMgr->FactionSpecific && session->GetSecurity() > 0)
        {
            handler->SendSysMessage("Please use |cff4CFF00.galliance|r or .|cff4CFF00ghorde|r for the GlobalChat as GM.");
            handler->SetSentErrorMessage(true);
            return true;
        }

        sGlobalChatMgr->SendGlobalChat(session, message.data());
        return true;
    }

    static bool HandleGMAllianceChatCommand(ChatHandler* handler, Tail message)
    {
        if (message.empty())
            return false;

        WorldSession* session = handler->GetSession();
        sGlobalChatMgr->SendGlobalChat(session, message.data(), TEAM_ALLIANCE);
        return true;
    }

    static bool HandleGMHordeChatCommand(ChatHandler* handler, Tail message)
    {
        if (message.empty())
            return false;

        WorldSession* session = handler->GetSession();
        sGlobalChatMgr->SendGlobalChat(session, message.data(), TEAM_HORDE);
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
            handler->SendSysMessage("The GlobalChat is already enabled.");
            handler->SetSentErrorMessage(true);
            return true;
        }

        sGlobalChatMgr->GlobalChatEnabled = true;
        sWorld->SendWorldText(LANG_GLOBALCHAT_STATE_ANNOUNCE_WORLD, playerName.c_str(), "enabled");
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
            handler->SendSysMessage("The GlobalChat is already disabled.");
            handler->SetSentErrorMessage(true);
            return true;
        }

        sGlobalChatMgr->GlobalChatEnabled = false;
        sWorld->SendWorldText(LANG_GLOBALCHAT_STATE_ANNOUNCE_WORLD, playerName.c_str(), "disabled");
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
        {
            handler->SendSysMessage(LANG_YOURS_SECURITY_IS_LOW);
            handler->SetSentErrorMessage(true);
            return true;
        }

        if (handler->GetSession())
            playerName = handler->GetSession()->GetPlayer()->GetName();

        std::string muteReasonStr{ muteReason };
        ObjectGuid guid = target->GetGUID();

        if (atoi(duration.c_str()) <= 0)
        {
            sGlobalChatMgr->Ban(guid);
            LOG_INFO("module", "GlobalChat: Player {} banned {} in GlobalChat.", playerName, target->GetName());

            if (sGlobalChatMgr->AnnounceMutes)
            {
                sWorld->SendWorldText(LANG_GLOBALCHAT_PLAYER_BANNED_ANNOUNCE_WORLD, playerName.c_str(), target->GetName().c_str(), muteReasonStr.c_str());
            }
            else
            {
                ChatHandler(target->GetSession()).PSendSysMessage(LANG_GLOBALCHAT_BANNED_ANNOUNCE_SELF, muteReasonStr.c_str());
                sWorld->SendGMText(LANG_GLOBALCHAT_PLAYER_BANNED_ANNOUNCE_WORLD, playerName.c_str(), target->GetName().c_str(), muteReasonStr.c_str());
            }

            return true;
        }

        uint32 durationSecs = TimeStringToSecs(duration);
        sGlobalChatMgr->Mute(guid, durationSecs);
        LOG_INFO("module", "GlobalChat: Player {} muted {} for {} in GlobalChat.", playerName, target->GetName(), secsToTimeString(durationSecs, true));

        if (sGlobalChatMgr->AnnounceMutes)
        {
            sWorld->SendWorldText(LANG_GLOBALCHAT_PLAYER_MUTED_ANNOUNCE_WORLD, playerName.c_str(), target->GetName().c_str(), secsToTimeString(durationSecs, true).c_str(), muteReasonStr.c_str());
        }
        else
        {
            ChatHandler(target->GetSession()).PSendSysMessage(LANG_GLOBALCHAT_MUTED_ANNOUNCE_SELF, secsToTimeString(durationSecs, true).c_str(), muteReasonStr.c_str());
            sWorld->SendGMText(LANG_GLOBALCHAT_PLAYER_MUTED_ANNOUNCE_WORLD, playerName.c_str(), target->GetName().c_str(), secsToTimeString(durationSecs, true).c_str(), muteReasonStr.c_str());
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

    static bool HandleBlacklistAddCommand(ChatHandler* handler, Tail phrase)
    {
        if (phrase.empty())
            return false;

        QueryResult check = CharacterDatabase.Query("SELECT * FROM `globalchat_blacklist` WHERE `phrase` = '{}'", phrase);
        if (check)
        {
            handler->SendSysMessage("Phrase is already blacklisted.");
            handler->SetSentErrorMessage(true);
            return true;
        }

        CharacterDatabase.Query("INSERT INTO `globalchat_blacklist` VALUES ('{}')", phrase);
        sGlobalChatMgr->ProfanityBlacklist[phrase.data()] = std::regex{phrase.data(), std::regex::icase | std::regex::optimize};
        handler->PSendSysMessage("Phrase '%s' is now blacklisted in the GlobalChat.", phrase);
        LOG_INFO("module", "GlobalChat: Phrase '{}' is now blacklisted.", phrase);

        return true;
    };

    static bool HandleBlacklistRemoveCommand(ChatHandler* handler, Tail phrase)
    {
        if (phrase.empty())
            return false;

        QueryResult check = CharacterDatabase.Query("SELECT * FROM `globalchat_blacklist` WHERE `phrase` = '{}'", phrase);
        if (!check)
        {
            handler->SendSysMessage("Phrase is not blacklisted.");
            handler->SetSentErrorMessage(true);
            return true;
        }

        CharacterDatabase.Query("DELETE FROM `globalchat_blacklist` WHERE `phrase` = '{}'", phrase);
        sGlobalChatMgr->ProfanityBlacklist.erase(phrase.data());
        handler->PSendSysMessage("Phrase '%s' is no longer blacklisted in the GlobalChat.", phrase);
        LOG_INFO("module", "GlobalChat: Phrase '{}' is no longer blacklisted.", phrase);

        return true;
    };

    static bool HandleBlacklistReloadCommand(ChatHandler* handler)
    {
        sGlobalChatMgr->ProfanityBlacklist.clear();
        sGlobalChatMgr->LoadBlacklistDB();

        if (sGlobalChatMgr->ProfanityFromDBC)
            sGlobalChatMgr->LoadProfanityDBC();

        handler->SendSysMessage("GlobalChat Blacklist reloaded.");
        LOG_INFO("module", "GlobalChat: Profanity Blacklist reloaded.");

        return true;
    };
};

void AddSC_globalchat_commandscript()
{
    new globalchat_commandscript();
}
