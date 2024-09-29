
/*
 * File:			SigmaStudioFW.h
 *
 * Description:  	SigmaStudio System Framwork macro definitions. These
 *				macros should be implemented for your system's software.
 *
 * This software is distributed in the hope that it will be useful,
 * but is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * This software may only be used to program products purchased from
 * Analog Devices for incorporation by you into audio products that
 * are intended for resale to audio product end users. This software
 * may not be distributed whole or in any part to third parties.
 *
 * Copyright � 2020 Analog Devices, Inc. All rights reserved.
 */

#ifndef __SIGMASTUDIOFW_H__
#define __SIGMASTUDIOFW_H__

#include "USER_SETTINGS.h"
#include "inttypes.h"

typedef unsigned short ADI_DATA_U16;
typedef unsigned char ADI_REG_TYPE;

/*
 * Parameter data format
 */
#define SIGMASTUDIOTYPE_FIXPOINT 0
#define SIGMASTUDIOTYPE_INTEGER  1

int32_t SIGMASTUDIOTYPE_FIXPOINT_CONVERT(double value);

// For compatibility with certain export files, redirect SIGMASTUDIOTYPE_8_24_CONVERT to
// SIGMASTUDIOTYPE_FIXPOINT_CONVERT
#define SIGMASTUDIOTYPE_8_24_CONVERT(x) SIGMASTUDIOTYPE_FIXPOINT_CONVERT(x)

// Separate a 32-bit floating point value into four bytes
void SIGMASTUDIOTYPE_REGISTER_CONVERT(int32_t fixpt_val, uint8_t dest[4]);

/** Return the depth (in bytes) of a certain DSP memory location.
 * Currently this function is only implemented for data memory and program memory.
 * Control registers are not included.
 * Function is only required for I2C; it exists because of buffer size limitations in the Teensy I2C library.
 */
uint8_t getMemoryDepth(uint32_t address);

void SIGMA_WRITE_REGISTER_BLOCK(uint8_t devAddress, int address, int length, uint8_t pData[]);

// Write a 32-bit integer to the DSP. NOTE: 5.23 not supported quite yet.
void SIGMA_WRITE_REGISTER_INTEGER(int address, int32_t pData);

void SIGMA_WRITE_REGISTER_FLOAT(int address, double pData);

void SIGMA_WRITE_DELAY(uint8_t devAddress, int length, uint8_t pData[]);

// Function to read back data from the DSP, not called by SigmaStudio export files
// void SIGMA_READ_REGISTER_BYTES(int address, int length, uint8_t *pData);

// int32_t SIGMA_READ_REGISTER_INTEGER(int address, int length);

// double SIGMA_READ_REGISTER_FLOAT(int address);

/* SIGMASTUDIOTYPE_INTEGER_CONVERT is included in export files but not generally required.
 * Here it is simply a passthrough macro.
 */
#define SIGMASTUDIOTYPE_INTEGER_CONVERT(_value) (_value)

#endif
