#!/bin/sh
v4l2-ctl --device 1 \
 --set-ctrl brightness=0 \
 --set-ctrl contrast=255 \
 --set-ctrl saturation=255 \
 --set-ctrl sharpness=0 \
 --set-ctrl exposure_auto=1 \
 --set-ctrl exposure_absolute=5 
