# add-only

Change a program's behavior by **adding a file**, never by editing an existing
one. The point is conflict-free parallelism: if every change is its own new
file and no file edits another, then many people (or many agents) can work at
once and their work always unions cleanly. There is nothing to merge.

## How it works

The core is written once with **seams**: instead of hard-coding a step, it reads
it from a `Registry`. An override is a new file under `overrides/` that, at
program startup, registers itself into a seam. No existing file is touched.

```
core/registry.h     generic "newest wins" keyed registry (header-only)
core/pricing.*      seam #1: adjustments that STACK (every key's winner applies)
core/receipt.*      seam #2: a SINGLE renderer (one winner)
app/main.cpp        the program
overrides/*.cpp     the changes, one file each, self-registering
```

Three facts make it work, in order of how-surprising:

1. **A global variable is initialized before `main`.** Each override ends with
   `const bool _registered = ...set(key, recency, fn);`. Creating that global
   runs `set`, which pins the override into the registry. The file wires itself
   in just by being compiled in. (`build.sh` compiles every `overrides/*.cpp`.)
2. **The registry is a runtime list, not a file.** Two overrides each carry
   their own `set(...)` line in their own file, so no shared source is edited.
   The combined list is assembled in memory at startup. Git never sees a shared
   line, so there is never a conflict.
3. **Conflicts are resolved by an explicit recency stamp, not arrival order.**
   Static-initialization order across files is unspecified, so "who registered
   last" is unreliable. Each override carries a recency (the local time in ms at
   scaffold time). For a given key, the highest recency wins. The comparison is
   order-independent, so the winner is the same no matter how the linker ordered
   things.

## Conflict policy: one winner, most recent

When two overrides claim the same key, the newer recency wins; the older is
ignored (it still compiles in, it just loses). So to change an existing
behavior, drop a new override for the same key: being newer, it supersedes the
old one with no edit to anything.

`overrides/tax_v1.cpp` (8%) and `tax_v2.cpp` (5%) both claim `100-tax`. `tax_v2`
is newer, so the app charges 5%.

## Apply order (for stacking seams)

Adjustments apply in **key order** (the registry is a sorted map). Prefix keys
with a number to sequence them: `100-tax` runs before `200-discount`.

## Use

```sh
./build.sh                       # compile core + every override into build/app
./build/app                      # run it
./verify.sh                      # build and assert expected output

./override adjust shipping 150-shipping   # scaffold a stacking adjustment
./override render plain                    # scaffold a replacement renderer
# edit the generated overrides/<name>.cpp body, then ./build.sh
```

## The rule that keeps it conflict-free

```sh
tools/additive-guard.sh          # fails if any change touches anything but overrides/
```

This is the structural enforcement: contributors (human or agent) may only add
files under `overrides/`. Because no one edits shared bytes, parallel work never
conflicts. The guard is what makes that a guarantee instead of a convention.

## Limits (be honest)

- **Build-time + restart.** Overrides take effect on the next compile and run,
  not live.
- **Recency is the local clock at scaffold time.** Fine on one machine. Across
  machines with skewed clocks the ordering can lie; use git commit time instead
  if that matters.
- **Stacking vs replacing collisions differ.** Two overrides of *different*
  adjustment keys both apply (by design). Two of the *same* key is a real
  contest that recency settles. The guard prevents *git* conflicts, not the
  logical decision of who should win.
