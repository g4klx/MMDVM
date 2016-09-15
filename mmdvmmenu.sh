#!/bin/bash

###############################################################################
#
# mmdvmmenu.sh
#
# Copyright (C) 2016 by Paul Nannery KC2VRJ
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
###############################################################################
#
# On a Linux based system, such as a Raspberry Pi, this script will perform
# Modafacation to the Config.c file for most options. It makes a Back up when
# you staet the script if none is present. You must recompile and load firmwhare
# on Due if changes are made.
#
###############################################################################
#
#                              CONFIGURATION
#
# Location of Config.h
conf=Config.h
#Location of backup file
confbak=Config.h.bak

################################################################################
#
# Do not edit below here
#
###############################################################################



# Check for backup file and make one if not present

if [ ! -f $confbak ];then

cp -f $conf $confbak

fi

while :
do
    clear
    cat<<EOF
    ==============================================================
                       MMDVM Configuration Options
    --------------------------------------------------------------
    Please enter your choice:

    (1) Enable 12.0 MHZ Clock
    (2) Enable 14.4 MHz Clock
    (3) Enable 19.2 MHz Clock
    (4) Use the COS to lockout the modem
    (5) Use pins to output the current mode
    (6) Use layout for the PAPA board
    (7) Use layout for ZUM board V1.0 and V1.2
    (8) Use layout for SP8NTH board
    (9) Return to Default

                         (Q)uit
    ---------------------------------------------------------------
EOF
    read -n1 -s
    case "$REPLY" in
    "1")  sed -e 's/\/\/ #define EXTERNAL_OSC 12000000/#define EXTERNAL_OSC 12000000/' $conf > $conf.tmp && mv -f $conf.tmp $conf && echo "12.0 MHz clock enabled";;
    "2")  sed -e 's/\/\/ #define EXTERNAL_OSC 14400000/#define EXTERNAL_OSC 14400000/' $conf > $conf.tmp && mv -f $conf.tmp $conf && echo "14.4 MHz clock enabled";;
    "3")  sed -e 's/\/\/ #define EXTERNAL_OSC 19200000/#define EXTERNAL_OSC 19200000/' $conf > $conf.tmp && mv -f $conf.tmp $conf && echo "19.2 MHz clock enabled";;
    "4")  sed -e 's/\/\/ #define USE_COS_AS_LOCKOUT /#define USE_COS_AS_LOCKOUT/'      $conf > $conf.tmp && mv -f $conf.tmp $conf && echo "COS as Lockout enabled";;
    "5")  sed -e 's/\/\/ #define ARDUINO_MODE_PINS/#define ARDUINO_MODE_PINS/'         $conf > $conf.tmp && mv -f $conf.tmp $conf && echo "Mode pins Enabled";;
    "6")  sed -e 's/\/\/ #define ARDUINO_DUE_PAPA/#define  ARDUINO_DUE_PAPA/'          $conf > $conf.tmp && mv -f $conf.tmp $conf && echo "Layout for the PAPA board enabled";;
    "7")  sed -e 's/\/\/ #define ARDUINO_DUE_ZUM_V10/#define ARDUINO_DUE_ZUM_V10/'     $conf > $conf.tmp && mv -f $conf.tmp $conf && echo "Layout for ZUM board V1.0 and V1.2 enabled";;
    "8")  sed -e 's/\/\/ #define ARDUINO_DUE_NTH/#define ARDUINO_DUE_NTH/'             $conf > $conf.tmp && mv -f $conf.tmp $conf && echo "Layout for SP8NTH board enabled";;
    "9")  mv -f $confbak $conf ;;
    "Q")  echo "If any changes are made you need to (re-)upload the firmware to MMDVM" && exit;;
    "q")  echo "If any changes are made you need to (re-)upload the firmware to MMDVM" && exit;;
     * )  echo "invalid option"     ;;
    esac
    sleep 1
done
