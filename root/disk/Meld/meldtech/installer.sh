#!/bin/sh
################################################################################
#
#       (c) 2012 Copyright Spectrum Bridge, Inc.
#       All rights reserved.
#
#       This software contains confidential and proprietary information.
#       The content or any derivative work may be used only as authorized
#       by a licensing agreement from Spectrum Bridge, Inc.  All copies must
#       include this header.
#
################################################################################

#       installer.sh - $Revision: 94 $ $Date: 2012-10-25 14:10:23 -0400 (Thu, 25 Oct 2012) $ $Author: mario $
#       This script installs the meldtech files for WhiteSpaces		#
#	TODO: Remove /disk/lib, disable thttpd logging, add upgrade process

# Define all the files in the package
clear_config_files="bin/clear_config_files"
thttpd="bin/thttpd"
whitespaceagent="bin/whitespaceagent"
wsagent_start="bin/wsagent_start"
wsagent_stop="bin/wsagent_stop"
xml_conversion="bin/xml_conversion"

CAFILE="etc/config/cafile.cert"
CHANNEL_STATUS="etc/config/channel_status"
CHANNELS="etc/config/channels"
FCCID_SERIAL_INFO="etc/config/fccidSerialInfo"
MELDTECH_DEVICE_CAPABILITY="etc/config/meldTechDeviceCapability.xml"
OPERATING="etc/config/operating"
REGISTRATION="etc/config/registration"
SBI="etc/config/sbi"
TEMPLATE_FILE="etc/config/template_file"
TMP_FILE="etc/config/tmp_file"
TRANSMITTER="etc/config/transmitter"

LIBSSL="disk/lib/libssl.so.0.9.8"

RCS="etc/init.d/rcS"
WHITESPACEAGENT="wsagent_start"
THTTPD="thttpd"
CLEANUP="clear_config_files"
setipeth0="setipeth0"

# Define all required directories
BIN="disk/usr/bin"
ETC="etc"
CONFIG="etc/config"
DISK="disk"
WWW="disk/www"
INIT="etc/init.d/"
# LIB="disk/lib"	THIS IS NO LONGER NEEDED BECAUSE whitespaceagent IS STATIC

directory_names="$CONFIG $WWW"

# Define destination root file path
FILEPATH="/"

# Put all the file names in a array
# file_names=($clear_config_files $thttpd $whitespaceagent $wsagent_start $wsagent_stop $xml_conversion $CAFILE $CHANNEL_STATUS $CHANNELS $FCCID_SERIAL_INFO $MELDTECH_DEVICE_CAPABILITY $OPERATING $REGISTRATION $SBI $TEMPLATE $TMP_FILE $TRANSMITTER)

# Create the necessary directories if needed
create_directories() {
    echo "Install directory check..."
    IFS=" "
    set -- $directory_names
    for d in $@
    do
	if [ ! -d $FILEPATH$d ]
	then
	    echo -e "  The directory $d is missing. Creating it..."
	    mkdir "$FILEPATH$d"
	else
	    echo -e "Directory $d exists"
	fi
    done
    echo -e "Directory check done!\n" 
}

# Copy the files blindly (without checking them)
bcopy_files() {
    echo "Installing files..."
    #echo "cp -R ${BIN} ${FILEPATH}"
    #cp -R "${BIN}" "${FILEPATH}"
    echo "cp -R ${DISK} ${FILEPATH}"
    cp -R "${DISK}" "${FILEPATH}"
    echo "cp -R ${CONFIG} ${FILEPATH}${ETC}"
    cp -R "${CONFIG}" "${FILEPATH}${ETC}"
    echo "cp ${INIT}${setipeth0} ${FILEPATH}${INIT}"
    cp ${INIT}${setipeth0} ${FILEPATH}${INIT}
    echo -e "Installing files done!\n"
}

# Check for script and add it if not existing
check_script() {
    if ! grep -q $1 "${FILEPATH}${RCS}"
    then
	echo "  The command ${1} not found in rcS.  Adding ${1} to rcS"
	if [ $1 = "$WHITESPACEAGENT" ]
	then
	    echo -e "\nwsagent_start" >> "${FILEPATH}${RCS}"
	elif [ $1 = "$THTTPD" ]
	then
	    #echo -e "\nthttpd -C /disk/www/thttpd.conf" >> "${FILEPATH}${RCS}"
	    echo -e "\nthttpd -l /dev/null -C /disk/www/thttpd.conf" >> "${FILEPATH}${RCS}"

	elif [ $1 = "$CLEANUP" ]
	then
	    echo -e "\nclear_config_files" >> "${FILEPATH}${RCS}"

	fi
    else
	echo "${1} already in place!"
    fi
}

# Add whitespaceagent, web server, and file cleanup scripts to rcS
add_scripts() {

    echo "Checking Cleanup Script..."
    check_script $CLEANUP

    echo "Checking WhiteSpaceAgent..."
    check_script $WHITESPACEAGENT

    echo "Checking Web Server..."
    check_script $THTTPD

    echo "Done updating rcS file"
}

#########################################################################################
#
#	Running the script starts here
#
#########################################################################################

# COPY THE FILES
create_directories && bcopy_files && add_scripts
