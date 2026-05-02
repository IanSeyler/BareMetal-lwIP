/* lwIP options for BareMetal - NO_SYS=1, callback API only */
#ifndef LWIPOPTS_H
#define LWIPOPTS_H

/* Single-threaded, no OS */
#define NO_SYS                          1
#define SYS_LIGHTWEIGHT_PROT            0

/* Use newlib malloc for all lwIP memory */
#define MEM_LIBC_MALLOC                 1
#define MEMP_MEM_MALLOC                 1
#define MEM_ALIGNMENT                   8

/* IPv4 only */
#define LWIP_IPV4                       1
#define LWIP_IPV6                       0

/* Protocols */
#define LWIP_ARP                        1
#define LWIP_ICMP                       1
#define LWIP_UDP                        1
#define LWIP_TCP                        1
#define LWIP_DHCP                       1
#define LWIP_DNS                        0
#define LWIP_IGMP                       0
#define LWIP_RAW                        0
#define LWIP_AUTOIP                     0

/* Address Conflict Detection (used by DHCP) */
#define LWIP_ACD                        1
#define LWIP_DHCP_DOES_ACD_CHECK        0

/* TCP buffers - sized for a minimal HTTP server */
#define TCP_MSS                         1460
#define TCP_SND_BUF                     (4 * TCP_MSS)
#define TCP_SND_QUEUELEN                8
#define TCP_WND                         (4 * TCP_MSS)

/* Pool counts */
#define MEMP_NUM_TCP_PCB                4
#define MEMP_NUM_TCP_PCB_LISTEN         2
#define MEMP_NUM_TCP_SEG                16
#define MEMP_NUM_UDP_PCB                4
#define MEMP_NUM_SYS_TIMEOUT            16
#define PBUF_POOL_SIZE                  16
#define PBUF_POOL_BUFSIZE               1536
#define PBUF_LINK_HLEN                  14

/* Disable sequential and socket APIs (require OS) */
#define LWIP_NETCONN                    0
#define LWIP_SOCKET                     0

/* No loopback interface */
#define LWIP_HAVE_LOOPIF                0
#define LWIP_NETIF_LOOPBACK             0

/* Disable stats to save code space */
#define LWIP_STATS                      0

/* Disable debug output unless debugging */
#define LWIP_DEBUG                      0

/* httpd: only serve static files, no CGI/SSI needed */
#define LWIP_HTTPD_CGI                  0
#define LWIP_HTTPD_SSI                  0
#define LWIP_HTTPD_SUPPORT_POST         0
#define LWIP_HTTPD_DYNAMIC_HEADERS      1

#endif /* LWIPOPTS_H */
