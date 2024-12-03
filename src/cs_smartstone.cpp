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

#include "Chat.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Smartstone.h"

using namespace Acore::ChatCommands;

class smartstone_commandscript : public CommandScript
{
public:
    smartstone_commandscript() : CommandScript("smartstone_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable unlockServiceTable =
        {
            { "pet", HandleSmartstonePet,    SEC_MODERATOR, Console::Yes },
        };

        static ChatCommandTable smartstoneTable =
        {
            { "unlock", unlockServiceTable },
            { "",       HandleSmartStoneCommand, SEC_MODERATOR, Console::No },
        };

        static ChatCommandTable commandTable =
        {
            { "smartstone", smartstoneTable },
        };

        return commandTable;
    }

    static bool HandleSmartStoneCommand(ChatHandler* handler)
    {
        if (!sSmartstone->IsSmartstoneEnabled())
        {
            handler->GetPlayer()->SendSystemMessage("The smartstone is disabled.");
            handler->SetSentErrorMessage(true);
            return true;
        }

        if (Player* player = handler->GetPlayer())
        {
            if (!player->HasItemCount(sSmartstone->GetSmartstoneItemID(), 1))
                player->AddItem(sSmartstone->GetSmartstoneItemID(), 1);
            else
                player->SendSystemMessage("You already have a smartstone.");
        }

        return true;
    }

    static bool HandleSmartstonePet(ChatHandler* handler, PlayerIdentifier player, uint32 petId, bool add)
    {
        if (!sSmartstone->IsSmartstoneEnabled())
        {
            handler->GetPlayer()->SendSystemMessage("The smartstone is disabled.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (add)
        {
            if (Player* target = player.GetConnectedPlayer())
            {
                SmartstonePetData petData = sSmartstone->GetPetData(ACTION_RANGE_SUMMON_PET + petId);

                if (target->GetPlayerSetting(ModName, petId).IsEnabled())
                {
                    handler->PSendSysMessage("The pet {} is already unlocked.", petData.Description);
                    handler->SetSentErrorMessage(true);
                    return false;
                }

                target->UpdatePlayerSetting(ModName, petId, true);
                handler->PSendSysMessage("The pet {} has been unlocked for {}.", petData.Description, target->GetName());
            }
        }
        else
        {
            if (Player* target = player.GetConnectedPlayer())
            {
                SmartstonePetData petData = sSmartstone->GetPetData(ACTION_RANGE_SUMMON_PET + petId);
                if (!target->GetPlayerSetting(ModName, petId).IsEnabled())
                {
                    handler->PSendSysMessage("The player does not have the pet {}.", petData.Description);
                    handler->SetSentErrorMessage(true);
                    return false;
                }

                target->UpdatePlayerSetting(ModName, petId, false);
                handler->PSendSysMessage("The pet {} has been removed for {}.", petData.Description, target->GetName());
            }
        }

        return true;
    }
};

void AddSC_smartstone_commandscript()
{
    new smartstone_commandscript();
}
