#!/bin/sh
# install.sh - make the MT300 start the database stand-in at every boot.
# Run ON THE DEVICE (telnet root shell) after copying sb_mips and sb_start to
# /disk (see docs/on-device-spoof.md section 3). Safe to run again.
#
#   1. /usr/bin/sb_mips, /usr/bin/sb_start  (rootfs, next to /etc/hosts + rcS)
#   2. /etc/hosts: tvws.spectrumbridge.com -> 127.0.0.1 (backup /etc/hosts.orig)
#   3. /etc/init.d/rcS: start sb_start just before wsagent_start
#      (backup /etc/init.d/rcS.orig)
set -e
SRC=${1:-/disk}

for f in sb_mips sb_start; do
  [ -f $SRC/$f ] || { echo "missing $SRC/$f"; exit 1; }
  cp $SRC/$f /usr/bin/$f
  chmod 755 /usr/bin/$f
done
md5sum /usr/bin/sb_mips

[ -f /etc/hosts.orig ] || cp -p /etc/hosts /etc/hosts.orig
if grep -q 'tvws\.spectrumbridge\.com' /etc/hosts; then
  sed -i 's/^[0-9. \t]*tvws\.spectrumbridge\.com.*/127.0.0.1   tvws.spectrumbridge.com/' /etc/hosts
else
  echo '127.0.0.1   tvws.spectrumbridge.com' >> /etc/hosts
fi
grep tvws /etc/hosts

if grep -q '/usr/bin/sb_start' /etc/init.d/rcS; then
  echo "rcS already starts sb_start"
else
  [ -f /etc/init.d/rcS.orig ] || cp -p /etc/init.d/rcS /etc/init.d/rcS.orig
  awk '/^wsagent_start$/ {
         print "# SpectrumBridge stand-in (local database for whitespaceagent); see /usr/bin/sb_start"
         print "/usr/bin/sb_start &"
         print "sleep 2" }
       { print }' /etc/init.d/rcS > /etc/init.d/rcS.new
  grep -q '/usr/bin/sb_start' /etc/init.d/rcS.new || { echo "no wsagent_start line in rcS"; rm /etc/init.d/rcS.new; exit 1; }
  sh -n /etc/init.d/rcS.new
  chmod 755 /etc/init.d/rcS.new
  mv /etc/init.d/rcS.new /etc/init.d/rcS
  echo "rcS updated"
fi
sync
echo "done - reboot, or: killall sb_mips; /usr/bin/sb_start & ; wsagent_stop; wsagent_start"
