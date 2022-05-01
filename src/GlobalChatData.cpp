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

#include "GlobalChatData.h"

GlobalChatData::GlobalChatData()
{
    inChat = sGlobalChatMgr->EnableOnLogin;
    lastMessage = 0;
    muteTime = 0;
    totalMutes = 0;
    banned = false;
}

GlobalChatData::~GlobalChatData()
{
}

bool GlobalChatData::IsInChat() const
{
    return inChat;
}

void GlobalChatData::SetInChat(bool _inChat)
{
    inChat = _inChat;
}

time_t GlobalChatData::GetLastMessage() const
{
    return lastMessage;
}

void GlobalChatData::SetLastMessage(time_t _lastMessage)
{
    lastMessage = _lastMessage;
}

time_t GlobalChatData::GetMuteTime() const
{
    return muteTime;
}

void GlobalChatData::SetMuteTime(time_t _muteTime)
{
    muteTime = _muteTime;
}

uint32 GlobalChatData::GetTotalMutes() const
{
    return totalMutes;
}

void GlobalChatData::SetTotalMutes(uint32 _totalMutes)
{
    totalMutes = _totalMutes;
}

bool GlobalChatData::IsBanned() const
{
    return banned;
}

void GlobalChatData::SetBanned(bool _banned)
{
    banned = _banned;
}