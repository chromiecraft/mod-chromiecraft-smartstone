/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under
 * GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 *
 * Smartstone-owned pet scripts.
 *
 * smartstone_warlock_pet_script:
 *   PetScript that overrides a warlock pet's displayId when it enters
 *   the world. Reads the owner's per-player #warlock_pet setting; the
 *   slot is picked from the pet's creature entry. BG / arena suppress
 *   the override the same way druid / shaman forms do.
 *
 *   The hook fires for every transition into the world: fresh summon,
 *   stable swap, login restore, zone change. No separate reapply path
 *   is needed.
 */

#include "Pet.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Smartstone.h"

class smartstone_warlock_pet_script : public PetScript
{
public:
    smartstone_warlock_pet_script() : PetScript("smartstone_warlock_pet_script") { }

    void OnPetAddToWorld(Pet* pet) override
    {
        if (!pet)
            return;

        Unit* ownerUnit = pet->GetOwner();
        Player* owner = ownerUnit ? ownerUnit->ToPlayer() : nullptr;
        if (!owner || owner->getClass() != CLASS_WARLOCK)
            return;

        // BG / arena: always show the canonical model. Same policy as
        // druid forms / shaman ghost wolf.
        if (owner->InBattleground() || owner->InArena())
            return;

        int32 slot = Smartstone::GetWarlockPetSlotForEntry(pet->GetEntry());
        if (slot < 0)
            return;

        if (uint32 displayId = sSmartstone->GetWarlockPetDisplay(owner, static_cast<uint8>(slot)))
        {
            pet->SetDisplayId(displayId);

            // Shivarra: model is oversized relative to the canonical
            // succubus rig, so scale down to match.
            constexpr uint32 SHIVARRA_DISPLAY_ID = 21503;
            if (displayId == SHIVARRA_DISPLAY_ID)
                pet->SetObjectScale(0.6f);
        }
    }
};

void AddSC_smartstone_pets()
{
    new smartstone_warlock_pet_script();
}
