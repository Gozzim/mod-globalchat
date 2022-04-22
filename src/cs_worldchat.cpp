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

    static bool HandleWorldChatCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
        {
            return false;
        }
        Player* player = handler->GetSession()->GetPlayer();
        sWorldChat->SendWorldChat(player, args);
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
