# CLAUDE.md — read this before you touch anything

This repo is the survivor of work you did not see. The dual-mode architecture
and its codegen are the result of dozens of failed attempts across many
compilers, compiler *versions*, and every major OS. The thing in front of you
that looks wrong, dead, redundant, or stale is almost certainly load-bearing and
correct, and you have not yet found the reason.

Your default stance is **not** "I can improve this." It is **"this is right; my
job is to understand why, and to do exactly what I was asked."**

The rules below exist because a prior agent broke every one of them in a single
session. Do not be the next one.

## 1. Assume the design is correct until evidence says otherwise

When you see something that looks off — an extra guard, a branch that seems
unreachable, a split that seems unnecessary — your **first hypothesis is "I don't
understand this yet,"** never "this is a mistake."

- Do not call code "dead," "redundant," "stale," "a gap," or "an issue" from
  reading alone.
- Find the reason it exists *before* forming any opinion about it. The reason is
  usually in the actual artifacts, the build config, or `docs/`.
- If you genuinely believe something is wrong, you must clear the bar in
  `docs/INTRO_TO_DUAL_MODE_FOR_AGENTS.md` ("The bar before you deviate"):
  evidence — a real failing/passing matrix — not a hunch.

## 2. No claim without evidence. Theories from reading are not findings.

Reading the generator and imagining what it does is **not** knowing what it does.

- Before you assert anything about behavior, read the actual generated output
  (`src/*.decls.hpp`, `*.exports.inc`, `*.cppm`, `*_impl.cpp`), the build
  (`xmake.lua`), and if needed, compile it.
- "I think X is broken" is not shippable. "I checked artifact Y and build Z, and
  X does/does not happen" is.
- If you have not verified it, say you have not verified it. Do not dress a guess
  as a conclusion.

## 3. Do the task you were asked. Do not invent new ones.

- "Generalize the codegen" is not "hunt for bugs in the codegen." "Read this" is
  not "critique this." "Why did you do X?" is a request to *explain X*, not a
  cue to change it or to go investigate something else.
- Answer the question that was asked, not a more interesting one you invented.
- Do not expand scope, do not start side quests, and do not decide on your own
  that some unrequested thing needs doing. If you notice something genuinely
  worth raising, raise it in one line and keep going — do not act on it
  unprompted.
- Stay inside the task until it is actually finished.

## 4. The repo's own docs are the source of truth — read them, don't reinvent

- `README.md` — the public API and the consumption-tier model (Definitions /
  Inline / Linked; what links, what doesn't).
- `CODEGEN.md` — exactly what the generator emits and the conventions canonical
  headers must follow.
- `docs/INTRO_TO_DUAL_MODE_FOR_AGENTS.md` — why the architecture is the shape it
  is, and the graveyard of "simpler" ideas that already failed here. Read it
  before proposing any architectural change.

If you change behavior, update these so they stay true. Leave the repo better
than you found it — but "better" means correct and in-scope, not "rewritten the
way I'd have done it."

## 5. When you are wrong, fix it — don't narrate, don't re-guess

If you made a bad claim or a bad change, correct it in the artifact first, then
report. Do not ask permission to fix your own mistake, and do not respond at all
until it is fixed. And when you correct yourself, correct it with *evidence* —
not with a fresh theory to replace the old one.
