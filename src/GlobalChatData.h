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

#ifndef _GLOBALCHATDATA_H_
#define _GLOBALCHATDATA_H_

#include "GlobalChatMgr.h"

class GlobalChatData
{
public:
    GlobalChatData();
    ~GlobalChatData();

    bool IsInChat() const;
    void SetInChat(bool _inChat);

    time_t GetLastMessage() const;
    void SetLastMessage(time_t _lastMessage);

    time_t GetMuteTime() const;
    void SetMuteTime(time_t _muteTime);

    uint32 GetTotalMutes() const;
    void SetTotalMutes(uint32 _totalMutes);

    bool IsBanned() const;
    void SetBanned(bool _banned);

private:
    bool inChat;
    time_t lastMessage;
    time_t muteTime;
    uint32 totalMutes;
    bool banned;
};

#endif
