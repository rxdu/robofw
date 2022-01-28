#!/bin/bash

START_STR='"'
END_STR='"'
echo "CONFIG_BOOT_SIGNATURE_KEY_FILE=$START_STR`pwd`/robofw.pem$END_STR" > mcuboot.conf