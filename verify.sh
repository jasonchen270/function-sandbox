#!/bin/sh
# Build and assert the app behaves as the overrides dictate. This is the
# regression check: it proves newest-wins (5% tax, not 8%), stacking order
# (tax before discount), and the render override (fancy receipt).
set -e
cd "$(dirname "$0")"

./build.sh >/dev/null
got="$(./build/app)"

# Cart{200} -> tax winner 5% -> 210 -> discount -5 -> 205, fancy receipt.
read -r -d '' want <<'EOF' || true
=== RECEIPT ===
  subtotal: $200.00
  total:    $205.00
===============
EOF

if [ "$got" = "$want" ]; then
    echo "PASS"
    exit 0
fi
echo "FAIL"
echo "--- got ---";  printf '%s\n' "$got"
echo "--- want ---"; printf '%s\n' "$want"
exit 1
