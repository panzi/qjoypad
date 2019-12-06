#!/usr/bin/env bash
# This is a wrapper to uninstall a snap.
# Usage: ./scripts/uninstall-snap.sh <variant>

# shellcheck disable=SC1091
source ./scripts/_helpers.sh

Variant=$1

SnapName=$(get_snap_name "$Variant")

if test $? -ne 0; then
  exit $?
fi

sudo snap remove --purge "$SnapName"
