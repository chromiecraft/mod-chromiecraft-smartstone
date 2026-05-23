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
    struct SlotInfo
    {
        int32 slot;       // -1 if the spell id isn't mapped.
        bool  isShaman;
    };

    static SlotInfo GetSlotForSpell(uint32 spellId)
    {
        switch (spellId)
        {
            case  5487: case 9634:  return { DRUID_FORM_BEAR,         false };
            case   768:             return { DRUID_FORM_CAT,          false };
            case   783:             return { DRUID_FORM_TRAVEL,       false };
            case 33943: case 40120: return { DRUID_FORM_FLIGHT,       false };
            case  1066:             return { DRUID_FORM_AQUATIC,      false };
            case 33891:             return { DRUID_FORM_TREE,         false };
            case 24858:             return { DRUID_FORM_MOONKIN,      false };
            case  2645:             return { SHAMAN_FORM_GHOST_WOLF,  true  };
            default:                return { -1,                      false };
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

        // Shaman ghost-wolf-derived creature models render much larger
        // than the player; shrink to fit. Druid form models match the
        // existing form scale already.
        float scale = info.isShaman ? 0.5f : 1.0f;
        target->SetDisplayId(displayId, scale);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(
            spell_smartstone_form_display_override::HandleApply,
            EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_smartstone_spells()
{
    RegisterSpellScript(spell_smartstone_form_display_override);
}
