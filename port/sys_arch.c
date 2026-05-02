/* BareMetal lwIP port - sys_arch implementation (NO_SYS=1) */

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "libBareMetal.h"

void sys_init(void) {
    /* Nothing to initialize - single-threaded, no OS */
}

/*
 * Return current time in milliseconds.
 * b_system(TIMECOUNTER,0,0) returns the number of nanoseconds since boot.
 */
u32_t sys_now(void) {
    return (u32_t)(b_system(TIMECOUNTER, 0, 0) / 1000000);
}

/*
 * Simple LCG PRNG seeded from the TSC.
 * Used by lwIP for initial TCP sequence numbers and DHCP transaction IDs.
 */
unsigned int lwip_port_rand(void) {
    static u32_t seed = 0;
    if (seed == 0) {
        seed = (u32_t)b_system(TSC, 0, 0);
        if (seed == 0) seed = 1;
    }
    seed = seed * 1664525u + 1013904223u;
    return seed;
}
