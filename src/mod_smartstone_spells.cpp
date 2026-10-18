/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under
 * GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 *
 * Smartstone-owned spell scripts.
 *
 * spell_smartstone_form_display_override:
 *   Generic AuraScript bound to every form spell exposed by the perks
 *   system. The spell's id is mapped to (namespace, slot); the script
 *   reads the matching per-player setting and overrides the displayId
 *   when a non-zero value is stored. Zero / missing setting = no-op.
 *
 *   Bindings:
 *     Druid (#druid_form):
 *       0 - Bear / Dire Bear  (spell ids 5487, 9634)
 *       1 - Cat               (spell id   768)
 *       2 - Travel            (spell id   783)
 *       3 - Flight / Swift    (spell ids 33943, 40120)
 *       4 - Aquatic           (spell id  1066)
 *       5 - Tree of Life      (spell id 33891)
 *       6 - Moonkin           (spell id 24858)
 *     Shaman (#shaman_form):
 *       0 - Ghost Wolf        (spell id  2645)
 *     Priest (#priest_form):
 *       0 - Spirit of Redemption (spell id 27827; SPELL_AURA_MOD_SHAPESHIFT
 *         lives at a non-zero effect index, hence EFFECT_FIRST_FOUND below)
 *       1 - Shadowform              (spell id 15473)
 *
 *   In BG / arena the override is suppressed — the engine's default
 *   model is left in place.
 */

#include "Random.h"
#include "ScriptMgr.h"
#include "Smartstone.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "Unit.h"
#include <array>

class spell_smartstone_form_display_override : public AuraScript
{
    PrepareAuraScript(spell_smartstone_form_display_override);

    enum FormNamespace { NS_DRUID, NS_SHAMAN, NS_PRIEST };

    struct SlotInfo
    {
        int32         slot;   // -1 if the spell id isn't mapped.
        FormNamespace ns;
        uint8         effect; // SmartstonePerkEffect for scale lookup.
    };

    static SlotInfo GetSlotForSpell(uint32 spellId)
    {
        switch (spellId)
        {
            case  5487: case 9634:  return { DRUID_FORM_BEAR,                 NS_DRUID,  PERK_EFFECT_DRUID_FORM_BEAR              };
            case   768:             return { DRUID_FORM_CAT,                  NS_DRUID,  PERK_EFFECT_DRUID_FORM_CAT               };
            case   783:             return { DRUID_FORM_TRAVEL,               NS_DRUID,  PERK_EFFECT_DRUID_FORM_TRAVEL            };
            case 33943: case 40120: return { DRUID_FORM_FLIGHT,               NS_DRUID,  PERK_EFFECT_DRUID_FORM_FLIGHT            };
            case  1066:             return { DRUID_FORM_AQUATIC,              NS_DRUID,  PERK_EFFECT_DRUID_FORM_AQUATIC           };
            case 33891:             return { DRUID_FORM_TREE,                 NS_DRUID,  PERK_EFFECT_DRUID_FORM_TREE              };
            case 24858:             return { DRUID_FORM_MOONKIN,              NS_DRUID,  PERK_EFFECT_DRUID_FORM_MOONKIN           };
            case  2645:             return { SHAMAN_FORM_GHOST_WOLF,          NS_SHAMAN, PERK_EFFECT_SHAMAN_GHOST_WOLF            };
            case 27827:             return { PRIEST_FORM_SPIRIT_OF_REDEMPTION, NS_PRIEST, PERK_EFFECT_PRIEST_SPIRIT_OF_REDEMPTION  };
            case 15473:             return { PRIEST_FORM_SHADOW,              NS_PRIEST, PERK_EFFECT_PRIEST_SHADOWFORM           };
            default:                return { -1,                              NS_DRUID,  PERK_EFFECT_NONE                         };
        }
    }

    bool Load() override
    {
        Unit* caster = GetCaster();
        return caster && caster->IsPlayer();
    }

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        if (!target)
            return;

        Player* player = target->ToPlayer();
        if (!player)
            return;

        SlotInfo info = GetSlotForSpell(GetSpellInfo()->Id);
        if (info.slot < 0)
            return;

        if (info.ns == NS_DRUID && info.slot == DRUID_FORM_MOONKIN)
            sSmartstone->UpdateAstralFormVisual(player);

        // Suppress overrides inside BG / arena — opponents shouldn't be
        // misled by custom skins.
        if (player->InBattleground() || player->InArena())
            return;

        uint32 displayId = 0;
        switch (info.ns)
        {
            case NS_DRUID:  displayId = sSmartstone->GetDruidFormDisplay (player, static_cast<uint8>(info.slot)); break;
            case NS_SHAMAN: displayId = sSmartstone->GetShamanFormDisplay(player, static_cast<uint8>(info.slot)); break;
            case NS_PRIEST: displayId = sSmartstone->GetPriestFormDisplay(player, static_cast<uint8>(info.slot)); break;
        }
        if (!displayId)
            return;

        // Scale depends on the form + chosen model (e.g. ghost-wolf-derived
        // creature models render far larger than the player); it's stored
        // per-(effect, displayId) in smartstone_perks.Scale. Falls back to
        // 1.0 when no override is configured.
        float const scale = sSmartstone->GetEffectivePerkScale(info.effect, displayId);
        if (info.ns == NS_DRUID)
            displayId = Smartstone::ResolveFormDisplay(player, displayId);

        target->SetDisplayId(displayId, scale);
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        Player* player = target ? target->ToPlayer() : nullptr;
        if (!player)
            return;

        if (GetSpellInfo()->Id == 24858)
        {
            player->RemoveAurasDueToSpell(SPELL_ASTRAL_FORM_VISUAL);
            return;
        }

        // Only shadowform needs an explicit native-restore on remove —
        // every other form already drops back to the player's body via
        // the engine's normal removal flow.
        if (GetSpellInfo()->Id != 15473)
            return;

        // No-op when no override was applied.
        if (!sSmartstone->GetPriestFormDisplay(player, PRIEST_FORM_SHADOW))
            return;

        target->SetDisplayId(player->GetNativeDisplayId(), 1.0f);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(
            spell_smartstone_form_display_override::HandleApply,
            EFFECT_FIRST_FOUND, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(
            spell_smartstone_form_display_override::HandleRemove,
            EFFECT_FIRST_FOUND, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
    }
};

// 51533 - Feral Spirit (Spirit Wolves)
// 46585 - Raise Dead (guardian ghoul; the Master of Ghouls pet goes through the PetScript)
// 42651 - Army of the Dead (one ghoul per channel tick)
// 70907 - Summon Water Elemental (temporary)
// 70908 - Summon Water Elemental (Glyph of Eternal Water)
// Re-skins the guardians the cast summoned. Fires once per cast, which is
// cheaper than an AllCreatureScript filtering every creature spawn.
class spell_smartstone_guardian_display : public SpellScript
{
    PrepareSpellScript(spell_smartstone_guardian_display);

    bool Load() override
    {
        Unit* caster = GetCaster();
        return caster && caster->IsPlayer();
    }

    void HandleAfterCast()
    {
        // By AfterCast the summon effects have run and the guardians are in
        // the caster's controlled set. Non-perk minions are skipped by entry.
        sSmartstone->RefreshControlledMinionPerkDisplays(GetCaster());
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_smartstone_guardian_display::HandleAfterCast);
    }
};

// 42650 - Army of the Dead
// Plays the chosen skin's smartstone_costume_sound row (keyed by displayId)
// when the channel starts, so the caster hears e.g. murlocs instead of ghouls.
class spell_smartstone_army_of_the_dead_sound : public SpellScript
{
    PrepareSpellScript(spell_smartstone_army_of_the_dead_sound);

    bool Load() override
    {
        Unit* caster = GetCaster();
        return caster && caster->IsPlayer();
    }

    void HandleAfterCast()
    {
        Player* player = GetCaster()->ToPlayer();
        if (player->InBattleground() || player->InArena())
            return;

        if (uint32 displayId = sSmartstone->GetDeathKnightGuardianDisplay(player, DK_GUARDIAN_ARMY_OF_THE_DEAD))
            sSmartstone->PlayCostumeSound(player, displayId);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_smartstone_army_of_the_dead_sound::HandleAfterCast);
    }
};

// 118, 12824, 12825, 12826 - Polymorph (sheep)
// Swaps the sheep for the caster's Polymorph skin. Also hooked on
// SEND_FOR_CLIENT, since RestoreDisplayId re-runs the transform in that mode
// (e.g. when a shapeshift ends mid-Polymorph) and would bring the sheep back.
class spell_smartstone_polymorph_display : public AuraScript
{
    PrepareAuraScript(spell_smartstone_polymorph_display);

    // SpellVisualKit ids: Arcane Explosion cast, and the red / blue / green /
    // red-white-blue / yellow rose firework casts.
    static constexpr uint32 VISUAL_ARCANE_EXPLOSION = 988;
    static constexpr std::array<uint32, 5> FireworkVisuals = { 516, 2085, 2086, 2087, 2088 };

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes mode)
    {
        Player* mage = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
        Unit* target = GetTarget();
        if (!mage || !target)
            return;

        if (mage->InBattleground() || mage->InArena())
            return;

        uint32 const displayId = sSmartstone->GetPolymorphSkin(mage);
        if (!displayId)
            return;

        target->SetDisplayId(displayId, sSmartstone->GetEffectivePerkScale(PERK_EFFECT_MAGE_POLYMORPH, displayId));

        // Wild Polymorph flourish, only on the real apply (not on model re-sends).
        if ((mode & AURA_EFFECT_HANDLE_REAL) && sSmartstone->GetMageSpellDisplay(mage, MAGE_SPELL_WILD_POLYMORPH))
        {
            target->SendPlaySpellVisual(VISUAL_ARCANE_EXPLOSION);
            target->SendPlaySpellVisual(FireworkVisuals[urand(0, FireworkVisuals.size() - 1)]);
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_smartstone_polymorph_display::HandleApply,
            EFFECT_1, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_SEND_FOR_CLIENT_MASK);
    }
};

void AddSC_smartstone_spells()
{
    RegisterSpellScript(spell_smartstone_form_display_override);
    RegisterSpellScript(spell_smartstone_guardian_display);
    RegisterSpellScript(spell_smartstone_army_of_the_dead_sound);
    RegisterSpellScript(spell_smartstone_polymorph_display);
}
