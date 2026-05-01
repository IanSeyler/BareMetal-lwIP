/*
 * BareMetal Ethernet network interface for lwIP.
 *
 * Bridges the BareMetal b_net_tx / b_net_rx syscalls to the lwIP netif layer.
 * Operates at the Ethernet frame level; lwIP handles all higher-layer processing.
 *
 * MAC address retrieval:
 *   b_system(NET_STATUS, 0, 0) is assumed to return the 6-byte MAC address
 *   packed into the low 48 bits of the return value (bytes 0-5 = bits 47-0,
 *   big-endian within those bytes).  If this does not match your BareMetal
 *   build, replace get_mac_address() with the correct call or hardcode the
 *   MAC from your hardware.
 */

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/etharp.h"
#include "netif/ethernet.h"
#include "baremetalif.h"

#include <string.h>
#include <stdio.h>

#include "libBareMetal.h"

#define NETIF_MTU    1500
#define NETIF_IID    0       /* interface index: 0 = first NIC */

/* Scratch buffer for linearising pbuf chains on transmit */
static u8_t tx_buf[1536];

static void get_mac_address(u8_t mac[6]) {
    /*
     * Ask BareMetal for the MAC address.
     * NET_STATUS (0x30) with var1=0 is expected to return the MAC packed
     * into the lower 6 bytes of the u64.  Bytes are ordered so that
     * mac[0] is in bits 40-47 and mac[5] is in bits 0-7.
     *
     * If b_system returns 0 here (unsupported call or no NIC), we fall
     * back to a locally-administered placeholder MAC.
     */
    u64 raw = b_system(NET_STATUS, 0, 0);
    if (raw != 0) {
        mac[0] = (u8)(raw >> 40);
        mac[1] = (u8)(raw >> 32);
        mac[2] = (u8)(raw >> 24);
        mac[3] = (u8)(raw >> 16);
        mac[4] = (u8)(raw >> 8);
        mac[5] = (u8)(raw);
    } else {
        /* Locally-administered placeholder - replace with actual MAC */
        mac[0] = 0xDE;
        mac[1] = 0xAD;
        mac[2] = 0xBE;
        mac[3] = 0xEF;
        mac[4] = 0x01;
        mac[5] = 0x02;
    }
}

/* Transmit a pbuf chain as a single Ethernet frame */
static err_t low_level_output(struct netif *netif, struct pbuf *p) {
    LWIP_UNUSED_ARG(netif);

    u16_t len = pbuf_copy_partial(p, tx_buf, sizeof(tx_buf), 0);
    if (len == 0) return ERR_BUF;

    b_net_tx(tx_buf, (u64)len, NETIF_IID);
    return ERR_OK;
}

/* Initialise low-level hardware fields of the netif */
static err_t low_level_init(struct netif *netif) {
    netif->hwaddr_len = ETH_HWADDR_LEN;
    get_mac_address(netif->hwaddr);

    printf("lwIP netif MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
           netif->hwaddr[0], netif->hwaddr[1], netif->hwaddr[2],
           netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);

    netif->mtu = NETIF_MTU;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
    return ERR_OK;
}

/*
 * ethernetif_init - called by netif_add() to initialise the interface.
 * Sets callbacks and calls low_level_init().
 */
err_t ethernetif_init(struct netif *netif) {
    netif->name[0] = 'e';
    netif->name[1] = 'n';
    netif->output     = etharp_output;
    netif->linkoutput = low_level_output;
    return low_level_init(netif);
}

/*
 * ethernetif_input - poll for received frames and feed them into lwIP.
 * Call this from the main loop as often as practical.
 */
void ethernetif_input(struct netif *netif) {
    void *buffer = NULL;
    u64 len = b_net_rx(&buffer, NETIF_IID);

    if (len == 0) return;

//    printf("b_net_rx: len=%lu buffer=%p\n", len, (void *)buffer);

    //debug: print the address of the buffer pointer
//    printf("Buffer pointer post: %p\n", buffer);

    //debug: dump packet length
//    printf("Packet received: len=%lu\n", len);

//    //debug: dump packet contents
//    printf("Packet contents: ");
//    for (u64 i = 0; i < len; i++) {
//        printf("%02X ", ((u8_t *)buffer)[i]);
//    }
//    printf("\n");

    if (len > 1514) len = 1514;

    struct pbuf *p = pbuf_alloc(PBUF_RAW, (u16_t)len, PBUF_POOL);
    if (p == NULL) return;

    pbuf_take(p, buffer, (u16_t)len);

    if (netif->input(p, netif) != ERR_OK) {
        pbuf_free(p);
    }
}
