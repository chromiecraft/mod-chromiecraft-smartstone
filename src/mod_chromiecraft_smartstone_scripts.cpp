/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Chat.h"
#include "Smartstone.h"

enum GameObjectEntry
{
    GO_BARBERSHOP_CHAIR = 191029,
};

enum CreatureEntries
{
    NPC_BARBER = 80000
};

enum Spells
{
    SPELL_FLYING_MACHINE = 44153
};

enum StoneActions
{
    SMARTSTONE_ACTION_BARBERSHOP = 1,
    SMARTSTONE_TEST              = 2
};

enum Texts
{
    SAY_BARBER_SPAWN           = 0,
    SAY_BARBER_DESPAWN         = 1
};

enum Settings
{
    SETTING_MEMBERSHIP_LEVEL
};

struct SmartstoneServices
{
    uint8 Id;
    std::string ServiceTitle;
    uint8 SubscriptionLevelRequired;
};

const SmartstoneServices StoneServices[] =
{
    { SMARTSTONE_ACTION_BARBERSHOP, "Request Barber Services", 0 }
};

Smartstone* Smartstone::instance()
{
    static Smartstone instance;
    return &instance;
}

const std::string SubsModName = "acore_cms_subscriptions";

class item_chromiecraft_smartstone : public ItemScript
{
public:

    item_chromiecraft_smartstone() : ItemScript("item_chromiecraft_smartstone") { }

    void OnGossipSelect(Player* player, Item* /*item*/, uint32  /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        player->PlayerTalkClass->SendCloseGossip();

        switch (action)
        {
            case SMARTSTONE_ACTION_BARBERSHOP:
                if (player->GetMap()->IsBattleground() || player->GetMap()->IsDungeon()
                    || sSmartstone->BarberDuration == Seconds::zero())
                    return;

                if (Creature* barber = player->SummonCreature(NPC_BARBER, player->GetNearPosition(2.0f, 0.0f), TEMPSUMMON_MANUAL_DESPAWN))
                {
                    barber->AI()->Talk(SAY_BARBER_SPAWN);
                    barber->SummonGameObject(GO_BARBERSHOP_CHAIR, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2000);
                    barber->SetFacingToObject(player);

                    barber->m_Events.AddEventAtOffset([barber] {
                        barber->AddAura(SPELL_FLYING_MACHINE, barber);
                        barber->AI()->Talk(SAY_BARBER_DESPAWN);
                        Position pos = barber->GetNearPosition(20.0f, 0.0f);
                        barber->GetMotionMaster()->MovePoint(0, pos);
                        barber->DespawnOrUnsummon(4000);
                    }, sSmartstone->BarberDuration);
                }
                break;
            default:
                break;
        }
    }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/) override
    {
        if (!sSmartstone->IsEnabled)
            return false;

        player->PlayerTalkClass->ClearMenus();

        for (auto const& service : StoneServices)
        {
            if (player->GetPlayerSetting(SubsModName, SETTING_MEMBERSHIP_LEVEL).value >= service.SubscriptionLevelRequired)
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem(service.Id, 0, service.ServiceTitle, 0, 1, "", 0);
        }

        player->PlayerTalkClass->SendGossipMenu(92000, item->GetGUID());
        return false;
    }
};

class mod_chromiecraft_smartstone_worldscript : public WorldScript
{
public:
    mod_chromiecraft_smartstone_worldscript() : WorldScript("mod_chromiecraft_smartstone_worldscript") { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        sSmartstone->IsEnabled = sConfigMgr->GetOption<bool>("ModChromiecraftSmartstone.Enable", false);
        sSmartstone->BarberDuration = Seconds(sConfigMgr->GetOption<int32>("ModChromiecraftSmartstone.Features.BarberDuration", 300));
    }
};

void Addmod_cc_smartstoneScripts()
{
    new item_chromiecraft_smartstone();
    new mod_chromiecraft_smartstone_worldscript();
}
