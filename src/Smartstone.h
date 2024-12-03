#ifndef DEF_SMARTSTONE_H
#define DEF_SMARTSTONE_H

#include "Player.h"
#include "Config.h"
#include "InstanceScript.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"

struct SmartstonePetData
{
    uint32 CreatureId;
    std::string Description;
};

struct SmartstoneServices
{
    uint8 Id;
    std::string ServiceTitle;
    uint8 SubscriptionLevelRequired;
};

enum Misc
{
    ACTION_RANGE_SUMMON_PET = 80000
};

const std::string ModName = "mod-cc-smartstone";

class Smartstone
{
private:
    uint32 SmartstoneItemID = 0;
    bool IsEnabled{ true };
    Seconds BarberDuration = 1min;
public:
    static Smartstone* instance();

    void SetEnabled(bool enabled) { IsEnabled = enabled; }
    [[nodiscard]] bool IsSmartstoneEnabled() { return IsEnabled; }

    void SetBarberDuration(Seconds duration) { BarberDuration = duration; }
    [[nodiscard]] Seconds GetBarberDuration() { return BarberDuration; }

    void SetSmartstoneItemID(uint32 itemId) { SmartstoneItemID = itemId; }
    [[nodiscard]] uint32 GetSmartstoneItemID() { return SmartstoneItemID; }

    void LoadServices();
    void LoadPets();

    [[nodiscard]] SmartstonePetData GetPetData(uint32 creatureId) const;

    std::vector<SmartstonePetData> Pets;
    std::vector<SmartstoneServices> Services;
};

#define sSmartstone Smartstone::instance()

#endif
