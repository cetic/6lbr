#!/bin/sh

echo "Reset and scan USB devices..."

#Rebind USB controller
echo "usb1" > /sys/bus/usb/drivers/usb/unbind
sleep 1
echo "usb1" > /sys/bus/usb/drivers/usb/bind
sleep 1

#Disable auto-suspend
echo "on" > /sys/bus/usb/devices/usb1/power/control

#Force USB controller scan
cat /dev/bus/usb/001/001 > /dev/null
