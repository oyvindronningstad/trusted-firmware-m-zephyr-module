/*
 * Copyright (c) 2018-2020 Arm Limited. All rights reserved.
 * Copyright (c) 2020 Nordic Semiconductor ASA. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __FLASH_LAYOUT_H__
#define __FLASH_LAYOUT_H__

/* Flash layout on NRF5340 Application MCU with BL2:
 *
 * 0x0000_0000 BL2 - MCUBoot (64 KB)
 * 0x0001_0000 Primary image area (448 KB):
 *    0x0001_0000 Secure     image primary (256 KB)
 *    0x0005_0000 Non-secure image primary (192 KB)
 * 0x0008_0000 Secondary image area (448 KB):
 *    0x0008_0000 Secure     image secondary (256 KB)
 *    0x000c_0000 Non-secure image secondary (192 KB)
 * 0x000f_0000 Protected Storage Area (16 KB)
 * 0x000f_4000 Internal Trusted Storage Area (8 KB)
 * 0x000f_6000 NV counters area (4 KB)
 * 0x000f_7000 Unused
 *
 * Flash layout on NRF5340 Application MCU without BL2:
 *
 * 0x0000_0000 Primary image area (960 KB):
 *    0x0000_0000 Secure     image primary (480 KB)
 *    0x0007_8000 Non-secure image primary (480 KB)
 * 0x000f_0000 Protected Storage Area (16 KB)
 * 0x000f_4000 Internal Trusted Storage Area (8 KB)
 * 0x000f_6000 NV counters area (4 KB)
 * 0x000f_7000 Unused
 */

/* This header file is included from linker scatter file as well, where only a
 * limited C constructs are allowed. Therefore it is not possible to include
 * here the platform_base_address.h to access flash related defines. To resolve
 * this some of the values are redefined here with different names, these are
 * marked with comment.
 */

#define FLASH_MAX_PARTITION_SIZE        ((FLASH_S_PARTITION_SIZE >   \
                                          FLASH_NS_PARTITION_SIZE) ? \
                                         FLASH_S_PARTITION_SIZE :    \
                                         FLASH_NS_PARTITION_SIZE)

/* Sector size of the embedded flash hardware (erase/program) */
#define FLASH_AREA_IMAGE_SECTOR_SIZE        (0x1000)           /* 4 KB. Flash memory program/erase operations have a page granularity. */

/* FLASH size */
#define FLASH_TOTAL_SIZE                    (0x100000)    /* 1024 kB. */

/* Flash layout info for BL2 bootloader */
#define FLASH_BASE_ADDRESS                  (0x00000000)


/* Offset and size definitions of the flash partitions that are handled by the
 * bootloader. The image swapping is done between IMAGE_PRIMARY and
 * IMAGE_SECONDARY, SCRATCH is used as a temporary storage during image
 * swapping.
 */
#define FLASH_AREA_BL2_OFFSET      (0x0)

#if !defined(MCUBOOT_IMAGE_NUMBER) || (MCUBOOT_IMAGE_NUMBER == 1)
/* Secure + Non-secure image primary slot */
#define FLASH_AREA_0_ID            (1)
#define FLASH_AREA_0_OFFSET        (FLASH_AREA_BL2_OFFSET + FLASH_AREA_BL2_SIZE)
#define FLASH_AREA_0_SIZE          (FLASH_S_PARTITION_SIZE + \
                                    FLASH_NS_PARTITION_SIZE)
/* Secure + Non-secure secondary slot */
#define FLASH_AREA_2_ID            (FLASH_AREA_0_ID + 1)
#define FLASH_AREA_2_OFFSET        (FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE)
#define FLASH_AREA_2_SIZE          (FLASH_S_PARTITION_SIZE + \
                                    FLASH_NS_PARTITION_SIZE)
/* Not used, only the Non-swapping firmware upgrade operation
 * is supported on NRF5340 Application MCU.
 */
#define FLASH_AREA_SCRATCH_ID      (FLASH_AREA_2_ID + 1)
#define FLASH_AREA_SCRATCH_OFFSET  (FLASH_AREA_2_OFFSET + FLASH_AREA_2_SIZE)
#define FLASH_AREA_SCRATCH_SIZE    (0)
/* Maximum number of image sectors supported by the bootloader. */
#define MCUBOOT_MAX_IMG_SECTORS    ((FLASH_S_PARTITION_SIZE + \
                                     FLASH_NS_PARTITION_SIZE) / \
                                    FLASH_AREA_IMAGE_SECTOR_SIZE)
#elif (MCUBOOT_IMAGE_NUMBER == 2)
/* Secure image primary slot */
#define FLASH_AREA_0_ID            (1)
#define FLASH_AREA_0_OFFSET        (FLASH_AREA_BL2_OFFSET + FLASH_AREA_BL2_SIZE)
#define FLASH_AREA_0_SIZE          (FLASH_S_PARTITION_SIZE)
/* Non-secure image primary slot */
#define FLASH_AREA_1_ID            (FLASH_AREA_0_ID + 1)
#define FLASH_AREA_1_OFFSET        (FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE)
#define FLASH_AREA_1_SIZE          (FLASH_NS_PARTITION_SIZE)
/* Secure image secondary slot */
#define FLASH_AREA_2_ID            (FLASH_AREA_1_ID + 1)
#define FLASH_AREA_2_OFFSET        (FLASH_AREA_1_OFFSET + FLASH_AREA_1_SIZE)
#define FLASH_AREA_2_SIZE          (FLASH_S_PARTITION_SIZE)
/* Non-secure image secondary slot */
#define FLASH_AREA_3_ID            (FLASH_AREA_2_ID + 1)
#define FLASH_AREA_3_OFFSET        (FLASH_AREA_2_OFFSET + FLASH_AREA_2_SIZE)
#define FLASH_AREA_3_SIZE          (FLASH_NS_PARTITION_SIZE)
/* Not used, only the Non-swapping firmware upgrade operation
 * is supported on NRF5340 Application MCU.
 */
#define FLASH_AREA_SCRATCH_ID      (FLASH_AREA_3_ID + 1)
#define FLASH_AREA_SCRATCH_OFFSET  (FLASH_AREA_3_OFFSET + FLASH_AREA_3_SIZE)
#define FLASH_AREA_SCRATCH_SIZE    (0)
/* Maximum number of image sectors supported by the bootloader. */
#define MCUBOOT_MAX_IMG_SECTORS    (FLASH_MAX_PARTITION_SIZE / \
                                    FLASH_AREA_IMAGE_SECTOR_SIZE)
#else /* MCUBOOT_IMAGE_NUMBER > 2 */
#error "Only MCUBOOT_IMAGE_NUMBER 1 and 2 are supported!"
#endif /* MCUBOOT_IMAGE_NUMBER */

/* Not used, only the Non-swapping firmware upgrade operation
 * is supported on nRF5340. The maximum number of status entries
 * supported by the bootloader.
 */
#define MCUBOOT_STATUS_MAX_ENTRIES      (0)


/* Protected Storage (PS) Service definitions */
#define FLASH_PS_AREA_OFFSET            (FLASH_AREA_SCRATCH_OFFSET + \
                                         FLASH_AREA_SCRATCH_SIZE)
#define FLASH_PS_AREA_SIZE              (0x4000)   /* 16 KB */

/* Internal Trusted Storage (ITS) Service definitions */
#define FLASH_ITS_AREA_OFFSET           (FLASH_PS_AREA_OFFSET + \
                                         FLASH_PS_AREA_SIZE)
#define FLASH_ITS_AREA_SIZE             (0x2000)   /* 8 KB */

/* NV Counters definitions */
#define FLASH_NV_COUNTERS_AREA_OFFSET   (FLASH_ITS_AREA_OFFSET + \
                                         FLASH_ITS_AREA_SIZE)
#define FLASH_NV_COUNTERS_AREA_SIZE     (FLASH_AREA_IMAGE_SECTOR_SIZE)

/* Offset and size definition in flash area used by assemble.py */
#define SECURE_IMAGE_OFFSET             (0x0)
#define SECURE_IMAGE_MAX_SIZE           FLASH_S_PARTITION_SIZE

#define NON_SECURE_IMAGE_OFFSET         (SECURE_IMAGE_OFFSET + \
                                         SECURE_IMAGE_MAX_SIZE)
#define NON_SECURE_IMAGE_MAX_SIZE       FLASH_NS_PARTITION_SIZE

/* Flash device name used by BL2
 * Name is defined in flash driver file: Driver_Flash.c
 */
#define FLASH_DEV_NAME Driver_FLASH0

/* Protected Storage (PS) Service definitions
 * Note: Further documentation of these definitions can be found in the
 * TF-M PS Integration Guide.
 */
#define PS_FLASH_DEV_NAME Driver_FLASH0

/* In this target the CMSIS driver requires only the offset from the base
 * address instead of the full memory address.
 */
#define PS_FLASH_AREA_ADDR     FLASH_PS_AREA_OFFSET
/* Dedicated flash area for PS */
#define PS_FLASH_AREA_SIZE     FLASH_PS_AREA_SIZE
#define PS_SECTOR_SIZE         FLASH_AREA_IMAGE_SECTOR_SIZE
/* Number of PS_SECTOR_SIZE per block */
#define PS_SECTORS_PER_BLOCK   (0x1)
/* Specifies the smallest flash programmable unit in bytes */
#define PS_FLASH_PROGRAM_UNIT  (0x4)

/* Internal Trusted Storage (ITS) Service definitions
 * Note: Further documentation of these definitions can be found in the
 * TF-M ITS Integration Guide. The ITS should be in the internal flash, but is
 * allocated in the external flash just for development platforms that don't
 * have internal flash available.
 */
#define ITS_FLASH_DEV_NAME Driver_FLASH0

/* In this target the CMSIS driver requires only the offset from the base
 * address instead of the full memory address.
 */
#define ITS_FLASH_AREA_ADDR     FLASH_ITS_AREA_OFFSET
/* Dedicated flash area for ITS */
#define ITS_FLASH_AREA_SIZE     FLASH_ITS_AREA_SIZE
#define ITS_SECTOR_SIZE         FLASH_AREA_IMAGE_SECTOR_SIZE
/* Number of ITS_SECTOR_SIZE per block */
#define ITS_SECTORS_PER_BLOCK   (0x1)
/* Specifies the smallest flash programmable unit in bytes */
#define ITS_FLASH_PROGRAM_UNIT  (0x4)

/* NV Counters definitions */
#define TFM_NV_COUNTERS_AREA_ADDR    FLASH_NV_COUNTERS_AREA_OFFSET
#define TFM_NV_COUNTERS_AREA_SIZE    (0x18) /* 24 Bytes */
#define TFM_NV_COUNTERS_SECTOR_ADDR  FLASH_NV_COUNTERS_AREA_OFFSET
#define TFM_NV_COUNTERS_SECTOR_SIZE  FLASH_AREA_IMAGE_SECTOR_SIZE

/* Use Flash memory to store Code data */
#define FLASH_BASE_ADDRESS (0x00000000)
#define S_ROM_ALIAS_BASE   FLASH_AREA_START
#define NS_ROM_ALIAS_BASE  FLASH_AREA_START

/* Use SRAM memory to store RW data */
#define SRAM_BASE_ADDRESS (0x20000000)
#define S_RAM_ALIAS_BASE  SRAM_AREA_START
#define NS_RAM_ALIAS_BASE SRAM_AREA_START

#define TOTAL_ROM_SIZE FLASH_TOTAL_SIZE
#define TOTAL_RAM_SIZE (0x00080000)     /* 512 kB */

#if FLASH_AREA_START < FLASH_BASE_ADDRESS
        #error Flash area starts out of bounds.
#endif

#if FLASH_AREA_END > (FLASH_BASE_ADDRESS + TOTAL_ROM_SIZE)
        #error Flash area extends out of bounds.
#endif

#if FLASH_AREA_END < (FLASH_AREA_START + FLASH_NV_COUNTERS_AREA_OFFSET + \
                                         FLASH_NV_COUNTERS_AREA_SIZE)
        #error Flash area reserved for TF-M is too small
#endif

#if SRAM_AREA_START < SRAM_BASE_ADDRESS
        #error SRAM area starts out of bounds.
#endif

#if (SRAM_AREA_START + SRAM_S_PARTITON_SIZE + SRAM_NS_PARTITON_SIZE) \
     > (SRAM_BASE_ADDRESS + TOTAL_RAM_SIZE)
        #error SRAM area extends out of bounds.
#endif

#endif /* __FLASH_LAYOUT_H__ */
