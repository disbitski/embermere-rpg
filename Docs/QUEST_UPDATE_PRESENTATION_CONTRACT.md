# Quest Update Presentation

## Authority

The quest log publishes a copied, immutable native result after a successful
live acceptance, progress, readiness, or reward commit. It contains stable
quest/objective IDs, authored title/instruction, previous/current/required
counts, a kind, and a session-only sequence. Reaching the objective cap emits
one Ready result, not Progress plus Ready. Rejected, duplicate, wrong-ID,
focus-only, and restore operations emit no live result. Restore instead emits
a presentation-reset signal. Neither signal is serialized.

The existing state-change delegate remains available to Blueprint consumers.
The new delegate does not replace quest, reward, or persistence ownership.
Mutation is guarded across synchronous callbacks; result payloads never refer
to mutable array elements. Progress saturates without signed integer overflow.

## Observer

A removable native widget consumes only these results. One fixed 420x108
surface sits at the top right, below the Inventory command. It is hit-test
invisible. The latest valid result replaces the previous result; there is no
queue, history, accumulating widget list, or delayed replay. A result lasts
3 seconds, including a final 0.4-second fade. A duplicate/older sequence is
ignored. Title and description remain clipped/wrapped within fixed regions.

Inventory and peer modal panels suppress and clear the observer. Death, load,
unbinding, expiry, and teardown clear it. A fresh binding never synthesizes
past events. Chat and the quest ledger remain durable fallbacks. The observer
cannot change focus, quests, stats, wallet, inventory, rewards, or save data.

## Art

The optional 48x48 journal emblem is rendered from deterministic project-owned
Blender geometry. Its moss cover, pale pages, iron clasp, and ember diamond
reuse the Fenwatch visual vocabulary. It is decoration, not a button or item.
The observer remains fully usable with no texture. Build in a factory-clean
headless process through Blender MCP; leave the live scene and Safe Mode alone.

## Acceptance Gates

- Test exact post-commit payloads, cap saturation, rejection silence, reward
  state at callback time, reentrant mutation rejection, and restore silence.
- Test latest-wins, duplicates, fixed geometry, expiry, death, modal
  suppression, unbinding, and repeated restore without gameplay drift.
- Build without hot reload, pass the complete suite and saved-package checks.
- In PIE inspect real F acceptance, live progress, ready, and completion,
  including overlap with dialogue, reward, XP, target, and peer panels.
- Inspect the Blender source/render and imported texture independently.

Status: implemented; final isolated automation passed 91/91 and all 21
saved-package validators passed with explicit markers and no Python errors.
All six initialized-world route/collision validators also passed after the
final restart. Save version 3 and all map placements are unchanged.

After local unlock, fresh Q traversal plus W cancellation and real F accepted
Mara's quest. A labeled temporary objective fixture showed 1/3 progress and
3/3 ready, followed by real F completion: 125 XP, 20 copper, one Recruit Pack.
The completion, reward, and level-up surfaces had clear separation. Inventory
cleared the observer; closing it did not replay it. Full real-combat/Still
Waters routes, remaining peer panels, and held mouse/modifier checks remain
open. Injected progress is not combat proof. The old bottom dialogue/hotbar
overlap is outside this observer. See the dated Astra evaluation report.
