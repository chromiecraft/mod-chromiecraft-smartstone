#ifndef DEF_SMARTSTONE_H
#define DEF_SMARTSTONE_H

#include "Player.h"
#include "Config.h"
#include "InstanceScript.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include <limits>    // per std::numeric_limits

class Creature;

enum Misc
{

    CATEGORY_MAIN = 0,

    // Display-options submenu under Character (smartstone_categories.Id 11);
    // rendered in C++ rather than from DB services.
    CATEGORY_DISPLAY_OPTIONS = 11,

    // Vouchers submenu under Character (smartstone_categories.Id 12). The list
    // is per-account and rendered in C++ from smartstone_account_vouchers, not
    // from DB menu items; the category only shows when unconsumed vouchers exist.
    CATEGORY_VOUCHERS = 12,

    // Module-integration subcategories under Character. Gated in the menu
    // builder against the matching cached server-side config.
    CATEGORY_XP_RATES               = 9,
    CATEGORY_SCROLL_OF_RESURRECTION = 10,

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
    SETTING_CURR_AURA    = 1,
    // Per-observer opt-outs (1 = hide that override category from this player).
    SETTING_HIDE_COSTUMES = 2, // costume morphs
    SETTING_HIDE_FORMS    = 3, // druid / shaman form-display perks
    SETTING_HIDE_MINIONS  = 4  // warlock pet / feral-spirit display perks
};

// Per-player slot indices for class display-override namespaces.
// Each slot stores a displayId (0 = use default model). Namespace is
// chosen by the helper used (#druid_form, #shaman_form, etc.).
enum DruidFormSlot
{
    DRUID_FORM_BEAR    = 0,
    DRUID_FORM_CAT     = 1,
    DRUID_FORM_TRAVEL  = 2,
    DRUID_FORM_FLIGHT  = 3,
    DRUID_FORM_AQUATIC = 4,
    DRUID_FORM_TREE    = 5,
    DRUID_FORM_MOONKIN = 6,
    MAX_DRUID_FORM_SLOTS
};

enum ShamanFormSlot
{
    SHAMAN_FORM_GHOST_WOLF = 0,
    MAX_SHAMAN_FORM_SLOTS
};

enum PriestFormSlot
{
    PRIEST_FORM_SPIRIT_OF_REDEMPTION = 0,
    PRIEST_FORM_SHADOW               = 1,
    MAX_PRIEST_FORM_SLOTS
};

// Guardians (TempSummons) rather than shapeshift forms — separate
// namespace so they can't collide with form slots.
enum ShamanGuardianSlot
{
    SHAMAN_GUARDIAN_FERAL_SPIRIT = 0,
    MAX_SHAMAN_GUARDIAN_SLOTS
};

enum WarlockPetSlot
{
    WARLOCK_PET_IMP        = 0,
    WARLOCK_PET_VOIDWALKER = 1,
    WARLOCK_PET_FELHUNTER  = 2,
    WARLOCK_PET_SUCCUBUS   = 3,
    WARLOCK_PET_FELGUARD   = 4,
    WARLOCK_PET_DOOMGUARD  = 5,
    MAX_WARLOCK_PET_SLOTS
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
    SMARTSTONE_ACTION_RESET_MOONKIN_FORM     = 14,
    SMARTSTONE_ACTION_RESET_GHOST_WOLF_FORM  = 15,
    // Per-pet warlock resets. Order matches WarlockPetSlot so we can
    // derive the slot from (actionId - SMARTSTONE_ACTION_RESET_WARLOCK_PET_IMP).
    SMARTSTONE_ACTION_RESET_WARLOCK_PET_IMP        = 16,
    SMARTSTONE_ACTION_RESET_WARLOCK_PET_VOIDWALKER = 17,
    SMARTSTONE_ACTION_RESET_WARLOCK_PET_FELHUNTER  = 18,
    SMARTSTONE_ACTION_RESET_WARLOCK_PET_SUCCUBUS   = 19,
    SMARTSTONE_ACTION_RESET_WARLOCK_PET_FELGUARD   = 20,
    SMARTSTONE_ACTION_RESET_WARLOCK_PET_DOOMGUARD  = 21,
    SMARTSTONE_ACTION_RESET_FERAL_SPIRITS          = 22,
    // Per-player "hide other players' X" view preferences.
    SMARTSTONE_ACTION_TOGGLE_HIDE_COSTUMES         = 23,
    SMARTSTONE_ACTION_TOGGLE_HIDE_FORMS            = 24,
    SMARTSTONE_ACTION_TOGGLE_HIDE_MINIONS          = 25,
    SMARTSTONE_ACTION_TOGGLE_TRANSMOG              = 26,
    SMARTSTONE_ACTION_RESET_SPIRIT_OF_REDEMPTION   = 27,
    SMARTSTONE_ACTION_RESET_SHADOWFORM             = 28,
    MAX_SMARTSTONE_ACTIONS
};

// One-shot character-service voucher kinds. Value is persisted in
// smartstone_account_vouchers.VoucherType — never renumber existing entries.
enum SmartstoneVoucherType : uint8
{
    VOUCHER_NONE           = 0,
    VOUCHER_RENAME         = 1, // AT_LOGIN_RENAME
    VOUCHER_FACTION_CHANGE = 2, // AT_LOGIN_CHANGE_FACTION
    VOUCHER_RACE_CHANGE    = 3, // AT_LOGIN_CHANGE_RACE
    VOUCHER_CUSTOMIZE      = 4, // AT_LOGIN_CUSTOMIZE
    MAX_VOUCHER_TYPE
};

// A single unconsumed voucher row for an account. Id is the primary key of
// smartstone_account_vouchers and doubles as the gossip action id.
struct SmartstoneVoucher
{
    uint32 Id;
    uint8 Type; // SmartstoneVoucherType
};

enum Service
{
    SERVICE_BARBERSHOP        = 1,
    SERVICE_XP_RATE_2X        = 2,
    SERVICE_XP_RATE_1X        = 3,
    SERVICE_XP_RATE_DISABLED  = 4,
    SERVICE_JOYOUS_JOURNEYS   = 5,
    SERVICE_RES_SCROLL_BONUS  = 6,
    SERVICE_RES_SCROLL_INFO   = 7,
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
    ACTION_TYPE_VOUCHER   = 10, // One-shot account-scoped character-service vouchers
    ACTION_TYPE_NONE      = 11, // No action type, used for invalid or uninitialized
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
    PERK_EFFECT_DRUID_FORM_MOONKIN  = 7,
    PERK_EFFECT_SHAMAN_GHOST_WOLF   = 8,
    PERK_EFFECT_WARLOCK_PET_IMP        = 9,
    PERK_EFFECT_WARLOCK_PET_VOIDWALKER = 10,
    PERK_EFFECT_WARLOCK_PET_FELHUNTER  = 11,
    PERK_EFFECT_WARLOCK_PET_SUCCUBUS   = 12,
    PERK_EFFECT_WARLOCK_PET_FELGUARD   = 13,
    PERK_EFFECT_WARLOCK_PET_DOOMGUARD  = 14,
    PERK_EFFECT_SHAMAN_FERAL_SPIRIT    = 15,
    PERK_EFFECT_PRIEST_SPIRIT_OF_REDEMPTION = 16,
    PERK_EFFECT_PRIEST_SHADOWFORM           = 17,
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
    float Scale;   // 0 = use model default; >0 overrides object scale for Value
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
    bool MailUnlocksEnabled{ false };
    // Cached on startup from mod-weekend-xp's XPWeekend.IsJoyousJourneysActive
    // config so the Joyous Journeys toggle button can hide itself when the
    // event is off, without re-reading sConfigMgr at runtime.
    bool JoyousJourneysActive{ false };
    bool ResScrollEnabled{ false };
    // When true, the challenge-xp PlayerScript snaps the player's weekend-xp
    // rate back to 1x on login and level-up. Defaults to enabled.
    bool ChallengeXpResetEnabled{ true };
    // Master switch for the per-observer display opt-out toggles.
    bool DisplayOptOutEnabled{ true };
    // Cached from mod-transmog's Transmogrification.Enable; gates the
    // transmog on/off entry in the Display Options menu.
    bool TransmogToggleEnabled{ false };
    Seconds BarberDuration = 1min;
public:
    static Smartstone* instance();

    bool IsPetAvailable(Player* player, SmartstonePetData service, uint8 subscriptionLevel) const;

    bool CanUseSmartstone(Player* player) const { return !player->GetMap()->IsDungeon() && !player->GetMap()->IsBattlegroundOrArena(); }

    // Part of the PvP restriction: blocked while an active Battlefield
    // (Wintergrasp) battle is in progress, unless PvP use is allowed.
    [[nodiscard]] bool IsBattlefieldUseBlocked(Player* player) const;

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

    // Per-observer opt-outs, enforced in the values-update patch hook.
    void SetHideCostumes(Player* player, bool hide) { player->UpdatePlayerSetting(ModName + "#misc", SETTING_HIDE_COSTUMES, hide ? 1 : 0); }
    [[nodiscard]] bool IsHidingCostumes(Player* player) const { return player->GetPlayerSetting(ModName + "#misc", SETTING_HIDE_COSTUMES).value != 0; }

    void SetHideForms(Player* player, bool hide) { player->UpdatePlayerSetting(ModName + "#misc", SETTING_HIDE_FORMS, hide ? 1 : 0); }
    [[nodiscard]] bool IsHidingForms(Player* player) const { return player->GetPlayerSetting(ModName + "#misc", SETTING_HIDE_FORMS).value != 0; }

    void SetHideMinions(Player* player, bool hide) { player->UpdatePlayerSetting(ModName + "#misc", SETTING_HIDE_MINIONS, hide ? 1 : 0); }
    [[nodiscard]] bool IsHidingMinions(Player* player) const { return player->GetPlayerSetting(ModName + "#misc", SETTING_HIDE_MINIONS).value != 0; }

    // Return true if the unit currently shows the override, and fill the
    // value an opted-out observer should see instead (canonical form model /
    // native minion display id).
    [[nodiscard]] bool GetActiveFormPerkSubstitute(Player* player, uint32& canonicalModel) const;
    [[nodiscard]] bool GetActiveMinionPerkSubstitute(Creature* creature, uint32& nativeDisplay) const;

    // Re-send nearby overridden units to `observer` so a toggle applies at once.
    void RefreshSmartstoneVisibilityFor(Player* observer) const;

    void SetDebugEnabled(bool enabled) { IsDebugEnabled = enabled; }
    [[nodiscard]] bool IsSmartstoneDebugEnabled() const { return IsDebugEnabled; }

    void SetIndividualCostumeCooldowns(bool enabled) { IndividualCostumeCooldowns = enabled; }
    [[nodiscard]] bool HasIndividualCostumeCooldowns() const { return IndividualCostumeCooldowns; }

    void SetCostumeConvertEnabled(bool enabled) { CostumeConvertEnabled = enabled; }
    [[nodiscard]] bool IsCostumeConvertEnabled() const { return CostumeConvertEnabled; }

    void SetMailUnlocksEnabled(bool enabled) { MailUnlocksEnabled = enabled; }
    [[nodiscard]] bool IsMailUnlocksEnabled() const { return MailUnlocksEnabled; }

    void SetJoyousJourneysActive(bool active) { JoyousJourneysActive = active; }
    [[nodiscard]] bool IsJoyousJourneysActive() const { return JoyousJourneysActive; }

    void SetResScrollEnabled(bool enabled) { ResScrollEnabled = enabled; }
    [[nodiscard]] bool IsResScrollEnabled() const { return ResScrollEnabled; }

    void SetChallengeXpResetEnabled(bool enabled) { ChallengeXpResetEnabled = enabled; }
    [[nodiscard]] bool IsChallengeXpResetEnabled() const { return ChallengeXpResetEnabled; }

    void SetDisplayOptOutEnabled(bool enabled) { DisplayOptOutEnabled = enabled; }
    [[nodiscard]] bool IsDisplayOptOutEnabled() const { return DisplayOptOutEnabled; }

    void SetTransmogToggleEnabled(bool enabled) { TransmogToggleEnabled = enabled; }
    [[nodiscard]] bool IsTransmogToggleEnabled() const { return TransmogToggleEnabled; }

    void SetCostumeCooldown(Player* player, uint32 costumeId);
    [[nodiscard]] bool HasCostumeCooldown(Player* player, uint32 costumeId) const;
    [[nodiscard]] uint32 GetCostumeCooldownRemaining(Player* player, uint32 costumeId) const;

    [[nodiscard]] bool IsInCostumeGracePeriod(Player* player, uint32 costumeId) const;

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
            case ACTION_TYPE_VOUCHER: return "Voucher";
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
            case ACTION_TYPE_VOUCHER: return "One-shot character-service voucher.";
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

    // Challenge-mode marker auras from mod-chromiecraft/challenge_modes.sql.
    // Mirrors the check in mod-resurrection-scroll's ProcessBonusChecks so
    // challenge characters are excluded from the same player-facing XP and
    // bonus toggles.
    //
    // Returns false at level 70+ — once a character reaches the endgame the
    // XP-rate restrictions no longer make sense, so they get the full menu
    // (and the rate-reset PlayerScript stops firing) regardless of marker auras.
    [[nodiscard]] static bool IsChallengeCharacter(Player const* player)
    {
        if (!player || player->GetLevel() >= 70)
            return false;
        return player->HasAnyAuras(2000100, 2000101, 2000102);
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

    // Voucher type -> the at-login flag its consumption sets on the character.
    // Returns 0 for unknown types. AT_LOGIN_* live in Player.h (included above).
    [[nodiscard]] static uint16 GetVoucherAtLoginFlag(uint8 type)
    {
        switch (type)
        {
            case VOUCHER_RENAME:         return AT_LOGIN_RENAME;
            case VOUCHER_FACTION_CHANGE: return AT_LOGIN_CHANGE_FACTION;
            case VOUCHER_RACE_CHANGE:    return AT_LOGIN_CHANGE_RACE;
            case VOUCHER_CUSTOMIZE:      return AT_LOGIN_CUSTOMIZE;
            default:                     return 0;
        }
    }

    // Module-string id for a voucher type's display name (see LANG_MOD_VOUCHER_NAME_*).
    [[nodiscard]] static uint32 GetVoucherNameStringId(uint8 type);

    // Voucher persistence lives in the auth DB (account-scoped, cross-realm),
    // mirroring smartstone_account_settings. Rows are queried live on gossip
    // open / claim, never cached in a hot path.
    [[nodiscard]] std::vector<SmartstoneVoucher> GetAccountVouchers(uint32 accountId) const;
    [[nodiscard]] bool HasAccountVouchers(uint32 accountId) const;
    // Inserts a voucher and returns its new row id (for addressing it in the
    // claim reminder). 0 if the id could not be read back.
    uint32 GrantVoucher(uint32 accountId, uint8 type, uint32 grantedByAccount);
    // Atomically consume voucher `voucherId` for `accountId` and apply its
    // service to `consumer`. Returns false if the row is missing, belongs to a
    // different account, or was already consumed (guards double-click).
    bool ConsumeVoucher(uint32 voucherId, uint32 accountId, Player* consumer);

    // Druid form display overrides (per-character).
    void SetDruidFormDisplay(Player* player, uint8 slot, uint32 displayId)
    {
        player->UpdatePlayerSetting(ModName + "#druid_form", slot, displayId);
    }
    [[nodiscard]] uint32 GetDruidFormDisplay(Player* player, uint8 slot) const
    {
        return player->GetPlayerSetting(ModName + "#druid_form", slot).value;
    }

    // Shaman form display overrides (per-character). Currently only
    // SHAMAN_FORM_GHOST_WOLF; add slots here if more shaman forms are
    // ever exposed.
    void SetShamanFormDisplay(Player* player, uint8 slot, uint32 displayId)
    {
        player->UpdatePlayerSetting(ModName + "#shaman_form", slot, displayId);
    }
    [[nodiscard]] uint32 GetShamanFormDisplay(Player* player, uint8 slot) const
    {
        return player->GetPlayerSetting(ModName + "#shaman_form", slot).value;
    }

    void SetPriestFormDisplay(Player* player, uint8 slot, uint32 displayId)
    {
        player->UpdatePlayerSetting(ModName + "#priest_form", slot, displayId);
    }
    [[nodiscard]] uint32 GetPriestFormDisplay(Player* player, uint8 slot) const
    {
        return player->GetPlayerSetting(ModName + "#priest_form", slot).value;
    }

    // Shaman guardian display overrides (per-character). Used for
    // TempSummon creatures (Feral Spirit wolves, etc.) rather than
    // shapeshift forms.
    void SetShamanGuardianDisplay(Player* player, uint8 slot, uint32 displayId)
    {
        player->UpdatePlayerSetting(ModName + "#shaman_guardian", slot, displayId);
    }
    [[nodiscard]] uint32 GetShamanGuardianDisplay(Player* player, uint8 slot) const
    {
        return player->GetPlayerSetting(ModName + "#shaman_guardian", slot).value;
    }

    // Warlock pet display overrides (per-character). Slot = WarlockPetSlot,
    // value = displayId to apply when that pet is summoned (0 = default).
    void SetWarlockPetDisplay(Player* player, uint8 slot, uint32 displayId)
    {
        player->UpdatePlayerSetting(ModName + "#warlock_pet", slot, displayId);
    }
    [[nodiscard]] uint32 GetWarlockPetDisplay(Player* player, uint8 slot) const
    {
        return player->GetPlayerSetting(ModName + "#warlock_pet", slot).value;
    }

    // Composite key for PerkScaleOverrides. Scale depends on the render
    // context, not just the model: the same creature displayId worn as a
    // player shapeshift vs. summoned as a guardian needs different scales.
    // SmartstonePerkEffect is that context, so (effect, displayId) keys it.
    [[nodiscard]] static constexpr uint64 PerkScaleKey(uint8 effect, uint32 displayId)
    {
        return (static_cast<uint64>(effect) << 32) | displayId;
    }

    // Per-(effect, model) object-scale override, populated from
    // smartstone_perks.Scale in LoadPerks(). Scale lives with the perk
    // definition rather than per-player; the apply sites know their own
    // effect and the stored displayId, so they resolve scale through here.
    // Returns 0 when no override is configured.
    [[nodiscard]] float GetPerkScale(uint8 effect, uint32 displayId) const
    {
        auto it = PerkScaleOverrides.find(PerkScaleKey(effect, displayId));
        return it != PerkScaleOverrides.end() ? it->second : 0.0f;
    }

    // Effective scale for SetDisplayId: the configured override, or 1.0
    // when none is set (matching the engine's default model scale).
    [[nodiscard]] float GetEffectivePerkScale(uint8 effect, uint32 displayId) const
    {
        float scale = GetPerkScale(effect, displayId);
        return scale > 0.0f ? scale : 1.0f;
    }

    // Map a warlock pet creature entry to its WarlockPetSlot index, or -1
    // if the entry isn't a recognised warlock pet.
    [[nodiscard]] static int32 GetWarlockPetSlotForEntry(uint32 entry)
    {
        switch (entry)
        {
            case   416: return WARLOCK_PET_IMP;
            case  1860: return WARLOCK_PET_VOIDWALKER;
            case   417: return WARLOCK_PET_FELHUNTER;
            case  1863: return WARLOCK_PET_SUCCUBUS;
            case 17252: return WARLOCK_PET_FELGUARD;
            case 11859: return WARLOCK_PET_DOOMGUARD;
            default:    return -1;
        }
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
            case FORM_MOONKIN:     currentSlot = DRUID_FORM_MOONKIN; spellId = 24858; break;
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
        uint8 effect;
        uint32 spellId;
        switch (form)
        {
            case FORM_BEAR:        slot = DRUID_FORM_BEAR;    effect = PERK_EFFECT_DRUID_FORM_BEAR;    spellId =  5487; break;
            case FORM_DIREBEAR:    slot = DRUID_FORM_BEAR;    effect = PERK_EFFECT_DRUID_FORM_BEAR;    spellId =  9634; break;
            case FORM_CAT:         slot = DRUID_FORM_CAT;     effect = PERK_EFFECT_DRUID_FORM_CAT;     spellId =   768; break;
            case FORM_TRAVEL:      slot = DRUID_FORM_TRAVEL;  effect = PERK_EFFECT_DRUID_FORM_TRAVEL;  spellId =   783; break;
            case FORM_FLIGHT:      slot = DRUID_FORM_FLIGHT;  effect = PERK_EFFECT_DRUID_FORM_FLIGHT;  spellId = 33943; break;
            case FORM_FLIGHT_EPIC: slot = DRUID_FORM_FLIGHT;  effect = PERK_EFFECT_DRUID_FORM_FLIGHT;  spellId = 40120; break;
            case FORM_AQUA:        slot = DRUID_FORM_AQUATIC; effect = PERK_EFFECT_DRUID_FORM_AQUATIC; spellId =  1066; break;
            case FORM_TREE:        slot = DRUID_FORM_TREE;    effect = PERK_EFFECT_DRUID_FORM_TREE;    spellId = 33891; break;
            case FORM_MOONKIN:     slot = DRUID_FORM_MOONKIN; effect = PERK_EFFECT_DRUID_FORM_MOONKIN; spellId = 24858; break;
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
            player->SetDisplayId(displayId, GetEffectivePerkScale(effect, displayId));
    }

    // Shaman counterpart of RestoreDefaultFormDisplay. Currently only one
    // form (Ghost Wolf), but kept slot-parameterised for symmetry.
    void RestoreDefaultShamanFormDisplay(Player* player, uint8 slot)
    {
        if (!player)
            return;

        ShapeshiftForm form = player->GetShapeshiftForm();
        if (form != FORM_GHOSTWOLF || slot != SHAMAN_FORM_GHOST_WOLF)
            return;

        if (uint32 model = player->GetModelForForm(form, 2645)) // Ghost Wolf
            player->SetDisplayId(model, 1.0f);
    }

    // Shaman counterpart of ReapplyActiveDruidFormDisplay. In BG / arena
    // forces the canonical model; otherwise re-stamps the saved override.
    void ReapplyActiveShamanFormDisplay(Player* player)
    {
        if (!player)
            return;

        if (player->GetShapeshiftForm() != FORM_GHOSTWOLF)
            return;

        if (player->InBattleground() || player->InArena())
        {
            if (uint32 model = player->GetModelForForm(FORM_GHOSTWOLF, 2645))
                player->SetDisplayId(model, 1.0f);
            return;
        }

        if (uint32 displayId = GetShamanFormDisplay(player, SHAMAN_FORM_GHOST_WOLF))
            player->SetDisplayId(displayId, GetEffectivePerkScale(PERK_EFFECT_SHAMAN_GHOST_WOLF, displayId));
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
    std::unordered_map<uint64, float> PerkScaleOverrides; // PerkScaleKey(effect, displayId) -> scale
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
    // Service unlock notifications (70-72)
    LANG_MOD_SERVICE_UNLOCK_NOTIFY     = 70,
    LANG_MOD_SERVICE_UNLOCK_MAIL_SUBJ  = 71,
    LANG_MOD_SERVICE_UNLOCK_MAIL_BODY  = 72,
    // Debug command messages (73-75)
    LANG_MOD_DEBUG_NO_TARGET           = 73,
    LANG_MOD_DEBUG_TARGET_OFFLINE      = 74,
    LANG_MOD_DEBUG_COSTUME_DUMP        = 75,
    // Challenge-character XP-rate countermeasure (76)
    LANG_MOD_XP_RATE_RESET_NOTICE      = 76,
    // Display opt-out toggles (77-82)
    LANG_MOD_COSTUMES_NOW_HIDDEN       = 77,
    LANG_MOD_COSTUMES_NOW_SHOWN        = 78,
    LANG_MOD_FORMS_NOW_HIDDEN          = 79,
    LANG_MOD_FORMS_NOW_SHOWN           = 80,
    LANG_MOD_MINIONS_NOW_HIDDEN        = 81,
    LANG_MOD_MINIONS_NOW_SHOWN         = 82,
    // Voucher names (83-86) — keep aligned with SmartstoneVoucherType so
    // GetVoucherNameStringId can index off the type.
    LANG_MOD_VOUCHER_NAME_RENAME       = 83,
    LANG_MOD_VOUCHER_NAME_FACTION      = 84,
    LANG_MOD_VOUCHER_NAME_RACE         = 85,
    LANG_MOD_VOUCHER_NAME_CUSTOMIZE    = 86,
    // Voucher flow messages (87-97)
    LANG_MOD_VOUCHER_GRANTED           = 87, // GM ack: granted '{name}' to {account}
    LANG_MOD_VOUCHER_APPLIED           = 88, // '{name}' applied, takes effect at char select
    LANG_MOD_VOUCHER_NONE_AVAILABLE    = 89,
    LANG_MOD_VOUCHER_INVALID           = 90, // gone / already consumed
    LANG_MOD_VOUCHER_INVALID_TYPE      = 91, // command: bad type argument
    LANG_MOD_VOUCHER_LIST_HEADER       = 92,
    LANG_MOD_VOUCHER_LIST_ENTRY        = 93, // [{id}] {name}
    LANG_MOD_VOUCHER_LIST_NONE         = 94,
    LANG_MOD_VOUCHER_REVOKED           = 95,
    LANG_MOD_VOUCHER_REVOKE_NOT_FOUND  = 96,
    LANG_MOD_VOUCHER_LOGIN_NOTICE      = 97, // per-voucher blue login reminder
    LANG_MOD_VOUCHER_LIST_ENTRY_CLAIMED = 98, // [{id}] {name} (claimed by {char} on {date})
};

#define sSmartstone Smartstone::instance()

#endif
