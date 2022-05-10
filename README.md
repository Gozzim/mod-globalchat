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

|                                                                          Chat Customization                                                                          |                                                                              **Faction Specific Chat**                                                                               |
|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------:|:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
| *Chat Name & Prefix* ![Url](/images/ChatPrefix.jpg) <br/> *Player Icons* ![Icons](/images/PlayerIcons.jpg) <br/> *Player Name* ![PlayerName](/images/PlayerName.jpg) | *Viewpoint of the Horde* ![Horde](/images/HordeChat.png) <br/> *Viewpoint of the Alliance* ![Alliance](/images/AllianceChat.jpg) <br/> *Viewpoint of a GM* ![GM](/images/GMChat.jpg) |
|                                                                            **Filtering**                                                                             |                                                                                **Improved Usability**                                                                                |
|       *Offensive Language* ![Profanity](/images/Profanities.jpg) <br/> *URLs* ![Url](/images/Url.jpg) <br/> *PlayerInfo*  ![PlayerInfo](/images/ChatInfo.jpg)        | *Join and Leave the Chat* ![Join](/images/JoinLeaveGlobal.gif) <br/> *Click to Chat* ![Click](/images/ClickChat.gif) <br/>  *Use like a Channel* ![Channel](/images/JoinChannel.gif) |

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
