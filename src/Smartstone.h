#ifndef DEF_SMARTSTONE_H
#define DEF_SMARTSTONE_H

#include "Player.h"
#include "Config.h"
#include "InstanceScript.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"

enum Misc
{
    ACTION_RANGE_COSTUMES_CATEGORIES = 10000,
    ACTION_RANGE_COSTUMES            = 20000,
    ACTION_RANGE_SUMMON_PET          = 80000,
    ACTION_RANGE_SUMMON_COMBAT_PET   = 90000,

    // Completed tiers settings
    SETTING_BLACK_TEMPLE = 0,
    SETTING_ZULAMAN      = 1,
    SETTING_SSC          = 2,
    SETTING_HYJAL        = 3,
    SETTING_SWP          = 4,

    SETTING_CURR_COSTUME = 0
};

enum StoneActions
{
    SMARTSTONE_ACTION_BARBERSHOP             = 1,
    SMARTSTONE_ACTION_EXOTIC_PET_COLLECTION  = 2,
    SMARTSTONE_ACTION_LIMITED_DURATION_PETS  = 3,
    SMARTSTONE_ACTION_CHAR_SETTINGS          = 4,
    SMARTSTONE_ACTION_COSTUMES               = 5,
    MAX_SMARTSTONE_ACTIONS
};

enum ServiceCategory
{
    // catalogue categories
    CATEGORY_COSTUMES      = 0,

    // services
    SERVICE_CAT_PET        = 0,
    SERVICE_CAT_COMBAT_PET = 1,
    SERVICE_CAT_COSTUMES   = 2
};

struct SmartstonePetData
{
    uint32 CreatureId;
    std::string Description;
    uint32 Duration;
    uint8 Category;
    uint8 SubscriptionLevelRequired;

    uint32 GetServiceId() const { return Category == SERVICE_CAT_COMBAT_PET ?
        CreatureId - ACTION_RANGE_SUMMON_COMBAT_PET : CreatureId - ACTION_RANGE_SUMMON_PET; }
};

struct SmartstoneCostumeData
{
    uint32 Id;
    uint32 DisplayId;
    std::string Description;
    uint32 Duration;
    uint8 SubscriptionLevelRequired;
};

struct SmartstoneService
{
    uint8 Id;
    std::string ServiceTitle;
    uint8 SubscriptionLevelRequired;
};

struct SmartstoneCategoryData
{
    uint32 Id;
    uint8 Type;
    std::string Title;
    uint8 SubscriptionLevelRequired;
    uint32 NpcTextId;
};

struct SmartstoneServiceExpireInfo
{
    uint32 ServiceId;
    uint32 PlayerGUID;
    uint8 Category;
    uint32 ActivationTime;
    uint32 ExpirationTime;
};

enum Settings
{
    SETTING_MEMBERSHIP_LEVEL
};

const std::string ModName = "mod-cc-smartstone";
const std::string SubsModName = "acore_cms_subscriptions";

class Smartstone
{
private:
    uint32 SmartstoneItemID = 0;
    bool IsEnabled{ true };
    Seconds BarberDuration = 1min;
public:
    static Smartstone* instance();

    bool IsPetAvailable(Player* player, SmartstonePetData service, uint8 subscriptionLevel) const;

    bool CanUseSmartstone(Player* player) const { return !player->GetMap()->IsDungeon() && !player->GetMap()->IsBattlegroundOrArena(); }

    void SetEnabled(bool enabled) { IsEnabled = enabled; }
    [[nodiscard]] bool IsSmartstoneEnabled() { return IsEnabled; }

    void SetBarberDuration(Seconds duration) { BarberDuration = duration; }
    [[nodiscard]] Seconds GetBarberDuration() { return BarberDuration; }

    void SetSmartstoneItemID(uint32 itemId) { SmartstoneItemID = itemId; }
    [[nodiscard]] uint32 GetSmartstoneItemID() { return SmartstoneItemID; }

    void SetCurrentCostume(Player* player, uint32 costumeId) { player->UpdatePlayerSetting(ModName + "#costume", SETTING_CURR_COSTUME, costumeId); }
    [[nodiscard]] uint32 GetCurrentCostume(Player* player) { return player->GetPlayerSetting(ModName + "#costume", SETTING_CURR_COSTUME).value; }

    [[nodiscard]] uint32 GetNPCTextForCategory(uint32 type, uint8 category) const;
    [[nodiscard]] bool IsServiceAvailable(Player* player, std::string service, uint32 serviceId) const;

    void LoadServices();
    void LoadPets();
    void LoadCostumes();
    void LoadServiceExpirationInfo();
    void LoadCategories();

    void ProcessExpiredServices(Player* player);

    std::string GetModuleStringForCategory(uint8 category);

    [[nodiscard]] Milliseconds GetCostumeDuration(Player* player, uint32 duration) const;

    [[nodiscard]] SmartstoneServiceExpireInfo GetServiceExpireInfo(uint32 playerGUID, uint32 serviceId, uint8 category) const;
    [[nodiscard]] SmartstonePetData GetPetData(uint32 creatureId, uint8 category = SERVICE_CAT_PET) const;
    [[nodiscard]] SmartstoneCostumeData GetCostumeData(uint32 displayId) const;

    std::vector<SmartstonePetData> Pets;
    std::vector<SmartstonePetData> CombatPets;
    std::vector<SmartstoneService> Services;
    std::unordered_map<uint32, std::vector<SmartstoneCostumeData>> Costumes;
    std::unordered_map<uint32, std::vector<SmartstoneCategoryData>> Categories;
    std::map<uint32, std::list<SmartstoneServiceExpireInfo>> ServiceExpireInfo;
};

#define sSmartstone Smartstone::instance()

#endif
