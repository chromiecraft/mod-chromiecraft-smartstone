/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under
 * GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 *
 * Smartstone-owned pet scripts.
 *
 * smartstone_pet_display_script:
 *   PetScript that applies the owner's class-perk skin when a pet enters
 *   the world (warlock pets, Master of Ghouls ghoul). The skin is picked
 *   from the pet's creature entry. BG / arena suppress the override the
 *   same way druid / shaman forms do.
 *
 *   The hook fires for every transition into the world: fresh summon,
 *   stable swap, login restore, zone change. No separate reapply path
 *   is needed.
 */

#include "Pet.h"
#include "ScriptMgr.h"
#include "Smartstone.h"

class smartstone_pet_display_script : public PetScript
{
public:
    smartstone_pet_display_script() : PetScript("smartstone_pet_display_script") { }

    void OnPetAddToWorld(Pet* pet) override
    {
        if (pet)
            sSmartstone->RefreshMinionPerkDisplay(pet);
    }
};

void AddSC_smartstone_pets()
{
    new smartstone_pet_display_script();
}
