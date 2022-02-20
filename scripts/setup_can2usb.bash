#!/bin/bash

# enable kernel module: gs_usb
sudo modprobe gs_usb

# install can utils
sudo apt install -y can-utils
