/******************************************************************************
* Copyright (C) 2010 - 2022 Xilinx, Inc.  All rights reserved.
* Copyright (C) 2022 - 2025 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xscugic_hw.h
* @addtogroup scugic_api SCUGIC APIs
* @{
*
* The xscugic_hw.h header file contains identifiers and hardware access functions (or
* macros) that can be used to access the device. The user should refer to the
* hardware device specification for more details of the device operation.
* The driver functions/APIs are defined in xscugic.h.
*
* This GIC device has two parts, a distributor and CPU interface(s). Each part
* has separate register definition sections.
*
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------------
* 1.00a drg  01/19/10 First release
* 1.01a sdm  11/09/11 "xil_exception.h" added as include.
*		      Macros XScuGic_EnableIntr and XScuGic_DisableIntr are
*		      added to enable or disable interrupts based on
*		      Distributor Register base address. Normally users use
*		      XScuGic instance and call XScuGic_Enable or
*		      XScuGic_Disable to enable/disable interrupts. These
*		      new macros are provided when user does not want to
*		      use an instance pointer but still wants to enable or
*		      disable interrupts.
*		      Function prototypes for functions (present in newly
*		      added file xscugic_hw.c) are added.
* 1.03a srt  02/27/13 Moved Offset calculation macros from *_hw.c (CR
*		      702687).
* 1.04a hk   05/04/13 Fix for CR#705621. Moved function prototypes
*		      XScuGic_SetPriTrigTypeByDistAddr and
*         	      XScuGic_GetPriTrigTypeByDistAddr here from xscugic.h
* 3.0	pkp  12/09/14 changed XSCUGIC_MAX_NUM_INTR_INPUTS for
*		      Zynq Ultrascale Mp
* 3.0   kvn  02/13/14 Modified code for MISRA-C:2012 compliance.
* 3.2	pkp  11/09/15 Corrected the interrupt processsor target mask value
*					  for CPU interface 2 i.e. XSCUGIC_SPI_CPU2_MASK
* 3.9   mus  02/21/18 Added new API's XScuGic_InterruptUnmapFromCpuByDistAddr
*					  and XScuGic_UnmapAllInterruptsFromCpuByDistAddr, These
*					  API's can be used by applications to unmap specific/all
*					  interrupts from target CPU. It fixes CR#992490.
* 3.10  aru  08/23/18 Resolved MISRA-C:2012 compliance mandatory violations
* 4.1   asa  03/30/19 Removed macros for XScuGic_EnableIntr, and
*                     XScuGic_DisableIntr. These are now C functions. This
*                     change was to fix CR-1024716.
* 4.1   mus  06/12/19 Updated XSCUGIC_MAX_NUM_INTR_INPUTS for Versal.
* 4.6	sk   06/07/21 Delete the commented macro code to fix the MISRA-C warning.
* 4.6	sk   08/05/21 Fix Scugic Misrac violations.
* 4.7	sk   12/10/21 Update XSCUGIC_SPI_INT_ID_START macro from signed to unsigned
* 		      to fix misrac violation.
* 4.7   mus  03/17/22 GICv3 coupled with A72 has different redistributor for
*                     each core, and each redistributor has different address,
*                     Updated #define for re-distributor address to have correct
*                     value based on the cpu number. It fixes CR#1126156.
* 5.0   mus  22/02/22 Added support for VERSAL NET
* 5.1   mus  02/13/23 Added #defines required for logic to find redistributor
*                     based address for specific CPU core. Also, added new macro
*                     XScuGic_ReadReg64 to read 64 bit value from specific address.
* 5.1   mus  02/15/23 Added support for VERSAL_NET APU and RPU GIC.
* 5.2   ml   03/02/23 Add description to fix Doxygen warnings.
* 5.2   mus  03/26/23 Fixed calculation for XSCUGIC_RDIST_INT_PRIORITY_OFFSET_CALC.
* 5.4   ksr  07/23/24 Added support for Versal_2VE_2VM
* 5.5   ml   01/08/25 Update datatype of function arguments from u32 to UINTPTR to
*                     support both 32bit and 64bit platforms.
* </pre>
*
******************************************************************************/

#ifndef XSCUGIC_HW_H /**< prevent circular inclusions */
#define XSCUGIC_HW_H /**< by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "xil_types.h"
#include "xil_assert.h"
#include "xil_io.h"
#include "xil_exception.h"
#include "bspconfig.h"

/************************** Constant Definitions *****************************/
#if (defined (versal) && !defined(ARMR5)) || defined (ARMR52)
#define GICv3
#endif

#if defined (VERSAL_NET) && ! defined (ARMR52)
#define GIC600
#endif

#if defined (VERSAL_NET)
#if defined (ARMR52)
#define XSCUGIC_NUM_OF_CORES_PER_CLUSTER 2U
#else
#define XSCUGIC_NUM_OF_CORES_PER_CLUSTER 4U
#endif
#endif

/*
 * The maximum number of interrupts supported by the hardware.
 */
#ifdef PLATFORM_ZYNQ
#define XSCUGIC_MAX_NUM_INTR_INPUTS    95U /**< Maximum number of
                                                interrupt defined by
                                                Zynq */
#elif defined (VERSAL_2VE_2VM)
#define XSCUGIC_MAX_NUM_INTR_INPUTS     288U /**< Maximum number of
                                                  interrupt sources in
                                                  Versal_2VE_2VM */
#elif defined (VERSAL_NET)
#define XSCUGIC_MAX_NUM_INTR_INPUTS     256U /**< Maximum number of
                                                  interrupt sources in
                                                  VERSAL NET */
#elif defined (versal)
#define XSCUGIC_MAX_NUM_INTR_INPUTS     192U /**< Maximum number of
                                                  interrupt sources in
                                                  VERSAL */
#else
#define XSCUGIC_MAX_NUM_INTR_INPUTS     195U /**< Maximum number of
                                                  interrupt defined by
                                                  Zynq Ultrascale Mp */
#endif

#define XSCUGIC_SPI_INT_ID_START       0x20U /**< First Interrupt Id for
                                                   SPI interrupts. */

#define XSCUGIC_MAX_INTR_PRIO_VAL      248U /**< The maximum priority value
                                                  that can be used in
                                                  the GIC. */
#define XSCUGIC_INTR_PRIO_MASK         0x000000F8U /**< The Interrupt
                                                       priority mask value */

/** @name Distributor Interface Register Map
 *
 * Define the offsets from the base address for all Distributor registers of
 * the interrupt controller, some registers may be reserved in the hardware
 * device.
 * @{
 */
#define XSCUGIC_DIST_EN_OFFSET		0x00000000U /**< Distributor Enable
							Register */
#define XSCUGIC_IC_TYPE_OFFSET		0x00000004U /**< Interrupt Controller
							Type Register */
#define XSCUGIC_DIST_IDENT_OFFSET	0x00000008U /**< Implementor ID
							Register */
#define XSCUGIC_SECURITY_OFFSET		0x00000080U /**< Interrupt Security
							Register */
#define XSCUGIC_ENABLE_SET_OFFSET	0x00000100U /**< Enable Set
							Register */
#define XSCUGIC_DISABLE_OFFSET		0x00000180U /**< Enable Clear Register */
#define XSCUGIC_PENDING_SET_OFFSET	0x00000200U /**< Pending Set
							Register */
#define XSCUGIC_PENDING_CLR_OFFSET	0x00000280U /**< Pending Clear
							Register */
#define XSCUGIC_ACTIVE_OFFSET		0x00000300U /**< Active Status Register */
#define XSCUGIC_PRIORITY_OFFSET		0x00000400U /**< Priority Level Register */
#define XSCUGIC_SPI_TARGET_OFFSET	0x00000800U /**< SPI Target
							Register 0x800-0x8FB */
#define XSCUGIC_INT_CFG_OFFSET		0x00000C00U /**< Interrupt Configuration
							Register 0xC00-0xCFC */
#define XSCUGIC_PPI_STAT_OFFSET		0x00000D00U /**< PPI Status Register */
#define XSCUGIC_SPI_STAT_OFFSET		0x00000D04U /**< SPI Status Register
							0xd04-0xd7C */
#define XSCUGIC_AHB_CONFIG_OFFSET	0x00000D80U /**< AHB Configuration
							Register */
#define XSCUGIC_SFI_TRIG_OFFSET		0x00000F00U /**< Software Triggered
							Interrupt Register */
#define XSCUGIC_PERPHID_OFFSET		0x00000FD0U /**< Peripheral ID Reg */
#if defined (GICv3)
#define XSCUGIC_PCELLID_OFFSET		0x0000FFF0U /**< Pcell ID Register */
#else
#define XSCUGIC_PCELLID_OFFSET		0x00000FF0U /**< Pcell ID Register */
#endif
/* @} */

/** @name  Distributor Enable Register
 * Controls if the distributor response to external interrupt inputs.
 * @{
 */
#if defined (GICv3)
#define XSCUGIC_EN_INT_MASK		0x00000003U /**< Interrupt In Enable */
#else
#define XSCUGIC_EN_INT_MASK		0x00000001U /**< Interrupt In Enable */
#endif
/* @} */

/** @name  Interrupt Controller Type Register
 * @{
 */
#define XSCUGIC_LSPI_MASK	0x0000F800U /**< Number of Lockable
						Shared Peripheral
						Interrupts*/
#define XSCUGIC_DOMAIN_MASK	0x00000400U /**< Number os Security domains*/
#define XSCUGIC_CPU_NUM_MASK	0x000000E0U /**< Number of CPU Interfaces */
#define XSCUGIC_NUM_INT_MASK	0x0000001FU /**< Number of Interrupt IDs */
/* @} */

/** @name  Implementor ID Register
 * Implementor and revision information.
 * @{
 */
#define XSCUGIC_REV_MASK	0x00FFF000U /**< Revision Number */
#define XSCUGIC_IMPL_MASK	0x00000FFFU /**< Implementor */
/* @} */

/** @name  Interrupt Security Registers
 * Each bit controls the security level of an interrupt, either secure or non
 * secure. These registers can only be accessed using secure read and write.
 * There are registers for each of the CPU interfaces at offset 0x080.  A
 * register set for the SPI interrupts is available to all CPU interfaces.
 * There are up to 32 of these registers staring at location 0x084.
 * @{
 */
#define XSCUGIC_INT_NS_MASK	0x00000001U /**< Each bit corresponds to an
						INT_ID */
/* @} */

/** @name  Enable Set Register
 * Each bit controls the enabling of an interrupt, a 0 is disabled, a 1 is
 * enabled. Writing a 0 has no effect. Use the ENABLE_CLR register to set a
 * bit to 0.
 * There are registers for each of the CPU interfaces at offset 0x100. With up
 * to 8 registers aliased to the same address. A register set for the SPI
 * interrupts is available to all CPU interfaces.
 * There are up to 32 of these registers staring at location 0x104.
 * @{
 */
#define XSCUGIC_INT_EN_MASK	0x00000001U /**< Each bit corresponds to an
						INT_ID */
/* @} */

/** @name  Enable Clear Register
 * Each bit controls the disabling of an interrupt, a 0 is disabled, a 1 is
 * enabled. Writing a 0 has no effect. Writing a 1 disables an interrupt and
 * sets the corresponding bit to 0.
 * There are registers for each of the CPU interfaces at offset 0x180. With up
 * to 8 registers aliased to the same address.
 * A register set for the SPI interrupts is available to all CPU interfaces.
 * There are up to 32 of these registers staring at location 0x184.
 * @{
 */
#define XSCUGIC_INT_CLR_MASK	0x00000001U /**< Each bit corresponds to an
						INT_ID */
/* @} */

/** @name  Pending Set Register
 * Each bit controls the Pending or Active and Pending state of an interrupt, a
 * 0 is not pending, a 1 is pending. Writing a 0 has no effect. Writing a 1 sets
 * an interrupt to the pending state.
 * There are registers for each of the CPU interfaces at offset 0x200. With up
 * to 8 registers aliased to the same address.
 * A register set for the SPI interrupts is available to all CPU interfaces.
 * There are up to 32 of these registers staring at location 0x204.
 * @{
 */
#define XSCUGIC_PEND_SET_MASK	0x00000001U /**< Each bit corresponds to an
						INT_ID */
/* @} */

/** @name  Pending Clear Register
 * Each bit can clear the Pending or Active and Pending state of an interrupt, a
 * 0 is not pending, a 1 is pending. Writing a 0 has no effect. Writing a 1
 * clears the pending state of an interrupt.
 * There are registers for each of the CPU interfaces at offset 0x280. With up
 * to 8 registers aliased to the same address.
 * A register set for the SPI interrupts is available to all CPU interfaces.
 * There are up to 32 of these registers staring at location 0x284.
 * @{
 */
#define XSCUGIC_PEND_CLR_MASK	0x00000001U /**< Each bit corresponds to an
						INT_ID */
/* @} */

/** @name  Active Status Register
 * Each bit provides the Active status of an interrupt, a
 * 0 is not Active, a 1 is Active. This is a read only register.
 * There are registers for each of the CPU interfaces at offset 0x300. With up
 * to 8 registers aliased to each address.
 * A register set for the SPI interrupts is available to all CPU interfaces.
 * There are up to 32 of these registers staring at location 0x380.
 * @{
 */
#define XSCUGIC_ACTIVE_MASK	0x00000001U /**< Each bit corresponds to an
					      INT_ID */
/* @} */

/** @name  Priority Level Register
 * Each byte in a Priority Level Register sets the priority level of an
 * interrupt. Reading the register provides the priority level of an interrupt.
 * There are registers for each of the CPU interfaces at offset 0x400 through
 * 0x41C. With up to 8 registers aliased to each address.
 * 0 is highest priority, 0xFF is lowest.
 * A register set for the SPI interrupts is available to all CPU interfaces.
 * There are up to 255 of these registers staring at location 0x420.
 * @{
 */
#define XSCUGIC_PRIORITY_MASK	0x000000FFU /**< Each Byte corresponds to an
						INT_ID */
#define XSCUGIC_PRIORITY_MAX	0x000000FFU /**< Highest value of a priority
						actually the lowest priority*/
/* @} */

/** @name  SPI Target Register 0x800-0x8FB
 * Each byte references a separate SPI and programs which of the up to 8 CPU
 * interfaces are sent a Pending interrupt.
 * There are registers for each of the CPU interfaces at offset 0x800 through
 * 0x81C. With up to 8 registers aliased to each address.
 * A register set for the SPI interrupts is available to all CPU interfaces.
 * There are up to 255 of these registers staring at location 0x820.
 *
 * This driver does not support multiple CPU interfaces. These are included
 * for complete documentation.
 * @{
 */
#define XSCUGIC_SPI_CPU7_MASK	0x00000080U /**< CPU 7 Mask*/
#define XSCUGIC_SPI_CPU6_MASK	0x00000040U /**< CPU 6 Mask*/
#define XSCUGIC_SPI_CPU5_MASK	0x00000020U /**< CPU 5 Mask*/
#define XSCUGIC_SPI_CPU4_MASK	0x00000010U /**< CPU 4 Mask*/
#define XSCUGIC_SPI_CPU3_MASK	0x00000008U /**< CPU 3 Mask*/
#define XSCUGIC_SPI_CPU2_MASK	0x00000004U /**< CPU 2 Mask*/
#define XSCUGIC_SPI_CPU1_MASK	0x00000002U /**< CPU 1 Mask*/
#define XSCUGIC_SPI_CPU0_MASK	0x00000001U /**< CPU 0 Mask*/
/* @} */

/** @name  Interrupt Configuration Register 0xC00-0xCFC
 * The interrupt configuration registers program an SFI to be active HIGH level
 * sensitive or rising edge sensitive.
 * Each bit pair describes the configuration for an INT_ID.
 * SFI    Read Only    b10 always
 * PPI    Read Only    depending on how the PPIs are configured.
 *                    b01    Active HIGH level sensitive
 *                    b11 Rising edge sensitive
 * SPI                LSB is read only.
 *                    b01    Active HIGH level sensitive
 *                    b11 Rising edge sensitive/
 * There are registers for each of the CPU interfaces at offset 0xC00 through
 * 0xC04. With up to 8 registers aliased to each address.
 * A register set for the SPI interrupts is available to all CPU interfaces.
 * There are up to 255 of these registers staring at location 0xC08.
 * @{
 */
#define XSCUGIC_INT_CFG_MASK    0x00000003U /**< Interrupt configuration Mask */
/* @} */

/** @name  PPI Status Register
 * Enables an external AMBA master to access the status of the PPI inputs.
 * A CPU can only read the status of its local PPI signals and cannot read the
 * status for other CPUs.
 * This register is aliased for each CPU interface.
 * @{
 */
#define XSCUGIC_PPI_C15_MASK	0x00008000U    /**< PPI Status */
#define XSCUGIC_PPI_C14_MASK	0x00004000U    /**< PPI Status */
#define XSCUGIC_PPI_C13_MASK	0x00002000U    /**< PPI Status */
#define XSCUGIC_PPI_C12_MASK	0x00001000U    /**< PPI Status */
#define XSCUGIC_PPI_C11_MASK	0x00000800U    /**< PPI Status */
#define XSCUGIC_PPI_C10_MASK	0x00000400U    /**< PPI Status */
#define XSCUGIC_PPI_C09_MASK	0x00000200U    /**< PPI Status */
#define XSCUGIC_PPI_C08_MASK	0x00000100U    /**< PPI Status */
#define XSCUGIC_PPI_C07_MASK	0x00000080U    /**< PPI Status */
#define XSCUGIC_PPI_C06_MASK	0x00000040U    /**< PPI Status */
#define XSCUGIC_PPI_C05_MASK	0x00000020U    /**< PPI Status */
#define XSCUGIC_PPI_C04_MASK	0x00000010U    /**< PPI Status */
#define XSCUGIC_PPI_C03_MASK	0x00000008U    /**< PPI Status */
#define XSCUGIC_PPI_C02_MASK	0x00000004U    /**< PPI Status */
#define XSCUGIC_PPI_C01_MASK	0x00000002U    /**< PPI Status */
#define XSCUGIC_PPI_C00_MASK	0x00000001U    /**< PPI Status */
/* @} */

/** @name  SPI Status Register 0xd04-0xd7C
 * Enables an external AMBA master to access the status of the SPI inputs.
 * There are up to 63 registers if the maximum number of SPI inputs are
 * configured.
 * @{
 */
#define XSCUGIC_SPI_N_MASK    0x00000001U    /**< Each bit corresponds to an SPI
					     input */
/* @} */

/** @name  AHB Configuration Register
 * Provides the status of the CFGBIGEND input signal and allows the endianness
 * of the GIC to be set.
 * @{
 */
#define XSCUGIC_AHB_END_MASK       0x00000004U    /**< 0-GIC uses little Endian,
                                                  1-GIC uses Big Endian */
#define XSCUGIC_AHB_ENDOVR_MASK    0x00000002U    /**< 0-Uses CFGBIGEND control,
                                                  1-use the AHB_END bit */
#define XSCUGIC_AHB_TIE_OFF_MASK   0x00000001U    /**< State of CFGBIGEND */

/* @} */

/** @name  Software Triggered Interrupt Register
 * Controls issuing of software interrupts.
 * @{
 */
#define XSCUGIC_SFI_SELFTRIG_MASK	0x02010000U
#define XSCUGIC_SFI_TRIG_TRGFILT_MASK    0x03000000U    /**< Target List filter
                                                            b00-Use the target List
                                                            b01-All CPUs except requester
                                                            b10-To Requester
                                                            b11-reserved */
#define XSCUGIC_SFI_TRIG_CPU_MASK	0x00FF0000U    /**< CPU Target list */
#define XSCUGIC_SFI_TRIG_SATT_MASK	0x00008000U    /**< 0= Use a secure interrupt */
#define XSCUGIC_SFI_TRIG_INTID_MASK	0x0000000FU    /**< Set to the INTID
                                                        signaled to the CPU*/
/* @} */

/** @name CPU Interface Register Map
 *
 * Define the offsets from the base address for all CPU registers of the
 * interrupt controller, some registers may be reserved in the hardware device.
 * @{
 */
#define XSCUGIC_CONTROL_OFFSET		0x00000000U /**< CPU Interface Control
							Register */
#define XSCUGIC_CPU_PRIOR_OFFSET	0x00000004U /**< Priority Mask Reg */
#define XSCUGIC_BIN_PT_OFFSET		0x00000008U /**< Binary Point Register */
#define XSCUGIC_INT_ACK_OFFSET		0x0000000CU /**< Interrupt ACK Reg */
#define XSCUGIC_EOI_OFFSET		0x00000010U /**< End of Interrupt Reg */
#define XSCUGIC_RUN_PRIOR_OFFSET	0x00000014U /**< Running Priority Reg */
#define XSCUGIC_HI_PEND_OFFSET		0x00000018U /**< Highest Pending Interrupt
							Register */
#define XSCUGIC_ALIAS_BIN_PT_OFFSET	0x0000001CU /**< Aliased non-Secure
						        Binary Point Register */

/**<  0x00000020 to 0x00000FBC are reserved and should not be read or written
 * to. */
/* @} */


/** @name Control Register
 * CPU Interface Control register definitions
 * All bits are defined here although some are not available in the non-secure
 * mode.
 * @{
 */
#define XSCUGIC_CNTR_SBPR_MASK	0x00000010U    /**< Secure Binary Pointer,
                                                 0=separate registers,
                                                 1=both use bin_pt_s */
#define XSCUGIC_CNTR_FIQEN_MASK	0x00000008U    /**< Use nFIQ_C for secure
                                                  interrupts,
                                                  0= use IRQ for both,
                                                  1=Use FIQ for secure, IRQ for non*/
#define XSCUGIC_CNTR_ACKCTL_MASK	0x00000004U    /**< Ack control for secure or non secure */
#define XSCUGIC_CNTR_EN_NS_MASK		0x00000002U    /**< Non Secure enable */
#define XSCUGIC_CNTR_EN_S_MASK		0x00000001U    /**< Secure enable, 0=Disabled, 1=Enabled */
/* @} */


/** @name Binary Point Register
 * Binary Point register definitions
 * @{
 */

#define XSCUGIC_BIN_PT_MASK	0x00000007U  /**< Binary point mask value
						Value  Secure  Non-secure
						b000    0xFE    0xFF
						b001    0xFC    0xFE
						b010    0xF8    0xFC
						b011    0xF0    0xF8
						b100    0xE0    0xF0
						b101    0xC0    0xE0
						b110    0x80    0xC0
						b111    0x00    0x80
						*/
/*@}*/

/** @name Interrupt Acknowledge Register
 * Interrupt Acknowledge register definitions
 * Identifies the current Pending interrupt, and the CPU ID for software
 * interrupts.
 */
#define XSCUGIC_ACK_INTID_MASK		0x000003FFU /**< Interrupt ID */
#define XSCUGIC_CPUID_MASK		0x00000C00U /**< CPU ID */
/* @} */

/** @name End of Interrupt Register
 * End of Interrupt register definitions
 * Allows the CPU to signal the GIC when it completes an interrupt service
 * routine.
 */
#define XSCUGIC_EOI_INTID_MASK		0x000003FFU /**< Interrupt ID */

/* @} */

/** @name Running Priority Register
 * Running Priority register definitions
 * Identifies the interrupt priority level of the highest priority active
 * interrupt.
 */
#define XSCUGIC_RUN_PRIORITY_MASK	0x000000FFU    /**< Interrupt Priority */
/* @} */

#if defined (GICv3)
#define XSCUGIC_IROUTER_BASE_OFFSET 0x6000U
#endif
/*
 * Highest Pending Interrupt register definitions
 * Identifies the interrupt priority of the highest priority pending interrupt
 */
#define XSCUGIC_PEND_INTID_MASK		0x000003FFU /**< Pending Interrupt ID */
/* @} */
#if defined (GICv3)
/** @name ReDistributor Interface Register Map
 *
 * @{
 */
#if defined (VERSAL_NET) && ! defined (ARMR52)
#define XSCUGIC_RDIST_START_ADDR	0xE2060000U
#define XSCUGIC_RDIST_END_ADDR		0xE2260000U
#elif defined (ARMR52)
#define XSCUGIC_RDIST_START_ADDR        0xE2100000U
#define XSCUGIC_RDIST_END_ADDR          0xE2130000U
#else
#if EL1_NONSECURE
#define XSCUGIC_RDIST_START_ADDR        0x03020000U
#define XSCUGIC_RDIST_END_ADDR          0x03040000U
#else
#define XSCUGIC_RDIST_START_ADDR        0xF9080000U
#define XSCUGIC_RDIST_END_ADDR          0xF90B0000U
#endif
#endif
#define XSCUGIC_RDIST_OFFSET		0x20000U /* offset between consecutive redistributors */
#define XSCUGIC_RDIST_SGI_PPI_OFFSET	0x10000U  /* offset between control redistributor and SGI/PPI redistributor */
#define XSCUGIC_GICR_TYPER_AFFINITY_SHIFT 32U
#define XSCUGIC_GICR_TYPER_AFFINITY_MASK 0xFFFFFFFF00000000UL

#define XSCUGIC_RDIST_ISENABLE_OFFSET     0x100U
#define XSCUGIC_RDIST_IPRIORITYR_OFFSET   0x400U
#define XSCUGIC_RDIST_IGROUPR_OFFSET      0x80U

#define XSCUGIC_RDIST_GRPMODR_OFFSET      0xD00U
#define XSCUGIC_RDIST_INT_CONFIG_OFFSET   0xC00U
#define XSCUGIC_RDIST_TYPER_OFFSET        0x8U
#define XSCUGIC_RDIST_WAKER_OFFSET        0x14U
#define XSCUGIC_SGIR_EL1_INITID_SHIFT    24U

#if defined (GIC600)
#define XSCUGIC_RDIST_PWRR_OFFSET	0x24U
#endif
/*
 * GICR_IGROUPR  register definitions
 */
#if EL3
#define XSCUGIC_DEFAULT_SECURITY    0x0U
#else
#define XSCUGIC_DEFAULT_SECURITY    0xFFFFFFFFU
#endif
/*
 * GICR_WAKER  register definitions
 */
#define XSCUGIC_RDIST_WAKER_LOW_POWER_STATE_MASK    0x7
#define XSCUGIC_RDIST_PWRR_RDPD_MASK	0x1U
#endif
/***************** Macros (Inline Functions) Definitions *********************/

/****************************************************************************/
/**
*
* Reads the Interrupt Configuration Register offset for an interrupt ID.
*
* @param	InterruptID Interrupt number.
*
* @return	The 32-bit value of the offset
*
*
*****************************************************************************/
#define XSCUGIC_INT_CFG_OFFSET_CALC(InterruptID) \
	((u32)XSCUGIC_INT_CFG_OFFSET + (((InterruptID)/16U) * 4U))

/****************************************************************************/
/**
*
* Reads the Interrupt Priority Register offset for an interrupt ID.
*
* @param	InterruptID Interrupt number.
*
* @return	The 32-bit value of the offset
*
*
*****************************************************************************/
#define XSCUGIC_PRIORITY_OFFSET_CALC(InterruptID) \
	((u32)XSCUGIC_PRIORITY_OFFSET + (((InterruptID)/4U) * 4U))

/****************************************************************************/
/**
*
* Reads the Interrupt Routing Register offset for an interrupt ID.
*
* @param	InterruptID Interrupt number.
*
* @return	The 32-bit value of the offset
*
*
*****************************************************************************/
#define XSCUGIC_IROUTER_OFFSET_CALC(InterruptID) \
	((u32)XSCUGIC_IROUTER_BASE_OFFSET + (InterruptID * 8))

/****************************************************************************/
/**
*
* Reads the SPI Target Register offset for an interrupt ID.
*
* @param	InterruptID Interrupt number.
*
* @return	The 32-bit value of the offset.
*
*
*****************************************************************************/
#define XSCUGIC_SPI_TARGET_OFFSET_CALC(InterruptID) \
	((u32)XSCUGIC_SPI_TARGET_OFFSET + (((InterruptID)/4U) * 4U))
/****************************************************************************/
/**
*
* Reads the SPI Target Register offset for an interrupt ID.
*
* @param	InterruptID Interrupt number.
*
* @return	The 32-bit value of the offset
*
*
*****************************************************************************/
#define XSCUGIC_SECURITY_TARGET_OFFSET_CALC(InterruptID) \
	((u32)XSCUGIC_SECURITY_OFFSET + (((InterruptID)/32U)*4U))

/****************************************************************************/
/**
*
* Reads the Re-distributor Interrupt configuration register offset.
*
* @param	InterruptID Interrupt number.
*
* @return	The 32-bit value of the offset
*
*
*****************************************************************************/
#define XSCUGIC_RDIST_INT_CONFIG_OFFSET_CALC(InterruptID) \
	((u32)XSCUGIC_RDIST_INT_CONFIG_OFFSET + ((InterruptID /16)*4))

/****************************************************************************/
/**
*
* Reads the Re-distributor Interrupt Priority register offset.
*
* @param	InterruptID Interrupt number.
*
* @return	The 32-bit value of the offset
*
*
*****************************************************************************/
#define XSCUGIC_RDIST_INT_PRIORITY_OFFSET_CALC(InterruptID) \
	((u32)XSCUGIC_RDIST_IPRIORITYR_OFFSET + ((InterruptID/4) * 4))
/****************************************************************************/
/**
*
* Reads the Interrupt Clear-Enable Register offset for an interrupt ID.
*
* @param	Register Register offset for the clear/enable bank.
* @param	InterruptID Interrupt number.
*
* @return	The 32-bit value of the offset
*
*
*****************************************************************************/
#define XSCUGIC_EN_DIS_OFFSET_CALC(Register, InterruptID) \
		((Register) + (((InterruptID)/32U) * 4U))

/****************************************************************************/
/**
*
* Reads the given Intc register.
*
* @param	BaseAddress Base address of the device.
* @param	RegOffset Register offset to be read.
*
* @return	The 32-bit value of the register
*
* @note  C-style signature:
*        u32 XScuGic_ReadReg(u32 BaseAddress, u32 RegOffset)
*
*****************************************************************************/
#define XScuGic_ReadReg(BaseAddress, RegOffset) \
	(Xil_In32((BaseAddress) + (RegOffset)))

/****************************************************************************/
/**
*
* Reads the given Intc register.
*
* @param        BaseAddress Base address of the device.
* @param        RegOffset Register offset to be read.
*
* @return       The 64-bit value of the register
*
* @note  C-style signature:
*        u32 XScuGic_ReadReg64(UINTPTR BaseAddress, u32 RegOffset)
*
*****************************************************************************/
#define XScuGic_ReadReg64(BaseAddress, RegOffset) \
        (Xil_In64((BaseAddress) + (RegOffset)))

/****************************************************************************/
/**
*
* Writes the given Intc register.
*
* @param	BaseAddress Base address of the device.
* @param	RegOffset Register offset to be written
* @param	Data 32-bit value to write to the register
*
* @return	None.
*
* @note  C-style signature:
*        void XScuGic_WriteReg(u32 BaseAddress, u32 RegOffset, u32 Data)
*
*****************************************************************************/
#define XScuGic_WriteReg(BaseAddress, RegOffset, Data) \
	(Xil_Out32(((BaseAddress) + (RegOffset)), ((u32)(Data))))


/************************** Function Prototypes ******************************/

void XScuGic_DeviceInterruptHandler(void *DeviceId);
s32  XScuGic_DeviceInitialize(u32 DeviceId);
void XScuGic_RegisterHandler(UINTPTR BaseAddress, s32 InterruptID,
			     Xil_InterruptHandler IntrHandler, void *CallBackRef);
void XScuGic_SetPriTrigTypeByDistAddr(UINTPTR DistBaseAddress, u32 Int_Id,
                                        u8 Priority, u8 Trigger);
void XScuGic_GetPriTrigTypeByDistAddr(UINTPTR DistBaseAddress, u32 Int_Id,
					u8 *Priority, u8 *Trigger);
void XScuGic_InterruptMapFromCpuByDistAddr(UINTPTR DistBaseAddress,
							u8 Cpu_Id, u32 Int_Id);
void XScuGic_InterruptUnmapFromCpuByDistAddr(UINTPTR DistBaseAddress,
											u8 Cpu_Id, u32 Int_Id);
void XScuGic_UnmapAllInterruptsFromCpuByDistAddr(UINTPTR DistBaseAddress,
												u8 Cpu_Id);
void XScuGic_EnableIntr (UINTPTR DistBaseAddress, u32 Int_Id);
void XScuGic_DisableIntr (UINTPTR DistBaseAddress, u32 Int_Id);
#if defined(GICv3)
UINTPTR XScuGic_GetRedistBaseAddr(void);
#endif
/************************** Variable Definitions *****************************/
#ifdef __cplusplus
}
#endif

#endif            /* end of protection macro */
/** @} */
