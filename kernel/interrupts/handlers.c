#include "handlers.h"
#include "../lib/printf.h"
#include "../lib/util.h"
#include <stdint.h>

// GP handler
void general_protection_fault_interrupt(uint64_t ip) {
	printf(RED "GP | V=0x0D | IP=0x%llX\n" RESET, ip);
	panic();
}


// PF handler
void page_fault_interrupt(uint64_t ip) {
	printf(RED "PF | V=0x0E | IP=0x%llX\n" RESET, ip);
	panic();
}

// DF handler
void double_fault_interrupt(uint64_t ip) {
	printf(RED "DF | V=0x08 | IP=0x%llX\n" RESET, ip);
	panic();
}