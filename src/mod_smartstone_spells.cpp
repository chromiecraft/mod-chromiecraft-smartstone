/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under
 * GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 *
 * Smartstone-owned spell scripts.
 *
 * spell_dru_form_display_override:
 *   Generic AuraScript bound to the four druid form spells. Reads a
 *   per-player setting (#druid_form namespace) to find the displayId to
 *   apply when the form is taken. A zero/missing setting means "use the
 *   default model" — the script becomes a no-op in that case.
 *
 *   Slot mapping (see DruidFormSlot in Smartstone.h):
 *     0 - Bear / Dire Bear  (spell ids 5487, 9634)
 *     1 - Cat               (spell id   768)
 *     2 - Travel            (spell id   783)
 *     3 - Flight / Swift    (spell ids 33943, 40120)
 *     4 - Aquatic           (spell id  1066)
 */

#include "ScriptMgr.h"
#include "Smartstone.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "Unit.h"

class spell_dru_form_display_override : public AuraScript
{
    PrepareAuraScript(spell_dru_form_display_override);

    bool Load() override
    {
        // Only run for player casters. Shapeshift form auras are self-cast,
        // so this also rules out anything cast by NPCs / pets.
        Unit* caster = GetCaster();
        return caster && caster->IsPlayer();
    }

    static int32 GetSlotForSpell(uint32 spellId)
    {
        switch (spellId)
        {
            case 5487: case 9634:   return DRUID_FORM_BEAR;
            case 768:               return DRUID_FORM_CAT;
            case 783:               return DRUID_FORM_TRAVEL;
            case 33943: case 40120: return DRUID_FORM_FLIGHT;
            case 1066:              return DRUID_FORM_AQUATIC;
            default:                return -1;
        }
    }

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        if (!target)
            return;

        Player* player = target->ToPlayer();
        if (!player)
            return;

        int32 slot = GetSlotForSpell(GetSpellInfo()->Id);
        if (slot < 0)
            return;

        // Form display overrides are never honoured inside battlegrounds
        // or arenas — keep the default model so opponents can't be misled
        // by custom skins.
        if (player->InBattleground() || player->InArena())
            return;

        uint32 displayId = sSmartstone->GetDruidFormDisplay(player, static_cast<uint8>(slot));
        if (displayId)
            target->SetDisplayId(displayId);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(
            spell_dru_form_display_override::HandleApply,
            EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_smartstone_spells()
{
    RegisterSpellScript(spell_dru_form_display_override);
}
