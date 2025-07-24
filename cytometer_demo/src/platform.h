#ifndef __PLATFORM_HEADER__
#define __PLATFORM_HEADER__

#include "arch/cc.h"
#include "xstatus.h"

XStatus init_platform();

u64_t get_platform_time();

u64_t get_platform_time_ms();
#endif