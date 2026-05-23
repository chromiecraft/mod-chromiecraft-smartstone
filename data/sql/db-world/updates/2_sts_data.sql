--
-- Smartstone Update Script
--

DELETE FROM `smartstone_categories` WHERE `Id` IN (1, 2, 3, 4, 5, 9);
INSERT INTO `smartstone_categories` (`Id`, `ParentCategory`, `Title`, `SubscriptionLevel`, `NPCTextId`, `Enabled`) VALUES
(1,0, '|TInterface/icons/Ability_Hunter_Invigeration:30:30:-18:0|t Pets', 0, 92002, 1),
(2,0, '|TInterface/icons/Ability_Hunter_KillCommand:30:30:-18:0|t Combat Pets', 0, 92003, 1),
(3,0, '|TInterface/icons/Ability_Rogue_Disguise:30:30:-18:0|t Costumes', 0, 92005, 1),
(4,0, '|TInterface/icons/Spell_Shadow_Charm:30:30:-18:0|t Character', 0, 92020, 1),
(5,3,'|TInterface/icons/Spell_Arcane_PrismaticCloak:30:30:-18:0|t Azeroth Villains',0, 92006,1),
(9,4, '|TInterface/icons/Spell_Nature_TimeStop:30:30:-18:0|t Experience rates', 0, 92019, 1);

DELETE FROM `smartstone_services` WHERE `ServiceId` IN (1, 2, 3, 4, 5);
INSERT INTO `smartstone_services` (`ServiceId`, `Category`, `Title`, `SubscriptionLevel`, `Enabled`) VALUES
(1, 4, '|TInterface/icons/Spell_Misc_ConjureManaJewel:30:30:-18:0|t Request Barber Services', 2, 1),
(2, 9, '|TInterface/icons/Spell_ChargePositive:30:30:-18:0|t 2x experience (default)', 0, 1),
(3, 9, '|TInterface/icons/Spell_Holy_BorrowedTime:30:30:-18:0|t 1x experience', 0, 1),
(4, 9, '|TInterface/icons/Spell_ChargeNegative:30:30:-18:0|t Disable experience gains', 0, 1),
-- The C++ menu builder hides this row when XPWeekend.IsJoyousJourneysActive is false,
-- and swaps the label to "Enable Joyous Journeys" when the player has opted out.
(5, 9, '|TInterface/icons/INV_Misc_PocketWatch_01:30:30:-18:0|t Disable Joyous Journeys', 0, 1);

DELETE FROM `smartstone_costumes` WHERE `Id` IN (20001, 20002);
INSERT INTO `smartstone_costumes` (`Id`, `DisplayId`, `Category`, `SubscriptionLevel`, `Duration`, `Description`, `Enabled`) VALUES
(20001, 2029,5,1,0,'Edwin VanCleef',1),
(20002, 2043,5,1,0,'High Inquisitor Whitemane', 1);

UPDATE `item_template` SET `name` = "Smartstone", `ScriptName` = 'item_chromiecraft_smartstone', stackable = 1, `spellid_1` = 36177, maxcount = 1 WHERE (entry = 32547);
UPDATE `item_template_locale` SET `Name` = "Smartstone" WHERE (ID = 32547);

DELETE FROM `creature_template` WHERE `entry` = 80000;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `name`, `subname`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `rank`, `dmgschool`, `DamageModifier`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
(80000, 0, 0, 0, 0, 0, 'Ripsy Blastfuse', 'Azeroth\'s Finest Hair Stylist', 92001, 70, 70, 0, 35, 1, 1, 1.14286, 0, 0, 1, 2000, 2000, 1, 1, 1, 33536, 2048, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 1.35, 1, 1, 0, 0, 1, 2, '', 0);

DELETE FROM `creature_template_model` WHERE `CreatureID` = 80000;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(80000, 0, 18969, 1, 1, 0);

DELETE FROM `creature_text` WHERE `CreatureID` = 80000;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Comment`) VALUES
(80000, 0, 0, 'Looking for a change, ay? I got just what you need! Come, take a seat.', 12, 0, 'Smartstone - Barber - Spawn'),
(80000, 1, 0, 'Time is up, pal! Off we go!', 12, 0, 'Smartstone - Barber - Despawn');

DELETE FROM `npc_text` WHERE `ID` IN (92000, 92001, 92002, 92003, 92004, 92005, 92006, 92007, 92008, 92009, 92010, 92011, 92012, 92013, 92014, 92015, 92016, 92017, 92018, 92019, 92020);
INSERT INTO `npc_text` (`ID`, `text0_0`) VALUES
(92000, 'In the heart of Azeroth\'s magic, there exists a legendary artifact known as the Smartstone. This ancient gem, brimming with mystical energy and boundless wisdom, serves as a guide and companion to the brave and the curious. With a shimmer of its enchanted facets, the Smartstone can reveal hidden treasures, dispense sage advice, or provide crucial insights into the arcane and mundane alike.'),
(92001, 'Hey there, pal! Welcome to the finest chop shop in Azeroth! Need a trim, a shave, or a whole new look? We’ve got styles to make your enemies quake in fear—or at least wonder where you got such a snazzy do! Sit down, relax, and let’s get you looking spectacular!'),
(92002, 'Scattered across Azeroth and beyond, these elusive creatures embody the essence of their environments. From spectral wolves prowling shadowed forests to fiery phoenixes soaring above molten peaks, each rare pet is a testament to the untamed beauty and danger of the world. Adventurers who seek them must be cunning, patient, and attuned to the whispers of the wild, for these beasts appear only to those who prove themselves worthy.'),
(92003, 'Whispered through the ages, these rare companions hail from Azeroth’s most elusive corners—be it deep jungles, ancient ruins, or shadowed realms. Each carries a spark of their origin’s mystery, offering unique abilities to amuse and amaze. But beware, their bond with mortals is fleeting, and their return to legend is inevitable. Claim them while their tale lingers!'),
(92004, 'Hey there, chum! Winter Veil enthusiast and purveyor of holiday cheer—Goblin-style at your service, if ya catch my drift. Ya look like someone who could use a little festive mischief. What d’ya say? Care to join ol’ Snizzlefizz for some presents, punch, and maybe a harmless boom or two? Ho, ho... ha-HA!"'),
(92005, 'Bound within this stone lies the magic of transformation, a power to reshape one’s form into that of legends and beasts alike. With but a touch, the enchantment takes hold, weaving illusion and essence together, granting the guise of heroes, monsters, and all manner of fabled beings. Whether to inspire awe or sow fear, the choice is yours — the veil of reality is but a whisper away'),
(92006, 'The line between myth and mortal blurs. This relic bends that line, allowing you to wear the shape of Azeroth’s darkest names. Appearances deceive — let yours terrify.'),
(92007, 'Harness the power of the Smartstone to call forth mighty vessels of magic and steel! From draconic guardians to enchanted constructs, these are no mere mounts—they are extensions of your will. Choose your vehicle and command the skies, the seas, or the very earth itself.'),
(92008, 'Bound here are the small graces of your calling — gestures, guises, and habits passed from teacher to student, shaped by every champion who wore your colours before you. They confer no greater might, but they remember. Choose one to carry forward, and let those who behold you wonder which legend stands among them.\n\n(Cosmetic only — they grant no advantage in combat.)'),
(92009, 'For those who answer the call of steel: a stance, a bellow, a stride that shakes the line. These are not blows that strike harder, but the bearing of veterans — every champion who held the front before you stood this way, and now so do you.'),
(92010, 'For those bound to the Light: a gleam at the edge of armour, a verse hummed beneath the breath, the posture of unyielding watch. These are not blessings of greater power, but the bearing of the sworn — and the Light remembers its own.'),
(92011, 'For those who keep company with the wild: a tracker''s eye, a hawk''s stillness, the easy stance of one at home beneath the open sky. These are not sharper arrows, but the bearing of every wanderer who walked your trails before you.'),
(92012, 'For those who move in the gaps between heartbeats: a quieter step, a smile half-hidden, the lean of one who has slipped a hundred guards. These are not deadlier blades, but the bearing of every shadow who walked unseen before you.'),
(92013, 'For those who tend the spirit: a soft cadence, a steady hand, the calm carriage of one who has heard the unspoken. These are not greater miracles, but the bearing of every confessor and comforter who knelt before you.'),
(92014, 'For those bound by oath broken and remade: the cold of the grave clinging to your breath, the hush that trails your step, the bearing of one already past death''s door. These are not sharper runes, but the silence the Scourge could not take from you.'),
(92015, 'For those who speak with the elements: a flicker at the heel, a hush of wind at the shoulder, the easy weight of one whose ancestors stand near. These are not stronger callings, but the bearing of every speaker who listened well before you.'),
(92016, 'For those who reshape the world by study: a glimmer at the fingertip, a robe that never quite settles, the poise of one who has unbent a law of nature. These are not greater spells, but the bearing of every adept who turned the first page before you.'),
(92017, 'For those who bargain with the abyss: a coal-light behind the eye, a shadow that lingers a moment too long, the calm of one who has read a contract few would sign. These are not crueller fels, but the bearing of every pact-bearer who returned before you.'),
(92018, 'For those who walk the wild ways: dirt at the heel, leaf in the hair, the easy carriage of one who has worn many shapes. These are not stronger forms, but the bearing of every changeling who answered the wild''s call before you.'),
(92019, 'Choose how quickly you wish to advance, and return here at any time to change your mind.\n\nPrefer a finer-grained pace? You can set any custom rate up to the server-allowed maximum with the |cff4CFF00.weekendxp rate|r command.\n '),
(92020, 'Here lie the threads of self — the cut of your hair, the pace of your becoming, the small marks of your calling. Tend to them as you wish; the stone holds no judgement on what shape you take, only the means to take it.\n ');

--
-- Localized npc_text. Locales match the set used by module_string_locale
-- (deDE, frFR, esES, esMX, ruRU, koKR, zhCN, zhTW). Missing locales fall
-- back to the enUS row above. Translations are best-effort; native-speaker
-- review recommended for the florid class-perk register (92008-92018).
--
DELETE FROM `npc_text_locale` WHERE `ID` BETWEEN 92000 AND 92020;
INSERT INTO `npc_text_locale` (`ID`, `Locale`, `Text0_0`) VALUES

-- deDE
(92000, 'deDE', 'Im Herzen der Magie Azeroths existiert ein legendäres Artefakt, bekannt als der Smartstone. Dieser uralte Edelstein, erfüllt von mystischer Energie und grenzenloser Weisheit, dient den Mutigen und den Neugierigen als Führer und Begleiter. Mit einem Schimmer seiner verzauberten Facetten kann der Smartstone verborgene Schätze offenbaren, weise Ratschläge erteilen oder entscheidende Einsichten in Arkanes wie Alltägliches gewähren.'),
(92001, 'deDE', 'Hallo, Freund! Willkommen im feinsten Friseursalon Azeroths! Brauchst du einen Trim, eine Rasur oder ein ganz neues Aussehen? Wir haben Frisuren, die deine Feinde erzittern lassen — oder sie zumindest fragen, woher du diesen schicken Schnitt hast! Setz dich, entspann dich, und lass uns dich spektakulär aussehen lassen!'),
(92002, 'deDE', 'Verstreut über Azeroth und darüber hinaus verkörpern diese schwer fassbaren Kreaturen die Essenz ihrer Umgebungen. Von gespenstischen Wölfen, die durch schattige Wälder streifen, bis hin zu feurigen Phönixen, die über geschmolzenen Gipfeln gleiten — jedes seltene Tier ist ein Zeugnis der ungezähmten Schönheit und Gefahr dieser Welt. Wer sie sucht, muss listig, geduldig und auf das Flüstern der Wildnis eingestimmt sein, denn diese Wesen erscheinen nur jenen, die sich ihrer würdig erweisen.'),
(92003, 'deDE', 'Durch die Zeitalter geflüstert, stammen diese seltenen Gefährten aus Azeroths verborgensten Winkeln — seien es tiefe Dschungel, uralte Ruinen oder verschattete Reiche. Jeder trägt einen Funken vom Geheimnis seines Ursprungs in sich und bietet einzigartige Fähigkeiten, die erheitern und erstaunen. Doch hüte dich: ihre Bindung an Sterbliche ist flüchtig, und ihre Rückkehr in die Legende unausweichlich. Beanspruche sie, solange ihre Geschichte noch währt!'),
(92004, 'deDE', 'He, Kumpel! Winterhauch-Enthusiast und Verkäufer festlicher Freude — auf Goblin-Art zu Diensten, wenn du verstehst, was ich meine. Du siehst aus wie jemand, der ein bisschen festlichen Schabernack vertragen könnte. Was sagst du? Lust, dich dem alten Snizzlefizz für Geschenke, Punsch und vielleicht den einen oder anderen harmlosen Knall anzuschließen? Ho, ho... ha-HA!'),
(92005, 'deDE', 'Gebunden in diesem Stein liegt die Magie der Verwandlung — eine Macht, die eigene Gestalt in jene von Legenden und Bestien zu formen. Mit einer Berührung greift der Zauber, verwebt Illusion und Wesen, gewährt das Antlitz von Helden, Ungeheuern und allerlei sagenhaften Wesen. Ob Ehrfurcht erwecken oder Furcht säen — die Wahl ist dein. Der Schleier der Wirklichkeit ist nur ein Hauch entfernt.'),
(92006, 'deDE', 'Die Grenze zwischen Mythos und Sterblichem verschwimmt. Dieses Relikt verbiegt sie und erlaubt dir, die Gestalt von Azeroths finstersten Namen zu tragen. Erscheinungen trügen — lass deine erschrecken.'),
(92007, 'deDE', 'Nutze die Kraft des Smartstones, um mächtige Gefährte aus Magie und Stahl herbeizurufen! Von drakonischen Wächtern bis zu verzauberten Konstrukten — dies sind keine bloßen Reittiere, sondern Verlängerungen deines Willens. Wähle dein Gefährt und gebiete über Himmel, Meere oder die Erde selbst.'),
(92008, 'deDE', 'Hier liegen die kleinen Gaben deiner Berufung — Gesten, Hüllen und Gewohnheiten, von Meister zu Schüler weitergegeben, geformt von jedem Champion, der deine Farben vor dir trug. Sie verleihen keine größere Macht, doch sie erinnern. Wähle eine, um sie weiterzutragen, und lass jene, die dich erblicken, sich fragen, welche Legende unter ihnen weilt.\n\n(Nur kosmetisch — sie gewähren keinen Kampfvorteil.)'),
(92009, 'deDE', 'Für jene, die dem Ruf des Stahls folgen: eine Haltung, ein Brüllen, ein Schritt, der die Front erschüttert. Dies sind keine härteren Schläge, sondern die Art der Veteranen — jeder Champion, der vor dir an der Front stand, stand so, und nun stehst auch du so.'),
(92010, 'deDE', 'Für jene, die dem Licht verbunden sind: ein Glanz am Rand der Rüstung, ein Vers, unter dem Atem gesummt, die Haltung unerschütterlicher Wacht. Dies sind keine größeren Segnungen, sondern die Art der Geweihten — und das Licht erinnert sich an die Seinen.'),
(92011, 'deDE', 'Für jene, die mit der Wildnis ziehen: das Auge eines Spurensuchers, die Stille eines Falken, die gelassene Haltung dessen, der unter offenem Himmel zu Hause ist. Dies sind keine schärferen Pfeile, sondern die Art jedes Wanderers, der vor dir deine Pfade beschritt.'),
(92012, 'deDE', 'Für jene, die sich in den Lücken zwischen Herzschlägen bewegen: ein leiserer Schritt, ein halb verborgenes Lächeln, die Neigung dessen, der hundert Wachen entglitten ist. Dies sind keine tödlicheren Klingen, sondern die Art jedes Schattens, der ungesehen vor dir wandelte.'),
(92013, 'deDE', 'Für jene, die den Geist hüten: eine sanfte Stimme, eine ruhige Hand, die stille Haltung dessen, der das Unausgesprochene gehört hat. Dies sind keine größeren Wunder, sondern die Art jedes Beichtvaters und Trösters, der vor dir kniete.'),
(92014, 'deDE', 'Für jene, gebunden durch zerbrochenen und neu geschmiedeten Eid: die Kälte des Grabes am Atem, das Schweigen, das deinen Schritt verfolgt, die Haltung dessen, der bereits jenseits der Pforte des Todes wandelt. Dies sind keine schärferen Runen, sondern die Stille, die die Geißel dir nicht nehmen konnte.'),
(92015, 'deDE', 'Für jene, die mit den Elementen sprechen: ein Flackern an der Ferse, ein Hauch von Wind an der Schulter, das gelassene Gewicht dessen, dessen Ahnen nahe stehen. Dies sind keine stärkeren Anrufungen, sondern die Art jedes Sprechers, der vor dir wohl zuhörte.'),
(92016, 'deDE', 'Für jene, die die Welt durch Studium neu formen: ein Schimmer an der Fingerspitze, eine Robe, die nie ganz ruht, die Haltung dessen, der ein Naturgesetz gebeugt hat. Dies sind keine größeren Zauber, sondern die Art jedes Adepten, der vor dir die erste Seite umblätterte.'),
(92017, 'deDE', 'Für jene, die mit dem Abgrund verhandeln: ein Glutlicht hinter dem Auge, ein Schatten, der einen Moment zu lang verweilt, die Ruhe dessen, der einen Vertrag gelesen hat, den wenige unterzeichnen würden. Dies sind keine grausameren Fels, sondern die Art jedes Pakt-Trägers, der vor dir zurückkehrte.'),
(92018, 'deDE', 'Für jene, die die wilden Pfade gehen: Erde an der Ferse, ein Blatt im Haar, die leichte Haltung dessen, der viele Gestalten getragen hat. Dies sind keine stärkeren Formen, sondern die Art jedes Wandlers, der vor dir dem Ruf der Wildnis antwortete.'),
(92019, 'deDE', 'Wähle, wie rasch du voranschreiten möchtest, und kehre jederzeit hierher zurück, um deine Wahl zu ändern.\n\nBevorzugst du ein feineres Tempo? Mit dem Befehl |cff4CFF00.weekendxp rate|r kannst du jede beliebige Rate bis zum vom Server erlaubten Maximum einstellen.\n '),
(92020, 'deDE', 'Hier liegen die Fäden des Selbst — der Schnitt deines Haares, das Tempo deines Werdens, die kleinen Zeichen deiner Berufung. Pflege sie, wie es dir beliebt; der Stein urteilt nicht über die Gestalt, die du annimmst, sondern bietet nur die Mittel, sie anzunehmen.\n '),

-- frFR
(92000, 'frFR', 'Au cœur de la magie d''Azeroth, il existe un artefact légendaire connu sous le nom de Smartstone. Cette pierre antique, imprégnée d''énergie mystique et de sagesse infinie, sert de guide et de compagnon aux braves et aux curieux. D''un scintillement de ses facettes enchantées, le Smartstone peut révéler des trésors cachés, dispenser de sages conseils ou offrir des aperçus essentiels aussi bien sur l''arcane que sur le quotidien.'),
(92001, 'frFR', 'Salut l''ami ! Bienvenue dans le meilleur salon de coiffure d''Azeroth ! Besoin d''une coupe, d''un rasage ou d''un look entièrement neuf ? On a des styles qui feront trembler vos ennemis — ou au moins se demander où vous avez trouvé une coupe si classe ! Asseyez-vous, détendez-vous, et préparons-vous à vous rendre spectaculaire !'),
(92002, 'frFR', 'Dispersées à travers Azeroth et au-delà, ces créatures insaisissables incarnent l''essence de leur environnement. Des loups spectraux rôdant dans les forêts d''ombre aux phénix flamboyants planant au-dessus des cimes en fusion, chaque familier rare témoigne de la beauté sauvage et du danger du monde. Les aventuriers qui les cherchent doivent être rusés, patients et à l''écoute des murmures de la nature, car ces bêtes n''apparaissent qu''à ceux qui se sont montrés dignes.'),
(92003, 'frFR', 'Murmurés à travers les âges, ces rares compagnons proviennent des recoins les plus secrets d''Azeroth — jungles profondes, ruines anciennes ou royaumes ombragés. Chacun porte une étincelle du mystère de son origine et offre des capacités uniques pour amuser et émerveiller. Mais prenez garde, leur lien avec les mortels est fugace, et leur retour à la légende inévitable. Réclamez-les tant que leur récit perdure !'),
(92004, 'frFR', 'Salut, mon pote ! Passionné du Voile d''hiver et fournisseur d''ambiance festive — à la mode gobeline, si tu vois ce que je veux dire. T''as l''air de quelqu''un qui aurait besoin d''un peu de joyeux désordre. Qu''est-ce que t''en dis ? Ça te dirait de rejoindre ce bon vieux Snizzlefizz pour des cadeaux, du punch et peut-être un ou deux pétards inoffensifs ? Ho, ho... ha-HA !'),
(92005, 'frFR', 'Liée à l''intérieur de cette pierre se trouve la magie de la transformation, un pouvoir capable de reforger sa forme en celles de légendes et de bêtes. D''un simple contact, l''enchantement opère, entrelaçant illusion et essence, accordant l''apparence de héros, de monstres et de toutes sortes de créatures fabuleuses. Inspirer l''admiration ou semer la peur — le choix vous appartient. Le voile de la réalité n''est plus qu''un murmure.'),
(92006, 'frFR', 'La frontière entre le mythe et le mortel s''estompe. Cette relique infléchit cette frontière et vous permet de revêtir la forme des noms les plus sombres d''Azeroth. Les apparences trompent — que la vôtre terrifie.'),
(92007, 'frFR', 'Maîtrisez le pouvoir du Smartstone pour invoquer de puissants véhicules de magie et d''acier ! Des gardiens draconiques aux constructions enchantées, ce ne sont pas de simples montures — ce sont des extensions de votre volonté. Choisissez votre véhicule et commandez les cieux, les mers ou la terre elle-même.'),
(92008, 'frFR', 'Ici se trouvent les petites grâces de votre vocation — gestes, parures et habitudes transmis de maître à élève, façonnés par chaque champion qui a porté vos couleurs avant vous. Ils ne confèrent aucun pouvoir supplémentaire, mais ils se souviennent. Choisissez-en un à perpétuer, et laissez ceux qui vous regardent se demander quelle légende se tient parmi eux.\n\n(Cosmétique uniquement — sans avantage en combat.)'),
(92009, 'frFR', 'Pour ceux qui répondent à l''appel de l''acier : une posture, un rugissement, une foulée qui ébranle la ligne. Ce ne sont pas des coups plus durs, mais le port des vétérans — chaque champion qui a tenu le front avant vous se tenait ainsi, et désormais vous aussi.'),
(92010, 'frFR', 'Pour ceux liés à la Lumière : un éclat au bord de l''armure, un verset murmuré sous le souffle, la posture d''une vigilance inébranlable. Ce ne sont pas de plus grandes bénédictions, mais le port des consacrés — et la Lumière se souvient des siens.'),
(92011, 'frFR', 'Pour ceux qui fraternisent avec la nature : l''œil du traqueur, l''immobilité du faucon, la posture aisée de qui se sent chez soi sous le ciel ouvert. Ce ne sont pas des flèches plus acérées, mais le port de chaque vagabond qui a foulé vos sentiers avant vous.'),
(92012, 'frFR', 'Pour ceux qui se glissent dans l''espace entre deux battements de cœur : un pas plus discret, un sourire à demi caché, l''inclinaison de qui a déjoué cent gardes. Ce ne sont pas des lames plus mortelles, mais le port de chaque ombre qui a marché invisible avant vous.'),
(92013, 'frFR', 'Pour ceux qui veillent sur l''esprit : une cadence douce, une main ferme, le maintien calme de qui a entendu l''inexprimé. Ce ne sont pas de plus grands miracles, mais le port de chaque confesseur et consolateur qui s''est agenouillé avant vous.'),
(92014, 'frFR', 'Pour ceux liés par un serment brisé et reforgé : le froid de la tombe au souffle, le silence qui suit votre pas, le port de qui a déjà passé la porte de la mort. Ce ne sont pas des runes plus tranchantes, mais le silence que le Fléau n''a pu vous arracher.'),
(92015, 'frFR', 'Pour ceux qui parlent aux éléments : un éclat au talon, un souffle de vent à l''épaule, le poids tranquille de qui sent ses ancêtres tout près. Ce ne sont pas des invocations plus puissantes, mais le port de chaque orateur qui a su écouter avant vous.'),
(92016, 'frFR', 'Pour ceux qui reforgent le monde par l''étude : un scintillement au bout des doigts, une robe qui ne se pose jamais tout à fait, la prestance de qui a infléchi une loi de la nature. Ce ne sont pas de plus grands sorts, mais le port de chaque adepte qui a tourné la première page avant vous.'),
(92017, 'frFR', 'Pour ceux qui marchandent avec l''abîme : une lueur de braise au fond de l''œil, une ombre qui s''attarde un peu trop, le calme de qui a lu un contrat que peu signeraient. Ce ne sont pas des feux gangrenés plus cruels, mais le port de chaque porteur de pacte qui est revenu avant vous.'),
(92018, 'frFR', 'Pour ceux qui marchent les voies sauvages : terre au talon, feuille dans les cheveux, le port aisé de qui a revêtu mille formes. Ce ne sont pas des formes plus puissantes, mais le port de chaque changeant qui a répondu à l''appel sauvage avant vous.'),
(92019, 'frFR', 'Choisissez à quelle vitesse vous souhaitez progresser, et revenez ici à tout moment pour changer d''avis.\n\nVous préférez un réglage plus fin ? Vous pouvez définir n''importe quel taux personnalisé jusqu''au maximum autorisé par le serveur avec la commande |cff4CFF00.weekendxp rate|r.\n '),
(92020, 'frFR', 'Ici reposent les fils de soi — la coupe de vos cheveux, le rythme de votre devenir, les petites marques de votre vocation. Soignez-les à votre guise ; la pierre ne juge pas la forme que vous prenez, elle n''offre que les moyens de la prendre.\n '),

-- esES
(92000, 'esES', 'En el corazón de la magia de Azeroth existe un artefacto legendario conocido como la Smartstone. Esta gema antigua, rebosante de energía mística y de sabiduría sin límites, sirve de guía y compañera a los valientes y los curiosos. Con un destello de sus facetas encantadas, la Smartstone puede revelar tesoros ocultos, dispensar sabios consejos o brindar perspectivas cruciales tanto sobre lo arcano como sobre lo mundano.'),
(92001, 'esES', '¡Hola, amigo! ¡Bienvenido a la mejor barbería de Azeroth! ¿Necesitas un corte, un afeitado o todo un cambio de imagen? ¡Tenemos estilos que harán temblar a tus enemigos — o al menos preguntarse de dónde sacaste semejante peinado! Siéntate, relájate y vamos a dejarte espectacular.'),
(92002, 'esES', 'Esparcidas por Azeroth y más allá, estas criaturas esquivas encarnan la esencia de sus entornos. Desde lobos espectrales que merodean por bosques umbríos hasta fénix ardientes que sobrevuelan cumbres fundidas, cada mascota rara es un testimonio de la belleza salvaje y el peligro del mundo. Quien las busque deberá ser astuto, paciente y atento al susurro de la naturaleza, pues estas bestias solo aparecen ante quienes demuestran ser dignos.'),
(92003, 'esES', 'Susurrados a través de las eras, estos raros compañeros provienen de los rincones más esquivos de Azeroth — selvas profundas, ruinas antiguas o reinos sombríos. Cada uno lleva una chispa del misterio de su origen y ofrece habilidades únicas para divertir y maravillar. Pero ten cuidado: su vínculo con los mortales es fugaz, y su retorno a la leyenda, inevitable. ¡Reclámalos mientras dure su relato!'),
(92004, 'esES', '¡Hola, compinche! Entusiasta del Velo de Invierno y proveedor de buen ánimo festivo — al estilo goblin, si captas la idea. Tienes pinta de necesitar un poco de travesura festiva. ¿Qué dices? ¿Te apetece unirte al viejo Snizzlefizz para algunos regalos, un poco de ponche y quizá una explosión inofensiva o dos? ¡Jo, jo... ja-JA!'),
(92005, 'esES', 'Atada a esta piedra yace la magia de la transformación, un poder para remodelar la propia forma en la de leyendas y bestias por igual. Con un solo toque, el encantamiento se afianza, entretejiendo ilusión y esencia, otorgando la apariencia de héroes, monstruos y toda clase de seres fabulosos. Inspirar admiración o sembrar miedo — la elección es tuya. El velo de la realidad no es más que un susurro.'),
(92006, 'esES', 'La línea entre el mito y el mortal se desdibuja. Esta reliquia tuerce esa línea y te permite vestir la forma de los nombres más oscuros de Azeroth. Las apariencias engañan — que la tuya aterrorice.'),
(92007, 'esES', '¡Aprovecha el poder de la Smartstone para invocar poderosos vehículos de magia y acero! Desde guardianes dracónicos hasta constructos encantados, no son meras monturas: son extensiones de tu voluntad. Elige tu vehículo y domina los cielos, los mares o la tierra misma.'),
(92008, 'esES', 'Aquí reposan las pequeñas gracias de tu vocación — gestos, atavíos y costumbres transmitidos de maestro a discípulo, moldeados por cada campeón que llevó tus colores antes que tú. No otorgan mayor poder, pero recuerdan. Elige una para llevarla adelante, y deja que quienes te contemplen se pregunten qué leyenda camina entre ellos.\n\n(Sólo cosmético — no otorgan ventaja en combate.)'),
(92009, 'esES', 'Para quienes responden al llamado del acero: una postura, un bramido, un paso que sacude la línea. No son golpes más duros, sino el porte de los veteranos — todo campeón que sostuvo el frente antes que tú estuvo así, y ahora tú también.'),
(92010, 'esES', 'Para quienes están ligados a la Luz: un destello al borde de la armadura, un verso murmurado bajo el aliento, la postura de la guardia inquebrantable. No son bendiciones de mayor poder, sino el porte de los consagrados — y la Luz recuerda a los suyos.'),
(92011, 'esES', 'Para quienes andan en compañía de lo salvaje: ojo de rastreador, quietud de halcón, la postura serena de quien está en casa bajo el cielo abierto. No son flechas más afiladas, sino el porte de cada errante que recorrió tus sendas antes que tú.'),
(92012, 'esES', 'Para quienes se mueven en los huecos entre latidos: un paso más silencioso, una sonrisa a medias, la inclinación de quien ha burlado a cien guardias. No son hojas más mortales, sino el porte de cada sombra que caminó invisible antes que tú.'),
(92013, 'esES', 'Para quienes cuidan del espíritu: una cadencia suave, una mano firme, el porte sereno de quien ha oído lo no dicho. No son milagros mayores, sino el porte de cada confesor y consolador que se arrodilló antes que tú.'),
(92014, 'esES', 'Para quienes están atados por juramento roto y rehecho: el frío de la tumba aferrado al aliento, el silencio que sigue a tu paso, el porte de quien ya pasó la puerta de la muerte. No son runas más afiladas, sino el silencio que la Plaga no pudo arrebatarte.'),
(92015, 'esES', 'Para quienes hablan con los elementos: un parpadeo en el talón, un susurro de viento en el hombro, el peso sereno de quien siente cerca a sus ancestros. No son llamados más fuertes, sino el porte de cada orador que escuchó bien antes que tú.'),
(92016, 'esES', 'Para quienes remodelan el mundo por el estudio: un fulgor en la yema del dedo, una túnica que nunca acaba de asentarse, el porte de quien ha doblegado una ley de la naturaleza. No son hechizos mayores, sino el porte de cada adepto que pasó la primera página antes que tú.'),
(92017, 'esES', 'Para quienes negocian con el abismo: una luz de brasa tras la mirada, una sombra que se demora un instante de más, la calma de quien ha leído un contrato que pocos firmarían. No son vilfuegos más crueles, sino el porte de cada portador de pacto que regresó antes que tú.'),
(92018, 'esES', 'Para quienes caminan las vías salvajes: tierra en el talón, hoja en el cabello, el porte sereno de quien ha vestido muchas formas. No son formas más fuertes, sino el porte de cada cambiante que respondió al llamado salvaje antes que tú.'),
(92019, 'esES', 'Elige a qué velocidad deseas avanzar y vuelve aquí en cualquier momento para cambiar de opinión.\n\n¿Prefieres un ritmo más afinado? Puedes establecer cualquier tasa personalizada hasta el máximo permitido por el servidor con el comando |cff4CFF00.weekendxp rate|r.\n '),
(92020, 'esES', 'Aquí reposan los hilos del yo — el corte de tu cabello, el ritmo de tu llegar a ser, las pequeñas marcas de tu vocación. Cuídalos como gustes; la piedra no juzga la forma que tomes, sólo ofrece los medios para tomarla.\n '),

-- esMX (identical to esES)
(92000, 'esMX', 'En el corazón de la magia de Azeroth existe un artefacto legendario conocido como la Smartstone. Esta gema antigua, rebosante de energía mística y de sabiduría sin límites, sirve de guía y compañera a los valientes y los curiosos. Con un destello de sus facetas encantadas, la Smartstone puede revelar tesoros ocultos, dispensar sabios consejos o brindar perspectivas cruciales tanto sobre lo arcano como sobre lo mundano.'),
(92001, 'esMX', '¡Hola, amigo! ¡Bienvenido a la mejor barbería de Azeroth! ¿Necesitas un corte, un afeitado o todo un cambio de imagen? ¡Tenemos estilos que harán temblar a tus enemigos — o al menos preguntarse de dónde sacaste semejante peinado! Siéntate, relájate y vamos a dejarte espectacular.'),
(92002, 'esMX', 'Esparcidas por Azeroth y más allá, estas criaturas esquivas encarnan la esencia de sus entornos. Desde lobos espectrales que merodean por bosques umbríos hasta fénix ardientes que sobrevuelan cumbres fundidas, cada mascota rara es un testimonio de la belleza salvaje y el peligro del mundo. Quien las busque deberá ser astuto, paciente y atento al susurro de la naturaleza, pues estas bestias solo aparecen ante quienes demuestran ser dignos.'),
(92003, 'esMX', 'Susurrados a través de las eras, estos raros compañeros provienen de los rincones más esquivos de Azeroth — selvas profundas, ruinas antiguas o reinos sombríos. Cada uno lleva una chispa del misterio de su origen y ofrece habilidades únicas para divertir y maravillar. Pero ten cuidado: su vínculo con los mortales es fugaz, y su retorno a la leyenda, inevitable. ¡Reclámalos mientras dure su relato!'),
(92004, 'esMX', '¡Hola, compinche! Entusiasta del Velo de Invierno y proveedor de buen ánimo festivo — al estilo goblin, si captas la idea. Tienes pinta de necesitar un poco de travesura festiva. ¿Qué dices? ¿Te apetece unirte al viejo Snizzlefizz para algunos regalos, un poco de ponche y quizá una explosión inofensiva o dos? ¡Jo, jo... ja-JA!'),
(92005, 'esMX', 'Atada a esta piedra yace la magia de la transformación, un poder para remodelar la propia forma en la de leyendas y bestias por igual. Con un solo toque, el encantamiento se afianza, entretejiendo ilusión y esencia, otorgando la apariencia de héroes, monstruos y toda clase de seres fabulosos. Inspirar admiración o sembrar miedo — la elección es tuya. El velo de la realidad no es más que un susurro.'),
(92006, 'esMX', 'La línea entre el mito y el mortal se desdibuja. Esta reliquia tuerce esa línea y te permite vestir la forma de los nombres más oscuros de Azeroth. Las apariencias engañan — que la tuya aterrorice.'),
(92007, 'esMX', '¡Aprovecha el poder de la Smartstone para invocar poderosos vehículos de magia y acero! Desde guardianes dracónicos hasta constructos encantados, no son meras monturas: son extensiones de tu voluntad. Elige tu vehículo y domina los cielos, los mares o la tierra misma.'),
(92008, 'esMX', 'Aquí reposan las pequeñas gracias de tu vocación — gestos, atavíos y costumbres transmitidos de maestro a discípulo, moldeados por cada campeón que llevó tus colores antes que tú. No otorgan mayor poder, pero recuerdan. Elige una para llevarla adelante, y deja que quienes te contemplen se pregunten qué leyenda camina entre ellos.\n\n(Sólo cosmético — no otorgan ventaja en combate.)'),
(92009, 'esMX', 'Para quienes responden al llamado del acero: una postura, un bramido, un paso que sacude la línea. No son golpes más duros, sino el porte de los veteranos — todo campeón que sostuvo el frente antes que tú estuvo así, y ahora tú también.'),
(92010, 'esMX', 'Para quienes están ligados a la Luz: un destello al borde de la armadura, un verso murmurado bajo el aliento, la postura de la guardia inquebrantable. No son bendiciones de mayor poder, sino el porte de los consagrados — y la Luz recuerda a los suyos.'),
(92011, 'esMX', 'Para quienes andan en compañía de lo salvaje: ojo de rastreador, quietud de halcón, la postura serena de quien está en casa bajo el cielo abierto. No son flechas más afiladas, sino el porte de cada errante que recorrió tus sendas antes que tú.'),
(92012, 'esMX', 'Para quienes se mueven en los huecos entre latidos: un paso más silencioso, una sonrisa a medias, la inclinación de quien ha burlado a cien guardias. No son hojas más mortales, sino el porte de cada sombra que caminó invisible antes que tú.'),
(92013, 'esMX', 'Para quienes cuidan del espíritu: una cadencia suave, una mano firme, el porte sereno de quien ha oído lo no dicho. No son milagros mayores, sino el porte de cada confesor y consolador que se arrodilló antes que tú.'),
(92014, 'esMX', 'Para quienes están atados por juramento roto y rehecho: el frío de la tumba aferrado al aliento, el silencio que sigue a tu paso, el porte de quien ya pasó la puerta de la muerte. No son runas más afiladas, sino el silencio que la Plaga no pudo arrebatarte.'),
(92015, 'esMX', 'Para quienes hablan con los elementos: un parpadeo en el talón, un susurro de viento en el hombro, el peso sereno de quien siente cerca a sus ancestros. No son llamados más fuertes, sino el porte de cada orador que escuchó bien antes que tú.'),
(92016, 'esMX', 'Para quienes remodelan el mundo por el estudio: un fulgor en la yema del dedo, una túnica que nunca acaba de asentarse, el porte de quien ha doblegado una ley de la naturaleza. No son hechizos mayores, sino el porte de cada adepto que pasó la primera página antes que tú.'),
(92017, 'esMX', 'Para quienes negocian con el abismo: una luz de brasa tras la mirada, una sombra que se demora un instante de más, la calma de quien ha leído un contrato que pocos firmarían. No son vilfuegos más crueles, sino el porte de cada portador de pacto que regresó antes que tú.'),
(92018, 'esMX', 'Para quienes caminan las vías salvajes: tierra en el talón, hoja en el cabello, el porte sereno de quien ha vestido muchas formas. No son formas más fuertes, sino el porte de cada cambiante que respondió al llamado salvaje antes que tú.'),
(92019, 'esMX', 'Elige a qué velocidad deseas avanzar y vuelve aquí en cualquier momento para cambiar de opinión.\n\n¿Prefieres un ritmo más afinado? Puedes establecer cualquier tasa personalizada hasta el máximo permitido por el servidor con el comando |cff4CFF00.weekendxp rate|r.\n '),
(92020, 'esMX', 'Aquí reposan los hilos del yo — el corte de tu cabello, el ritmo de tu llegar a ser, las pequeñas marcas de tu vocación. Cuídalos como gustes; la piedra no juzga la forma que tomes, sólo ofrece los medios para tomarla.\n '),

-- ruRU
(92000, 'ruRU', 'В сердце магии Азерота существует легендарный артефакт, известный как Smartstone. Этот древний камень, наполненный мистической энергией и безграничной мудростью, служит проводником и спутником для смелых и любознательных. Мерцанием своих зачарованных граней Smartstone способен открывать сокрытые сокровища, давать мудрые советы и являть важные истины как о тайном, так и о повседневном.'),
(92001, 'ruRU', 'Привет, дружище! Добро пожаловать в лучший салон Азерота! Нужна стрижка, бритьё или совершенно новый облик? У нас есть причёски, от которых враги затрепещут — или, по крайней мере, спросят, где ты раздобыл такой шикарный вид! Садись, расслабься — и сделаем из тебя зрелище.'),
(92002, 'ruRU', 'Разбросанные по Азероту и за его пределами, эти неуловимые существа воплощают суть своих краёв. От призрачных волков, крадущихся по тёмным лесам, до огненных фениксов, парящих над расплавленными вершинами — каждый редкий питомец свидетельствует о дикой красоте и опасности этого мира. Тот, кто их ищет, должен быть хитёр, терпелив и чуток к шёпоту дикой природы, ибо эти звери являются лишь достойным.'),
(92003, 'ruRU', 'Сквозь века передаваемые шёпотом, эти редкие спутники приходят из самых потаённых уголков Азерота — глубоких джунглей, древних руин или сумрачных царств. Каждый несёт в себе искру тайны своего происхождения, даря необычные способности на радость и удивление. Но берегись: их связь со смертными мимолётна, а возвращение в легенду неизбежно. Заяви права на них, пока длится их сказ!'),
(92004, 'ruRU', 'Эй, приятель! Любитель Зимнего Покрова и поставщик праздничного веселья — по-гоблински, если ты понимаешь, о чём я. Похоже, тебе не помешало бы немного праздничного озорства. Что скажешь? Не желаешь присоединиться к старине Снизлфиззу за подарками, пуншем и парой безобидных «бабахов»? Хо, хо... ха-ХА!'),
(92005, 'ruRU', 'Заключённая в этом камне магия превращения — сила, способная изменить твой облик в облик легенд и зверей. Одно прикосновение — и чары вступают в силу, сплетая иллюзию с сутью, даруя обличья героев, чудовищ и всевозможных сказочных созданий. Внушать благоговение или сеять страх — выбор за тобой. Покров реальности отделён лишь шёпотом.'),
(92006, 'ruRU', 'Грань между мифом и смертным стирается. Эта реликвия искажает её и позволяет носить облик самых тёмных имён Азерота. Внешность обманчива — пусть твоя ужасает.'),
(92007, 'ruRU', 'Призови силу Smartstone, чтобы вызвать могучих созданий из магии и стали! От драконьих стражей до зачарованных конструктов — это не просто скакуны, а продолжение твоей воли. Выбирай средство и владычествуй над небом, морем или самой землёй.'),
(92008, 'ruRU', 'Здесь хранятся малые дары твоего призвания — жесты, обличья и привычки, передаваемые от учителя к ученику, выкованные каждым поборником, носившим твои цвета прежде. Они не дают большей мощи, но они помнят. Выбери один, чтобы понести его дальше, и пусть взирающие на тебя гадают, какая легенда стоит среди них.\n\n(Только косметика — никакого преимущества в бою.)'),
(92009, 'ruRU', 'Для тех, кто отвечает зову стали: стойка, рёв, поступь, что сотрясает строй. Это не удары сильнее, но осанка ветеранов — каждый поборник, державший линию до тебя, стоял именно так, и теперь стоишь и ты.'),
(92010, 'ruRU', 'Для тех, кто связан со Светом: блеск на краю доспеха, стих, нашёптанный под нос, осанка непоколебимой стражи. Это не большее благословение, но осанка посвящённых — и Свет помнит своих.'),
(92011, 'ruRU', 'Для тех, кто водится с дикой природой: взгляд следопыта, неподвижность ястреба, лёгкая осанка того, кто чувствует себя дома под открытым небом. Это не более острые стрелы, но осанка каждого странника, прошедшего твоими тропами до тебя.'),
(92012, 'ruRU', 'Для тех, кто движется в промежутках между ударами сердца: шаг тише, улыбка вполовину, наклон того, кто ускользнул от сотни стражей. Это не более смертоносные клинки, но осанка каждой тени, шагавшей незримо до тебя.'),
(92013, 'ruRU', 'Для тех, кто бережёт дух: мягкая речь, твёрдая рука, спокойная осанка того, кто слышал несказанное. Это не большие чудеса, но осанка каждого исповедника и утешителя, преклонявшего колено до тебя.'),
(92014, 'ruRU', 'Для тех, кто связан клятвой сломанной и переделанной: холод могилы в дыхании, тишина за спиной, осанка того, кто уже миновал врата смерти. Это не более острые руны, но та тишина, что Плеть не смогла у тебя отнять.'),
(92015, 'ruRU', 'Для тех, кто говорит со стихиями: мерцание у пятки, шёпот ветра у плеча, спокойная тяжесть того, чьи предки стоят рядом. Это не более сильные призывы, но осанка каждого говорящего, умевшего слушать до тебя.'),
(92016, 'ruRU', 'Для тех, кто перестраивает мир изучением: блик на кончике пальца, мантия, что никак не уляжется, выправка того, кто согнул один из законов природы. Это не более сильные заклинания, но осанка каждого ученика, перелистнувшего первую страницу до тебя.'),
(92017, 'ruRU', 'Для тех, кто торгуется с бездной: уголёк в глубине глаза, тень, что задерживается чуть дольше, спокойствие того, кто прочёл договор, который немногие подпишут. Это не более жестокие скверны, но осанка каждого носителя пакта, вернувшегося до тебя.'),
(92018, 'ruRU', 'Для тех, кто ходит дикими путями: земля на пятке, лист в волосах, лёгкая осанка того, кто носил множество обличий. Это не более сильные формы, но осанка каждого перевёртыша, ответившего на зов дикой природы до тебя.'),
(92019, 'ruRU', 'Выбери, как быстро ты хочешь продвигаться, и возвращайся сюда в любое время, чтобы передумать.\n\nПредпочитаешь более тонкую настройку? Любой произвольный множитель в пределах разрешённого сервером максимума можно задать командой |cff4CFF00.weekendxp rate|r.\n '),
(92020, 'ruRU', 'Здесь сходятся нити самого себя — стрижка волос, темп твоего становления, малые знаки твоего призвания. Заботься о них как пожелаешь; камень не судит, какую форму ты примешь, — он лишь даёт средства её принять.\n '),

-- koKR
(92000, 'koKR', '아제로스 마법의 심장에는 스마트스톤이라 알려진 전설의 유물이 존재합니다. 이 고대의 보석은 신비로운 기운과 끝없는 지혜로 가득 차 있으며, 용감하고 호기심 많은 이들에게 길잡이이자 동반자가 되어 줍니다. 그 마법의 면들이 어른거리면 스마트스톤은 숨겨진 보물을 드러내거나 현명한 조언을 들려주고, 비전과 일상 모두에 대한 결정적인 통찰을 전해 줍니다.'),
(92001, 'koKR', '안녕, 친구! 아제로스 최고의 이발소에 어서 오게! 다듬기, 면도, 아니면 완전히 새로운 모습이 필요한가? 적들이 두려워 떨게 만들 — 적어도 어디서 이런 멋진 스타일을 받았는지 궁금해할 — 그런 스타일들이 우리에겐 있다네! 앉아서 편히 쉬시게, 멋지게 만들어 드리지!'),
(92002, 'koKR', '아제로스 곳곳에 흩어진 이 신비로운 존재들은 각자의 환경의 본질을 구현합니다. 그림자 숲을 떠도는 영혼 늑대부터 녹아내린 봉우리 위를 나는 불꽃 불사조까지, 모든 희귀 펫은 이 세상의 거친 아름다움과 위험의 증거입니다. 그들을 찾는 모험가는 영리하고 인내심이 있으며 자연의 속삭임에 귀를 기울여야 합니다. 이 짐승들은 자격을 증명한 자에게만 모습을 드러내기 때문입니다.'),
(92003, 'koKR', '오랜 세월 속삭임으로 전해진 이 진귀한 동반자들은 아제로스에서 가장 외진 곳들 — 깊은 정글, 고대 폐허, 그림자 어린 영역 — 에서 옵니다. 각자가 기원의 신비의 불씨를 품고 있어 즐거움과 놀라움을 줍니다. 그러나 조심하십시오. 인간과의 인연은 덧없고, 다시 전설로 돌아가는 길은 피할 수 없습니다. 그 이야기가 머무는 동안 그들을 차지하십시오!'),
(92004, 'koKR', '여, 친구! 겨울맞이 축제 애호가이자 명절 분위기 공급자 — 고블린식으로, 알아듣겠지? 자네는 살짝 명절 장난이 필요해 보이는군. 어떤가? 이 늙은 스니즐피즈와 선물, 펀치, 그리고 무해한 폭죽 한두 발을 함께 즐겨보지 않겠나? 호, 호... 하-하!'),
(92005, 'koKR', '이 돌 안에는 변신의 마법이 깃들어 있습니다. 자신의 모습을 전설과 짐승의 모습으로 바꿀 수 있는 힘입니다. 한 번의 손길로 마법이 발동되어 환영과 본질을 엮어 영웅, 괴물, 그리고 온갖 전설적 존재의 형상을 부여합니다. 경외를 자아낼지, 두려움을 심을지는 그대의 선택입니다 — 현실의 장막은 속삭임만큼이나 가까이 있습니다.'),
(92006, 'koKR', '신화와 인간의 경계가 흐려집니다. 이 유물은 그 경계를 휘게 하여, 아제로스의 가장 어두운 이름들의 모습을 입게 합니다. 외양은 속이는 법 — 그대의 외양으로 그들을 떨게 하십시오.'),
(92007, 'koKR', '스마트스톤의 힘을 활용해 마법과 강철로 빚어진 강력한 탈것을 불러내십시오! 용족 수호자부터 마법이 깃든 구조물까지, 이는 단순한 탈것이 아니라 그대 의지의 연장입니다. 자신의 탈것을 골라 하늘, 바다, 혹은 대지 그 자체를 호령하십시오.'),
(92008, 'koKR', '이곳에는 그대 소명의 자그마한 은총이 깃들어 있습니다 — 스승에게서 제자에게로 전해지고, 그대 이전에 그대의 색을 짊어졌던 모든 영웅에 의해 다듬어진 몸짓, 차림새, 습관들. 더 큰 힘을 주지는 않지만, 그것들은 기억합니다. 하나를 택해 이어 나가고, 그대를 바라보는 이들이 그들 사이에 어떤 전설이 서 있는지 궁금해하게 하십시오.\n\n(외형 전용 — 전투에서 어떤 이점도 주지 않습니다.)'),
(92009, 'koKR', '강철의 부름에 답하는 자들을 위해: 자세, 외침, 전열을 흔드는 발걸음. 더 강한 타격이 아니라, 전장에 섰던 모든 영웅들이 보여 온 풍모 — 그대 이전에 전선을 지킨 이들이 그러했고, 이제 그대 또한 그러합니다.'),
(92010, 'koKR', '빛에 묶인 자들을 위해: 갑옷 끝의 빛, 숨결 아래 흥얼거리는 한 구절, 흔들림 없는 경계의 자세. 더 큰 축복이 아니라 서약한 자들의 풍모 — 빛은 자신의 일원을 기억합니다.'),
(92011, 'koKR', '거친 자연과 함께하는 자들을 위해: 추적자의 눈, 매의 고요함, 열린 하늘 아래에서 편안한 이의 자연스러운 자세. 더 날카로운 화살이 아니라, 그대 이전에 같은 길을 걸은 모든 방랑자의 풍모입니다.'),
(92012, 'koKR', '심장 박동 사이의 틈에서 움직이는 자들을 위해: 더 조용한 발걸음, 반쯤 감춘 미소, 수많은 경비를 따돌린 이의 기울임. 더 치명적인 칼날이 아니라, 그대 이전에 보이지 않게 걸어 다닌 모든 그림자의 풍모입니다.'),
(92013, 'koKR', '영혼을 돌보는 자들을 위해: 부드러운 음조, 흔들림 없는 손, 말하지 않은 것을 들어 본 이의 차분한 자세. 더 큰 기적이 아니라, 그대 이전에 무릎 꿇었던 모든 고해자와 위로자의 풍모입니다.'),
(92014, 'koKR', '깨졌다가 다시 새겨진 맹세에 묶인 자들을 위해: 호흡에 매달린 무덤의 한기, 발걸음을 따르는 정적, 이미 죽음의 문턱을 넘은 이의 풍모. 더 날카로운 룬이 아니라, 천벌단조차 그대에게서 빼앗지 못한 침묵입니다.'),
(92015, 'koKR', '원소와 대화하는 자들을 위해: 뒤꿈치에 어른거리는 불꽃, 어깨에 스치는 바람의 숨결, 조상들이 곁에 있는 자의 차분한 무게. 더 강한 부름이 아니라, 그대 이전에 잘 들어 준 모든 화자의 풍모입니다.'),
(92016, 'koKR', '연구로 세상을 다시 빚는 자들을 위해: 손끝의 어른거림, 결코 완전히 가라앉지 않는 로브, 자연의 법칙 하나를 굽혀 본 이의 기품. 더 큰 주문이 아니라, 그대 이전에 첫 장을 넘긴 모든 학도의 풍모입니다.'),
(92017, 'koKR', '심연과 거래하는 자들을 위해: 눈 너머의 잉걸불, 너무 오래 머무는 그림자, 거의 누구도 서명하지 않을 계약을 읽어 본 이의 평정. 더 잔혹한 지옥불이 아니라, 그대 이전에 돌아온 모든 계약자의 풍모입니다.'),
(92018, 'koKR', '거친 길을 걷는 자들을 위해: 뒤꿈치의 흙, 머리카락 속의 잎, 여러 모습을 입어 온 이의 편안한 자세. 더 강한 형상이 아니라, 그대 이전에 야성의 부름에 응답한 모든 변신자의 풍모입니다.'),
(92019, 'koKR', '나아가고 싶은 속도를 선택하고, 마음이 바뀌면 언제든지 이곳으로 다시 돌아오십시오.\n\n더 세밀한 조정을 원하십니까? |cff4CFF00.weekendxp rate|r 명령으로 서버가 허용하는 최대치까지 원하는 비율을 설정할 수 있습니다.\n '),
(92020, 'koKR', '여기에는 자아의 실들이 놓여 있습니다 — 머리카락의 모양, 그대 되어 감의 속도, 소명의 작은 표식들. 원하는 대로 가꾸십시오. 이 돌은 그대가 어떤 모습을 취하든 판단하지 않으며, 다만 그렇게 할 수단만을 줍니다.\n '),

-- zhCN
(92000, 'zhCN', '在艾泽拉斯魔法的核心,存在着一件名为智慧石的传奇神器。这枚古老的宝石充盈着神秘能量与无尽智慧,为勇敢者和好奇者引路、相伴。当它附魔的切面闪烁微光,智慧石能揭示隐秘的宝藏、给予贤明的劝告,亦能在奥秘与寻常之间提供至关重要的洞见。'),
(92001, 'zhCN', '嘿,朋友!欢迎来到艾泽拉斯最棒的理发馆!需要修剪、刮净,还是焕然一新?我们这儿的发型能让你的敌人胆寒——至少他们会想知道你这帅气造型是从哪儿来的!坐下,放松,让我们把你打扮得耀眼夺目!'),
(92002, 'zhCN', '散布于艾泽拉斯及其外缘,这些神出鬼没的生灵承载着各自栖息地的气韵。从在幽暗森林潜行的幽灵之狼,到在熔岩之巅翱翔的火焰凤凰,每一只稀有宠物都是这世界狂野之美与凶险的见证。寻觅它们的冒险者须得机敏、耐心,并能聆听荒野的低语,因为这些灵兽只会出现在被证明值得的人面前。'),
(92003, 'zhCN', '世代之间口耳相传的低语之中,这些珍稀伙伴源自艾泽拉斯最隐秘的角落——或是深林,或是古迹,或是阴影领域。每一只都带着原乡神秘的火星,赋予独特的能力,令人欢愉与赞叹。然而须当谨慎:它们与凡人的羁绊转瞬即逝,回归传说乃是命中注定。趁其传说仍在,赶紧将其收入囊中吧!'),
(92004, 'zhCN', '嘿,老兄!冬幕节爱好者、节日气氛供应商——哥布林风范,听懂的话就懂啦。瞧你这模样,看来正需要一点节日里的小调皮。怎么样?要不要加入老斯尼兹菲兹,来点儿礼物、潘趣,再来一两个无害的小响声?嗬,嗬……哈哈!'),
(92005, 'zhCN', '此石之中封藏着变形之术——可将自身之形重塑为传奇与野兽之形的力量。只需轻轻一触,魔法便会显现,将幻象与本质交织,赋予英雄、怪物以及形形色色传说之物的外貌。激起敬畏,抑或播下恐惧,皆由你选。现实的帷幕,不过一声轻语之隔。'),
(92006, 'zhCN', '神话与凡尘的界限模糊。此遗物扭曲了这界限,让你披上艾泽拉斯最黑暗之名的外形。外貌惑人——愿你的外貌令人胆寒。'),
(92007, 'zhCN', '运用智慧石的力量,召唤由魔法与钢铁铸就的强力载具!从龙族守卫到附魔构造体,这些绝非寻常坐骑——它们是你意志的延伸。选择你的载具,号令天空、海洋,或大地本身。'),
(92008, 'zhCN', '此处汇集着你天职的些许小恩典——师徒相传的姿态、装束与习惯,由每一位曾披你色彩的英杰共同雕琢。它们不予更大威能,却能铭记。择一者承继之,让看见你的人猜测:他们之中,是哪一段传说在行走。\n\n(仅为装饰——在战斗中并不提供任何优势。)'),
(92009, 'zhCN', '为那些应钢铁号召之人:一姿势,一吼声,一步伐,可撼动整条战线。这并非更重的打击,而是老兵的气度——每一位在你之前坚守前线的英杰皆如此挺立,如今你亦如此。'),
(92010, 'zhCN', '为那些与圣光相系之人:盔甲边缘的微光,呼吸之间低吟的诗句,坚如磐石的守望之姿。这并非更宏大的祝福,而是受誓者的气度——圣光记得自己的子民。'),
(92011, 'zhCN', '为那些与荒野为伴之人:追踪者的眼,雄鹰的静,在开阔天空下安适的从容姿态。这并非更锋利的箭矢,而是每一位在你之前走过此径的浪迹者的气度。'),
(92012, 'zhCN', '为那些行走于心跳之隙者:更轻的步伐,半隐的微笑,曾闪过百道戒备之人的微微前倾。这并非更致命的刀刃,而是每一道在你之前隐于无形的暗影的气度。'),
(92013, 'zhCN', '为那些守护心灵之人:轻柔的语调,稳定的手,听见过无言之声者的沉静仪态。这并非更大的神迹,而是每一位在你之前跪下的告解者与抚慰者的气度。'),
(92014, 'zhCN', '为那些被破誓重铸所束之人:呼吸间挥之不去的坟墓寒意,跟随脚步的寂静,已迈过死亡之门者的气度。这并非更锋利的符文,而是天灾军团也未能从你身上夺去的沉默。'),
(92015, 'zhCN', '为那些与元素对话之人:足跟处的微光,肩头一缕风之低语,先祖近在身侧者沉稳的份量。这并非更强的召唤,而是每一位在你之前静心聆听的言说者的气度。'),
(92016, 'zhCN', '为那些以钻研重塑世界之人:指尖一闪,从未完全垂落的法袍,曾屈伸过一条自然法则者的从容。这并非更宏大的法术,而是每一位在你之前翻开第一页的修习者的气度。'),
(92017, 'zhCN', '为那些与深渊讨价还价之人:眼底的炽炭微光,徘徊得稍久一刻的影,读过少有人愿签之契者的平静。这并非更残酷的邪能,而是每一位在你之前归返的契约承载者的气度。'),
(92018, 'zhCN', '为那些行走荒野者:足跟之泥土,发间之绿叶,曾披过万千形貌者的从容。这并非更强大的形态,而是每一位在你之前回应荒野召唤的变形者的气度。'),
(92019, 'zhCN', '选择你希望前进的速度,日后可随时回到此处更改决定。\n\n想要更细致的调节?你可以使用 |cff4CFF00.weekendxp rate|r 命令将倍率自定义为服务器允许的任意最大值以内的数值。\n '),
(92020, 'zhCN', '此处汇聚自我的丝缕——你发的修饰,你成长的步调,你召命的细小印记。随心打理它们;此石不评判你所采取的形态,只赋予采取它的方法。\n '),

-- zhTW
(92000, 'zhTW', '在艾澤拉斯魔法的核心,存在著一件名為智慧石的傳奇神器。這枚古老的寶石充盈著神秘能量與無盡智慧,為勇敢者和好奇者引路、相伴。當它附魔的切面閃爍微光,智慧石能揭示隱秘的寶藏、給予賢明的勸告,亦能在奧秘與尋常之間提供至關重要的洞見。'),
(92001, 'zhTW', '嘿,朋友!歡迎來到艾澤拉斯最棒的理髮館!需要修剪、刮淨,還是煥然一新?我們這兒的髮型能讓你的敵人膽寒——至少他們會想知道你這帥氣造型是從哪兒來的!坐下,放鬆,讓我們把你打扮得耀眼奪目!'),
(92002, 'zhTW', '散布於艾澤拉斯及其外緣,這些神出鬼沒的生靈承載著各自棲息地的氣韻。從在幽暗森林潛行的幽靈之狼,到在熔岩之巔翱翔的火焰鳳凰,每一隻稀有寵物都是這世界狂野之美與凶險的見證。尋覓它們的冒險者須得機敏、耐心,並能聆聽荒野的低語,因為這些靈獸只會出現在被證明值得的人面前。'),
(92003, 'zhTW', '世代之間口耳相傳的低語之中,這些珍稀伙伴源自艾澤拉斯最隱秘的角落——或是深林,或是古蹟,或是陰影領域。每一隻都帶著原鄉神秘的火星,賦予獨特的能力,令人歡愉與讚嘆。然而須當謹慎:牠們與凡人的羈絆轉瞬即逝,回歸傳說乃是命中注定。趁其傳說仍在,趕緊將其收入囊中吧!'),
(92004, 'zhTW', '嘿,老兄!冬幕節愛好者、節日氣氛供應商——哥布林風範,聽懂的話就懂啦。瞧你這模樣,看來正需要一點節日裡的小調皮。怎麼樣?要不要加入老斯尼茲菲茲,來點兒禮物、潘趣,再來一兩個無害的小響聲?嗬,嗬……哈哈!'),
(92005, 'zhTW', '此石之中封藏著變形之術——可將自身之形重塑為傳奇與野獸之形的力量。只需輕輕一觸,魔法便會顯現,將幻象與本質交織,賦予英雄、怪物以及形形色色傳說之物的外貌。激起敬畏,抑或播下恐懼,皆由你選。現實的帷幕,不過一聲輕語之隔。'),
(92006, 'zhTW', '神話與凡塵的界限模糊。此遺物扭曲了這界限,讓你披上艾澤拉斯最黑暗之名的外形。外貌惑人——願你的外貌令人膽寒。'),
(92007, 'zhTW', '運用智慧石的力量,召喚由魔法與鋼鐵鑄就的強力載具!從龍族守衛到附魔構造體,這些絕非尋常坐騎——它們是你意志的延伸。選擇你的載具,號令天空、海洋,或大地本身。'),
(92008, 'zhTW', '此處匯集著你天職的些許小恩典——師徒相傳的姿態、裝束與習慣,由每一位曾披你色彩的英傑共同雕琢。它們不予更大威能,卻能銘記。擇一者承繼之,讓看見你的人猜測:他們之中,是哪一段傳說在行走。\n\n(僅為裝飾——在戰鬥中並不提供任何優勢。)'),
(92009, 'zhTW', '為那些應鋼鐵號召之人:一姿勢,一吼聲,一步伐,可撼動整條戰線。這並非更重的打擊,而是老兵的氣度——每一位在你之前堅守前線的英傑皆如此挺立,如今你亦如此。'),
(92010, 'zhTW', '為那些與聖光相繫之人:盔甲邊緣的微光,呼吸之間低吟的詩句,堅如磐石的守望之姿。這並非更宏大的祝福,而是受誓者的氣度——聖光記得自己的子民。'),
(92011, 'zhTW', '為那些與荒野為伴之人:追蹤者的眼,雄鷹的靜,在開闊天空下安適的從容姿態。這並非更鋒利的箭矢,而是每一位在你之前走過此徑的浪跡者的氣度。'),
(92012, 'zhTW', '為那些行走於心跳之隙者:更輕的步伐,半隱的微笑,曾閃過百道戒備之人的微微前傾。這並非更致命的刀刃,而是每一道在你之前隱於無形的暗影的氣度。'),
(92013, 'zhTW', '為那些守護心靈之人:輕柔的語調,穩定的手,聽見過無言之聲者的沉靜儀態。這並非更大的神蹟,而是每一位在你之前跪下的告解者與撫慰者的氣度。'),
(92014, 'zhTW', '為那些被破誓重鑄所束之人:呼吸間揮之不去的墳墓寒意,跟隨腳步的寂靜,已邁過死亡之門者的氣度。這並非更鋒利的符文,而是天譴軍團也未能從你身上奪去的沉默。'),
(92015, 'zhTW', '為那些與元素對話之人:足跟處的微光,肩頭一縷風之低語,先祖近在身側者沉穩的份量。這並非更強的召喚,而是每一位在你之前靜心聆聽的言說者的氣度。'),
(92016, 'zhTW', '為那些以鑽研重塑世界之人:指尖一閃,從未完全垂落的法袍,曾屈伸過一條自然法則者的從容。這並非更宏大的法術,而是每一位在你之前翻開第一頁的修習者的氣度。'),
(92017, 'zhTW', '為那些與深淵討價還價之人:眼底的熾炭微光,徘徊得稍久一刻的影,讀過少有人願簽之契者的平靜。這並非更殘酷的邪能,而是每一位在你之前歸返的契約承載者的氣度。'),
(92018, 'zhTW', '為那些行走荒野者:足跟之泥土,髮間之綠葉,曾披過萬千形貌者的從容。這並非更強大的形態,而是每一位在你之前回應荒野召喚的變形者的氣度。'),
(92019, 'zhTW', '選擇你希望前進的速度,日後可隨時回到此處更改決定。\n\n想要更細緻的調節?你可以使用 |cff4CFF00.weekendxp rate|r 指令將倍率自訂為伺服器允許的任意最大值以內的數值。\n '),
(92020, 'zhTW', '此處匯聚自我的絲縷——你髮的修飾,你成長的步調,你召命的細小印記。隨心打理它們;此石不評判你所採取的形態,只賦予採取它的方法。\n ');

DELETE FROM `gossip_menu` WHERE `MenuID` IN (92000, 92001, 92004);
INSERT INTO `gossip_menu` (`MenuID`, `TextID`) VALUES
(92000, 92000),
(92001, 92001),
(92004, 92004);

DELETE FROM `gossip_menu_option` WHERE `MenuID` = 92004;
INSERT INTO `gossip_menu_option` (`MenuID`, `OptionID`, `OptionIcon`, `OptionText`, `OptionType`, `OptionNpcFlag`, `ActionMenuID`) VALUES
(92004, 0, 0, "Okay, I think I had enough holiday cheer for now. Goodbye, little friend!", 1, 1, 0);

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 15) AND (`SourceGroup` = 92004) AND (`SourceEntry` = 0) AND (`SourceId` = 0) AND (`ConditionTypeOrReference` = 33);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(15, 92004, 0, 0, 0, 33, 1, 0, 3, 0, 0, 0, 0, '', 'Winterveil Helper - Only show gossip to summoner');

DELETE FROM `spell_dbc` WHERE `ID` IN (90000, 90001, 90002);
INSERT INTO `spell_dbc` (`ID`, `Category`, `DispelType`, `Mechanic`, `Attributes`, `AttributesEx`, `AttributesEx2`, `AttributesEx3`, `AttributesEx4`, `AttributesEx5`, `AttributesEx6`, `AttributesEx7`, `ShapeshiftMask`, `unk_320_2`, `ShapeshiftExclude`, `unk_320_3`, `Targets`, `TargetCreatureType`, `RequiresSpellFocus`, `FacingCasterFlags`, `CasterAuraState`, `TargetAuraState`, `ExcludeCasterAuraState`, `ExcludeTargetAuraState`, `CasterAuraSpell`, `TargetAuraSpell`, `ExcludeCasterAuraSpell`, `ExcludeTargetAuraSpell`, `CastingTimeIndex`, `RecoveryTime`, `CategoryRecoveryTime`, `InterruptFlags`, `AuraInterruptFlags`, `ChannelInterruptFlags`, `ProcTypeMask`, `ProcChance`, `ProcCharges`, `MaxLevel`, `BaseLevel`, `SpellLevel`, `DurationIndex`, `PowerType`, `ManaCost`, `ManaCostPerLevel`, `ManaPerSecond`, `ManaPerSecondPerLevel`, `RangeIndex`, `Speed`, `ModalNextSpell`, `CumulativeAura`, `Totem_1`, `Totem_2`, `Reagent_1`, `Reagent_2`, `Reagent_3`, `Reagent_4`, `Reagent_5`, `Reagent_6`, `Reagent_7`, `Reagent_8`, `ReagentCount_1`, `ReagentCount_2`, `ReagentCount_3`, `ReagentCount_4`, `ReagentCount_5`, `ReagentCount_6`, `ReagentCount_7`, `ReagentCount_8`, `EquippedItemClass`, `EquippedItemSubclass`, `EquippedItemInvTypes`, `Effect_1`, `Effect_2`, `Effect_3`, `EffectDieSides_1`, `EffectDieSides_2`, `EffectDieSides_3`, `EffectRealPointsPerLevel_1`, `EffectRealPointsPerLevel_2`, `EffectRealPointsPerLevel_3`, `EffectBasePoints_1`, `EffectBasePoints_2`, `EffectBasePoints_3`, `EffectMechanic_1`, `EffectMechanic_2`, `EffectMechanic_3`, `ImplicitTargetA_1`, `ImplicitTargetA_2`, `ImplicitTargetA_3`, `ImplicitTargetB_1`, `ImplicitTargetB_2`, `ImplicitTargetB_3`, `EffectRadiusIndex_1`, `EffectRadiusIndex_2`, `EffectRadiusIndex_3`, `EffectAura_1`, `EffectAura_2`, `EffectAura_3`, `EffectAuraPeriod_1`, `EffectAuraPeriod_2`, `EffectAuraPeriod_3`, `EffectMultipleValue_1`, `EffectMultipleValue_2`, `EffectMultipleValue_3`, `EffectChainTargets_1`, `EffectChainTargets_2`, `EffectChainTargets_3`, `EffectItemType_1`, `EffectItemType_2`, `EffectItemType_3`, `EffectMiscValue_1`, `EffectMiscValue_2`, `EffectMiscValue_3`, `EffectMiscValueB_1`, `EffectMiscValueB_2`, `EffectMiscValueB_3`, `EffectTriggerSpell_1`, `EffectTriggerSpell_2`, `EffectTriggerSpell_3`, `EffectPointsPerCombo_1`, `EffectPointsPerCombo_2`, `EffectPointsPerCombo_3`, `EffectSpellClassMaskA_1`, `EffectSpellClassMaskA_2`, `EffectSpellClassMaskA_3`, `EffectSpellClassMaskB_1`, `EffectSpellClassMaskB_2`, `EffectSpellClassMaskB_3`, `EffectSpellClassMaskC_1`, `EffectSpellClassMaskC_2`, `EffectSpellClassMaskC_3`, `SpellVisualID_1`, `SpellVisualID_2`, `SpellIconID`, `ActiveIconID`, `SpellPriority`, `Name_Lang_enUS`, `Name_Lang_enGB`, `Name_Lang_koKR`, `Name_Lang_frFR`, `Name_Lang_deDE`, `Name_Lang_enCN`, `Name_Lang_zhCN`, `Name_Lang_enTW`, `Name_Lang_zhTW`, `Name_Lang_esES`, `Name_Lang_esMX`, `Name_Lang_ruRU`, `Name_Lang_ptPT`, `Name_Lang_ptBR`, `Name_Lang_itIT`, `Name_Lang_Unk`, `Name_Lang_Mask`, `NameSubtext_Lang_enUS`, `NameSubtext_Lang_enGB`, `NameSubtext_Lang_koKR`, `NameSubtext_Lang_frFR`, `NameSubtext_Lang_deDE`, `NameSubtext_Lang_enCN`, `NameSubtext_Lang_zhCN`, `NameSubtext_Lang_enTW`, `NameSubtext_Lang_zhTW`, `NameSubtext_Lang_esES`, `NameSubtext_Lang_esMX`, `NameSubtext_Lang_ruRU`, `NameSubtext_Lang_ptPT`, `NameSubtext_Lang_ptBR`, `NameSubtext_Lang_itIT`, `NameSubtext_Lang_Unk`, `NameSubtext_Lang_Mask`, `Description_Lang_enUS`, `Description_Lang_enGB`, `Description_Lang_koKR`, `Description_Lang_frFR`, `Description_Lang_deDE`, `Description_Lang_enCN`, `Description_Lang_zhCN`, `Description_Lang_enTW`, `Description_Lang_zhTW`, `Description_Lang_esES`, `Description_Lang_esMX`, `Description_Lang_ruRU`, `Description_Lang_ptPT`, `Description_Lang_ptBR`, `Description_Lang_itIT`, `Description_Lang_Unk`, `Description_Lang_Mask`, `AuraDescription_Lang_enUS`, `AuraDescription_Lang_enGB`, `AuraDescription_Lang_koKR`, `AuraDescription_Lang_frFR`, `AuraDescription_Lang_deDE`, `AuraDescription_Lang_enCN`, `AuraDescription_Lang_zhCN`, `AuraDescription_Lang_enTW`, `AuraDescription_Lang_zhTW`, `AuraDescription_Lang_esES`, `AuraDescription_Lang_esMX`, `AuraDescription_Lang_ruRU`, `AuraDescription_Lang_ptPT`, `AuraDescription_Lang_ptBR`, `AuraDescription_Lang_itIT`, `AuraDescription_Lang_Unk`, `AuraDescription_Lang_Mask`, `ManaCostPct`, `StartRecoveryCategory`, `StartRecoveryTime`, `MaxTargetLevel`, `SpellClassSet`, `SpellClassMask_1`, `SpellClassMask_2`, `SpellClassMask_3`, `MaxTargets`, `DefenseType`, `PreventionType`, `StanceBarOrder`, `EffectChainAmplitude_1`, `EffectChainAmplitude_2`, `EffectChainAmplitude_3`, `MinFactionID`, `MinReputation`, `RequiredAuraVision`, `RequiredTotemCategoryID_1`, `RequiredTotemCategoryID_2`, `RequiredAreasID`, `SchoolMask`, `RuneCostID`, `SpellMissileID`, `PowerDisplayID`, `EffectBonusMultiplier_1`, `EffectBonusMultiplier_2`, `EffectBonusMultiplier_3`, `SpellDescriptionVariableID`, `SpellDifficultyID`) VALUE
(90000,0,0,0,262416,0,0,536870912,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,31,0,0,0,101,0,0,0,0,21,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,28,0,0,1,0,0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000,0,0,0,0,0,0,0,0,190011,0,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,458,0,2808,0,0,'Summon Exotic Pet','','','','','','','','','','','','','','','',16712190,'','','','','','','','','','','','','','','','',16712190,'Right Click to summon and dismiss your Exotic Pet.','','','','','','','','','','','','','','','',16712190,'','','','','','','','','','','','','','','','',16712190,0,133,1500,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0),
(90001,0,0,0,262416,0,0,536870912,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,31,0,0,0,101,0,0,0,0,21,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,28,0,0,1,0,0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000,0,0,0,0,0,0,0,0,190011,0,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,458,0,2808,0,0,'Summon Combat Pet','','','','','','','','','','','','','','','',16712190,'','','','','','','','','','','','','','','','',16712190,'Right Click to summon and dismiss your Exotic Pet.','','','','','','','','','','','','','','','',16712190,'','','','','','','','','','','','','','','','',16712190,0,133,1500,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0),
(90002,0,0,0,262416,0,0,536870912,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,31,0,0,0,101,0,0,0,0,21,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,458,0,2808,0,0,'Costume Cooldown Marker (DUMMY)','','','','','','','','','','','','','','','',16712190,'','','','','','','','','','','','','','','','',16712190,'Costume cooldown marker (dummy spell).','','','','','','','','','','','','','','','',16712190,'','','','','','','','','','','','','','','','',16712190,0,133,1500,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

UPDATE `spell_dbc` SET `EffectMiscValueB_1` = 67 WHERE `ID` = 90001;

SET
@Entry := 80001,
@Name := "Amani'shi Groundbreaker",
@Model := 22353,
@Scale := 0.30,
@SubLevel := 2,
@Description := "Summon Amani'shi Groundbreaker";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);

DELETE FROM `smartstone_pets` WHERE `CreatureId` = @Entry;
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `Category`, `SubscriptionLevel`, `Description`) VALUES
(@Entry, 1, 1, @SubLevel, @Description);

SET
@Entry := 80002,
@Name := "Hyjal Wisp",
@Model := 10045,
@Scale := 0.3,
@SubLevel := 0,
@Description := "Summon Hyjal Wisp";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);

DELETE FROM `smartstone_pets` WHERE `CreatureId` = @Entry;
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `Category`, `SubscriptionLevel`, `Description`) VALUES
(@Entry, 1, 1, @SubLevel, @Description);

SET
@Entry := 80003,
@Name := "Serpentshrine Waterspawn",
@Model := 20076,
@Scale := 0.2,
@SubLevel := 0,
@Description := "Summon Serpentshrine Waterspawn";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);

DELETE FROM `smartstone_pets` WHERE `CreatureId` = @Entry;
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `Category`, `SubscriptionLevel`, `Description`) VALUES
(@Entry, 1, 1, @SubLevel, @Description);

SET
@Entry := 80004,
@Name := "Vashj'ir Enforcer",
@Model := 20200,
@Scale := 0.2,
@SubLevel := 2,
@Description := "Summon Vashj'ir Enforcer";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);

DELETE FROM `smartstone_pets` WHERE `CreatureId` = @Entry;
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `Category`, `SubscriptionLevel`, `Description`) VALUES
(@Entry, 1, 1, @SubLevel, @Description);

SET
@Entry := 80005,
@Name := "Infernaling",
@Model := 12817,
@Scale := 0.2,
@SubLevel := 2,
@Description := "Summon Infernaling";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);

DELETE FROM `smartstone_pets` WHERE `CreatureId` = @Entry;
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `Category`, `SubscriptionLevel`, `Description`) VALUES
(@Entry, 1, 1, @SubLevel, @Description);

SET
@Entry := 80006,
@Name := "Thunderwing",
@Model := 22255,
@Scale := 0.40,
@SubLevel := 0,
@Description := "Summon Thunderwing";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);

DELETE FROM `smartstone_pets` WHERE `CreatureId` = @Entry;
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `Category`, `SubscriptionLevel`, `Description`) VALUES
(@Entry, 1, 1, @SubLevel, @Description);

-- Thunderwing AI
UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 80006;

DELETE FROM `smart_scripts` WHERE (`source_type` = 0 AND `entryorguid` = 80006);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `event_param6`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(80006, 0, 0, 0, 1, 0, 100, 1, 1, 1, 0, 0, 0, 0, 11, 37964, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Thunderwing - Out of Combat - Cast \'Call Lightning (Cosmetic)\' (No Repeat)'),
(80006, 0, 1, 0, 1, 0, 100, 0, 3000, 6000, 900000, 1800000, 0, 0, 75, 32910, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Thunderwing - Out of Combat - Add Aura \'Windfury\'');

SET
@Entry := 80007,
@Name := "Wretched Servant",
@Model := 22845,
@Scale := 0.50,
@SubLevel := 2,
@Description := "Summon Wretched Servant";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);

DELETE FROM `smartstone_pets` WHERE `CreatureId` = @Entry;
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `Category`, `SubscriptionLevel`, `Description`) VALUES
(@Entry, 1, 1, @SubLevel, @Description);

SET
@Entry := 80008,
@Name := "Mistress of Suffering",
@Model := 21503,
@Scale := 0.10,
@SubLevel := 2,
@Description := "Summon Mistress of Suffering";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);

DELETE FROM `smartstone_pets` WHERE `CreatureId` = @Entry;
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `Category`, `SubscriptionLevel`, `Description`) VALUES
(@Entry, 1, 1, @SubLevel, @Description);

SET
@Entry := 80009,
@Name := "Sunblade Mini Protector",
@Model := 23240,
@Scale := 0.10,
@SubLevel := 2,
@Description := "Summon Sunblade Mini Protector";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);

DELETE FROM `smartstone_pets` WHERE `CreatureId` = @Entry;
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `Category`, `SubscriptionLevel`, `Description`) VALUES
(@Entry, 1, 1, @SubLevel, @Description);

SET
@Entry := 80010,
@Name := "Alythessa",
@Model := 23334,
@Scale := 0.20,
@SubLevel := 0,
@Description := "Summon Alythessa";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);

DELETE FROM `smartstone_pets` WHERE `CreatureId` = @Entry;
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `Category`, `SubscriptionLevel`, `Description`) VALUES
(@Entry, 1, 1, @SubLevel, @Description);

DELETE FROM `creature_template_addon` WHERE `entry` = @ENTRY;
INSERT INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `visibilityDistanceType`, `auras`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 0, '45457');

SET
@Entry := 80011,
@Name := "Scorchling of Azzinoth",
@Model := 21321,
@Scale := 0.40,
@SubLevel := 0,
@Description := "Summon Scorchling of Azzinoth";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);

DELETE FROM `smartstone_pets` WHERE `CreatureId` = @Entry;
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `Category`, `SubscriptionLevel`, `Description`) VALUES
(@Entry, 1, 1, @SubLevel, @Description);

SET
@Entry := 90001,
@Name := "Winterveil Helper",
@Model := 19342,
@Scale := 1,
@Description := "Summon Winter Reveler";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `rank`, `dmgschool`, `BaseAttackTime`, `RangeAttackTime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, 0, 0, 2000, 0, 1, 768, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);

DELETE FROM `creature_template_spell`  WHERE `CreatureId` = @Entry;
INSERT INTO `creature_template_spell` (`CreatureId`, `Index`, `Spell`) VALUES
(@Entry, 0, 46661),
(@Entry, 1, 44755);

DELETE FROM `smartstone_pets` WHERE `CreatureId` = @Entry;
INSERT INTO `smartstone_pets` (`CreatureId`, `Type`, `Category`, `Enabled`, `Description`) VALUES
(@Entry, 1, 2, 1, @Description);
