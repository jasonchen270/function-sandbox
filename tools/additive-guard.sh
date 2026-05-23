#!/bin/sh
# additive guard: the structural rule that makes this conflict-free.
#
# A change is only allowed to ADD files under overrides/. Any change to core/,
# app/, the tooling, or any existing file is rejected. This is what guarantees
# two agents never touch the same bytes, so their work always unions cleanly.
# Run it as a pre-commit check, or by hand.
set -e
cd "$(dirname "$0")/.."

# Every path with a pending change (staged, unstaged, or untracked).
changed="$(git status --porcelain | sed 's/^...//; s/.* -> //')"

bad=""
for p in $changed; do
    case "$p" in
        overrides/*) : ;;          # allowed
        *) bad="$bad $p" ;;        # everything else is frozen
    esac
done

if [ -n "$bad" ]; then
    echo "ADDITIVE GUARD: changes outside overrides/ are not allowed:" >&2
    for p in $bad; do echo "  $p" >&2; done
    exit 1
fi
echo "additive guard: OK (changes are confined to overrides/)"
