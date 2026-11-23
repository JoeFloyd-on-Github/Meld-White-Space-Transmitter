#! /bin/sh

# pja until the linux crash problem is solved the "avs_if_restart.has" script
# cannot be used to restart the "av_streamer" process

# pja comment must go to AV30 rather than AV19
# cd /disk/Meld/AV19/test_script

echo "/disk/Meld/AV30/test_script/avs_if_restart.sh"
echo

MELD_PLATFORM=`fw_printenv -n MELD_PLATFORM`

if [[ $MELD_PLATFORM != "MT300-AV" ]] ; then
  echo "MELD_PLATFORM = $MELD_PLATFORM"
  echo "Cannot start \"av_streamer\" on this MELD PLATFORM"
  exit
fi

cd /disk/Meld/AV30/test_script

#if [ -r /tmp/av_streamer.pid ] ; then
#    echo "!!! restart av_streamer"
#    ./avs_stop.sh
#    ./avs_start.sh
#else 
#     echo "!!! av_streamer not started"
#		./avs_start.sh
#fi

if [ -r /tmp/av_streamer.pid ] ; then
    echo "!!! stopping av_streamer"
    echo "!!! stopping av_streamer" > /dev/console
    ./avs_stop.sh
fi

echo "!!! Linux Reboot"
echo "!!! Linux Reboot" > /dev/console
reboot


