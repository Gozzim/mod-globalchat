#include "Channel.h"
#include "WorldChat.h"
#include "ScriptMgr.h"

class WorldChat_Config : public WorldScript
{
public: WorldChat_Config() : WorldScript("WorldChat_Config") { };

    void OnBeforeConfigLoad(bool reload) override
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
        if (sWorldChat->JoinChannelAllowed && sWorldChat->ChannelName != "" && lang != LANG_ADDON && !strcmp(channel->GetName().c_str(), sWorldChat->ChannelName.c_str()))
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
