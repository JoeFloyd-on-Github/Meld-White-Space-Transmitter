#!/bin/sh
# install_av.sh - install the SpectrumBridge database stand-in on an MT300-AV
# (video) unit running the v45 whitespaceagent. Run ON THE DEVICE (telnet root
# shell) after copying sb_mips and sb_start to /disk. Idempotent.
#
# Why a separate script from install.sh:
#   1. On AV firmware rcS does NOT launch the agent - /etc/init.d/setipeth0 calls
#      wsagent_start. So the sb_start boot hook goes in setipeth0, not rcS.
#   2. The v45 agent's get_ip() connects to the PTR *name* of the resolved
#      address, not the dotted quad. A plain "127.0.0.1 tvws.spectrumbridge.com"
#      hosts line makes it reverse-resolve 127.0.0.1 -> "localhost" and die with
#      "localhost is not a valid IP address". The cure is to make the FIRST hosts
#      name for 127.0.0.1 a literal dotted quad ("127.0.0.1 127.0.0.1 localhost")
#      so the reverse lookup yields a valid IP string.
#   3. wsagent_start only launches whitespaceagent when /etc/config/WSAgentRestart
#      exists (otherwise it runs "MeldAgent -O", a radio read that never contacts
#      the database). A pristine, never-web-registered unit lacks that file, so we
#      create it - this is what the web UI's "register" does.
#
# See docs/on-device-spoof.md and docs/units/mt300-av-677.md.
set -e
SRC=${1:-/disk}

for f in sb_mips sb_start; do
  [ -f $SRC/$f ] || { echo "missing $SRC/$f"; exit 1; }
  cp $SRC/$f /usr/bin/$f
  chmod 755 /usr/bin/$f
done
md5sum /usr/bin/sb_mips /usr/bin/sb_start

# /etc/hosts: reverse-name trick for the v45 resolver (see header note 2).
[ -f /etc/hosts.orig ] || cp -p /etc/hosts /etc/hosts.orig
printf '127.0.0.1\t127.0.0.1 localhost\n127.0.0.1\ttvws.spectrumbridge.com\n' > /etc/hosts
echo "hosts now:"; cat /etc/hosts

# boot hook: start sb_start just before wsagent_start in setipeth0 (not rcS).
HOOK=/etc/init.d/setipeth0
if grep -q '/usr/bin/sb_start' $HOOK; then
  echo "setipeth0 already hooks sb_start"
else
  [ -f /etc/init.d/setipeth0.presb ] || cp -p $HOOK /etc/init.d/setipeth0.presb
  awk '/^wsagent_start$/ {
         print "# SpectrumBridge stand-in (local database for whitespaceagent); see /usr/bin/sb_start"
         print "/usr/bin/sb_start &"
         print "sleep 2" }
       { print }' $HOOK > $HOOK.new
  grep -q '/usr/bin/sb_start' $HOOK.new || { echo "no wsagent_start line in setipeth0"; rm $HOOK.new; exit 1; }
  sh -n $HOOK.new
  chmod 755 $HOOK.new
  mv $HOOK.new $HOOK
  echo "setipeth0 hooked"
fi

# registration trigger: make wsagent_start launch whitespaceagent (see header 3).
[ -f /etc/config/WSAgentRestart ] || touch /etc/config/WSAgentRestart
echo "WSAgentRestart present: $(ls -l /etc/config/WSAgentRestart)"

sync
echo "done - reboot, or: killall sb_mips sb_start whitespaceagent; /usr/bin/sb_start & ; wsagent_stop; wsagent_start"
