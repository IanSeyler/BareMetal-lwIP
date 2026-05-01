/* BareMetal lwIP port - compiler/CPU abstraction */
#ifndef LWIP_ARCH_CC_H
#define LWIP_ARCH_CC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Newlib defines ssize_t as 'long int' but does not define SSIZE_MAX.
 * lwIP/arch.h uses "#ifdef SSIZE_MAX" as a guard: when SSIZE_MAX is absent
 * it typedefs ssize_t as 'int', conflicting with newlib.  Pre-define
 * SSIZE_MAX so lwIP skips the typedef entirely.
 * Also suppress the unistd.h include (it lacks SSIZE_MAX too and drags in
 * machine/endian.h which conflicts with lwIP's own LITTLE_ENDIAN define).
 */
#define SSIZE_MAX        0x7fffffffffffffffL
#define LWIP_NO_UNISTD_H 1

/* x86-64 is little-endian */
#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

/* Packed struct support */
#define PACK_STRUCT_FIELD(x)  x
#define PACK_STRUCT_STRUCT    __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

/* Diagnostics */
#define LWIP_PLATFORM_DIAG(x)    do { printf x; } while(0)
#define LWIP_PLATFORM_ASSERT(x)  do { printf("lwIP ASSERT: %s\n", x); while(1) {} } while(0)

/* Random number generator (implemented in sys_arch.c) */
unsigned int lwip_port_rand(void);
#define LWIP_RAND() lwip_port_rand()

/* Critical section type - no OS, so no real protection needed */
typedef unsigned int sys_prot_t;

#endif /* LWIP_ARCH_CC_H */
