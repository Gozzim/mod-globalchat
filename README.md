# This is a module for  ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

## Advanced GlobalChat

[![core-build](https://github.com/Gozzim/mod-globalchat/actions/workflows/core-build.yml/badge.svg)](https://github.com/Gozzim/mod-globalchat)
[![CodeFactor](https://www.codefactor.io/repository/github/gozzim/mod-globalchat/badge)](https://www.codefactor.io/repository/github/gozzim/mod-globalchat)

## Description

**Advanced GlobalChat** with around **30 configuration options**.\
This GlobalChat allows you to configure the module to your individual needs!

## Features

- Enable/Disable Chat per command
- Players can join and leave the chat
- Enable/Disable Faction specific chats
- Set Chat Name/NameColor/TextColor
- Configure Class/Race/Faction Icons to your need
- Spam Protection
- URL Filters/Censoring
- Offensive language Filters/Censoring
- Support for custom GM ranks
- Information of players having left or joined the Chat, their Mutes and Bans in the GlobalChat is saved and loaded.
- And a total of almost **30 Configuration Options** to customize the Chat to your needs!

## Commands

### Player Commands

- `.chat` or `.global` - Write a message in the GlobalChat
- `.joinglobal` - Join the GlobalChat
- `.leaveglobal` - Leave the GlobalChat
- `/join <GlobalChatName>` - Join the GlobalChat like a normal Channel (Name configured in conf file)

### GM Commands

- `.genable` - Enables the GlobalChat
- `.gdisable` - Disables the GlobalChat (GMs can still send messages)
- `.gmute` - Mute a player in the GlobalChat for a certain amount of time or permanently
- `.gunmute` - Unmute a player in the GlobalChat
- `.ginfo` - Get information about player (Mutes, etc.)
- `.galliance` or `.ghorde` - Chat in Faction GlobalChat (if Faction specific chats are enabled)
- `.gblacklist` - Add, Remove or Reload Phrases to and from the Blacklisted Obscenities in the GlobalChat

## Screenshots

|                                                                                                                                                                                                                                          Chat Customization                                                                                                    |                                                                                                                                                                                      **Faction Specific Chat**                                                                                                                                                                 |
|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
| *Chat Name & Prefix* ![Url](https://raw.githubusercontent.com/Gozzim/mod-globalchat/master/images/ChatPrefix.jpg) <br/> *Player Icons* ![Icons](https://raw.githubusercontent.com/Gozzim/mod-globalchat/master/images/PlayerIcons.jpg) <br/> *Player Name* ![PlayerName](https://raw.githubusercontent.com/Gozzim/mod-globalchat/master/images/PlayerName.jpg) | *Viewpoint of the Horde* ![Horde](https://raw.githubusercontent.com/Gozzim/mod-globalchat/master/images/HordeChat.png) <br/> *Viewpoint of the Alliance* ![Alliance](https://raw.githubusercontent.com/Gozzim/mod-globalchat/master/images/AllianceChat.jpg) <br/> *Viewpoint of a GM* ![GM](https://raw.githubusercontent.com/Gozzim/mod-globalchat/master/images/GMChat.jpg) |
|                                                                                                                                                                         **Filtering**                                                                                                                                                                          |                                                                                                                                                                             **Improved Usability**                                                                                                                                                                             |
|       *Offensive Language* ![Profanity](https://raw.githubusercontent.com/Gozzim/mod-globalchat/master/images/Profanities.jpg) <br/> *URLs* ![Url](https://raw.githubusercontent.com/Gozzim/mod-globalchat/master/images/Url.jpg) <br/> *PlayerInfo*  ![PlayerInfo](https://raw.githubusercontent.com/Gozzim/mod-globalchat/master/images/ChatInfo.jpg)        | *Join and Leave the Chat* ![Join](https://raw.githubusercontent.com/Gozzim/mod-globalchat/master/images/JoinLeaveGlobal.gif) <br/> *Click to Chat* ![Click](https://raw.githubusercontent.com/Gozzim/mod-globalchat/master/images/ClickChat.gif) <br/>  *Use like a Channel* ![Channel](https://raw.githubusercontent.com/Gozzim/mod-globalchat/master/images/JoinChannel.gif) |

## Installation

```
1) Simply place the module under the `modules` directory of your AzerothCore source. 
2) Import the SQL into the Database.
3) Re-run cmake and launch a clean build of AzerothCore.
```

## Edit module configuration (optional)

If you need to change the module configuration, go to your server configuration folder (where your worldserver or worldserver.exe is), copy globalchat.conf.dist to globalchat.conf and edit that new file.

## Credits

- Module for AzerothCore created by [Gozzim](https://github.com/Gozzim)
- AzerothCore: [repository](https://github.com/azerothcore) - [website](http://azerothcore.org/)

## License

This code and content is released under the [GNU AGPL-3.0 license](https://github.com/Gozzim/mod-globalchat/blob/master/LICENSE).
