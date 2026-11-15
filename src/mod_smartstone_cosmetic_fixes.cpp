/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 *
 * Fixes for side effects of perk skins, where a skin's creature model makes the
 * client or core behave differently than with the form's normal model.
 *
 * Ghost Wolf: Skoll collision:
 *   The Skoll display uses a model without collision data, so the client
 *   swims too deep and the server starts the breath bar.
 *
 * Form skin weapons:
 *   Form skins use creature models with weapon attach points, so the equipped
 *   weapon shows on the paw.
 *
 * Form skin mount rule:
 *   The core decides mounting from the shown display. A Moonkin / Tree of
 *   Life skin (Astral Form included) allows it where the normal model does
 *   not, so the normal model's rule is applied while a skin is shown.
 */

#include "DBCStores.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Smartstone.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "WorldPacket.h"
#include "WorldSession.h"

#include <algorithm>
#include <array>
#include <string_view>
#include <vector>

namespace
{
    // Ghost Wolf: Skoll (display 29673) uses model 3155, which has no collision data.
    constexpr uint32       MODEL_GHOST_WOLF_SKOLL     = 29673;
    constexpr Milliseconds SKOLL_COLLISION_SYNC_DELAY = 500ms;

    // Per-player flag: a Ghost Wolf collision height was sent to the client and
    // the race height must be restored once the player leaves the form.
    struct SmartstoneSkollCollisionData : public DataMap::Base
    {
        bool WolfHeightSent = false;
    };

    constexpr char const* SMARTSTONE_SKOLL_COLLISION_KEY = "SmartstoneSkollCollision";

    float GetDisplayCollisionHeight(Player* player, uint32 displayId)
    {
        // Same formula as Unit::GetCollisionHeight (unmounted branch), but for the
        // given display instead of the native one.
        float scaleMod = player->GetObjectScale();
        float defaultHeight = DEFAULT_COLLISION_HEIGHT * scaleMod;

        CreatureDisplayInfoEntry const* displayInfo = sCreatureDisplayInfoStore.LookupEntry(displayId);
        if (!displayInfo)
            return defaultHeight;

        CreatureModelDataEntry const* modelData = sCreatureModelDataStore.LookupEntry(displayInfo->ModelId);
        if (!modelData)
            return defaultHeight;

        float collisionHeight = scaleMod * modelData->CollisionHeight * modelData->Scale * displayInfo->scale;
        return collisionHeight == 0.0f ? defaultHeight : collisionHeight;
    }

    void SendCollisionHeight(Player* player, float height)
    {
        WorldPacket data(SMSG_MOVE_SET_COLLISION_HGT, player->GetPackGUID().size() + 4 + 4);
        data << player->GetPackGUID();
        data << player->GetSession()->GetOrderCounter(); // movement counter
        data << height;
        player->SendDirectMessage(&data);
        player->GetSession()->IncrementOrderCounter();
    }

    // While Skoll is shown, the client is sent the standard Ghost Wolf collision
    // height; other Ghost Wolf displays get their own height, and the race height
    // is restored once the player leaves the form.
    void SyncSkollCollisionHeight(Player* player)
    {
        // Decides from the current state, not from the display that triggered the
        // sync. No IsSmartstoneEnabled() check: the revert must still run if the
        // module is disabled on reload.
        if (!player || !player->IsInWorld())
            return;

        bool wantWolf = player->GetShapeshiftForm() == FORM_GHOSTWOLF
            && player->GetDisplayId() == MODEL_GHOST_WOLF_SKOLL;

        if (wantWolf)
        {
            // Always resend: after login or a map change the client starts over.
            uint32 wolfDisplayId = player->GetModelForForm(FORM_GHOSTWOLF, SPELL_GHOST_WOLF);
            SendCollisionHeight(player, GetDisplayCollisionHeight(player, wolfDisplayId));
            auto* sent = player->CustomData.GetDefault<SmartstoneSkollCollisionData>(SMARTSTONE_SKOLL_COLLISION_KEY);
            sent->WolfHeightSent = true;
            return;
        }

        auto* data = player->CustomData.Get<SmartstoneSkollCollisionData>(SMARTSTONE_SKOLL_COLLISION_KEY);
        if (!data || !data->WolfHeightSent)
            return;

        // Still in Ghost Wolf with another display (reset, other skin, BG/arena):
        // send that display's height, not the race height. The flag stays set so
        // leaving the form still restores the race height.
        if (player->GetShapeshiftForm() == FORM_GHOSTWOLF)
        {
            SendCollisionHeight(player, GetDisplayCollisionHeight(player, player->GetDisplayId()));
            return;
        }

        SendCollisionHeight(player, player->GetCollisionHeight());
        data->WolfHeightSent = false;
    }

    // True while a druid form / Ghost Wolf perk skin is the live model (Astral
    // Form keeps the character's own model, so no).
    bool IsFormPerkHidingWeapons(Player* player)
    {
        uint32 canonicalModel = 0;
        if (!sSmartstone->GetActiveFormPerkSubstitute(player, canonicalModel))
            return false;

        if (player->GetShapeshiftForm() == FORM_MOONKIN
            && sSmartstone->GetDruidFormDisplay(player, DRUID_FORM_MOONKIN) == FORM_DISPLAY_OWN_MODEL)
            return false;

        return true;
    }

    // Mirrors the display part of Unit::IsInDisallowedMountForm, for `displayId`
    // instead of the unit's current display.
    bool IsMountDisallowedForDisplay(Unit const* unit, uint32 displayId)
    {
        if (displayId == unit->GetNativeDisplayId())
            return false;

        CreatureDisplayInfoEntry const* display = sCreatureDisplayInfoStore.LookupEntry(displayId);
        if (!display)
            return true;

        CreatureDisplayInfoExtraEntry const* displayExtra =
            sCreatureDisplayInfoExtraStore.LookupEntry(display->ExtendedDisplayInfoID);
        if (!displayExtra)
            return true;

        CreatureModelDataEntry const* model = sCreatureModelDataStore.LookupEntry(display->ModelId);
        ChrRacesEntry const* race = sChrRacesStore.LookupEntry(displayExtra->DisplayRaceID);

        return model && !model->HasFlag(CREATURE_MODEL_DATA_FLAGS_CAN_MOUNT)
            && race && !race->HasFlag(CHRRACES_FLAGS_CAN_MOUNT);
    }

    // True when the player shows a form skin whose canonical model would make
    // the core refuse mounting, while the skin's display makes it allow it.
    bool IsMountBlockedByFormPerk(Player* player)
    {
        // The core already refuses on its own (every non-stance form, Ghost Wolf included).
        if (player->IsInDisallowedMountForm())
            return false;

        // A transform that allows mounting lets the core allow it with the canonical model too.
        if (SpellInfo const* transformSpellInfo = sSpellMgr->GetSpellInfo(player->getTransForm()))
            if (transformSpellInfo->HasAttribute(SPELL_ATTR0_ALLOW_WHILE_MOUNTED))
                return false;

        uint32 canonicalModel = 0;
        if (!sSmartstone->GetActiveFormPerkSubstitute(player, canonicalModel))
            return false;

        return IsMountDisallowedForDisplay(player, canonicalModel);
    }

    // True when `spellId` is bound to the core script `scriptName` in spell_script_names.
    bool IsSpellBoundToScript(uint32 spellId, std::string_view scriptName)
    {
        SpellScriptsBounds bounds = sObjectMgr->GetSpellScriptsBounds(spellId);
        for (auto itr = bounds.first; itr != bounds.second; ++itr)
            if (sObjectMgr->GetScriptName(itr->second) == scriptName)
                return true;

        return false;
    }
}

// The sync is delayed because this hook runs inside SetDisplayId, before the
// display update reaches the client; a direct packet would arrive first.
class unit_smartstone_skoll_collision_script : public UnitScript
{
public:
    unit_smartstone_skoll_collision_script() : UnitScript("unit_smartstone_skoll_collision_script", true, {
        UNITHOOK_ON_DISPLAYID_CHANGE
    }) { }

    void OnDisplayIdChange(Unit* unit, uint32 displayId) override
    {
        Player* player = unit->ToPlayer();
        if (!player)
            return;

        if (displayId != MODEL_GHOST_WOLF_SKOLL)
        {
            auto const* data = player->CustomData.Get<SmartstoneSkollCollisionData>(SMARTSTONE_SKOLL_COLLISION_KEY);
            if (!data || !data->WolfHeightSent)
                return;
        }

        player->m_Events.AddEventAtOffset([player] {
            SyncSkollCollisionHeight(player);
        }, SKOLL_COLLISION_SYNC_DELAY);
    }
};

// Only the packet changes: the wearer gets zeroed weapon visible-item fields
// (their character panel reads the real items), other players get the disarmed
// flags (their Inspect reads the visible-item fields, so those stay intact).
static constexpr std::array<uint16, 4> WeaponVisibleFields =
{
    PLAYER_VISIBLE_ITEM_16_ENTRYID,
    PLAYER_VISIBLE_ITEM_16_ENCHANTMENT,
    PLAYER_VISIBLE_ITEM_17_ENTRYID,
    PLAYER_VISIBLE_ITEM_17_ENCHANTMENT
};

class unit_smartstone_form_weapon_script : public UnitScript
{
public:
    unit_smartstone_form_weapon_script() : UnitScript("unit_smartstone_form_weapon_script", true, {
        UNITHOOK_SHOULD_TRACK_VALUES_UPDATE_POS_BY_INDEX,
        UNITHOOK_ON_PATCH_VALUES_UPDATE,
        UNITHOOK_ON_DISPLAYID_CHANGE
    }) { }

    // Called while building the shared cache: depends only on unit type and index.
    bool ShouldTrackValuesUpdatePosByIndex(Unit const* unit, uint8 /*updateType*/, uint16 index) override
    {
        if (!unit->IsPlayer())
            return false;

        if (index == UNIT_FIELD_FLAGS_2)
            return true;

        return std::find(WeaponVisibleFields.begin(), WeaponVisibleFields.end(), index) != WeaponVisibleFields.end();
    }

    void OnPatchValuesUpdate(Unit const* unit, ByteBuffer& valuesUpdateBuf, BuildValuesCachePosPointers& posPointers,
        Player* target) override
    {
        if (!unit->IsPlayer() || unit->ToPlayer()->GetShapeshiftForm() == FORM_NONE)
            return;

        bool const self = target == unit;
        auto const flags2It = posPointers.other.find(UNIT_FIELD_FLAGS_2);
        bool const hasFlags2 = flags2It != posPointers.other.end();

        if (self)
        {
            bool hasWeaponField = false;
            for (uint16 field : WeaponVisibleFields)
            {
                if (posPointers.other.count(field))
                {
                    hasWeaponField = true;
                    break;
                }
            }

            if (!hasWeaponField)
                return;
        }
        else if (posPointers.UnitFieldFlagsPos < 0 && !hasFlags2)
            return;

        Player* wearer = const_cast<Player*>(unit->ToPlayer());
        if (!IsFormPerkHidingWeapons(wearer))
            return;

        // Observers opted out of form skins see the canonical model, which has no weapon issue.
        if (sSmartstone->IsDisplayOptOutEnabled() && sSmartstone->IsHidingForms(target))
            return;

        if (self)
        {
            for (uint16 field : WeaponVisibleFields)
                if (auto pos = posPointers.other.find(field); pos != posPointers.other.end())
                    valuesUpdateBuf.put(pos->second, uint32(0));
            return;
        }

        if (posPointers.UnitFieldFlagsPos >= 0)
            valuesUpdateBuf.put(posPointers.UnitFieldFlagsPos,
                valuesUpdateBuf.read<uint32>(posPointers.UnitFieldFlagsPos) | uint32(UNIT_FLAG_DISARMED));

        if (hasFlags2)
            valuesUpdateBuf.put(flags2It->second,
                valuesUpdateBuf.read<uint32>(flags2It->second) | uint32(UNIT_FLAG2_DISARM_OFFHAND));
    }

    // Entering or leaving a skin doesn't change these fields server-side, so
    // force a resend; the patch above decides the value at send time.
    void OnDisplayIdChange(Unit* unit, uint32 /*displayId*/) override
    {
        if (!unit->IsPlayer())
            return;

        unit->ForceValuesUpdateAtIndex(UNIT_FIELD_FLAGS);
        unit->ForceValuesUpdateAtIndex(UNIT_FIELD_FLAGS_2);
        for (uint16 field : WeaponVisibleFields)
            unit->ForceValuesUpdateAtIndex(field);
    }
};

// Mirrors the shapeshift removal of Player::ActivateTaxiPathTo without a flight
// master and of ContinueTaxiFlight, which have no hook before it: a druid whose
// form skin hides a disallowed canonical model loses the form on the next update
// of a flight (spell, SmartAI or script taxi).
class player_smartstone_form_mount_script : public PlayerScript
{
public:
    player_smartstone_form_mount_script() : PlayerScript("player_smartstone_form_mount_script", {
        PLAYERHOOK_ON_UPDATE
    }) { }

    void OnPlayerUpdate(Player* player, uint32 /*p_time*/) override
    {
        if (player->GetShapeshiftForm() == FORM_NONE || !player->IsInFlight())
            return;

        if (IsMountBlockedByFormPerk(player))
            player->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
    }
};

// Oculus drakes: spell_oculus_rider_aura drops a disallowed mount form once
// the rider is on the drake; do the same for a druid whose form skin hides a
// disallowed canonical model.
class unit_smartstone_form_mount_script : public UnitScript
{
public:
    unit_smartstone_form_mount_script() : UnitScript("unit_smartstone_form_mount_script", true, {
        UNITHOOK_ON_AURA_APPLY
    }) { }

    static constexpr char const* SCRIPT_OCULUS_RIDER_AURA = "spell_oculus_rider_aura";

    void OnAuraApply(Unit* /*unit*/, Aura* aura) override
    {
        // Runs for every aura: the GUID and spell checks skip the caster lookup for all
        // but the Oculus rider aura cast by a player.
        if (!aura->GetCasterGUID().IsPlayer())
            return;

        if (!IsSpellBoundToScript(aura->GetId(), SCRIPT_OCULUS_RIDER_AURA))
            return;

        Unit* caster = aura->GetCaster();
        Player* player = caster ? caster->ToPlayer() : nullptr;
        if (!player || player->GetShapeshiftForm() == FORM_NONE)
            return;

        if (!player->GetVehicleCreatureBase())
            return;

        if (IsMountBlockedByFormPerk(player))
            player->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
    }
};

// Flight master taxi and Trial of the Champion vehicles, for a druid whose form
// skin hides a disallowed canonical model. The checks the core runs before its
// shapeshift test are left to the core, so its own errors come first.
class server_smartstone_form_mount_script : public ServerScript
{
public:
    server_smartstone_form_mount_script() : ServerScript("server_smartstone_form_mount_script", {
        SERVERHOOK_CAN_PACKET_RECEIVE
    }) { }

    bool CanPacketReceive(WorldSession* session, WorldPacket const& packet) override
    {
        switch (packet.GetOpcode())
        {
            case CMSG_ACTIVATETAXI:
            case CMSG_ACTIVATETAXIEXPRESS:
                return CanActivateTaxi(session, packet);
            case CMSG_SPELLCLICK:
                HandleSpellClick(session, packet);
                return true;
            default:
                return true;
        }
    }

private:
    static constexpr char const* SCRIPT_TOC5_PLAYER_VEHICLE = "npc_toc5_player_vehicle";

    // Packet layouts: CMSG_ACTIVATETAXI is guid + 2 nodes, CMSG_ACTIVATETAXIEXPRESS
    // is guid + node count + nodes, CMSG_SPELLCLICK is guid.
    static constexpr std::size_t PACKET_GUID_SIZE = sizeof(uint64);
    static constexpr std::size_t TAXI_NODE_SIZE   = sizeof(uint32);
    static constexpr std::size_t TAXI_NODE_COUNT  = 2;

    static Player* GetShapeshiftedPlayer(WorldSession* session)
    {
        Player* player = session->GetPlayer();
        return player && player->GetShapeshiftForm() != FORM_NONE ? player : nullptr;
    }

    // Mirrors HandleActivateTaxi(Express)Opcode and the flight master branch of
    // Player::ActivateTaxiPathTo up to its IsInDisallowedMountForm check.
    static bool CanActivateTaxi(WorldSession* session, WorldPacket const& packet)
    {
        Player* player = GetShapeshiftedPlayer(session);
        if (!player)
            return true;

        std::size_t pos = PACKET_GUID_SIZE;
        std::size_t nodeCount = TAXI_NODE_COUNT;
        if (packet.GetOpcode() == CMSG_ACTIVATETAXIEXPRESS)
        {
            if (packet.size() < pos + sizeof(uint32))
                return true;

            nodeCount = packet.read<uint32>(pos);
            pos += sizeof(uint32);
        }

        // Short packets are left to the core handler.
        if (packet.size() < pos + nodeCount * TAXI_NODE_SIZE)
            return true;

        std::vector<uint32> nodes;
        nodes.reserve(nodeCount);
        for (std::size_t i = 0; i < nodeCount; ++i, pos += TAXI_NODE_SIZE)
            nodes.push_back(packet.read<uint32>(pos));

        ObjectGuid const npcGuid(packet.read<uint64>(0));
        if (!player->GetNPCIfCanInteractWith(npcGuid, UNIT_NPC_FLAG_FLIGHTMASTER))
            return true;

        if (!player->isTaxiCheater())
            for (uint32 node : nodes)
                if (!player->m_taxi.IsTaximaskNodeKnown(node))
                    return true;

        if (nodes.size() < TAXI_NODE_COUNT)
            return true;

        if (session->IsLoggingOut() || player->IsInCombat() || player->HasUnitState(UNIT_STATE_STUNNED)
            || player->HasUnitState(UNIT_STATE_ROOT))
            return true;

        if (player->HasUnitFlag(UNIT_FLAG_DISABLE_MOVE) || player->IsMounted())
            return true;

        if (!IsMountBlockedByFormPerk(player))
            return true;

        session->SendActivateTaxiReply(ERR_TAXIPLAYERSHAPESHIFTED);
        return false;
    }

    // Mirrors the BeforeSpellClick of npc_toc5_player_vehicle, which drops a
    // disallowed mount form before boarding. The click always goes on to the core.
    static void HandleSpellClick(WorldSession* session, WorldPacket const& packet)
    {
        Player* player = GetShapeshiftedPlayer(session);
        if (!player || packet.size() < PACKET_GUID_SIZE)
            return;

        Creature* creature = ObjectAccessor::GetCreatureOrPetOrVehicle(*player, ObjectGuid(packet.read<uint64>(0)));
        if (!creature || !creature->IsInWorld() || !creature->IsAIEnabled)
            return;

        if (creature->GetScriptName() != SCRIPT_TOC5_PLAYER_VEHICLE)
            return;

        if (IsMountBlockedByFormPerk(player))
            player->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
    }
};

// Mount spells fail with SPELL_FAILED_NOT_SHAPESHIFT, and the tournament mount
// and Jokkum summons drop the form, when the shown form skin hides a canonical
// model the core would refuse.
class spell_smartstone_form_mount_rule : public AllSpellScript
{
public:
    spell_smartstone_form_mount_rule() : AllSpellScript("spell_smartstone_form_mount_rule", {
        ALLSPELLHOOK_ON_SPELL_CHECK_CAST,
        ALLSPELLHOOK_ON_CAST
    }) { }

    // Core spell scripts that drop a disallowed mount form.
    static constexpr char const* SCRIPT_TOURNAMENT_MOUNT = "spell_gen_summon_tournament_mount";
    static constexpr char const* SCRIPT_JOKKUM_SUMMON    = "spell_q13010_jokkum_summon";

    void OnSpellCheckCast(Spell* spell, bool /*strict*/, SpellCastResult& res) override
    {
        if (res != SPELL_CAST_OK)
            return;

        Player* player = spell->GetCaster() ? spell->GetCaster()->ToPlayer() : nullptr;
        if (!player || player->GetShapeshiftForm() == FORM_NONE)
            return;

        // Runs for every spell: the spell filters come before the costlier perk check.
        SpellInfo const* spellInfo = spell->GetSpellInfo();

        // The core script drops the form before its lance check and lets the cast go on.
        // This hook runs at the start of Spell::CheckCast, so unlike the core the form also
        // drops when a generic check (cooldown, GCD, outdoors, combat) fails later.
        if (IsSpellBoundToScript(spellInfo->Id, SCRIPT_TOURNAMENT_MOUNT))
        {
            if (IsMountBlockedByFormPerk(player))
                player->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);

            return;
        }

        if (!spellInfo->HasAura(SPELL_AURA_MOUNTED))
            return;

        // Leave the errors of the core's SPELL_AURA_MOUNTED branch to the core. The generic
        // checks before it (cooldown, GCD, outdoors, combat) run after this hook, so their
        // errors give way to SPELL_FAILED_NOT_SHAPESHIFT; the cast fails either way.
        if (player->IsInWater() && spellInfo->HasAura(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED))
            return;

        Map const* map = player->GetMap();
        bool allowMount = !map->IsDungeon() || map->IsBattlegroundOrArena();
        if (InstanceTemplate const* instance = sObjectMgr->GetInstanceTemplate(player->GetMapId()))
            allowMount = instance->AllowMount;
        if (!allowMount && !spellInfo->AreaGroupId)
            return;

        if (IsMountBlockedByFormPerk(player))
            res = SPELL_FAILED_NOT_SHAPESHIFT;
    }

    void OnSpellCast(Spell* /*spell*/, Unit* caster, SpellInfo const* spellInfo, bool /*skipCheck*/) override
    {
        Player* player = caster ? caster->ToPlayer() : nullptr;
        if (!player || player->GetShapeshiftForm() == FORM_NONE)
            return;

        // The core script drops the form before summoning Jokkum; boarding doesn't depend on it.
        if (!IsSpellBoundToScript(spellInfo->Id, SCRIPT_JOKKUM_SUMMON))
            return;

        if (IsMountBlockedByFormPerk(player))
            player->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
    }
};

void AddSC_smartstone_cosmetic_fixes()
{
    new unit_smartstone_skoll_collision_script();
    new unit_smartstone_form_weapon_script();
    new player_smartstone_form_mount_script();
    new unit_smartstone_form_mount_script();
    new server_smartstone_form_mount_script();
    new spell_smartstone_form_mount_rule();
}
