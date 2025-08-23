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
    QueryResult result = WorldDatabase.Query("SELECT CreatureId, Description, Type, Category, Duration, SubscriptionLevel FROM smartstone_pets WHERE Enabled = 1");
    SmartstonePetData petData;

    Pets.clear();

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            petData.CreatureId = fields[0].Get<uint32>();
            petData.Description = fields[1].Get<std::string>();
            petData.Type = static_cast<STSPetType>(fields[2].Get<uint8>());
            petData.Category = fields[3].Get<uint32>();
            petData.Duration = fields[4].Get<uint32>();
            petData.SubscriptionLevelRequired = fields[5].Get<uint8>();
            petData.Type == PET_TYPE_COMBAT ? CombatPets.push_back(petData) : Pets.push_back(petData);

            MenuItems[petData.Category].push_back(MenuItem{
                petData.CreatureId,
                petData.Description,
                0, // NpcTextId
                GOSSIP_ICON_TABARD,
                petData.Type == PET_TYPE_COMBAT ? ACTION_TYPE_PET : ACTION_TYPE_COMPANION,
                petData.SubscriptionLevelRequired
            });
        } while (result->NextRow());
    }
}

void Smartstone::LoadCostumes()
{
    // Load costumes from the database
    QueryResult result = WorldDatabase.Query("SELECT DisplayId, Category, SubscriptionLevel, Duration, Description, Id, Scale FROM smartstone_costumes WHERE Enabled = 1");
    SmartstoneCostumeData costumeData;
    Costumes.clear();
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            costumeData.DisplayId = fields[0].Get<uint32>();
            auto const& category = fields[1].Get<uint32>();
            costumeData.SubscriptionLevelRequired = fields[2].Get<uint8>();
            costumeData.Duration = fields[3].Get<uint32>();
            costumeData.Description = fields[4].Get<std::string>();
            costumeData.Id = fields[5].Get<uint32>();
            costumeData.Scale = fields[6].Get<float>();
            Costumes[category].push_back(costumeData);

            MenuItems[category].push_back(MenuItem{
                costumeData.Id,
                costumeData.Description,
                0, // NpcTextId
                GOSSIP_ICON_TABARD,
                ACTION_TYPE_COSTUME,
                costumeData.SubscriptionLevelRequired
            });
        } while (result->NextRow());
    }
}

void Smartstone::LoadServices()
{
    // Load services from the database
    QueryResult result = WorldDatabase.Query("SELECT ServiceId, Title, SubscriptionLevel, Category FROM smartstone_services WHERE Enabled = 1");
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
            serviceData.Category = fields[3].Get<uint8>();
            Services[serviceData.Category].push_back(serviceData);

            MenuItems[serviceData.Category].push_back(MenuItem{
                serviceData.Id,
                serviceData.ServiceTitle,
                0, // NpcTextId
                GOSSIP_ICON_TABARD,
                ACTION_TYPE_SERVICE,
                serviceData.SubscriptionLevelRequired
            });
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
            expireInfo.ServiceType = fields[2].Get<uint8>();
            expireInfo.ActivationTime = fields[3].Get<uint32>();
            expireInfo.ExpirationTime = fields[4].Get<uint32>();
            ServiceExpireInfo[expireInfo.PlayerGUID].push_back(expireInfo);
        } while (result->NextRow());
    }
}

void Smartstone::LoadCategories()
{
    // Load categories from the database
    QueryResult result = WorldDatabase.Query("SELECT ParentCategory, Title, SubscriptionLevel, NpcTextId, Id FROM smartstone_categories WHERE Enabled = 1");
    SmartstoneCategoryData categoryData;
    Categories.clear();
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            auto const& parentCategory = fields[0].Get<uint32>();
            categoryData.ParentId = parentCategory;
            categoryData.Title = fields[1].Get<std::string>();
            categoryData.SubscriptionLevelRequired = fields[2].Get<uint8>();
            categoryData.NpcTextId = fields[3].Get<uint32>();
            categoryData.Id = fields[4].Get<uint32>();
            Categories[categoryData.ParentId].push_back(categoryData);

            MenuItems[categoryData.ParentId].push_back(MenuItem{
                categoryData.Id,
                categoryData.Title,
                categoryData.NpcTextId,
                GOSSIP_ICON_TABARD,
                ACTION_TYPE_CATEGORY,
                categoryData.SubscriptionLevelRequired
            });
        } while (result->NextRow());
    }
}

void Smartstone::LoadAuras()
{
    // Load auras from the database
    QueryResult result = WorldDatabase.Query("SELECT Id, SpellId, Description, SubscriptionLevel FROM smartstone_auras WHERE Enabled = 1");
    SmartstoneAuraData auraData;

    Auras.clear();

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            auraData.Id = fields[0].Get<uint32>();
            auraData.SpellID = fields[1].Get<uint32>();
            auraData.Description = fields[2].Get<std::string>();
            auraData.SubscriptionLevelRequired = fields[3].Get<uint8>();
            Auras.push_back(auraData);
            MenuItems[ACTION_TYPE_AURA].push_back(MenuItem{
                auraData.Id,
                auraData.Description,
                0, // NpcTextId
                GOSSIP_ICON_TABARD,
                ACTION_TYPE_AURA,
                auraData.SubscriptionLevelRequired
                });
        } while (result->NextRow());
    }
}

SmartstoneServiceExpireInfo Smartstone::GetServiceExpireInfo(uint32 playerGUID, uint32 serviceId, uint8 serviceType) const
{
    auto const& expireInfo = ServiceExpireInfo.find(playerGUID);

    if (expireInfo != ServiceExpireInfo.end())
    {
        for (auto const& info : expireInfo->second)
        {
            if (info.ServiceId == serviceId && info.ServiceType == serviceType)
                return info;
        }
    }

    // Return empty/default expire info with ServiceId = 0 to indicate not found
    SmartstoneServiceExpireInfo defaultInfo = {};
    defaultInfo.ServiceId = 0;
    defaultInfo.PlayerGUID = 0;
    defaultInfo.ServiceType = 0;
    defaultInfo.ActivationTime = 0;
    defaultInfo.ExpirationTime = 0;
    return defaultInfo;
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
                switch (info.ServiceType)
                {
                    case ACTION_TYPE_PET:
                        player->UpdatePlayerSetting(ModName + "#combatpet", info.ServiceId, false);
                        ChatHandler(player->GetSession()).PSendSysMessage("Your grasp on {} wanes, you can no longer summon that pet.", GetPetData(info.ServiceId, ACTION_TYPE_PET).Description);
                        toRemove.push_back(info);
                        CharacterDatabase.Execute("DELETE FROM smartstone_char_temp_services WHERE PlayerGUID = {} AND ServiceId = {} AND Category = {}", playerGUID, info.ServiceId, info.ServiceType);
                        break;
                }
            }
        }

        for (const auto& info : toRemove)
        {
            ServiceExpireInfo[playerGUID].remove_if([&](const SmartstoneServiceExpireInfo& data) -> bool {
                return data.ServiceId == info.ServiceId && data.ServiceType == info.ServiceType;
            });
        }
    }
}

SmartstonePetData Smartstone::GetPetData(uint32 creatureId, uint8 petType) const
{
    const auto& petList = (petType == PET_TYPE_COMBAT) ? CombatPets : Pets;

    for (auto const& pet : petList)
    {
        if (pet.CreatureId == creatureId)
            return pet;
    }

    // Return empty/default pet data with CreatureId = 0 to indicate not found
    SmartstonePetData defaultPet = {};
    defaultPet.CreatureId = 0;
    return defaultPet;
}

SmartstoneCostumeData Smartstone::GetCostumeData(uint32 id) const
{
    for (auto const& category : Costumes)
    {
        for (auto const& costume : category.second)
        {
            if (costume.Id == id)
                return costume;
        }
    }

    // Return empty/default costume data with Id = 0 to indicate not found
    SmartstoneCostumeData defaultCostume = {};
    defaultCostume.Id = 0;
    return defaultCostume;
}

SmartstoneAuraData Smartstone::GetAuraData(uint32 id) const
{
    for (auto const& aura : Auras)
    {
        if (aura.Id == id)
            return aura;
    }

    // Return empty/default aura data with Id = 0 to indicate not found
    SmartstoneAuraData defaultCostume = {};
    defaultCostume.Id = 0;
    return defaultCostume;
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
        SmartstoneServiceExpireInfo expireInfo = GetServiceExpireInfo(player->GetGUID().GetCounter(), pet.CreatureId, pet.Type);
        if (expireInfo.ExpirationTime >= GameTime::GetGameTime().count())
            return true;
    }

    if (pet.SubscriptionLevelRequired && subscriptionLevel >= pet.SubscriptionLevelRequired)
        return true;

    std::string const& setting = pet.Type == PET_TYPE_COMBAT ? ModName + "#combatpet" : ModName + "#pet";

    if (pet.Type == PET_TYPE_COMPANION)
    {
        const std::string ModZoneDifficultyString = "mod-zone-difficulty#ct";

        switch (pet.CreatureId)
        {
            case 80002: // Hyjal Wisp
                return player->GetPlayerSetting(ModZoneDifficultyString, SETTING_HYJAL).IsEnabled();
            case 80003: // Serpentshrine Waterspawn
                return player->GetPlayerSetting(ModZoneDifficultyString, SETTING_SSC).IsEnabled();
            case 80006: // Thunderwing (Zul'Aman)
                return player->GetPlayerSetting(ModZoneDifficultyString, SETTING_ZULAMAN).IsEnabled();
            case 80010: // Alythessa
                return player->GetPlayerSetting(ModZoneDifficultyString, SETTING_SWP).IsEnabled();
            case 80011: // Scorchling of Azzinoth
                return player->GetPlayerSetting(ModZoneDifficultyString, SETTING_BLACK_TEMPLE).IsEnabled();
        }
    }

    return player->GetPlayerSetting(setting, pet.GetId()).IsEnabled();
}

bool Smartstone::IsServiceAvailable(Player* player, std::string service, uint32 serviceId) const
{
    if (player->IsGameMaster())
        return true;

    return player->GetPlayerSetting(ModName + service, serviceId).IsEnabled();
}

uint32 Smartstone::GetNPCTextForCategory(uint32 type, uint8 category) const
{
    auto categoryIt = Categories.find(type);
    if (categoryIt == Categories.end()) {
        return 1; // Default NPC text ID if category type not found
    }

    for (auto const& categoryData : categoryIt->second)
    {
        if (categoryData.Id == category && categoryData.NpcTextId != 0)
            return categoryData.NpcTextId;
    }
    return 1; // Default NPC text ID if not found
}

void Smartstone::ApplyCostume(Player* player, SmartstoneCostumeData const& costume)
{
    player->SetDisplayId(costume.DisplayId);
    player->SetObjectScale(costume.Scale);
    sSmartstone->SetCurrentCostume(player, costume.DisplayId);
}

std::string Smartstone::GetModuleStringForService(uint8 serviceType) const
{
    switch (serviceType)
    {
        case ACTION_TYPE_COMPANION:
            return ModName + "#pet";
        case ACTION_TYPE_PET:
            return ModName + "#combatpet";
        case ACTION_TYPE_COSTUME:
            return ModName + "#costume";
        case ACTION_TYPE_AURA:
            return ModName + "#aura";
        default:
            return "";
    }
}

void Smartstone::ApplyCostume(Player* player, uint32 costumeId)
{
    SmartstoneCostumeData costume = GetCostumeData(costumeId);
    player->SetDisplayId(costume.DisplayId, costume.Scale);
    SetCurrentCostume(player, costume.DisplayId);

    player->AddSpellCooldown(90002, 0, 30 * MINUTE * IN_MILLISECONDS);

    Milliseconds duration = GetCostumeDuration(player, costume.Duration);
    if (duration > 0s)
    {
        player->m_Events.AddEventAtOffset([player] {
            if (player->GetDisplayId() == Smartstone::instance()->GetCurrentCostume(player))
                player->SetDisplayId(player->GetNativeDisplayId());
        }, duration);
    }
}
