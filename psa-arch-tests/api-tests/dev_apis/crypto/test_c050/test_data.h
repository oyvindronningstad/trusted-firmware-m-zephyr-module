/** @file
 * Copyright (c) 2019-2020, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#include "val_crypto.h"

typedef struct {
    char                    test_desc[75];
    psa_key_type_t          key_type;
    psa_key_id_t            key_id;
    uint8_t                 key_data[34];
    uint32_t                key_length;
    size_t                  attr_bits;
    psa_key_usage_t         usage;
    psa_algorithm_t         key_alg;
    psa_key_lifetime_t      key_lifetime;
    uint32_t                expected_bit_length;
    uint32_t                expected_key_length;
    psa_status_t            expected_status;
} test_data;

static const uint8_t rsa_384_keypair[1];
static const uint8_t rsa_384_keydata[1];

static const uint8_t rsa_256_keypair[] = {
 0x30, 0x82, 0x04, 0xA5, 0x02, 0x01, 0x00, 0x02, 0x82, 0x01, 0x01, 0x00, 0xC0,
 0x95, 0x08, 0xE1, 0x57, 0x41, 0xF2, 0x71, 0x6D, 0xB7, 0xD2, 0x45, 0x41, 0x27,
 0x01, 0x65, 0xC6, 0x45, 0xAE, 0xF2, 0xBC, 0x24, 0x30, 0xB8, 0x95, 0xCE, 0x2F,
 0x4E, 0xD6, 0xF6, 0x1C, 0x88, 0xBC, 0x7C, 0x9F, 0xFB, 0xA8, 0x67, 0x7F, 0xFE,
 0x5C, 0x9C, 0x51, 0x75, 0xF7, 0x8A, 0xCA, 0x07, 0xE7, 0x35, 0x2F, 0x8F, 0xE1,
 0xBD, 0x7B, 0xC0, 0x2F, 0x7C, 0xAB, 0x64, 0xA8, 0x17, 0xFC, 0xCA, 0x5D, 0x7B,
 0xBA, 0xE0, 0x21, 0xE5, 0x72, 0x2E, 0x6F, 0x2E, 0x86, 0xD8, 0x95, 0x73, 0xDA,
 0xAC, 0x1B, 0x53, 0xB9, 0x5F, 0x3F, 0xD7, 0x19, 0x0D, 0x25, 0x4F, 0xE1, 0x63,
 0x63, 0x51, 0x8B, 0x0B, 0x64, 0x3F, 0xAD, 0x43, 0xB8, 0xA5, 0x1C, 0x5C, 0x34,
 0xB3, 0xAE, 0x00, 0xA0, 0x63, 0xC5, 0xF6, 0x7F, 0x0B, 0x59, 0x68, 0x78, 0x73,
 0xA6, 0x8C, 0x18, 0xA9, 0x02, 0x6D, 0xAF, 0xC3, 0x19, 0x01, 0x2E, 0xB8, 0x10,
 0xE3, 0xC6, 0xCC, 0x40, 0xB4, 0x69, 0xA3, 0x46, 0x33, 0x69, 0x87, 0x6E, 0xC4,
 0xBB, 0x17, 0xA6, 0xF3, 0xE8, 0xDD, 0xAD, 0x73, 0xBC, 0x7B, 0x2F, 0x21, 0xB5,
 0xFD, 0x66, 0x51, 0x0C, 0xBD, 0x54, 0xB3, 0xE1, 0x6D, 0x5F, 0x1C, 0xBC, 0x23,
 0x73, 0xD1, 0x09, 0x03, 0x89, 0x14, 0xD2, 0x10, 0xB9, 0x64, 0xC3, 0x2A, 0xD0,
 0xA1, 0x96, 0x4A, 0xBC, 0xE1, 0xD4, 0x1A, 0x5B, 0xC7, 0xA0, 0xC0, 0xC1, 0x63,
 0x78, 0x0F, 0x44, 0x37, 0x30, 0x32, 0x96, 0x80, 0x32, 0x23, 0x95, 0xA1, 0x77,
 0xBA, 0x13, 0xD2, 0x97, 0x73, 0xE2, 0x5D, 0x25, 0xC9, 0x6A, 0x0D, 0xC3, 0x39,
 0x60, 0xA4, 0xB4, 0xB0, 0x69, 0x42, 0x42, 0x09, 0xE9, 0xD8, 0x08, 0xBC, 0x33,
 0x20, 0xB3, 0x58, 0x22, 0xA7, 0xAA, 0xEB, 0xC4, 0xE1, 0xE6, 0x61, 0x83, 0xC5,
 0xD2, 0x96, 0xDF, 0xD9, 0xD0, 0x4F, 0xAD, 0xD7, 0x02, 0x03, 0x01, 0x00, 0x01,
 0x02, 0x82, 0x01, 0x01, 0x00, 0x9A, 0xD0, 0x34, 0x0F, 0x52, 0x62, 0x05, 0x50,
 0x01, 0xEF, 0x9F, 0xED, 0x64, 0x6E, 0xC2, 0xC4, 0xDA, 0x1A, 0xF2, 0x84, 0xD7,
 0x92, 0x10, 0x48, 0x92, 0xC4, 0xE9, 0x6A, 0xEB, 0x8B, 0x75, 0x6C, 0xC6, 0x79,
 0x38, 0xF2, 0xC9, 0x72, 0x4A, 0x86, 0x64, 0x54, 0x95, 0x77, 0xCB, 0xC3, 0x9A,
 0x9D, 0xB7, 0xD4, 0x1D, 0xA4, 0x00, 0xC8, 0x9E, 0x4E, 0xE4, 0xDD, 0xC7, 0xBA,
 0x67, 0x16, 0xC1, 0x74, 0xBC, 0xA9, 0xD6, 0x94, 0x8F, 0x2B, 0x30, 0x1A, 0xFB,
 0xED, 0xDF, 0x21, 0x05, 0x23, 0xD9, 0x4A, 0x39, 0xBD, 0x98, 0x6B, 0x65, 0x9A,
 0xB8, 0xDC, 0xC4, 0x7D, 0xEE, 0xA6, 0x43, 0x15, 0x2E, 0x3D, 0xBE, 0x1D, 0x22,
 0x60, 0x2A, 0x73, 0x30, 0xD5, 0x3E, 0xD8, 0xA2, 0xAC, 0x86, 0x43, 0x2E, 0xC4,
 0xF5, 0x64, 0x5E, 0x3F, 0x89, 0x75, 0x0F, 0x11, 0xD8, 0x51, 0x25, 0x4E, 0x9F,
 0xD8, 0xAA, 0xA3, 0xCE, 0x60, 0xB3, 0xE2, 0x8A, 0xD9, 0x7E, 0x1B, 0xF0, 0x64,
 0xCA, 0x9A, 0x5B, 0x05, 0x0B, 0x5B, 0xAA, 0xCB, 0xE5, 0xE3, 0x3F, 0x6E, 0x32,
 0x22, 0x05, 0xF3, 0xD0, 0xFA, 0xEF, 0x74, 0x52, 0x81, 0xE2, 0x5F, 0x74, 0xD3,
 0xBD, 0xFF, 0x31, 0x83, 0x45, 0x75, 0xFA, 0x63, 0x7A, 0x97, 0x2E, 0xD6, 0xB6,
 0x19, 0xC6, 0x92, 0x26, 0xE4, 0x28, 0x06, 0x50, 0x50, 0x0E, 0x78, 0x2E, 0xA9,
 0x78, 0x0D, 0x14, 0x97, 0xB4, 0x12, 0xD8, 0x31, 0x40, 0xAB, 0xA1, 0x01, 0x41,
 0xC2, 0x30, 0xF8, 0x07, 0x5F, 0x16, 0xE4, 0x61, 0x77, 0xD2, 0x60, 0xF2, 0x9F,
 0x8D, 0xE8, 0xF4, 0xBA, 0xEB, 0x63, 0xDE, 0x2A, 0x97, 0x81, 0xEF, 0x4C, 0x6C,
 0xE6, 0x55, 0x34, 0x51, 0x2B, 0x28, 0x34, 0xF4, 0x53, 0x1C, 0xC4, 0x58, 0x0A,
 0x3F, 0xBB, 0xAF, 0xB5, 0xF7, 0x4A, 0x85, 0x43, 0x2D, 0x3C, 0xF1, 0x58, 0x58,
 0x81, 0x02, 0x81, 0x81, 0x00, 0xF2, 0x2C, 0x54, 0x76, 0x39, 0x23, 0x63, 0xC9,
 0x10, 0x32, 0xB7, 0x93, 0xAD, 0xAF, 0xBE, 0x19, 0x75, 0x96, 0x81, 0x64, 0xE6,
 0xB5, 0xB8, 0x89, 0x42, 0x41, 0xD1, 0x6D, 0xD0, 0x1C, 0x1B, 0xF8, 0x1B, 0xAC,
 0x69, 0xCB, 0x36, 0x3C, 0x64, 0x7D, 0xDC, 0xF4, 0x19, 0xB8, 0xC3, 0x60, 0xB1,
 0x57, 0x48, 0x5F, 0x52, 0x4F, 0x59, 0x3A, 0x55, 0x7F, 0x32, 0xC0, 0x19, 0x43,
 0x50, 0x3F, 0xAE, 0xCE, 0x6F, 0x17, 0xF3, 0x0E, 0x9F, 0x40, 0xCA, 0x4E, 0xAD,
 0x15, 0x3B, 0xC9, 0x79, 0xE9, 0xC0, 0x59, 0x38, 0x73, 0x70, 0x9C, 0x0A, 0x7C,
 0xC9, 0x3A, 0x48, 0x32, 0xA7, 0xD8, 0x49, 0x75, 0x0A, 0x85, 0xC2, 0xC2, 0xFD,
 0x15, 0x73, 0xDA, 0x99, 0x09, 0x2A, 0x69, 0x9A, 0x9F, 0x0A, 0x71, 0xBF, 0xB0,
 0x04, 0xA6, 0x8C, 0x7A, 0x5A, 0x6F, 0x48, 0x5A, 0x54, 0x3B, 0xC6, 0xB1, 0x53,
 0x17, 0xDF, 0xE7, 0x02, 0x81, 0x81, 0x00, 0xCB, 0x93, 0xDE, 0x77, 0x15, 0x5D,
 0xB7, 0x5C, 0x5C, 0x7C, 0xD8, 0x90, 0xA9, 0x98, 0x2D, 0xD6, 0x69, 0x0E, 0x63,
 0xB3, 0xA3, 0xDC, 0xA6, 0xCC, 0x8B, 0x6A, 0xA4, 0xA2, 0x12, 0x8C, 0x8E, 0x7B,
 0x48, 0x2C, 0xB2, 0x4B, 0x37, 0xDC, 0x06, 0x18, 0x7D, 0xEA, 0xFE, 0x76, 0xA1,
 0xD4, 0xA1, 0xE9, 0x3F, 0x0D, 0xCD, 0x1B, 0x5F, 0xAF, 0x5F, 0x9E, 0x96, 0x5B,
 0x5B, 0x0F, 0xA1, 0x7C, 0xAF, 0xB3, 0x9B, 0x90, 0xDB, 0x57, 0x73, 0x3A, 0xED,
 0xB0, 0x23, 0x44, 0xAE, 0x41, 0x4F, 0x1F, 0x07, 0x42, 0x13, 0x23, 0x4C, 0xCB,
 0xFA, 0xF4, 0x14, 0xA4, 0xD5, 0xF7, 0x9E, 0x36, 0x7C, 0x5B, 0x9F, 0xA8, 0x3C,
 0xC1, 0x85, 0x5F, 0x74, 0xD2, 0x39, 0x2D, 0xFF, 0xD0, 0x84, 0xDF, 0xFB, 0xB3,
 0x20, 0x7A, 0x2E, 0x9B, 0x17, 0xAE, 0xE6, 0xBA, 0x0B, 0xAE, 0x5F, 0x53, 0xA4,
 0x52, 0xED, 0x1B, 0xC4, 0x91, 0x02, 0x81, 0x81, 0x00, 0xEC, 0x98, 0xDA, 0xBB,
 0xD5, 0xFE, 0xF9, 0x52, 0x4A, 0x7D, 0x02, 0x55, 0x49, 0x6F, 0x55, 0x6E, 0x52,
 0x2F, 0x84, 0xA3, 0x2B, 0xB3, 0x86, 0x62, 0xB3, 0x54, 0xD2, 0x63, 0x52, 0xDA,
 0xE3, 0x88, 0x76, 0xA0, 0xEF, 0x8B, 0x15, 0xA5, 0xD3, 0x18, 0x14, 0x72, 0x77,
 0x5E, 0xC7, 0xA3, 0x04, 0x1F, 0x9E, 0x19, 0x62, 0xB5, 0x1B, 0x1B, 0x9E, 0xC3,
 0xF2, 0xB5, 0x32, 0xF9, 0x4C, 0xC1, 0xAA, 0xEB, 0x0C, 0x26, 0x7D, 0xD4, 0x5F,
 0x4A, 0x51, 0x5C, 0xA4, 0x45, 0x06, 0x70, 0x44, 0xA7, 0x56, 0xC0, 0xD4, 0x22,
 0x14, 0x76, 0x9E, 0xD8, 0x63, 0x50, 0x89, 0x90, 0xD3, 0xE2, 0xBF, 0x81, 0x95,
 0x92, 0x31, 0x41, 0x87, 0x39, 0x1A, 0x43, 0x0B, 0x18, 0xA5, 0x53, 0x1F, 0x39,
 0x1A, 0x5F, 0x1F, 0x43, 0xBC, 0x87, 0x6A, 0xDF, 0x6E, 0xD3, 0x22, 0x00, 0xFE,
 0x22, 0x98, 0x70, 0x4E, 0x1A, 0x19, 0x29, 0x02, 0x81, 0x81, 0x00, 0x8A, 0x41,
 0x56, 0x28, 0x51, 0x9E, 0x5F, 0xD4, 0x9E, 0x0B, 0x3B, 0x98, 0xA3, 0x54, 0xF2,
 0x6C, 0x56, 0xD4, 0xAA, 0xE9, 0x69, 0x33, 0x85, 0x24, 0x0C, 0xDA, 0xD4, 0x0C,
 0x2D, 0xC4, 0xBF, 0x4F, 0x02, 0x69, 0x38, 0x7C, 0xD4, 0xE6, 0xDC, 0x4C, 0xED,
 0xD7, 0x16, 0x11, 0xC3, 0x3E, 0x00, 0xE7, 0xC3, 0x26, 0xC0, 0x51, 0x02, 0xDE,
 0xBB, 0x75, 0x9C, 0x6F, 0x56, 0x9C, 0x7A, 0xF3, 0x8E, 0xEF, 0xCF, 0x8A, 0xC5,
 0x2B, 0xD2, 0xDA, 0x06, 0x6A, 0x44, 0xC9, 0x73, 0xFE, 0x6E, 0x99, 0x87, 0xF8,
 0x5B, 0xBE, 0xF1, 0x7C, 0xE6, 0x65, 0xB5, 0x4F, 0x6C, 0xF0, 0xC9, 0xC5, 0xFF,
 0x16, 0xCA, 0x8B, 0x1B, 0x17, 0xE2, 0x58, 0x3D, 0xA2, 0x37, 0xAB, 0x01, 0xBC,
 0xBF, 0x40, 0xCE, 0x53, 0x8C, 0x8E, 0xED, 0xEF, 0xEE, 0x59, 0x9D, 0xE0, 0x63,
 0xE6, 0x7C, 0x5E, 0xF5, 0x8E, 0x4B, 0xF1, 0x3B, 0xC1, 0x02, 0x81, 0x80, 0x4D,
 0x45, 0xF9, 0x40, 0x8C, 0xC5, 0x5B, 0xF4, 0x2A, 0x1A, 0x8A, 0xB4, 0xF2, 0x1C,
 0xAC, 0x6B, 0xE9, 0x0C, 0x56, 0x36, 0xB7, 0x4E, 0x72, 0x96, 0xD5, 0xE5, 0x8A,
 0xD2, 0xE2, 0xFF, 0xF1, 0xF1, 0x18, 0x13, 0x3D, 0x86, 0x09, 0xB8, 0xD8, 0x76,
 0xA7, 0xC9, 0x1C, 0x71, 0x52, 0x94, 0x30, 0x43, 0xE0, 0xF1, 0x78, 0x74, 0xFD,
 0x61, 0x1B, 0x4C, 0x09, 0xCC, 0xE6, 0x68, 0x2A, 0x71, 0xAD, 0x1C, 0xDF, 0x43,
 0xBC, 0x56, 0xDB, 0xA5, 0xA4, 0xBE, 0x35, 0x70, 0xA4, 0x5E, 0xCF, 0x4F, 0xFC,
 0x00, 0x55, 0x99, 0x3A, 0x3D, 0x23, 0xCF, 0x67, 0x5A, 0xF5, 0x22, 0xF8, 0xB5,
 0x29, 0xD0, 0x44, 0x11, 0xEB, 0x35, 0x2E, 0x46, 0xBE, 0xFD, 0x8E, 0x18, 0xB2,
 0x5F, 0xA8, 0xBF, 0x19, 0x32, 0xA1, 0xF5, 0xDC, 0x03, 0xE6, 0x7C, 0x9A, 0x1F,
 0x0C, 0x7C, 0xA9, 0xB0, 0x0E, 0x21, 0x37, 0x3B, 0xF1, 0xB0};

static const uint8_t rsa_256_keydata[] = {
 0x30, 0x82, 0x01, 0x0A,
 0x02, 0x82, 0x01, 0x01, 0x00, 0xDB, 0x1C, 0x7F, 0x2E, 0x0B, 0xCD, 0xBF, 0xCE, 0xD1,
 0x75, 0x10, 0xA0, 0xA2, 0xB8, 0xCE, 0x7D, 0xAA, 0xE2, 0x05, 0xE0, 0x7A, 0xD8, 0x44,
 0x63, 0x8F, 0xB5, 0xBD, 0xC0, 0xB0, 0x19, 0xB9, 0x37, 0xB8, 0x19, 0x4A, 0x0E, 0xF1,
 0x5D, 0x74, 0x80, 0x67, 0x46, 0x87, 0x06, 0xDE, 0x5B, 0x7F, 0x06, 0x03, 0xBD, 0xC1,
 0x8D, 0x5E, 0x07, 0x15, 0xD4, 0x5B, 0xF4, 0xDC, 0xE5, 0xCF, 0x3D, 0xF9, 0xC1, 0x11,
 0x2C, 0xAE, 0x6A, 0xB9, 0x8A, 0xBD, 0x1D, 0x67, 0x66, 0x17, 0xEA, 0x4E, 0xBD, 0xDB,
 0x15, 0x9A, 0x82, 0x87, 0xE4, 0xF0, 0x78, 0xC3, 0xA3, 0x85, 0x87, 0xB0, 0xFD, 0x9F,
 0xA9, 0x99, 0x5F, 0xE3, 0x33, 0xEC, 0xCC, 0xEA, 0x0B, 0xB5, 0x61, 0x5E, 0xF1, 0x49,
 0x7E, 0x3F, 0xA3, 0x2D, 0xEA, 0x01, 0x0C, 0xCC, 0x42, 0x9A, 0x76, 0x9B, 0xC4, 0xD0,
 0x37, 0xD3, 0xB1, 0x17, 0x01, 0x61, 0x01, 0x16, 0x59, 0x7E, 0x1C, 0x17, 0xC3, 0x53,
 0xFD, 0xD1, 0x72, 0xCB, 0x4C, 0x60, 0x15, 0xDA, 0x7D, 0xE2, 0xEA, 0xAD, 0x50, 0xEF,
 0x8E, 0xE2, 0x8B, 0xD4, 0x6A, 0x77, 0x55, 0xD6, 0x70, 0xD9, 0x6B, 0xBB, 0xF1, 0xEE,
 0x39, 0x04, 0x38, 0xA3, 0xBD, 0xE2, 0xD1, 0xE0, 0x66, 0x6B, 0xE2, 0x9C, 0x47, 0x99,
 0xE9, 0x28, 0xE6, 0xB6, 0xFC, 0x2E, 0xCA, 0x67, 0x43, 0x84, 0xE8, 0xD5, 0x83, 0xD6,
 0x9D, 0x98, 0x6B, 0x01, 0x3E, 0x81, 0xDC, 0x3C, 0x7A, 0xCA, 0xF9, 0xF3, 0x9C, 0xF7,
 0xD6, 0x28, 0x1B, 0x27, 0x78, 0x7C, 0xC3, 0xD0, 0xD5, 0x63, 0xA7, 0x81, 0x34, 0x89,
 0xAD, 0x25, 0x6A, 0xBD, 0xF2, 0xEA, 0xED, 0xFA, 0x57, 0xFC, 0xE5, 0x34, 0xC6, 0xC1,
 0x0F, 0x71, 0x2D, 0xD2, 0x08, 0x10, 0x1B, 0xAD, 0x44, 0x41, 0xE0, 0xFE, 0x79, 0xA0,
 0x63, 0x93, 0x8A, 0xB1, 0x5D, 0xE9, 0xB0, 0xEE, 0x6F, 0x02, 0x03, 0x01, 0x00, 0x01};

static const uint8_t ec_keydata[] = {
 0x04, 0xde, 0xa5, 0xe4, 0x5d, 0x0e, 0xa3, 0x7f, 0xc5, 0x66, 0x23, 0x2a, 0x50, 0x8f,
 0x4a, 0xd2, 0x0e, 0xa1, 0x3d, 0x47, 0xe4, 0xbf, 0x5f, 0xa4, 0xd5, 0x4a, 0x57, 0xa0,
 0xba, 0x01, 0x20, 0x42, 0x08, 0x70, 0x97, 0x49, 0x6e, 0xfc, 0x58, 0x3f, 0xed, 0x8b,
 0x24, 0xa5, 0xb9, 0xbe, 0x9a, 0x51, 0xde, 0x06, 0x3f, 0x5a, 0x00, 0xa8, 0xb6, 0x98,
 0xa1, 0x6f, 0xd7, 0xf2, 0x9b, 0x54, 0x85, 0xf3, 0x20};

static const uint8_t ec_keypair[] = {
 0x68, 0x49, 0xf9, 0x7d, 0x10, 0x66, 0xf6, 0x99, 0x77, 0x59, 0x63, 0x7c, 0x7e, 0x38,
 0x99, 0x46, 0x4c, 0xee, 0x3e, 0xc7, 0xac, 0x97, 0x06, 0x53, 0xa0, 0xbe, 0x07, 0x42};

static test_data check1[] = {
#ifdef ARCH_TEST_CIPHER_MODE_CTR
#ifdef ARCH_TEST_AES_128
{"Test psa_open_key 16 Byte AES\n", PSA_KEY_TYPE_AES, 0x12,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9, 0x77},
 AES_16B_KEY_SIZE, 0, PSA_KEY_USAGE_EXPORT, PSA_ALG_CTR, PSA_KEY_LIFETIME_PERSISTENT,
 BYTES_TO_BITS(AES_16B_KEY_SIZE), AES_16B_KEY_SIZE, PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_AES_192
{"Test psa_open_key 24 Byte AES\n", PSA_KEY_TYPE_AES, 0x34,
{0x24, 0x13, 0x61, 0x47, 0x61, 0xB8, 0xC8, 0xF0, 0xDF, 0xAB, 0x5A, 0x0E, 0x87,
 0x40, 0xAC, 0xA3, 0x90, 0x77, 0x83, 0x52, 0x31, 0x74, 0xF9, 0x05},
 AES_24B_KEY_SIZE, BYTES_TO_BITS(AES_24B_KEY_SIZE), PSA_KEY_USAGE_EXPORT, PSA_ALG_CTR,
 PSA_KEY_LIFETIME_PERSISTENT,
 BYTES_TO_BITS(AES_24B_KEY_SIZE), AES_24B_KEY_SIZE, PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_AES_256
{"Test psa_open_key 32 Byte AES\n", PSA_KEY_TYPE_AES, 0x56,
{0xEA, 0xD5, 0xE6, 0xC8, 0x51, 0xF9, 0xEC, 0xBB, 0x9B, 0x57, 0x7C, 0xED, 0xD2,
 0x4B, 0x82, 0x84, 0x9F, 0x9F, 0xE6, 0x73, 0x21, 0x3D, 0x1A, 0x05, 0xC9, 0xED,
 0xDF, 0x25, 0x17, 0x68, 0x86, 0xAE},
 AES_32B_KEY_SIZE, 0, PSA_KEY_USAGE_EXPORT, PSA_ALG_CTR, PSA_KEY_LIFETIME_PERSISTENT,
 BYTES_TO_BITS(AES_32B_KEY_SIZE), AES_32B_KEY_SIZE, PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_RSA_PKCS1V15_SIGN_RAW
#ifdef ARCH_TEST_RSA_2048
{"Test psa_open_key 2048 RSA public key\n", PSA_KEY_TYPE_RSA_PUBLIC_KEY, 0x78,
 {0},
 270, 0, PSA_KEY_USAGE_EXPORT, PSA_ALG_RSA_PKCS1V15_SIGN_RAW, PSA_KEY_LIFETIME_PERSISTENT,
 2048, 270, PSA_SUCCESS
},

{"Test psa_open_key with RSA 2048 keypair\n", PSA_KEY_TYPE_RSA_KEY_PAIR, 0x89,
 {0},
 1193, 0, PSA_KEY_USAGE_EXPORT, PSA_ALG_RSA_PKCS1V15_SIGN_RAW, PSA_KEY_LIFETIME_PERSISTENT,
 2048, 1193, PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_CIPHER_MODE_CTR
#ifdef ARCH_TEST_DES_1KEY
{"Test psa_open_key with DES 64 bit key\n", PSA_KEY_TYPE_DES, 0x90,
 {0x70, 0x24, 0x55, 0x0C, 0x14, 0x9D, 0xED, 0x29},
 DES_8B_KEY_SIZE, BYTES_TO_BITS(DES_8B_KEY_SIZE), PSA_KEY_USAGE_EXPORT, PSA_ALG_CTR,
 PSA_KEY_LIFETIME_PERSISTENT,
 BYTES_TO_BITS(DES_8B_KEY_SIZE), DES_8B_KEY_SIZE, PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_2KEY
{"Test psa_open_key with Triple DES 2-Key\n", PSA_KEY_TYPE_DES, 0x123,
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
 DES3_2B_KEY_SIZE, 0, PSA_KEY_USAGE_EXPORT, PSA_ALG_CTR, PSA_KEY_LIFETIME_PERSISTENT,
 BYTES_TO_BITS(DES3_2B_KEY_SIZE), DES3_2B_KEY_SIZE, PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_3KEY
{"Test psa_open_key with Triple DES 3-Key\n", PSA_KEY_TYPE_DES, 0x456,
{0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
 0xF1, 0xE0, 0xD3, 0xC2, 0xB5, 0xA4, 0x97, 0x86,
 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10},
 DES3_3B_KEY_SIZE, 0, PSA_KEY_USAGE_EXPORT, PSA_ALG_CTR, PSA_KEY_LIFETIME_PERSISTENT,
 BYTES_TO_BITS(DES3_3B_KEY_SIZE), DES3_3B_KEY_SIZE, PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_ECDSA
#ifdef ARCH_TEST_ECC_CURVE_SECP256R1
{"Test psa_open_key with EC Public key\n",
 PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_CURVE_SECP256R1), 0x789,
 {0},
 65, 0, PSA_KEY_USAGE_EXPORT, PSA_ALG_ECDSA_ANY, PSA_KEY_LIFETIME_PERSISTENT,
 256, 65, PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_ECC_CURVE_SECP224R1
{"Test psa_open_key with EC keypair\n",
 PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_CURVE_SECP224R1), 0x1234,
 {0},
 28, 0, PSA_KEY_USAGE_EXPORT, PSA_ALG_ECDSA_ANY, PSA_KEY_LIFETIME_PERSISTENT,
 224, 28, PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_CIPHER_MODE_CTR
#ifdef ARCH_TEST_AES
{"Test psa_open_key with volatile key\n", PSA_KEY_TYPE_AES,
 0x5678,
{0x24, 0x13, 0x61, 0x47, 0x61, 0xB8, 0xC8, 0xF0, 0xDF, 0xAB, 0x5A, 0x0E, 0x87,
 0x40, 0xAC, 0xA3, 0x90, 0x77, 0x83, 0x52, 0x31, 0x74, 0xF9, 0x05, 0xC9, 0xED,
 0xDF, 0x25, 0x17, 0x68, 0x86, 0xAE},
 AES_32B_KEY_SIZE, 0, PSA_KEY_USAGE_EXPORT, PSA_ALG_CTR, PSA_KEY_LIFETIME_VOLATILE,
 BYTES_TO_BITS(AES_32B_KEY_SIZE), AES_32B_KEY_SIZE, PSA_ERROR_DOES_NOT_EXIST
},
#endif
#endif
};
