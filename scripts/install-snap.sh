#!/usr/bin/env bash
# This is a wrapper to install a snap and set appropriate
# connections/permissions.
# Usage: ./scripts/install-snap.sh <variant> <snap-file-path>

# shellcheck disable=SC1091
source ./scripts/_helpers.sh

Variant=$1
SnapFilePath=$2

SnapName=$(get_snap_name "$Variant")

if test $? -ne 0; then
  echo "Usage: ./scripts/install-snap.sh <variant> <snap-file-path>"
  exit $?
fi

sudo snap install --dangerous "$SnapFilePath" &&\
  sudo snap connect "$SnapName:joystick" :joystick
