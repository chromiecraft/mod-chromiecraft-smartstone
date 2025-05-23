/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "Chat.h"
#include "GameTime.h"
#include "ScriptMgr.h"
#include "Smartstone.h"
#include "Pet.h"
#include "Player.h"

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

enum Texts
{
    SAY_BARBER_SPAWN           = 0,
    SAY_BARBER_DESPAWN         = 1
};

class item_chromiecraft_smartstone : public ItemScript
{
public:

    item_chromiecraft_smartstone() : ItemScript("item_chromiecraft_smartstone") { }

    void OnGossipSelect(Player* player, Item* item, uint32  /*sender*/, uint32 action) override
    {
        sSmartstone->ProcessExpiredServices(player);

        uint8 subscriptionLevel = player->IsGameMaster() ? 3
            : player->GetPlayerSetting(SubsModName, SETTING_MEMBERSHIP_LEVEL).value;

        if (action > ACTION_RANGE_COSTUMES_CATEGORIES && action < ACTION_RANGE_COSTUMES)
        {
            player->PlayerTalkClass->ClearMenus();

            for (auto const& costume : sSmartstone->Costumes[action - ACTION_RANGE_COSTUMES_CATEGORIES])
            {
                if (sSmartstone->IsServiceAvailable(player, "#costume", costume.Id - ACTION_RANGE_COSTUMES)
                    || subscriptionLevel >= costume.SubscriptionLevelRequired)
                    player->PlayerTalkClass->GetGossipMenu().AddMenuItem(costume.DisplayId, 0, costume.Description, 0, costume.Id, "", 0);
            }

            player->PlayerTalkClass->SendGossipMenu(sSmartstone->GetNPCTextForCategory(CATEGORY_COSTUMES, action - ACTION_RANGE_COSTUMES_CATEGORIES), item->GetGUID());
            return;
        }

        if (action != SMARTSTONE_ACTION_EXOTIC_PET_COLLECTION)
        {
            player->PlayerTalkClass->ClearMenus();
            player->PlayerTalkClass->SendCloseGossip();
        }

        if (action > ACTION_RANGE_COSTUMES && action < ACTION_RANGE_SUMMON_PET)
        {
            if (player->HasSpellCooldown(90002))
            {
                uint32 remaining = player->GetSpellCooldownDelay(90002); // in milliseconds
                uint32 seconds = remaining / 1000;
                uint32 minutes = seconds / 60;
                seconds = seconds % 60;
                std::string message = Acore::StringFormat("You cannot use this feature for another {} minute(s) and {} second(s).", minutes, seconds);
                player->SendSystemMessage(message);
                return;
            }

            SmartstoneCostumeData costume = sSmartstone->GetCostumeData(action);
            player->SetDisplayId(costume.DisplayId);
            sSmartstone->SetCurrentCostume(player, costume.DisplayId);

            player->AddSpellCooldown(90002, 0, 30 * MINUTE * IN_MILLISECONDS);

            Milliseconds duration = sSmartstone->GetCostumeDuration(player, costume.Duration);
            if (duration > 0s)
            {
                player->m_Events.AddEventAtOffset([player] {
                    if (player->GetDisplayId() == sSmartstone->GetCurrentCostume(player))
                        player->SetDisplayId(player->GetNativeDisplayId());
                }, duration);
            }
            return;
        }

        if (action > ACTION_RANGE_SUMMON_PET && action < ACTION_RANGE_SUMMON_COMBAT_PET)
        {
            player->CastCustomSpell(90000, SPELLVALUE_MISCVALUE0, action);
            return;
        }

        if (action > ACTION_RANGE_SUMMON_COMBAT_PET)
        {
            if (!sSmartstone->CanUseSmartstone(player))
            {
                player->SendSystemMessage("You cannot use this feature inside instances or battlegrounds.");
                return;
            }

            player->CastCustomSpell(90001, SPELLVALUE_MISCVALUE0, action);
            return;
        }

        ProcessGossipAction(player, action, item, subscriptionLevel);
    }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/) override
    {
        if (!sSmartstone->IsSmartstoneEnabled())
            return false;

        player->PlayerTalkClass->ClearMenus();

        uint8 subscriptionLevel = player->IsGameMaster() ? 3
            : player->GetPlayerSetting(SubsModName, SETTING_MEMBERSHIP_LEVEL).value;

        auto const& services = sSmartstone->Services;

        for (auto const& service : services)
        {
            if (subscriptionLevel >= service.SubscriptionLevelRequired)
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem(service.Id, 0, service.ServiceTitle, 0, service.Id, "", 0);
        }

        player->PlayerTalkClass->SendGossipMenu(92000, item->GetGUID());
        return false;
    }

    void ProcessGossipAction(Player* player, uint32 action, Item* item, uint8 subscriptionLevel)
    {
        auto pets = sSmartstone->Pets;
        auto costumes = sSmartstone->Costumes;

        switch (action)
        {
            case SMARTSTONE_ACTION_BARBERSHOP:
                if (sSmartstone->GetBarberDuration() == Seconds::zero())
                    return;

                if (!sSmartstone->CanUseSmartstone(player))
                {
                    player->SendSystemMessage("You cannot use this feature inside instances or battlegrounds.");
                    return;
                }

                if (player->FindNearestCreature(NPC_BARBER, 100.0f))
                {
                    player->SendSystemMessage("The barber is already summoned.");
                    return;
                }

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
                    }, sSmartstone->GetBarberDuration());
                }
                break;
            case SMARTSTONE_ACTION_EXOTIC_PET_COLLECTION:
                player->PlayerTalkClass->ClearMenus();

                if (player->GetCompanionPet())
                    player->PlayerTalkClass->GetGossipMenu().AddMenuItem(ACTION_RANGE_SUMMON_PET, 0, "Unsummon current pet", 0, ACTION_RANGE_SUMMON_PET, "", 0);

                for (auto const& pet : pets)
                {
                    if (sSmartstone->IsPetAvailable(player, pet, subscriptionLevel))
                        player->PlayerTalkClass->GetGossipMenu().AddMenuItem(pet.CreatureId, 0, pet.Description, 0, pet.CreatureId, "", 0);
                }

                player->PlayerTalkClass->SendGossipMenu(92002, item->GetGUID());
                break;
            case SMARTSTONE_ACTION_LIMITED_DURATION_PETS:
            {
                player->PlayerTalkClass->ClearMenus();

                pets = sSmartstone->CombatPets;

                auto& expireInfo = sSmartstone->ServiceExpireInfo[player->GetGUID().GetCounter()];

                std::map<uint32, tm> expireInfoMap;

                for (auto const& info : expireInfo)
                    expireInfoMap[info.ServiceId] = Acore::Time::TimeBreakdown(info.ExpirationTime);

                for (auto const& pet : pets)
                {
                    std::string expireMsg = "";

                    if (expireInfoMap[pet.CreatureId].tm_year != 1970)
                        expireMsg = Acore::StringFormat("\n(Expires: {:%Y-%m-%d %H:%M})", expireInfoMap[pet.CreatureId]);

                    if (sSmartstone->IsPetAvailable(player, pet, subscriptionLevel))
                        player->PlayerTalkClass->GetGossipMenu().AddMenuItem(pet.CreatureId, 0, pet.Description + expireMsg, 0, pet.CreatureId, "", 0);
                }

                player->PlayerTalkClass->SendGossipMenu(92003, item->GetGUID());
                break;
            }
            case SMARTSTONE_ACTION_COSTUMES:
                player->PlayerTalkClass->ClearMenus();

                for (auto const& category : sSmartstone->Categories[CATEGORY_COSTUMES])
                    player->PlayerTalkClass->GetGossipMenu().AddMenuItem(category.Id + 10000, 0, category.Title, 0, category.Id + 10000, "", 0);

                player->PlayerTalkClass->SendGossipMenu(92005, item->GetGUID());
                break;
            case ACTION_RANGE_SUMMON_PET:
                if (Creature* cr = player->GetCompanionPet())
                    cr->DespawnOrUnsummon();
                break;
            case ACTION_RANGE_SUMMON_COMBAT_PET:
                if (Pet* cr = player->GetPet())
                    cr->DespawnOrUnsummon();
                break;
            default:
                break;
        }
    }
};

class mod_chromiecraft_smartstone_worldscript : public WorldScript
{
public:
    mod_chromiecraft_smartstone_worldscript() : WorldScript("mod_chromiecraft_smartstone_worldscript") { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        sSmartstone->SetEnabled(sConfigMgr->GetOption<bool>("ModChromiecraftSmartstone.Enable", false));
        sSmartstone->SetBarberDuration(Seconds(sConfigMgr->GetOption<int32>("ModChromiecraftSmartstone.Features.BarberDuration", 300)));
        sSmartstone->SetSmartstoneItemID(sConfigMgr->GetOption<uint32>("ModChromiecraftSmartstone.ItemID", 32547));

        if (sSmartstone->IsSmartstoneEnabled())
        {
            sSmartstone->LoadServices();
            sSmartstone->LoadPets();
            sSmartstone->LoadCostumes();
            sSmartstone->LoadServiceExpirationInfo();
            sSmartstone->LoadCategories();
        }
    }
};

void Addmod_cc_smartstoneScripts()
{
    new item_chromiecraft_smartstone();
    new mod_chromiecraft_smartstone_worldscript();
}
