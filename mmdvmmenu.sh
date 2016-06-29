#! /bin/bash

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
                       MMDVM COnfiguration Options
    --------------------------------------------------------------
    Please enter your choice:
 
    (1) Enable 12.0 MHZ CLock (4) Use the COS to lockout the modem
    (2) Enable 14.4 MHz Clock (5) Use pins to output the current mode
    (3) Enable 19.2 MHz Clock (6) Return to Default
             
                         (Q)uit
    ---------------------------------------------------------------
EOF
    read -n1 -s
    case "$REPLY" in
    "1")  sudo sed -e 's/\/\/ #define EXTERNAL_OSC 12/ #define EXTERNAL_OSC 12/g' $conf > $conf.tmp && mv -f $conf.tmp $conf && echo "12.0 MHZ CLock Enabled";;
    "2")  sudo sed -e 's/\/\/ #define EXTERNAL_OSC 14/ #define EXTERNAL_OSC 14/g' $conf > $conf.tmp && mv -f $conf.tmp $conf && echo "14.4 MHZ CLock Enabled";;
    "3")  sudo sed -e 's/\/\/ #define EXTERNAL_OSC 19/ #define EXTERNAL_OSC 19/g' $conf > $conf.tmp && mv -f $conf.tmp $conf && echo "19.2 MHZ CLock Enabled";; 
    "4")  sudo sed -e 's/\/\/ #define USE/             #define USE/g'             $conf > $conf.tmp && mv -f $conf.tmp $conf && echo "COS As Lockout Enabled";;
    "5")  sudo sed -e 's/\/\/ #define ARDUINO_MODE/    #define ARDUINO_MODE/g'    $conf > $conf.tmp && mv -f $conf.tmp $conf && echo "Mode Pins Enabled";;
    "6")  MV -f $confbak $conf ;;
    "Q")  echo "If any changes are made you need to upload to MMDVM" && exit;;
    "q")  echo "If any changes are made you need to upload to MMDVM" && exit;; 
     * )  echo "invalid option"     ;;
    esac
    sleep 1
done
