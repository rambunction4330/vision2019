#!/bin/sh
v4l2-ctl --device 1 \
 --set-ctrl brightness=0 \
 --set-ctrl contrast=0 \
 --set-ctrl saturation=255 \
 --set-ctrl gain=0 \
 --set-ctrl sharpness=0 \
 --set-ctrl exposure_auto=1 \
 --set-ctrl exposure_absolute=60 \
 --set-ctrl white_balance_temperature_auto=0 \
 --set-ctrl white_balance_temperature=6500
