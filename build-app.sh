#!/bin/bash

set -e

echo "Building BareMetal lwIP web server..."

root=$(pwd)
LWIP=$root/lwip-2.2.1/src
PORT=$root/port

# Check for existing lwIP directory
if [ ! -d "$root/lwip-2.2.1/" ]; then
    # Extract lwIP source zip
    unzip lwip-2.2.1.zip
fi

CC=gcc
LD=ld
OBJCOPY=objcopy

# ---- Build makefsdata and generate fsdata.c from fs/ ----
MAKEFSDATA_SRC=$LWIP/apps/http/makefsdata/makefsdata.c
MAKEFSDATA_BIN=$root/makefsdata

echo "Building makefsdata..."
gcc \
    -I "$LWIP/include" \
    -I "$root/lwip-2.2.1/contrib/ports/unix/posixlib" \
    -I "$root/lwip-2.2.1/contrib/ports/unix/port/include" \
    -o "$MAKEFSDATA_BIN" "$MAKEFSDATA_SRC"

echo "Generating fsdata.c from fs/..."
(cd "$root" && "$MAKEFSDATA_BIN" fs -f:"$root/fsdata.c")
# -------------------------------------------------------------------------

CFLAGS="${CFLAGS_FOR_TARGET} -fno-stack-protector"
CFLAGS="$CFLAGS -D_POSIX_TIMERS=1 -D_POSIX_MONOTONIC_CLOCK=1"
CFLAGS="$CFLAGS -I $root"
CFLAGS="$CFLAGS -I $root/include"
CFLAGS="$CFLAGS -DHTTPD_FSDATA_FILE=\"$root/fsdata.c\""
CFLAGS="$CFLAGS -I $LWIP/include"
CFLAGS="$CFLAGS -I $PORT"

LDFLAGS="-T $root/app.ld -z max-page-size=0x1000 -L $root/lib"

# ---- lwIP core ----
LWIP_CORE_SRCS="
  $LWIP/core/init.c
  $LWIP/core/def.c
  $LWIP/core/dns.c
  $LWIP/core/inet_chksum.c
  $LWIP/core/ip.c
  $LWIP/core/mem.c
  $LWIP/core/memp.c
  $LWIP/core/netif.c
  $LWIP/core/pbuf.c
  $LWIP/core/raw.c
  $LWIP/core/stats.c
  $LWIP/core/sys.c
  $LWIP/core/altcp.c
  $LWIP/core/altcp_alloc.c
  $LWIP/core/altcp_tcp.c
  $LWIP/core/tcp.c
  $LWIP/core/tcp_in.c
  $LWIP/core/tcp_out.c
  $LWIP/core/timeouts.c
  $LWIP/core/udp.c
"

# ---- lwIP IPv4 ----
LWIP_IPV4_SRCS="
  $LWIP/core/ipv4/acd.c
  $LWIP/core/ipv4/autoip.c
  $LWIP/core/ipv4/dhcp.c
  $LWIP/core/ipv4/etharp.c
  $LWIP/core/ipv4/icmp.c
  $LWIP/core/ipv4/igmp.c
  $LWIP/core/ipv4/ip4.c
  $LWIP/core/ipv4/ip4_addr.c
  $LWIP/core/ipv4/ip4_frag.c
"

# ---- lwIP netif ----
LWIP_NETIF_SRCS="
  $LWIP/netif/ethernet.c
"

# ---- lwIP httpd app ----
LWIP_HTTP_SRCS="
  $LWIP/apps/http/httpd.c
  $LWIP/apps/http/fs.c
"
# Note: fsdata.c is #include'd by fs.c (via HTTPD_FSDATA_FILE macro); do not compile it separately.

# ---- BareMetal port ----
PORT_SRCS="
  $PORT/sys_arch.c
  $PORT/netif/baremetalif.c
"

ALL_SRCS="
  $LWIP_CORE_SRCS
  $LWIP_IPV4_SRCS
  $LWIP_NETIF_SRCS
  $LWIP_HTTP_SRCS
  $PORT_SRCS
  $root/libBareMetal.c
  $root/test.c
"

OBJ_FILES=""
for src in $ALL_SRCS; do
    # Derive a unique object name from the source path
    obj=$(basename "${src%.c}").o
    # Handle name collisions by prepending a hash of the dir
    dir=$(dirname "$src")
    prefix=$(echo "$dir" | tr '/' '_' | tr -d '.' | sed 's/^_//;s/_home_.*_lwip_2_2_1_src_/lwip_/')
    obj="${prefix}_${obj}"

    echo "  CC $src"
    $CC $CFLAGS -c "$src" -o "$obj"
    OBJ_FILES="$OBJ_FILES $obj"
done

echo "Linking test..."
$LD $LDFLAGS -o test $root/lib/crt0.o $OBJ_FILES -lc

echo "Generating test.app..."
$OBJCOPY -O binary test test.app

echo "Cleaning up..."
rm -f *.o "$MAKEFSDATA_BIN" "$root/fsdata.c"

echo ""
echo "Build complete: test.app"
