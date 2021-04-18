#!/usr/bin/env bash
DebugName="qjoypad-debug"
ReleaseName="qjoypad-release"
StoreName="qjoypad-ahimta"

get_snap_name() {
  local Variant=$1

  case  $Variant  in
  "debug")
    echo $DebugName
    return 0
    ;;
  "release")
    echo $ReleaseName
    return 0
    ;;
  "store")
    echo $StoreName
    return 0
    ;;
  *)
    print_variant_error_help_and_exit "$Variant"
    exit 1
    ;;
  esac
}

print_variant_error_help_and_exit() {
  local Variant=$1

  echo "Error: Incorrect or missing variant parameter of '$Variant'." 1>&2
  echo "<variant> must be one of the following:" 1>&2
  echo "- 'debug': For general local development and testing. And will result in '$DebugName' after installing the resulting snap." 1>&2
  echo "- 'release': For testing the closest possible package to the Snap store. And will result in '$ReleaseName' after installing the resulting snap." 1>&2
  echo "- 'store': For releasing the package to the Snap store. And will result in '$StoreName' after installing the resulting snap." 1>&2

  return 0
}
