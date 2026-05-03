/*
 * BareMetal lwIP iperf server demo.
 *
 * This is iperf2.. not iperf3
 *
 * Initialises lwIP in NO_SYS=1 (polling) mode, obtains an IP address via
 * DHCP, then starts an lwiperf TCP server on port 5001.
 *
 * NOTE: lwiperf speaks the iperf2 wire protocol. Use an iperf2 client:
 *   iperf -c <ip>              (default 10-second TCP throughput test)
 *   iperf -c <ip> -t 30 -i 1  (30 seconds, 1-second intervals)
 * Do NOT use iperf3 — it uses a different, incompatible protocol.
 *
 * Main loop:
 *   1. Poll the NIC for received frames.
 *   2. Call sys_check_timeouts() so DHCP retransmits, TCP keepalives, and
 *      the lwiperf idle-timeout all fire on schedule.
 */

#include <stdio.h>
#include <string.h>

#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "lwip/dhcp.h"
#include "lwip/ip_addr.h"
#include "lwip/apps/lwiperf.h"
#include "netif/ethernet.h"

#include "libBareMetal.h"
#include "netif/baremetalif.h"

static struct netif netif;
static int dhcp_bound = 0;

static void iperf_report(void *arg, enum lwiperf_report_type report_type,
    const ip_addr_t *local_addr, u16_t local_port,
    const ip_addr_t *remote_addr, u16_t remote_port,
    u32_t bytes_transferred, u32_t ms_duration, u32_t bandwidth_kbitpsec)
{
    (void)arg;
    const char *type_str;
    switch (report_type) {
        case LWIPERF_TCP_DONE_SERVER:       type_str = "server done";       break;
        case LWIPERF_TCP_DONE_CLIENT:       type_str = "client done";       break;
        case LWIPERF_TCP_ABORTED_LOCAL:     type_str = "aborted (local)";   break;
        case LWIPERF_TCP_ABORTED_LOCAL_DATAERROR: type_str = "aborted (data error)"; break;
        case LWIPERF_TCP_ABORTED_LOCAL_TXERROR:   type_str = "aborted (tx error)";   break;
        case LWIPERF_TCP_ABORTED_REMOTE:    type_str = "aborted (remote)";  break;
        default:                            type_str = "unknown";           break;
    }
    printf("iperf: %s  remote=%s:%u  local=%s:%u\n",
           type_str,
           ipaddr_ntoa(remote_addr), (unsigned)remote_port,
           ipaddr_ntoa(local_addr),  (unsigned)local_port);
    printf("  Transferred: %lu bytes  Duration: %lu ms\n",
           (unsigned long)bytes_transferred, (unsigned long)ms_duration);
    printf("  Bandwidth:   %lu kbits/s  (%.2f Mbits/s)\n",
           (unsigned long)bandwidth_kbitpsec,
           (double)bandwidth_kbitpsec / 1000.0);
}

static void check_dhcp_state(void) {
    if (dhcp_bound) return;
    if (!dhcp_supplied_address(&netif)) return;

    dhcp_bound = 1;
    printf("DHCP bound!\n");
    printf("  IP  : %s\n", ipaddr_ntoa(&netif.ip_addr));
    printf("  Mask: %s\n", ipaddr_ntoa(&netif.netmask));
    printf("  GW  : %s\n", ipaddr_ntoa(&netif.gw));
    printf("iperf server listening on port %u\n", LWIPERF_TCP_PORT_DEFAULT);
    printf("Connect with: iperf -c %s\n", ipaddr_ntoa(&netif.ip_addr));
}

int main(void) {
    printf("BareMetal lwIP iperf Server\n");
    printf("============================\n");

    printf("Waiting for network...\n");
    while (b_system(NET_STATUS, 0, 0) == 0) {
        /* spin */
    }
    printf("Network interface ready.\n");

    lwip_init();

    ip4_addr_t ipaddr, netmask, gw;
    IP4_ADDR(&ipaddr,  0, 0, 0, 0);
    IP4_ADDR(&netmask, 0, 0, 0, 0);
    IP4_ADDR(&gw,      0, 0, 0, 0);

    if (netif_add(&netif, &ipaddr, &netmask, &gw,
                  NULL, ethernetif_init, ethernet_input) == NULL) {
        printf("netif_add failed\n");
        return 1;
    }
    netif_set_default(&netif);
    netif_set_up(&netif);

    printf("Starting DHCP...\n");
    dhcp_start(&netif);

    if (lwiperf_start_tcp_server_default(iperf_report, NULL) == NULL) {
        printf("lwiperf_start_tcp_server_default failed\n");
        return 1;
    }

    while (1) {
        ethernetif_input(&netif);
        sys_check_timeouts();
        check_dhcp_state();
    }

    return 0;
}
