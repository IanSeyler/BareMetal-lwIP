// =============================================================================
// BareMetal -- a 64-bit OS written in Assembly for x86-64 systems
// Copyright (C) 2008-2026 Return Infinity -- see LICENSE.TXT
//
// Version 1.0
// =============================================================================

// Note: All functions do not clobber registers between calls.
// The kernel is responsible for saving and restoring registers if needed.


#include "libBareMetal.h"


// Input/Output

// b_input - Read a single character from input
u8 b_input(void) {
	u8 chr;
	asm volatile ("call *0x00100010"
		: "=a" (chr)
		: /* no inputs */
		: "memory", "cc");
	return chr;
}

// b_output - Output a string (str) of length (len)
void b_output(const char *str, u64 len) {
	asm volatile ("call *0x00100018"
		: /* no outputs */
		: "S"(str), "c"(len)
		: "memory", "cc");
}


// Network

// b_net_tx - Transmit a packet from memory (mem) of length (len) with network interface id (iid)
void b_net_tx(void *mem, u64 len, u64 iid) {
	asm volatile ("call *0x00100020"
		: /* no outputs */
		: "S"(mem), "c"(len), "d"(iid)
		: "memory", "cc");
}

// b_net_rx - Receive a packet with network interface id (iid). Returns length of packet and pointer to packet memory (mem)
u64 b_net_rx(void **mem, u64 iid) {
	void *ptr;
	u64 tlong;
	asm volatile ("call *0x00100028"
		: "=D"(ptr), "=c"(tlong)
		: "d"(iid)
		: "memory", "cc");
	*mem = ptr;
	return tlong;
}


// Non-volatile Storage

// b_nvs_read - Read data from non-volatile storage to memory (mem) starting at sector (start), reading (num) bytes with drive number (drivenum)
u64 b_nvs_read(void *mem, u64 start, u64 num, u64 drivenum) {
	u64 tlong;
	register u64 rcx_in asm("rcx") = num;
	asm volatile ("call *0x00100030"
		: "=c"(tlong)
		: "a"(start), "0"(rcx_in), "d"(drivenum), "D"(mem)
		: "memory", "cc");
	return tlong;
}

// b_nvs_write - Write data to non-volatile storage from memory (mem) starting at sector (start), writing (num) bytes with drive number (drivenum)
u64 b_nvs_write(void *mem, u64 start, u64 num, u64 drivenum) {
	u64 tlong = 0;
	register u64 rcx_in asm("rcx") = num;
	asm volatile ("call *0x00100038"
		: "=c"(tlong)
		: "a"(start), "0"(rcx_in), "d"(drivenum), "S"(mem)
		: "memory", "cc");
	return tlong;
}


// System

// b_system - System call interface
u64 b_system(u64 function, u64 var1, u64 var2) {
	u64 tlong;
	register u64 rax_in asm("rax") = var1;
	asm volatile ("call *0x00100040"
		: "=a"(tlong)
		: "c"(function), "0"(rax_in), "d"(var2)
		: "memory", "cc");
	return tlong;
}


// =============================================================================
// EOF
