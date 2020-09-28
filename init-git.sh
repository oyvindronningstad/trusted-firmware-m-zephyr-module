#!/bin/sh

TFM_GIT_SHA=upstreaming
TFM_TESTS_GIT_SHA=778942344c5fea2c9fc9c13cdc562666fa0460e2
MCUBOOT_GIT_SHA=v1.6.0
MBEDTLS_GIT_SHA=mbedtls-2.23.0
PSA_ARCH_TESTS_GIT_SHA=v20.03_API1.0

if [ ! -d trusted-firmware-m/.git ]; then
   git clone https://github.com/oyvindronningstad/trusted-firmware-m.git -b ${TFM_GIT_SHA} --bare trusted-firmware-m/.git
   cd trusted-firmware-m
   git init
   git checkout -f
   cd ..
fi

if [ ! -d mbedtls/.git ]; then
   git clone https://github.com/ARMmbed/mbedtls.git -b ${MBEDTLS_GIT_SHA} --bare mbedtls/.git
   cd mbedtls
   git init
   git checkout -f
   cd ..
fi

if [ ! -d mcuboot/.git ]; then
   git clone https://github.com/JuulLabs-OSS/mcuboot.git -b v1.4.0 -b ${MCUBOOT_GIT_SHA} --bare mcuboot/.git
   cd mcuboot
   git init
   git checkout -f
   cd ..
fi

if [ ! -d tf-m-tests/.git ]; then
   git clone https://git.trustedfirmware.org/TF-M/tf-m-tests.git -b ${TFM_TESTS_GIT_SHA} --bare tf-m-tests/.git
   cd tf-m-tests
   git init
   git checkout -f
   cd ..
fi

if [ ! -d psa-arch-tests/.git ]; then
   git clone https://github.com/ARM-software/psa-arch-tests.git -b ${PSA_ARCH_TESTS_GIT_SHA} --bare psa-arch-tests/.git
   cd psa-arch-tests
   git init
   git checkout -f
   cd ..
fi
