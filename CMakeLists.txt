# Copyright (c) 2019, 2020 Linaro
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

# Adds trusted-firmware-m as an external project, and provides output values
# that are required to link against the secure TF-M binary.
#
# When called from a Zephyr module, the following input values can be provided
# to configure the TF-M build:
#
# BINARY_DIR: The location where the build outputs will be written
# BOARD: The string identifying the board target for TF-M (AN521, etc.)
# CFGFILE: The TF-M config file to use, without the .cmake extension
# IPC: Build TFM IPC library. This library allows a non-secure application to
#      interface to secure domain using IPC.
# BL2: Boolean if build uses MCUboot.
#
# The following output values can also be used:
#
# OUT_VENEERS_FILE: The path and filename of the veneer library to link against,
#                   which identifies where the veneer functions are in memory.
#                   Should be added via "target_link_libraries"
#
# Example usage:
#
# trusted_firmware_build(BINARY_DIR ${CMAKE_BINARY_DIR}/tfm
#                        BOARD ${TFMBOARD}
#                        CFGFILE "ConfigRegressionIPC"
#                        BL2 True)
function(trusted_firmware_build)
  set(options IPC)
  set(oneValueArgs BINARY_DIR BOARD BL2 ISOLATION_LEVEL)
  cmake_parse_arguments(TFM "${options}" "${oneValueArgs}" "" ${ARGN})

  if(DEFINED TFM_BL2)
    set(TFM_BL2_ARG "-DBL2=${TFM_BL2}")
  endif()
  if(DEFINED TFM_ISOLATION_LEVEL)
    set(TFM_ISOLATION_LEVEL_ARG -DTFM_ISOLATION_LEVEL=${TFM_ISOLATION_LEVEL})
  endif()
  if(DEFINED TFM_IPC)
    set(TFM_IPC_ARG -DTFM_PSA_API=ON)
  endif()

  set(VENEERS_FILE ${TFM_BINARY_DIR}/secure_fw/s_veneers.o)
  set(PSA_API_NS_PATH ${TFM_BINARY_DIR}/interface/libpsa_api_ns.a)

  # Get the toolchain variant
  # TODO: Add support for cross-compile toolchain variant
  # TODO: Enforce GCC version check against TF-M compiler requirements
  if(${ZEPHYR_TOOLCHAIN_VARIANT} STREQUAL "zephyr")
    set(TFM_TOOLCHAIN "GNUARM")
    set(TFM_TOOLCHAIN_PREFIX "arm-zephyr-eabi")
    set(TFM_TOOLCHAIN_PATH ${ZEPHYR_SDK_INSTALL_DIR}/arm-zephyr-eabi)
  elseif(${ZEPHYR_TOOLCHAIN_VARIANT} STREQUAL "gnuarmemb")
    set(TFM_TOOLCHAIN "GNUARM")
    set(TFM_TOOLCHAIN_PREFIX "arm-none-eabi")
    set(TFM_TOOLCHAIN_PATH ${GNUARMEMB_TOOLCHAIN_PATH})
  else()
    message(FATAL_ERROR "Unsupported ZEPHYR_TOOLCHAIN_VARIANT: ${ZEPHYR_TOOLCHAIN_VARIANT}")
  endif()

  include(ExternalProject)

  message("TFM_BINARY_DIR: ${TFM_BINARY_DIR}")

  ExternalProject_Add(
    tfm
    SOURCE_DIR ${ZEPHYR_TFM_MODULE_DIR}/trusted-firmware-m
    BINARY_DIR ${TFM_BINARY_DIR}
    CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${ZEPHYR_TFM_MODULE_DIR}/trusted-firmware-m/toolchain_GNUARM.cmake
               -DMCUBOOT_PATH=${ZEPHYR_TFM_MODULE_DIR}/../../../bootloader/mcuboot
               -DPSA_ARCH_TESTS_PATH=${ZEPHYR_TFM_MODULE_DIR}/psa-arch-tests
               -DTFM_TEST_REPO_PATH=${ZEPHYR_TFM_MODULE_DIR}/tf-m-tests
               -DMBEDCRYPTO_PATH=${ZEPHYR_TFM_MODULE_DIR}/mbedtls
               -DCMAKE_BUILD_TYPE=RelWithDebInfo
               -DTFM_PLATFORM=${TFM_BOARD}
               ${TFM_BL2_ARG}
               -DGNUARM_PREFIX=${TFM_TOOLCHAIN_PREFIX}
               -DGNUARM_PATH=${TFM_TOOLCHAIN_PATH}
               -DTEST_S=ON
               -DTEST_NS=ON
               ${TFM_IPC_ARG}
               ${TFM_ISOLATION_LEVEL_ARG}
    BUILD_ALWAYS True
    USES_TERMINAL_BUILD True
    BUILD_BYPRODUCTS ${VENEERS_FILE} ${PSA_API_NS_PATH}
  )

  add_library(tfm_ipc_psa_api STATIC IMPORTED)
  set_target_properties(tfm_ipc_psa_api PROPERTIES
    IMPORTED_LOCATION ${PSA_API_NS_PATH}
  )
  add_library(veneer_lib STATIC IMPORTED)
  set_target_properties(veneer_lib PROPERTIES
    IMPORTED_LOCATION ${VENEERS_FILE}
  )
  add_dependencies(tfm_ipc_psa_api tfm)
  file(MAKE_DIRECTORY ${TFM_BINARY_DIR}/generated/interface/include)
  target_include_directories(tfm_ipc_psa_api INTERFACE
    ${ZEPHYR_TFM_MODULE_DIR}/trusted-firmware-m/interface/include
    ${TFM_BINARY_DIR}/generated/interface/include
  )
  target_link_libraries(tfm_ipc_psa_api INTERFACE zephyr_interface veneer_lib)

endfunction()

# Gets a list of commands to be performed after a successful Zephyr build that
# involves trusted-firmware-m.
function(trusted_firmware_get_post_build_commands out_commands)

  if(NOT CONFIG_TFM_BL2_FALSE)

    # Set default image versions if not defined elsewhere
    if(NOT DEFINED TFM_IMAGE_VERSION_S)
      set(TFM_IMAGE_VERSION_S 0.0.0+0)
    endif()
    if(NOT DEFINED TFM_IMAGE_VERSION_NS)
      set(TFM_IMAGE_VERSION_NS 0.0.0+0)
    endif()

    set(TFM_MCUBOOT_DIR "${ZEPHYR_TFM_MODULE_DIR}/trusted-firmware-m/bl2/ext/mcuboot")
    set(PREPROCESSED_FILE_S "${CMAKE_BINARY_DIR}/tfm/bl2/ext/mcuboot/CMakeFiles/signing_layout_s.dir/signing_layout_s.o")
    set(PREPROCESSED_FILE_NS "${CMAKE_BINARY_DIR}/tfm/bl2/ext/mcuboot/CMakeFiles/signing_layout_ns.dir/signing_layout_ns.o")

    if(NOT DEFINED TFM_PUBLIC_KEY_FORMAT)
      set(TFM_PUBLIC_KEY_FORMAT "full")
    endif()

    set (ADD_SECURITY_COUNTER_NS -s auto)

    set(${out_commands}

      COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/tfm/bin/tfm_s_signed.bin
        ${CMAKE_BINARY_DIR}/tfm_s_signed.bin

      # Sign non-secure binary image with public key
      COMMAND ${PYTHON_EXECUTABLE} ${TFM_MCUBOOT_DIR}/scripts/wrapper/wrapper.py
        --layout ${PREPROCESSED_FILE_NS}
        -k ${CONFIG_TFM_KEY_FILE_NS}
        --public-key-format ${TFM_PUBLIC_KEY_FORMAT}
        --align 1
        -v ${TFM_IMAGE_VERSION_NS}
        ${ADD_S_IMAGE_MIN_VER}
        ${ADD_SECURITY_COUNTER_NS}
        -H 0x400
        ${CMAKE_BINARY_DIR}/zephyr/${KERNEL_BIN_NAME}
        ${CMAKE_BINARY_DIR}/zephyr_ns_signed.bin

      # Create concatenated binary image from the two independently signed binary files
      COMMAND ${PYTHON_EXECUTABLE} ${TFM_MCUBOOT_DIR}/scripts/assemble.py
        --layout ${PREPROCESSED_FILE_S}
        -s ${CMAKE_BINARY_DIR}/tfm_s_signed.bin
        -n ${CMAKE_BINARY_DIR}/zephyr_ns_signed.bin
        -o ${CMAKE_BINARY_DIR}/tfm_sign.bin

      # Copy mcuboot.bin
      COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/tfm/bin/bl2.bin
        ${CMAKE_BINARY_DIR}/mcuboot.bin
      COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/tfm/bin/bl2.hex
        ${CMAKE_BINARY_DIR}/mcuboot.hex

      PARENT_SCOPE
    )

  endif()

endfunction()
