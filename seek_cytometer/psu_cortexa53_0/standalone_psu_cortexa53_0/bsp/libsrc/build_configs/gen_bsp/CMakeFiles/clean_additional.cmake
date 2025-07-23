# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "/home/ran/seek_cytometer/software/seek_cytometer/psu_cortexa53_0/standalone_psu_cortexa53_0/bsp/include/lwipopts.h"
  "/home/ran/seek_cytometer/software/seek_cytometer/psu_cortexa53_0/standalone_psu_cortexa53_0/bsp/include/sleep.h"
  "/home/ran/seek_cytometer/software/seek_cytometer/psu_cortexa53_0/standalone_psu_cortexa53_0/bsp/include/xemac_ieee_reg.h"
  "/home/ran/seek_cytometer/software/seek_cytometer/psu_cortexa53_0/standalone_psu_cortexa53_0/bsp/include/xemacpsif_hw.h"
  "/home/ran/seek_cytometer/software/seek_cytometer/psu_cortexa53_0/standalone_psu_cortexa53_0/bsp/include/xiltimer.h"
  "/home/ran/seek_cytometer/software/seek_cytometer/psu_cortexa53_0/standalone_psu_cortexa53_0/bsp/include/xlwipconfig.h"
  "/home/ran/seek_cytometer/software/seek_cytometer/psu_cortexa53_0/standalone_psu_cortexa53_0/bsp/include/xtimer_config.h"
  "/home/ran/seek_cytometer/software/seek_cytometer/psu_cortexa53_0/standalone_psu_cortexa53_0/bsp/lib/liblwip220.a"
  "/home/ran/seek_cytometer/software/seek_cytometer/psu_cortexa53_0/standalone_psu_cortexa53_0/bsp/lib/libxiltimer.a"
  )
endif()
