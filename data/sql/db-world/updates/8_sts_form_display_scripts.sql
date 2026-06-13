-- Bind every form spell exposed by the perks system to the unified
-- spell_smartstone_form_display_override AuraScript. The script consults
-- the per-player setting (#druid_form / #shaman_form, picked by spell id)
-- and overrides the displayId only when a non-zero value is stored.
-- Idempotent: re-applies cleanly across module reloads.
DELETE FROM `spell_script_names`
 WHERE `ScriptName` IN (
    'spell_smartstone_form_display_override',
    'spell_sha_feral_spirit_display',
    'spell_pri_spirit_of_redemption_display', -- short-lived split, now folded back in
    'spell_dru_form_display_override',     -- previous druid-only name
    'spell_sha_form_display_override',     -- previous shaman-only name
    'spell_dru_cat_form_panther'           -- obsolete prototype binding
 );

INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
( 5487, 'spell_smartstone_form_display_override'),  -- Druid: Bear Form
( 9634, 'spell_smartstone_form_display_override'),  -- Druid: Dire Bear Form
(  768, 'spell_smartstone_form_display_override'),  -- Druid: Cat Form
(  783, 'spell_smartstone_form_display_override'),  -- Druid: Travel Form
(33943, 'spell_smartstone_form_display_override'),  -- Druid: Flight Form
(40120, 'spell_smartstone_form_display_override'),  -- Druid: Swift Flight Form
( 1066, 'spell_smartstone_form_display_override'),  -- Druid: Aquatic Form
(33891, 'spell_smartstone_form_display_override'),  -- Druid: Tree of Life
(24858, 'spell_smartstone_form_display_override'),  -- Druid: Moonkin Form
( 2645, 'spell_smartstone_form_display_override'),  -- Shaman: Ghost Wolf
(27827, 'spell_smartstone_form_display_override'),  -- Priest: Spirit of Redemption
(15473, 'spell_smartstone_form_display_override'),  -- Priest: Shadowform
(51533, 'spell_sha_feral_spirit_display');          -- Shaman: Feral Spirit (Spirit Wolves)
