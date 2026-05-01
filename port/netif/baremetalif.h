/* BareMetal Ethernet network interface for lwIP */
#ifndef BAREMETALIF_H
#define BAREMETALIF_H

#include "lwip/netif.h"

err_t ethernetif_init(struct netif *netif);
void  ethernetif_input(struct netif *netif);

#endif /* BAREMETALIF_H */
