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

    item_chromiecraft_smartstone() : ItemScript("item_chromiecraft_smartstone"), _lastAction(0), _currentPage(0), _lastCategory(0), _menuHistory() { }

    void OnGossipSelect(Player* player, Item* item, uint32  /*sender*/, uint32 action) override
    {
        sSmartstone->ProcessExpiredServices(player);

        uint8 subscriptionLevel = GetPlayerSubscriptionLevel(player);

        auto pets = sSmartstone->Pets;
        auto costumes = sSmartstone->Costumes;

        const auto actionKey = sSmartstone->DecodeActionId(action);
        const auto actionId = actionKey->actionId;
        const auto actionType = actionKey->type;

        /**
         * @todo: refactor in multiple functions
         *
         */
        switch (actionType) {
            case ACTION_TYPE_CATEGORY:
            {
                // Show items in the selected category
                ShowCategoryItems(actionId, player, item, subscriptionLevel);
                return;
            }
            case ACTION_TYPE_UTIL: {
                switch(actionId) {
                    case SMARTSTONE_ACTION_BACK:
                    {
                        if (!_menuHistory.empty())
                        {
                            MenuState previousState = _menuHistory.back();
                            _menuHistory.pop_back();

                            // Restore previous state
                            _lastAction = previousState.lastAction;
                            _lastCategory = previousState.lastCategory;
                            _currentPage = previousState.currentPage;
                            // Go back to main menu
                            ClearLastAction();
                            ClearLastCategory();
                            ShowCategoryItems(_lastCategory, player, item, subscriptionLevel);
                        }
                        else
                        {
                            // No history, go to main menu
                            ClearLastAction();
                            ClearLastCategory();
                            ShowMainMenu(player, item, subscriptionLevel);
                        }
                        return;
                    }
                    case SMARTSTONE_ACTION_NEXT_PAGE:
                    case SMARTSTONE_ACTION_LAST_PAGE:
                    {
                        actionId == SMARTSTONE_ACTION_LAST_PAGE ? PreviousPage() : NextPage();
                        ShowCategoryItems(_lastCategory, player, item, subscriptionLevel, _currentPage);
                        return;
                    }
                    case SMARTSTONE_ACTION_UNSUMMON_COMPANION:
                    {
                        if (Creature* cr = player->GetCompanionPet())
                        {
                            cr->DespawnOrUnsummon();
                            ClearLastAction();
                        }
                        return;
                    }
                    case SMARTSTONE_ACTION_UNSUMMON_PET:
                    {
                        if (Guardian* cr = player->GetGuardianPet())
                        {
                            cr->DespawnOrUnsummon();
                            ClearLastAction();
                        }
                        return;
                    }
                    case SMARTSTONE_ACTION_REMOVE_COSTUME:
                    {
                        if (sSmartstone->GetCurrentCostume(player))
                        {
                            player->DeMorph();
                            sSmartstone->SetCurrentCostume(player, 0);
                        }
                        ClearLastAction();
                        return;
                    }
                }
            }
            case ACTION_TYPE_SERVICE:
            {
                switch (actionId)
                {
                    case SERVICE_BARBERSHOP:
                    {
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
                    }
                }
            }
            case ACTION_TYPE_PET:
            {
                if (!sSmartstone->CanUseSmartstone(player))
                {
                    player->SendSystemMessage("You cannot use this feature inside instances or battlegrounds.");
                    return;
                }

                player->CastCustomSpell(90001, SPELLVALUE_MISCVALUE0, action);
                break;
            }
            case ACTION_TYPE_COMPANION:
            {
                player->CastCustomSpell(90000, SPELLVALUE_MISCVALUE0, action);
                break;
            }
            case ACTION_TYPE_COSTUME:
            {
                if (player->HasSpellCooldown(90002) && !player->GetCommandStatus(CHEAT_COOLDOWN))
                {
                    uint32 remaining = player->GetSpellCooldownDelay(90002); // in milliseconds
                    uint32 seconds = remaining / 1000;
                    uint32 minutes = seconds / 60;
                    seconds = seconds % 60;
                    std::string message = Acore::StringFormat("You cannot use this feature for another {} minute(s) and {} second(s).", minutes, seconds);
                    player->SendSystemMessage(message);
                    ClearLastAction();
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
                break;
            }
            case ACTION_TYPE_NONE:
            case MAX_ACTION_TYPE:
            default:
            {
                // Invalid action type, show an error message
                player->SendSystemMessage("Invalid action type selected.");
                return;
            }
        }


        ClearLastAction();
    }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/) override
    {
        if (!sSmartstone->IsSmartstoneEnabled())
            return false;

        player->PlayerTalkClass->ClearMenus();

        ResetPagination();
        ClearLastAction();
        ClearLastCategory();
        ClearMenuHistory();

        uint8 subscriptionLevel = GetPlayerSubscriptionLevel(player);

        ShowMainMenu(player, item, subscriptionLevel);
        return false;
    }

    private:
        struct MenuState
        {
            uint16 lastAction;
            uint16 currentPage;
            uint32 lastCategory;

            MenuState(uint16 action, uint16 page, uint32 category)
                : lastAction(action), currentPage(page), lastCategory(category) {}
        };

        uint16 _lastAction;
        uint16 _currentPage;
        uint32 _lastCategory;
        std::vector<MenuState> _menuHistory;

        void SetLastAction(uint16 action) { _lastAction = action; }
        void ClearLastAction() { _lastAction = SMARTSTONE_ACTION_NONE; }
        void SetLastCategory(uint32 category) { _lastCategory = category; }
        void ClearLastCategory() { _lastCategory = 0; }
        void ResetPagination() { _currentPage = 0; }
        void NextPage() { ++_currentPage; }
        void PreviousPage() { if (_currentPage > 0) --_currentPage; }
        void PushMenuState() { _menuHistory.emplace_back(_lastAction, _currentPage, _lastCategory); }
        void ClearMenuHistory() { _menuHistory.clear(); }

        uint8 GetPlayerSubscriptionLevel(Player* player) const
        {
            return player->IsGameMaster() ? 3 : player->GetPlayerSetting(SubsModName, SETTING_MEMBERSHIP_LEVEL).value;
        }

        void ShowMainMenu(Player* player, Item* item, uint8 subscriptionLevel)
        {
            ShowCategoryItems(CATEGORY_MAIN, player, item, subscriptionLevel);
        }

        void ShowCategoryItems(uint32 ParentCategoryId, Player* player, Item* item, uint8 subscriptionLevel, uint8 currentPage = 0)
        {
            player->PlayerTalkClass->ClearMenus();

            PushMenuState();

            auto const& menuItems = sSmartstone->MenuItems[ParentCategoryId];

            auto& expireInfo = sSmartstone->ServiceExpireInfo[player->GetGUID().GetCounter()];

            std::map<uint32, tm> expireInfoMap;

            for (auto const& info : expireInfo)
                expireInfoMap[info.ServiceId] = Acore::Time::TimeBreakdown(info.ExpirationTime);

            int32 menuItemIndex = 0;

            uint32 itemsPerPage = 20; // we need to include the utils buttons too
            uint32 pageNumber = currentPage;

            /**
             * @brief Utility buttons
             *
             */
            if (player->GetCompanionPet())
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem(menuItemIndex++ , 0, "|TInterface/icons/Spell_Nature_SpiritWolf:30:30:-18:0|t Unsummon current companion", 0, sSmartstone->GetActionTypeId(ACTION_TYPE_UTIL, SMARTSTONE_ACTION_UNSUMMON_COMPANION), "", 0);

            if (player->GetGuardianPet())
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem(menuItemIndex++, 0, "|TInterface/icons/Spell_Nature_SpiritWolf:30:30:-18:0|t Unsummon current pet", 0, sSmartstone->GetActionTypeId(ACTION_TYPE_UTIL, SMARTSTONE_ACTION_UNSUMMON_PET), "", 0);

            if (sSmartstone->GetCurrentCostume(player))
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem(menuItemIndex++, 0, "|TInterface/PaperDollInfoFrame/UI-GearManager-Undo:30:30:-18:0|t Remove current costume", 0, sSmartstone->GetActionTypeId(ACTION_TYPE_UTIL, SMARTSTONE_ACTION_REMOVE_COSTUME), "", 0);

            uint32 totalItems = menuItems.size();
            uint32 startIndex = pageNumber * itemsPerPage;
            uint32 endIndex = std::min(startIndex + itemsPerPage, totalItems);

            /**
             * @brief Process each menu item
             */
            for (auto const& menuItem : menuItems)
            {

                if (menuItem.ServiceType == ACTION_TYPE_PET) {
                    player->PlayerTalkClass->ClearMenus();

                    auto pet = sSmartstone->CombatPets[menuItem.ItemId];

                    std::string expireMsg = "";

                    if (expireInfoMap[pet.CreatureId].tm_year && (expireInfoMap[pet.CreatureId].tm_year != 1900))
                        expireMsg = Acore::StringFormat("\n(Expires: {:%Y-%m-%d %H:%M})", expireInfoMap[pet.CreatureId]);

                    if (sSmartstone->IsPetAvailable(player, pet, subscriptionLevel))
                        player->PlayerTalkClass->GetGossipMenu().AddMenuItem(menuItemIndex++, 0, pet.Description + expireMsg, 0, sSmartstone->GetActionTypeId(ACTION_TYPE_PET, pet.CreatureId), "", 0);
                    else if (totalItems > 0)
                        totalItems--;
                }

                if (menuItem.ServiceType == ACTION_TYPE_COMPANION)
                {
                    player->PlayerTalkClass->ClearMenus();

                    auto pet = sSmartstone->Pets[menuItem.ItemId];

                    if (sSmartstone->IsPetAvailable(player, pet, subscriptionLevel))
                        player->PlayerTalkClass->GetGossipMenu().AddMenuItem(menuItemIndex++, 0, pet.Description, 0, sSmartstone->GetActionTypeId(ACTION_TYPE_COMPANION, pet.CreatureId), "", 0);
                    else if (totalItems > 0)
                        totalItems--;
                }

                if (menuItem.ServiceType == ACTION_TYPE_COSTUME)
                {
                    if (sSmartstone->IsServiceAvailable(player, "#costume", menuItem.ItemId)
                        || subscriptionLevel >= menuItem.SubscriptionLevelRequired)
                    {
                        SmartstoneCostumeData costume = sSmartstone->GetCostumeData(menuItem.ItemId);
                        player->PlayerTalkClass->GetGossipMenu().AddMenuItem(menuItemIndex++, GOSSIP_ICON_TABARD, costume.Description, 0, sSmartstone->GetActionTypeId(ACTION_TYPE_COSTUME, costume.Id), "", 0);
                    }
                    else if (totalItems > 0)
                        totalItems--;
                }
            }

            /**
             * @brief Navigation and utility buttons
             */
            if (totalItems == 0)
            {
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem(0, GOSSIP_ICON_TALK, "No actions available.", 0, 0, "", 0);
            }
            else
            {
                if (endIndex < totalItems)
                    player->PlayerTalkClass->GetGossipMenu().AddMenuItem(menuItemIndex++, GOSSIP_ICON_DOT, "|TInterface/icons/Spell_ChargePositive:30:30:-18:0|t Next page", 0, sSmartstone->GetActionTypeId(ACTION_TYPE_UTIL, SMARTSTONE_ACTION_NEXT_PAGE), "", 0);

                if (pageNumber > 0)
                    player->PlayerTalkClass->GetGossipMenu().AddMenuItem(menuItemIndex++, GOSSIP_ICON_DOT, "|TInterface/icons/Spell_ChargeNegative:30:30:-18:0|t Previous page", 0, sSmartstone->GetActionTypeId(ACTION_TYPE_UTIL, SMARTSTONE_ACTION_LAST_PAGE), "", 0);
            }

            // Add back button
            if (ParentCategoryId != CATEGORY_MAIN)
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem(menuItemIndex++, GOSSIP_ICON_DOT, "Back", 0, sSmartstone->GetActionTypeId(ACTION_TYPE_UTIL, SMARTSTONE_ACTION_BACK), "", 0);

            SetLastCategory(ParentCategoryId);

            player->PlayerTalkClass->SendGossipMenu(92000, item->GetGUID());
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
