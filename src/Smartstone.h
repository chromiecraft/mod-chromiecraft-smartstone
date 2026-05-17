#ifndef DEF_SMARTSTONE_H
#define DEF_SMARTSTONE_H

#include "Player.h"
#include "Config.h"
#include "InstanceScript.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include <limits>    // per std::numeric_limits

enum Misc
{

    CATEGORY_MAIN = 0,

    // Completed tiers settings
    SETTING_BLACK_TEMPLE = 0,
    SETTING_ZULAMAN      = 1,
    SETTING_SSC          = 2,
    SETTING_HYJAL        = 3,
    SETTING_SWP          = 4,

    SETTING_CURR_COSTUME = 0,
    SETTING_CURR_AURA    = 1
};

enum UtilActions
{
    SMARTSTONE_ACTION_NONE                   = 0,
    SMARTSTONE_ACTION_UNSUMMON_PET           = 1,
    SMARTSTONE_ACTION_UNSUMMON_COMPANION     = 2,
    SMARTSTONE_ACTION_REMOVE_COSTUME         = 3,
    SMARTSTONE_ACTION_LAST_PAGE              = 4,
    SMARTSTONE_ACTION_NEXT_PAGE              = 5,
    SMARTSTONE_ACTION_BACK                   = 6,
    SMARTSTONE_ACTION_REMOVE_AURA            = 7,
    MAX_SMARTSTONE_ACTIONS
};

enum Service
{
    SERVICE_BARBERSHOP = 1,
};

/**
 * @brief This is the ID used to identify the type of service.
 * This id is also used by the command to unlock the service.
 */
enum ActionType
{
    ACTION_TYPE_COMPANION = 0,
    ACTION_TYPE_PET       = 1,
    ACTION_TYPE_COSTUME   = 2,
    ACTION_TYPE_CATEGORY  = 3,
    ACTION_TYPE_SERVICE   = 4, // Character services like barber, name change, etc.
    ACTION_TYPE_UTIL      = 5, // Utility actions like next/previous page, back,
    ACTION_TYPE_AURA      = 6,
    ACTION_TYPE_VEHICLE   = 7,
    ACTION_TYPE_MOUNT     = 8,
    ACTION_TYPE_NONE      = 9, // No action type, used for invalid or uninitialized
    MAX_ACTION_TYPE,
};

constexpr uint32_t ID_RANGE_PER_TYPE = std::numeric_limits<uint32_t>::max() / MAX_ACTION_TYPE;

/**
 * @brief This enum is used to identify the type of pet.
 * It must match the service type.
 */
enum STSPetType
{
    PET_TYPE_COMPANION = ACTION_TYPE_COMPANION,
    PET_TYPE_COMBAT     = ACTION_TYPE_PET,
};

enum SmartstoneVehicleFlags
{
    SMARTSTONE_VEHICLE_FLAG_NONE   = 0x0,
    SMARTSTONE_VEHICLE_FLAG_FLY    = 0x1,
    SMARTSTONE_VEHICLE_FLAG_FOLLOW = 0x2,
};

struct MenuItem
{
    uint32 ItemId;
    std::string Text;
    uint32 NpcTextId;
    uint32 Icon;
    ActionType ServiceType;
    uint8 SubscriptionLevelRequired;
};

struct SmartstonePetData
{
    uint32 CreatureId;
    std::string Description;
    uint32 Duration;
    STSPetType Type;
    uint32 Category;
    uint8 SubscriptionLevelRequired;

public:
    // Low id: used to store player settings
    [[nodiscard]] uint32 GetId() const { return CreatureId - (Type == PET_TYPE_COMPANION ? 80000 : 90000); };
};

struct SmartstoneCostumeData
{
    uint32 Id;
    uint32 DisplayId;
    float Scale;
    std::string Description;
    uint32 Duration;
    uint32 Cooldown;
    uint8 SubscriptionLevelRequired;
};

struct SmartstoneService
{
    uint8 Id;
    uint32 Category;
    std::string ServiceTitle;
    uint8 SubscriptionLevelRequired;
};

struct SmartstoneCategoryData
{
    uint32 Id;
    uint32 ParentId;
    uint8 Type;
    std::string Title;
    uint8 SubscriptionLevelRequired;
    uint32 NpcTextId;
};

struct SmartstoneServiceExpireInfo
{
    uint32 ServiceId;
    uint32 PlayerGUID;
    uint8 ServiceType;
    uint32 ActivationTime;
    uint32 ExpirationTime;
};

struct SmartstoneAuraData
{
    uint32 Id;
    uint32 SpellID;
    std::string Description;
    uint8 SubscriptionLevelRequired;
};

struct SmartstoneVehicleData
{
    uint32 Id;
    uint32 CreatureId;
    uint32 Flags;
    std::string Description;
    uint8 SubscriptionLevelRequired;

    [[nodiscard]] bool HasFlag(uint32 flag) const { return (Flags & flag) != 0; };
};

struct SmartstoneMountData
{
    uint32 Id;
    uint32 ModelID;
    std::string Description;
    uint8 SubscriptionLevelRequired;
};

struct SmartstoneCostumeSoundData
{
    uint32 Sound;
    uint8 Mode; // 1 = self only, 2 = surround
};

struct SmartstoneMenuState
{
    uint16 currentPage;
    uint32 lastCategory;

    SmartstoneMenuState() : currentPage(0), lastCategory(0) { }
    SmartstoneMenuState(uint16 page, uint32 category) : currentPage(page), lastCategory(category) { }
};

enum Settings
{
    SETTING_MEMBERSHIP_LEVEL
};

struct ActionKey
{
    ActionType type;
    uint32_t actionId;
};

const std::string ModName = "mod-cc-smartstone";
const std::string SubsModName = "acore_cms_subscriptions";

class Smartstone
{
private:
    uint32 SmartstoneItemID = 0;
    bool IsEnabled{ true };
    bool CanUseInArena{ false };
    bool CanUseInBG{ false };
    bool CanUseInCombat{ false };
    bool CanUseInPvP{ false };
    bool IsDebugEnabled{ false };
    bool IndividualCostumeCooldowns{ false };
    bool CostumeConvertEnabled{ false };
    Seconds BarberDuration = 1min;
public:
    static Smartstone* instance();

    bool IsPetAvailable(Player* player, SmartstonePetData service, uint8 subscriptionLevel) const;

    bool CanUseSmartstone(Player* player) const { return !player->GetMap()->IsDungeon() && !player->GetMap()->IsBattlegroundOrArena(); }

    void SetEnabled(bool enabled) { IsEnabled = enabled; }
    [[nodiscard]] bool IsSmartstoneEnabled() const { return IsEnabled; }

    void SetCanUseInArena(bool enabled) { CanUseInArena = enabled; }
    [[nodiscard]] bool IsSmartstoneCanUseInArena() const { return CanUseInArena; }

    void SetCanUseInBG(bool enabled) { CanUseInBG = enabled; }
    [[nodiscard]] bool IsSmartstoneCanUseInBG() const { return CanUseInBG; }

    void SetCanUseInCombat(bool enabled) { CanUseInCombat = enabled; }
    [[nodiscard]] bool IsSmartstoneCanUseInCombat() const { return CanUseInCombat; }

    void SetCanUseInPvP(bool enabled) { CanUseInPvP = enabled; }
    [[nodiscard]] bool IsSmartstoneCanUseInPvP() const { return CanUseInPvP; }

    void SetBarberDuration(Seconds duration) { BarberDuration = duration; }
    [[nodiscard]] Seconds GetBarberDuration() const { return BarberDuration; }

    void SetSmartstoneItemID(uint32 itemId) { SmartstoneItemID = itemId; }
    [[nodiscard]] uint32 GetSmartstoneItemID() const { return SmartstoneItemID; }

    void SetCurrentCostume(Player* player, uint32 costumeId) { player->UpdatePlayerSetting(ModName + "#misc", SETTING_CURR_COSTUME, costumeId); }
    [[nodiscard]] uint32 GetCurrentCostume(Player* player) const { return player->GetPlayerSetting(ModName + "#misc", SETTING_CURR_COSTUME).value; }

    void SetCurrentAura(Player* player, uint32 spellId) { player->UpdatePlayerSetting(ModName + "#misc", SETTING_CURR_AURA, spellId); }
    [[nodiscard]] uint32 GetCurrentAura(Player* player) const { return player->GetPlayerSetting(ModName + "#misc", SETTING_CURR_AURA).value; };

    void SetDebugEnabled(bool enabled) { IsDebugEnabled = enabled; }
    [[nodiscard]] bool IsSmartstoneDebugEnabled() const { return IsDebugEnabled; }

    void SetIndividualCostumeCooldowns(bool enabled) { IndividualCostumeCooldowns = enabled; }
    [[nodiscard]] bool HasIndividualCostumeCooldowns() const { return IndividualCostumeCooldowns; }

    void SetCostumeConvertEnabled(bool enabled) { CostumeConvertEnabled = enabled; }
    [[nodiscard]] bool IsCostumeConvertEnabled() const { return CostumeConvertEnabled; }

    void SetCostumeCooldown(Player* player, uint32 costumeId);
    [[nodiscard]] bool HasCostumeCooldown(Player* player, uint32 costumeId) const;
    [[nodiscard]] uint32 GetCostumeCooldownRemaining(Player* player, uint32 costumeId) const;

    void ApplyCostume(Player* player, uint32 costumeId);

    [[nodiscard]] constexpr uint32_t GetActionTypeBaseId(ActionType type)
    {
        return static_cast<uint32_t>(type) * ID_RANGE_PER_TYPE;
    }
    [[nodiscard]] constexpr uint32_t GetActionTypeId(ActionType type, uint32_t id)
    {
        return GetActionTypeBaseId(type) + id;
    }

    [[nodiscard]]
    constexpr std::optional<ActionKey> DecodeActionId(uint32_t fullId)
    {
        constexpr uint32_t RANGE = ID_RANGE_PER_TYPE;
        uint32_t typeIndex = fullId / RANGE;
        if (typeIndex >= MAX_ACTION_TYPE)
            return std::nullopt;

        return ActionKey{
            static_cast<ActionType>(typeIndex),
            fullId % RANGE
        };
    }

    [[nodiscard]] std::string GetActionTypeName(ActionType type) const
    {
        switch (type)
        {
            case ACTION_TYPE_COMPANION: return "Companion";
            case ACTION_TYPE_PET: return "Pet";
            case ACTION_TYPE_COSTUME: return "Costume";
            case ACTION_TYPE_CATEGORY: return "Category";
            case ACTION_TYPE_SERVICE: return "Service";
            case ACTION_TYPE_UTIL: return "Utility";
            case ACTION_TYPE_VEHICLE: return "Vehicles";
            case ACTION_TYPE_MOUNT: return "Mounts";
            default: return "None";
        }
    }

    [[nodiscard]] std::string GetActionTypeDescription(ActionType type) const
    {
        switch (type)
        {
            case ACTION_TYPE_COMPANION: return "Companion service for pets.";
            case ACTION_TYPE_PET: return "Combat pet service.";
            case ACTION_TYPE_COSTUME: return "Costume service.";
            case ACTION_TYPE_CATEGORY: return "Category service.";
            case ACTION_TYPE_SERVICE: return "Character services like barber, name change, etc.";
            case ACTION_TYPE_UTIL: return "Utility actions like next/previous page, back.";
            case ACTION_TYPE_VEHICLE: return "Vehicles service.";
            case ACTION_TYPE_MOUNT: return "Mounts service.";
            default: return "No action type.";
        }
    }

    [[nodiscard]] std::string GetServiceName(uint8 serviceType) const;
    [[nodiscard]] std::string GetServiceDescription(uint8 serviceType) const;

    [[nodiscard]] uint32 GetNPCTextForService(uint32 type, uint8 subscriptionLevel) const;

    [[nodiscard]] uint32 GetNPCTextForCategory(uint32 type, uint8 category) const;
    [[nodiscard]] bool IsServiceAvailable(Player* player, std::string service, uint32 serviceId) const;

    void LoadSmartstoneData();
    void LoadServices();
    void LoadPets();
    void LoadCostumes();
    void LoadServiceExpirationInfo();
    void LoadCategories();
    void LoadAuras();
    void LoadVehicles();
    void LoadMounts();
    void LoadLegacyCostumes();
    void LoadCostumeSounds();

    void ProcessExpiredServices(Player* player);

    std::string GetModuleStringForService(uint8 serviceType) const;

    [[nodiscard]] Milliseconds GetCostumeDuration(Player* player, uint32 duration) const;

    [[nodiscard]] SmartstoneServiceExpireInfo GetServiceExpireInfo(uint32 playerGUID, uint32 serviceId, uint8 category) const;
    [[nodiscard]] SmartstonePetData GetPetData(uint32 creatureId, uint8 serviceType = ACTION_TYPE_COMPANION) const;
    [[nodiscard]] SmartstoneCostumeData GetCostumeData(uint32 id) const;
    [[nodiscard]] SmartstoneCostumeData GetCostumeDataByDisplayId(uint32 displayId) const;
    [[nodiscard]] SmartstoneAuraData GetAuraData(uint32 id) const;
    [[nodiscard]] SmartstoneVehicleData GetVehicleData(uint32 id) const;
    [[nodiscard]] SmartstoneMountData GetMountData(uint32 id) const;

    std::unordered_map<uint32, uint32> LegacyCostumeItemToDisplayId;

    std::vector<SmartstonePetData> Pets;
    std::vector<SmartstonePetData> CombatPets;
    std::vector<SmartstoneAuraData> Auras;
    std::vector<SmartstoneVehicleData> Vehicles;
    std::vector<SmartstoneMountData> Mounts;

    std::unordered_map<uint32, std::vector<MenuItem>> MenuItems;
    std::unordered_map<uint32, std::vector<SmartstoneCostumeData>> Costumes;
    std::unordered_map<uint32, SmartstoneCostumeSoundData> CostumeSounds; // keyed by DisplayId
    std::unordered_map<uint32, std::vector<SmartstoneCategoryData>> Categories;
    std::unordered_map<uint32, std::vector<SmartstoneService>> Services;
    std::unordered_map<ObjectGuid, std::vector<SmartstoneMenuState>> MenuStateHolder;
    std::map<uint32, std::list<SmartstoneServiceExpireInfo>> ServiceExpireInfo;

    std::vector<SmartstoneMenuState> GetMenuStates(const ObjectGuid& guid) { return MenuStateHolder[guid]; }
    typedef std::unordered_map<uint32, PlayerSettingVector> AccountSettingsMap;
    std::unordered_map<uint32, AccountSettingsMap> AccountSettings;

    PlayerSetting GetAccountSetting(uint32 accountId, uint32 service, uint32 index);
    void UpdateAccountSetting(uint32 accountId, uint32 service, uint32 index, uint32 value);
    void LoadAccountSettings(uint32 accountId);
    void ClearAccountSettings(uint32 accountId) { AccountSettings.erase(accountId); }

    void removeCurrentAura(Player* player)
    {
        if (uint32 spellId = GetCurrentAura(player))
        {
            player->RemoveAurasDueToSpell(spellId);
            SetCurrentAura(player, 0);
        }
    }

    void ApplyCostume(Player* player, SmartstoneCostumeData const& costume);
    void PlayCostumeSound(Player* player, uint32 displayId);

    [[nodiscard]] uint32 GetShortId(uint32 fullId, uint8 type) const
    {
        switch (type)
        {
            case ACTION_TYPE_COMPANION:
                return fullId - 80000;
            case ACTION_TYPE_PET:
                return fullId - 90000;
            case ACTION_TYPE_COSTUME:
                return fullId - 20000;
            case ACTION_TYPE_VEHICLE:
                return fullId - 70000;
        }

        return fullId;
    }
};

enum SmartstoneStringId : uint32
{
    // Item / gossip / player action messages
    LANG_MOD_INVALID_ACTION             = 1,
    LANG_MOD_NO_BG_ARENA               = 2,
    LANG_MOD_NO_COMBAT                 = 3,
    LANG_MOD_NO_PVP                    = 4,
    LANG_MOD_BARBER_UNAVAILABLE        = 5,
    LANG_MOD_NO_INSTANCE_BG            = 6,
    LANG_MOD_BARBER_ALREADY_SUMMONED   = 7,
    LANG_MOD_COSTUME_COOLDOWN_MINS     = 8,
    LANG_MOD_AURA_ALREADY_ACTIVE       = 9,
    LANG_MOD_AURA_NOW_ACTIVE           = 10,
    LANG_MOD_NO_VEHICLE_COMBAT         = 11,
    LANG_MOD_NEED_RIDING_SKILL         = 12,
    LANG_MOD_NO_FLY_HERE               = 13,
    LANG_MOD_MUST_BE_MOUNTED           = 14,
    LANG_MOD_INVALID_ACTION_TYPE       = 15,
    LANG_MOD_NOT_CONFIGURED            = 16,
    LANG_MOD_CATEGORY_NOT_FOUND        = 17,
    LANG_MOD_NO_ACTIONS                = 18,
    // Singleton system messages
    LANG_MOD_PET_WANES                 = 19,
    // Command messages
    LANG_MOD_DISABLED                  = 20,
    LANG_MOD_COSTUME_NOT_EXIST         = 21,
    LANG_MOD_NO_ACCESS                 = 22,
    LANG_MOD_COSTUME_NO_BG_ARENA       = 23,
    LANG_MOD_COSTUME_NO_COMBAT         = 24,
    LANG_MOD_COSTUME_NO_PVP            = 25,
    LANG_MOD_COSTUME_ON_COOLDOWN       = 26,
    LANG_MOD_COSTUME_APPLIED           = 27,
    LANG_MOD_PET_NOT_EXIST             = 28,
    LANG_MOD_NO_COMBAT_PET_INSTANCE    = 29,
    LANG_MOD_CONSOLE_NEEDS_PLAYER      = 30,
    LANG_MOD_LOOKUP_PET_FORMAT         = 31,
    LANG_MOD_NO_PETS_AVAILABLE         = 32,
    LANG_MOD_NO_PETS_MATCHING_FOR      = 33,
    LANG_MOD_LOOKUP_COSTUME_FORMAT     = 34,
    LANG_MOD_NO_COSTUMES_AVAILABLE     = 35,
    LANG_MOD_NO_COSTUMES_MATCHING_FOR  = 36,
    LANG_MOD_COOLDOWN_REMAINING        = 37,
    LANG_MOD_ALL_COOLDOWN_REMAINING    = 38,
    LANG_MOD_GLOBAL_CD_OFFLINE         = 39,
    LANG_MOD_NO_COSTUME_COOLDOWNS      = 40,
    LANG_MOD_NO_PETS_LOADED            = 41,
    LANG_MOD_NO_PETS_MATCHING          = 42,
    LANG_MOD_CONVERT_UNAVAILABLE       = 43,
    LANG_MOD_NO_LEGACY_COSTUMES        = 44,
    LANG_MOD_NO_MATCHING_COSTUME       = 45,
    LANG_MOD_ITEM_ALREADY_UNLOCKED     = 46,
    LANG_MOD_CONVERTED                 = 47,
    LANG_MOD_NO_CONVERTIBLE            = 48,
    LANG_MOD_ALREADY_HAVE_STONE        = 49,
    LANG_MOD_SERVICE_ALREADY_UNLOCKED  = 50,
    LANG_MOD_SERVICE_ALREADY_LOCKED    = 51,
    LANG_MOD_SERVICE_BEEN_UNLOCKED     = 52,
    LANG_MOD_SERVICE_BEEN_REMOVED      = 53,
    LANG_MOD_PET_SERVICE_NOT_EXIST     = 54,
    LANG_MOD_COSTUME_SERVICE_NOT_EXIST = 55,
    LANG_MOD_AURA_NOT_EXIST            = 56,
    LANG_MOD_VEHICLE_NOT_EXIST         = 57,
    LANG_MOD_UNKNOWN_SERVICE_TYPE      = 58,
    LANG_MOD_DATA_RELOADED             = 59,
    // Costume removal notifications
    LANG_MOD_COSTUME_EXPIRED           = 60,
    LANG_MOD_COSTUME_REMOVED_BG_ARENA  = 61,
};

#define sSmartstone Smartstone::instance()

#endif
