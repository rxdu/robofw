#!/bin/bash

# bring up can interface
sudo ip link set can0 up type can bitrate 250000
sudo ip link set can0 txqueuelen 10000
