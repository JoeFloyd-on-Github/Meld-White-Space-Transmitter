#!/bin/sh
#
#Meld Technology, Inc
# 20140114
# PJA
#
# SW Release : MT300 : sb1p38 : r2 : wsagent45_patch
#
##############################################################

echo
echo "==== AppScript_MT300_sb1p38_r2_wsagent_45_patch_tftp.sh ===="
echo
echo "  Meld Technology, Inc"
echo "  SW Release : MT300 : sb1p38 : wsagent45_patch"
echo "  20140109"
echo
#echo "  BSDK_XCodePro100_BETA_8B_20121113"
#echo "  Mips 2.6.36"
#echo "  U-Boot with GPIO 6 & 7 set to ON"
#echo "  Linux_UBoot_genlock_patch"
#echo "  SP5172-2013-04-04"
#echo "  NandScript_AV29_8B_nfs.sh, with:"
#echo "    8B_boot10.tar"
#echo "    8B_disk15_lib[1-4].tar"
#echo "    8B_disk15_nolibs.tar"
#echo "    8B_disk15_MeldApps.tar"
echo "  SpectrumBridge meldtech_installer_1.38"
echo "  Meld AV29_wsagent_45_patch"
#echo "  Meld /etc/init.d/setipeth0 and /etc/init.d/rcS"
#echo "  Meld Meld_AV_Script_2p08_20131211.tar.gz"
#echo "  Meld Meld_Edid_r12_720p_pcm48.txt"
#echo "  Meld upnp_installer_1p01.tar.gz"
echo
sleep 1

##############################################################

TFTP_IP=192.168.1.123

if [ $# -eq 1 ]; then
  TFTP_IP=$1
elif [ $# -gt 1 ]; then
  echo "  Error: Too many parameters!"
  echo "  $0 $*"
  exit 1
fi

echo "  $0 $*"
echo "  TFTP_IP=$TFTP_IP"
echo

REM_DIR=AV29_files

SRC_DIR=/disk/Meld/AV29_files

#AV_DIR=/disk/Meld/AV29

#UDEV_UPDATES=8B_disk15_updates.tar

WHITESPACE_APP=meldtech_installer_1.38.tar.gz

WSAGENT_45_PATCH=AV29_wsagent_45_patch.tar.gz

#MELD_ETC_INIT=Meld_Etc_Init.tar.gz

#STREAMING_APP=Meld_AV_Script_2p08_20131211.tar.gz

#UPNP_DRVRS=upnp_installer_1p01.tar.gz


echo "  REM_DIR          = $REM_DIR"
echo "  SRC_DIR          = $SRC_DIR"
#echo "  AV_DIR         = $AV_DIR"
#echo "  UDEV_UPDATES   = $UDEV_UPDATES"
echo "  WHITESPACE_APP   = $WHITESPACE_APP"
echo "  WSAGENT_45_PATCH = $WSAGENT_45_PATCH"
#echo "  MELD_ETC_INIT  = $MELD_ETC_INIT"
#echo "  STREAMING_APP  = $STREAMING_APP"
#echo "  UPNP_DRVRS     = $UPNP_DRVRS"
echo

echo "Clear any previous installations"
cd /disk/Meld
rm -rf meldtech Meld_Etc_Init AV?? AV??? AV*_files *.tar *.gz *.bz upnp*
sync
echo

if ! [[ -d $SRC_DIR ]] ; then
  echo "Create the source directory $SRC_DIR"
  echo
  mkdir $SRC_DIR
fi

echo "Stopping any \"av_streamer\", \"whitespaceagent\" or \"upnp\" processes"
echo

killall av_streamer
wsagent_stop
killall lld2d
killall upnp_device
sleep 1

echo "Use TFTP to get the Application and Update files"
echo

#echo "  get $UDEV_UPDATES"
#tftp -g -r $REM_DIR/$UDEV_UPDATES -l $SRC_DIR/$UDEV_UPDATES $TFTP_IP
#sync
#echo

echo "  get $WHITESPACE_APP"
tftp -g -r $REM_DIR/$WHITESPACE_APP -l $SRC_DIR/$WHITESPACE_APP $TFTP_IP
sync
echo

echo "  get $WSAGENT_45_PATCH"
tftp -g -r $REM_DIR/$WSAGENT_45_PATCH -l $SRC_DIR/$WSAGENT_45_PATCH $TFTP_IP
sync
echo


#echo "  get $MELD_ETC_INIT"
#tftp -g -r $REM_DIR/$MELD_ETC_INIT -l $SRC_DIR/$MELD_ETC_INIT $TFTP_IP
#sync
#echo

#echo "  get $STREAMING_APP"
#tftp -g -r $REM_DIR/$STREAMING_APP -l $SRC_DIR/$STREAMING_APP $TFTP_IP
#sync
#echo

#echo "  get $UPNP_DRVRS"
#tftp -g -r $REM_DIR/$UPNP_DRVRS -l $SRC_DIR/$UPNP_DRVRS $TFTP_IP
#sync
#echo

#echo "Update Udev"
#cd /disk
#tar xvf $SRC_DIR/$UDEV_UPDATES
#echo
#sync

echo "Untar: $WHITESPACE_APP"
cd /disk/Meld
tar zxvf $SRC_DIR/$WHITESPACE_APP
echo
#echo "Apply Meld specific customization"
#echo
#echo "Untar: $MELD_ETC_INIT"
#tar zxvf $SRC_DIR/$MELD_ETC_INIT
#cp -p -f /disk/Meld/Meld_Etc_Init/rcS /etc/init.d
#cp -p -f /disk/Meld/Meld_Etc_Init/setipeth0 /etc/init.d
#cp -p -f /disk/Meld/Meld_Etc_Init/resolv.conf /etc
#cp -p -f /disk/Meld/Meld_Etc_Init/rcS /disk/Meld/meldtech/etc/init.d
#cp -p -f /disk/Meld/Meld_Etc_Init/setipeth0 /disk/Meld/meldtech/etc/init.d
sync
echo

echo "Install: $WHITESPACE_APP"
cd /disk/Meld/meldtech
./installer.sh
sync
echo

echo "Install: $WSAGENT_45_PATCH"
tar xvf $SRC_DIR/$WSAGENT_45_PATCH -C /
sync
echo

#echo "Install: $STREAMING_APP"
#if [ ! -d $AV_DIR ]
#  then
#    echo -e "Creating directory: $AV_DIR"
#    mkdir "$AV_DIR"
#  fi
#cd $AV_DIR
#tar xvf $SRC_DIR/$STREAMING_APP
#cd $AV_DIR/av_script
#./meld_av_installer.sh av_script.sh
#sync
#echo

#echo "Untar: $UPNP_DRVRS"
#cd /disk/Meld
#tar xvf $SRC_DIR/$UPNP_DRVRS
#echo
#cd /disk/Meld/upnp
#./upnp_installer.sh
#sync
#echo

echo "====================== DONE ==========================="
echo

