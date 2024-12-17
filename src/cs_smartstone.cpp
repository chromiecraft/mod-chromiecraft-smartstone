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
#include "GameTime.h"
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
        static ChatCommandTable smartstoneTable =
        {
            { "unlock service", HandleSmartStoneUnlockServiceCommand, SEC_MODERATOR, Console::No},
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

    static bool HandleSmartStoneUnlockServiceCommand(ChatHandler* handler, PlayerIdentifier player, uint8 category, uint32 petId, bool add)
    {
        if (!sSmartstone->IsSmartstoneEnabled())
        {
            handler->SendErrorMessage("The smartstone is disabled.");
            return false;
        }

        Player* target = player.GetConnectedPlayer();

        if (!target)
        {
            handler->SendErrorMessage("The player is not online.");
            return false;
        }

        std::string ModuleString = ModName + GetModuleStringForCategory(category);

        if (add)
        {
            SmartstonePetData petData = sSmartstone->GetPetData(category == SERVICE_CAT_PET ? ACTION_RANGE_SUMMON_PET + petId : ACTION_RANGE_SUMMON_COMBAT_PET + petId, category);

            if (!petData.CreatureId)
            {
                handler->SendErrorMessage("The pet {} does not exist.", petId);
                return false;
            }

            if (target->GetPlayerSetting(ModuleString, petId).IsEnabled())
            {
                handler->PSendSysMessage("The pet {} is already unlocked.", petData.Description);
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (petData.Duration)
            {
                uint32 expireDate = 0;
                if (petData.Duration > 31556926)
                    expireDate = petData.Duration;
                else
                    expireDate = GameTime::GetGameTime().count() + petData.Duration;

                CharacterDatabase.Execute("INSERT INTO smartstone_char_temp_services (PlayerGUID, ServiceId, Category, ActivationTime, ExpirationTime) VALUES ({}, {}, {}, UNIX_TIMESTAMP(), {})",
                    target->GetGUID().GetCounter(), petData.CreatureId, category, expireDate);

                SmartstoneServiceExpireInfo expireInfo;
                expireInfo.PlayerGUID = target->GetGUID().GetCounter();
                expireInfo.ServiceId = petData.CreatureId;
                expireInfo.Category = category;
                expireInfo.ActivationTime = GameTime::GetGameTime().count();
                expireInfo.ExpirationTime = expireDate;
                sSmartstone->ServiceExpireInfo[target->GetGUID().GetCounter()].push_back(expireInfo);
            }

            target->UpdatePlayerSetting(ModuleString, petId, true);
            handler->PSendSysMessage("The pet {} has been unlocked for {}.", petData.Description, target->GetName());
        }
        else
        {
            SmartstonePetData petData = sSmartstone->GetPetData(category == SERVICE_CAT_PET ? ACTION_RANGE_SUMMON_PET + petId : ACTION_RANGE_SUMMON_COMBAT_PET + petId, category);
            if (!target->GetPlayerSetting(ModuleString, petId).IsEnabled())
            {
                handler->PSendSysMessage("The player does not have the pet {}.", petData.Description);
                handler->SetSentErrorMessage(true);
                return false;
            }

            target->UpdatePlayerSetting(ModuleString, petId, false);
            handler->PSendSysMessage("The pet {} has been removed for {}.", petData.Description, target->GetName());
        }

        return true;
    }

    static std::string GetModuleStringForCategory(uint8 category)
    {
        switch (category)
        {
            case SERVICE_CAT_PET:
                return "#pet";
            case SERVICE_CAT_COMBAT_PET:
                return "#combatpet";
            default:
                return "";
        }
    }
};

void AddSC_smartstone_commandscript()
{
    new smartstone_commandscript();
}
