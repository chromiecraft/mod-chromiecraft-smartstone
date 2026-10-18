/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "Chat.h"
#include "Creature.h"
#include "GameTime.h"
#include "Map.h"
#include "Pet.h"
#include "ScriptMgr.h"
#include "Smartstone.h"
#include "Player.h"
#include "Containers.h"
#include "Random.h"
#include <algorithm>
#include <array>
#include <vector>

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
    QueryResult result = WorldDatabase.Query("SELECT DisplayId, Category, SubscriptionLevel, Duration, Description, Id, Scale, Cooldown FROM smartstone_costumes WHERE Enabled = 1");
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
            costumeData.Cooldown = fields[7].Get<uint32>();
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

void Smartstone::LoadPerks()
{
    // Load class perks from the database. Each perk belongs to one of the
    // hardcoded class subcategories (1001-1010); class-gating is applied
    // again in ShowCategoryItems for defense in depth.
    // ORDER BY groups perks by their effect (e.g. all Bear Form perks
    // adjacent, then all Cat Form perks, etc.) so the gossip menu lists
    // them in clusters. Id is the secondary key to keep ordering stable
    // when multiple perks share the same effect.
    QueryResult result = WorldDatabase.Query("SELECT Id, Title, Description, ClassId, Category, Effect, Value, Scale, SubscriptionLevel FROM smartstone_perks WHERE Enabled = 1 ORDER BY Effect, Id");
    SmartstonePerkData perkData;

    Perks.clear();
    PerkScaleOverrides.clear();

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            perkData.Id = fields[0].Get<uint32>();
            perkData.Title = fields[1].Get<std::string>();
            perkData.Description = fields[2].Get<std::string>();
            perkData.ClassId = fields[3].Get<uint8>();
            perkData.Category = fields[4].Get<uint32>();
            perkData.Effect = fields[5].Get<uint8>();
            perkData.Value = fields[6].Get<uint32>();
            perkData.Scale = fields[7].Get<float>();
            perkData.SubscriptionLevelRequired = fields[8].Get<uint8>();
            Perks[perkData.Category].push_back(perkData);

            // Cache the scale so the form / pet apply sites can resolve it
            // from (their effect + the stored displayId). Keyed by effect
            // so one model can carry different scales per render context.
            if (perkData.Value && perkData.Scale > 0.0f)
                PerkScaleOverrides[PerkScaleKey(perkData.Effect, perkData.Value)] = perkData.Scale;

            MenuItems[perkData.Category].push_back(MenuItem{
                perkData.Id,
                perkData.Title,
                0, // NpcTextId
                GOSSIP_ICON_TABARD,
                ACTION_TYPE_PERK,
                perkData.SubscriptionLevelRequired
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
                        ChatHandler(player->GetSession()).PSendModuleSysMessage(ModName, LANG_MOD_PET_WANES, GetPetData(info.ServiceId, ACTION_TYPE_PET).Description);
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

SmartstoneCostumeData Smartstone::GetCostumeDataByDisplayId(uint32 displayId) const
{
    for (auto const& category : Costumes)
    {
        for (auto const& costume : category.second)
        {
            if (costume.DisplayId == displayId)
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

SmartstoneVehicleData Smartstone::GetVehicleData(uint32 id) const
{
    for (auto const& vehicle : Vehicles)
    {
        if (vehicle.Id == id)
            return vehicle;
    }

    // Return empty/default vehicle data with Id = 0 to indicate not found
    SmartstoneVehicleData defaultVehicle = {};
    defaultVehicle.Id = 0;
    return defaultVehicle;
}

SmartstonePerkData Smartstone::GetPerkData(uint32 id) const
{
    for (auto const& [category, perks] : Perks)
    {
        for (auto const& perk : perks)
        {
            if (perk.Id == id)
                return perk;
        }
    }

    SmartstonePerkData defaultPerk = {};
    defaultPerk.Id = 0;
    return defaultPerk;
}

SmartstoneMountData Smartstone::GetMountData(uint32 id) const
{
    for (auto const& mount : Mounts)
    {
        if (mount.Id == id)
            return mount;
    }

    // Return empty/default mount data with Id = 0 to indicate not found
    SmartstoneMountData defaultMount = {};
    defaultMount.Id = 0;
    return defaultMount;
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
    if (player->IsGameMaster() || sSmartstone->IsSmartstoneDebugEnabled())
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
    if (player->IsGameMaster() || sSmartstone->IsSmartstoneDebugEnabled())
        return true;

    if (service == "#costume")
        return sSmartstone->GetAccountSetting(player->GetSession()->GetAccountId(), ACTION_TYPE_COSTUME, serviceId).IsEnabled();

    if (service == "#perk")
    {
        uint32 slot = Smartstone::GetPerkAccountSettingForClass(player->getClass());
        if (!slot)
            return false;
        return sSmartstone->GetAccountSetting(player->GetSession()->GetAccountId(), slot, serviceId).IsEnabled();
    }

    return player->GetPlayerSetting(ModName + service, serviceId).IsEnabled();
}

uint32 Smartstone::GetNPCTextForCategory(uint32 /*type*/, uint32 category) const
{
    if (category == CATEGORY_MAIN)
        return 92000;

    // Categories map is keyed by ParentId, so a category at any depth
    // can sit in any bucket. Scan all buckets — `type` (always 0 at the
    // call site) was unreliable for non-top-level categories.
    for (auto const& [parentId, list] : Categories)
    {
        for (auto const& categoryData : list)
        {
            if (categoryData.Id == category && categoryData.NpcTextId != 0)
                return categoryData.NpcTextId;
        }
    }
    return 1; // Default NPC text ID if not found
}

void Smartstone::PlayCostumeSound(Player* player, uint32 displayId)
{
    auto it = CostumeSounds.find(displayId);
    if (it == CostumeSounds.end())
        return;

    SmartstoneCostumeSoundData const& soundData = it->second;
    if (soundData.Mode == 2)
        player->PlayDirectSound(soundData.Sound);
    else
        player->PlayDirectSound(soundData.Sound, player);
}

void Smartstone::ApplyCostume(Player* player, SmartstoneCostumeData const& costume)
{
    player->SetDisplayId(costume.DisplayId);
    player->SetObjectScale(costume.Scale);
    sSmartstone->SetCurrentCostume(player, costume.DisplayId);
    PlayCostumeSound(player, costume.DisplayId);
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
        case ACTION_TYPE_VEHICLE:
            return ModName + "#vehicle";
        case ACTION_TYPE_MOUNT:
            return ModName + "#mount";
        case ACTION_TYPE_PERK:
            return ModName + "#perk";
        default:
            return "";
    }
}

bool Smartstone::IsBattlefieldUseBlocked(Player* player) const
{
    if (CanUseInPvP)
        return false;

    Battlefield* bf = sBattlefieldMgr->GetBattlefieldToZoneId(player->GetZoneId());
    return bf && bf->IsWarTime();
}

void Smartstone::ApplyCostume(Player* player, uint32 costumeId)
{
    SmartstoneCostumeData costume = GetCostumeData(costumeId);

    bool inGrace = IsInCostumeGracePeriod(player, costumeId);

    player->SetDisplayId(costume.DisplayId, costume.Scale);
    SetCurrentCostume(player, costume.DisplayId);
    PlayCostumeSound(player, costume.DisplayId);

    // Grace reapply: original cooldown and duration timer still running, don't re-stamp.
    if (inGrace)
        return;

    SetCostumeCooldown(player, costumeId);

    Milliseconds duration = GetCostumeDuration(player, costume.Duration);
    if (duration > 0s)
    {
        std::string description = costume.Description;
        player->m_Events.AddEventAtOffset([player, description] {
            if (player->GetDisplayId() == Smartstone::instance()->GetCurrentCostume(player))
            {
                player->SetDisplayId(player->GetNativeDisplayId());
                Smartstone::instance()->SetCurrentCostume(player, 0);
                ChatHandler(player->GetSession()).PSendModuleSysMessage(ModName, LANG_MOD_COSTUME_EXPIRED, description);
            }
        }, duration);
    }
}

enum SmartstonePerkMinionEntry : uint32
{
    NPC_FERAL_SPIRIT_WOLF       = 29264,
    NPC_DK_GHOUL                = 26125, // Raise Dead, as guardian or Master of Ghouls pet
    NPC_DK_ARMY_GHOUL           = 24207,
    NPC_WATER_ELEMENTAL         = 510,
    NPC_WATER_ELEMENTAL_GLYPHED = 37994, // Glyph of Eternal Water
};

// Army of the Dead skins whose summons each roll their own look and weapons,
// keyed by the perk's displayId (smartstone_perks.Value).
// Every look is equally likely, so the armed / unarmed list sizes set the mix.
struct ArmySkin
{
    uint32 PerkDisplay;
    std::vector<uint32> ArmedDisplays;   // roll one of Weapons
    std::vector<uint32> UnarmedDisplays; // models that can't hold weapons
    std::vector<std::pair<uint32, uint32>> Weapons; // main hand, off hand
};

static std::array<ArmySkin, 4> const ArmySkins = { {
    // Dead: gray / white murlocs with Vile Fin style rusty gear.
    { 346, { 346, 531, 540, 3619, 3940 }, { }, { { 2023, 0 }, { 1895, 0 }, { 1900, 2053 }, { 3368, 0 } } },
    // Living: green / blue Elwynn murlocs with their own weapons.
    { 441, { 441, 527, 1994, 617, 983, 1305 }, { }, { { 1895, 0 }, { 2184, 0 }, { 1897, 0 }, { 2695, 0 }, { 2023, 0 } } },
    // Tiny: Murky and the other baby murlocs.
    { 15369, { }, { 15369, 15394, 15395, 15396, 15397, 15398 }, { } },
    // Depths: naga myrmidons with tridents / spears / swords, plus sea turtles (2:1).
    { 4762, { 4762, 4036, 6748, 22876 }, { 1244, 27690 }, { { 5746, 0 }, { 12063, 0 }, { 5745, 0 }, { 2023, 0 }, { 1899, 1896 } } },
} };

static ArmySkin const* FindArmySkin(uint32 perkDisplay)
{
    auto it = std::find_if(ArmySkins.begin(), ArmySkins.end(),
        [perkDisplay](ArmySkin const& skin) { return skin.PerkDisplay == perkDisplay; });
    return it != ArmySkins.end() ? &*it : nullptr;
}

// Pets keep a direct owner pointer, which still resolves while the pet is
// being added to the world; the GUID lookup needs the owner on the same map.
static Player* GetMinionOwner(Creature const* minion)
{
    if (minion->IsPet())
        return static_cast<Pet const*>(minion)->GetOwner();

    return minion->GetCharmerOrOwnerPlayerOrPlayerItself();
}

// Form -> (slot, namespace, spell), mirroring spell_smartstone_form_display_override.
bool Smartstone::GetActiveFormPerkSubstitute(Player* player, uint32& canonicalModel) const
{
    if (!player)
        return false;

    ShapeshiftForm form = player->GetShapeshiftForm();
    uint8 slot;
    bool isShaman = false;
    uint32 spellId;
    switch (form)
    {
        case FORM_BEAR:        slot = DRUID_FORM_BEAR;        spellId =  5487; break;
        case FORM_DIREBEAR:    slot = DRUID_FORM_BEAR;        spellId =  9634; break;
        case FORM_CAT:         slot = DRUID_FORM_CAT;         spellId =   768; break;
        case FORM_TRAVEL:      slot = DRUID_FORM_TRAVEL;      spellId =   783; break;
        case FORM_FLIGHT:      slot = DRUID_FORM_FLIGHT;      spellId = 33943; break;
        case FORM_FLIGHT_EPIC: slot = DRUID_FORM_FLIGHT;      spellId = 40120; break;
        case FORM_AQUA:        slot = DRUID_FORM_AQUATIC;     spellId =  1066; break;
        case FORM_TREE:        slot = DRUID_FORM_TREE;        spellId = 33891; break;
        case FORM_MOONKIN:     slot = DRUID_FORM_MOONKIN;     spellId = 24858; break;
        case FORM_GHOSTWOLF:   slot = SHAMAN_FORM_GHOST_WOLF; isShaman = true; spellId = 2645; break;
        default:               return false;
    }

    uint32 overrideDisplay = isShaman
        ? GetShamanFormDisplay(player, slot)
        : GetDruidFormDisplay(player, slot);
    if (!overrideDisplay)
        return false;

    // Skip when the override isn't live (e.g. BG/arena shows the canonical model).
    if (player->GetDisplayId() != overrideDisplay)
        return false;

    canonicalModel = player->GetModelForForm(form, spellId);
    return canonicalModel != 0;
}

uint32 Smartstone::GetMinionPerkDisplay(Creature const* minion, uint8& effect) const
{
    if (!minion)
        return 0;

    // Entry gate first so unrelated creatures skip the owner lookup.
    uint32 const entry = minion->GetEntry();
    int32 const warlockSlot = Smartstone::GetWarlockPetSlotForEntry(entry);
    if (warlockSlot >= 0)
        effect = PERK_EFFECT_WARLOCK_PET_IMP + static_cast<uint8>(warlockSlot);
    else
    {
        switch (entry)
        {
            case NPC_FERAL_SPIRIT_WOLF:       effect = PERK_EFFECT_SHAMAN_FERAL_SPIRIT;  break;
            case NPC_DK_GHOUL:                effect = PERK_EFFECT_DK_GHOUL;             break;
            case NPC_DK_ARMY_GHOUL:           effect = PERK_EFFECT_DK_ARMY_OF_THE_DEAD;  break;
            case NPC_WATER_ELEMENTAL:
            case NPC_WATER_ELEMENTAL_GLYPHED: effect = PERK_EFFECT_MAGE_WATER_ELEMENTAL; break;
            default:                          return 0;
        }
    }

    Player* owner = GetMinionOwner(minion);
    if (!owner)
        return 0;

    switch (effect)
    {
        case PERK_EFFECT_SHAMAN_FERAL_SPIRIT:  return GetShamanGuardianDisplay(owner, SHAMAN_GUARDIAN_FERAL_SPIRIT);
        case PERK_EFFECT_DK_GHOUL:             return GetDeathKnightGuardianDisplay(owner, DK_GUARDIAN_GHOUL);
        case PERK_EFFECT_DK_ARMY_OF_THE_DEAD:  return GetDeathKnightGuardianDisplay(owner, DK_GUARDIAN_ARMY_OF_THE_DEAD);
        case PERK_EFFECT_MAGE_WATER_ELEMENTAL: return GetMageGuardianDisplay(owner, MAGE_GUARDIAN_WATER_ELEMENTAL);
        default:                               return GetWarlockPetDisplay(owner, static_cast<uint8>(warlockSlot));
    }
}

void Smartstone::RefreshMinionPerkDisplay(Creature* minion) const
{
    uint8 effect = PERK_EFFECT_NONE;
    uint32 displayId = GetMinionPerkDisplay(minion, effect);
    if (!effect)
        return;

    if (Player* owner = GetMinionOwner(minion))
        if (owner->InBattleground() || owner->InArena())
            displayId = 0;

    if (!displayId)
    {
        if (minion->GetDisplayId() != minion->GetNativeDisplayId())
        {
            minion->SetDisplayId(minion->GetNativeDisplayId(), minion->GetNativeObjectScale());
            // Summons have no spawn data, so their gear is template equipment id 1.
            minion->LoadEquipment(0, true);
            minion->LoadEquipment();
        }
        return;
    }

    ArmySkin const* armySkin = FindArmySkin(displayId);
    if (!armySkin)
    {
        minion->SetDisplayId(displayId, GetEffectivePerkScale(effect, displayId));
        return;
    }

    // Refreshes run for every controlled minion on each summon tick; keep an
    // already rolled look instead of re-rolling the whole army.
    if (IsMinionPerkModel(displayId, minion->GetDisplayId()))
        return;

    std::size_t const armedCount = armySkin->ArmedDisplays.size();
    std::size_t const roll = urand(0, armedCount + armySkin->UnarmedDisplays.size() - 1);
    bool const armed = roll < armedCount;

    uint32 const look = armed ? armySkin->ArmedDisplays[roll] : armySkin->UnarmedDisplays[roll - armedCount];
    minion->SetDisplayId(look, GetEffectivePerkScale(effect, displayId));

    std::pair<uint32, uint32> weapons = { 0, 0 };
    if (armed && !armySkin->Weapons.empty())
        weapons = Acore::Containers::SelectRandomContainerElement(armySkin->Weapons);

    minion->SetVirtualItem(0, weapons.first);
    minion->SetVirtualItem(1, weapons.second);
}

bool Smartstone::IsMinionPerkModel(uint32 skinDisplay, uint32 shownDisplay) const
{
    if (shownDisplay == skinDisplay)
        return true;

    ArmySkin const* armySkin = FindArmySkin(skinDisplay);
    if (!armySkin)
        return false;

    auto contains = [shownDisplay](std::vector<uint32> const& displays)
    {
        return std::find(displays.begin(), displays.end(), shownDisplay) != displays.end();
    };
    return contains(armySkin->ArmedDisplays) || contains(armySkin->UnarmedDisplays);
}

void Smartstone::RefreshControlledMinionPerkDisplays(Unit* owner) const
{
    for (Unit* controlled : owner->m_Controlled)
        if (Creature* minion = controlled ? controlled->ToCreature() : nullptr)
            RefreshMinionPerkDisplay(minion);
}

bool Smartstone::GetActiveMinionPerkSubstitute(Creature* creature, uint32& nativeDisplay) const
{
    uint8 effect = PERK_EFFECT_NONE;
    uint32 overrideDisplay = GetMinionPerkDisplay(creature, effect);
    if (!overrideDisplay)
        return false;

    if (!IsMinionPerkModel(overrideDisplay, creature->GetDisplayId()))
        return false;

    nativeDisplay = creature->GetNativeDisplayId();
    return nativeDisplay != 0;
}

void Smartstone::RefreshSmartstoneVisibilityFor(Player* observer) const
{
    if (!observer || !observer->IsInWorld())
        return;

    Map* map = observer->GetMap();
    if (!map)
        return;

    // A toggle doesn't dirty any field, so push a create block for each
    // affected unit in range to re-run the patch hook. Only fires on toggle.
    float const range = observer->GetVisibilityRange();

    Map::PlayerList const& players = map->GetPlayers();
    for (Map::PlayerList::const_iterator it = players.begin(); it != players.end(); ++it)
    {
        Player* p = it->GetSource();
        if (!p || !p->IsInWorld())
            continue;

        bool const pInRange = (p == observer) || observer->IsWithinDistInMap(p, range);

        // Player-level overrides: costume morph or shapeshift-form perk.
        if (pInRange)
        {
            uint32 canonical = 0;
            if (GetCurrentCostume(p) || GetActiveFormPerkSubstitute(p, canonical))
                p->SendUpdateToPlayer(observer);
        }

        // Minion overrides live on the player's controlled creatures.
        for (Unit* controlled : p->m_Controlled)
        {
            Creature* c = controlled ? controlled->ToCreature() : nullptr;
            if (!c || !c->IsInWorld())
                continue;

            uint32 native = 0;
            if (!GetActiveMinionPerkSubstitute(c, native))
                continue;

            if (!observer->IsWithinDistInMap(c, range))
                continue;

            c->SendUpdateToPlayer(observer);
        }
    }
}

void Smartstone::SetCostumeCooldown(Player* player, uint32 costumeId)
{
    if (IndividualCostumeCooldowns)
    {
        SmartstoneCostumeData costume = GetCostumeData(costumeId);

        uint32 cooldownSeconds = costume.Cooldown
            ? costume.Cooldown
            : std::chrono::duration_cast<std::chrono::seconds>(30min).count();

        uint32 expireTime = GameTime::GetGameTime().count() + cooldownSeconds;

        player->UpdatePlayerSetting(ModName + "#ccd", costumeId, expireTime);
    }
    else
    {
        player->AddSpellCooldown(90002, 0, 30 * MINUTE * IN_MILLISECONDS);
    }
}

bool Smartstone::HasCostumeCooldown(Player* player, uint32 costumeId) const
{
    if (IsInCostumeGracePeriod(player, costumeId))
        return false;

    if (IndividualCostumeCooldowns)
    {
        uint32 expireTime = player->GetPlayerSetting(ModName + "#ccd", costumeId).value;
        return expireTime > GameTime::GetGameTime().count();
    }

    return player->HasSpellCooldown(90002);
}

uint32 Smartstone::GetCostumeCooldownRemaining(Player* player, uint32 costumeId) const
{
    if (IsInCostumeGracePeriod(player, costumeId))
        return 0;

    if (IndividualCostumeCooldowns)
    {
        uint32 expireTime = player->GetPlayerSetting(ModName + "#ccd", costumeId).value;
        uint32 now = GameTime::GetGameTime().count();
        return expireTime > now ? expireTime - now : 0;
    }

    return player->GetSpellCooldownDelay(90002) / 1000;
}

bool Smartstone::IsInCostumeGracePeriod(Player* player, uint32 costumeId) const
{
    SmartstoneCostumeData costume = GetCostumeData(costumeId);
    if (!costume.Id)
        return false;

    Milliseconds duration = GetCostumeDuration(player, costume.Duration);
    bool unlimited = (duration <= 0s);
    uint32 durationSeconds = unlimited ? 0
        : std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    uint32 now = GameTime::GetGameTime().count();

    if (IndividualCostumeCooldowns)
    {
        uint32 cooldownExpire =
            player->GetPlayerSetting(ModName + "#ccd", costumeId).value;
        if (cooldownExpire <= now)
            return false;

        // Unlimited-duration costumes are meant to stay on indefinitely;
        // any reapply while the cooldown is still active counts as grace.
        if (unlimited)
            return true;

        uint32 cooldownSeconds = costume.Cooldown
            ? costume.Cooldown
            : std::chrono::duration_cast<std::chrono::seconds>(30min).count();

        // applyTime = cooldownExpire - cooldownSeconds
        // graceEnd  = applyTime + durationSeconds
        // inGrace iff graceEnd > now iff (cooldownExpire - cooldownSeconds + durationSeconds) > now
        // Guard against underflow when durationSeconds < cooldownSeconds.
        if (durationSeconds >= cooldownSeconds)
            return true;
        uint32 elapsedSinceApply = cooldownSeconds - (cooldownExpire - now);
        return elapsedSinceApply < durationSeconds;
    }

    // Shared-cooldown mode (spell 90002, hardcoded 30min).
    uint32 cooldownRemaining = player->GetSpellCooldownDelay(90002) / 1000;
    if (!cooldownRemaining)
        return false;

    if (unlimited)
        return true;

    uint32 cooldownSeconds =
        std::chrono::duration_cast<std::chrono::seconds>(30min).count();
    if (durationSeconds >= cooldownSeconds)
        return true;
    uint32 elapsedSinceApply = cooldownSeconds - cooldownRemaining;
    return elapsedSinceApply < durationSeconds;
}

PlayerSetting Smartstone::GetAccountSetting(uint32 accountId, uint32 service, uint32 index)
{
    auto& vec = AccountSettings[accountId][service];
    if (index >= vec.size())
        return PlayerSetting(0);

    return vec[index];
}

void Smartstone::UpdateAccountSetting(uint32 accountId, uint32 service, uint32 index, uint32 value)
{
    auto& vec = AccountSettings[accountId][service];

    if (index >= vec.size())
        vec.resize(index + 1, PlayerSetting(0));

    vec[index] = PlayerSetting(value);

    LoginDatabase.Query("REPLACE INTO smartstone_account_settings (accountId, settingId, data) VALUES ({}, {}, '{}')",
        accountId, service, PlayerSettingsStore::SerializeSettingsData(vec));
}

void Smartstone::LoadAccountSettings(uint32 accountId)
{
    QueryResult result = LoginDatabase.Query("SELECT settingId, data FROM smartstone_account_settings WHERE accountId = {}", accountId);
    if (!result)
        return;
    do
    {
        Field* fields = result->Fetch();
        uint32 settingId = fields[0].Get<uint32>();
        std::string data = fields[1].Get<std::string>();
        PlayerSettingVector settings = PlayerSettingsStore::ParseSettingsData(data);
        AccountSettings[accountId][settingId] = settings;
    } while (result->NextRow());
}

uint32 Smartstone::GetTokenNameStringId(uint8 type)
{
    switch (type)
    {
        case TOKEN_RENAME:         return LANG_MOD_TOKEN_NAME_RENAME;
        case TOKEN_FACTION_CHANGE: return LANG_MOD_TOKEN_NAME_FACTION;
        case TOKEN_RACE_CHANGE:    return LANG_MOD_TOKEN_NAME_RACE;
        case TOKEN_CUSTOMIZE:      return LANG_MOD_TOKEN_NAME_CUSTOMIZE;
        default:                     return 0;
    }
}

std::vector<SmartstoneToken> Smartstone::GetAccountTokens(uint32 accountId) const
{
    std::vector<SmartstoneToken> tokens;
    QueryResult result = LoginDatabase.Query(
        "SELECT Id, TokenType FROM smartstone_account_tokens WHERE AccountId = {} AND ConsumedByGUID = 0 ORDER BY Id",
        accountId);
    if (!result)
        return tokens;

    do
    {
        Field* fields = result->Fetch();
        tokens.push_back(SmartstoneToken{ fields[0].Get<uint32>(), fields[1].Get<uint8>() });
    } while (result->NextRow());

    return tokens;
}

bool Smartstone::HasAccountTokens(uint32 accountId) const
{
    return LoginDatabase.Query(
        "SELECT 1 FROM smartstone_account_tokens WHERE AccountId = {} AND ConsumedByGUID = 0 LIMIT 1",
        accountId) != nullptr;
}

uint32 Smartstone::GrantToken(uint32 accountId, uint8 type, uint32 grantedByAccount)
{
    // Synchronous insert so the id read-back below sees the new row.
    LoginDatabase.DirectExecute(
        "INSERT INTO smartstone_account_tokens (AccountId, TokenType, GrantedBy, GrantedTime) VALUES ({}, {}, {}, UNIX_TIMESTAMP())",
        accountId, type, grantedByAccount);

    // Auto-increment guarantees the row we just inserted has the highest id
    // for this account + type, so MAX(Id) among unconsumed rows is it.
    QueryResult result = LoginDatabase.Query(
        "SELECT MAX(Id) FROM smartstone_account_tokens WHERE AccountId = {} AND TokenType = {} AND ConsumedByGUID = 0",
        accountId, type);
    uint32 tokenId = result ? result->Fetch()[0].Get<uint32>() : 0;

    LOG_INFO("smartstone", "Token {} granted: type {} to account {} (granted by account {}).", tokenId, type, accountId, grantedByAccount);
    return tokenId;
}

bool Smartstone::ConsumeToken(uint32 tokenId, uint32 accountId, Player* consumer)
{
    if (!consumer)
        return false;

    QueryResult result = LoginDatabase.Query(
        "SELECT TokenType FROM smartstone_account_tokens WHERE Id = {} AND AccountId = {} AND ConsumedByGUID = 0",
        tokenId, accountId);
    if (!result)
        return false;

    uint8 type = result->Fetch()[0].Get<uint8>();
    uint16 flag = GetTokenAtLoginFlag(type);
    if (!flag)
        return false;

    uint32 guid = consumer->GetGUID().GetCounter();

    // Claim synchronously (DirectExecute) so a follow-up click on the same
    // menu sees the row already consumed rather than re-reading it as free.
    // The ConsumedByGUID = 0 guard makes the claim a no-op if it lost a race.
    LoginDatabase.DirectExecute(
        "UPDATE smartstone_account_tokens SET ConsumedByGUID = {}, ConsumedTime = UNIX_TIMESTAMP() "
        "WHERE Id = {} AND AccountId = {} AND ConsumedByGUID = 0",
        guid, tokenId, accountId);

    // Persist the at-login flag immediately, and mirror it on the in-memory
    // Player: the periodic/logout character save writes at_login from the
    // in-memory value, so skipping this would let a later save clobber the DB.
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);
    stmt->SetData(0, flag);
    stmt->SetData(1, guid);
    trans->Append(stmt);
    CharacterDatabase.CommitTransaction(trans);

    consumer->SetAtLoginFlag(static_cast<AtLoginFlags>(flag));

    LOG_INFO("smartstone", "Token {} (type {}) claimed by character {} on account {}; at-login flag {} set.",
        tokenId, type, guid, accountId, flag);
    return true;
}

void Smartstone::LoadVehicles()
{
    QueryResult result = WorldDatabase.Query("SELECT Id, CreatureId, Description, SubscriptionLevel, Flags FROM smartstone_vehicles WHERE Enabled = 1");
    SmartstoneVehicleData vehicleData;
    Vehicles.clear();
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            vehicleData.Id = fields[0].Get<uint32>();
            vehicleData.CreatureId = fields[1].Get<uint32>();
            vehicleData.Description = fields[2].Get<std::string>();
            vehicleData.SubscriptionLevelRequired = fields[3].Get<uint8>();
            vehicleData.Flags = fields[4].Get<uint32>();
            Vehicles.push_back(vehicleData);
            MenuItems[ACTION_TYPE_VEHICLE].push_back(MenuItem{
                vehicleData.Id,
                vehicleData.Description,
                0, // NpcTextId
                GOSSIP_ICON_TABARD,
                ACTION_TYPE_VEHICLE,
                vehicleData.SubscriptionLevelRequired
                });
        } while (result->NextRow());
    }
}

void Smartstone::LoadMounts()
{
    // Load mounts from the database
    QueryResult result = WorldDatabase.Query("SELECT Id, ModelId, Description, SubscriptionLevel FROM smartstone_mounts WHERE Enabled = 1");
    SmartstoneMountData mountData;
    Mounts.clear();
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            mountData.Id = fields[0].Get<uint32>();
            mountData.ModelID = fields[1].Get<uint32>();
            mountData.Description = fields[2].Get<std::string>();
            mountData.SubscriptionLevelRequired = fields[3].Get<uint8>();
            Mounts.push_back(mountData);
            MenuItems[ACTION_TYPE_MOUNT].push_back(MenuItem{
                mountData.Id,
                mountData.Description,
                0, // NpcTextId
                GOSSIP_ICON_TABARD,
                ACTION_TYPE_MOUNT,
                mountData.SubscriptionLevelRequired
                });
        } while (result->NextRow());
    }
}

void Smartstone::LoadLegacyCostumes()
{
    LegacyCostumeItemToDisplayId.clear();

    if (!IsCostumeConvertEnabled())
        return;

    QueryResult result = WorldDatabase.Query("SELECT item_entry, display_id FROM costume");
    if (!result)
    {
        LOG_WARN("smartstone", "Legacy costume table 'costume' is empty. No costumes available for conversion.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        uint32 entry = fields[0].Get<uint32>();
        uint32 displayId = fields[1].Get<uint32>();
        LegacyCostumeItemToDisplayId[entry] = displayId;
    } while (result->NextRow());

    LOG_INFO("smartstone", "Loaded {} legacy costume entries for conversion.", LegacyCostumeItemToDisplayId.size());
}

void Smartstone::LoadCostumeSounds()
{
    CostumeSounds.clear();
    QueryResult result = WorldDatabase.Query("SELECT DisplayId, Sound, Mode FROM smartstone_costume_sound");
    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();
        uint32 displayId = fields[0].Get<uint32>();
        CostumeSounds[displayId] = SmartstoneCostumeSoundData{
            fields[1].Get<uint32>(),
            fields[2].Get<uint8>()
        };
    } while (result->NextRow());
}

void Smartstone::LoadSmartstoneData()
{
    if (sSmartstone->IsSmartstoneEnabled())
    {
        sSmartstone->MenuStateHolder.clear();
        sSmartstone->MenuItems.clear();
        sSmartstone->LoadServices();
        sSmartstone->LoadPets();
        sSmartstone->LoadCostumes();
        sSmartstone->LoadCostumeSounds();
        sSmartstone->LoadServiceExpirationInfo();
        sSmartstone->LoadCategories();
        sSmartstone->LoadAuras();
        sSmartstone->LoadVehicles();
        sSmartstone->LoadMounts();
        sSmartstone->LoadLegacyCostumes();
        sSmartstone->LoadPerks();
    }
}
