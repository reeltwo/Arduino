#!/bin/sh
##################################################
# This script should be installed in /home/pi/roms
##################################################
ARDUINO_ROOT=/usr/share/arduino
ARDUINO_TOOLS=${ARDUINO_ROOT}/hardware/tools
AVRDUDE="${ARDUINO_TOOLS}/avr/bin/avrdude -C ${ARDUINO_TOOLS}/avr/etc/avrdude.conf"
AVRDUDE_OPTS=-q

PORT=/dev/ttyUSB0
TARGET=Mega2560
ARDUINO_FQBN=mega:cpu=atmega2560
UPLOAD_DEVICE=atmega2560
BAUDRATE=115200
AVRDUDE_OPTS="${AVRDUDE_OPTS} -cwiring"

ID=$1
echo ID : $1
ROM=/home/pi/roms/"$ID".ino.hex
md5sum "${ROM}" | awk '{print $1}' > "${ROM}".newsig
if [ -f "${ROM}".flashsig ]; then
 diff "${ROM}".newsig "${ROM}".flashsig
 if [ $? -eq 0 ]; then
  echo Already flashed with "${ROM}"
  exit 0
 fi
fi
sudo systemctl stop "RSeries$1"
${AVRDUDE} -p${UPLOAD_DEVICE} -carduino ${AVRDUDE_OPTS} -P${PORT} -b${BAUDRATE} -D -U flash:w:"${ROM}":i
if [ $? -eq 0 ]; then
  md5sum "${ROM}" | awk '{print $1}' > "${ROM}".flashsig 
  sudo systemctl start "RSeries$1"
fi
