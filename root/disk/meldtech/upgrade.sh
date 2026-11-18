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

#       upgrade.sh - $Revision: 85 $ $Date: 2012-10-05 17:20:49 -0400 (Fri, 05 Oct 2012) $ $Author: mario $
#       This script upgrades the meldtech files for WhiteSpaces         #

# THIS UPGRADE DOES THE FOLLOWING:
# 1- UPDATES TO sbi FILE:
#    a) ContactCountry TO US
#    b) DATE/VERSION TO 1.21 October-25-2012
#    c) HostAddress TO POINT TO LIVE IF DEVICE OPERATES AS FIXED
#    d) ADD StreamingIpAddress
# 2- REPLACES whitespaceagent WITH VERSION 27
# 3- REPLACES ALL /disk/www/ FILES
# 4- REPLACE setipeth0 TO KILL PREVIOUS udhcpc PROCESSES
# 5- REPLACE THE template_file TO INCLUDE StreamingIpAddress
# 6- DISABLE LOGGING FOR thttpd SERVER

# DEFINE PATHS
source="/disk/www/resources/meldtech"
disk="/disk/"
bin_destination=${disk}"usr/bin/"
www_destination=${disk}"www"
config="/etc/config/"
sbi=${config}"sbi"
init="/etc/init.d/"
setip=${init}"setipeth0"

# CHECK VERSION
ver=$(grep Version /etc/config/sbi | awk '{print $3}')
if [ "$ver" = "'1.21'" ]; then
  exit 1
else
  # CLEANUP upgrade.log
  echo "" > /disk/www/upgrade.log

  # 1.
  # a) ContactCountry
  sed -i "s/option 'ContactCountry'.*/option 'ContactCountry' 'US'/" ${sbi}

  # b) Version & Date
  sed -i "s/option 'Version'.*/option 'Version' '1.21'/" ${sbi}
  sed -i "s/option 'Date'.*/option 'Date' 'October-25-2012'/" ${sbi}

  # c) HostAddress
  devType=$(grep ChLookupDeviceType /etc/config/sbi | awk '{print $3}')
  if [ "$devType" = "'8'" ]; then
    sed -i "s/option 'HostAddress'.*/option 'HostAddress' 'tvws.spectrumbridge.com'/" ${sbi}
  fi

  # d) StreamingIpAddress
  # DO THIS ONLY FOR VERSIONS THAT DON'T HAVE THE ENTRY
  if grep StreamingIpAddress /etc/config/sbi
  then
    echo -e "StreamingIpAddress already set!"
  else
    stream_option="\    option 'StreamingIpAddress' ''"
    sed "22 i ${stream_option}" ${sbi} > /etc/config/tmp_sbi && mv /etc/config/tmp_sbi ${sbi}
  fi

  #e) ChLookupDeviceType
  #echo -e "sed -i "s/option 'ChLookupDeviceType'.*/option 'ChLookupDeviceType' '8'/" ${sbi}"
  #sed -i "s/option 'ChLookupDeviceType'.*/option 'ChLookupDeviceType' '8'/" ${sbi}

  # 2. Whitespace Agent
  echo -e "cp -f ${source}${bin_destination}whitespaceagent ${bin_destination}"
  cp -f ${source}${bin_destination}whitespaceagent ${bin_destination}

  # 3. /disk/www
  echo -e "cp -rf ${source}${www_destination} ${disk}"
  cp -rf ${source}${www_destination} ${disk}

  # 4 setipeth0
  # DO THIS ONLY FOR VERSIONS THAT DON'T HAVE THE ENTRY
  if grep killall ${setip}
  then
    echo -e "killall already set"
  else
    killall_line="\  killall udhcpc"
    tmp_setip=${init}"tmp_setipeth0"
    echo -e "sed "11 i ${killall_line}" ${setip} > ${tmp_setip} && mv ${tmp_setip} ${setip}"
    sed "11 i ${killall_line}" ${setip} > ${tmp_setip} && mv ${tmp_setip} ${setip}
    chmod +x ${setip}
  fi

  # 5 template_file
  echo -e "cp -f ${source}${config}template_file ${config}"
  cp -f ${source}${config}template_file ${config}

  # 6 thttpd
  thttpd_line="thttpd -l /dev/null -C /disk/www/thttpd.conf"
  echo -e "sed -i "s@thttpd.*@${thttpd_line}@" ${init}rcS"
  sed -i "s@thttpd.*@${thttpd_line}@" ${init}rcS
  
  # RESTART THE WSAGENT
  wsagent_stop
  wsagent_start

fi
