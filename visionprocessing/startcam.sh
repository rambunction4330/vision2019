#!/bin/sh
v4l2-ctl --set-ctrl brightness=0
v4l2-ctl --set-ctrl contrast=0
v4l2-ctl --set-ctrl saturation=255
v4l2-ctl --set-ctrl gain=0
v4l2-ctl --set-ctrl sharpness=0
v4l2-ctl --set-ctrl exposure_auto=1
v4l2-ctl --set-ctrl exposure_absolute=60
v4l2-ctl --set-ctrl white_balance_temperature_auto=0
v4l2-ctl --set-ctrl white_balance_temperature=6500
