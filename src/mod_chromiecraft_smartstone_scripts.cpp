/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "Chat.h"
#include "CombatAI.h"
#include "GameTime.h"
#include "ScriptMgr.h"
#include "Smartstone.h"
#include "Pet.h"
#include "Player.h"
#include "Vehicle.h"

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

        uint8 subscriptionLevel = GetPlayerSubscriptionLevel(player);

        auto pets = sSmartstone->Pets;
        auto costumes = sSmartstone->Costumes;

        const auto actionKey = sSmartstone->DecodeActionId(action);
        if (!actionKey.has_value())
        {
            player->SendSystemMessage("Invalid action selected.");
            return;
        }
        const auto actionId = actionKey->actionId;
        const auto actionType = actionKey->type;

        /**
         * @todo: refactor in multiple functions
         *
         */
        switch (actionType)
        {
            case ACTION_TYPE_CATEGORY:
            {
                // Save current state before navigating to new category
                PushMenuState(player);
                // Show items in the selected category
                ShowCategoryItems(actionId, player, item, subscriptionLevel);
                return;
            }
            case ACTION_TYPE_UTIL:
            {
                switch (actionId)
                {
                    case SMARTSTONE_ACTION_BACK:
                    {
                        auto _menuHistory = sSmartstone->MenuStateHolder[player->GetGUID()];
                        if (!_menuHistory.empty())
                        {
                            SmartstoneMenuState previousState = _menuHistory.back();
                            _menuHistory.pop_back();

                            // Restore previous state
                            _currentMenuState[player->GetGUID()].lastCategory = previousState.lastCategory;
                            _currentMenuState[player->GetGUID()].currentPage = previousState.currentPage;

                            // Show the previous category (don't clear the restored state!)
                            auto const& _menuState = _currentMenuState[player->GetGUID()];
                            ShowCategoryItems(_menuState.lastCategory, player, item, subscriptionLevel, _menuState.currentPage);
                        }
                        else
                        {
                            // No history, go to main menu
                            ClearLastCategory(player);
                            ShowMainMenu(player, item, subscriptionLevel);
                        }
                        return;
                    }
                    case SMARTSTONE_ACTION_NEXT_PAGE:
                    case SMARTSTONE_ACTION_LAST_PAGE:
                    {
                        actionId == SMARTSTONE_ACTION_LAST_PAGE ? PreviousPage(player) : NextPage(player);
                        auto const& _menuState = _currentMenuState[player->GetGUID()];
                        ShowCategoryItems(_menuState.lastCategory, player, item, subscriptionLevel, _menuState.currentPage);
                        return;
                    }
                    case SMARTSTONE_ACTION_UNSUMMON_COMPANION:
                    {
                        if (Creature* cr = player->GetCompanionPet())
                            cr->DespawnOrUnsummon();
                        break;
                    }
                    case SMARTSTONE_ACTION_UNSUMMON_PET:
                    {
                        if (Guardian* cr = player->GetGuardianPet())
                            cr->DespawnOrUnsummon();
                        break;
                    }
                    case SMARTSTONE_ACTION_REMOVE_COSTUME:
                    {
                        if (sSmartstone->GetCurrentCostume(player))
                        {
                            player->DeMorph();
                            sSmartstone->SetCurrentCostume(player, 0);
                        }
                        break;
                    }
                    case SMARTSTONE_ACTION_REMOVE_AURA:
                    {
                        sSmartstone->removeCurrentAura(player);
                        break;
                    }
                }
                break;
            }
            case ACTION_TYPE_SERVICE:
            {
                switch (actionId)
                {
                    case SERVICE_BARBERSHOP:
                    {
                        if (sSmartstone->GetBarberDuration() == Seconds::zero())
                        {
                            player->SendSystemMessage("Barbershop service is currently unavailable.");
                            break;
                        }

                        if (!sSmartstone->CanUseSmartstone(player))
                        {
                            player->SendSystemMessage("You cannot use this feature inside instances or battlegrounds.");
                            break;
                        }

                        if (player->FindNearestCreature(NPC_BARBER, 100.0f))
                        {
                            player->SendSystemMessage("The barber is already summoned.");
                            break;
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
                                barber->DespawnOrUnsummon(4s);
                            }, sSmartstone->GetBarberDuration());
                        }
                        break;
                    }
                }
                break;
            }
            case ACTION_TYPE_PET:
            {
                if (!sSmartstone->CanUseSmartstone(player))
                {
                    player->SendSystemMessage("You cannot use this feature inside instances or battlegrounds.");
                    break;
                }

                player->CastCustomSpell(90001, SPELLVALUE_MISCVALUE0, actionId);
                break;
            }
            case ACTION_TYPE_COMPANION:
            {
                player->CastCustomSpell(90000, SPELLVALUE_MISCVALUE0, actionId);
                break;
            }
            case ACTION_TYPE_COSTUME:
            {

                if ((!sSmartstone->IsSmartstoneCanUseInBG() && player->InBattleground()) ||
                    (!sSmartstone->IsSmartstoneCanUseInArena() && player->InArena()))
                    {
                    player->SendSystemMessage("You cannot use this feature in battlegrounds or arenas.");
                    break;
                }

                if (!sSmartstone->IsSmartstoneCanUseInCombat() && player->IsInCombat()) {
                    player->SendSystemMessage("You cannot use this feature while in combat.");
                    break;
                }

                if (!sSmartstone->IsSmartstoneCanUseInPvP() && player->IsInCombat() && player->IsPvP()) {
                    player->SendSystemMessage("You cannot use this feature while in PvP.");
                    break;
                }

                if (player->HasSpellCooldown(90002) && !player->GetCommandStatus(CHEAT_COOLDOWN))
                {
                    uint32 remaining = player->GetSpellCooldownDelay(90002); // in milliseconds
                    uint32 seconds = remaining / 1000;
                    uint32 minutes = seconds / 60;
                    seconds = seconds % 60;
                    std::string message = Acore::StringFormat("You cannot use this feature for another {} minute(s) and {} second(s).", minutes, seconds);
                    player->SendSystemMessage(message);
                    break;
                }

                sSmartstone->ApplyCostume(player, actionId);

                break;
            }
            case ACTION_TYPE_AURA:
            {
                SmartstoneAuraData aura = sSmartstone->GetAuraData(actionId);
                if (player->HasAura(aura.SpellID))
                {
                    player->SendSystemMessage("You already have this aura active.");
                    break;
                }

                sSmartstone->removeCurrentAura(player);

                sSmartstone->SetCurrentAura(player, aura.SpellID);
                player->AddAura(aura.SpellID, player);
                player->SendSystemMessage(aura.Description + " is now active.");
                break;
            }
            case ACTION_TYPE_VEHICLES:
            {
                if (!sSmartstone->CanUseSmartstone(player))
                {
                    player->SendSystemMessage("You cannot use this feature inside instances or battlegrounds.");
                    break;
                }

                Creature* creature = player->SummonCreature(actionId, *player, TEMPSUMMON_MANUAL_DESPAWN);
                ObjectGuid vehicleGuid = creature->GetGUID();
                player->m_Events.AddEventAtOffset([player, actionId, vehicleGuid] {
                    if (Creature* vehicle = ObjectAccessor::GetCreature(*player, vehicleGuid))
                    {
                        player->CastCustomSpell(60683, SPELLVALUE_BASE_POINT0, 1, vehicle, true);
                        if (vehicle->CanFly())
                        {
                            vehicle->GetMotionMaster()->MoveTakeoff(0, *player);
                            vehicle->SetDisableGravity(true);
                            vehicle->SetCanFly(true);
                        }
                    }
                }, 500ms);

                break;
            }
            case ACTION_TYPE_NONE:
            case MAX_ACTION_TYPE:
            default:
            {
                // Invalid action type, show an error message
                player->SendSystemMessage("Invalid action type selected.");
                break;
            }
        }

        // As default, after processing an action, we return to the main menu
        ShowMainMenu(player, item, subscriptionLevel);
    }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/) override
    {
        if (!sSmartstone->IsSmartstoneEnabled())
            return false;

        // Check if smartstone data is properly initialized
        if (sSmartstone->MenuItems.empty())
        {
            player->SendSystemMessage("Smartstone is not properly configured. Please contact an administrator.");
            return false;
        }

        player->PlayerTalkClass->ClearMenus();

        ClearMenuHistory(player);

        uint8 subscriptionLevel = GetPlayerSubscriptionLevel(player);

        ShowMainMenu(player, item, subscriptionLevel);
        return false;
    }

    private:
        std::unordered_map<ObjectGuid, SmartstoneMenuState> _currentMenuState;

        /**
         * @brief Sets the current category ID for menu navigation tracking.
         * Used to remember which category the player is currently viewing, enabling proper
         * back navigation and state management when moving between menu levels.
         *
         * @param category The category ID to set as current (0 for main menu)
         */
        void SetLastCategory(Player* player, uint32 category) { _currentMenuState[player->GetGUID()].lastCategory = category; }

        /**
         * @brief Clears the current category by resetting it to 0 (main menu).
         * Used when returning to the main menu or resetting the smartstone state.
         * Called during initialization and when navigating back to the root level.
         */
        void ClearLastCategory(Player* player) { SetLastCategory(player, 0); }

        /**
         * @brief Resets pagination to the first page (page 0).
         * Called when initializing the smartstone interface or when starting fresh
         * navigation to ensure the user sees the first page of available items.
         */
        void ResetPagination(Player* player) { _currentMenuState[player->GetGUID()].currentPage = 0; }

        /**
         * @brief Advances to the next page in the current category.
         * Used when the player selects "Next page" to view more items in a category
         * that has more items than can fit on a single page (20 items per page).
         */
        void NextPage(Player* player) { ++_currentMenuState[player->GetGUID()].currentPage; }

        /**
         * @brief Goes back to the previous page in the current category.
         * Only decrements if current page is greater than 0 to prevent underflow.
         * Used when the player selects "Previous page" to view earlier items.
         */
        void PreviousPage(Player* player)
        {
            if (_currentMenuState[player->GetGUID()].currentPage)
                --_currentMenuState[player->GetGUID()].currentPage;
        }

        /**
         * @brief Saves the current menu state (page, category) to navigation history.
         * Creates a snapshot of the current menu state before navigating to a new category,
         * enabling the "Back" functionality to restore the exact previous state including
         * the current page number and category context.
         */
        void PushMenuState(Player* player) { sSmartstone->GetMenuStates(player->GetGUID()).emplace_back(_currentMenuState[player->GetGUID()]); }

        /**
         * @brief Clears all navigation history.
         * Used during smartstone initialization to start with a clean navigation state,
         * preventing stale history from previous sessions from affecting current navigation.
         */
        void ClearMenuHistory(Player* player) { sSmartstone->MenuStateHolder[player->GetGUID()].clear(); }

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

            // Update current page
            _currentMenuState[player->GetGUID()].currentPage = currentPage;

            // Check if category exists
            if (sSmartstone->MenuItems.find(ParentCategoryId) == sSmartstone->MenuItems.end())
            {
                player->SendSystemMessage("Category not found.");

                ClearMenuHistory(player);
                ShowMainMenu(player, item, GetPlayerSubscriptionLevel(player));
                return;
            }

            auto const& menuItems = sSmartstone->MenuItems[ParentCategoryId];
            auto& expireInfo = sSmartstone->ServiceExpireInfo[player->GetGUID().GetCounter()];

            std::map<uint32, tm> expireInfoMap;
            for (auto const& info : expireInfo)
                expireInfoMap[info.ServiceId] = Acore::Time::TimeBreakdown(info.ExpirationTime);

            int32 menuItemIndex = 0;
            uint32 itemsPerPage = 20;
            uint32 pageNumber = currentPage;

            /**
             * Utility buttons
             */
            if (player->GetCompanionPet())
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem(menuItemIndex++, 0,
                    "|TInterface/icons/Spell_Nature_SpiritWolf:30:30:-18:0|t Unsummon current companion",
                    0, sSmartstone->GetActionTypeId(ACTION_TYPE_UTIL, SMARTSTONE_ACTION_UNSUMMON_COMPANION), "", 0);

            if (player->GetGuardianPet())
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem(menuItemIndex++, 0,
                    "|TInterface/icons/Spell_Nature_SpiritWolf:30:30:-18:0|t Unsummon current pet",
                    0, sSmartstone->GetActionTypeId(ACTION_TYPE_UTIL, SMARTSTONE_ACTION_UNSUMMON_PET), "", 0);

            if (sSmartstone->GetCurrentCostume(player))
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem(menuItemIndex++, 0,
                    "|TInterface/PaperDollInfoFrame/UI-GearManager-Undo:30:30:-18:0|t Remove current costume",
                    0, sSmartstone->GetActionTypeId(ACTION_TYPE_UTIL, SMARTSTONE_ACTION_REMOVE_COSTUME), "", 0);

            if (sSmartstone->GetCurrentAura(player))
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem(menuItemIndex++, 0,
                    "|TInterface/icons/Spell_Nature_WispSplode:30:30:-18:0|t Remove current aura",
                    0, sSmartstone->GetActionTypeId(ACTION_TYPE_UTIL, SMARTSTONE_ACTION_REMOVE_AURA), "", 0);

            /**
             * Paginated items
             */
            uint32 shownCount = 0;
            uint32 skipped = 0;
            uint32 totalVisible = 0;

            for (uint32 idx = 0; idx < menuItems.size(); ++idx)
            {
                const auto& menuItem = menuItems[idx];
                bool available = false;

                // Availability checks
                if (menuItem.ServiceType == ACTION_TYPE_PET)
                {
                    auto pet = sSmartstone->GetPetData(menuItem.ItemId, ACTION_TYPE_PET);
                    if (pet.CreatureId && sSmartstone->IsPetAvailable(player, pet, subscriptionLevel))
                        available = true;
                }
                else if (menuItem.ServiceType == ACTION_TYPE_COMPANION)
                {
                    auto pet = sSmartstone->GetPetData(menuItem.ItemId, ACTION_TYPE_COMPANION);
                    if (pet.CreatureId && sSmartstone->IsPetAvailable(player, pet, subscriptionLevel))
                        available = true;
                }
                else if (menuItem.ServiceType == ACTION_TYPE_COSTUME)
                {
                    if (sSmartstone->IsServiceAvailable(player, "#costume", menuItem.ItemId - 20000)
                        || subscriptionLevel >= menuItem.SubscriptionLevelRequired)
                        available = true;
                }
                else if (menuItem.ServiceType == ACTION_TYPE_AURA)
                {
                    if (sSmartstone->IsServiceAvailable(player, "#aura", menuItem.ItemId)
                        || subscriptionLevel >= menuItem.SubscriptionLevelRequired)
                        available = true;
                }
                else if (menuItem.ServiceType == ACTION_TYPE_CATEGORY)
                {
                    if (sSmartstone->IsServiceAvailable(player, "#category", menuItem.ItemId)
                        || subscriptionLevel >= menuItem.SubscriptionLevelRequired)
                        available = true;
                }
                else if (menuItem.ServiceType == ACTION_TYPE_SERVICE)
                {
                    if (subscriptionLevel >= menuItem.SubscriptionLevelRequired)
                        available = true;
                }
                else if (menuItem.ServiceType == ACTION_TYPE_VEHICLES)
                {
                    if (sSmartstone->IsServiceAvailable(player, "#vehicle", menuItem.ItemId)
                        || subscriptionLevel >= menuItem.SubscriptionLevelRequired)
                        available = true;
                }

                if (!available)
                    continue;

                totalVisible++;

                // Skip until correct page start
                if (skipped < pageNumber * itemsPerPage)
                {
                    skipped++;
                    continue;
                }

                if (shownCount >= itemsPerPage)
                    break;

                // Add item to gossip
                if (menuItem.ServiceType == ACTION_TYPE_PET)
                {
                    auto pet = sSmartstone->GetPetData(menuItem.ItemId, ACTION_TYPE_PET);
                    std::string expireMsg;
                    if (expireInfoMap[pet.CreatureId].tm_year && expireInfoMap[pet.CreatureId].tm_year != 1900)
                        expireMsg = Acore::StringFormat("\n(Expires: {:%Y-%m-%d %H:%M})", expireInfoMap[pet.CreatureId]);

                    player->PlayerTalkClass->GetGossipMenu().AddMenuItem(
                        menuItemIndex++, 0, pet.Description + expireMsg, 0,
                        sSmartstone->GetActionTypeId(ACTION_TYPE_PET, pet.CreatureId), "", 0);
                }
                else if (menuItem.ServiceType == ACTION_TYPE_COMPANION)
                {
                    auto pet = sSmartstone->GetPetData(menuItem.ItemId, ACTION_TYPE_COMPANION);
                    player->PlayerTalkClass->GetGossipMenu().AddMenuItem(
                        menuItemIndex++, 0, pet.Description, 0,
                        sSmartstone->GetActionTypeId(ACTION_TYPE_COMPANION, pet.CreatureId), "", 0);
                }
                else if (menuItem.ServiceType == ACTION_TYPE_COSTUME)
                {
                    SmartstoneCostumeData costume = sSmartstone->GetCostumeData(menuItem.ItemId);
                    if (costume.Id)
                        player->PlayerTalkClass->GetGossipMenu().AddMenuItem(
                            menuItemIndex++, GOSSIP_ICON_TABARD, costume.Description, 0,
                            sSmartstone->GetActionTypeId(ACTION_TYPE_COSTUME, costume.Id), "", 0);
                }
                else if (menuItem.ServiceType == ACTION_TYPE_AURA)
                {
                    player->PlayerTalkClass->GetGossipMenu().AddMenuItem(
                        menuItemIndex++, GOSSIP_ICON_CHAT, menuItem.Text, 0,
                        sSmartstone->GetActionTypeId(ACTION_TYPE_AURA, menuItem.ItemId), "", 0);
                }
                else if (menuItem.ServiceType == ACTION_TYPE_CATEGORY)
                {
                    player->PlayerTalkClass->GetGossipMenu().AddMenuItem(
                        menuItemIndex++, GOSSIP_ICON_CHAT, menuItem.Text, 0,
                        sSmartstone->GetActionTypeId(ACTION_TYPE_CATEGORY, menuItem.ItemId), "", 0);
                }
                else if (menuItem.ServiceType == ACTION_TYPE_SERVICE)
                {
                    player->PlayerTalkClass->GetGossipMenu().AddMenuItem(
                        menuItemIndex++, GOSSIP_ICON_CHAT, menuItem.Text, 0,
                        sSmartstone->GetActionTypeId(ACTION_TYPE_SERVICE, menuItem.ItemId), "", 0);
                }
                else if (menuItem.ServiceType == ACTION_TYPE_VEHICLES)
                {
                    player->PlayerTalkClass->GetGossipMenu().AddMenuItem(
                        menuItemIndex++, GOSSIP_ICON_CHAT, menuItem.Text, 0,
                        sSmartstone->GetActionTypeId(ACTION_TYPE_VEHICLES, menuItem.ItemId), "", 0);
                }

                shownCount++;
            }

            /**
             * Navigation buttons
             */
            if (menuItemIndex == 0)
            {
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem(menuItemIndex++, GOSSIP_ICON_TALK,
                    "No actions available.", 0,
                    sSmartstone->GetActionTypeId(ACTION_TYPE_UTIL, SMARTSTONE_ACTION_NONE), "", 0);
            }
            else
            {
                if ((pageNumber + 1) * itemsPerPage < totalVisible)
                    player->PlayerTalkClass->GetGossipMenu().AddMenuItem(menuItemIndex++, GOSSIP_ICON_DOT,
                        "|TInterface/icons/Spell_ChargePositive:30:30:-18:0|t Next page",
                        0, sSmartstone->GetActionTypeId(ACTION_TYPE_UTIL, SMARTSTONE_ACTION_NEXT_PAGE), "", 0);

                if (pageNumber > 0)
                    player->PlayerTalkClass->GetGossipMenu().AddMenuItem(menuItemIndex++, GOSSIP_ICON_DOT,
                        "|TInterface/icons/Spell_ChargeNegative:30:30:-18:0|t Previous page",
                        0, sSmartstone->GetActionTypeId(ACTION_TYPE_UTIL, SMARTSTONE_ACTION_LAST_PAGE), "", 0);
            }

            // Back button
            if (ParentCategoryId != CATEGORY_MAIN)
                player->PlayerTalkClass->GetGossipMenu().AddMenuItem(menuItemIndex++, GOSSIP_ICON_DOT,
                    "Back", 0, sSmartstone->GetActionTypeId(ACTION_TYPE_UTIL, SMARTSTONE_ACTION_BACK), "", 0);

            SetLastCategory(player, ParentCategoryId);
            player->PlayerTalkClass->SendGossipMenu(sSmartstone->GetNPCTextForCategory(0, ParentCategoryId), item->GetGUID());
        }
    };

class mod_chromiecraft_smartstone_worldscript : public WorldScript
{
public:
    mod_chromiecraft_smartstone_worldscript() : WorldScript("mod_chromiecraft_smartstone_worldscript") { }

    void OnAfterConfigLoad(bool reload) override
    {
        sSmartstone->SetEnabled(sConfigMgr->GetOption<bool>("ModChromiecraftSmartstone.Enable", false));
        sSmartstone->SetBarberDuration(Seconds(sConfigMgr->GetOption<int32>("ModChromiecraftSmartstone.Features.BarberDuration", 300)));
        sSmartstone->SetSmartstoneItemID(sConfigMgr->GetOption<uint32>("ModChromiecraftSmartstone.ItemID", 32547));
        sSmartstone->SetCanUseInArena(sConfigMgr->GetOption("ModChromiecraftSmartstone.CanUseInArena", false));
        sSmartstone->SetCanUseInBG(sConfigMgr->GetOption("ModChromiecraftSmartstone.CanUseInBg", false));
        sSmartstone->SetCanUseInCombat(sConfigMgr->GetOption("ModChromiecraftSmartstone.CanUseInCombat", false));
        sSmartstone->SetCanUseInPvP(sConfigMgr->GetOption("ModChromiecraftSmartstone.CanUseInPvP", false));
        sSmartstone->SetDebugEnabled(sConfigMgr->GetOption("ModChromiecraftSmartstone.Debug", false));

        if (!reload)
            sSmartstone->LoadSmartstoneData();
    }
};

class mod_chromiecraft_smartstone_playerscript : public PlayerScript
{
public:
    mod_chromiecraft_smartstone_playerscript() : PlayerScript("mod_chromiecraft_smartstone_playerscript") { }

    void OnPlayerLogin(Player* player) override
    {
        if (sSmartstone->IsSmartstoneEnabled())
        {
            sSmartstone->LoadAccountSettings(player->GetSession()->GetAccountId());
        }
    }

    void OnPlayerBeforeLogout(Player* player) override
    {
        if (sSmartstone->IsSmartstoneEnabled())
        {
            sSmartstone->removeCurrentAura(player);
            sSmartstone->ClearAccountSettings(player->GetSession()->GetAccountId());
        }
    }

    void OnPlayerMapChanged(Player* player) override
    {
        if (!sSmartstone->IsSmartstoneEnabled())
            return;

        if ((!sSmartstone->IsSmartstoneCanUseInBG() && player->InBattleground()) ||
            (!sSmartstone->IsSmartstoneCanUseInArena() && player->InArena()))
        {
            player->DeMorph();
            sSmartstone->SetCurrentCostume(player, 0);
        }

        player->m_Events.AddEventAtOffset([player] {
            if (uint32 currentCostume = sSmartstone->GetCurrentCostume(player))
            {
                SmartstoneCostumeData const& costume = sSmartstone->GetCostumeDataByDisplayId(currentCostume);
                sSmartstone->ApplyCostume(player, costume);
            }
        }, 1s);
    }
};

struct npc_smartstone_vehicle : public VehicleAI
{
    npc_smartstone_vehicle(Creature* pCreature) : VehicleAI(pCreature) {}

    void PassengerBoarded(Unit*, int8, bool apply) override
    {
        if (!apply)
        {
            me->SetDisplayId(11686); // prevents nasty falling animation at despawn
            me->DespawnOrUnsummon(1ms);
        }
    }
};


void Addmod_cc_smartstoneScripts()
{
    new item_chromiecraft_smartstone();
    new mod_chromiecraft_smartstone_worldscript();
    new mod_chromiecraft_smartstone_playerscript();
    RegisterCreatureAI(npc_smartstone_vehicle);
}
