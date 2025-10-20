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
            { "unlock service", HandleSmartStoneUnlockServiceCommand, SEC_MODERATOR,     Console::Yes },
            { "reload",         HandleSmartstoneReloadCommand,        SEC_ADMINISTRATOR, Console::Yes },
            { "",               HandleSmartStoneCommand, SEC_PLAYER, Console::No },
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

    static bool HandleSmartStoneUnlockServiceCommand(ChatHandler* handler, PlayerIdentifier player, uint8 serviceType, uint32 id, bool add)
    {
        if (!sSmartstone->IsSmartstoneEnabled())
        {
            handler->SendErrorMessage("The smartstone is disabled.");
            return false;
        }

        std::string playerName;
        sCharacterCache->GetCharacterNameByGuid(player.GetGUID(), playerName);

        std::string module = sSmartstone->GetModuleStringForService(serviceType);

        Player* target = player.GetConnectedPlayer();

        auto sendDupError = [&](std::string_view desc) {
            handler->SendErrorMessage("The {} is already {}.", desc, add ? "unlocked" : "locked");
        };

        auto sendSuccess = [&](std::string_view desc) {
            handler->PSendSysMessage("{} has been {} for {}.", desc, add ? "unlocked" : "removed", playerName);
        };

        uint32 settingId = sSmartstone->GetShortId(id, serviceType);

        switch (serviceType)
        {
            case ACTION_TYPE_COMPANION:
            case ACTION_TYPE_PET:
            {
                SmartstonePetData pet = sSmartstone->GetPetData(id, serviceType);
                if (!pet.CreatureId)
                {
                    handler->SendErrorMessage("The pet {} does not exist.", id);
                    LOG_ERROR("smartstone", "HandleSmartStoneUnlockServiceCommand: The pet {} does not exist.", id);
                    return false;
                }

                if (target && target->GetPlayerSetting(module, settingId).IsEnabled() == add)
                {
                    sendDupError(pet.Description);
                    LOG_ERROR("smartstone", "HandleSmartStoneUnlockServiceCommand: The pet {} is already {} for player {}.", id, add ? "unlocked" : "locked", playerName);
                    return false;
                }

                if (target && add && pet.Duration)
                {
                    uint32 expiration = (pet.Duration > 31556926)
                        ? pet.Duration
                        : GameTime::GetGameTime().count() + pet.Duration;

                    CharacterDatabase.Execute(
                        "INSERT INTO smartstone_char_temp_services (PlayerGUID, ServiceId, Category, ActivationTime, ExpirationTime) VALUES ({}, {}, {}, UNIX_TIMESTAMP(), {})",
                        target->GetGUID().GetCounter(), pet.CreatureId, serviceType, expiration);

                    SmartstoneServiceExpireInfo expireInfo;
                    expireInfo.PlayerGUID = target->GetGUID().GetCounter();
                    expireInfo.ServiceId = pet.CreatureId;
                    expireInfo.ServiceType = serviceType;
                    expireInfo.ActivationTime = GameTime::GetGameTime().count();
                    expireInfo.ExpirationTime = expiration;
                    sSmartstone->ServiceExpireInfo[target->GetGUID().GetCounter()].push_back(expireInfo);
                }

                if (target)
                    target->UpdatePlayerSetting(module, settingId, add);
                else
                    PlayerSettingsStore::UpdateSetting(player.GetGUID().GetCounter(), module, settingId, add);

                sendSuccess(pet.Description);
                break;
            }

            case ACTION_TYPE_COSTUME:
            {
                SmartstoneCostumeData costume = sSmartstone->GetCostumeData(id);
                if (!costume.DisplayId)
                {
                    handler->SendErrorMessage("The costume {} does not exist.", id);
                    LOG_ERROR("smartstone", "HandleSmartStoneUnlockServiceCommand: The costume {} does not exist.", id);
                    return false;
                }

                uint32 accountId = sCharacterCache->GetCharacterAccountIdByGuid(player.GetGUID());

                if (sSmartstone->GetAccountSetting(accountId, serviceType, id).IsEnabled() == add)
                {
                    sendDupError(costume.Description);
                    LOG_ERROR("smartstone", "HandleSmartStoneUnlockServiceCommand: The costume {} is already {} for player {}.", id, add ? "unlocked" : "locked", playerName);
                    return false;
                }

                sSmartstone->UpdateAccountSetting(accountId, serviceType, settingId, add);
                sendSuccess(costume.Description);
                break;
            }

            case ACTION_TYPE_AURA:
            {
                SmartstoneAuraData aura = sSmartstone->GetAuraData(id);
                if (!aura.SpellID)
                {
                    handler->SendErrorMessage("The aura {} does not exist.", id);
                    LOG_ERROR("smartstone", "HandleSmartStoneUnlockServiceCommand: The aura {} does not exist.", id);
                    return false;
                }

                if (target && target->GetPlayerSetting(module, id).IsEnabled() == add)
                {
                    sendDupError(aura.Description);
                    LOG_ERROR("smartstone", "HandleSmartStoneUnlockServiceCommand: The aura {} is already {} for player {}.", id, add ? "unlocked" : "locked", playerName);
                    return false;
                }

                if (target)
                    target->UpdatePlayerSetting(module, id, add);
                else
                    PlayerSettingsStore::UpdateSetting(player.GetGUID().GetCounter(), module, id, add);

                sendSuccess(aura.Description);
                break;
            }
            case ACTION_TYPE_VEHICLES:
            {
                SmartstoneVehicleData vehicleData = sSmartstone->GetVehicleData(id);
                if (!vehicleData.CreatureId)
                {
                    handler->SendErrorMessage("The vehicle {} does not exist.", id);
                    LOG_ERROR("smartstone", "HandleSmartStoneUnlockServiceCommand: The vehicle {} does not exist.", id);
                    return false;
                }

                if (target && target->GetPlayerSetting(module, settingId).IsEnabled() == add)
                {
                    sendDupError(vehicleData.Description);
                    LOG_ERROR("smartstone", "HandleSmartStoneUnlockServiceCommand: The vehicleData {} is already {} for player {}.", id, add ? "unlocked" : "locked", playerName);
                    return false;
                }

                if (target)
                    target->UpdatePlayerSetting(module, settingId, add);
                else
                    PlayerSettingsStore::UpdateSetting(player.GetGUID().GetCounter(), module, settingId, add);

                sendSuccess(vehicleData.Description);
                break;
            }

            default:
                handler->SendErrorMessage("Unknown service type.");
                LOG_ERROR("smartstone", "HandleSmartStoneUnlockServiceCommand: Unknown service type {}.", serviceType);
                return false;
        }

        return true;
    }

    static bool HandleSmartstoneReloadCommand(ChatHandler* handler)
    {
        sSmartstone->LoadSmartstoneData();
        handler->SendSysMessage("Smartstone data reloaded.");
        return true;
    }
};

void AddSC_smartstone_commandscript()
{
    new smartstone_commandscript();
}
