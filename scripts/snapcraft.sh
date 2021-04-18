#!/usr/bin/env bash
# This is a wrapper for `snapcraft` to make it easier to develop, test, and
# release without any conflict with the local installation.
# Usage: ./scripts/snapcraft.sh <variant> [snpacraft-arg...]

# shellcheck disable=SC1091
source ./scripts/_helpers.sh

get_version_part() {
  local PartName=$1

  local CMakeListsFile=CMakeLists.txt

  local PartLine
  local PartName
  PartLine=$(grep "$PartName" $CMakeListsFile)
  PartName=$(echo "$PartLine" | sed --regexp-extended 's#^.*([0-9]+).*$#\1#')

  echo "$PartName"
  return 0
}

Variant=$1
Name=$(get_snap_name "$Variant")

case  $Variant  in
"debug")
  Title="QJoyPad (Debug)"
  Grade="devel"
  ;;
"release")
  Title="QJoyPad (Release)"
  Grade="stable"
  ;;
"store")
  Title="QJoyPad"
  Grade="stable"
  ;;
"help"|"--help"|*)
  echo "Usage: ./scripts/snapcraft.sh <variant> [snpacraft-arg...]"
  exit 0
  ;;
esac

AllArgs=("$@")
RestArgs=("${AllArgs[@]:1}")
SnapcraftArgs=("${RestArgs[@]}")

SnapcraftFile=snap/snapcraft.yaml
SnapcraftTemplate=snap/local/snapcraft.scaffold.yaml

VersionMajor=$(get_version_part QJOYPAD_MAJOR)
VersionMinor=$(get_version_part QJOYPAD_MINOR)
VersionPatch=$(get_version_part QJOYPAD_PATCH)
VersionGit=$(git rev-parse --short HEAD)
Version="$VersionMajor.$VersionMinor.$VersionPatch+git-$VersionGit"

rm --force ./*.snap
rm --force $SnapcraftFile

cp $SnapcraftTemplate $SnapcraftFile

GenerationNote="# NOTE: This file is generated using 'scripts/snapcraft.sh'."
sed --in-place "1s|^|$GenerationNote\n|" $SnapcraftFile

sed --in-place "s|@@TITLE@@|$Title|g" $SnapcraftFile
sed --in-place "s|@@NAME@@|$Name|g" $SnapcraftFile
sed --in-place "s|@@GRADE@@|$Grade|g" $SnapcraftFile
sed --in-place "s|@@VERSION@@|$Version|g" $SnapcraftFile

snapcraft "${SnapcraftArgs[@]}"
