# Embermere Practice Target Contract

The Fenwatch practice dummy is world art. The training workshop is world art.
Neither asset owns combat, targeting, rewards, trainer services, or quest state.
The repeatable training loop belongs to a separate native gameplay actor placed
at the dummy's transform.

## Authority Boundary

`AEmbermerePracticeTargetActor` owns only:

- target eligibility through `IEmbermereTargetable`;
- a readable display name, health pool, nameplate, and cyan target circle;
- accepting normal ability damage and temporary control effects;
- a short defeated state followed by a full-health reset;
- freezing the inherited character movement at the authored transform;
- presentation refresh needed to repeat that loop.

It explicitly does not own:

- static or skeletal character art;
- collision, navigation, aggro, chasing, retaliation, or leashing;
- loot, XP, quest credit, wallet changes, or inventory mutation;
- trainer offerings, interaction prompts, dialogue, markers, or service UI;
- persistence in save version 1.

The separate `Embermere_FenwatchPracticeDummy_TrainingYard_01` static-mesh
actor continues to own the visible timber, stone, moss, iron, and ember model
and its two purposeful solid colliders. The separate
`Embermere_FenwatchTrainingWorkshop_Armsmaster_01` actor continues to own the
surrounding architecture. The existing art-free armsmaster service remains the
only owner of Combat Drills interaction and progression.

## Defeat Credit

Targetability and reward eligibility are independent contracts. The
`IEmbermereTargetable::ShouldGrantDefeatCredit` policy defaults to false on a
general Embermere character, is enabled by ordinary enemies, and is disabled
by the practice target. Combat checks that policy before advancing
`StarterEnemyDefeated`, so defeating a dummy cannot complete Mara's quest.

The practice actor also disables enemy loot at the source. Embermere currently
does not grant generic kill XP, and focused automation locks the expected zero
XP and zero-inventory-mutation result so a later reward system cannot silently
turn training into farming.

## Reset Contract

The accepted prototype target has 150 health and a three-second defeated
window. A reset restores full health and clears timed effects while preserving
the no-collision policy. Construction, `BeginPlay`, and reset all enforce zero
gravity, zero velocity, and `MOVE_None`; disabling collision alone does not stop
an inherited `ACharacter` from falling. Normal lethal combat clears the
player's target before the reset. An explicit manual reset preserves an active
selection, which keeps debug and future trainer-driven resets from leaving
`CurrentTarget` and world presentation out of sync.

## Presentation Contract

The gameplay actor reuses the established native enemy nameplate and complete
48-segment cyan target circle, but supplies no model. Its 150 cm ring clears
the dummy's broad target face and arms. Because a vertical trace through the
dummy would hit the dummy's own solid core, this actor uses a fixed 16 cm
ground offset instead of interpreting separate art collision as terrain.

## Acceptance Gates

1. Native automation proves target eligibility, policy defaults, no AI,
   collision, loot, reward, service, or art ownership, damage/control-effect
   acceptance, lethal target clearing, exact quest/XP/inventory exclusion, and
   repeatable reset.
2. A fresh saved-map validator proves exactly one native gameplay actor at the
   accepted dummy transform, requires its stationary movement contract, and
   confirms the visible dummy remains a separate static-mesh actor.
3. Clean PIE proves `Tab` selection, cyan ring and nameplate readability,
   normal hotbar damage, three-second reset, unchanged Mara objective count,
   no retaliation or movement, and an unaffected armsmaster training loop.

The rule is simple: the dummy can be replaced without changing combat, and the
practice-target actor can be removed without changing the art or trainer.
