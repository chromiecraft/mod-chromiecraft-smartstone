#ifndef DEF_SMARTSTONE_H
#define DEF_SMARTSONE_H

#include "Player.h"
#include "Config.h"
#include "InstanceScript.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"

class Smartstone
{
public:
    static Smartstone* instance();

    bool IsEnabled{ true };
    Seconds BarberDuration = 1min;
};

#define sSmartstone Smartstone::instance()

#endif
