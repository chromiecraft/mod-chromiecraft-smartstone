-- Bind every druid form spell to spell_dru_form_display_override.
-- The script consults the per-player #druid_form setting and overrides
-- the displayId only when a non-zero value is stored for that slot.
-- Idempotent: re-applies cleanly across module reloads.
DELETE FROM `spell_script_names`
 WHERE `ScriptName` = 'spell_dru_form_display_override';

-- Also remove the obsolete prototype binding from an earlier iteration.
DELETE FROM `spell_script_names`
 WHERE `ScriptName` = 'spell_dru_cat_form_panther';

INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
( 5487, 'spell_dru_form_display_override'),  -- Bear Form
( 9634, 'spell_dru_form_display_override'),  -- Dire Bear Form
(  768, 'spell_dru_form_display_override'),  -- Cat Form
(  783, 'spell_dru_form_display_override'),  -- Travel Form
(33943, 'spell_dru_form_display_override'),  -- Flight Form
(40120, 'spell_dru_form_display_override'),  -- Swift Flight Form
( 1066, 'spell_dru_form_display_override'),  -- Aquatic Form
(33891, 'spell_dru_form_display_override');  -- Tree of Life Form
