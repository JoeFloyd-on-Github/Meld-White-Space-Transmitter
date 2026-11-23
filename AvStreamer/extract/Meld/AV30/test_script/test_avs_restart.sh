#! /bin/sh

echo "/disk/Meld/AV30/test_script/test_avs_restart.sh"
echo
echo "  This test assumes the system was running normally before starting."
echo
echo "  test_avs_restart : `date`"
echo

cp -p avs.config avs.config_save

Restarts=0
RestartsMax=100
iterationDelay=35

echo "  test_avs_restart : There will be $RestartsMax av_restarts"
echo

cp -p avs.config avs.tmp

while [ "$Restarts" -le $RestartsMax ]
do
  for codec in 0 1; do
    for output in 0 1 2; do
      for scaler in 0 1; do
        for rmxrate in 190 140 60; do
          if   [ $rmxrate == "190" ]; then bitrate=170
          elif [ $rmxrate == "140" ]; then bitrate=120
          else bitrate=40
          fi
          echo "  test_avs_restart : `date`"
          echo "  test_avs_restart : Restarts = $Restarts"
          echo "  test_avs_restart : Restarts = $Restarts" | logger
          echo "  test_avs_restart : Codec = $codec, Output = $output, Scaler = $scaler, Bitrate = $bitrate, Rmxrate = $rmxrate"
          echo "  test_avs_restart : Codec = $codec, Output = $output, Scaler = $scaler, Bitrate = $bitrate, Rmxrate = $rmxrate" | logger
          echo
          sed -i s/.*CODEC.*/CODEC=$codec/ avs.tmp
          sed -i s/.*OUTPUT.*/OUTPUT=$output/ avs.tmp
          sed -i s/.*SCALER.*/SCALER=$scaler/ avs.tmp
          sed -i s/.*BITRATE.*/BITRATE=$bitrate/ avs.tmp
          sed -i s/.*RMXRATE.*/RMXRATE=$rmxrate/ avs.tmp
	  cp avs.tmp avs.config
#          ./avs_if_restart.sh
	  ./avs_stop.sh
	  sleep 5
	  ./avs_start.sh
          let "Restarts+=1"
          sleep $iterationDelay
          echo "  test_avs_restart : Restarts = $Resarts, done"
          echo "  test_avs_restart : Restarts = $Resarts, done" | logger
          echo
        done
      done
    done
  done
done

cp -p avs.config_save avs.config

