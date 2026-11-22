#!/usr/bin/env python3
"""Run shell commands on a Meld device over its passwordless BusyBox telnetd.

The Meld firmware starts `telnetd -l /bin/sh` with no authentication, so TCP 23
drops straight to a root shell. This helper opens that shell, runs each command
argument in turn (one shell session for all of them), and prints the output of
each under a `### <cmd>` header. It handles telnet IAC negotiation, disables
echo, and uses a sentinel to know when each command has finished.

Usage:
    meld_telnet.py [-H HOST] [-t TIMEOUT] 'cmd1' 'cmd2' ...

Example:
    meld_telnet.py -H 192.168.1.194 'cat /proc/mtd' 'uname -a'

This is a convenience for scripting (see backup-meld.sh). For a hand-run backup
you can just use the interactive `telnet HOST` client instead.
"""
import socket
import sys
import time
import argparse

IAC, DONT, DO, WONT, WILL, SB, SE = 255, 254, 253, 252, 251, 250, 240
MARK = "__MELD_DONE_7f3a__"


def strip_iac(buf, sock):
    """Remove telnet IAC command sequences, answering negotiations minimally."""
    out, i = bytearray(), 0
    while i < len(buf):
        b = buf[i]
        if b == IAC and i + 1 < len(buf):
            cmd = buf[i + 1]
            if cmd in (DO, DONT, WILL, WONT) and i + 2 < len(buf):
                opt = buf[i + 2]
                if cmd == DO:
                    sock.sendall(bytes([IAC, WONT, opt]))
                elif cmd == WILL:
                    # accept echo (1) and suppress-go-ahead (3), refuse the rest
                    sock.sendall(bytes([IAC, DO if opt in (1, 3) else DONT, opt]))
                i += 3
                continue
            if cmd == SB:
                j = buf.find(bytes([IAC, SE]), i)
                i = len(buf) if j < 0 else j + 2
                continue
            i += 2
            continue
        out.append(b)
        i += 1
    return bytes(out)


def read_until(sock, token, timeout):
    sock.settimeout(0.5)
    data = b""
    end = time.time() + timeout
    while time.time() < end:
        try:
            chunk = sock.recv(65536)
            if not chunk:
                break
            data += strip_iac(chunk, sock)
            if token.encode() in data:
                break
        except socket.timeout:
            pass
    return data.decode("latin-1")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("-H", "--host", default="192.168.1.194")
    ap.add_argument("-t", "--timeout", type=float, default=30,
                    help="per-command timeout in seconds (default 30)")
    ap.add_argument("cmds", nargs="+")
    a = ap.parse_args()

    s = socket.create_connection((a.host, 23), timeout=5)
    read_until(s, "#", 5)
    s.sendall(b"stty -echo 2>/dev/null; PS1=''\r\n")
    read_until(s, "\n", 2)
    time.sleep(0.3)
    read_until(s, "zzz", 0.5)  # drain

    for c in a.cmds:
        s.sendall((c + f"; echo {MARK}\r\n").encode())
        out = read_until(s, MARK + "\r\n", a.timeout)
        out = out.replace(f"; echo {MARK}", "")
        lines = [l for l in out.replace("\r", "").split("\n")
                 if MARK not in l and l.strip() != c.strip()]
        print(f"### {c}\n" + "\n".join(lines).strip())

    s.sendall(b"exit\r\n")
    s.close()


if __name__ == "__main__":
    main()
