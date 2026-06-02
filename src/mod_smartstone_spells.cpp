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
 *
 *   In BG / arena the override is suppressed — the engine's default
 *   model is left in place.
 */

#include "ScriptMgr.h"
#include "Smartstone.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "Unit.h"

class spell_smartstone_form_display_override : public AuraScript
{
    PrepareAuraScript(spell_smartstone_form_display_override);

    // Which namespace + slot a spell id maps to. `isShaman == false`
    // means the druid namespace (#druid_form); true means shaman.
    // `effect` is the SmartstonePerkEffect for this form, used to resolve
    // the per-context scale override.
    struct SlotInfo
    {
        int32 slot;       // -1 if the spell id isn't mapped.
        bool  isShaman;
        uint8 effect;
    };

    static SlotInfo GetSlotForSpell(uint32 spellId)
    {
        switch (spellId)
        {
            case  5487: case 9634:  return { DRUID_FORM_BEAR,        false, PERK_EFFECT_DRUID_FORM_BEAR    };
            case   768:             return { DRUID_FORM_CAT,         false, PERK_EFFECT_DRUID_FORM_CAT     };
            case   783:             return { DRUID_FORM_TRAVEL,      false, PERK_EFFECT_DRUID_FORM_TRAVEL  };
            case 33943: case 40120: return { DRUID_FORM_FLIGHT,      false, PERK_EFFECT_DRUID_FORM_FLIGHT  };
            case  1066:             return { DRUID_FORM_AQUATIC,     false, PERK_EFFECT_DRUID_FORM_AQUATIC };
            case 33891:             return { DRUID_FORM_TREE,        false, PERK_EFFECT_DRUID_FORM_TREE    };
            case 24858:             return { DRUID_FORM_MOONKIN,     false, PERK_EFFECT_DRUID_FORM_MOONKIN };
            case  2645:             return { SHAMAN_FORM_GHOST_WOLF, true,  PERK_EFFECT_SHAMAN_GHOST_WOLF  };
            default:                return { -1,                     false, PERK_EFFECT_NONE               };
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

        // Suppress overrides inside BG / arena — opponents shouldn't be
        // misled by custom skins.
        if (player->InBattleground() || player->InArena())
            return;

        uint32 displayId = info.isShaman
            ? sSmartstone->GetShamanFormDisplay(player, static_cast<uint8>(info.slot))
            : sSmartstone->GetDruidFormDisplay(player, static_cast<uint8>(info.slot));

        if (!displayId)
            return;

        // Scale depends on the form + chosen model (e.g. ghost-wolf-derived
        // creature models render far larger than the player); it's stored
        // per-(effect, displayId) in smartstone_perks.Scale. Falls back to
        // 1.0 when no override is configured.
        target->SetDisplayId(displayId, sSmartstone->GetEffectivePerkScale(info.effect, displayId));
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(
            spell_smartstone_form_display_override::HandleApply,
            EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
    }
};

// 51533 - Feral Spirit (shaman)
// Re-skins the two Spirit Wolves (entry 29264) summoned by Feral Spirit
// when the caster has a non-zero SHAMAN_GUARDIAN_FERAL_SPIRIT override
// stored. SpellScript hook fires exactly once per cast — cheaper than
// an AllCreatureScript filtering every creature spawn.
class spell_sha_feral_spirit_display : public SpellScript
{
    PrepareSpellScript(spell_sha_feral_spirit_display);

    static constexpr uint32 SPIRIT_WOLF_ENTRY = 29264;

    bool Load() override
    {
        Unit* caster = GetCaster();
        return caster && caster->IsPlayer();
    }

    void HandleAfterCast()
    {
        Player* player = GetCaster()->ToPlayer();
        if (!player)
            return;

        if (player->InBattleground() || player->InArena())
            return;

        uint32 displayId = sSmartstone->GetShamanGuardianDisplay(
            player, SHAMAN_GUARDIAN_FERAL_SPIRIT);
        if (!displayId)
            return;

        // By AfterCast the summon effects have run and the wolves are in
        // the caster's controlled set. Apply the override to each.
        for (Unit* controlled : player->m_Controlled)
        {
            if (controlled && controlled->GetEntry() == SPIRIT_WOLF_ENTRY)
            {
                controlled->SetDisplayId(displayId);
                // Per-(effect, model) scale from smartstone_perks.Scale;
                // leave the creature's natural scale when none is set.
                if (float scale = sSmartstone->GetPerkScale(PERK_EFFECT_SHAMAN_FERAL_SPIRIT, displayId); scale > 0.0f)
                    controlled->SetObjectScale(scale);
            }
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_sha_feral_spirit_display::HandleAfterCast);
    }
};

void AddSC_smartstone_spells()
{
    RegisterSpellScript(spell_smartstone_form_display_override);
    RegisterSpellScript(spell_sha_feral_spirit_display);
}
