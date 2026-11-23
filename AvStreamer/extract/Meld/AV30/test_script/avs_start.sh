#! /bin/sh

echo "/disk/Meld/AV30/test_script/avs_start.sh"
echo

MELD_PLATFORM=`fw_printenv -n MELD_PLATFORM`

WSAGENTRESTART="/etc/config/WSAgentRestart"
TXAUTOENABLE="/etc/config/TXAutoEnable"
QUERYINFO="/etc/config/queryInfo"

if [[ $MELD_PLATFORM != "MT300-AV" ]] ; then
  echo "MELD_PLATFORM = $MELD_PLATFORM"
  echo "Cannot start \"av_streamer\" on this MELD PLATFORM"
  exit
fi

echo "avs_start.sh : Wait for file - \"$QUERYINFO\" - to be created"
echo "avs_start.sh : Wait for file - \"$QUERYINFO\" - to be created" | logger   
waitsec=0
while [[ ! -f $QUERYINFO ]] ;
  do
    echo "avs_start.sh : Waiting for file - \"$QUERYINFO\" - to be created : $waitsec"
    echo "avs_start.sh : Waiting for file - \"$QUERYINFO\" - to be created : $waitsec" | logger
    waitsec=$((waitsec+2))
    sleep 2
  done

echo "avs_start.sh : Wait for file - \"$QUERYINFO\" - to contain data"
echo "avs_start.sh : Wait for file - \"$QUERYINFO\" - to contain data" | logger
waitsec=0
while [[ ! -s $QUERYINFO ]]  ;
  do
    echo "avs_start.sh : Waiting for file - \"$QUERYINFO\" - to contain data : $waitsec"
    echo "avs_start.sh : Waiting for file - \"$QUERYINFO\" - to contain data : $waitsec" | logger
    waitsec=$((waitsec+1))
    sleep 1
  done

sleep 1

cd /disk/Meld/AV30/test_script

#codec :
#	0 = H264
#	1 = MPEG2
#	default = MPEG2

#output :
#	0 = DVO
#	1 = UDP
#	2 = UDP2
#	default = DVO

#scaler :
#	0 = OFF
#	1 = ON
#	default = OFF

#rmxrate :
#	Min =  15 (means  1,500,000 Mbs)
#	Max = 190 (means 19,000,000 Mbs)

#bitrate :
#	Min =  10 (means  1,000,000 Mbs)
#	Max = 170 (means 17,000,000 Mbs)

#dstttl :
#	Min =   0
#	Max = 255

codec=
output=
scaler=
bitrate=
rmxrate=
ipaddr=
ipport=
dstttl=32

file=./avs.config
[ -r $file ] || file=./avs.config.def

verLines=`./av_streamer -v`
ver=`echo $verLines | awk '($1=="av_streamer")&&($2=="version") {printf("%s\n", $3)}'`
`sed '/VERSION/d' $file > avs.config.1`
`cp -p avs.config.1 avs.config`
`echo "VERSION=$ver" >> avs.config`

while read line 
do
    if [ "$(echo $line | grep -io "CODEC")" = "CODEC" ]; then
        codec=${line##*=}
        case "$codec" in
            "1")
                codec="MPEG2"
            ;;
            "0")
                codec="H264"
            ;;
            *)
                codec="MPEG2"
            ;;
        esac
    fi
    if [ "$(echo $line | grep -io "OUTPUT")" = "OUTPUT" ]; then
        output=${line##*=}
        case "$output" in
            "0")
                output="DVO"
            ;;
            "1")
                output="UDP"
            ;;
	    "2")
	    	output="UDP2"
	    ;;
            *)
                output="DVO"
            ;;
        esac
    fi
    if [ "$(echo $line | grep -io "SCALER")" = "SCALER" ]; then
        scaler=${line##*=}
        case "$scaler" in
            "0")
                scaler="SCALER_OFF"
            ;;
            "1")
                scaler="SCALER_ON"
            ;;
            *)
                scaler="SCALER_OFF"
            ;;
        esac
    fi
    if [ "$(echo $line | grep -io "BITRATE")" = "BITRATE" ]; then
        bitrate=${line##*=}
        bitrate=$bitrate"00000"
    fi
    if [ "$(echo $line | grep -io "RMXRATE")" = "RMXRATE" ]; then
        rmxrate=${line##*=}
        rmxrate=$rmxrate"00000"
    fi
    if [ "$(echo $line | grep -io "IPADDR")" = "IPADDR" ]; then
        ipaddr=${line##*=}
    fi
    if [ "$(echo $line | grep -io "IPPORT")" = "IPPORT" ]; then
        ipport=${line##*=}
    fi
    if [ "$(echo $line | grep -io "DSTTTL")" = "DSTTTL" ]; then
        dstttl=${line##*=}
    fi
done < $file


if [ "$bitrate" -gt "17000000" ]; then
    bitrate=17000000;
elif [ "$bitrate" -lt "1000000" ]; then
    bitrate=1000000;
fi

if [ "$rmxrate" -gt "19000000" ]; then
    rmxrate=19000000;
elif [ "$rmxrate" -lt "1500000" ]; then
    rmxrate=1500000;
fi

if [ "$dstttl" -gt "255" ]; then
    dstttl=255;
elif [ "$dstttl" -lt "0" ]; then
    dstttl=0;
fi

if [ $output == "UDP2" ]; then  ## added "-g -t 1000"
    output="UDP -n $ipaddr -P $ipport -T $dstttl -p 48 -g -t 1000"; ## this works for MT300-IT to RF
elif [ $output == "UDP" ]; then  ## as original
    output="UDP -n $ipaddr -P $ipport -T $dstttl";
elif [ $output == "DVO" ]; then
    output="DVO -p 48 -g -t 1000";
fi

if [ $scaler == "SCALER_OFF" ]; then
    scaler="";
elif [ $scaler == "SCALER_ON" ]; then
    scaler=" -o 480i";
fi

echo "av_streamer version $ver"
echo "codec $codec"
echo "output $output"
echo "scaler $scaler"
echo "bitrate $bitrate"
echo "rmxrate $rmxrate"
echo "ipaddr $ipaddr"
echo "ipport $ipport"
echo "dstttl $dstttl"

echo "Load AV Driver"
./rc.xcode4 Board_ID=0x2f00
echo "Start AVStreamer"
./TsConfig meld_ts.cfg
echo "Use command \"logread -f\" to see syslog"
echo "  (^C to stop logread)"
/sbin/start-stop-daemon -S -m -b -p /tmp/av_streamer.pid -x ./av_streamer -- -e Meld_Edid_r12_720p_pcm48.txt -c $codec -m $output $scaler -R $bitrate -r $rmxrate
#./av_streamer -e Meld_Edid_r12_720p_pcm48.txt -c $codec -m $output $scaler -R $bitrate -r $rmxrate

