/*
 * BareMetal lwIP web server demo.
 *
 * Initialises lwIP in NO_SYS=1 (polling) mode, obtains an IP address via
 * DHCP, then serves HTTP on port 80 using lwIP's built-in httpd.
 *
 * Main loop:
 *   1. Poll the NIC for received frames.
 *   2. Call sys_check_timeouts() so DHCP retransmits, TCP keepalives, etc.
 *      all fire on schedule.
 *
 * The HTTP file system content is compiled in from
 * lwip-2.2.1/src/apps/http/fsdata.c (the default lwIP demo pages).
 */

#include <stdio.h>
#include <string.h>

#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "lwip/dhcp.h"
#include "lwip/ip_addr.h"
#include "lwip/apps/httpd.h"
#include "netif/ethernet.h"

#include "libBareMetal.h"
#include "netif/baremetalif.h"

static struct netif netif;
static int dhcp_bound = 0;

static void check_dhcp_state(void) {
    if (dhcp_bound) return;
    if (!dhcp_supplied_address(&netif)) return;

    dhcp_bound = 1;
    printf("DHCP bound!\n");
    printf("  IP  : %s\n", ipaddr_ntoa(&netif.ip_addr));
    printf("  Mask: %s\n", ipaddr_ntoa(&netif.netmask));
    printf("  GW  : %s\n", ipaddr_ntoa(&netif.gw));
    printf("HTTP server listening on port 80\n");
}

int main(void) {
    printf("BareMetal lwIP Web Server\n");
    printf("=========================\n");

    /* Wait for NIC to come up */
    printf("Waiting for network...\n");
    while (b_system(NET_STATUS, 0, 0) == 0) {
        /* spin - could add a timeout here */
    }
    printf("Network interface ready.\n");

    /* Initialise lwIP */
    lwip_init();

    /* Add the BareMetal Ethernet interface */
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

    /* Start DHCP */
    printf("Starting DHCP...\n");
    dhcp_start(&netif);

    /* Start HTTP server */
    httpd_init();

    /* Main polling loop */
    while (1) {
        ethernetif_input(&netif);
        sys_check_timeouts();
        check_dhcp_state();
    }

    return 0;
}
