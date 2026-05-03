/* lwIP options for BareMetal - NO_SYS=1, callback API only */

/* Tweaked settings for throughput in iperf */

#ifndef LWIPOPTS_H
#define LWIPOPTS_H

/* Single-threaded, no OS */
#define NO_SYS                          1
#define SYS_LIGHTWEIGHT_PROT            0

/* Use newlib malloc for all lwIP memory */
#define MEM_LIBC_MALLOC                 1
#define MEMP_MEM_MALLOC                 1
#define MEM_ALIGNMENT                   8
#define MEM_SIZE                        (1024 * 1024)

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

/* Fast path for a local Ethernet network */
//#define IP_REASSEMBLY                   0
//#define IP_FRAG                         0
//#define IP_OPTIONS_ALLOWED              0

/* Address Conflict Detection (used by DHCP) */
#define LWIP_ACD                        1
#define LWIP_DHCP_DOES_ACD_CHECK        0

/* TCP buffers - tuned for throughput over standard 1500-byte Ethernet */
#define TCP_MSS                         1460
#define TCP_SND_BUF                     (128 * TCP_MSS)
#define TCP_SND_QUEUELEN                (4 * (TCP_SND_BUF / TCP_MSS))
#define TCP_SNDLOWAT                    (16 * TCP_MSS)
#define TCP_SNDQUEUELOWAT               64
#define TCP_WND                         (256 * TCP_MSS)
#define TCP_WND_UPDATE_THRESHOLD        (16 * TCP_MSS)
#define TCP_QUEUE_OOSEQ                 1
#define TCP_OVERSIZE                    TCP_MSS
#define LWIP_WND_SCALE                  1
#define TCP_RCV_SCALE                   3
#define TCP_CALCULATE_EFF_SEND_MSS      1
#define LWIP_TCP_TIMESTAMPS             0

/* Pool counts */
#define MEMP_NUM_TCP_PCB                16
#define MEMP_NUM_TCP_PCB_LISTEN         4
#define MEMP_NUM_TCP_SEG                512
#define MEMP_NUM_UDP_PCB                8
#define MEMP_NUM_SYS_TIMEOUT            16
#define PBUF_POOL_SIZE                  512
#define PBUF_POOL_BUFSIZE               1536
#define PBUF_LINK_HLEN                  14
#define LWIP_NETIF_TX_SINGLE_PBUF       1

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
