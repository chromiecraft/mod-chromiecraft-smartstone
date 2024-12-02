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

class Smartstone
{
private:
    bool IsEnabled{ true };
    Seconds BarberDuration = 1min;
public:
    static Smartstone* instance();

    void SetEnabled(bool enabled) { IsEnabled = enabled; }
    [[nodiscard]] bool IsSmartstoneEnabled() { return IsEnabled; }

    void SetBarberDuration(Seconds duration) { BarberDuration = duration; }
    [[nodiscard]] Seconds GetBarberDuration() { return BarberDuration; }

    void LoadServices();
    void LoadPets();

    std::vector<SmartstonePetData> Pets;
    std::vector<SmartstoneServices> Services;
};

#define sSmartstone Smartstone::instance()

#endif
