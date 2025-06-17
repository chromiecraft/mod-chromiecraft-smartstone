/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "Chat.h"
#include "GameTime.h"
#include "ScriptMgr.h"
#include "Smartstone.h"
#include "Player.h"

Smartstone* Smartstone::instance()
{
    static Smartstone instance;
    return &instance;
}

void Smartstone::LoadPets()
{
    // Load pets from the database
    QueryResult result = WorldDatabase.Query("SELECT CreatureId, Description, Category, Duration, SubscriptionLevel FROM smartstone_pets WHERE Enabled = 1");
    SmartstonePetData petData;

    Pets.clear();

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            petData.CreatureId = fields[0].Get<uint32>();
            petData.Description = fields[1].Get<std::string>();
            petData.Category = fields[2].Get<uint8>();
            petData.Duration = fields[3].Get<uint32>();
            petData.SubscriptionLevelRequired = fields[4].Get<uint8>();
            fields[2].Get<uint32>() ? CombatPets.push_back(petData) : Pets.push_back(petData);
        } while (result->NextRow());
    }
}

void Smartstone::LoadCostumes()
{
    // Load costumes from the database
    QueryResult result = WorldDatabase.Query("SELECT DisplayId, Category, SubscriptionLevel, Duration, Description, Id FROM smartstone_costumes WHERE Enabled = 1");
    SmartstoneCostumeData costumeData;
    Costumes.clear();
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            costumeData.DisplayId = fields[0].Get<uint32>();
            costumeData.SubscriptionLevelRequired = fields[2].Get<uint8>();
            costumeData.Duration = fields[3].Get<uint32>();
            costumeData.Description = fields[4].Get<std::string>();
            costumeData.Id = fields[5].Get<uint32>();
            Costumes[fields[1].Get<uint8>()].push_back(costumeData);
        } while (result->NextRow());
    }
}

void Smartstone::LoadServices()
{
    // Load services from the database
    QueryResult result = WorldDatabase.Query("SELECT ServiceId, Title, SubscriptionLevel FROM smartstone_services WHERE Enabled = 1");
    SmartstoneService serviceData;

    Services.clear();

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            serviceData.Id = fields[0].Get<uint32>();
            serviceData.ServiceTitle = fields[1].Get<std::string>();
            serviceData.SubscriptionLevelRequired = fields[2].Get<uint32>();
            Services.push_back(serviceData);
        } while (result->NextRow());
    }
}

void Smartstone::LoadServiceExpirationInfo()
{
    // Load service expiration info from the database
    QueryResult result = CharacterDatabase.Query("SELECT ServiceId, PlayerGUID, Category, ActivationTime, ExpirationTime FROM smartstone_char_temp_services");
    SmartstoneServiceExpireInfo expireInfo;
    ServiceExpireInfo.clear();
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            expireInfo.ServiceId = fields[0].Get<uint32>();
            expireInfo.PlayerGUID = fields[1].Get<uint32>();
            expireInfo.Category = fields[2].Get<uint8>();
            expireInfo.ActivationTime = fields[3].Get<uint32>();
            expireInfo.ExpirationTime = fields[4].Get<uint32>();
            ServiceExpireInfo[expireInfo.PlayerGUID].push_back(expireInfo);
        } while (result->NextRow());
    }
}

void Smartstone::LoadCategories()
{
    // Load categories from the database
    QueryResult result = WorldDatabase.Query("SELECT CategoryType, Title, SubscriptionLevel, NpcTextId, Id FROM smartstone_categories WHERE Enabled = 1");
    SmartstoneCategoryData categoryData;
    Categories.clear();
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            categoryData.Type = fields[0].Get<uint8>();
            categoryData.Title = fields[1].Get<std::string>();
            categoryData.SubscriptionLevelRequired = fields[2].Get<uint8>();
            categoryData.NpcTextId = fields[3].Get<uint32>();
            categoryData.Id = fields[4].Get<uint32>();
            Categories[fields[0].Get<uint32>()].push_back(categoryData);
        } while (result->NextRow());
    }
}

SmartstoneServiceExpireInfo Smartstone::GetServiceExpireInfo(uint32 playerGUID, uint32 serviceId, uint8 category) const
{
    auto const& expireInfo = ServiceExpireInfo.find(playerGUID);

    if (expireInfo != ServiceExpireInfo.end())
    {
        for (auto const& info : expireInfo->second)
        {
            if (info.ServiceId == serviceId && info.Category == category)
                return info;
        }
    }

    return SmartstoneServiceExpireInfo();
}

void Smartstone::ProcessExpiredServices(Player* player)
{
    uint32 playerGUID = player->GetGUID().GetCounter();
    auto const& expireInfo = ServiceExpireInfo.find(playerGUID);

    if (expireInfo != ServiceExpireInfo.end())
    {
        std::vector<SmartstoneServiceExpireInfo> toRemove;

        for (auto const& info : expireInfo->second)
        {
            if (info.ExpirationTime < GameTime::GetGameTime().count())
            {
                switch (info.Category)
                {
                    case SERVICE_CAT_COMBAT_PET:
                        player->UpdatePlayerSetting(ModName + "#combatpet", info.ServiceId - ACTION_RANGE_SUMMON_COMBAT_PET, false);
                        ChatHandler(player->GetSession()).PSendSysMessage("Your grasp on {} wanes, you can no longer summon that pet.", GetPetData(info.ServiceId, SERVICE_CAT_COMBAT_PET).Description);
                        toRemove.push_back(info);
                        CharacterDatabase.Execute("DELETE FROM smartstone_char_temp_services WHERE PlayerGUID = {} AND ServiceId = {} AND Category = {}", playerGUID, info.ServiceId, info.Category);
                        break;
                }
            }
        }

        for (const auto& info : toRemove)
        {
            ServiceExpireInfo[playerGUID].remove_if([&](const SmartstoneServiceExpireInfo& data) -> bool {
                return data.ServiceId == info.ServiceId && data.Category == info.Category;
            });
        }
    }
}

SmartstonePetData Smartstone::GetPetData(uint32 creatureId, uint8 category) const
{
    for (auto const& pet : category ? sSmartstone->CombatPets : sSmartstone->Pets)
    {
        if (pet.CreatureId == creatureId)
            return pet;
    }

    return SmartstonePetData();
}

SmartstoneCostumeData Smartstone::GetCostumeData(uint32 id) const
{
    for (auto const& category : sSmartstone->Categories[CATEGORY_COSTUMES])
    {
        for (auto const& costume : sSmartstone->Costumes[category.Id])
        {
            if (costume.Id == id)
                return costume;
        }
    }

    return SmartstoneCostumeData();
}

Milliseconds Smartstone::GetCostumeDuration(Player* player, uint32 costumeDuration) const
{
    // If the costume has a duration override set in database, use it instead
    if (costumeDuration)
        return Minutes(costumeDuration);

    Milliseconds duration = 30min;

    uint8 membershipLevel = player->GetPlayerSetting(SubsModName, SETTING_MEMBERSHIP_LEVEL).value;
    switch (membershipLevel)
    {
        case 0:
            duration = 15min;
            break;
        case 1:
            duration = 30min;
            break;
        case 2:
            duration = 1h;
            break;
        case 3:
            duration = 0s;
            break;
        default:
            break;

    }

    return duration;
}

bool Smartstone::IsPetAvailable(Player* player, SmartstonePetData pet, uint8 subscriptionLevel) const
{
    if (player->IsGameMaster())
        return true;

    if (pet.Duration)
    {
        SmartstoneServiceExpireInfo expireInfo = GetServiceExpireInfo(player->GetGUID().GetCounter(), pet.CreatureId, pet.Category);
        if (expireInfo.ExpirationTime >= GameTime::GetGameTime().count())
            return true;
    }

    if (pet.SubscriptionLevelRequired && subscriptionLevel >= pet.SubscriptionLevelRequired)
        return true;

    std::string const& setting = pet.Category == SERVICE_CAT_COMBAT_PET ? ModName + "#combatpet" : ModName + "#pet";

    if (pet.Category == SERVICE_CAT_PET)
    {
        const std::string ModZoneDifficultyString = "mod-zone-difficulty#ct";

        switch (pet.GetServiceId())
        {
            case 2: // Hyjal Wisp
                return player->GetPlayerSetting(ModZoneDifficultyString, SETTING_HYJAL).IsEnabled();
            case 3: // Serpentshrine Waterspawn
                return player->GetPlayerSetting(ModZoneDifficultyString, SETTING_SSC).IsEnabled();
            case 6: // Thunderwing (Zul'Aman)
                return player->GetPlayerSetting(ModZoneDifficultyString, SETTING_ZULAMAN).IsEnabled();
            case 10: // Alythessa
                return player->GetPlayerSetting(ModZoneDifficultyString, SETTING_SWP).IsEnabled();
            case 11: // Scorchling of Azzinoth
                return player->GetPlayerSetting(ModZoneDifficultyString, SETTING_BLACK_TEMPLE).IsEnabled();
        }
    }

    return player->GetPlayerSetting(setting, pet.GetServiceId()).IsEnabled();
}

bool Smartstone::IsServiceAvailable(Player* player, std::string service, uint32 serviceId) const
{
    if (player->IsGameMaster())
        return true;

    return player->GetPlayerSetting(ModName + service, serviceId).IsEnabled();
}

uint32 Smartstone::GetNPCTextForCategory(uint32 type, uint8 category) const
{
    for (auto const& categoryData : sSmartstone->Categories[type])
    {
        if (categoryData.Id == category)
            return categoryData.NpcTextId;
    }
    return 1; // Default NPC text ID if not found
}

std::string Smartstone::GetModuleStringForCategory(uint8 category)
{
    switch (category)
    {
        case SERVICE_CAT_PET:
            return "#pet";
        case SERVICE_CAT_COMBAT_PET:
            return "#combatpet";
        case SERVICE_CAT_COSTUMES:
            return "#costume";
        default:
            return "";
    }
}

