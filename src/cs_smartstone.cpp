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

#include "AccountMgr.h"
#include "Chat.h"
#include "GameTime.h"
#include "Mail.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Smartstone.h"
#include "Timer.h"
#include "WorldSession.h"
#include "WorldSessionMgr.h"
#include <algorithm>
#include <unordered_set>

using namespace Acore::ChatCommands;

class smartstone_commandscript : public CommandScript
{
public:
    smartstone_commandscript() : CommandScript("smartstone_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable smartstoneLookupTable =
        {
            { "pets",     HandleSmartstoneLookupPetsCommand,     SEC_PLAYER,     Console::Yes },
            { "costumes", HandleSmartstoneLookupCostumesCommand, SEC_PLAYER,     Console::Yes },
            { "allpets",  HandleSmartstoneLookupAllPetsCommand,  SEC_GAMEMASTER, Console::Yes },
        };

        static ChatCommandTable smartstoneUseTable =
        {
            { "costume", HandleSmartStoneUseCostumeCommand, SEC_PLAYER, Console::No },
            { "pet",     HandleSmartStoneUsePetCommand,     SEC_PLAYER, Console::No },
        };

        static ChatCommandTable smartstoneCostumeTable =
        {
            { "convert", HandleSmartstoneCostumeConvertCommand, SEC_PLAYER, Console::No },
        };

        static ChatCommandTable smartstoneToggleTable =
        {
            { "costumes", HandleSmartstoneToggleCostumesCommand, SEC_PLAYER, Console::No },
            { "forms",    HandleSmartstoneToggleFormsCommand,    SEC_PLAYER, Console::No },
            { "minions",  HandleSmartstoneToggleMinionsCommand,  SEC_PLAYER, Console::No },
        };

        static ChatCommandTable smartstoneDebugTable =
        {
            { "costume", HandleSmartstoneDebugCostumeCommand, SEC_GAMEMASTER, Console::No },
        };

        static ChatCommandTable smartstoneTokenTable =
        {
            { "claim",  HandleSmartstoneTokenClaimCommand,  SEC_PLAYER,    Console::No  },
            { "grant",  HandleSmartstoneTokenGrantCommand,  SEC_MODERATOR, Console::Yes },
            { "list",   HandleSmartstoneTokenListCommand,   SEC_MODERATOR, Console::Yes },
            { "revoke", HandleSmartstoneTokenRevokeCommand, SEC_MODERATOR, Console::Yes },
        };

        static ChatCommandTable smartstoneTable =
        {
            { "unlock service", HandleSmartStoneUnlockServiceCommand, SEC_MODERATOR,     Console::Yes },
            { "unlock account", HandleSmartStoneUnlockAccountCommand, SEC_MODERATOR,     Console::Yes },
            { "reload",         HandleSmartstoneReloadCommand,        SEC_ADMINISTRATOR, Console::Yes },
            { "cooldowns",      HandleSmartstoneCooldownsCommand,     SEC_PLAYER,        Console::Yes },
            { "lookup",         smartstoneLookupTable },
            { "use",            smartstoneUseTable },
            { "costume",        smartstoneCostumeTable },
            { "toggle",         smartstoneToggleTable },
            { "debug",          smartstoneDebugTable },
            { "token",          smartstoneTokenTable },
            { "",               HandleSmartStoneCommand, SEC_PLAYER, Console::No },
        };

        static ChatCommandTable commandTable =
        {
            { "smartstone", smartstoneTable },
        };

        return commandTable;
    }

    static bool HandleSmartStoneUseCostumeCommand(ChatHandler* handler, uint32 id)
    {
        if (!sSmartstone->IsSmartstoneEnabled())
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetPlayer();

        SmartstoneCostumeData costume = sSmartstone->GetCostumeData(id);
        if (!costume.DisplayId)
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_COSTUME_NOT_EXIST, id);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint8 subscriptionLevel = player->IsGameMaster() ? 3 : player->GetPlayerSetting(SubsModName, SETTING_MEMBERSHIP_LEVEL).value;

        if (!sSmartstone->IsServiceAvailable(player, "#costume", id - 20000)
            && subscriptionLevel < costume.SubscriptionLevelRequired)
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_NO_ACCESS, costume.Description);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if ((!sSmartstone->IsSmartstoneCanUseInBG() && player->InBattleground()) ||
            (!sSmartstone->IsSmartstoneCanUseInArena() && player->InArena()))
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_COSTUME_NO_BG_ARENA);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!sSmartstone->IsSmartstoneCanUseInCombat() && player->IsInCombat())
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_COSTUME_NO_COMBAT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if ((!sSmartstone->IsSmartstoneCanUseInPvP() && player->IsInCombat() && player->IsPvP())
            || sSmartstone->IsBattlefieldUseBlocked(player))
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_COSTUME_NO_PVP);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (sSmartstone->HasCostumeCooldown(player, id) && !player->GetCommandStatus(CHEAT_COOLDOWN))
        {
            uint32 remaining = sSmartstone->GetCostumeCooldownRemaining(player, id);
            handler->PSendModuleSysMessage(ModName, LANG_MOD_COSTUME_ON_COOLDOWN, costume.Description, remaining / 60, remaining % 60);
            handler->SetSentErrorMessage(true);
            return false;
        }

        sSmartstone->ApplyCostume(player, id);
        handler->PSendModuleSysMessage(ModName, LANG_MOD_COSTUME_APPLIED, costume.Description);
        return true;
    }

    // Flip one per-player hide-X view preference (shared by the toggle cmds).
    static bool ToggleDisplayPreference(ChatHandler* handler, uint32 settingSlot, uint32 hiddenMsg, uint32 shownMsg)
    {
        if (!sSmartstone->IsDisplayOptOutEnabled())
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetPlayer();
        if (!player)
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_CONSOLE_NEEDS_PLAYER);
            handler->SetSentErrorMessage(true);
            return false;
        }

        bool const nowHidden = player->GetPlayerSetting(ModName + "#misc", settingSlot).value == 0;
        player->UpdatePlayerSetting(ModName + "#misc", settingSlot, nowHidden ? 1 : 0);
        sSmartstone->RefreshSmartstoneVisibilityFor(player);
        handler->PSendModuleSysMessage(ModName, nowHidden ? hiddenMsg : shownMsg);
        return true;
    }

    static bool HandleSmartstoneToggleCostumesCommand(ChatHandler* handler)
    {
        return ToggleDisplayPreference(handler, SETTING_HIDE_COSTUMES, LANG_MOD_COSTUMES_NOW_HIDDEN, LANG_MOD_COSTUMES_NOW_SHOWN);
    }

    static bool HandleSmartstoneToggleFormsCommand(ChatHandler* handler)
    {
        return ToggleDisplayPreference(handler, SETTING_HIDE_FORMS, LANG_MOD_FORMS_NOW_HIDDEN, LANG_MOD_FORMS_NOW_SHOWN);
    }

    static bool HandleSmartstoneToggleMinionsCommand(ChatHandler* handler)
    {
        return ToggleDisplayPreference(handler, SETTING_HIDE_MINIONS, LANG_MOD_MINIONS_NOW_HIDDEN, LANG_MOD_MINIONS_NOW_SHOWN);
    }

    static bool HandleSmartStoneUsePetCommand(ChatHandler* handler, uint32 id)
    {
        if (!sSmartstone->IsSmartstoneEnabled())
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetPlayer();

        // Search companions first, then combat pets
        SmartstonePetData pet = sSmartstone->GetPetData(id, ACTION_TYPE_COMPANION);
        bool isCombat = false;

        if (!pet.CreatureId)
        {
            pet = sSmartstone->GetPetData(id, ACTION_TYPE_PET);
            isCombat = true;
        }

        if (!pet.CreatureId)
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_PET_NOT_EXIST, id);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint8 subscriptionLevel = player->IsGameMaster() ? 3 : player->GetPlayerSetting(SubsModName, SETTING_MEMBERSHIP_LEVEL).value;

        if (!sSmartstone->IsPetAvailable(player, pet, subscriptionLevel))
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_NO_ACCESS, pet.Description);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (isCombat && !sSmartstone->CanUseSmartstone(player))
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_NO_COMBAT_PET_INSTANCE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (isCombat && sSmartstone->IsBattlefieldUseBlocked(player))
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_NO_PVP);
            handler->SetSentErrorMessage(true);
            return false;
        }

        player->CastCustomSpell(isCombat ? 90001 : 90000, SPELLVALUE_MISCVALUE0, id);
        return true;
    }

    static bool HandleSmartstoneLookupPetsCommand(ChatHandler* handler, Optional<PlayerIdentifier> target, Optional<std::string_view> filter)
    {
        Player* self = handler->GetPlayer();
        bool isGM = self && self->GetSession()->GetSecurity() >= SEC_GAMEMASTER;

        if (!self)
        {
            if (!target)
            {
                handler->PSendModuleSysMessage(ModName, LANG_MOD_CONSOLE_NEEDS_PLAYER);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }
        else if (!isGM)
        {
            target = PlayerIdentifier::FromSelf(handler);
        }
        else if (!target)
        {
            target = PlayerIdentifier::FromSelf(handler);
        }

        std::string playerName;
        sCharacterCache->GetCharacterNameByGuid(target->GetGUID(), playerName);

        std::string filterStr = filter ? std::string(*filter) : "";
        std::transform(filterStr.begin(), filterStr.end(), filterStr.begin(), ::tolower);

        bool found = false;
        Player* player = target->GetConnectedPlayer();

        if (player)
        {
            uint8 subscriptionLevel = player->IsGameMaster() ? 3 : player->GetPlayerSetting(SubsModName, SETTING_MEMBERSHIP_LEVEL).value;

            auto checkAndPrint = [&](SmartstonePetData const& pet, std::string_view typeStr)
            {
                if (!sSmartstone->IsPetAvailable(player, pet, subscriptionLevel))
                    return;

                std::string descLower = pet.Description;
                std::transform(descLower.begin(), descLower.end(), descLower.begin(), ::tolower);

                if (!filterStr.empty() && descLower.find(filterStr) == std::string::npos)
                    return;

                handler->PSendModuleSysMessage(ModName, LANG_MOD_LOOKUP_PET_FORMAT, pet.CreatureId, pet.Description, typeStr);
                found = true;
            };

            for (auto const& pet : sSmartstone->Pets)
                checkAndPrint(pet, "Companion");

            for (auto const& pet : sSmartstone->CombatPets)
                checkAndPrint(pet, "Combat Pet");
        }
        else
        {
            uint32 guidLow = target->GetGUID().GetCounter();

            uint8 subscriptionLevel = 0;
            PlayerSettingVector petSettings;
            PlayerSettingVector combatPetSettings;
            PlayerSettingVector zoneDifficultySettings;

            QueryResult settingsResult = CharacterDatabase.Query(
                "SELECT `source`, `data` FROM `character_settings` WHERE `guid` = {} AND `source` IN ('{}', '{}', '{}', '{}')",
                guidLow,
                SubsModName,
                ModName + "#pet",
                ModName + "#combatpet",
                "mod-zone-difficulty#ct");

            if (settingsResult)
            {
                do
                {
                    Field* fields = settingsResult->Fetch();
                    std::string source = fields[0].Get<std::string>();
                    std::string data = fields[1].Get<std::string>();
                    PlayerSettingVector settings = PlayerSettingsStore::ParseSettingsData(data);

                    if (source == SubsModName)
                    {
                        if (SETTING_MEMBERSHIP_LEVEL < settings.size())
                            subscriptionLevel = settings[SETTING_MEMBERSHIP_LEVEL].value;
                    }
                    else if (source == ModName + "#pet")
                        petSettings = settings;
                    else if (source == ModName + "#combatpet")
                        combatPetSettings = settings;
                    else if (source == "mod-zone-difficulty#ct")
                        zoneDifficultySettings = settings;
                } while (settingsResult->NextRow());
            }

            std::unordered_set<uint32> activeTempServices;
            QueryResult tempResult = CharacterDatabase.Query(
                "SELECT `ServiceId` FROM `smartstone_char_temp_services` WHERE `PlayerGUID` = {} AND `ExpirationTime` >= {}",
                guidLow, GameTime::GetGameTime().count());

            if (tempResult)
            {
                do
                {
                    activeTempServices.insert(tempResult->Fetch()[0].Get<uint32>());
                } while (tempResult->NextRow());
            }

            auto checkAndPrintOffline = [&](SmartstonePetData const& pet, std::string_view typeStr)
            {
                bool available = false;

                if (pet.Duration && activeTempServices.count(pet.CreatureId))
                    available = true;

                if (!available && pet.SubscriptionLevelRequired && subscriptionLevel >= pet.SubscriptionLevelRequired)
                    available = true;

                if (!available && pet.Type == PET_TYPE_COMPANION)
                {
                    switch (pet.CreatureId)
                    {
                        case 80002:
                            available = (SETTING_HYJAL < zoneDifficultySettings.size() && zoneDifficultySettings[SETTING_HYJAL].IsEnabled());
                            break;
                        case 80003:
                            available = (SETTING_SSC < zoneDifficultySettings.size() && zoneDifficultySettings[SETTING_SSC].IsEnabled());
                            break;
                        case 80006:
                            available = (SETTING_ZULAMAN < zoneDifficultySettings.size() && zoneDifficultySettings[SETTING_ZULAMAN].IsEnabled());
                            break;
                        case 80010:
                            available = (SETTING_SWP < zoneDifficultySettings.size() && zoneDifficultySettings[SETTING_SWP].IsEnabled());
                            break;
                        case 80011:
                            available = (SETTING_BLACK_TEMPLE < zoneDifficultySettings.size() && zoneDifficultySettings[SETTING_BLACK_TEMPLE].IsEnabled());
                            break;
                    }
                }

                if (!available)
                {
                    uint32 shortId = pet.GetId();
                    PlayerSettingVector const& settings = (pet.Type == PET_TYPE_COMBAT) ? combatPetSettings : petSettings;
                    if (shortId < settings.size() && settings[shortId].IsEnabled())
                        available = true;
                }

                if (!available)
                    return;

                std::string descLower = pet.Description;
                std::transform(descLower.begin(), descLower.end(), descLower.begin(), ::tolower);

                if (!filterStr.empty() && descLower.find(filterStr) == std::string::npos)
                    return;

                handler->PSendModuleSysMessage(ModName, LANG_MOD_LOOKUP_PET_FORMAT, pet.CreatureId, pet.Description, typeStr);
                found = true;
            };

            for (auto const& pet : sSmartstone->Pets)
                checkAndPrintOffline(pet, "Companion");

            for (auto const& pet : sSmartstone->CombatPets)
                checkAndPrintOffline(pet, "Combat Pet");
        }

        if (!found)
        {
            if (filterStr.empty())
                handler->PSendModuleSysMessage(ModName, LANG_MOD_NO_PETS_AVAILABLE, playerName);
            else
                handler->PSendModuleSysMessage(ModName, LANG_MOD_NO_PETS_MATCHING_FOR, filterStr, playerName);
        }

        return true;
    }

    static bool HandleSmartstoneLookupCostumesCommand(ChatHandler* handler, Optional<PlayerIdentifier> target, Optional<std::string_view> filter)
    {
        Player* self = handler->GetPlayer();
        bool isGM = self && self->GetSession()->GetSecurity() >= SEC_GAMEMASTER;

        if (!self)
        {
            if (!target)
            {
                handler->PSendModuleSysMessage(ModName, LANG_MOD_CONSOLE_NEEDS_PLAYER);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }
        else if (!isGM)
        {
            target = PlayerIdentifier::FromSelf(handler);
        }
        else if (!target)
        {
            target = PlayerIdentifier::FromSelf(handler);
        }

        std::string playerName;
        sCharacterCache->GetCharacterNameByGuid(target->GetGUID(), playerName);

        std::string filterStr = filter ? std::string(*filter) : "";
        std::transform(filterStr.begin(), filterStr.end(), filterStr.begin(), ::tolower);

        bool found = false;
        Player* player = target->GetConnectedPlayer();

        if (player)
        {
            uint8 subscriptionLevel = player->IsGameMaster() ? 3 : player->GetPlayerSetting(SubsModName, SETTING_MEMBERSHIP_LEVEL).value;

            for (auto const& [category, costumeList] : sSmartstone->Costumes)
            {
                for (auto const& costume : costumeList)
                {
                    if (!sSmartstone->IsServiceAvailable(player, "#costume", costume.Id - 20000)
                        && subscriptionLevel < costume.SubscriptionLevelRequired)
                        continue;

                    std::string descLower = costume.Description;
                    std::transform(descLower.begin(), descLower.end(), descLower.begin(), ::tolower);

                    if (!filterStr.empty() && descLower.find(filterStr) == std::string::npos)
                        continue;

                    handler->PSendModuleSysMessage(ModName, LANG_MOD_LOOKUP_COSTUME_FORMAT, costume.Id, costume.Description);
                    found = true;
                }
            }
        }
        else
        {
            uint32 accountId = sCharacterCache->GetCharacterAccountIdByGuid(target->GetGUID());

            sSmartstone->LoadAccountSettings(accountId);

            uint8 subscriptionLevel = 0;
            QueryResult subResult = CharacterDatabase.Query(
                "SELECT `data` FROM `character_settings` WHERE `guid` = {} AND `source` = '{}'",
                target->GetGUID().GetCounter(), SubsModName);

            if (subResult)
            {
                PlayerSettingVector subSettings = PlayerSettingsStore::ParseSettingsData(subResult->Fetch()[0].Get<std::string>());
                if (SETTING_MEMBERSHIP_LEVEL < subSettings.size())
                    subscriptionLevel = subSettings[SETTING_MEMBERSHIP_LEVEL].value;
            }

            for (auto const& [category, costumeList] : sSmartstone->Costumes)
            {
                for (auto const& costume : costumeList)
                {
                    if (!sSmartstone->GetAccountSetting(accountId, ACTION_TYPE_COSTUME, costume.Id - 20000).IsEnabled()
                        && subscriptionLevel < costume.SubscriptionLevelRequired)
                        continue;

                    std::string descLower = costume.Description;
                    std::transform(descLower.begin(), descLower.end(), descLower.begin(), ::tolower);

                    if (!filterStr.empty() && descLower.find(filterStr) == std::string::npos)
                        continue;

                    handler->PSendModuleSysMessage(ModName, LANG_MOD_LOOKUP_COSTUME_FORMAT, costume.Id, costume.Description);
                    found = true;
                }
            }
        }

        if (!found)
        {
            if (filterStr.empty())
                handler->PSendModuleSysMessage(ModName, LANG_MOD_NO_COSTUMES_AVAILABLE, playerName);
            else
                handler->PSendModuleSysMessage(ModName, LANG_MOD_NO_COSTUMES_MATCHING_FOR, filterStr, playerName);
        }

        return true;
    }

    static bool HandleSmartstoneCooldownsCommand(ChatHandler* handler, Optional<PlayerIdentifier> target)
    {
        if (!sSmartstone->IsSmartstoneEnabled())
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* self = handler->GetPlayer();
        bool isGM = self && self->GetSession()->GetSecurity() >= SEC_GAMEMASTER;

        if (!self)
        {
            if (!target)
            {
                handler->PSendModuleSysMessage(ModName, LANG_MOD_CONSOLE_NEEDS_PLAYER);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }
        else if (!isGM)
        {
            target = PlayerIdentifier::FromSelf(handler);
        }
        else if (!target)
        {
            target = PlayerIdentifier::FromSelf(handler);
        }

        Player* player = target->GetConnectedPlayer();

        std::string playerName;
        sCharacterCache->GetCharacterNameByGuid(target->GetGUID(), playerName);

        bool found = false;

        if (sSmartstone->HasIndividualCostumeCooldowns())
        {
            if (player)
            {
                bool filterByAccess = !isGM && self;
                uint8 subscriptionLevel = 0;
                if (filterByAccess)
                    subscriptionLevel = player->GetPlayerSetting(SubsModName, SETTING_MEMBERSHIP_LEVEL).value;

                for (auto const& [category, costumeList] : sSmartstone->Costumes)
                {
                    for (auto const& costume : costumeList)
                    {
                        if (filterByAccess
                            && !sSmartstone->IsServiceAvailable(player, "#costume", costume.Id - 20000)
                            && subscriptionLevel < costume.SubscriptionLevelRequired)
                            continue;

                        if (sSmartstone->HasCostumeCooldown(player, costume.Id))
                        {
                            uint32 remaining = sSmartstone->GetCostumeCooldownRemaining(player, costume.Id);
                            handler->PSendModuleSysMessage(ModName, LANG_MOD_COOLDOWN_REMAINING, costume.Id, costume.Description, remaining / 60, remaining % 60);
                            found = true;
                        }
                    }
                }
            }
            else
            {
                QueryResult result = CharacterDatabase.Query(
                    "SELECT `data` FROM `character_settings` WHERE `guid` = {} AND `source` = 'mod-cc-smartstone#ccd'",
                    target->GetGUID().GetCounter());

                if (result)
                {
                    std::string data = result->Fetch()[0].Get<std::string>();
                    PlayerSettingVector settings = PlayerSettingsStore::ParseSettingsData(data);
                    uint32 now = GameTime::GetGameTime().count();

                    for (auto const& [category, costumeList] : sSmartstone->Costumes)
                    {
                        for (auto const& costume : costumeList)
                        {
                            if (costume.Id < settings.size())
                            {
                                uint32 expireTime = settings[costume.Id].value;
                                if (expireTime > now)
                                {
                                    uint32 remaining = expireTime - now;
                                    handler->PSendModuleSysMessage(ModName, LANG_MOD_COOLDOWN_REMAINING, costume.Id, costume.Description, remaining / 60, remaining % 60);
                                    found = true;
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            if (player)
            {
                if (player->HasSpellCooldown(90002))
                {
                    uint32 remaining = player->GetSpellCooldownDelay(90002) / 1000;
                    handler->PSendModuleSysMessage(ModName, LANG_MOD_ALL_COOLDOWN_REMAINING, remaining / 60, remaining % 60);
                    found = true;
                }
            }
            else
            {
                handler->PSendModuleSysMessage(ModName, LANG_MOD_GLOBAL_CD_OFFLINE);
                return true;
            }
        }

        if (!found)
            handler->PSendModuleSysMessage(ModName, LANG_MOD_NO_COSTUME_COOLDOWNS, playerName);

        return true;
    }

    static bool HandleSmartstoneLookupAllPetsCommand(ChatHandler* handler, Optional<std::string_view> filter)
    {
        std::string filterStr = filter ? std::string(*filter) : "";
        std::transform(filterStr.begin(), filterStr.end(), filterStr.begin(), ::tolower);

        bool found = false;

        auto checkAndPrint = [&](SmartstonePetData const& pet, std::string_view typeStr)
        {
            std::string descLower = pet.Description;
            std::transform(descLower.begin(), descLower.end(), descLower.begin(), ::tolower);

            if (!filterStr.empty() && descLower.find(filterStr) == std::string::npos)
                return;

            handler->PSendModuleSysMessage(ModName, LANG_MOD_LOOKUP_PET_FORMAT, pet.CreatureId, pet.Description, typeStr);
            found = true;
        };

        for (auto const& pet : sSmartstone->Pets)
            checkAndPrint(pet, "Companion");

        for (auto const& pet : sSmartstone->CombatPets)
            checkAndPrint(pet, "Combat Pet");

        if (!found)
        {
            if (filterStr.empty())
                handler->PSendModuleSysMessage(ModName, LANG_MOD_NO_PETS_LOADED);
            else
                handler->PSendModuleSysMessage(ModName, LANG_MOD_NO_PETS_MATCHING, filterStr);
        }

        return true;
    }

    static bool HandleSmartstoneCostumeConvertCommand(ChatHandler* handler)
    {
        if (!sSmartstone->IsSmartstoneEnabled())
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!sSmartstone->IsCostumeConvertEnabled())
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_CONVERT_UNAVAILABLE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (sSmartstone->LegacyCostumeItemToDisplayId.empty())
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_NO_LEGACY_COSTUMES);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetPlayer();
        uint32 accountId = player->GetSession()->GetAccountId();

        sSmartstone->LoadAccountSettings(accountId);

        bool foundAny = false;

        for (auto const& [itemEntry, displayId] : sSmartstone->LegacyCostumeItemToDisplayId)
        {
            if (!player->HasItemCount(itemEntry, 1))
                continue;

            foundAny = true;

            ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(itemEntry);
            std::string itemName = itemTemplate ? itemTemplate->Name1 : std::to_string(itemEntry);

            SmartstoneCostumeData newCostume = sSmartstone->GetCostumeDataByDisplayId(displayId);
            if (!newCostume.Id)
            {
                handler->PSendModuleSysMessage(ModName, LANG_MOD_NO_MATCHING_COSTUME, itemName, displayId);
                continue;
            }

            uint32 settingId = newCostume.Id - 20000;

            player->DestroyItemCount(itemEntry, 1, true);

            if (sSmartstone->GetAccountSetting(accountId, ACTION_TYPE_COSTUME, settingId).IsEnabled())
            {
                handler->PSendModuleSysMessage(ModName, LANG_MOD_ITEM_ALREADY_UNLOCKED, newCostume.Description);
                continue;
            }

            sSmartstone->UpdateAccountSetting(accountId, ACTION_TYPE_COSTUME, settingId, 1);
            handler->PSendModuleSysMessage(ModName, LANG_MOD_CONVERTED, itemName, newCostume.Description);
        }

        if (!foundAny)
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_NO_CONVERTIBLE);
            return true;
        }

        return true;
    }

    static bool HandleSmartStoneCommand(ChatHandler* handler)
    {
        if (!sSmartstone->IsSmartstoneEnabled())
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_DISABLED);
            handler->SetSentErrorMessage(true);
            return true;
        }

        if (Player* player = handler->GetPlayer())
        {
            if (!player->HasItemCount(sSmartstone->GetSmartstoneItemID(), 1))
                player->AddItem(sSmartstone->GetSmartstoneItemID(), 1);
            else
                handler->PSendModuleSysMessage(ModName, LANG_MOD_ALREADY_HAVE_STONE);
        }

        return true;
    }

    // Resolve the locale to use for module strings sent to a given target
    // character. Falls back to the DBC default for offline players.
    static LocaleConstant GetLocaleForCharacter(ObjectGuid::LowType lowGuid)
    {
        if (Player* p = ObjectAccessor::FindPlayerByLowGUID(lowGuid))
            if (WorldSession* s = p->GetSession())
                return s->GetSessionDbLocaleIndex();
        return LocaleConstant(sObjectMgr->GetDBCLocaleIndex());
    }

    // Send the "you have unlocked X" notification (chat + optional mail) to a
    // single target character. Safe for offline players: chat is silently
    // skipped, mail is sent via low-guid so it lands when they next log in.
    // Mail is gated by the ModChromiecraftSmartstone.UnlockMail.Enable config.
    static void NotifyUnlock(ObjectGuid::LowType lowGuid, std::string_view typeName, std::string_view serviceName)
    {
        Player* connected = ObjectAccessor::FindPlayerByLowGUID(lowGuid);
        if (connected)
        {
            ChatHandler(connected->GetSession()).PSendModuleSysMessage(
                ModName, LANG_MOD_SERVICE_UNLOCK_NOTIFY, typeName, serviceName);
        }

        if (!sSmartstone->IsMailUnlocksEnabled())
            return;

        LocaleConstant locale = GetLocaleForCharacter(lowGuid);
        std::string const* subjectFmt = sObjectMgr->GetModuleString(ModName, LANG_MOD_SERVICE_UNLOCK_MAIL_SUBJ, locale);
        std::string const* bodyFmt    = sObjectMgr->GetModuleString(ModName, LANG_MOD_SERVICE_UNLOCK_MAIL_BODY, locale);
        if (!subjectFmt || !bodyFmt)
            return;

        std::string subject = Acore::StringFormat(subjectFmt->c_str(), typeName, serviceName);
        std::string body    = Acore::StringFormat(bodyFmt->c_str(), typeName, serviceName);

        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
        MailDraft(std::move(subject), std::move(body))
            .SendMailTo(trans, MailReceiver(connected, lowGuid),
                MailSender(MAIL_NORMAL, 0, MAIL_STATIONERY_GM));
        CharacterDatabase.CommitTransaction(trans);
    }

    // Account-wide variant: fan out NotifyUnlock to every character on the
    // account, so all of them learn about the unlock (the underlying setting
    // is shared, so each character benefits equally).
    static void NotifyUnlockForAccount(uint32 accountId, std::string_view typeName, std::string_view serviceName)
    {
        QueryResult result = CharacterDatabase.Query(
            "SELECT guid FROM characters WHERE account = {}", accountId);
        if (!result)
            return;

        do
        {
            NotifyUnlock(result->Fetch()[0].Get<uint32>(), typeName, serviceName);
        } while (result->NextRow());
    }

    static bool HandleSmartStoneUnlockServiceCommand(ChatHandler* handler, PlayerIdentifier player, uint8 serviceType, uint32 id, bool add)
    {
        if (!sSmartstone->IsSmartstoneEnabled())
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::string playerName;
        sCharacterCache->GetCharacterNameByGuid(player.GetGUID(), playerName);

        std::string module = sSmartstone->GetModuleStringForService(serviceType);

        Player* target = player.GetConnectedPlayer();

        auto sendDupError = [&](std::string_view desc) {
            handler->PSendModuleSysMessage(ModName,
                add ? LANG_MOD_SERVICE_ALREADY_UNLOCKED : LANG_MOD_SERVICE_ALREADY_LOCKED,
                desc);
            handler->SetSentErrorMessage(true);
        };

        auto sendSuccess = [&](std::string_view desc) {
            handler->PSendModuleSysMessage(ModName,
                add ? LANG_MOD_SERVICE_BEEN_UNLOCKED : LANG_MOD_SERVICE_BEEN_REMOVED,
                desc, playerName);

            if (!add)
                return;

            std::string typeName = sSmartstone->GetActionTypeName(static_cast<ActionType>(serviceType));

            // Costumes and perks are account-wide — notify every character
            // on the owning account so they all see the unlock. Everything
            // else is per-character.
            if (serviceType == ACTION_TYPE_COSTUME || serviceType == ACTION_TYPE_PERK)
                NotifyUnlockForAccount(sCharacterCache->GetCharacterAccountIdByGuid(player.GetGUID()), typeName, desc);
            else
                NotifyUnlock(player.GetGUID().GetCounter(), typeName, desc);
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
                    handler->PSendModuleSysMessage(ModName, LANG_MOD_PET_SERVICE_NOT_EXIST, id);
                    handler->SetSentErrorMessage(true);
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
                    handler->PSendModuleSysMessage(ModName, LANG_MOD_COSTUME_SERVICE_NOT_EXIST, id);
                    handler->SetSentErrorMessage(true);
                    LOG_ERROR("smartstone", "HandleSmartStoneUnlockServiceCommand: The costume {} does not exist.", id);
                    return false;
                }

                uint32 accountId = sCharacterCache->GetCharacterAccountIdByGuid(player.GetGUID());

                sSmartstone->LoadAccountSettings(accountId);

                if (sSmartstone->GetAccountSetting(accountId, serviceType, id).IsEnabled() == add)
                {
                    sendDupError(costume.Description);
                    LOG_ERROR("smartstone", "HandleSmartStoneUnlockServiceCommand: The costume {} is already {} for player {}.", id, add ? "unlocked" : "locked", playerName);
                    return false;
                }

                sSmartstone->UpdateAccountSetting(accountId, serviceType, settingId, add);
                sendSuccess(costume.Description);

                if (!target)
                    sSmartstone->ClearAccountSettings(accountId);
                break;
            }

            case ACTION_TYPE_AURA:
            {
                SmartstoneAuraData aura = sSmartstone->GetAuraData(id);
                if (!aura.SpellID)
                {
                    handler->PSendModuleSysMessage(ModName, LANG_MOD_AURA_NOT_EXIST, id);
                    handler->SetSentErrorMessage(true);
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
            case ACTION_TYPE_VEHICLE:
            {
                SmartstoneVehicleData vehicleData = sSmartstone->GetVehicleData(id);
                if (!vehicleData.CreatureId)
                {
                    handler->PSendModuleSysMessage(ModName, LANG_MOD_VEHICLE_NOT_EXIST, id);
                    handler->SetSentErrorMessage(true);
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

            case ACTION_TYPE_PERK:
            {
                SmartstonePerkData perk = sSmartstone->GetPerkData(id);
                if (!perk.Id)
                {
                    handler->PSendModuleSysMessage(ModName, LANG_MOD_UNKNOWN_SERVICE_TYPE);
                    handler->SetSentErrorMessage(true);
                    LOG_ERROR("smartstone", "HandleSmartStoneUnlockServiceCommand: The perk {} does not exist.", id);
                    return false;
                }

                uint32 perkSlot = Smartstone::GetPerkAccountSettingForClass(perk.ClassId);
                if (!perkSlot)
                {
                    handler->PSendModuleSysMessage(ModName, LANG_MOD_UNKNOWN_SERVICE_TYPE);
                    handler->SetSentErrorMessage(true);
                    LOG_ERROR("smartstone", "HandleSmartStoneUnlockServiceCommand: Perk {} has unsupported ClassId {}.", id, perk.ClassId);
                    return false;
                }

                uint32 accountId = sCharacterCache->GetCharacterAccountIdByGuid(player.GetGUID());
                sSmartstone->LoadAccountSettings(accountId);

                if (sSmartstone->GetAccountSetting(accountId, perkSlot, perk.Id).IsEnabled() == add)
                {
                    sendDupError(perk.Title);
                    LOG_ERROR("smartstone", "HandleSmartStoneUnlockServiceCommand: The perk {} is already {} for player {}.", id, add ? "unlocked" : "locked", playerName);
                    return false;
                }

                sSmartstone->UpdateAccountSetting(accountId, perkSlot, perk.Id, add);
                sendSuccess(perk.Title);

                if (!target)
                    sSmartstone->ClearAccountSettings(accountId);
                break;
            }

            default:
                handler->PSendModuleSysMessage(ModName, LANG_MOD_UNKNOWN_SERVICE_TYPE);
                handler->SetSentErrorMessage(true);
                LOG_ERROR("smartstone", "HandleSmartStoneUnlockServiceCommand: Unknown service type {}.", serviceType);
                return false;
        }

        return true;
    }

    static bool HandleSmartStoneUnlockAccountCommand(ChatHandler* handler, AccountIdentifier account, uint8 serviceType, uint32 id, bool add)
    {
        if (!sSmartstone->IsSmartstoneEnabled())
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 accountId = account.GetID();
        std::string const& accountName = account.GetName();

        auto sendDupError = [&](std::string_view desc) {
            handler->PSendModuleSysMessage(ModName,
                add ? LANG_MOD_SERVICE_ALREADY_UNLOCKED : LANG_MOD_SERVICE_ALREADY_LOCKED,
                desc);
            handler->SetSentErrorMessage(true);
        };

        auto sendSuccess = [&](std::string_view desc) {
            handler->PSendModuleSysMessage(ModName,
                add ? LANG_MOD_SERVICE_BEEN_UNLOCKED : LANG_MOD_SERVICE_BEEN_REMOVED,
                desc, accountName);

            if (add)
                NotifyUnlockForAccount(accountId,
                    sSmartstone->GetActionTypeName(static_cast<ActionType>(serviceType)), desc);
        };

        sSmartstone->LoadAccountSettings(accountId);

        switch (serviceType)
        {
            case ACTION_TYPE_COSTUME:
            {
                SmartstoneCostumeData costume = sSmartstone->GetCostumeData(id);
                if (!costume.DisplayId)
                {
                    handler->PSendModuleSysMessage(ModName, LANG_MOD_COSTUME_SERVICE_NOT_EXIST, id);
                    handler->SetSentErrorMessage(true);
                    LOG_ERROR("smartstone", "HandleSmartStoneUnlockAccountCommand: The costume {} does not exist.", id);
                    return false;
                }

                uint32 settingId = sSmartstone->GetShortId(id, serviceType);

                if (sSmartstone->GetAccountSetting(accountId, serviceType, id).IsEnabled() == add)
                {
                    sendDupError(costume.Description);
                    LOG_ERROR("smartstone", "HandleSmartStoneUnlockAccountCommand: The costume {} is already {} for account {}.", id, add ? "unlocked" : "locked", accountName);
                    return false;
                }

                sSmartstone->UpdateAccountSetting(accountId, serviceType, settingId, add);
                sendSuccess(costume.Description);
                break;
            }

            case ACTION_TYPE_PERK:
            {
                SmartstonePerkData perk = sSmartstone->GetPerkData(id);
                if (!perk.Id)
                {
                    handler->PSendModuleSysMessage(ModName, LANG_MOD_UNKNOWN_SERVICE_TYPE);
                    handler->SetSentErrorMessage(true);
                    LOG_ERROR("smartstone", "HandleSmartStoneUnlockAccountCommand: The perk {} does not exist.", id);
                    return false;
                }

                uint32 perkSlot = Smartstone::GetPerkAccountSettingForClass(perk.ClassId);
                if (!perkSlot)
                {
                    handler->PSendModuleSysMessage(ModName, LANG_MOD_UNKNOWN_SERVICE_TYPE);
                    handler->SetSentErrorMessage(true);
                    LOG_ERROR("smartstone", "HandleSmartStoneUnlockAccountCommand: Perk {} has unsupported ClassId {}.", id, perk.ClassId);
                    return false;
                }

                if (sSmartstone->GetAccountSetting(accountId, perkSlot, perk.Id).IsEnabled() == add)
                {
                    sendDupError(perk.Title);
                    LOG_ERROR("smartstone", "HandleSmartStoneUnlockAccountCommand: The perk {} is already {} for account {}.", id, add ? "unlocked" : "locked", accountName);
                    return false;
                }

                sSmartstone->UpdateAccountSetting(accountId, perkSlot, perk.Id, add);
                sendSuccess(perk.Title);
                break;
            }

            default:
                handler->PSendModuleSysMessage(ModName, LANG_MOD_UNKNOWN_SERVICE_TYPE);
                handler->SetSentErrorMessage(true);
                LOG_ERROR("smartstone", "HandleSmartStoneUnlockAccountCommand: Service type {} is not account-wide. Use 'unlock service' instead.", serviceType);
                return false;
        }

        if (!sWorldSessionMgr->FindSession(accountId))
            sSmartstone->ClearAccountSettings(accountId);

        return true;
    }

    // Localized display name for a token type in the given locale, or a
    // plain fallback if the module string is missing.
    static std::string GetTokenName(uint8 tokenType, LocaleConstant locale)
    {
        if (uint32 stringId = Smartstone::GetTokenNameStringId(tokenType))
            if (std::string const* name = sObjectMgr->GetModuleString(ModName, stringId, locale))
                return *name;
        return std::to_string(tokenType);
    }

    static bool HandleSmartstoneTokenClaimCommand(ChatHandler* handler, uint32 tokenId)
    {
        if (!sSmartstone->IsSmartstoneEnabled())
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetPlayer();
        if (!player)
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_CONSOLE_NEEDS_PLAYER);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // No combat / BG / instance gate: claiming only sets an at-login flag
        // that applies at the character screen, so it is safe anywhere.
        uint32 accountId = player->GetSession()->GetAccountId();

        // Resolve the type before consuming so the ack can name it.
        uint8 tokenType = TOKEN_NONE;
        for (auto const& token : sSmartstone->GetAccountTokens(accountId))
            if (token.Id == tokenId)
            {
                tokenType = token.Type;
                break;
            }

        if (!sSmartstone->ConsumeToken(tokenId, accountId, player))
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_TOKEN_INVALID);
            handler->SetSentErrorMessage(true);
            LOG_ERROR("smartstone", "HandleSmartstoneTokenClaimCommand: token {} not claimable for account {} (character {}).",
                tokenId, accountId, player->GetGUID().ToString());
            return false;
        }

        LocaleConstant locale = player->GetSession()->GetSessionDbLocaleIndex();
        handler->PSendModuleSysMessage(ModName, LANG_MOD_TOKEN_APPLIED, GetTokenName(tokenType, locale));
        return true;
    }

    static bool HandleSmartstoneTokenGrantCommand(ChatHandler* handler, AccountIdentifier account, uint8 tokenType)
    {
        if (!sSmartstone->IsSmartstoneEnabled())
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (tokenType == TOKEN_NONE || tokenType >= MAX_TOKEN_TYPE)
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_TOKEN_INVALID_TYPE);
            handler->SetSentErrorMessage(true);
            LOG_ERROR("smartstone", "HandleSmartstoneTokenGrantCommand: invalid token type {}.", tokenType);
            return false;
        }

        uint32 accountId = account.GetID();

        uint32 grantedBy = 0;
        if (Player* gm = handler->GetPlayer())
            grantedBy = gm->GetSession()->GetAccountId();

        uint32 tokenId = sSmartstone->GrantToken(accountId, tokenType, grantedBy);

        LocaleConstant handlerLocale = LocaleConstant(handler->GetSessionDbLocaleIndex());
        handler->PSendModuleSysMessage(ModName, LANG_MOD_TOKEN_GRANTED,
            GetTokenName(tokenType, handlerLocale), account.GetName());

        // Best-effort notify: an account has at most one online session, so
        // ping it in its own locale with the same reminder shown on login (path
        // + claim command). Offline recipients get it on next login instead.
        if (WorldSession* session = sWorldSessionMgr->FindSession(accountId))
        {
            LocaleConstant sessionLocale = session->GetSessionDbLocaleIndex();
            ChatHandler(session).PSendModuleSysMessage(ModName, LANG_MOD_TOKEN_LOGIN_NOTICE,
                GetTokenName(tokenType, sessionLocale), tokenId);
        }

        return true;
    }

    static bool HandleSmartstoneTokenListCommand(ChatHandler* handler, AccountIdentifier account, Optional<std::string_view> mode)
    {
        uint32 accountId = account.GetID();

        bool includeClaimed = false;
        if (mode)
        {
            std::string m(*mode);
            std::transform(m.begin(), m.end(), m.begin(), ::tolower);
            includeClaimed = (m == "all");
        }

        LocaleConstant handlerLocale = LocaleConstant(handler->GetSessionDbLocaleIndex());

        // Default: unclaimed only. `all` also lists consumed rows with who
        // claimed them and when (the rows are kept for audit).
        if (!includeClaimed)
        {
            std::vector<SmartstoneToken> tokens = sSmartstone->GetAccountTokens(accountId);
            if (tokens.empty())
            {
                handler->PSendModuleSysMessage(ModName, LANG_MOD_TOKEN_LIST_NONE, account.GetName());
                return true;
            }

            handler->PSendModuleSysMessage(ModName, LANG_MOD_TOKEN_LIST_HEADER, account.GetName());
            for (auto const& token : tokens)
                handler->PSendModuleSysMessage(ModName, LANG_MOD_TOKEN_LIST_ENTRY,
                    token.Id, GetTokenName(token.Type, handlerLocale));
            return true;
        }

        QueryResult result = LoginDatabase.Query(
            "SELECT Id, TokenType, ConsumedByGUID, ConsumedTime FROM smartstone_account_tokens WHERE AccountId = {} ORDER BY Id",
            accountId);
        if (!result)
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_TOKEN_LIST_NONE, account.GetName());
            return true;
        }

        handler->PSendModuleSysMessage(ModName, LANG_MOD_TOKEN_LIST_HEADER, account.GetName());
        do
        {
            Field* fields = result->Fetch();
            uint32 id = fields[0].Get<uint32>();
            uint8 type = fields[1].Get<uint8>();
            uint32 consumedBy = fields[2].Get<uint32>();
            uint32 consumedTime = fields[3].Get<uint32>();
            std::string name = GetTokenName(type, handlerLocale);

            if (!consumedBy)
            {
                handler->PSendModuleSysMessage(ModName, LANG_MOD_TOKEN_LIST_ENTRY, id, name);
                continue;
            }

            std::string charName;
            sCharacterCache->GetCharacterNameByGuid(ObjectGuid::Create<HighGuid::Player>(consumedBy), charName);
            if (charName.empty())
                charName = std::to_string(consumedBy);

            std::string date = Acore::StringFormat("{:%Y-%m-%d %H:%M}", Acore::Time::TimeBreakdown(consumedTime));
            handler->PSendModuleSysMessage(ModName, LANG_MOD_TOKEN_LIST_ENTRY_CLAIMED, id, name, charName, date);
        } while (result->NextRow());

        return true;
    }

    static bool HandleSmartstoneTokenRevokeCommand(ChatHandler* handler, uint32 tokenId)
    {
        // Delete only an unconsumed row so we never erase audit history for a
        // token a player already used.
        QueryResult result = LoginDatabase.Query(
            "SELECT AccountId, TokenType FROM smartstone_account_tokens WHERE Id = {} AND ConsumedByGUID = 0",
            tokenId);
        if (!result)
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_TOKEN_REVOKE_NOT_FOUND, tokenId);
            handler->SetSentErrorMessage(true);
            LOG_ERROR("smartstone", "HandleSmartstoneTokenRevokeCommand: no unconsumed token with id {}.", tokenId);
            return false;
        }

        Field* fields = result->Fetch();
        uint32 accountId = fields[0].Get<uint32>();
        uint8 tokenType = fields[1].Get<uint8>();

        LoginDatabase.Execute("DELETE FROM smartstone_account_tokens WHERE Id = {} AND ConsumedByGUID = 0", tokenId);

        std::string accountName;
        AccountMgr::GetName(accountId, accountName);

        uint32 byAccount = 0;
        if (Player* gm = handler->GetPlayer())
            byAccount = gm->GetSession()->GetAccountId();
        LOG_INFO("smartstone", "Token {} (type {}) revoked from account {} (by account {}).", tokenId, tokenType, accountId, byAccount);

        LocaleConstant handlerLocale = LocaleConstant(handler->GetSessionDbLocaleIndex());
        handler->PSendModuleSysMessage(ModName, LANG_MOD_TOKEN_REVOKED,
            tokenId, GetTokenName(tokenType, handlerLocale), accountName);
        return true;
    }

    static bool HandleSmartstoneReloadCommand(ChatHandler* handler)
    {
        sSmartstone->LoadSmartstoneData();
        handler->PSendModuleSysMessage(ModName, LANG_MOD_DATA_RELOADED);
        return true;
    }

    static bool HandleSmartstoneDebugCostumeCommand(ChatHandler* handler, uint32 id, Optional<PlayerIdentifier> target)
    {
        if (!target)
            target = PlayerIdentifier::FromSelf(handler);

        if (!target)
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_DEBUG_NO_TARGET);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = target->GetConnectedPlayer();
        if (!player)
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_DEBUG_TARGET_OFFLINE, target->GetName());
            handler->SetSentErrorMessage(true);
            return false;
        }

        SmartstoneCostumeData costume = sSmartstone->GetCostumeData(id);
        if (!costume.Id)
        {
            handler->PSendModuleSysMessage(ModName, LANG_MOD_COSTUME_NOT_EXIST, id);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint8 membership = player->IsGameMaster()
            ? 3
            : player->GetPlayerSetting(SubsModName, SETTING_MEMBERSHIP_LEVEL).value;

        Milliseconds effectiveDuration = sSmartstone->GetCostumeDuration(player, costume.Duration);
        uint32 effectiveDurationSec =
            std::chrono::duration_cast<std::chrono::seconds>(effectiveDuration).count();
        bool unlimited = (effectiveDuration <= 0s);

        bool individual = sSmartstone->HasIndividualCostumeCooldowns();
        uint32 now = GameTime::GetGameTime().count();

        uint32 cooldownRemaining = 0;
        uint32 cooldownTotal = 0;
        bool cooldownActive = false;

        if (individual)
        {
            uint32 cooldownExpire = player->GetPlayerSetting(ModName + "#ccd", id).value;
            cooldownActive = cooldownExpire > now;
            cooldownRemaining = cooldownActive ? (cooldownExpire - now) : 0;
            cooldownTotal = costume.Cooldown
                ? costume.Cooldown
                : std::chrono::duration_cast<std::chrono::seconds>(30min).count();
        }
        else
        {
            cooldownRemaining = player->GetSpellCooldownDelay(90002) / 1000;
            cooldownActive = cooldownRemaining > 0;
            cooldownTotal = std::chrono::duration_cast<std::chrono::seconds>(30min).count();
        }

        uint32 elapsedSinceApply = cooldownActive && cooldownRemaining <= cooldownTotal
            ? cooldownTotal - cooldownRemaining
            : 0;

        bool inGrace = sSmartstone->IsInCostumeGracePeriod(player, id);
        bool hasCooldown = sSmartstone->HasCostumeCooldown(player, id);
        uint32 reportedRemaining = sSmartstone->GetCostumeCooldownRemaining(player, id);

        std::string effectiveDurationStr = unlimited
            ? "0 (unlimited)"
            : std::to_string(effectiveDurationSec);

        handler->PSendModuleSysMessage(ModName, LANG_MOD_DEBUG_COSTUME_DUMP,
            costume.Id, costume.Description, target->GetName(),
            costume.DisplayId, costume.Duration, costume.Cooldown,
            uint32(membership), effectiveDurationStr,
            individual ? "individual (#ccd)" : "shared (spell 90002)",
            cooldownTotal,
            cooldownActive ? "yes" : "no",
            cooldownRemaining, cooldownRemaining / 60, cooldownRemaining % 60,
            elapsedSinceApply,
            inGrace ? "true" : "false",
            hasCooldown ? "true" : "false",
            reportedRemaining);

        return true;
    }
};

void AddSC_smartstone_commandscript()
{
    new smartstone_commandscript();
}
