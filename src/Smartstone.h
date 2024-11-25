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

class Smartstone
{
public:
    static Smartstone* instance();

    bool IsEnabled{ true };
    Seconds BarberDuration = 1min;
    std::vector<SmartstonePetData> Pets;
};

#define sSmartstone Smartstone::instance()

#endif
