# Embermere Daily TODO

This is the daily handoff file for Codex work. Each session should start here, compare against the broader project plan in `README.md`, `JOURNEY.md`, and `Docs/PLAYTESTING.md`, then update this file before stopping.

For a fresh Codex task or context reset, read [Docs/THREAD_HANDOFF.md](Docs/THREAD_HANDOFF.md) before this daily file.

## Start Here

- September 4 Astra continuation: the new quest-update observer is implemented
  and the final isolated suite passes `91/91`. Read
  `Docs/QUEST_UPDATE_PRESENTATION_CONTRACT.md` and
  `Docs/ASTRA_EMBERMERE_EVALUATION_2026-09-04.md` before extending it. The
  journal emblem is project-owned Blender UI art, not a new world placement.
  After local unlock, fresh Human Warrior Q traversal plus W cancellation
  reached Mara and real F showed acceptance. A labeled temporary objective
  fixture then showed 1/3 progress and 3/3 ready; real F committed 125 XP,
  20 copper, and one Recruit Pack. Completion, reward, and level-up panels
  coexisted without mutual overlap; Inventory cleared the notice without replay.
  **First remaining gate:** replay the real Prowler and Still Waters routes,
  remaining peer panels, and held right-mouse/Ctrl+M input. Do not call injected
  objective progress a combat-route replay. Do not overwrite the user's slot:
  Chronicle currently rejects contradictory legacy quest state in its v3 save;
  this session only inspected it and did not save or load it.
  Check the inherited long Still Waters tracker instruction for clipping;
  the full ledger detail remains the readable fallback. No map art moved.
- Confirm Unreal is running the 2026-09-04 no-hot-reload quest-update observer
  and saved `T_FenwatchJournalEmblem` texture over the quest-objective
  presentation module and both saved quest packages over the 2026-09-03
  selected-quest detail module, the 2026-09-02 native Quest Ledger, the
  2026-09-01 `Still
  Waters` quest/service/router module plus the
  saved `DQ_FenwatchStillWaters` and map packages, the 2026-08-31
  save-version-3 multi-quest module, and the
  2026-08-29 communal-well rest-presentation
  module and accepted observer/map package, the 2026-08-28
  rest-service module and data package, and the 2026-08-27
  Fenwatch communal-well art package, the 2026-08-26
  class-colored level-up world-VFX module, and the 2026-08-25 level-gated trainer,
  progression-presentation, post-creation input, and Chronicle current/saved-state module plus the
  accepted derived-level module and serialized
  `DA_EmbermereRules` thresholds/growth,
  save-version-2 character identity, the accepted Fenwatch notice-board asset/map package and
  combat-feedback module,
  along with the accepted rigged Fenwatch keeper, armsmaster, and quartermaster
  packages and
  the Fenwatch vendor-stall, first closed cottage, and training-workshop/map
  packages, the saved native Fenwatch practice target, quest-owned Mara
  greeting copy, and saved read-only greeting observer. Restart if the editor
  predates that work or test discovery exposes fewer than 91 Embermere tests.
  Start MCP with `-ModelContextProtocolStartServer
  -ModelContextProtocolPort=8123`; on macOS pass the full `.uproject` after
  `open ... --args`. Confirm Blender only when original-art work is selected.
- Discover and run all 91 tests, especially
  `Embermere.Quests.LiveUpdateContract`,
  `Embermere.UI.QuestUpdatePresentation`,
  `Embermere.UI.QuestUpdateLifecycle`,
  `Embermere.UI.QuestObjectiveDisplayPresentation`,
  `Embermere.UI.QuestLedgerDetailsPresentation`,
  `Embermere.UI.QuestLedgerDetailLifecycle`,
  `Embermere.UI.QuestLedgerPresentation`,
  `Embermere.UI.QuestLedgerFocusLifecycle`,
  `Embermere.Quests.StillWatersServiceContract`,
  `Embermere.Quests.StillWatersRestRouting`,
  `Embermere.Persistence.StillWatersRoundTrip`,
  `Embermere.Quests.MultiQuestRuntime`,
  `Embermere.Persistence.MultiQuestRoundTrip`,
  `Embermere.Persistence.LegacyQuestCompatibility`,
  `Embermere.Persistence.MultiQuestValidationRollback`,
  `Embermere.UI.RestWorldPresentation`,
  `Embermere.Rest.ServiceContract`,
  `Embermere.Rest.RecoveryTransactions`,
  `Embermere.Rest.InterruptionAndCombat`,
  `Embermere.UI.LevelUpWorldVfxPresentation`,
  `Embermere.UI.ExperienceProgressPresentation`,
  `Embermere.UI.LevelUpPresentation`,
  `Embermere.Progression.LevelRules`,
  `Embermere.Progression.LiveExperienceAndEquipment`,
  `Embermere.Progression.RewardOwners`,
  `Embermere.Progression.ValidationRollback`,
  `Embermere.UI.CharacterCreationInitialState`,
  `Embermere.UI.CharacterCreationRestrictions`,
  `Embermere.CharacterCreation.ConfirmationLoadout`,
  `Embermere.CharacterCreation.ControllerLifecycle`,
  `Embermere.Combat.ResultContract`,
  `Embermere.UI.CombatFeedbackPresentation`,
  `Embermere.Combat.PracticeTargetPolicy`,
  `Embermere.Combat.PracticeTargetCombatReset`,
  `Embermere.Persistence.RoundTrip`,
  `Embermere.Persistence.CharacterIdentityRoundTrip`,
  `Embermere.Persistence.CharacterIdentityRollback`,
  `Embermere.Persistence.LegacyV1CharacterFallback`,
  `Embermere.Persistence.ValidationRollback`,
  `Embermere.Persistence.SlotInspection`,
  `Embermere.UI.SaveLoadPanel`,
  `Embermere.Economy.FenwatchRewardsAndValues`,
  `Embermere.Vendor.TransactionRules`,
  `Embermere.Vendor.SellBuybackTransactions`,
  `Embermere.Vendor.ServiceContract`,
  `Embermere.Vendor.FenwatchStockData`,
  `Embermere.UI.VendorPanel`,
  `Embermere.Trainer.TransactionRules`,
  `Embermere.Trainer.ServiceContract`,
  `Embermere.Trainer.FenwatchOfferingsData`,
  `Embermere.Trainer.LevelGatedProgression`,
  `Embermere.Trainer.LevelGatedPersistence`,
  `Embermere.NPC.FenwatchArmsmasterPresentation`,
  `Embermere.NPC.FenwatchArmsmasterIdlePresentation`,
  `Embermere.NPC.FenwatchQuartermasterIdlePresentation`,
  `Embermere.NPC.FenwatchKeeperPresentation`,
  `Embermere.NPC.FenwatchKeeperIdlePresentation`,
  `Embermere.NPC.ContextGreetingPresentation`,
  `Embermere.UI.TrainerPanel`,
  `Embermere.NPC.SkeletalIdlePresentation`, the three established NPC
  presentation tests, Prowler/world presentation, player recovery, and
  inventory transaction suites. The authoritative 2026-08-18 isolated
  headless run passed 56/56; the no-hot-reload Mac build and fresh-process
  combat-feedback, practice-target, greeting,
  vendor-stall, cottage, training-workshop,
  keeper-rig, saved-map, UI-art, armsmaster-rig,
  quartermaster-rig, practice-dummy, Fenwatch-trainer, vendor-economy, and
  initialized-world route validators also passed. The authoritative latest
  2026-09-04 Astra no-hot-reload build and final isolated commandlet passed
  91/91. New coverage proves post-commit copied quest updates, rejection
  silence, saturating progress, exact reward state, reentrancy guards,
  initialized fixed widget geometry, latest-wins/expiry/modal/death cleanup,
  and full version-3 restore silence twice. Existing coverage proves
  quest-owned objective instructions, their stable fallback,
  fixed tracker/detail clipping, active/ready/completed continuity, and
  selection/focus independence alongside the fixed native Quest Ledger,
  selected-quest details and exact item/no-item rewards, transient
  stable-ID focus, full eight-row geometry, peer-panel lifecycle, and zero
  quest/reward mutation, the data-driven `Still Waters` owner and
  committed-rest router, two simultaneous keyed quests, exact objective
  routing and turn-in, reward-preflight rollback, exactly-once completion,
  native version-3 round-trip, repeated-load idempotence, full-ledger malformed
  rollback, and read-only version-1/version-2 singular adapters. The run also retained
  the idempotent character-creation input lock, bottom-right
  Chronicle command geometry, live Dwarf Warrior identity, and fixed current-
  versus-saved Chronicle layout; the
  standalone `Still Waters`, communal-well presentation, communal-well rest,
  communal-well art, trainer, progression, and quest-objective validators plus
  the fresh aggregate (now 21 with the journal texture validator)
  passed with
  explicit success markers while retaining 53 grounded Fab plus 24 original-
  art actors. Clean PIE accepted the real `F` rest channel at the separate
  art-free service, exact `70/100 -> 100/100` health and `30/50 -> 50/50`
  mana restoration, fixed prompt/chat feedback, and full-resource rejection.
  Clean PIE accepted the separate observer's twelve-segment cyan shaft channel
  and committed mint recovery bloom with fixed bounds, no collision, no HUD
  movement, and exact service-owned `35 Health` / `20 Mana` recovery.
  Initialized-world traces retained all six intentional well surfaces, clear
  decoration, and every protected village route. Clean PIE confirmed Human
  Warrior creation, Inventory close, Q movement from `(-2400,-1200)` to
  `(-1793.189,-831.382)`, exact W cancellation, and physical `F` Mara
  acceptance. Clean PIE then used physical `F` to accept `Still Waters` at the
  notice board, accept Mara independently, reject one moved rest without
  progress, commit one real rest to reach `1/1`, and turn in exactly `50` XP
  plus `10` copper once. An isolated version-3 probe slot restored completed
  `Still Waters`, active Mara, `50` XP, and `50` copper on two confirmed loads,
  then deleted itself without touching the user's Chronicle slot. The
  2026-09-03 clean PIE gate retained active Mara at `0/3`, completed Still
  Waters through a real communal-well `F` channel, and paid its exact `50` XP
  plus `10` copper once. The Quest Ledger then showed both fixed
  rows as Mara `ACTIVE 0/3` and Still Waters `COMPLETED 1/1`; the fixed detail
  region showed Mara's exact `125 XP | 20 copper | Recruit Pack` and Still
  Waters' `50 XP | 10 copper | No item reward`. Physical Up/Down refreshed only
  details, Enter switched the compact tracker, and `I` replaced the ledger
  without record or reward mutation. The 2026-09-04 clean PIE gate physically
  accepted both quests, showed each exact objective instruction in the corrected
  fixed `260x68` tracker and unchanged `596x120` detail region, then selected
  Mara while Still Waters remained tracked. All initialized-world route and
  collision checks remained clean.
- Retain the accepted save-version-3 multi-quest boundary:
  - `QuestStates` is the only mutable runtime quest authority and is bounded to
    eight unique stable quest IDs;
  - every progress and completion mutation targets an exact quest/objective ID
    pair; a wrong giver cannot complete or reward another ready quest;
  - `ActiveQuest` is a derived compatibility projection for existing Blueprint
    and HUD readers, while `FocusedQuestId` is transient and not serialized;
  - native version `3` saves a stable-ID/asset/objective/progress/completion
    record array; the entire candidate ledger must resolve and validate before
    one atomic replacement;
  - versions `1` and `2` remain readable by adapting their zero-or-one singular
    record through the same validator without rewriting the source slot;
  - duplicate, missing, mismatched, over-capacity, invalid-progress, and
    contradictory records reject the whole load without wallet, XP, item,
    equipment, quest, or vendor mutation;
  - `Still Waters` is accepted through the separate art-free notice-board
    owner and dedicated committed-rest objective router defined in
    [Docs/MULTI_QUEST_CONTRACT.md](Docs/MULTI_QUEST_CONTRACT.md); the notice-board
    mesh, communal-well art, rest service/presentation, and trainer remain
    quest-free;
  - only an immutable committed rest `Success` advances
    `FenwatchStillWaters/FenwatchRestCompleted`; pending, interrupted, rejected,
    duplicate, presentation, and loaded state cannot advance it;
  - its physical turn-in grants exactly `50` XP and `10` copper once, coexists
    with Mara, and round-trips as a second version-3 record without durable
    focus or schema expansion.
- Retain the accepted native Quest Ledger boundary:
  - the centered panel remains fixed at `620x550`, with eight reserved
    `596x30` rows, one fixed `596x120` selected-record detail region, exact
    tracked-count copy, a fixed footer/action region, and no layout growth for
    empty, one-, two-, or eight-record states;
  - every tracked row reads stable quest identity, quest-owned title,
    `ACTIVE`, `READY`, or `COMPLETED` state, and objective progress from the
    authoritative quest log and data asset;
  - row selection remains widget-local while mouse action, `Focus Quest`, or
    Enter requests one explicit stable-ID focus; Up/Down wraps only over
    occupied rows and duplicate focus is harmless;
  - selected details read the quest-owned title, description, state copy,
    objective progress, distinct objective instruction, XP, copper, and
    optional item display name; no item and unusable item metadata use stable
    presentation-only fallbacks;
  - the compact tracker remains a fixed `260x68` clipped three-line surface and
    reads the same data-owned instruction; blank or missing metadata reports
    `Objective details unavailable.` without changing stable IDs, numeric
    requirements, progress, save bytes, or rewards;
  - `J` and the fixed `140x38` bottom-right command open the same panel eight
    pixels above Chronicle; Inventory, Vendor, Trainer, Chronicle, and Quest
    Ledger remain mutually exclusive under controller-owned cursor/input state;
  - focus changes only the compact `ActiveQuest` compatibility projection and
    never acceptance, progress, completion, rewards, wallet, XP, inventory,
    equipment, save bytes, or service state. Treat
    [Docs/QUEST_LEDGER_PRESENTATION_CONTRACT.md](Docs/QUEST_LEDGER_PRESENTATION_CONTRACT.md)
    as the presentation boundary.
  Treat [Docs/QUEST_OBJECTIVE_PRESENTATION_CONTRACT.md](Docs/QUEST_OBJECTIVE_PRESENTATION_CONTRACT.md)
  as the descriptive-copy boundary.
- Retain the accepted character-creation authority and lifecycle contract:
  - the centered fixed `940x560` modal shows all eight races and four classes
    before gameplay while the normal HUD remains hidden;
  - invalid classes stay visible and disabled, changing race never silently
    replaces the pending class, and both Dwarf Ranger and Bullywug Wizard
    remain impossible to confirm;
  - `UEmbermereRulesData` exclusively owns legality, class starting
    attributes, and starter ability IDs; the widget owns only pending choice;
  - one successful character-side confirmation atomically applies identity,
    full starting vitals, attributes, and four hotbar abilities, then the
    controller restores the normal HUD and game-only input path;
  - the controller owns exactly one idempotent character-creation move/look
    suppression lock even when both `OnPossess` and `BeginPlay` request the
    modal; one confirmation releases it completely;
  - clean PIE accepted Elf Wizard at exact `80/80` health, `110/110` mana, and
    Spark Bolt, Frost Root, Arcane Burst, and Meditate, with
    `Journey begun: Elf Wizard` in chat;
  - the focused regression accepted Dwarf Warrior, closed the initially open
    Inventory with `I`, and proved movement/look were both enabled, the cursor
    was hidden, and classic mouse camera control returned immediately;
  - Human Warrior remains the reversible construction fallback. Save version
    `3` persists a deliberately confirmed race/class pair through the version-2
    identity fields plus the new quest ledger, while version `1` remains
    loadable through an explicit current-rules
    Human Warrior compatibility fallback without rewriting the old slot. Treat
    [Docs/CHARACTER_CREATION_CONTRACT.md](Docs/CHARACTER_CREATION_CONTRACT.md)
    as the authority and first-slice boundary.
- Retain the accepted post-modal input and Chronicle layout contract:
  - closing Inventory clears held mouse state, enters game-only input with the
    capture mouse-down left unconsumed, and restores the first right-mouse
    camera press instead of requiring a throwaway click;
  - the `Chronicle` command is fixed to the bottom-right at `140x38` with a
    `24`-pixel edge margin, remains clear of the top-right Inventory and the
    bottom hotbar/chat, and keeps its existing `M` keyboard path;
  - the centered Chronicle panel is fixed at `500x320`; its gold `Current
    Journey` row reads the possessed live character, while `Saved Journey`
    remains a separate read-only inspection of `EmbermerePrototype` and may
    legitimately show a different identity;
  - the saved summary owns a fixed `460x78` clipped region followed by 12
    pixels of spacing before `Save Current` and `Load Saved`, so its bag,
    equipment, and quest line cannot collide with either action;
  - clean PIE accepted current `Dwarf Warrior | Level 1` above the existing
    saved `Elf Wizard | Level 1` slot without overwriting or loading it;
  - `Embermere.CharacterCreation.ControllerLifecycle` covers duplicate lock
    acquisition/release, and `Embermere.UI.SaveLoadPanel` covers the durable
    Chronicle anchor, current/saved labels, summary ordering, spacing, and
    dimensions.
- Retain the accepted derived-level progression contract:
  - durable XP remains the only serialized progression quantity; level is
    derived from cumulative thresholds `0`, `100`, `250`, `450`, and `700`
    for the bounded level `1` through `5` curve;
  - `UEmbermereRulesData` owns strictly increasing thresholds plus nonnegative
    race/class growth, while Stats owns live XP and the resulting level;
  - identity base stats resolve as class starting attributes plus combined
    race/class growth for each gained level, then equipment remains one
    separate additive layer;
  - live XP grants preserve absolute missing health/mana and publish exact XP
    plus one multi-level-aware level-up message; load resolves silently without
    replaying feedback, rewards, or growth;
  - persistence preflights item level requirements against the candidate level
    implied by saved XP, never the unrelated pre-load live level;
  - accepted Human Warrior PIE progressed from Trainer-owned `25` XP to
    Mara-owned `150` total XP and level `2`, with exact `110/110` health,
    `53/53` mana, `12` Attack Power, `12` Strength, `9` Spirit, `11.25`
    Agility, `7.75` Intellect, and the unchanged Warrior hotbar;
  - Chronicle read the same state as `Human Warrior | Level 2`, `50 copper`,
    `150 XP`, one bag stack, no equipment, and the completed Mara quest.
  Treat [Docs/LEVEL_PROGRESSION_CONTRACT.md](Docs/LEVEL_PROGRESSION_CONTRACT.md)
  as the XP, level, growth, load, and presentation authority boundary.
- Retain the accepted progression-presentation contract:
  - Stats alone resolves one read-only snapshot containing derived level,
    total XP, current/next thresholds, cap state, and normalized within-level
    progress; UMG owns no threshold table or level calculation;
  - the top-left status uses fixed `260x8` gold progress geometry and shows
    `Level 1   XP 0 / 100`, exact next-threshold totals, or explicit `CAP` copy;
  - the separate hit-test-invisible observer uses a fixed `360x76` panel and
    `2.75`-second lifetime for one-level or multi-level live transitions;
  - exact chat remains the durable fallback, and silent load updates steady
    state without broadcasting or replaying transient celebration;
  - the player also observes that same post-commit live-only transition through
    twelve transient project-material ground segments. They resolve color from
    the confirmed class, expand and rotate for `1.6` seconds, remain
    `NoCollision` and navigation-free, then clear on expiry, death, or teardown;
  - clean PIE used four real Combat Drills transactions to reach Human Warrior
    level `2` at `100` XP, proved all twelve orange-gold segments live, and
    visually accepted their readable ground contact beneath the first-person
    camera with Inventory both open and closed;
  - Trainer, Mara, rules, Stats, equipment, rewards, and persistence retain all
    existing authority.
- Retain the accepted Fenwatch practice-target split:
  - `Embermere_FenwatchPracticeDummy_TrainingYard_01` remains the visible
    project-owned art and the sole owner of its two purposeful solid boxes;
  - `Embermere_FenwatchPracticeTarget_Gameplay_01` remains a separate native,
    art-free actor at the same `(-1120, -1120, 0)`, yaw `45` transform;
  - `Tab` selects `Fenwatch Practice Target`, its native nameplate and complete
    48-segment cyan circle read around the dummy, and normal class abilities
    damage its 150-health pool;
  - it remains stationary at the exact authored transform with zero gravity,
    `MOVE_None`, no collision/navigation, AI, aggro, retaliation, leash, loot,
    XP, quest credit, trainer interaction, or persistence authority;
  - lethal damage clears selection, the visible dummy remains, and the native
    target resets to full health after three seconds for another round;
  - the accepted six-Strike PIE proof left player health at `100`, XP at `0`,
    inventory stacks at `0`, and Mara objective progress at `0`, then `Tab`
    reacquired the reset target immediately.
  Treat [Docs/PRACTICE_TARGET_CONTRACT.md](Docs/PRACTICE_TARGET_CONTRACT.md) as
  the authority boundary.
- Retain the accepted floating combat-feedback contract:
  - combat publishes one immutable result only after an outcome is committed,
    with exact post-mitigation amount, source, target, ability, and lethal state;
  - the standalone hit-test-invisible observer shows only damage and future
    miss results in at most three fixed `112x32` slots for `1.25` seconds;
  - the newest result appears first, a fourth evicts the oldest, and target
    switch, deselection, defeat, practice-target reset, invalidation, expiry,
    or world teardown clears stale presentation;
  - the targetable presentation anchor excludes screen-space nameplate bounds
    and validates older Blueprint event results before using the native fallback;
  - normal-camera PIE accepted exact `28` damage beside both the practice
    target and a saved Marsh Prowler with fixed `104x30` rendered geometry,
    readable nameplate/cyan-circle coexistence, immediate clear, and unchanged
    target HP plus bottom-left chat fallbacks.
  Treat [Docs/COMBAT_FEEDBACK_CONTRACT.md](Docs/COMBAT_FEEDBACK_CONTRACT.md) as
  the outcome/presentation authority boundary.
- Recheck the accepted Fenwatch vendor loop through normal `F` interaction:
  - presentation actor `Embermere_FenwatchQuartermaster_Vendor_01` remains
    art-only at `(-1530, -1190, 0)`, yaw `100`, unit scale and `NoCollision`;
  - co-located `Embermere_FenwatchQuartermaster_Service_01` owns interaction
    and vendor behavior but no mesh, collision, navigation, or art;
  - `DA_FenwatchQuartermasterStock` offers unlimited Marsh Tonic at `8` copper
    and one Recruit Pack at `30` copper;
  - a new player starts with `40` copper; buying a tonic produces `32`, selling
    that exact selected stack produces `35`, and buying it back for the same
    `3` copper produces `32` with the original item identity restored;
  - buying the one Recruit Pack then produces `2`, grants the item, leaves its
    stock at zero, and a second tonic request is rejected for insufficient
    funds without mutation;
  - Marsh Tonic and Recruit Pack sell values are data-driven at `3` and `12`
    copper; quest-category or zero-value items are unsellable;
  - completing Mara's quest grants `20` copper exactly once alongside `125` XP
    and the Recruit Pack reward, making currency part of the playable loop;
  - the fixed `500x325` native panel keeps title, purse, stock, icon/detail,
    two-line result, Buy, Sell selected, latest Buyback, and footer inside
    bounds, hides Inventory while open, posts chat feedback, and restores
    game-only input when closed;
  - full-bag, insufficient-funds, sold-out, not-owned, unsellable, and wallet-
    overflow rejections preserve copper, stock, buyback, and inventory without
    partial mutation.
- Recheck the accepted Fenwatch trainer loop through normal `F` interaction:
  - presentation actor `Embermere_FenwatchArmsmaster_Trainer_01` remains
    grounded art-only at `(-1320, -920, 0)`, yaw `100`, unit scale and
    `NoCollision`;
  - co-located `Embermere_FenwatchArmsmaster_Service_01` exclusively owns the
    marker, interaction, offering data, and transaction behavior without art;
  - `DA_FenwatchArmsmasterOfferings` exposes two stable-ID repeatable lessons:
    level-1 Combat Drills costs `10` copper for `25` XP, while level-2 Advanced
    Combat Drills costs `20` copper for `50` XP;
  - at level 1, Advanced Combat Drills remains visible and inspectable but
    locked, exposes exact `Advanced Combat Drills requires level 2.` copy, and
    rejects direct requests without mutating copper or XP;
  - fresh PIE starts at `40` copper and `0` XP; Combat Drills must still produce
    exactly `30` copper and `25` XP with current panel status and exact chat
    feedback;
  - after authoritative XP derives level 2, Advanced Combat Drills becomes
    `READY` without reopening the panel or retaining stale rejection copy. The
    accepted isolated transaction changed `50` copper / `100` XP to exactly
    `30` copper / `150` XP and remained level 2;
  - exhausting the purse disables training, reports insufficient funds, and
    preserves nonnegative copper and exact XP;
  - the fixed `500x300` panel keeps both offering names, compact lock/ready
    state, level/cost/reward/repeatability detail, current preflight status,
    footer, and close control inside bounds and clear of chat/hotbar;
  - Inventory and Chronicle each replace the trainer cleanly, while closing the
    final interactive panel restores classic game-only input.
- Treat [Docs/VENDOR_SERVICE_CONTRACT.md](Docs/VENDOR_SERVICE_CONTRACT.md) as
  the economy boundary. The NPC wrapper owns only art; the service owns
  interaction and stock; the wallet and inventory own player state; the HUD
  only presents and requests transactions.
- Treat [Docs/TRAINER_SERVICE_CONTRACT.md](Docs/TRAINER_SERVICE_CONTRACT.md) as
  the progression boundary. The armsmaster owns only art; the service owns
  interaction and offerings; wallet/stats own player state; the HUD only
  presents and requests a transaction.
- Treat [Docs/SAVE_GAME_CONTRACT.md](Docs/SAVE_GAME_CONTRACT.md) as the durable
  state boundary. Version `2` stores confirmed race/class identity, copper, XP,
  inventory stack identity and
  quantity, equipped-item identity and slot, quest state/progress, and finite
  vendor stock through stable IDs plus validated soft paths. Capture and load
  are preflighted before mutation; unknown assets, bad versions, invalid
  quantities, capacity conflicts, mismatched IDs/slots, and malformed vendor
  records reject the entire restore. Identity and saved XP derive level and
  identity-owned base stats atomically before the starter hotbar and equipment
  restore; equipment bonuses are rebuilt once and quest rewards are not
  replayed. No duplicate level is serialized. Version `1` remains readable as
  Human Warrior through current rules without implicit migration.
- Retain the accepted live persistence proof. In one normal PIE session the
  vendor and quest loop saved exactly `22` copper, `125` XP, one Marsh Tonic,
  one bagged Recruit Pack, one equipped Back-slot Recruit Pack, derived level
  `2`, completed Mara state, and exhausted finite Recruit Pack stock. A fresh PIE session restored
  that exact state through `EmbermereLoad`, and a second load remained
  idempotent with no duplication or stat inflation. `EmbermereSave` and
  `EmbermereLoad` use prototype slot `EmbermerePrototype`; buyback history,
  transient combat, cooldowns, temporary effects, and world position are
  intentionally session-only/reset state.
- Retain the accepted trainer-to-Chronicle persistence proof. Fresh PIE trained
  exactly once from `40` copper/`0` XP to `30` copper/`25` XP, saved through the
  real Chronicle control, stopped PIE, and began a second world at the normal
  `40`/`0` baseline. The first Chronicle load restored `30`/`25`; a second load
  remained exactly `30`/`25` with no items, equipment, quest, vendor-stock, or
  buyback mutation. Trainer offerings, selection, panel state, and interaction
  remain transient, and save version 1 gained no trainer-specific field.
- Retain the accepted Advanced Combat Drills Chronicle proof. One Elf Wizard
  world completed Mara's authoritative reward flow, ran one real Advanced transaction,
  equipped the quest Recruit Pack, and saved through the visible Chronicle at
  exactly level `2`, `175` XP, and `40` copper with completed quest state and
  untouched finite vendor stock. A genuinely fresh Dwarf Warrior world began
  at level `1`, `0` XP, and `40` copper before two confirmed Chronicle loads
  restored the exact Elf Wizard identity, base/equipment stats, Wizard hotbar,
  quest, stock, copper, and XP without reward replay, duplicate bonuses,
  transient trainer state, schema expansion, or drift.
- Retain the accepted player-facing Chronicle. `M` opens a centered `500x320`
  one-slot panel while `Ctrl+M` still toggles mouse inversion. A distinct
  `Current Journey` row reads the live possessed character; the separately
  labeled `Saved Journey | Local slot` inspects `EmbermerePrototype` without
  mutating it and displays its race/class above the accepted `Level 2`,
  `22 copper | 125 XP`, `2 bag stacks | 1 equipped | Quest complete` summary.
  Version `1` slots label their explicit Human Warrior legacy fallback.
  `Save Current` and `Load Saved` describe their direction explicitly and sit
  below the complete saved summary rather than sharing its vertical space.
  Save requires explicit overwrite confirmation when the slot exists; Load
  always confirms replacement of the current session. Empty, unreadable,
  unsupported-version, missing-asset, and other rejected states surface the
  persistence result and keep Load disabled or leave live state unchanged.
  Inventory, Vendor, and Chronicle remain mutually exclusive, and closing the
  panel restores game-only input. `EmbermereSave` and `EmbermereLoad` remain
  console fallbacks; there is still no autosave, deletion, multiple profiles,
  or implicit migration.
- Retain the accepted EverQuest-inspired target circle. Clean normal-route PIE
  proved all three Marsh Prowlers own exactly 48 stationary, non-colliding
  cyan-blue segments at 16 cm effective surface clearance. The visible circle
  leaves the selected Prowler's footprint readable, switches immediately with
  `Tab`, and clears from the old target. Do not retune it without a concrete
  normal-camera readability issue.
- Retain finite-world recovery. A sustained transform-proven `Q` autorun probe
  exposed that the player could leave the prototype foundation and fall
  forever. Crossing `Z=-1000` now cancels autorun, forces death even during
  recovery immunity, freezes movement, and uses the existing five-second
  village respawn. Fresh PIE proved full health at `(-2400, -1200, 90.15)`,
  `MOVE_Walking`, zero velocity, and the three-second protection message.
- In clean PIE, accept the first original Marsh Prowler as a complete gameplay
  presentation:
  - peat, charcoal, moss, mud, bone, and amber materials read at normal camera
    distance;
  - paws contact terrain and the capsule, target ring, and nameplate remain
    readable;
  - Idle, Walk, Run, Attack, Hit, and Death route from generic enemy state;
  - Tab targeting, Strike, real retaliation through player death/recovery,
    target clear, Marsh Tonic loot, death hold, hide, and respawn complete
    without changing combat or quest rules;
  - all three saved map instances retain the skeletal mesh and animation set,
    not only the Blueprint CDO.
- Recheck the completed grounding pass:
  - 53 grounded upright `FabPass_` actors and 24 original-art placements;
  - all ordinary art that previously sat at `Z=20` now rests at `Z=0`;
  - the unsupported SoulCave arch/pillar accents and three enemy marker meshes
    remain removed;
  - the project-owned 38-expression moss/earth material keeps its broad
    variation and `300` cm worn-road half width;
  - four 1,012-triangle visual-only marsh-reed clusters use the terrain material
    on their low footprint and remain `NoCollision`;
  - foliage, reeds, rocks, enemies, target rings, and nameplates leave the road
    and combat pocket readable.
- Walk from PlayerStart through Mara, the road gate, and the three Prowler
  homes. The reproduced straight-line `Q` contact was
  `Embermere_SupplyChest_Vendor_01`; its accepted transform is now
  `(-1740, -1180, 0)`, yaw `108`. Clean PIE moved beyond the old stop point,
  while the saved-map validator enforces a `225` cm spawn-corridor clearance
  and the live trace validator proves the player-height corridor clear.
  Preserve independent `W` and `S` autorun cancellation, physical `Ctrl+M`
  feedback, three clear gate lanes, and solo `525` cm pulls.
- Inspect `Embermere_FenwatchShelter_Mara_01` at `(-1740, -700, 0)`, yaw
  `-64`. Its 4,348-triangle open-sided silhouette must sit behind Mara, reuse
  all five road-family materials, keep four grounded supports solid, and leave
  its center and overhead roof clear. The old stone backdrop, market cover,
  vendor cube, and trainer cube must remain absent. A clean PIE route already
  moved from `(-2400, -1200, 90.15)` to `(-1793.188, -831.382, 90.15)` under
  `Q`, then froze at that exact transform after `W` cancellation.
- Inspect Mara's rigged project-owned keeper presentation. The deterministic
  source retains the accepted 3,280-triangle, `107.45 x 71.0 x 207.5` cm,
  six-material silhouette, adds nine authored bones with complete rigid
  weights, and supplies a 109-frame, 30-fps, exact 3.6-second Idle. Classic FBX
  adds one imported Armature root, so Unreal validates ten reference bones.
  The art-only `Embermere_FenwatchKeeper_Mara_Presentation_01` wrapper sits at
  `(-2050, -850, 0)`, yaw `135`, prefers the exact skeletal mesh and Idle,
  remains `NoCollision`, and retains `SM_EmbermereFenwatchKeeper_Mara_01` as its
  reversible fallback. The original `BP_QuestGiver` stays at
  `(-2050, -850, 140)`, yaw `35`, and remains the only owner of Mara's gold
  marker, display name, interaction, dialogue, quest, and rewards. Its dormant
  SCS visual keeps local `(0, 0, -140)`, yaw `100`, unit scale, and
  `NoCollision`, but deliberately has no render mesh. Fresh PIE kept the marker
  and silhouette readable while the Idle advanced from `0.333814` to
  `1.525603` seconds.
- Retain Mara's accepted contextual greeting observer. The quest data owns four
  short lines and the art-only wrapper only resolves/presents them while the
  player is within `420` cm: gold `!` available, cyan `...` active, green `?`
  ready to turn in, and muted green `+` completed. The fixed `320x56`
  screen-space panel sits at local `(0, 0, 235)`, remains hit-test-invisible,
  `NoCollision`, and contributes no navigation. Clean PIE proved the greeting
  hidden outside range, then exercised all four states. Physical `F` still
  accepted and completed the original Blueprint-owned quest for exactly
  `125` XP, `20` copper, and one Recruit Pack; a second `F` replayed no reward.
- Inspect `Embermere_FenwatchQuartermaster_Vendor_01` beside the accepted
  supply chest. Its 3,632-triangle, `120.842 x 93.0 x 217.0` cm static mesh
  uses six project-owned materials and should read as a stocky moss-capped
  merchant with apron, ledger, pouches, and keys. Retain `(-1530, -1190, 0)`,
  yaw `100`, unit scale, grounded feet, `NoCollision`, and clear Mara/chest/
  PlayerStart sightlines. The native `AEmbermereNpcPresentationActor` owns only
  interchangeable static/skeletal visuals and one shared transform; it must
  remain free of interaction, vendor, trainer, quest, and dialogue authority.
- Retain the quartermaster's accepted production skeletal lane. Its
  deterministic rig reuses the exact reviewed 3,632-triangle geometry and six
  materials, adds nine authored bones with complete rigid one-bone weights,
  and supplies a 121-frame, 30-fps, exact 4.0-second Idle. Classic FBX adds one
  imported Armature root, so Unreal validates ten reference bones while
  retaining every authored name. The saved wrapper prefers
  `SK_EmbermereFenwatchQuartermaster_01` and
  `A_EmbermereFenwatchQuartermaster_Idle`, remains `NoCollision`, and retains
  `SM_EmbermereFenwatchQuartermaster_01` as its reversible static fallback.
  Clean PIE kept `playing=true` while the animation position advanced from
  `0.853735` to `2.195707` seconds. The co-located vendor service still owns
  all interaction, stock, transaction, UI, and persistence authority.
- Inspect `Embermere_FenwatchVendorStall_Quartermaster_01` behind the
  quartermaster at `(-1530, -1430, 0)`, yaw `180`. The project-owned
  `394 x 242 x 306` cm module has 5,476 triangles, five shared Fenwatch
  materials, four solid support boxes, and one solid counter box. Its customer
  approach and east bypass must remain clear, the supply chest must stay
  accessible, and the co-located art-free service must still open Fenwatch
  Supplies through normal `F`. The 180-degree placement yaw is intentional:
  classic FBX mirrors Blender local Y, and the accepted rotation keeps the
  authored counter facing the quartermaster. `FabPass_Village_Fence_01` must
  remain absent.
- Inspect `Embermere_FenwatchCottage_West_01` at `(-2480, -260, 0)`, yaw
  `38`. Retain the grounded `580 x 422 x 503` cm, 6,616-triangle closed
  stone/timber/moss dwelling, five shared materials, exact two authored body/
  doorstep boxes, and project-owned tag. The body and step must block while
  roof, chimney, trim, windows, and crest remain visual-only. Preserve at least
  `650` cm PlayerStart-to-Mara route clearance, the west bypass, Mara's marker
  and contextual greeting, and the absent `FabPass_Village_Fence_02`.
- Inspect `Embermere_FenwatchArmsmaster_Trainer_01` beside the Fenwatch service
  cluster. Its deterministic rigged Blender source is a grounded
  `154.5 x 87.0 x 228.0` cm, 2,824-triangle model with six project-owned
  materials, nine authored bones, rigid one-bone weights, and a 3.2-second
  Idle. Classic FBX adds one imported Armature root, so the saved Unreal
  reference skeleton deliberately has ten bones while retaining all nine
  authored names and the authored `root` beneath that imported root. Retain
  `(-1320, -920, 0)`, yaw `100`, unit scale, `NoCollision`, readable shield/
  staff silhouette, clear route/marker sightlines, and the exact project-owned
  Idle in `AnimationSingleNode` mode. The original static mesh remains the
  reversible fallback. The co-located trainer service owns interaction and
  progression; the armsmaster owns none.
- Inspect `Embermere_FenwatchPracticeDummy_TrainingYard_01` at
  `(-1120, -1120, 0)`, yaw `45`. Its `252.0 x 100.879 x 245.0` cm,
  2,572-triangle silhouette reuses the five shared stone, moss, timber, iron,
  and ember materials. Keep the front target facing the armsmaster, the base
  and torso/core solid through exactly two authored boxes, both outstretched
  arms clear, terrain contact grounded, and the replaced
  `FabPass_Village_Crate_C` absent. It is world art, not trainer authority.
- Inspect `Embermere_FenwatchTrainingWorkshop_Armsmaster_01` at
  `(-690, -1030, 0)`, yaw `-100`. Retain its grounded
  `460 x 270.403 x 369` cm open-front silhouette, 5,624 triangles, five shared
  Fenwatch materials, and exact four authored collision boxes on the two front
  supports, rear wall, and workbench. Keep the center and player-height bay
  clear, the roof/trim/crest/tools visual-only, the armsmaster-to-dummy approach
  and road-side east bypass open, and `FabPass_Village_Fence_03` absent. The
  workshop is world art and owns no trainer, combat, or interaction authority.
- Inspect `Embermere_FenwatchNoticeBoard_Road_01` at
  `(-1560, -260, 0)`, yaw `-35`. Retain its grounded
  `286 x 93.927 x 277` cm silhouette, 3,684 triangles, five shared Fenwatch
  materials, and exact three authored collision boxes on the two supports and
  central panel. Keep the roof, notice cards, pins, trim, and ember crest
  visual-only. The board must remain presentation-only and free of quest,
  marker, dialogue, interaction, reward, vendor, trainer, and persistence
  authority while preserving the PlayerStart-to-Mara, village-to-road,
  quartermaster, and armsmaster routes.
- Retain the accepted communal-well art/service/presentation split. The static
  `Embermere_FenwatchCommunalWell_SouthCommons_01` remains removable
  presentation art with six purposeful curb/upright colliders and clear shaft,
  roof, rope, bucket, trim, and crest. The separate art-free
  `Embermere_FenwatchCommunalWell_RestService_01` exclusively owns the existing
  `F` interaction, `300` cm range, `1.5` second stay-still channel, `35` cm
  movement interruption, full Health/Mana transaction, and `12` second
  session-only cooldown. The separate
  `Embermere_FenwatchCommunalWell_RestPresentation_01` only observes immutable
  outcomes: twelve cyan segments remain inside the shaft during a live channel,
  and a committed result expands into a mint bloom for exactly `1.1` seconds.
  It is non-colliding, navigation-free, clears on interruption, death, unbind,
  expiry, or teardown, and cannot replay on load. Reject full resources, range,
  death, live enemy engagement, cooldown, duplicates, malformed data, and
  interruption without partial mutation. Practice-target selection is not
  combat. Preserve save
  version `2` and treat [Docs/REST_SERVICE_CONTRACT.md](Docs/REST_SERVICE_CONTRACT.md)
  as the recovery authority boundary and
  [Docs/REST_PRESENTATION_CONTRACT.md](Docs/REST_PRESENTATION_CONTRACT.md) as the
  observer boundary.
- Retain the accepted wrapper Idle lane. Anim Blueprint art takes precedence;
  otherwise a skeleton-compatible soft Idle asset uses `AnimationSingleNode`
  with data-driven loop and play rate. The production armsmaster is the first
  saved NPC to use that rigged lane: fresh package validation locks its exact
  mesh, Skeleton, 3.2-second Idle, loop/rate, transform, static fallback, and
  absence of collision or service authority. Clean PIE kept `playing=true`
  while the animation position advanced from `0.193888` to `1.670905` seconds.
  The quartermaster and Blueprint-backed Mara presentation now use the same
  accepted production lane with their own 4.0- and 3.6-second Idles and exact
  static fallbacks. Do not accept future NPC
  animation from serialized `AnimationData` alone: prove the live position
  advances.
- Retain the accepted HUD, effects, inventory, and equipment baseline:
  fixed timed-status cells, four-class ability art and semantics, the
  `700x330` paper-doll inventory, Recruit Pack bag/Back transactions, Marsh
  Tonic use, tooltips/comparison, Sort, the `236x62` drag token, cooldowns,
  clipped chat, and atomic full-bag rollback.
- Retain the asset-agnostic world-status presentation: eight small non-colliding
  segments subscribe to the same successful-effect snapshots as the HUD.
  Harmful auras now size from the transformed creature footprint and trace the
  supporting surface instead of orbiting the actor root. On a Marsh Prowler the
  accepted inner aura resolves to a `94.352` cm radius at `21` cm above the
  support hit. Snare is marsh green and Frost Root is a lighter ice-white cyan
  (`0.46, 0.92, 1.0`), leaving both distinct from the calm outer 48-segment
  cyan target circle. Harmful effects take visual priority and all segments
  hide when no timed effect is active or the character is dead.
- The imported KiteDemo meshes still reference some absent internal vendor
  materials/textures in fresh commandlet logs even though project-owned
  component overrides keep the visible zone readable. Do not edit or commit raw
  vendor packages. Prefer replacing affected meshes with project-owned Blender
  art or a complete signed-in Stylized Classic pack.
- Highest-value next work:
  1. retain the accepted Advanced Chronicle proof: deliberately different live
     and saved identities, one real Advanced transaction, exact version-3 owner
     restoration, and a second idempotent load with no transient trainer state;
  2. retain the accepted class-colored level-up world observer: twelve
     non-colliding segments, live-event-only activation, fixed `1.6`-second
     teardown, chat/HUD coexistence, and no replay on load. Sample Cleric,
     Ranger, and Wizard palettes in PIE only if a normal-camera issue is
     suspected; do not duplicate the authoritative event or progression math;
  3. retain the accepted practice-target loop in clean PIE: art/gameplay split,
     stationary transform, real hotbar damage, immediate target clear, exact
     three-second reset, and zero retaliation/reward/quest mutation;
  4. retain the accepted immutable combat-result and fixed floating-feedback
     presentation on both the practice target and saved Prowlers: exact applied
     amounts, three-entry cap, 1.25-second expiry, nameplate clearance, target
     switch/death/reset clearing, and unchanged chat/target-HP fallbacks;
  5. retain the accepted training workshop in clean PIE: complete open-front
     read, grounded supports, solid rear wall and bench, visual-only roof/tools,
     visible practice dummy, clear armsmaster approach, and road-side bypass;
  6. retain all three accepted production Idles from the normal village route:
     grounded feet, restrained motion, readable silhouettes, advancing clocks,
     clear markers/path, and unchanged quest, vendor, and trainer interactions;
  7. retain both accepted Chronicle proofs, including trainer-produced
     `30`-copper/`25`-XP state, slot inspection, confirmation, rejection,
     panel handoff, and two-session idempotence contracts; keep console commands
     as debug fallbacks and defer autosave, deletion, profiles, and migrations;
  8. retain Mara's accepted four-state contextual greeting and original real
     interaction loop; the wrapper may observe authority but must never own
     `F`, dialogue, marker, quest mutation, rewards, vendor, or trainer logic;
  9. tune Prowler timing or add subtle `NoCollision` marsh dressing only when
     normal-route PIE exposes a concrete issue;
  10. retain the accepted `Still Waters` content slice over version `3`: a
      separate art-free notice-board quest owner, one dedicated router for the
      rest service's committed `Success`, exact keyed progress/turn-in, and a
      two-quest double-load proof. The notice-board mesh, well art, rest
      service/presentation, and trainer remain quest-free. Retain both distinct
      quest-owned objective instructions in the fixed tracker and ledger detail.

## Full Manual Regression Checklist

- Restart Unreal before manual PIE when the editor predates the 2026-09-04
  quest-objective presentation module and saved quest packages, the 2026-09-03
  selected-quest detail module, the 2026-09-02 native Quest Ledger, the
  2026-09-01 `Still Waters`
  quest/service/router module and saved quest/map packages, the
  2026-08-31 save-version-3 multi-quest module, or the 2026-08-29 communal-well
  rest-presentation module, the 2026-08-28 rest-service module,
  the 2026-08-26
  level-up world-VFX module and 2026-08-25 level-gated trainer/progression
  module, character-identity persistence,
  rigged Fenwatch keeper, armsmaster, and
  quartermaster module and accepted
  skeletal-mesh/Skeleton/Idle/offerings/Chronicle/item/quest/stock packages.
  Current code passes all 91 tests.
- Verify the original Blender assets in clean-restart PIE:
  - find `Embermere_Waystone_Road_01` where the temporary road stump used to be;
  - approach it from the rune side and confirm scale, terrain contact, camera
    readability, and both-box collision without blocking the village route;
  - confirm its pale stone, moss accent, and ember rune remain readable in the
    current daylight alongside both matching ember lamps, the road signpost,
    and mixed Fab foliage;
  - inspect both ember lamps for scale, ground contact, warm crystal readability,
    two-box collision, and clear village/road traversal.
  - inspect the road signpost for readable directional silhouette, saved timber
    material, two-box base/post collision, and non-colliding overhead arms.
  - inspect the road gate for terrain contact, route framing, all five shared
    fantasy materials, a clear center opening, four support colliders, and a
    non-colliding overhead span.
  - inspect both boundary fences for terrain contact, all five shared materials,
    three colliders each, a solid crossed-rail body, and no intrusion into the
    road-gate opening;
  - inspect both rune-topped boundary stones for terrain contact, all five
    shared materials, two colliders each, readable ember crests, and clean
    termination of the fence silhouette after the south-side foliage cleanup.
  - inspect `Embermere_SupplyChest_Vendor_01` for its route-facing lock and
    plank silhouette, five shared materials, terrain contact, two solid authored
    body/lid boxes, accepted `(-1740, -1180, 0)` transform, and clear
    PlayerStart-to-Mara traversal.
  - inspect `Embermere_FenwatchShelter_Mara_01` behind Mara for all five shared
    materials, terrain contact, four solid support boxes, a clear center and
    roof span, readable quest marker/name, and an unobstructed spawn route.
  - inspect Mara's Fenwatch keeper for grounded feet, PlayerStart-facing pose,
    readable staff/satchel/moss-cowl silhouette, no collision, and an
    unobstructed gold quest marker and name.
  - inspect the Fenwatch quartermaster beside the supply chest for grounded
    feet, readable moss-cap/apron/ledger/pouch silhouette, no collision, a
    readable service marker, a clear route, and separation from Mara and her
    marker;
  - press `F` in range and run the vendor acceptance sequence from Start Here,
    including both successful buys, sold-out/insufficient-funds states, chat,
    item inventory changes, fixed panel copy, and close/input restoration.
  - inspect the rigged Fenwatch armsmaster for grounded feet, planted staff,
    restrained looping Idle, readable shield/staff silhouette, no collision,
    clear route separation, and its nearby service marker; press `F` and run
    the trainer acceptance sequence from Start Here, including the visible
    level-2 lock, `40 -> 30` copper / `0 -> 25` XP Combat Drills path, live
    Advanced unlock, exact `20` copper / `50` XP advanced transaction,
    insufficient-funds rejection, fixed panel bounds, chat, and Inventory/
    Chronicle/close handoffs.
  - inspect the Fenwatch practice dummy at `(-1120, -1120, 0)`, yaw `45`, for
    grounded contact, a target face aimed toward the armsmaster, five shared
    materials, a solid base and torso/core, clear outstretched arms, open route
    spacing, and the absence of the generic crate it replaced.
  - `Tab` the colocated native practice target and confirm the dummy gains a
    readable `Fenwatch Practice Target` nameplate plus cyan ring without the
    gameplay actor supplying art or collision. Strike it through defeat;
    confirm no retaliation, loot, XP, inventory, quest, trainer, or service
    mutation; selection clears; and full health returns after three seconds.
  - inspect the Fenwatch training workshop at `(-690, -1030, 0)`, yaw `-100`,
    for grounded stone-footed supports, an open front facing the training yard,
    readable bench/tool silhouettes, solid front posts/rear wall/workbench,
    clear roof/trim/tools, an open player-height bay, and unobstructed
    armsmaster-to-dummy and road-side bypass routes. Confirm
    `FabPass_Village_Fence_03` remains absent.
  - inspect the Fenwatch communal well at `(-950, -1600, 0)`, yaw `-135`, then
    damage Health and spend Mana before pressing `F` at its separate service.
    Confirm the fixed prompt, exact stay-still copy, full recovery after `1.5`
    seconds, exact chat amounts, full-resource rejection, movement/combat
    interruption, cooldown copy, clear shaft/decoration, and unchanged village
    routes. The static mesh must remain free of interaction or recovery
    components and the service must remain art-free and non-colliding.
- Confirm both first-class MCP servers after their host apps restart. Blender's
  bridge remains Safe Mode on, inline code off, localhost-only, and limited to
  approved project script roots.
- Confirm the local Fab/Epic folders are present but ignored by Git:
  - `Content/KiteDemo/`
  - `Content/SoulCave/`
  - `Content/ParagonProps/`
  - `Content/Scifi_desert_city/`
- Run these automation tests:
  - `Embermere.Combat.TargetSelectionPresentation`
  - `Embermere.Combat.DeadCasterRejected`
  - `Embermere.Combat.StarterAbilityEffects`
  - `Embermere.Enemy.LeashRules`
  - `Embermere.Enemy.LootRules`
  - `Embermere.Enemy.MarshProwlerPresentation`
  - `Embermere.Economy.FenwatchRewardsAndValues`
  - `Embermere.Equipment.SlotRules`
  - `Embermere.Equipment.InventoryTransactions`
  - `Embermere.Equipment.StatApplication`
  - `Embermere.Inventory.CapacityTransactions`
  - `Embermere.Inventory.ConsumableUse`
  - `Embermere.Inventory.IdentityActions`
  - `Embermere.Inventory.StableSorting`
  - `Embermere.Input.AutorunCancellation`
  - `Embermere.NPC.FenwatchKeeperPresentation`
  - `Embermere.NPC.FenwatchQuartermasterPresentation`
  - `Embermere.NPC.PresentationContract`
  - `Embermere.NPC.SkeletalIdlePresentation`
  - `Embermere.NPC.FenwatchArmsmasterPresentation`
  - `Embermere.NPC.FenwatchArmsmasterIdlePresentation`
  - `Embermere.Persistence.RoundTrip`
  - `Embermere.Persistence.CharacterIdentityRoundTrip`
  - `Embermere.Persistence.CharacterIdentityRollback`
  - `Embermere.Persistence.LegacyV1CharacterFallback`
  - `Embermere.Persistence.SlotInspection`
  - `Embermere.Persistence.ValidationRollback`
  - `Embermere.Player.OutOfBoundsRecovery`
  - `Embermere.Quests.CompletionRewards`
  - `Embermere.Rest.ServiceContract`
  - `Embermere.Rest.RecoveryTransactions`
  - `Embermere.Rest.InterruptionAndCombat`
  - `Embermere.Rules.RaceClassMatrix`
  - `Embermere.Stats.DamageImmunity`
  - `Embermere.UI.AbilityIconPresentation`
  - `Embermere.UI.ChatLog`
  - `Embermere.UI.EnemyNameplateWidget`
  - `Embermere.UI.HotbarCooldownDisplay`
  - `Embermere.UI.IconPresentation`
  - `Embermere.UI.InventoryDragDrop`
  - `Embermere.UI.InventoryToggle`
  - `Embermere.UI.ItemComparison`
  - `Embermere.UI.PaperDollPresentation`
  - `Embermere.UI.SaveLoadPanel`
  - `Embermere.UI.TimedStatusPresentation`
  - `Embermere.UI.TrainerPanel`
  - `Embermere.UI.WorldStatusVfxPresentation`
  - `Embermere.UI.VendorPanel`
  - `Embermere.Vendor.FenwatchStockData`
  - `Embermere.Vendor.ServiceContract`
  - `Embermere.Vendor.SellBuybackTransactions`
  - `Embermere.Vendor.TransactionRules`
  - `Embermere.Trainer.FenwatchOfferingsData`
  - `Embermere.Trainer.ServiceContract`
  - `Embermere.Trainer.TransactionRules`
- Manually verify the styled first-pass HUD in PIE:
  - player HP, mana, XP, health bar, and mana bar are visible;
  - target panel appears after `Tab` and shows target HP plus range state;
  - quest progress updates after accepting Mara's quest and defeating enemies;
  - all hotbar slots show stable labels for `1`, `2`, `3`, `4`, `Alt+R`, `Alt+E`, `R`, `X`, `E`, and `F`;
  - Mara interaction shows the dialogue panel;
  - the structured inventory window starts empty and shows its title, `Slots 0 / 24`, clickable item list, selected-item detail pane, quest-reward hint, and keyboard/mouse footer;
  - the inventory starts in cursor-aware game/UI mode; pressing `I` hides it, hides the cursor, and restores classic game-only mouse input;
  - clicking a populated row or pressing `[` and `]` changes the inspected inventory stack after multiple stacks exist;
  - hovering a populated row shows quantity, category, slot/level, effects, comparison, and description without expanding the fixed window;
  - clicking Sort groups weapon, armor, consumable, quest, and misc stacks in that priority, sorts names inside each category, preserves the inspected item occurrence, posts chat feedback, and is unavailable during drag;
  - dragging an item shows the fixed project-owned fantasy token with a category sigil, warm item name, and slot/level or consumable-effect context without overlapping the inventory footer or hotbar;
  - equippable item details show net HP, mana, armor, and power changes against the currently equipped item, or against an empty destination slot;
  - Mara's Recruit Pack identifies as level-1 Back armor with `+5 HP, +1 Armor`; equipping it removes it from the bag, updates the gold Back-slot control, bonus totals, HP to `105/105`, and chat; clicking the occupied Back slot returns it to the bag and restores base stats;
  - a full bag refuses unequip/replacement without losing, duplicating, or partially moving either item;
  - the 700x330 inventory/equipment window keeps all three columns and its footer inside the shaded panel without overlapping the hotbar;
  - defeating a Marsh Prowler drops one stackable Marsh Tonic, posts a loot message, and adds it to inventory;
  - after taking damage, selecting Marsh Tonic and clicking `Use` restores up to 25 health and 10 mana and consumes one tonic, while full resources disable use and preserve inventory;
  - quest completion/reward shows the loot popup and the inventory panel lists the reward item;
  - combat, target, quest, XP, inventory, mouse, cooldown, and death/recovery messages appear clipped as single-line rows inside the bottom-left chat/combat log instead of overlapping the top-left player status panel or spilling beyond the chat panel border.
- Manually verify prototype persistence in two clean PIE sessions:
  - confirm a legal non-default race/class, complete the exact vendor and Mara
    sequence from Start Here, equip one
    Recruit Pack in Back, open Chronicle with `M`, inspect the exact slot
    summary, cancel one overwrite request, then confirm Save Journey;
  - stop PIE, start a fresh PIE session, deliberately confirm a different legal
    race/class, open Chronicle, confirm Load Journey, and confirm the saved
    identity, class base stats, starter hotbar, `22` copper, `125` XP, exact
    bag/equipment identities and quantities, completed quest state, zero finite
    Recruit Pack stock, and correct equipment-derived stats;
  - load again and confirm no identity/stat drift, duplicate items, repeated
    quest payment, or doubled equipment bonuses;
  - confirm Chronicle, Inventory, and Vendor hand off visibility and cursor
    mode cleanly, and that `M`/`X` closes Chronicle back to game-only input;
  - confirm vendor buyback history is empty after load because version `2` keeps
    it session-only. Retain `EmbermereSave` and `EmbermereLoad` as debug
    fallbacks.
- Manually verify selected-target world readability in PIE:
  - `Tab` shows the selected enemy's UMG nameplate, selected marker, HP text,
    HP bar, and complete cyan-blue emissive circle around its footprint rather
    than buried inside the supporting surface or touching its paws;
  - the 48 overlapping segments remain restrained and stationary, follow a
    target switch immediately, and coexist with Snare/Frost Root world VFX;
  - the nameplate accent/HP bar changes toward red/orange as enemy HP falls;
  - switching targets clears the old enemy marker;
  - killing the selected enemy clears the target and marker.
- Manually verify the original Marsh Prowler presentation:
  - all three saved enemies use the project-owned skeletal mesh and retain the
    complete Idle, Walk, Run, Attack, Hit, and Death set;
  - peat/charcoal/moss/mud, bone, and amber materials read at gameplay distance;
  - paws contact terrain while the capsule, target ring, and nameplate remain
    clear;
  - targeting, Strike, retaliation, death, target clear, Marsh Tonic loot,
    hide, and respawn preserve the existing gameplay rules.
- Manually verify the live control fix in PIE:
  - `W`/`S` should cancel autorun.
  - `Ctrl+M` should toggle mouse Y inversion and show a temporary message.
  - Crossing below the prototype world at `Z=-1000` should cancel autorun,
    show `You have fallen`, freeze movement, and return the player to the
    village after five seconds at full health with zero residual velocity,
    walking restored, and three seconds of damage protection.
  - The fix now runs through the pawn/controller path known to receive live movement input, and the project builds successfully.
- Manually verify hotbar cooldown feedback in PIE:
  - Press `1` to use `Strike`.
  - Confirm the slot dims and displays its live cooldown countdown.
  - Press `1` again before cooldown expires and confirm the bottom-left chat/combat log shows a ready-time message.
  - Confirm non-empty failed hotbar activations show an `Unable to use ...` message instead of silently failing.
- Manually verify world-status VFX in PIE:
  - Battle Shout and Nature's Focus show eight restrained orange-gold segments
    around the player while their HUD status cell remains authoritative;
  - Ward uses the same fixed footprint with a blue-white palette;
  - Snare shows a grounded marsh-green inner aura on the affected Prowler and
    Frost Root uses a visibly lighter ice-white cyan;
  - both harmful auras resolve to the Prowler footprint (`94.352` cm in the
    accepted mesh/scale) and `21` cm above the support hit, staying inside the
    stationary outer cyan target circle without touching the paws;
  - harmful effects take presentation priority when both effect types overlap;
  - expiration, death, respawn, and status clearing hide the segments, while
    instantaneous Meditate creates no world aura;
  - the non-colliding presentation does not move the HUD, obscure the character,
    or become a gameplay authority.
- Manually verify the new Mara marker in PIE:
  - Mara should show a temporary gold `!` and name marker above her.
- Manually verify the first `FabPass_` environment layer in PIE:
  - the map has a blue atmospheric sky, readable ambient fill, and the
    38-expression moss/earth road material instead of the previous black
    sky/white or flat-green ground presentation;
  - all 53 remaining Fab art-pass actors are grounded and upright; all 23
    original placements remain present, including the vendor stall, practice
    dummy, closed cottage, training workshop, notice board, and four
    terrain-blended `NoCollision` marsh-reed clusters;
  - the three oversized sci-fi building shells, old Mara backdrop/market cover,
    and vendor/trainer cubes are gone; PlayerStart has an unobstructed view
    toward Mara in front of the Fenwatch shelter;
  - the unsupported SoulCave accents and redundant enemy marker meshes remain removed;
  - village props leave Mara, PlayerStart, and the quest interaction route readable;
  - the road dressing points naturally from the village toward the wilderness pocket;
  - foliage and rocks frame the enemy pocket without hiding enemies, target rings, or nameplates;
  - the upgraded ruin reads as a landmark and does not trap the player;
  - any sci-fi-kit props that read too futuristic are noted for replacement.
- Manually verify starter enemy leash behavior in PIE:
  - pull a Marsh Prowler away from the wilderness pocket;
  - confirm it drops aggro and returns home instead of chasing indefinitely into the village;
  - tune `LeashRadius`, `ReturnHomeRadius`, and `ReturnHomeSpeedCmPerSecond` if it feels too abrupt or too permissive.
- Manually verify player respawn protection in PIE:
  - let the player die near the enemy pocket;
  - confirm the recovery message mentions the short protection window;
  - confirm immediate follow-up enemy damage is blocked during the protection window.
- Run the editor smoke test in [Docs/PLAYTESTING.md](Docs/PLAYTESTING.md).
- If Codex needs editor access, start MCP inside Unreal:

```text
ModelContextProtocol.StartServer 8123
```

## Current Prototype Status

Embermere has a working first-pass starter slice:

- player spawn in `L_Embermere_Prototype`;
- classic mouse camera behavior;
- `WASD`, `Q` autorun, tab target, and hotbar ability input;
- first quest giver, quest data, and reward item data;
- a separated Fenwatch vendor vertical slice with art-only quartermaster,
  art-free interactable service actor, data-driven stock/prices, player copper,
  atomic buy/rollback rules, finite/unlimited stock, native fixed vendor UI,
  inventory/chat feedback, and saved-package validation;
- a separated Fenwatch trainer vertical slice with an art-only original rigged
  and Idle-animated armsmaster plus reversible static fallback, an art-free
  interactable service actor, data-driven offerings,
  level/currency/XP eligibility checks, rollback-safe progression, a fixed
  native training panel, chat feedback, saved-package validation, and an
  accepted two-world Chronicle proof for trainer-produced `30` copper/`25` XP;
- a versioned, atomic prototype save/load contract for copper, XP, inventory
  and equipment identity, a bounded keyed multi-quest ledger, and finite vendor stock, with stable
  asset/service identifiers, malformed-record rollback, session-only buyback,
  native version-3 records plus version-1/version-2 quest adapters,
  explicit `EmbermereSave`/`EmbermereLoad` commands, and fresh-session PIE
  round-trip proof;
- a fixed native Quest Ledger with eight stable rows, transient stable-ID focus,
  a bounded selected-record detail surface for quest-owned narrative/state copy
  and exact rewards, keyboard/mouse paths, and mutually exclusive peer-panel
  lifecycle without quest or persistence mutation;
- hostile starter enemies that aggro, chase, attack, die, and respawn;
- a separate art-free Fenwatch practice target colocated with the visible
  project-owned dummy: normal `Tab`/hotbar/nameplate/cyan-circle behavior,
  150 health, immediate target clear, three-second repeatable reset, and
  explicit exclusion from AI, collision, retaliation, loot, XP, quest credit,
  trainer authority, and persistence;
- starter enemy leash and return-home behavior for safer village/wilderness boundaries;
- player respawn protection for safer recovery during prototype combat;
- bottom-left chat/combat log feedback for targeting, combat, death, respawn, quest progress, XP, inventory, and rewards;
- temporary in-world interactable markers, including a gold quest marker for Mara;
- styled native HUD panels for player status, target, range state, quest progress, dialogue, loot, and hotbar labels;
- structured first-pass inventory window with capacity header, item rows, selected-item details, empty/reward states, `[`/`]` inspection cycling, and `I` show/hide toggle;
- a data-driven project-owned icon set with explicit Recruit Pack and Marsh
  Tonic art, ten equipment-slot symbols, sixteen starter-ability illustrations,
  category/slot/ability missing-art fallbacks, fixed UI dimensions,
  deterministic source generation, and durable Unreal texture packages;
- mouse-clickable inventory rows and cursor-aware game/UI input while the inventory is open;
- data-driven item categories, paper-doll equipment slots, level requirements, stat-bonus fields, and transactional Equip/Unequip actions;
- ten clickable equipment-slot controls, atomic bag/equipment transfers, rollback-safe replacement, idempotent health/mana/armor/power application, armor mitigation, and safe consumable depletion;
- item-row and occupied-slot tooltips plus net stat comparison against the current item or empty equipment slot;
- item-identity action routing plus typed bag/equipment drag payloads, valid and invalid drop feedback, bag-to-slot equip, and equipment-to-bag return on top of atomic transactions;
- explicit stable category/name inventory sorting that preserves selected item identity and duplicate-stack occurrence and refuses to reorder during a pending or active drag;
- data-driven Marsh Tonic consumables dropped by starter enemies so recovery is reachable through the normal combat loop;
- selected-target UMG nameplate widget plus a bounds-aware, surface-traced,
  non-colliding cyan-blue 48-segment emissive target circle with restrained
  pulse and deterministic clear/switch behavior;
- first-pass hotbar cooldown enforcement and ready-time feedback;
- live hotbar slot countdown and unavailable-state dimming;
- data-driven starter utility effects: timed Attack Power and Armor buffs,
  Snare and Frost Root movement control, Meditate mana recovery, effective-stat
  combat consumption, natural expiration, and respawn-safe effect clearing;
- asset-agnostic world-status VFX made from eight fixed non-colliding segments
  that read the same successful-effect snapshots as the HUD, with distinct
  Attack Power, Armor, Snare, and Frost Root palettes and no authority over
  gameplay rules;
- fixed hotbar ability illustrations with class-specific palettes, data-driven
  soft references, accessible stat/description tooltips, and missing-art
  fallback;
- first local Fab/Epic environment pass over the village, road, wilderness pocket, and ruin landmark;
- a project-owned Blender waystone, ember-lamp, road-signpost, road-gate,
  boundary-fence, boundary-stone, supply-chest, and notice-board family with reviewed scripts,
  editable source, FBX, previews, authored collision, and eleven validated saved
  placements, plus a solid-core, clear-arm Fenwatch practice dummy, an
  open-front Fenwatch training workshop, and four visual-only marsh-reed
  placements;
- the first project-owned rigged enemy, `SK_EmbermereMarshProwler_01`, with
  deterministic Blender source, 7,464 triangles, 26 authored bones, five
  materials, six imported animations, asset-agnostic runtime routing, three
  saved enemy instances, and placeholder fallback;
- a project-owned fantasy inventory drag token with category sigils, contextual item text, and fixed bounds;
- first Mac-friendly sky, ambient fill, fog-readability correction, and a
  38-expression project-owned moss/earth/path material;
- a measured grounding pass that lowered ordinary art to the real ground
  surface, removed unsupported suspended accents and enemy marker geometry,
  restored readable foliage overrides, and added four terrain-blended,
  `NoCollision` marsh-reed clusters;
- automation coverage for the race/class matrix, quest completion rewards,
  selected-target presentation, dead-caster rejection, starter ability effects,
  timed-status and world-status VFX presentation, Prowler presentation, enemy
  leash and loot rules, equipment slot/stat/transaction rules, atomic inventory
  capacity, consumable use, item comparison/tooltips, identity-based inventory
  and drag/drop actions, autorun cancellation, damage immunity, enemy nameplate
  widget, chat log, hotbar cooldown display, item/slot/ability/paper-doll
  presentation, inventory toggle, buy/sell/buyback transactions, saved
  economy values, service ownership, saved stock, native vendor-panel behavior,
  persistence round-trip/rollback rules, and construction-safe plus live NPC
  Idle presentation, plus trainer transactions, service ownership, offering
  data, static and rigged armsmaster presentation, and native trainer-panel
  behavior, plus focused practice-target policy and combat/reset coverage and
  immutable combat-result/fixed floating-feedback presentation, pre-play
  character creation, and versioned character-identity persistence, for 63
  authoritative tests.

## How Far We Have To Go

The prototype foundation is alive, but it is still early. The environment is
upright, spawn-safe, and readable, while inventory/equipment now has clickable
and draggable gear, project-owned drag/icon/paper-doll presentation, stable
sorting, hover inspection, item comparison, and lossless transactional RPG
rules rather than display-only state. Starter combat feeds inventory through
Marsh Tonic drops, closing the first damage-loot-recovery loop. Clean PIE has
verified the full quest, a real bag-to-Back drag, the icon-bearing reward
popup, all four hotbar palettes, timed root/snare/buff/mana-recovery behavior,
identity-preserving Sort, solo Prowler pulls, collision-cleared encounter
markers, targeting, the original-art road family, transform-proven `W`/`S`
autorun cancellation, and the empty-state paper-doll composition. Data-driven
player and target status rows now make timed effects inspectable and passed
clean-PIE review alongside occupied paper-doll contrast. Embermere now has its
first original rigged creature and a grounded moss/earth road treatment, so the
art lane has moved beyond props without taking ownership of combat rules. The
first world-status aura now consumes those same data-driven snapshots: clean
PIE accepted the restrained orange-gold beneficial presentation plus grounded
marsh-green Snare and ice-white cyan Frost Root segments around the Prowler.
Selected targets now use a surface-traced cyan-blue circle sized from their
visual footprint instead of a fixed rotating gold marker; clean PIE accepted the
Prowler read under daylight. The first closed Fenwatch cottage now gives the
service cluster a real dwelling silhouette while preserving Mara's greeting
and every accepted route. The open-front Fenwatch training workshop now frames
the armsmaster and practice dummy with a grounded architectural home while its
purposeful support/wall/bench collision leaves the training bay and road-side
bypass open. The reproduced Mara-route contact is also resolved
by moving the supply chest away from the straight
PlayerStart corridor and proving both geometric clearance and a live native
trace. The populated drag token in motion and physical `Ctrl+M` feedback remain
honest physical-eye checks. The world remains stylistically mixed and still
needs a complete village-building family, player/race art, authored Niagara/
class-specific effects, and audio.

The first durable-state lane is now complete as a bounded prototype contract:
confirmed race/class identity, wallet, XP, inventory/equipment identity, quest
state, and finite vendor stock survive a fresh PIE session through validated
atomic restore. Version `2` uses stable semantic race/class IDs and rebuilds
base stats plus the starter hotbar before equipment and progression, while
version `1` remains loadable through an explicit Human Warrior compatibility
interpretation. Buyback and combat-temporary state remain deliberately
transient. Chronicle now provides the deliberate player-facing lifecycle,
read-only identity summary, and corruption/version feedback. Keep the bounded
one-slot contract stable instead of adding serialized fields by accident.
Trainer-produced progression has now passed the same lifecycle:
`30` copper and `25` XP restored exactly across a fresh world and a second
idempotent load without adding trainer-specific schema.

Combat readability now has the same ownership discipline. Abilities publish
one immutable result only after committing their outcome, and a standalone
native observer briefly renders exact applied damage beside the target's world
nameplate. Clean PIE accepted the same fixed-size `28` result on the art-free
practice target and a saved Marsh Prowler, with rapid-entry bounds, expiry,
deselect and lethal clearing, cyan-circle coexistence, and unchanged chat plus
target-frame fallbacks. The presentation does not own health, hit resolution,
targeting, rewards, AI, quests, or persistence.

## Next Work

- Define and implement one restrained presentation-only quest update observer
  over authoritative committed quest changes:
  - define the event and authority boundary before UI work. Quest owners and
    `QuestStates` must remain the only acceptance, progress, ready, completion,
    reward, and focus authorities; the observer may consume immutable results
    only after their mutation commits;
  - cover accepted, exact progress, ready-to-turn-in, and completed outcomes
    with stable quest-owned title/objective copy and exact counts. Keep chat,
    the compact tracker, contextual greetings, and the Quest Ledger as durable
    fallbacks;
  - use one fixed short-lived native surface with deterministic bounds and
    lifetime, a bounded rapid-update policy, hit-test-invisible behavior, and
    no movement of HUD, hotbar, chat, target, Chronicle, or ledger geometry;
  - clear safely on expiry, world teardown, death, and load. Silent restore
    must never replay acceptance, progress, ready, completion, or reward
    presentation;
  - add focused automation for exact post-commit payloads, wrong-ID rejection,
    duplicate/cap behavior, rapid updates, fixed geometry, expiry/teardown,
    peer-panel coexistence, silent load, and zero wallet/XP/item/reward drift;
  - in clean PIE, accept both quests, advance Mara once through real Prowler
    combat, complete one real Still Waters rest/turn-in, and judge each update
    at normal camera distance without obscuring existing world or HUD feedback.
  Do not add abandonment, sorting, map tracking, reward claims, another quest,
  audio, or persistence fields in this slice. If the runtime cannot expose one
  trustworthy immutable post-commit result without broad mutation changes,
  land the reviewed contract and smallest test-backed event seam first.
- Retain Mara's accepted contextual greeting contract in clean PIE:
  - outside `420` cm the observer must be hidden; inside range it must show the
    quest-owned available, active, ready, or completed copy;
  - the fixed `320x56` panel must fit beneath Mara's gold marker/name without
    shifting the HUD or intercepting mouse input;
  - physical `F` must remain owned by `Quest_Giver_Mara_Fenwatch`, with exact
    acceptance, `3/3`, completion, reward, and no-replay behavior;
  - removing or disabling the wrapper presentation must not change quest
    results.

- Retain the accepted rigged Fenwatch armsmaster in clean-restart PIE:
  - inspect grounded feet, planted staff, shield silhouette, restrained Idle,
    clear service marker, and open training-yard/village routes;
  - prove the exact 3.2-second Idle remains playing and advances while the
    wrapper remains `NoCollision` and the static fallback remains available;
  - run the normal trainer and Chronicle flows to prove the art upgrade cannot
    mutate progression, interaction, or persistence authority.
- Keep the accepted practice-dummy and practice-target split at
  `(-1120, -1120, 0)`, yaw `45`: visible art retains its two colliders, five
  materials, target-facing composition, clear arms, and absent generic crate;
  the colocated native gameplay actor remains art-free, stationary,
  non-colliding, resettable, and reward-free while reusing normal combat
  presentation.
- Retain the accepted training-workshop contract at `(-690, -1030, 0)`, yaw
  `-100`, with five shared materials, four purposeful colliders, a clear
  open-front bay, visible dummy, open armsmaster approach and east bypass, and
  absent generic fence. Combat behavior must remain removable from both the
  dummy and workshop meshes.
- Retain the accepted combat-feedback slice over the target contract: exact
  post-mitigation floating damage, fixed size and lifetime, three-entry rapid
  cap, shared Prowler/practice-target behavior, and strict presentation-only
  ownership. Keep bottom-left chat and target HP as durable fallbacks; do not
  publish `MISS` until an authoritative hit-resolution rule actually produces
  that outcome.
- Retain the accepted Fenwatch notice board as world art: exact metrics,
  materials, three purposeful colliders, decorative clearance, grounded road
  composition, and four protected routes. Do not make the mesh itself a quest,
  interaction, vendor, trainer, reward, or persistence authority.
- Retain `Embermere_FenwatchCommunalWell_SouthCommons_01` as presentation-only
  world art at `(-950, -1600, 0)`, yaw `-135`: exact `348 x 220 x 322` cm
  bounds, 6,760 triangles, five shared Fenwatch materials, six authored boxes
  on the four curb sections and two uprights, and `NoCollision` roof, axle,
  rope, bucket, crank, trim, and crest. Preserve the open shaft read, grounded
  contact, all seven protected neighbor spacings, and the PlayerStart-to-Mara,
  village-to-road, quartermaster, vendor-bypass, armsmaster, and workshop
  circulation traces. The art owns no quest, interaction, reward, service, or
  persistence behavior.
- Retain the accepted save version `3` contract: stable
  race/class IDs, current-rules legality validation, atomic class-stat and
  starter-hotbar restoration, repeated-load idempotence, read-only Chronicle
  identity, the bounded keyed quest ledger, malformed-record rollback, the
  explicit version `1` Human Warrior fallback, and version-1/version-2 singular
  quest adapters. Do not add appearance, naming, autosave, multiple profiles,
  deletion, or implicit migration without another deliberate versioned contract.
- Retain Chronicle's accepted two-state presentation: use a deliberately
  different live identity and saved identity, verify both labels and levels,
  keep the final bag/equipment/quest summary line fully above the action row,
  and do not overwrite or load the user's slot during a visual-only check.
- Retain the accepted data-driven level progression contract: level derives
  only from durable XP, the first cap remains level `5`, race/class growth is
  rules-owned, equipment remains additive once, and save/load remains silent,
  atomic, idempotent, and compatible with version `1` and `2` slots.
- Retain the accepted progression-presentation milestone:
  - verify the fixed gold XP bar reads `25 / 100` after Combat Drills and the
    top-center level-up observer reads `LEVEL 2` plus `150 XP | Next: 250 XP`
    after Mara's quest without overlapping loot, Chronicle, target, or chat;
  - verify the panel expires without layout movement and a confirmed Chronicle
    load restores the steady XP surface without replaying celebration;
  - keep the Stats snapshot, live-only level event, exact chat fallback, fixed
    geometry, multi-level/cap behavior, and every existing authority boundary.
- Retain the accepted art-free communal-well rest service: exact data ID and
  transform, `300` cm range, `1.5` second channel, `35` cm movement threshold,
  full Health/Mana atomic commit, `12` second session cooldown, fixed prompt
  and chat, every rejection/rollback edge, and zero art or persistence
  authority. Retain the separate accepted presentation observer: twelve fixed
  cyan shaft segments while channeling, one `1.1`-second mint committed bloom,
  no collision/navigation or gameplay mutation, immediate interruption/death/
  teardown clearing, and no load replay. Accepted `Still Waters` observes
  committed rest through its own dedicated router and never becomes another
  responsibility on the well, its service, or its presentation observer.
- Retain the accepted trainer Chronicle proof under save version `3`:
  - train once from fresh state to `30` copper plus `25` XP;
  - save deliberately, begin fresh PIE, load, then load again;
  - prove exact restoration, idempotence, and no item, equipment, quest,
    vendor-stock, buyback, reward, or schema mutation;
  - retain console commands as debug fallbacks and keep autosave, deletion,
    profiles, migration, and transient trainer-panel state out of scope.
- Retain the accepted persistence/economy loop without weakening its boundaries:
  - preserve Chronicle's one-slot inspection, confirmation, cancel, rejection,
    and panel-handoff behavior over the proven atomic contract;
  - keep buyback history session-only and document any future schema migration
    before changing `EmbermereSaveGameVersion::Current`;
  - retain one-at-a-time vendor quantities, the fixed vendor panel,
    visual/service split, stable IDs, and atomic transaction/load invariants.
- Replace temporary selected-target text with better world readability:
  - retain the UMG nameplate widget, selected marker, HP bar, and HP-aware
    accent color;
  - retain the accepted complete cyan-blue target circle on all three Prowlers,
    then add rune/soft-edge texture art only if it improves rather than obscures
    the classic target read;
  - tune screen-space widget size/height against camera distance.
- Improve inventory presentation:
  - after restarting Unreal, visually verify all ten empty equipment-slot icons,
    Recruit Pack and Marsh Tonic icons in rows/detail/occupied slots, fixed icon
    dimensions, text fallback/tooltips, category fallback, and no layout shift;
  - verify the 700x330 bag/detail/equipment layout, all ten slot controls, Recruit Pack bag-to-Back transfer, gold occupied state, slot-click unequip, stat changes, and full-bag failure message in PIE;
  - manually verify mouse row/action clicks, cursor capture/release, and bracket-key item cycling once inventory has multiple stacks;
  - verify Marsh Prowler deaths grant Marsh Tonic, repeated drops stack, and `Use` heals damaged players without wasting a full-resource tonic;
  - visually verify item comparison lines and row/occupied-slot hover tooltips after a clean restart;
  - visually verify bag-to-equipment and equipment-to-bag drag/drop, valid/invalid feedback, stale identity rejection, and click/keyboard fallbacks;
  - visually verify stable identity-preserving sorting and the new fixed-size fantasy drag token across armor and consumable payloads;
  - retain item-art reuse in the fantasy drag token and reward popup, then
    visually approve the `128x160` illustrated paper-doll backdrop beneath
    empty and occupied slots without contrast or layout regressions.
- Finish starter-class combat identity:
  - retain all sixteen accepted ability icons, class palettes, data-driven
    tooltips, and cooldown dimming/countdown without layout shift;
  - play each timed effect through normal input and judge duration, feedback,
    damage cadence, and enemy recovery from the player's perspective;
  - visually accept the new fixed player/target status rows, live countdowns,
    hover descriptions, duplicate prevention, and clearing rules;
  - retain the asset-agnostic eight-segment world aura, including grounded
    footprint-aware harmful presentation, before deciding whether authored
    Niagara or class-specific effects are warranted.
- Clean up WIP HUD layout issues:
  - manually verify the 2026-07-04 chat clipping fix in PIE after a clean editor restart;
  - continue tuning chat panel height/line count against the hotbar and common desktop viewport sizes.
- Manually review the first Fab/Epic environment pass in PIE:
  - verify the atmospheric sky, ambient fill, and fog balance on the Mac viewport;
  - confirm the removal of the oversized sci-fi shells leaves Mara and PlayerStart readable;
  - tune the validated sky/ambient/moss-ground balance only if clean-restart PIE exposes a readability problem;
  - confirm the dressed road still leads clearly from spawn to Mara and the enemy pocket;
  - confirm the wilderness foliage/rocks do not hide starter enemies or target nameplates;
  - confirm the upgraded ruin reads better than the old greybox and does not trap the player;
  - identify any sci-fi props that look too off-theme for the starter village.
- Continue the proper Stylized Classic village-building family from the
  accepted closed cottage now that the oversized sci-fi shells are gone.
- Expand the proven original Blender-to-Unreal lane from the waystone, ember
  lamps, road signpost, road gate, boundary fences, end stones, and supply chest
  into compact village pieces while the accepted creature lane remains
  independently reversible.
- Polish the accepted Marsh Prowler only from concrete PIE feedback: attack
  timing, action transitions, material balance, silhouette, physics bodies,
  target-ring/nameplate clearance, and respawn presentation.
- Retain the completed measured grounding baseline from
  `Docs/GROUNDING_AND_TERRAIN_PASS.md`; replace incomplete vendor meshes with
  project-owned or complete signed-in art rather than masking real placement or
  dependency errors with more foliage.
- Preserve the collision-cleared 525 cm solo-pull baseline while tuning leash,
  return-home speed, damage, and respawn timing only from normal-route PIE.
- Tune player respawn delay, protection duration, and recovery rules after in-editor playtesting.
- Keep automation coverage growing around cooldowns, death/respawn, targeting, and hotbar behavior.
- Keep world-status VFX subscribed to gameplay-owned status metadata; polish
  scale, color, or motion only from normal-camera feedback.

## Last Completed

- 2026-09-04 Astra evaluation: implemented copied live quest-update results
  and a fixed presentation-only observer, plus a deterministic Blender
  journal emblem and saved Unreal UI texture. Final isolated automation
  passed `91/91`; the sequential `21` package validators and all six live
  route/collision validators passed. No save schema or map placement changed.
  The final editor rediscovered 91 tests through first-class MCP. Added a
  candid evidence report and unpublished field-note draft. After local unlock,
  normal Q/W approach and F acceptance passed. Fixture-backed progress/ready,
  real F rewards, completion/level-up separation, and Inventory suppression
  were inspected. Full Prowler/Still Waters routes and held mouse/modifier
  checks remain explicit gates; objective injection is not combat proof.

- 2026-09-04: added quest-data-owned `ObjectiveInstructions` and authored exact
  `Defeat 3 Marsh Prowlers.` and `Complete a rest at the communal well.` copy
  into the two saved quest packages. The fixed compact tracker now reads title,
  exact progress, and instruction inside corrected `260x68` clipped bounds;
  the unchanged selected detail appends the same read-only metadata.
- Added the stable `Objective details unavailable.` fallback and
  `Embermere.UI.QuestObjectiveDisplayPresentation`, bringing the suite to `88`.
  The no-hot-reload build, isolated `88/88`, focused package validator,
  sequential 20-package aggregate, full-zone validation, and initialized-world
  route/collision traces passed with no Python errors.
- Clean PIE physically accepted Mara and Still Waters, displayed both exact
  instructions, and selected Mara through Up while Still Waters remained the
  compact focus. The fixed `620x550` ledger, eight `596x30` rows, `596x120`
  detail region, records, rewards, and save version `3` remained unchanged.

- 2026-09-03: extended the native Quest Ledger once to fixed `620x550` bounds
  with one `596x120` selected-record region for quest-owned narrative/state
  copy, objective progress, and exact XP/copper/optional-item rewards. Selection
  refreshes details only; explicit stable-ID focus remains separate.
- Added `Embermere.UI.QuestLedgerDetailsPresentation` and
  `Embermere.UI.QuestLedgerDetailLifecycle`, bringing the suite to `87`. The
  no-hot-reload build, isolated and restarted-editor suites, 19-package
  aggregate, full-zone validation, and clean two-quest PIE gate passed.

- 2026-09-02: implemented and accepted the fixed native player-facing Quest
  Ledger over the save-version-3 keyed quest collection. The centered
  `620x430` panel reserves eight stable `596x30` rows, exact active/ready/
  completed state and progress, a fixed empty state, and a bottom-right
  `140x38` command eight pixels above Chronicle.
- Added dedicated native row buttons, local selection, Up/Down wrapping,
  stable-ID Enter/button focus, duplicate-focus safety, `J` toggle, and
  controller-owned exclusive handoff with Inventory, Vendor, Trainer, and
  Chronicle. Focus remains session-only and can change only the compact
  compatibility projection.
- Added `Embermere.UI.QuestLedgerPresentation` and
  `Embermere.UI.QuestLedgerFocusLifecycle`, bringing the suite to `85`. The
  no-hot-reload build, isolated commandlet `85/85`, restarted-editor MCP
  `85/85`, six focused validators, fresh 19-package aggregate, full-zone
  validation, and all initialized-world route traces passed.
- Clean PIE completed Still Waters through real Prowler damage, physical well
  `F`, and physical notice-board turn-in while Mara remained active. The ledger
  showed Mara `ACTIVE 0/3` beside Still Waters `COMPLETED 1/1`, switched the
  compact tracker both ways, and preserved exact records and one-time rewards
  through `I`, `J`, and `M` peer-panel handoffs.

- 2026-09-01: implemented and accepted `Still Waters`, Embermere's first real
  parallel quest over save version `3`. `DQ_FenwatchStillWaters` owns stable
  IDs `FenwatchStillWaters/FenwatchRestCompleted`, one committed-rest
  requirement, four-state copy, `50` XP, `10` copper, and no item reward.
- Added the separate art-free
  `Embermere_FenwatchNoticeBoard_StillWatersService_01` at the accepted notice
  board plus `UEmbermereRestQuestObjectiveRouterComponent`. The quest owner
  alone offers and turns in; the router forwards only immutable committed rest
  `Success`. The notice-board mesh, well art, rest service/presentation, and
  trainer remain removable and quest-free. Objective-cap rejection also blocks
  duplicate post-completion progress.
- Added `Embermere.Quests.StillWatersServiceContract`,
  `Embermere.Quests.StillWatersRestRouting`, and
  `Embermere.Persistence.StillWatersRoundTrip`, bringing the suite to `83`.
  The no-hot-reload build, isolated commandlet `83/83`, restarted-editor MCP
  `83/83`, focused validators, fresh 19-package aggregate, full-zone validation,
  and initialized-world well/board/workshop/cottage/stall/road traces passed.
- Clean PIE physically accepted `Still Waters`, independently accepted Mara,
  proved a moved rest cannot advance progress, committed one real rest to
  `1/1`, turned in exactly `50` XP and `10` copper once, and rejected replay.
  An isolated `EmbermereStillWatersLiveProbe` slot then restored completed
  `Still Waters` plus active Mara, exact XP/copper, and both quest records on
  two loads before deleting itself and restoring the real Chronicle target.

- 2026-08-31: implemented the accepted save-version-3 multi-quest foundation.
  `UEmbermereQuestLogComponent` now owns a bounded eight-record keyed ledger;
  objective progress, completion, and reward commit target an exact stable
  quest/objective pair. `ActiveQuest` remains only a derived compatibility
  projection and `FocusedQuestId` remains transient.
- Version `3` serializes a stable quest-record array. Load resolves and
  validates the complete candidate before one atomic replacement; duplicate,
  missing, mismatched, invalid-progress, contradictory, mixed-format, and
  over-capacity records reject without partial mutation. Versions `1` and `2`
  adapt their zero-or-one singular record through the same validation path and
  are never rewritten.
- Replaced generic Prowler objective credit with exact Mara quest routing and
  made Mara's contextual observer query her stable quest ID instead of the
  transient focused projection. Wrong-giver turn-in cannot complete a different
  ready quest, and reward preflight remains exactly-once and rollback-safe.
- Added `Embermere.Quests.MultiQuestRuntime`,
  `Embermere.Persistence.MultiQuestRoundTrip`,
  `Embermere.Persistence.LegacyQuestCompatibility`, and
  `Embermere.Persistence.MultiQuestValidationRollback`, bringing the suite to
  80. The no-hot-reload build, isolated commandlet `80/80`, restarted-editor MCP
  `80/80`, focused validators, fresh 18-package aggregate, and all initialized-
  world route traces passed with no Python errors. Clean PIE retained Human
  Warrior creation, Inventory close, measured Q/W controls, and physical Mara
  acceptance; live inspection showed the exact keyed quest record and focus.

- 2026-08-30: traced the proposed second Fenwatch quest through the live and
  persistence owners and proved that save version `2` is intentionally a
  single-quest format. Added `Docs/MULTI_QUEST_CONTRACT.md` for the deliberate
  version-3 keyed ledger, legacy adapters, atomic validation/rollback, and the
  future `Still Waters` rest-success slice without moving quest authority into
  the notice board, well, rest service/presentation, or trainer.
- Replaced the ambiguous accept-failure turn-in fallback with typed quest
  acceptance results. Only the exact tracked quest may attempt completion; a
  different offer reports the occupied slot and cannot replace or complete
  Mara's quest or mutate rewards.
- Added `Embermere.Quests.SingleSlotCompatibility`, bringing the suite to
  77. The no-hot-reload build, isolated commandlet `77/77`, restarted-editor
  MCP `77/77`, five focused validators, and fresh 18-package aggregate passed
  with no Python errors. Clean PIE retained Human Warrior startup, Inventory
  handoff, Q movement, W cancellation, and physical-`F` Mara acceptance. The
  world remains 53 grounded Fab actors plus 24 original-art placements, and
  save version `2` remains unchanged.

- 2026-08-29: added the separate native
  `Embermere_FenwatchCommunalWell_RestPresentation_01` as a removable observer
  over immutable rest outcomes. It lazily creates twelve project-material
  segments: a restrained cyan channel remains inside the open well shaft, and
  only a committed service success produces the fixed `1.1`-second mint bloom.
  The observer owns no interaction, eligibility, timing, cooldown, Health/Mana
  mutation, chat, quest, reward, or persistence state.
- Added `Embermere.UI.RestWorldPresentation` plus focused saved-map validation.
  Automation locks deferred geometry, fixed bounds, non-collision/navigation,
  duplicate safety, success-only completion, interruption/death/unbind/expiry/
  teardown clearing, and the existing art/service authority split. Clean PIE
  visually accepted the real cyan channel and mint completion at normal camera
  distance while the service alone restored exactly `35 Health` and `20 Mana`.
- The no-hot-reload Mac build, all 76 isolated tests, focused validators, fresh
  18-package aggregate, full-zone validation, and initialized-world traces
  passed. The map remains at 53 grounded Fab actors plus 24 original-art
  placements, and save version `2` remains unchanged.

- 2026-08-28: added the separate native
  `Embermere_FenwatchCommunalWell_RestService_01` and data asset beside the
  accepted static well. The art-free service owns the existing `F` lane,
  complete preflight, a `1.5` second stationary channel, `35` cm movement
  interruption, exact full-Health/full-Mana atomic recovery, fixed outcome
  feedback, and a `12` second session-only cooldown. The mesh remains removable
  art with no interaction, recovery, quest, reward, combat, or save authority.
- Added `Embermere.Rest.ServiceContract`,
  `Embermere.Rest.RecoveryTransactions`, and
  `Embermere.Rest.InterruptionAndCombat`. The suite covers full and one-vital
  recovery, range, death, enemy engagement, full resources, cooldown,
  duplicate requests, malformed data, movement, practice-target exclusion,
  teardown, exact amounts, and no partial mutation. Clean PIE used real `F` to
  restore `30` Health and `20` Mana and then rejected a full-resource repeat.
- The no-hot-reload Mac build, all 75 isolated tests, four focused validators,
  fresh 17-package aggregate, full-zone validation, and initialized-world well
  traces passed with exact markers and no `LogPython: Error`. The map remains
  at 53 grounded Fab actors plus 24 original-art placements and save version
  `2` remains unchanged.
- 2026-08-27: built `SM_EmbermereFenwatchCommunalWell_01` through the reviewed
  deterministic Blender and classic-`FbxFactory` lane. The grounded
  `348 x 220 x 322` cm mesh has 6,760 triangles, one UV channel, clean
  topology, five shared Fenwatch materials, and six authored curb/upright
  collision boxes; roof, axle, rope, bucket, crank, trim, and crest remain
  decorative and non-colliding.
- Explicit import saved the mesh and map and placed
  `Embermere_FenwatchCommunalWell_SouthCommons_01` at
  `(-950, -1600, 0)`, yaw `-135`. Focused package validation, full-zone
  validation, and initialized-world traces locked its provenance, exact
  metrics, transform, art-only tag, collision intent, protected neighbor
  spacing, and six clear village/service circulation lanes. Clean PIE accepted
  the grounded south-commons composition; Q moved from `(-2400, -1200)` to
  `(-1793.188, -831.382)` and W stopped all later autorun movement.
- The no-hot-reload Mac build, all 72 isolated automation tests, focused
  trainer validator, standalone progression validator, and fresh 16-package
  aggregate passed with exact success markers and no `LogPython: Error`. The
  current map baseline is 53 grounded Fab actors plus 24 original-art
  placements.
- 2026-08-26: completed the deliberate Advanced Combat Drills persistence gate
  through the real Chronicle UI. An Elf Wizard completed Mara's owner-driven
  reward, trained once through Advanced, equipped the Recruit Pack, and saved
  exact level-2 `175` XP / `40` copper state. A fresh Dwarf Warrior world then
  loaded that slot twice, restoring exact identity, Wizard hotbar, base and
  equipment stats, completed quest, untouched finite stock, copper, and XP
  without reward replay, transient trainer serialization, duplication, or
  drift.
- Added a removable class-colored world observer to the existing post-commit
  live level event. It lazily creates twelve project-material segments, expands
  and rotates for `1.6` seconds, owns no progression state, collision, or
  navigation, and cannot replay on silent load. Clean PIE accepted the
  orange-gold Warrior treatment after four real Combat Drills transactions;
  tilting the live first-person view confirmed grounded readability and the
  normal Inventory-close handoff while the effect was active.
- Added `Embermere.UI.LevelUpWorldVfxPresentation` plus tracked live validation
  helpers. The no-hot-reload build, all 72 tests, focused trainer validator,
  standalone progression validator, and full 15-package aggregate passed with
  exact markers and no `LogPython: Error`, retaining 53 grounded Fab plus 23
  original-art actors.
- 2026-08-25: expanded `DA_FenwatchArmsmasterOfferings` from one lesson to two
  stable-ID, data-driven, repeatable lessons. Combat Drills remains level 1 at
  `10` copper for `25` XP; Advanced Combat Drills is level 2 at `20` copper for
  `50` XP. Complete offering validation now rejects malformed or duplicate
  IDs, and the service owns level, currency, overflow, repeatability, atomic
  spend/grant, and refund preflight while UMG remains a selector.
- The fixed `500x300` panel keeps both rows readable. Advanced remains visible
  and inspectable at level 1 with exact rejection and zero mutation, becomes
  ready from the live Stats-derived level without reopening the panel, and no
  longer retains stale lock copy after progression changes. Clean PIE accepted
  a real Advanced transaction from `50` copper / `100` XP to `30` / `150`.
- Added `Embermere.Trainer.LevelGatedProgression` and
  `Embermere.Trainer.LevelGatedPersistence`. The no-hot-reload build and all
  71 tests passed; the focused trainer validator, standalone progression
  validator, and full 15-package aggregate emitted exact success markers with
  no `LogPython: Error`, retaining 53 grounded Fab plus 23 original-art actors.
- 2026-08-24: clarified Chronicle's live-versus-saved state after a real Dwarf
  Warrior session correctly inspected an older Elf Wizard slot but presented
  the slot as though it were the current character. Chronicle now shows a gold
  `Current Journey | Dwarf Warrior | Level 1` row and a separate `Saved
  Journey | Local slot` section whose existing `Elf Wizard | Level 1` identity
  remains read-only until the player deliberately saves or loads.
- Enlarged the centered panel to `500x320`, reserved a fixed `460x78` saved-
  summary region plus a 12-pixel action gap, and renamed the controls `Save
  Current` and `Load Saved`. Clean PIE reproduced the mismatched identities
  without touching the save and confirmed the final bag/equipment/quest line
  no longer overlaps either button. The no-hot-reload build and all 69 tests
  passed.
- 2026-08-24: fixed the post-character-creation mouse lock by making the
  controller own one idempotent move/look suppression state. `OnPossess` and
  `BeginPlay` can both request the modal without stacking Unreal's reference-
  counted ignore-input calls, and confirmation now releases the owned lock
  exactly once.
- Returning from Inventory now uses game-only input without consuming the
  first capture mouse-down. Clean PIE confirmed the exact Dwarf Warrior ->
  close Inventory path with move/look enabled and the cursor hidden.
- Moved the larger `140x38` Chronicle command to the bottom-right with a fixed
  `24`-pixel margin. It remained clear of the open top-right Inventory, hotbar,
  and chat in the live viewport. The no-hot-reload build and all 69 tests
  passed in a fresh commandlet.
- 2026-08-24: added a Stats-owned progression presentation snapshot with
  derived level, total XP, current/next threshold, explicit cap state, and
  normalized within-level progress. UMG consumes the result without owning a
  threshold table or level calculation.
- Added a fixed `260x8` gold XP bar and next-threshold copy to the player status
  panel, plus a separate hit-test-invisible `360x76` live level-up observer with
  a `2.75`-second lifetime, multi-level/cap copy, fade, replacement, and safe
  teardown. Save restoration emits no level-transition event and cannot replay
  celebration.
- Added `Embermere.UI.ExperienceProgressPresentation` and
  `Embermere.UI.LevelUpPresentation`. The no-hot-reload build passed all 69
  tests in a fresh commandlet; the standalone progression marker and full
  15-package aggregate passed with no `LogPython: Error`, retaining 53 grounded
  Fab plus 23 original-art placements.
- 2026-08-23: made level a deterministic result of durable XP rather than a
  second saved field. Serialized the first `0/100/250/450/700` level curve and
  distinct race/class growth profiles in `DA_EmbermereRules` for a bounded
  level-5 cap.
- Stats now commits XP, derived level, and identity base growth together,
  preserves absolute missing health/mana, emits exact multi-level-aware live
  feedback, and restores silently. Equipment remains one additive layer.
- Persistence now preflights saved equipment against the candidate saved level
  and restores identity, XP, derived base stats, hotbar, and equipment without
  drift. Added four progression tests; the no-hot-reload build, 67/67 tests,
  focused persistence suite, and all 15 package validators passed.
- Clean PIE used the real Trainer transaction then Mara's original physical-F
  quest completion to reach Human Warrior level `2` at `150` XP with exact
  `110/110` health, `53/53` mana, `12` Attack Power, unchanged hotbar, and a
  matching read-only Chronicle summary.

- 2026-08-22: promoted the bounded persistence contract to save version `2`
  with stable race/class IDs, current-rules legality/loadout validation, atomic
  identity/base-stat/hotbar restore, and an explicit non-rewriting version `1`
  Human Warrior compatibility path.
- Chronicle now shows race/class as read-only slot metadata. Capture rejects an
  unconfirmed identity, malformed/unknown/illegal IDs fail before mutation,
  and repeated load cannot stack class stats, hotbar state, equipment bonuses,
  quest rewards, or progression.
- Added `Embermere.Persistence.CharacterIdentityRoundTrip`,
  `Embermere.Persistence.CharacterIdentityRollback`, and
  `Embermere.Persistence.LegacyV1CharacterFallback`. The no-hot-reload build,
  focused six-test persistence run, all 63 automation tests, and fresh
  14-validator package aggregate passed with no Embermere or Python failures.
- Clean PIE rejected Dwarf Ranger, accepted and saved Elf Wizard at exact
  `80/80` health and `110/110` mana, safely rejected a preexisting malformed
  v2 slot, then loaded that Elf Wizard over a fresh Lizardman Ranger twice with
  the exact Wizard hotbar and no drift.

- 2026-08-21: promoted the existing race/class scaffold into a fixed native
  pre-play picker with all eight races, all four classes, visible disabled
  combinations, mouse/keyboard navigation, explicit confirmation, and clean
  controller-owned HUD/input handoff.
- Kept legality and starter loadouts on `UEmbermereRulesData`; added
  data-driven class starting attributes and one-shot character-side atomic
  application of identity, vitals, attributes, and the first four hotbar slots.
- Added four focused tests for initial UI state, Dwarf Ranger/Bullywug Wizard
  restrictions, exact confirmed loadout, duplicate-confirmation safety, and
  controller lifecycle. The isolated commandlet passed all 60 tests.
- Clean PIE accepted the invalid paths and an Elf Wizard at `80/80` health,
  `110/110` mana, and the exact Wizard hotbar; the fresh 14-package aggregate
  and live notice-board route validator also passed.

- 2026-07-02: built successfully after target/inventory polish.
- Replaced the loud overhead `TARGET` text with a smaller selected marker and routed target nameplate text/color through reusable C++ helpers.
- Swapped the target ring off the engine debug material and onto a gold-tinted basic material path.
- Added automation assertions for selected-target presentation text and low-health color behavior.
- Expanded inventory reward inspection text to show the inspected item name and stack count.
- Ran the four-test suite after build through MCP: 4 passed, 0 failed. The already-open editor still emitted hot-reload no-world warnings from the transient target test; restart Unreal before treating warning absence as authoritative.
- Started and stopped PIE through MCP after the build. PIE started successfully, but full visual verification should be done after a clean editor restart because this run followed a live C++ build.
- 2026-07-03: added a native `UEmbermereEnemyNameplateWidget` and mounted it on enemies through a screen-space `UWidgetComponent`.
- The legacy text-render nameplate/marker remains as fallback, but selected enemies now use UMG for name, selected marker, HP text, and HP bar.
- Added `Embermere.UI.EnemyNameplateWidget`; MCP automation suite now runs 5 tests, all passing with 0 warnings.
- Built successfully after the nameplate widget work and booted/stopped PIE through MCP with no current Embermere gameplay or Blueprint errors.
- Direct unauthenticated Fab search/API access is Cloudflare-gated from terminal automation; first asset import should happen through the signed-in Unreal Fab window or Epic Launcher.
- 2026-07-03: moved gameplay feedback out of Unreal's top-left debug overlay and into a native bottom-left chat/combat log.
- Routed player, target, combat, quest, XP, inventory reward, dialogue, enemy attack, death, and respawn messages through the HUD log.
- Added `Embermere.UI.ChatLog`; headless automation now runs 6 tests, all passing with 0 warnings.
- 2026-07-04: tightened the bottom-left chat/combat log so its text is clipped inside the shaded panel.
- Reduced visible chat history to the configured message limit, set a fixed wrap width, and added explicit widget clipping to the panel, stack, and text rows.
- Built successfully and ran clean headless automation: 6 passed, 0 failed, 0 warnings.
- No local `/Game/ThirdParty/Fab` import was present yet, so the first asset pass still needs the signed-in Unreal Fab/Epic Launcher import step before Codex can wire assets into the level.
- 2026-07-05: added bracket-key inventory inspection cycling for multi-stack inventories.
- The top-right inventory panel now marks the selected stack, shows `Inspecting X/Y`, and uses `[`/`]` to wrap between stacks.
- Expanded `Embermere.UI.InventoryToggle` coverage to verify inventory selection advances and wraps in both directions.
- Built successfully and ran clean headless automation: 6 passed, 0 failed, 0 warnings.
- 2026-07-05: added the first local Fab/Epic environment pass for the starter zone.
- Ignored raw local marketplace asset folders in Git so the public repo does not redistribute vendor content.
- Added reusable placement and validation scripts for the tagged `FabPass_` art layer.
- Saved `L_Embermere_Prototype` with 68 created `FabPass_` actors after removing the old visual-only greybox village, road, and ruin markers.
- Built successfully, passed the headless FabPass map validator, and ran clean headless automation: 6 passed, 0 failed, 0 warnings.
- Manual PIE verification is still needed after restarting Unreal because MCP dropped during the first heavy asset compile pass.
- 2026-07-06: added hotbar cooldown enforcement and ready-time feedback.
- Combat now rejects abilities from dead characters, and `Embermere.Combat.DeadCasterRejected` covers that death/recovery contract.
- The bottom-left chat/combat log now uses clipped single-line rows to avoid wrapping below the shaded panel border.
- Built successfully with `-NoHotReloadFromIDE` and ran clean headless automation: 7 passed, 0 failed, 0 warnings.
- Captured the no-hot-reload build lesson in `Docs/UNREAL_LESSONS.md`; restart Unreal before manual PIE so the editor loads the relinked base module.
- 2026-07-07: added first-pass starter enemy leash and return-home behavior.
- Added `LeashRadius`, `ReturnHomeRadius`, and `ReturnHomeSpeedCmPerSecond` so enemies can drop aggro and return home if pulled too far from their wilderness pocket.
- Added `Embermere.Enemy.LeashRules`; headless automation now runs 8 tests.
- Built successfully with `-NoHotReloadFromIDE` and ran clean headless automation: 8 passed, 0 failed, 0 warnings.
- FabPass validator exited successfully when run sequentially. Do not run Unreal commandlets in parallel because they can race on shared UBT trace/log files.
- 2026-07-08: added short player respawn damage protection.
- `UEmbermereStatsComponent` now supports temporary damage immunity, and player respawn grants a configurable protection window.
- Added `Embermere.Stats.DamageImmunity`; headless automation now runs 9 tests.
- Build/test/validator remain clean when run sequentially. Manual PIE should verify the recovery message and damage-protection feel after restarting Unreal.
- 2026-07-09: tightened the inventory HUD empty/reward-state presentation.
- Added `GetInventoryDisplayText()` as a reusable/testable HUD API, so the live panel and automation use the same inventory text path.
- Empty inventory now explicitly says quest rewards appear there, and multi-stack inventory shows the bracket-key inspection hint.
- Expanded `Embermere.UI.InventoryToggle` coverage to assert empty text, selected stack text, stack quantities, stack limits, item descriptions, and bracket-key hint text.
- Built successfully with `-NoHotReloadFromIDE`, ran clean headless automation: 9 passed, 0 failed, 0 warnings, and reran the FabPass validator successfully.
- Direct MCP initialized, but raw `tools/call` streaming did not return usable output in this session. Manual PIE still needs a clean editor restart before validating the live UI and FabPass feel.
- 2026-07-09: the refreshed ChatGPT/Codex app exposed Unreal MCP as first-class tools; the live nine-test baseline passed in one MCP call and Slate screenshots captured the running HUD.
- Found that the Unreal Python placement helper mapped positional `Rotator` arguments into pitch instead of yaw, leaving almost the entire Fab art pass tilted.
- Corrected all 68 actor rotations, removed three oversized sci-fi building shells, restored PlayerStart, and saved a 65-actor upright art layer through Unreal's asset API.
- Strengthened the Fab validator to reject tilted actors; a fresh headless validation passed with 65 upright actors and all gameplay anchors intact.
- Replaced the compact inventory text block with a 510x292 structured inventory window containing capacity, item rows, selected-item metadata, descriptions, empty/reward state, and keyboard hints.
- Replaced chunky cube target-ring segments with 24 flat plane segments plus subtle rotation and pulse.
- Added live hotbar cooldown countdown text and unavailable-state dimming plus `Embermere.UI.HotbarCooldownDisplay`.
- Built successfully with `-NoHotReloadFromIDE`; the expanded suite passed 10/10 with zero warnings.
- Published the related Real World AI Lab field note, `When Unreal MCP Started Feeling Native`, plus flashcards and a private LinkedIn draft.
- 2026-07-10: verified the fresh live MCP baseline: 10 tests passed with zero warnings, and the structured HUD plus corrected Fab layer were present in PIE.
- Added `SkyAtmosphere`, movable real-time skylight fill, and restrained fog color/density; PIE now shows a blue sky and readable village route instead of the prior black-sky presentation.
- Added the atmosphere actor to `setup_prototype_level.py`, corrected that foundational helper's positional `unreal.Rotator` bug, and made the fresh-process Fab validator require the atmosphere.
- Replaced passive inventory rows with non-focusable clickable buttons, direct stack selection, selected-row highlighting, and controller input-mode/cursor switching while the inventory is open.
- Preserved `I` and `[`/`]` keyboard behavior and expanded `Embermere.UI.InventoryToggle` with direct-selection and invalid-index assertions.
- Created and saved `/Game/Art/Embermere/Targeting/M_EmbermereTargetRing`, an unlit additive parameterized gold material, and wired the 24 ring segments to it with an engine-material fallback.
- Added a target-presentation assertion proving the dedicated material resolves.
- Final no-hot-reload build succeeded; headless automation passed 10/10 with zero warnings; the saved-map validator passed with 65 upright Fab actors, required gameplay anchors, and the atmosphere present.
- Restart the interactive editor before manually validating row clicks/cursor behavior and the emissive ring, because the current editor predates the final C++ relink.
- 2026-07-11: added `/Game/Art/Embermere/Environment/M_EmbermereGround` and applied its muted moss treatment to the zone plane plus both raised prototype platforms.
- Switched the target-ring material from additive to opaque unlit/emissive and raised the constructor offset so its 24 segments clear the combat-pocket platform.
- Added data-driven item categories, equipment slots, level requirements, stat bonuses, and `UEmbermereEquipmentComponent` equip/replace/unequip rules.
- Added a real inventory Equip/Unequip button, dynamic action label, level gating, richer item metadata, and chat feedback.
- Migrated the tracked Recruit Pack quest reward to level-1 Back armor through an idempotent Unreal asset script so the action is testable in the normal loop.
- Added `Embermere.Equipment.SlotRules` and expanded `Embermere.UI.InventoryToggle`; final headless automation passed 11/11 with zero warnings.
- Made the level rebuild script reproduce the moss ground and exact successful daylight settings.
- Strengthened fresh-process map validation for the ground overrides, sun, real-time skylight, fog baseline, 65 upright Fab actors, and gameplay anchors; validation passed.
- Clean-restart PIE still needs to verify the newly linked Equip/Unequip button and target-ring clearance because safely closing the live editor requires explicit user confirmation.
- 2026-07-12: clean-restart MCP discovered and passed all 11 existing tests, and PIE verified daylight/moss terrain, Mara quest acceptance, the inventory shell, target/nameplate, chat clipping, and target-ring platform clearance.
- Lowered target-ring emissive strength from `2.5` to `0.75` and changed the runtime default to a more saturated orange-gold so outdoor exposure preserves its color.
- Expanded inventory to a 700x330 three-column bag/detail/equipment layout with visible slots, aggregate bonuses, item effect summaries, and `[E]` markers.
- Added idempotent equipment stat application for maximum health/mana, power, and armor plus missing-resource preservation and armor mitigation.
- Added data-driven consumable recovery and transactional `Use`: inventory decrements only after health or mana actually increases.
- Updated the Recruit Pack asset to grant `+5 HP` and `+1 Armor`.
- Added the stackable Marsh Tonic data asset with `+25 Health` and `+10 Mana` recovery and wired Marsh Prowlers to grant one on death.
- Added editable enemy loot item, quantity, and drop-chance rules plus player inventory/chat delivery.
- Added `Embermere.Equipment.StatApplication`, `Embermere.Inventory.ConsumableUse`, and `Embermere.Enemy.LootRules`; final headless automation passed 14/14 with zero warnings.
- Re-ran the saved-map validator: all 65 upright FabPass actors, gameplay anchors, moss ground, and daylight baseline remain intact.
- Restart Unreal before visually verifying today's newly linked equipment pane, stat changes, Marsh Tonic drop/use flow, and ring color.
- 2026-07-13: clean live PIE verified the daylight/moss/Fab baseline, Mara quest acceptance, targeting/nameplate, combat progression, and a real Marsh Tonic drop before source work.
- Replaced the equipment text list with ten stable clickable paper-doll slot controls; occupied slots use a restrained gold state and click-to-unequip behavior.
- Equipped items now leave the bag, replaced items return to it, and unequipped items return only when capacity exists.
- Added rollback-safe same-slot replacement and full-bag unequip rejection so gear cannot be lost or duplicated.
- Made inventory add/remove primitives atomic; failed capacity or quantity operations leave all stacks unchanged.
- Added `Embermere.Equipment.InventoryTransactions` and `Embermere.Inventory.CapacityTransactions`; final headless automation passed 16/16 with zero warnings.
- Built successfully with `-NoHotReloadFromIDE` and reran saved-map validation: 65 upright FabPass actors, gameplay anchors, moss ground, and daylight baseline remain intact.
- Restart Unreal before visually verifying the newly linked paper-doll slot grid and bag/equipment transfer flow.
- 2026-07-13: installed Blender 5.1.2 and the pinned structured Blender MCP
  bridge with Safe Mode on, inline code off, localhost binding, and project-only
  approved script roots.
- Generated `SM_EmbermereWaystone_01` from the tracked Blender script, revised
  its first dark preview, and validated 1,340 triangles, three materials, one UV
  channel, ground pivot, and two Unreal collision boxes.
- Imported and saved the waystone plus its materials under
  `/Game/Art/Embermere/Environment/PrototypeVillage`, replaced the temporary
  road stump, and tagged the map actor `EmbermereOriginalArt`.
- Caught and fixed a live snap-order issue that initially placed the waystone on
  top of the outgoing stump; deterministic placement now disables snap and the
  validator asserts Z `20`.
- Final Mac editor build succeeded; saved-map validation passed with 64 upright
  FabPass actors plus the original waystone; headless automation passed 16/16
  with zero failures and zero warnings.
- 2026-07-14: added net item comparison against equipped gear or an empty slot,
  with concise detail-pane output and hover tooltips for bag rows and occupied
  paper-doll slots.
- Added item-identity action routing and explicit target-slot validation so
  future drag/drop can reject wrong-slot and stale payloads before mutation
  while reusing the atomic equipment transaction path.
- Added [Docs/INVENTORY_INTERACTION_PLAN.md](Docs/INVENTORY_INTERACTION_PLAN.md)
  covering drag payload identity, bag/equipment rules, sorting invariants,
  implementation order, and loss/duplication edge cases.
- Added `Embermere.UI.ItemComparison` and
  `Embermere.Inventory.IdentityActions`; the authoritative Mac build succeeded
  and headless automation passed 18/18.
- First-class MCP proved the still-open editor was stale by discovering only 14
  tests. Restart Unreal before visual comparison/tooltip or paper-doll testing.
- 2026-07-15: added typed, item-identity drag payloads and bounded bag-to-slot
  plus equipment-to-bag drag/drop without replacing click, keyboard, or action
  button paths.
- Added gold valid-target, restrained red invalid-target, bag-return, footer,
  and compact drag-label feedback inside the fixed inventory window.
- Added `Embermere.UI.InventoryDragDrop` covering matching/wrong slot,
  under-level, stale bag/equipment identity, successful return, and full-bag
  preservation. Final headless automation passed 19/19.
- Built `SM_EmbermereEmberLamp_01` through the reviewed Blender MCP script lane:
  editable `.blend`, FBX, preview, metrics, one UV channel, no non-manifold
  edges, dark iron/stone/moss/ember materials, and two authored `UBX_` boxes.
- Replaced the two temporary sci-fi lamps with original Mara-side and road-side
  ember lamps, leaving 62 upright `FabPass_` actors plus three tagged original
  placements. The fresh-process map validator passed exact transform, mesh,
  bounds, classic FBX provenance, and collision assertions.
- Captured the UE 5.8 import lesson: the generic Interchange path discarded
  authored collision when `bCollision` stayed false; pinning `FbxFactory` and
  recreating the partial mesh package preserved both UBX colliders and clean
  `FbxStaticMeshImportData`.
- The no-hot-reload editor build succeeded, the final 19-test suite passed, and
  saved-map validation passed. Restart Unreal before authoritative drag/drop
  PIE because the open editor predates the final relink.
- 2026-07-16: clean live PIE verified the full Mara quest/reward loop, three
  Marsh Prowler kills and tonic stacking, target clear on death, bracket
  cycling, Recruit Pack comparison, valid/invalid drag targets,
  equipment-to-bag return, W autorun cancel, and both ember lamps in the saved
  route. Slate could not hold `Ctrl` across a later game tick, so `Ctrl+M`
  remains a manual input check rather than a detected regression.
- Added an explicit compact Sort control that orders weapon, armor, consumable,
  quest, and misc stacks by category then name, preserves selected item identity
  and duplicate-stack occurrence, and disables during pending or active drags.
- Added `Embermere.Inventory.StableSorting`; the authoritative Mac build
  succeeded and headless automation passed 20/20.
- Fresh-process map validation passed with 62 upright FabPass actors, the
  waystone and two ember lamps, all gameplay anchors, moss ground, and the exact
  daylight baseline intact. Restart Unreal before visually checking the newly
  linked Sort control.
- 2026-07-17: ran all 20 tests in the live editor and through authoritative
  headless automation; both suites passed with no test failures after a
  successful `-NoHotReloadFromIDE` Mac editor build.
- Completed the full Mara quest in clean PIE and verified tonic stacking,
  identity-preserving Sort, bracket cycling, Recruit Pack bag-to-Back and
  Back-to-bag drags, wrong-slot rejection, target clear, and W autorun cancel.
- Tuned the starter encounter around the proven combat cadence: default aggro
  is now `525` cm, while damage remains 6 and attack cooldown remains 2 seconds;
  automation asserts all three defaults.
- Used native WorldStatic overlap probes to move the Prowler homes to the
  collision-clear triangle `(1900, 300)`, `(1700, 1100)`, and `(2500, 1300)`.
  A focused PIE pull moved and activated only the nearest enemy while both
  neighbors remained at home.
- Made enemy marker cones plus safe/combat area cylinders explicitly
  `NoCollision` in both the saved map and foundational setup script. The map
  validator now rejects visual guides that become invisible blockers.
- Built `SM_EmbermereRoadSignpost_01` through Blender MCP with reviewed source,
  editable `.blend`, FBX, preview, deterministic metrics, 1,828 triangles, one
  UV, no non-manifold edges, shared fantasy materials, and two authored boxes.
- Imported the signpost through explicit `FbxFactory`, explicitly saved its new
  `M_EmbermereTimber` package, and placed `Embermere_RoadSignpost_01` beside the
  village road with base/post collision and non-colliding overhead arms.
- The tracked validator reloaded the saved level and passed 62 upright Fab
  actors, four exact original-art placements, the collision-cleared encounter,
  visual-only marker collision, gameplay anchors, moss ground, and daylight.
  A separate final commandlet launch stalled in macOS app-launch services
  before Unreal produced a log, so this result is recorded accurately as a
  live-editor saved-map reload rather than a fresh-process pass.
- 2026-07-18: separated automatic forward movement from the manual movement
  handler so `W` and `S` can cancel autorun without the autorun tick cancelling
  itself. Added `Embermere.Input.AutorunCancellation`; the authoritative
  no-hot-reload suite passed 21/21.
- Replaced the compact inventory drag label with a fixed 236x62 fantasy token
  using category sigils, warm item text, contextual slot/effect copy, and
  project-owned moss/gold presentation. Extended drag/drop automation for its
  armor sigil, context text, and accent color.
- Built `SM_EmbermereRoadGate_01` through Blender MCP with reviewed source,
  editable `.blend`, FBX, preview, deterministic metrics, 3,296 triangles, one
  UV, no non-manifold edges, five shared fantasy materials, and four authored
  support colliders.
- Imported the gate through explicit `FbxFactory`, placed
  `Embermere_RoadGate_01` at `(1080, 540, 20)`, yaw `20`, and saved the map.
  Native traces proved the 250 cm center opening is clear while a support is
  solid; the validator passed 62 upright Fab actors and five exact original-art
  placements.
- The open editor predates the final C++ relink. Restart Unreal before claiming
  authoritative live `W`/`S` cancellation or visual approval of the new drag
  token; those two checks are tomorrow's first manual boundary.
- 2026-07-19: rebuilt `EmbermereEditor` with `-NoHotReloadFromIDE` and ran the
  complete headless suite; all 21 tests passed with zero failures and zero
  warning tests. The clean live editor also discovered and passed all 21 after
  the final boundary-fence map save.
- Restarted Unreal cleanly, started MCP, and used timed player-transform reads
  in PIE to prove `Q` movement plus independent `W` and `S` cancellation. Both
  cancellation checks held the exact same transform for a full second.
- Built `SM_EmbermereBoundaryFence_01` through the reviewed Blender MCP script
  lane: editable `.blend`, FBX, preview, deterministic metrics, 2,632 triangles,
  one UV, no non-manifold edges, five shared fantasy materials, and three
  authored `UBX_` colliders.
- Imported through explicit `FbxFactory` and placed matching south/north fence
  instances beside `Embermere_RoadGate_01`. The map now contains 62 upright
  Fab actors plus seven exact original-art placements.
- Expanded the saved-map validator for both fence transforms, materials,
  bounds, tags, and three-box collision. Added a native trace validator proving
  three gate lanes clear, one gate support solid, and both fence centers solid.
- Inspected the Blender preview and live Unreal road approach. The north fence
  and center opening read clearly; an existing tree partly masks the south
  fence and is recorded for foliage-only composition review.
- Captured two durable UE 5.8 lessons: Python line traces return `HitResult` or
  `None`, and commandlet Python errors can coexist with a zero process exit, so
  validators must require a success marker and reject `LogPython: Error`.
- Slate's atomic `Ctrl+M` synthesis still cannot span the later controller tick
  that polls Ctrl state. Keep physical inversion feedback and populated-item
  drag-token appearance as the next manual visual checks.
- 2026-07-20: built `SM_EmbermereBoundaryStone_01` through the reviewed Blender
  MCP script lane with editable source, FBX, preview, deterministic metrics,
  1,872 triangles, five shared materials, one UV, no non-manifold edges, a
  ground pivot, and two authored `UBX_` colliders.
- Imported it through explicit `FbxFactory` and placed matching south/north
  rune-topped end markers outside the existing boundary fences. The saved map
  now contains 62 upright Fab actors plus nine exact original-art placements.
- Moved only `FabPass_Road_Pine_05` and `FabPass_Wild_Tree_South_01` within the
  south foliage band. A fresh road capture now shows both fences, both end
  markers, and the full gate opening without changing validated boundary
  geometry or the Fab actor count.
- Expanded fresh-process validation for the boundary-stone classic-FBX import,
  bounds, five materials, two colliders, tags, exact transforms, and both
  foliage transforms. Native traces pass three clear gate lanes, one gate
  support, two fence centers, and two boundary-stone cores.
- The authoritative Mac build succeeded with `-NoHotReloadFromIDE`; the live
  editor discovered and passed all 21 tests with no failures or warnings.
  Fresh PIE again proved `Q` movement and independent `W`/`S` cancellation by
  holding identical transforms for one second after each stop input.
- Recorded the Slate focus lesson: refresh the accessibility snapshot and click
  the game viewport before synthetic gameplay keys, or editor-tool calls can
  make a correct input path look broken. Physical `Ctrl+M` and populated-item
  drag-token appearance remain honest manual checks.
- 2026-07-21: generated a deterministic project-owned family of 14 square UI
  icons: Recruit Pack, Marsh Tonic, ten equipment slots, and explicit missing
  item/slot fallbacks. The tracked standard-library generator keeps the source
  art reproducible and the palette aligned with Embermere's moss, stone, iron,
  gold, leather, and ember roadside family.
- Added `UEmbermereUiIconSet`, which resolves explicit soft item references,
  category fallbacks, equipment-slot art, and missing-art fallbacks without
  hardwiring textures into inventory rules. Recruit Pack and Marsh Tonic now
  own explicit icon references in their saved data assets.
- Integrated fixed `18x18` row/slot icons and a fixed `42x42` detail icon into
  the native inventory window while preserving item names, hover tooltips,
  click/keyboard interaction, and collapsed text-only fallback behavior.
- Imported and explicitly saved all 14 `128x128` textures plus the icon-set data
  asset under `/Game/UI/Icons`. A fresh-process Python validator reloaded every
  package and passed exact dimensions, mappings, texture settings, and starter
  item assignments.
- Added `Embermere.UI.IconPresentation`; after adapting the source-dimension
  assertion for `-NullRHI`, the authoritative no-hot-reload build succeeded and
  the complete suite passed 22/22. Fresh saved-map and road-boundary validators
  also passed with the accepted 62 Fab actors, nine original placements, and
  three clear gate lanes intact.
- The already-open interactive editor predates the final icon C++ link. Restart
  it before visual approval of the row, detail, slot, fallback, and tooltip
  presentation in PIE.
- 2026-07-22: restarted PIE on the July 21 icon module, passed all 22 tests in
  the live editor, and inspected both the empty and populated inventory. The
  ten empty-slot symbols and starter-item art were distinct and stable, but the
  real saved `Embermere Recruit Pack` label clipped in its row and crowded the
  adjacent Equipment heading in the detail layout.
- Shortened the data-driven player-facing label to `Recruit Pack`, reproduced
  that value in `configure_starter_items.py`, saved the item package, and added
  the exact name to fresh-process icon validation. A second populated PIE
  capture accepted the corrected row and detail-heading fit. This keeps the
  underlying `RecruitPack` identity unchanged.
- Reused the existing resolver in the fixed `236x62` fantasy drag token and the
  temporary reward popup. Resolved item art now occupies a fixed `44x44` drag
  cell and a fixed `32x32` reward cell; category text and missing-item art remain
  fallbacks without changing drag payload or transaction identity.
- Expanded `Embermere.UI.InventoryDragDrop` and
  `Embermere.UI.IconPresentation` for resolved drag art, visual construction,
  and fixed reward-icon bounds. The authoritative no-hot-reload Mac build and
  fresh-process suite passed 22/22 with zero warnings or failures.
- Fresh-process icon validation passed all 14 textures, mappings, starter-item
  assignments, and the compact Recruit Pack name. Saved-zone validation passed
  62 upright Fab actors plus nine original placements; native traces again
  proved three clear gate lanes and solid gate/fence/end-stone collision.
- The interactive editor predates today's final relink. Clean-restart PIE still
  owns visual approval of the icon-bearing drag token, reward popup, and
  fallback appearance; the compact `Recruit Pack` copy is already accepted.
- The Codex automation read/update API stalled twice and was terminated without
  mutating `daily-embermere-rpg-build`. Its existing 8:00 AM heartbeat remains
  active and already treats this `Start Here` section as authoritative, so this
  handoff remains tomorrow's source of truth until the app endpoint recovers.
- 2026-07-23: clean-restart PIE accepted the fixed reward popup and a real
  Recruit Pack bag-to-Back drag on the July 22 module. The reward kept its
  `32x32` Marsh Tonic art and readable copy; the equipment transaction removed
  the item from the bag, populated the Back slot, raised HP to `105/105`, and
  posted one chat line. Slate cannot freeze its atomic drag mid-flight, so the
  populated drag-token appearance remains a physical-eye check.
- Extended the deterministic source family from 14 to 31 project-owned
  `128x128` textures: sixteen distinct starter-ability illustrations plus an
  explicit missing-ability fallback. Warrior uses ember/iron, Cleric
  gold/ivory, Ranger moss/leather, and Wizard frost/arcane while sharing the
  roadside family's frame language.
- Added direct soft icon references to every native ability definition,
  persisted matching assignments through `DA_EmbermereRules`, and extended
  `UEmbermereUiIconSet` with ability fallback resolution. The native hotbar now
  presents fixed `32x32` art inside the existing `92x64` slots, keeps
  cooldowns to a two-line layout, dims art and text together, and exposes
  data-driven description/power/mana/range/cooldown hover tooltips.
- Added `Embermere.UI.AbilityIconPresentation` and expanded the cooldown test.
  The authoritative no-hot-reload build succeeded and all 23 tests passed in a
  fresh process with zero failures. A separate fresh process reloaded all 31
  exact UI textures, all mappings, and all sixteen distinct ability references.
- Saved-zone validation remained green with 62 upright Fab actors and nine
  original placements. Native traces retained three clear gate lanes plus
  solid gate/fence/end-stone collision. The interactive editor now predates
  the final July 23 C++ link, so all-class hotbar appearance and tooltip fit
  are tomorrow's first clean-restart PIE acceptance pass.
- The Codex automation endpoint recovered. `daily-embermere-rpg-build` remains
  active at 8:00 AM and now starts from the 23-test ability-icon/hotbar handoff
  instead of the stale 21-test item-icon milestone.
- 2026-07-24: restarted Unreal onto the July 23 module, discovered and passed
  all 23 existing tests, and visually accepted Warrior, Cleric, Ranger, and
  Wizard hotbar palettes plus synchronized Warrior cooldown dimming/countdown.
- Added a data-driven ability-effect contract covering damage, healing, mana
  recovery, Attack Power buffs, Armor buffs, duration, and movement-speed
  multipliers. Saved rules now make Battle Shout, Ward, Snare, Nature's Focus,
  Frost Root, and Meditate functional instead of descriptive placeholders.
- Stats now expose effective Attack Power, Armor, and movement speed; damage
  consumes effective armor, attacks consume effective power, timed effects
  expire from world time, and vital initialization clears temporary effects so
  enemy respawn cannot preserve a root or snare.
- Player and enemy movement consume the shared multiplier. Live clean-PIE
  probes measured Snare at `0.50x`, Frost Root at `0.00x`, both offensive buffs
  at 18 effective Attack Power, Ward at 10 effective Armor, and Meditate
  restoring mana from 20 to 38.
- Added `Embermere.Combat.StarterAbilityEffects`, saved-data validation for all
  sixteen ability semantics, and deterministic
  `Scripts/configure_starter_abilities.py`. The authoritative no-hot-reload
  build succeeded, fresh headless automation passed 24/24, and UI/icon,
  saved-map, and road-boundary validators all passed without Python errors.
- 2026-07-25: generalized the deterministic UI generator and added a
  project-owned `128x160` armored-adventurer paper-doll backdrop in the shared
  stone/moss/iron/gold/ember language.
- Added a soft backdrop reference/resolver to `DA_EmbermereUiIconSet` and
  layered it beneath the unchanged ten-slot equipment grid through a
  hit-test-invisible native UMG overlay. Slot bounds, labels, icons, tooltips,
  drag targets, and transaction rules remain unchanged.
- Added `Embermere.UI.PaperDollPresentation`. The authoritative no-hot-reload
  build succeeded, fresh headless automation passed 25/25 with zero warnings
  or failures, and a second process reloaded the exact texture dimensions,
  texture settings, and saved mapping.
- Saved-zone validation retained 62 upright Fab actors and nine original-art
  placements; native traces retained all three clear gate lanes and required
  solid boundary collision. The running GUI editor still holds the prior
  module, so backdrop contrast and fit remain the next clean-restart PIE gate.
- 2026-07-26: accepted the centered paper-doll backdrop in clean PIE with an
  empty equipment grid; it remained readable beneath all ten fixed controls
  without moving the title, bonuses, footer, inventory, or hotbar.
- Added source-ability-backed timed-status snapshots to stats and registered
  them only after generic buff/control rules succeed. Player and selected-target
  HUD rows now render existing ability art, names, live countdowns, beneficial
  or harmful color, and hover descriptions without hard-coded class or ability
  IDs.
- Duplicate effect registration refreshes one record, world-time expiration
  removes it, and existing vital-reset/respawn paths clear both gameplay state
  and presentation metadata. Meditate remains instantaneous and creates no
  timed record.
- Added `Embermere.UI.TimedStatusPresentation`. The authoritative
  no-hot-reload build succeeded and fresh headless automation passed 26/26.
  UI-art, saved-map, and native road-boundary validators retained explicit
  success markers with no Python errors.
- The interactive editor now predates the final July 26 link. Clean-restart PIE
  still owns visual acceptance of the new status rows and the paper doll with
  Recruit Pack equipped.
- Refreshed the daily heartbeat to begin from this 26-test baseline and move
  next into clean-PIE status acceptance, restrained VFX, original village art,
  or concrete live combat tuning.
- 2026-07-27: accepted Battle Shout and Snare timed-status cells in clean PIE,
  including saved ability art, beneficial/harmful treatment, live countdowns,
  fixed bounds, and stable neighboring HUD layout. Meditate remains
  instantaneous and focused automation retains duplicate, expiration, respawn,
  death, and target-switch clearing.
- Accepted the occupied paper-doll state with Recruit Pack equipped: the Back
  slot uses its gold icon state, HP reads `105/105`, the bag is empty after the
  atomic transfer, and the backdrop remains readable without moving the title,
  bonuses, footer, or hotbar.
- Added `SM_EmbermereSupplyChest_01`, a 2,364-triangle deterministic Blender
  prop with five shared materials, one UV channel, two authored `UBX_` boxes,
  editable source, FBX, preview, and metrics. Its route-facing saved actor
  replaces `FabPass_Village_Crates_A` beside the vendor area.
- Saved-map validation now requires 61 upright Fab actors and ten original-art
  placements, including exact chest provenance, materials, bounds, collision,
  tag, and transform. Native traces in the initialized live editor proved the
  chest lid solid while retaining all three open gate lanes and required
  boundary collision.
- The authoritative no-hot-reload build succeeded; fresh commandlet automation
  passed 26/26 with zero failures or warnings; UI-art and saved-map validators
  emitted their success markers with no Python errors.
- Refreshed the 8:00 AM daily automation to start from the 61-Fab/10-original
  supply-chest baseline and to separate fresh-process persistence checks from
  initialized-live-world collision traces.
- 2026-07-27 evening: built Embermere's first original rigged creature,
  `SK_EmbermereMarshProwler_01`, from a reviewed deterministic Blender script.
  The saved source has 7,464 triangles, 3,878 vertices, 26 authored bones, five
  Stylized Classic marsh materials, applied transforms, and six actions:
  Idle, Walk, Run, Attack, Hit, and Death.
- Imported the skeletal mesh, skeleton, physics asset, materials, and six
  animation sequences into a project-owned Unreal folder. Generic enemy state
  now routes those animations without coupling combat, loot, target, quest, or
  respawn rules to the art asset. All three saved Prowler map instances were
  repaired and validated after discovering that a correct Blueprint CDO does
  not overwrite stale placed-instance component state.
- Completed the measured environment-grounding pass: ordinary art moved from
  the false `Z=20` baseline to the real `Z=0` surface; unsupported SoulCave
  accents and three visual enemy markers were removed; 21 foliage/tree
  placements received project-owned overrides; and the ground became a
  38-expression moss/earth material with a `300` cm worn-road half width.
- Added `SM_EmbermereMarshReedCluster_01`, a deterministic 1,012-triangle,
  four-material visual-only asset with a terrain-blended footprint. Four saved
  `NoCollision` placements add marsh identity without blocking the road or
  combat sightlines.
- Clean PIE exercised the real Prowler targeting, Strike, retaliation, death,
  target clear, Marsh Tonic loot, and respawn loop. Editor automation passed
  27/27 with zero failures, skips, or warnings. The no-hot-reload Mac build,
  fresh saved-map validator, road-boundary traces, and headless saved-Blender
  inspection all passed.
- Documented two honest follow-ups: straight-line autorun currently meets
  Mara's temporary village cluster, and several KiteDemo meshes retain absent
  internal vendor dependencies even though project-owned component overrides
  keep the visible map readable.
- 2026-07-28: reproduced the straight-line PlayerStart autorun stop at the
  supply chest, moved `Embermere_SupplyChest_Vendor_01` to
  `(-1740, -1180, 0)` without weakening its route-facing village composition,
  and saved the exact map transform.
- Added geometric validation requiring at least `225` cm of chest clearance
  from the spawn corridor and an initialized-world player-height trace proving
  the old blocked line is clear. Fresh PIE autorun moved well beyond the former
  stop point.
- Exercised the complete Marsh Prowler loop in clean PIE: real 6-damage
  retaliation killed the player and triggered recovery/protection plus
  return-home; a bounded zero-damage diagnostic then proved four Strikes,
  target clear, Marsh Tonic loot, death hold, hide, 12-second respawn, full HP,
  and untouched neighboring enemies.
- Added an asset-agnostic world-status presentation to every Embermere
  character. Eight fixed non-colliding segments consume the same successful
  status snapshots as the HUD, with orange-gold Attack Power, blue-white Armor,
  marsh-green Snare, frost-cyan Root, harmful priority, pulse/rotation, and
  dead/empty hiding.
- Added `Embermere.UI.WorldStatusVfxPresentation`; the authoritative
  no-hot-reload build succeeded and all 28 tests passed in a fresh commandlet.
  Saved-map validation, initialized-world road/chest traces, and fresh Blender
  Prowler inspection all retained explicit success markers.
- Clean PIE visually accepted Battle Shout's eight restrained orange-gold
  segments around the player alongside its status countdown, with no HUD
  movement or overlap. Ward, Nature's Focus, Snare, and Frost Root remain the
  next normal-route color/readability sweep rather than unverified claims.
- 2026-07-29: replaced the raised orange-gold rotating marker with an
  EverQuest-inspired cyan-blue target circle. Forty-eight overlapping
  non-colliding segments form a stationary complete read, resolve their radius
  from transformed capsule/mesh bounds plus `18` cm padding, and trace the
  supporting surface with a `16` cm effective clearance while selected. The
  larger value is visually flush in this prototype because the moss ground's
  rendered surface sits above the collision surface returned by the trace.
- Corrected the user-reported clean-PIE disappearance. The saved
  `BP_StarterEnemy` still inherited 24 older native segment templates while a
  native-only test constructed all 48. Runtime initialization now reuses saved
  segment components and creates any missing transient components until the
  exact 48-segment presentation contract is satisfied.
- Expanded `Embermere.Combat.TargetSelectionPresentation` to prove cyan color,
  continuity, restrained pulse, no rotation/collision, selection, target
  switching, clear behavior, and effective terrain clearance. Prowler
  presentation automation now constructs the actual Blueprint-generated class,
  proves runtime reconciliation to 48 non-colliding segments, and confirms its
  visual footprint expands beyond the generic minimum without becoming
  oversized.
- The no-hot-reload Mac build, fresh commandlet suite, restarted-editor MCP
  suite, saved-map validator, initialized-world road traces, and saved Blender
  Prowler inspection all passed. Corrected clean daylight PIE showed the cyan
  circle beneath a live selected Blueprint Prowler with its nameplate and HUD
  target frame intact. A final physical-eye sweep across the other two
  normal-route instances remains the honest acceptance check.
- 2026-07-30: retained the accepted cyan target circle across all three live
  Prowlers and added finite-world recovery below `Z=-1000`, including autorun
  cancellation, forced death through temporary immunity, frozen death movement,
  exact village recovery, full vitals, walking restoration, zero velocity, and
  three seconds of protection. The suite advanced to 29 tests.
- 2026-07-31: corrected harmful world auras that were structurally active but
  visually buried inside the Marsh Prowler. Harmful presentation now derives a
  bounded radius from transformed skeletal bounds and traces the support
  surface, resolving the live Prowler to a `94.352` cm inner radius at `21` cm
  above ground. Clean PIE accepted marsh-green Snare and lighter ice-white cyan
  Frost Root inside the unchanged outer target circle.
- Expanded `Embermere.UI.WorldStatusVfxPresentation` with the real Prowler mesh,
  footprint/grounding assertions, target-circle containment, and Root/Snare
  contrast checks. The authoritative no-hot-reload Mac build, all 29 tests,
  saved-zone and UI-art validators, and initialized-world route traces passed.
- 2026-08-01: built `SM_EmbermereFenwatchShelter_01`, Embermere's first full
  village-scale original module. The reviewed Blender source is 4,348 triangles
  at `438.0 x 296.782 x 369.5` cm with five shared materials, one UV channel,
  applied scale, a ground pivot, and four authored support boxes.
- Imported through classic `FbxFactory`, explicitly saved the mesh package, and
  placed `Embermere_FenwatchShelter_Mara_01` at `(-1740, -700, 0)`, yaw `-64`.
  It replaces the old Mara stone backdrop, mismatched market cover, and
  vendor/trainer cubes, bringing the map to 57 Fab actors and 15 original-art
  placements.
- Rejected the first technically valid shelter transform because clean PIE
  showed it hiding Mara. The accepted transform places the shelter behind her,
  retains the quest-marker/name read, and preserves the measured straight
  autorun route plus independent `W` cancellation.
- The no-hot-reload Mac build succeeded; fresh-process saved-map and UI-art
  validators emitted their success markers without `LogPython: Error`; all 29
  automation tests passed; and initialized-world traces proved the shelter
  center and spawn corridor clear plus all four supports solid.
- 2026-08-02: replaced Mara's stretched engine cube with the project-owned
  `SM_EmbermereFenwatchKeeper_Mara_01` static presentation. The reviewed
  Blender source is 3,280 triangles at `107.45 x 71.0 x 207.5` cm with six
  project-owned materials, one UV channel, applied scale, a ground pivot, and
  no collision.
- Imported through classic `FbxFactory`, explicitly saved the mesh and skin
  material packages, and reconciled both the real saved `BP_QuestGiver` SCS
  template and Mara's placed visual component. The actor and interactable
  gameplay ownership remain unchanged; only the art now uses grounded local
  offset `(0, 0, -140)`, PlayerStart-facing yaw `100`, unit scale, and
  `NoCollision`.
- Clean PIE accepted grounded feet, readable moss-cowl/staff/satchel silhouette,
  shelter composition, and unobstructed quest marker/name. The map now retains
  57 grounded upright Fab actors and 16 project-owned placements.
- Added `Embermere.NPC.FenwatchKeeperPresentation` against the actual saved SCS
  template. The authoritative no-hot-reload Mac build and all 30 tests passed;
  fresh-process saved-map and UI-art validators plus initialized-world route,
  shelter, boundary, and chest traces emitted their explicit success markers.
- 2026-08-03: built `SM_EmbermereFenwatchQuartermaster_01`, a deterministic
  3,632-triangle static merchant at `120.842 x 93.0 x 217.0` cm with six
  project-owned materials, one UV channel, applied scale, a ground pivot, and
  no collision. The accepted actor sits beside the supply chest at
  `(-1530, -1190, 0)`, yaw `100`, without obscuring Mara or the route.
- Added `AEmbermereNpcPresentationActor`, a reusable art-only wrapper with soft
  static/skeletal references, optional animation class, one shared authored
  transform, deterministic lane fallback, and permanent non-colliding visuals.
  It intentionally owns no interaction, vendor, trainer, dialogue, or quest
  behavior; the contract is recorded in `Docs/NPC_PRESENTATION_CONTRACT.md`.
- Added `Embermere.NPC.PresentationContract` and
  `Embermere.NPC.FenwatchQuartermasterPresentation`. The authoritative
  no-hot-reload Mac build and all 32 tests passed. Fresh-process saved-map and
  UI-art validators accepted 57 Fab actors plus 17 original placements, and
  initialized-world route/shelter/boundary/chest traces emitted their success
  marker without `LogPython: Error`.
- Clean PIE accepted the grounded chest-side quartermaster, Mara's readable
  shelter composition, and the open route. Separate transform-measured runs
  proved Q movement plus exact W and S autorun cancellation on the final map.
- 2026-08-04: added the first complete Fenwatch vendor service without moving
  economy rules into quartermaster art. `AEmbermereVendorServiceActor` owns an
  interactable and `UEmbermereVendorComponent` but no mesh or collision;
  `AEmbermereNpcPresentationActor` remains art-only.
- Added player copper, `UEmbermereVendorStockData`, finite/unlimited runtime
  stock, complete transaction preflight, exact charge/grant behavior, refund
  on an unexpected add failure, and mutation-free full-bag, sold-out, and
  insufficient-funds rejection.
- Saved `DA_FenwatchQuartermasterStock` with unlimited Marsh Tonic at 8 copper
  and one Recruit Pack at 30, plus the co-located art-free
  `Embermere_FenwatchQuartermaster_Service_01`. The focused fresh-process
  validator locks stock, transform, tags, references, and art/service
  separation.
- Built a fixed native `Fenwatch Supplies` panel with purse balance, four
  stable stock rows, item icons/details, Buy, close, readable two-line status,
  inventory exclusion, game/UI input, and bottom-left chat feedback.
- Added four focused tests for vendor transactions, service ownership, saved
  stock, and panel behavior. The authoritative no-hot-reload build and all
  36 tests passed; saved-map, UI-art, vendor, and initialized-world route
  validators passed without `LogPython: Error`.
- Clean live PIE proved normal `F` opening; 40 -> 32 copper tonic purchase;
  32 -> 2 finite Recruit Pack purchase; inventory updates; sold-out and
  insufficient-funds button states; non-overlapping result/footer copy; and
  close/input restoration.
- 2026-08-05: completed the two-sided Fenwatch economy loop. Data-driven sell
  values, exact-identity Sell/latest Buyback, wallet-overflow guards, and Mara's
  one-time 20-copper reward now preserve atomic wallet/inventory/stock state.
  Clean PIE proved `40 -> 32 -> 35 -> 32 -> 2 -> 22`; all 38 tests passed.
- 2026-08-06: added version-1 atomic persistence for copper, XP, inventory and
  equipped identities, quest state, and finite vendor stock through stable IDs
  and validated soft paths. A real fresh-world load restored the accepted
  22-copper/125-XP state twice without duplication, reward replay, stock reset,
  or equipment-stat inflation; all 40 tests passed.
- 2026-08-07: added the one-slot `Embermere Chronicle` player surface with
  non-mutating slot inspection, explicit overwrite/load confirmations, cancel
  paths, readable rejected-state feedback, panel handoff, and console-command
  fallbacks. Two-session PIE retained the accepted state; all 42 tests passed.
- 2026-08-08: completed the reusable NPC wrapper's skeletal Idle lane with soft
  animation data, Anim Blueprint precedence, compatibility checks, looping and
  play-rate controls, and explicit registered-component initialization. The
  live animation clock advanced at `0.75x`; all 43 tests passed.
- 2026-08-09: built the project-owned Fenwatch armsmaster and a separate,
  art-free trainer service. `DA_FenwatchArmsmasterOfferings` defines repeatable
  level-1 Combat Drills at 10 copper for 25 XP; preflight and exact refund keep
  the transaction atomic. The fixed `500x300` panel, chat, Inventory,
  Chronicle, and close handoffs all passed live PIE.
- The armsmaster's reviewed Blender source is 2,824 triangles; classic FBX
  correctly removes 24 degenerates and persists a 2,800-triangle Unreal mesh.
  The source and imported topology counts are intentionally separate acceptance
  metrics. The art-only and service actors remain co-located but independent at
  `(-1320, -920, 0)`, yaw `100`.
- Clean PIE proved the real trainer chain from 40 copper/0 XP to 30 copper/25
  XP, then exhaustion to zero copper with mutation-free insufficient-funds
  feedback. The no-hot-reload build, all 48 tests, trainer/map/UI validators,
  and initialized-world route traces passed.
- 2026-08-10: proved trainer-produced progression through the real Chronicle
  lifecycle without changing save version 1. A fresh `40`/`0` world trained to
  `30`/`25`, saved through Chronicle, restored exactly in a second world, and
  stayed exact through a second load with no unrelated durable or transient
  state mutation.
- Built `SM_EmbermereFenwatchPracticeDummy_01` through a factory-clean Blender
  process after the live bridge correctly rejected a scene-resetting script.
  The accepted source is `252.0 x 100.879 x 245.0` cm, 2,572 triangles, five
  shared materials, two authored UBX boxes, a solid base/core, and clear arms.
- Replaced `FabPass_Village_Crate_C` with
  `Embermere_FenwatchPracticeDummy_TrainingYard_01` at
  `(-1120, -1120, 0)`, yaw `45`. Human viewport review rejected the first
  technically valid orientation, and the deterministic import/placement script
  resaved the corrected target-facing composition.
- All 48 tests passed again. Fresh-process practice-dummy, zone (`56` Fab plus
  `19` original), trainer, vendor-economy, and UI-art validators emitted their
  success markers with no `LogPython: Error`; initialized-world native traces
  proved the dummy base/core solid and both arms clear.
- 2026-08-11: upgraded the accepted armsmaster wrapper to its own deterministic
  skeletal lane while preserving the original static mesh as a reversible
  fallback. The source retains 2,824 triangles, six materials, grounded bounds,
  and adds nine authored bones with complete rigid weights plus a 3.2-second
  Idle.
- Classic FBX adds one imported Armature root, so Unreal's accepted reference
  skeleton has ten bones while retaining all nine authored names. The importer
  now replaces packages in place, reuses the valid Skeleton, assigns material
  structs through Unreal Python's supported API, and saves every generated
  package explicitly.
- The no-hot-reload build and all 49 tests passed. Fresh rig, trainer, zone,
  practice-dummy, UI-art, and initialized-world route validators passed; clean
  PIE kept the Idle `playing=true` while its clock advanced from `0.193888` to
  `1.670905` seconds with the service boundary and route intact.
- 2026-08-12: upgraded the Fenwatch quartermaster through the same production
  skeletal lane without moving any vendor authority into art. The reviewed
  rig retains the exact static model's 3,632 triangles, grounded
  `120.842 x 93.0 x 217.0` cm bounds, and six materials, then adds nine
  authored bones, complete rigid weights, and a 121-frame, 30-fps,
  4.0-second Idle. Unreal deliberately retains one additional imported
  Armature root.
- UE 5.8 exposed a stricter import lifecycle during the quartermaster pass.
  Replacing an existing skeletal package could silently route through
  Interchange even when a classic `FbxFactory` was supplied. Normal reruns now
  validate eligible classic-FBX packages without importing; intentional source
  rebuilds run a narrow cleanup-only commandlet, a fresh classic-FBX creation
  process, and a separate fresh validation process.
- The no-hot-reload build and all 50 tests passed. Fresh quartermaster-rig,
  armsmaster-rig, vendor, trainer, practice-dummy, saved-zone, and UI-art
  validators passed. Initialized-world traces retained the spawn corridor,
  shelter, three gate lanes, solid chest and dummy core, and clear dummy arms.
  Clean PIE kept the quartermaster `playing=true` while its Idle advanced from
  `0.853735` to `2.195707` seconds; normal-camera review retained grounded
  chest-side composition and a clear service-cluster route.
- 2026-08-13: moved only Mara's keeper art onto the reusable skeletal wrapper
  while leaving the original `BP_QuestGiver`, interactable, dialogue, quest,
  marker, and reward authority intact. The deterministic keeper rig retains the
  accepted 3,280 triangles, six materials, and grounded
  `107.45 x 71.0 x 207.5` cm silhouette, then adds nine authored bones,
  complete rigid weights, and a 109-frame, 30-fps, exact 3.6-second Idle.
- The dormant Blueprint SCS static component keeps its authored transform and
  `NoCollision` state but no longer renders. The colocated art-only wrapper
  prefers the rig and Idle, retains the exact static fallback, and carries the
  project-owned art tag without gaining any quest or interaction component.
- The no-hot-reload build passed. Headless and freshly restarted editor MCP
  suites each passed all 51 tests; fresh keeper-rig and full-zone validators
  passed; initialized-editor validators retained the complete service, UI, and
  route baseline. Clean PIE kept Mara grounded beneath the shelter with her
  marker/name unobstructed, and two clock probes advanced the Idle, including
  `0.333814 -> 1.525603` seconds on the fresh module. The remaining manual gate
  was to walk into her real `F` radius and complete the dialogue/quest loop.
- 2026-08-14: physically accepted Mara's original gameplay ownership after the
  art extraction. A real `F` press opened the Blueprint-owned dialogue and
  accepted the quest, live combat against all three saved Prowlers advanced it
  to `3/3`, and the original interactable completed the return for exactly
  `125` XP, `20` copper, and one Recruit Pack. The rigged wrapper remained
  grounded, looping, non-colliding, and free of quest/interaction authority.
- Built and placed `SM_EmbermereFenwatchVendorStall_01` through the reviewed
  deterministic Blender/classic-FBX lane. The accepted asset is
  `394 x 242 x 306` cm, 5,476 triangles, five shared materials, four authored
  support colliders, and one counter collider. A live trace exposed classic
  FBX's local-Y mirror; rotating the saved placement to yaw `180` put the
  counter on the customer side instead of accepting a plausible backward
  composition. The stall replaced `FabPass_Village_Fence_01`, producing the
  current 55 Fab/20 original-art baseline.
- Focused PIE opened Fenwatch Supplies through normal `F` at the unchanged
  art-free service. Native traces passed four supports, the counter, service
  approach, and east bypass. The no-hot-reload build, all 51 tests, and fresh
  vendor-stall, saved-zone, UI-art, three NPC-rig, practice-dummy, vendor, and
  trainer validators passed with explicit success markers.
- 2026-08-15: added a fixed read-only contextual greeting to Mara's rigged
  presentation wrapper. The quest data asset owns four short lines; the wrapper
  explicitly observes the original `BP_QuestGiver` and authoritative player
  quest log, remains `NoCollision` and hit-test-invisible, and owns no marker,
  dialogue, interaction, progression, or reward behavior.
- Clean PIE accepted hidden-out-of-range plus available, active, ready, and
  completed states. Physical `F` preserved the original dialogue and exact
  `125` XP, `20` copper, and one-Recruit-Pack completion; a second `F` replayed
  nothing. The no-hot-reload build, all 52 tests, dedicated greeting/keeper/
  full-zone validators, existing package validators, and initialized-world
  vendor-stall and road traces passed.
- 2026-08-16: built Fenwatch's first closed project-owned cottage through the
  reviewed deterministic Blender/classic-FBX lane. The accepted asset is
  `580 x 422 x 503` cm, 6,616 triangles, one UV channel, five shared materials,
  clean topology, and two authored body/doorstep collision boxes. The roof,
  chimney, trim, windows, and ember crest remain visual-only.
- The deterministic preview initially clipped the upper roof; the tracked
  camera now uses a `5000` far clip. Original-resolution review then caught and
  repaired visible door-jamb gaps before Unreal import.
- Placed `Embermere_FenwatchCottage_West_01` at `(-2480, -260, 0)`, yaw `38`,
  replacing only `FabPass_Village_Fence_02`. Fresh validation proved exact
  provenance, metrics, materials, transform, tag, two colliders, `730.1` cm
  route clearance, and `730.1` cm Mara separation. Live traces proved the body
  and step solid, roof/chimney decoration clear, and both the direct Mara lane
  and west bypass open.
- Clean PIE preserved Mara, her greeting, and the service-cluster composition;
  physical `F` still opened the original dialogue and accepted the quest. The
  no-hot-reload Mac build succeeded, the live MCP and fresh-process suites both
  passed 52/52, all focused/saved-package validators passed, and the map now
  contains 54 grounded Fab actors plus 21 original-art placements.
- 2026-08-17: built `SM_EmbermereFenwatchTrainingWorkshop_01` through a
  factory-clean deterministic Blender pass. The accepted open-front module is
  `460 x 270.403 x 369` cm, 5,624 triangles, one UV channel, clean topology,
  five shared Fenwatch materials, and four UBX boxes for the two front posts,
  rear wall, and workbench. Roof, trim, ember crest, and tools remain
  decorative and non-colliding.
- Imported through classic `FbxFactory`, explicitly saved the package, and
  placed `Embermere_FenwatchTrainingWorkshop_Armsmaster_01` at
  `(-690, -1030, 0)`, yaw `-100`, replacing only
  `FabPass_Village_Fence_03`. Exact validation locks 439.3 cm dummy spacing,
  639.5 cm armsmaster spacing, and 401.6 cm road-pine spacing.
- Initialized-world traces proved both posts, rear wall, and workbench solid;
  the open-front center, player-height bay, armsmaster-to-dummy approach, and
  road-side east bypass clear; and roof/trim/crest/tools excluded from authored
  collision. The first east-bypass assertion began inside an existing road
  lamp, so the trace start was corrected before acceptance rather than moving
  valid world art to satisfy a bad test.
- Clean PIE preserved the real Mara `F` quest handoff and independently proved
  `Q` movement plus both `W` and `S` cancellation. The no-hot-reload build and
  authoritative 52/52 suite passed with zero warnings; all focused package,
  full-zone, UI, NPC-rig, vendor, trainer, cottage, stall, workshop, and native
  route validators passed. The map now contains 53 grounded Fab actors plus 22
  original-art placements.
- 2026-08-18: separated the accepted Fenwatch practice-dummy art from a new
  native `AEmbermerePracticeTargetActor` gameplay authority at the same saved
  transform. The actor supplies no mesh or collision, but reuses normal `Tab`,
  ability damage, nameplate, and 48-segment cyan-circle presentation around a
  150-health, three-second-reset training target.
- Added `ShouldGrantDefeatCredit` to the targetable contract and centralized
  native-versus-Blueprint interface dispatch. Ordinary enemies still grant
  Mara objective progress; the practice target explicitly grants no quest
  credit, loot, XP, inventory mutation, AI, aggro, retaliation, leash, trainer
  behavior, or persistence state.
- Fresh PIE exposed that a collision-free `ACharacter` still falls under
  CharacterMovement gravity. The target now freezes at its authored transform
  with gravity `0`, zero velocity, and `MOVE_None` in construction, BeginPlay,
  and reset paths. Its fixed ring-grounding policy also avoids tracing the
  separate dummy's own solid core as if it were terrain.
- Six real hotbar Strikes dealt `28, 28, 28, 28, 28, 10`, cleared selection on
  defeat, kept player health `100`, XP `0`, inventory stacks `0`, and quest
  progress `0`, then restored `150/150` at the exact transform after three
  seconds. `Tab` immediately reacquired the reset target and restored the
  native nameplate and cyan circle.
- The no-hot-reload Mac build passed and an actually isolated commandlet suite
  passed 54/54. The aggregate 13-package validator, full-zone validator,
  focused saved practice-target validator, initialized-world workshop/cottage/
  stall/road traces, and clean PIE all passed. The map remains at 53 grounded
  Fab actors and 22 original-art placements because the new actor is gameplay,
  not art.
- 2026-08-19: added an immutable post-commit combat-result contract carrying
  source, target, stable ability ID, exact applied amount, result kind, and
  lethal state without changing the established cooldown event or giving
  presentation any gameplay authority.
- Added a standalone hit-test-invisible native floating-feedback observer with
  three fixed `112x32` slots, newest-first rapid stacking, 1.25-second rise/
  fade lifetime, exact damage copy, future `MISS` support, and deterministic
  expiry, target-switch, defeat, reset, invalidation, and teardown clearing.
- Replaced generic actor-bounds placement with an explicit targetable combat-
  feedback anchor. Screen-space nameplates no longer inflate placement, and
  reflected anchors from older saved Blueprint classes are validated before a
  native fallback handles zeroed stale-event results.
- Clean normal-camera PIE accepted fixed `104x30` floating `28` results beside
  both the Fenwatch Practice Target and a saved Marsh Prowler, including a
  16-pixel nameplate gap, intact cyan circle, exact HP/chat updates, immediate
  deselect/lethal clearing, and no stale result after practice-target reset.
- The authoritative no-hot-reload build passed, all 56 tests passed, and the
  sequential 13-package aggregate validator retained the exact 53 grounded Fab
  plus 22 original-art baseline with no intentional map or vendor resave.
- 2026-08-20: built `SM_EmbermereFenwatchNoticeBoard_01` through a
  factory-clean deterministic Blender pass. The accepted roadside module is
  `286 x 93.927 x 277` cm, 3,684 triangles, one UV channel, clean topology,
  five shared Fenwatch materials, and three UBX boxes for the two supports and
  central panel. Roof, notices, pins, trim, and ember crest remain decorative.
- Imported through classic `FbxFactory`, explicitly saved the mesh and map,
  and placed `Embermere_FenwatchNoticeBoard_Road_01` at
  `(-1560, -260, 0)`, yaw `-35`. The board is presentation-only and owns no
  quest, marker, dialogue, interaction, reward, vendor, trainer, or persistence
  behavior.
- Fresh package validation locks provenance, metrics, materials, collision,
  tag, and transform. Post-restart initialized-world traces proved both
  supports and the panel solid, decorative geometry clear, and the
  PlayerStart-to-Mara, village-to-road, quartermaster, and armsmaster routes
  open. Normal-camera PIE accepted its grounded road-side read.
- The no-hot-reload Mac build succeeded, all 56 tests passed in an isolated
  commandlet, and the sequential aggregate expanded to 14 package validators
  with explicit success markers and no `LogPython: Error`. The map now retains
  53 grounded Fab actors plus 23 original-art placements.

## Asset Hunt

Start with free Unreal/Fab assets:

- stylized modular fantasy village kit;
- stylized forest/foliage pack;
- ruins/stone props;
- fantasy UI/icons;
- simple fantasy enemy or creature pack;
- weapon pack for sword, shield, staff, and bow.

Track candidates and install notes in [Docs/FAB_ASSET_PLAN.md](Docs/FAB_ASSET_PLAN.md).

## Daily Wrap

Before stopping each day:

- build the project;
- run available automation tests;
- note anything requiring manual PIE verification;
- commit and push intentional changes;
- update this file with the next starting point.
