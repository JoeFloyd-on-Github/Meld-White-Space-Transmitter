#! /bin/sh

echo "/disk/Meld/AV30/test_script/avs_stop.sh"
echo

pi=`cat /tmp/av_streamer.pid`
pkill -TERM av_streamer
rm /tmp/av_streamer.pid

i=0
echo "... : stopping av_streamer"
while ( [ -d /proc/$pi ] ); do
    if [ $((++i)) -eq 30 ]; then break; fi
    sleep 1
    echo "."
done

if [ $i -eq 30 ] ; then 
    echo "... : error av_streamer not stopped"
else
    echo "... : av_streamer stopped"
fi
