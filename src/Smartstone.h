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

    // Class Perks: parent category + 10 hardcoded per-class child IDs.
    // Class-gating is enforced in C++ (see GetClassPerkCategoryForClass).
    CATEGORY_CLASS_PERKS         = 1000,
    CATEGORY_CLASS_PERKS_WARRIOR = 1001,
    CATEGORY_CLASS_PERKS_PALADIN = 1002,
    CATEGORY_CLASS_PERKS_HUNTER  = 1003,
    CATEGORY_CLASS_PERKS_ROGUE   = 1004,
    CATEGORY_CLASS_PERKS_PRIEST  = 1005,
    CATEGORY_CLASS_PERKS_DK      = 1006,
    CATEGORY_CLASS_PERKS_SHAMAN  = 1007,
    CATEGORY_CLASS_PERKS_MAGE    = 1008,
    CATEGORY_CLASS_PERKS_WARLOCK = 1009,
    CATEGORY_CLASS_PERKS_DRUID   = 1010,

    // Completed tiers settings
    SETTING_BLACK_TEMPLE = 0,
    SETTING_ZULAMAN      = 1,
    SETTING_SSC          = 2,
    SETTING_HYJAL        = 3,
    SETTING_SWP          = 4,

    SETTING_CURR_COSTUME = 0,
    SETTING_CURR_AURA    = 1
};

// Per-player setting indices for the "#druid_form" namespace.
// Value at each slot is the displayId to apply when the form is taken,
// or 0 to leave the default model.
enum DruidFormSlot
{
    DRUID_FORM_BEAR    = 0,
    DRUID_FORM_CAT     = 1,
    DRUID_FORM_TRAVEL  = 2,
    DRUID_FORM_FLIGHT  = 3,
    DRUID_FORM_AQUATIC = 4,
    DRUID_FORM_TREE    = 5,
    MAX_DRUID_FORM_SLOTS
};

// Account-setting slot range for class perks. One slot per class so
// each class's unlocks live in their own PlayerSettingVector. Chosen
// above the ACTION_TYPE_* range (0-10) to avoid collision.
enum SmartstonePerkAccountSetting
{
    SETTING_PERK_BASE    = 100,
    SETTING_PERK_WARRIOR = SETTING_PERK_BASE + 1,
    SETTING_PERK_PALADIN = SETTING_PERK_BASE + 2,
    SETTING_PERK_HUNTER  = SETTING_PERK_BASE + 3,
    SETTING_PERK_ROGUE   = SETTING_PERK_BASE + 4,
    SETTING_PERK_PRIEST  = SETTING_PERK_BASE + 5,
    SETTING_PERK_DK      = SETTING_PERK_BASE + 6,
    SETTING_PERK_SHAMAN  = SETTING_PERK_BASE + 7,
    SETTING_PERK_MAGE    = SETTING_PERK_BASE + 8,
    SETTING_PERK_WARLOCK = SETTING_PERK_BASE + 9,
    SETTING_PERK_DRUID   = SETTING_PERK_BASE + 11,
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
    SMARTSTONE_ACTION_RESET_CAT_FORM         = 8,
    SMARTSTONE_ACTION_RESET_BEAR_FORM        = 9,
    SMARTSTONE_ACTION_RESET_TRAVEL_FORM      = 10,
    SMARTSTONE_ACTION_RESET_FLIGHT_FORM      = 11,
    SMARTSTONE_ACTION_RESET_AQUATIC_FORM     = 12,
    SMARTSTONE_ACTION_RESET_TREE_FORM        = 13,
    MAX_SMARTSTONE_ACTIONS
};

enum Service
{
    SERVICE_BARBERSHOP        = 1,
    SERVICE_XP_RATE_2X        = 2,
    SERVICE_XP_RATE_1X        = 3,
    SERVICE_XP_RATE_DISABLED  = 4,
    SERVICE_JOYOUS_JOURNEYS   = 5,
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
    ACTION_TYPE_PERK      = 9, // Class-gated perks (placeholder action for now)
    ACTION_TYPE_NONE      = 10, // No action type, used for invalid or uninitialized
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

// Numeric discriminator for perk dispatch. Keep in sync with the
// `Effect` column of smartstone_perks and the switch in OnGossipSelect.
enum SmartstonePerkEffect : uint8
{
    PERK_EFFECT_NONE                = 0,
    PERK_EFFECT_DRUID_FORM_BEAR     = 1,
    PERK_EFFECT_DRUID_FORM_CAT      = 2,
    PERK_EFFECT_DRUID_FORM_TRAVEL   = 3,
    PERK_EFFECT_DRUID_FORM_FLIGHT   = 4,
    PERK_EFFECT_DRUID_FORM_AQUATIC  = 5,
    PERK_EFFECT_DRUID_FORM_TREE     = 6,
};

struct SmartstonePerkData
{
    uint32 Id;
    std::string Title;
    std::string Description;
    uint8 ClassId; // matches Player::getClass() (Classes enum); 0 = any
    uint32 Category;
    uint8 Effect;  // SmartstonePerkEffect
    uint32 Value;  // effect-specific payload (e.g. a displayId)
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
    // Cached on startup from mod-weekend-xp's XPWeekend.IsJoyousJourneysActive
    // config so the Joyous Journeys toggle button can hide itself when the
    // event is off, without re-reading sConfigMgr at runtime.
    bool JoyousJourneysActive{ false };
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

    void SetJoyousJourneysActive(bool active) { JoyousJourneysActive = active; }
    [[nodiscard]] bool IsJoyousJourneysActive() const { return JoyousJourneysActive; }

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
            case ACTION_TYPE_PERK: return "Perk";
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
            case ACTION_TYPE_PERK: return "Class-gated perk.";
            default: return "No action type.";
        }
    }

    [[nodiscard]] static uint32 GetClassPerkCategoryForClass(uint8 cls)
    {
        switch (cls)
        {
            case 1:  return CATEGORY_CLASS_PERKS_WARRIOR;
            case 2:  return CATEGORY_CLASS_PERKS_PALADIN;
            case 3:  return CATEGORY_CLASS_PERKS_HUNTER;
            case 4:  return CATEGORY_CLASS_PERKS_ROGUE;
            case 5:  return CATEGORY_CLASS_PERKS_PRIEST;
            case 6:  return CATEGORY_CLASS_PERKS_DK;
            case 7:  return CATEGORY_CLASS_PERKS_SHAMAN;
            case 8:  return CATEGORY_CLASS_PERKS_MAGE;
            case 9:  return CATEGORY_CLASS_PERKS_WARLOCK;
            case 11: return CATEGORY_CLASS_PERKS_DRUID;
            default: return 0;
        }
    }

    [[nodiscard]] static bool IsClassPerkSubcategory(uint32 categoryId)
    {
        return categoryId >= CATEGORY_CLASS_PERKS_WARRIOR
            && categoryId <= CATEGORY_CLASS_PERKS_DRUID;
    }

    // Account-setting slot for a given player class (Classes enum values).
    // Returns 0 for unsupported classes (shouldn't happen in WotLK).
    [[nodiscard]] static uint32 GetPerkAccountSettingForClass(uint8 cls)
    {
        switch (cls)
        {
            case 1:  return SETTING_PERK_WARRIOR;
            case 2:  return SETTING_PERK_PALADIN;
            case 3:  return SETTING_PERK_HUNTER;
            case 4:  return SETTING_PERK_ROGUE;
            case 5:  return SETTING_PERK_PRIEST;
            case 6:  return SETTING_PERK_DK;
            case 7:  return SETTING_PERK_SHAMAN;
            case 8:  return SETTING_PERK_MAGE;
            case 9:  return SETTING_PERK_WARLOCK;
            case 11: return SETTING_PERK_DRUID;
            default: return 0;
        }
    }

    // Druid form display overrides (per-character).
    void SetDruidFormDisplay(Player* player, uint8 slot, uint32 displayId)
    {
        player->UpdatePlayerSetting(ModName + "#druid_form", slot, displayId);
    }
    [[nodiscard]] uint32 GetDruidFormDisplay(Player* player, uint8 slot) const
    {
        return player->GetPlayerSetting(ModName + "#druid_form", slot).value;
    }

    // If the player is currently shapeshifted into the form matching
    // `slot`, restore the canonical model for that form/race/gender so
    // a freshly cleared override is reflected immediately (otherwise
    // the visual update only happens on the next form change).
    void RestoreDefaultFormDisplay(Player* player, uint8 slot)
    {
        if (!player)
            return;

        ShapeshiftForm form = player->GetShapeshiftForm();
        uint8 currentSlot;
        uint32 spellId;
        switch (form)
        {
            case FORM_BEAR:        currentSlot = DRUID_FORM_BEAR;   spellId =  5487; break;
            case FORM_DIREBEAR:    currentSlot = DRUID_FORM_BEAR;   spellId =  9634; break;
            case FORM_CAT:         currentSlot = DRUID_FORM_CAT;    spellId =   768; break;
            case FORM_TRAVEL:      currentSlot = DRUID_FORM_TRAVEL; spellId =   783; break;
            case FORM_FLIGHT:      currentSlot = DRUID_FORM_FLIGHT; spellId = 33943; break;
            case FORM_FLIGHT_EPIC: currentSlot = DRUID_FORM_FLIGHT; spellId = 40120; break;
            case FORM_AQUA:        currentSlot = DRUID_FORM_AQUATIC; spellId = 1066; break;
            case FORM_TREE:        currentSlot = DRUID_FORM_TREE;    spellId = 33891; break;
            default:               return;
        }

        if (currentSlot != slot)
            return;

        if (uint32 model = player->GetModelForForm(form, spellId))
            player->SetDisplayId(model);
    }

    // Re-apply the player's saved druid-form displayId override if they
    // are currently shapeshifted. Used on login / map change, where the
    // engine reapplies the form aura after our AuraScript would normally
    // fire — causing the default model to overwrite our override.
    void ReapplyActiveDruidFormDisplay(Player* player)
    {
        if (!player)
            return;

        ShapeshiftForm form = player->GetShapeshiftForm();
        uint8 slot;
        uint32 spellId;
        switch (form)
        {
            case FORM_BEAR:        slot = DRUID_FORM_BEAR;   spellId =  5487; break;
            case FORM_DIREBEAR:    slot = DRUID_FORM_BEAR;   spellId =  9634; break;
            case FORM_CAT:         slot = DRUID_FORM_CAT;    spellId =   768; break;
            case FORM_TRAVEL:      slot = DRUID_FORM_TRAVEL; spellId =   783; break;
            case FORM_FLIGHT:      slot = DRUID_FORM_FLIGHT; spellId = 33943; break;
            case FORM_FLIGHT_EPIC: slot = DRUID_FORM_FLIGHT; spellId = 40120; break;
            case FORM_AQUA:        slot = DRUID_FORM_AQUATIC; spellId = 1066; break;
            case FORM_TREE:        slot = DRUID_FORM_TREE;    spellId = 33891; break;
            default:               return;
        }

        // Inside BG / arena: force the canonical model regardless of
        // any stored override (matches the spell-script gate).
        if (player->InBattleground() || player->InArena())
        {
            if (uint32 model = player->GetModelForForm(form, spellId))
                player->SetDisplayId(model);
            return;
        }

        if (uint32 displayId = GetDruidFormDisplay(player, slot))
            player->SetDisplayId(displayId);
    }

    [[nodiscard]] std::string GetServiceName(uint8 serviceType) const;
    [[nodiscard]] std::string GetServiceDescription(uint8 serviceType) const;

    [[nodiscard]] uint32 GetNPCTextForService(uint32 type, uint8 subscriptionLevel) const;

    [[nodiscard]] uint32 GetNPCTextForCategory(uint32 type, uint32 category) const;
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
    void LoadPerks();

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
    [[nodiscard]] SmartstonePerkData GetPerkData(uint32 id) const;

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
    std::unordered_map<uint32, std::vector<SmartstonePerkData>> Perks;
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
    // Perk messages (62-63)
    LANG_MOD_PERK_APPLIED              = 62,
    LANG_MOD_PERK_NO_IMPL              = 63,
    // Experience-rate messages (64-67)
    LANG_MOD_XP_RATE_2X                = 64,
    LANG_MOD_XP_RATE_1X                = 65,
    LANG_MOD_XP_RATE_DISABLED          = 66,
    LANG_MOD_XP_RATE_ENABLED           = 67,
    // Joyous Journeys toggle (68-69)
    LANG_MOD_JOYOUS_JOURNEYS_ENABLED   = 68,
    LANG_MOD_JOYOUS_JOURNEYS_DISABLED  = 69,
};

#define sSmartstone Smartstone::instance()

#endif
