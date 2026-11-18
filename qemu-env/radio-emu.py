#!/usr/bin/env python3
"""
radio-emu.py - a stand-in for the MT300's NXP white-space transmitter radio.

wsa's serial thread (serialProcessorThread, /dev/ttyS1) runs an init sequence and
will only signal its database thread to start once it has learned the FCC ID and
serial number from the radio (see docs/wsa-overview.md section 6.2). On a generic
qemu malta board there is no radio, so wsa blocks forever. This script speaks just
enough of the radio's serial protocol to unblock it.

qemu exposes guest ttyS1 as a TCP server (see run-qemu.sh); we connect to it and
answer wsa's queries. Protocol per docs/wsa-overview.md section 6.3:
  x    -> "S/N = <serial>"
  y    -> "FCC ID = <fccid>"
  l\\r  -> "Firmware version = ..."
  h    -> info dump terminated by a line starting with EOF
  b?   -> streaming IP, 8 hex digits starting at byte offset 3
  others (MELD_TECH_WS_ENABLE, e0/e1, c<ch>, p.., m#[..]) -> just acked/ignored
"""
import socket, sys, time

HOST, PORT = "127.0.0.1", 5556
SERIAL = "371"
FCCID  = "OKVMT300"
FWVER  = "1.3.0 v0036 Tx v2"
STREAM_IP_HEX = "C0A800CB"      # 192.168.0.203

def connect():
    while True:
        try:
            s = socket.create_connection((HOST, PORT), timeout=2)
            s.settimeout(0.05)
            print("[radio] connected to guest ttyS1", flush=True)
            return s
        except OSError:
            time.sleep(0.3)

def main():
    s = connect()
    buf = b""
    def send(msg):
        s.sendall(msg.encode() if isinstance(msg, str) else msg)
        print("[radio] -> %r" % msg, flush=True)

    while True:
        try:
            data = s.recv(256)
            if data == b"":
                print("[radio] guest closed; reconnecting", flush=True)
                s.close(); s = connect(); buf = b""; continue
            buf += data
        except socket.timeout:
            data = b""
        except OSError:
            s.close(); s = connect(); buf = b""; continue

        # Process whole \r-terminated commands first.
        while b"\r" in buf:
            line, buf = buf.split(b"\r", 1)
            cmd = line.decode("latin1").strip()
            if cmd == "" :                       continue
            elif cmd == "MELD_TECH_WS_ENABLE":   print("[radio] <- ENABLE", flush=True)
            elif cmd == "l":                     send("Firmware version = %s\r\n" % FWVER)
            elif cmd.startswith("e"):            print("[radio] <- TX %s" % cmd, flush=True)
            elif cmd.startswith("c"):            print("[radio] <- set channel %s" % cmd, flush=True)
            elif cmd.startswith("p"):            print("[radio] <- power %s" % cmd, flush=True)
            else:                                print("[radio] <- cmd %r" % cmd, flush=True)

        # Single-byte / unterminated commands (h, x, y, b?).
        while buf:
            c = buf[:1]
            if c == b"x":
                send("S/N = %s\r\n" % SERIAL); buf = buf[1:]
            elif c == b"y":
                send("FCC ID = %s\r\n" % FCCID); buf = buf[1:]
            elif c == b"h":
                send("MELD WS radio\r\nStatus OK\r\nEOF\r\n"); buf = buf[1:]
            elif buf[:2] == b"b?":
                send("b#[01%s]\r\n" % STREAM_IP_HEX); buf = buf[2:]
            elif buf[:2] == b"b#":
                # setter with trailing \r already consumed above if present; drop byte
                buf = buf[1:]
            elif buf[:2] == b"m#":
                print("[radio] <- m# (freq/pwr/mod)", flush=True); buf = buf[1:]
            else:
                # unknown stray byte; drop it so we don't spin
                buf = buf[1:]

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
