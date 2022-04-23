#ifndef _WORLDCHAT_H_
#define _WORLDCHAT_H_

#include "Common.h"
#include "Chat.h"
#include "Player.h"
#include <regex>
#include <unordered_map>

typedef struct
{
    bool enabled;
    time_t last_msg;
} WorldChatVars;

class WorldChat
{
public:
    static WorldChat* instance();

    bool WorldChatEnabled;
    bool Announce;
    std::string ChatName;
    std::string ChatNameColor;
    std::string ChatTextColor;
    bool FactionSpecific;
    bool EnableOnLogin;
    uint32 MinPlayTime;
    int BlockProfanities;
    uint32 ProfanityMute;
    int BlockURLs;
    uint32 URLMute;
    uint32 CoolDown;
    bool JoinChannelAllowed;

    const std::regex urlRegex = std::regex{ "((http|ftp)s?://)?([\\w]*(?::[\\w]*)?@)?((?:(www\\.)?(?:[a-zA-Z0-9-\\.]{1,256}\\.[a-zA-Z]{2,63}))|(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?))(:\\d+)?(/[\\w\\.()-/\\\\]*)?((\\?([^#\\s&]+=[^#\\s&]+)(?:&([^#\\s&]+=[^#\\s&]+))*)?(#\\S*)?)?" };

    std::vector <std::string> GMColors;
    std::vector <std::string> ProfanityBlacklist;
    std::vector <std::string> URLWhitelist;

    std::unordered_map <uint32, WorldChatVars> WorldChatMap;

    void LoadConfig(bool reload);

    bool HasForbiddenPhrase(std::string message);
    bool HasForbiddenURL(std::string message);

    void SendWorldChat(Player* player, const char* message);

private:
    std::string GetChatPrefix();
    std::string GetNameLink(Player* player);
    std::string BuildChatContent(const char* text);
    std::string BuildChatMessage(std::string prefix, std::string nameLink, std::string content);
};

#define sWorldChat WorldChat::instance()

#endif
