/*
    This file is a part of the wiringBone library
    Copyright (C) 2015 Abhraneel Bera

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA
*/

#ifndef COMMONDEFINES_H
#define COMMONDEFINES_H

#include "WConstants.h"

// GPIO Direction Enum
typedef enum {OUTPUT = 0, INPUT = 1} Direction;

// Dummy function prototypes
extern void sei(); // Enable interrupts (not used in this system)
extern void cli(); // Disable interrupts (not used in this system)

// PRU Control Register Definitions
#define CONTROL      0x0   // Control register offset
#define SOFT_RST_N   0     // Soft reset bit
#define ENABLE       1     // Enable bit

// IEP Register Definitions
#define GLOBAL_CFG   0x0   // Global configuration register offset
#define DEFAULT_INC  4     // Default increment bit
#define COUNT        0xC   // Count register offset
#define CNT_ENABLE   0     // Count enable bit

// PRU Memory Base Addresses
#define PRU_RAM0_BASE    0x4a300000
#define PRU_IEP_BASE     0x4a32e000
#define PRU0_IRAM_BASE   0x4a334000
#define PRU1_IRAM_BASE   0x4a338000
#define PRU0_CTRL_BASE   0x4a322000
#define PRU1_CTRL_BASE   0x4a324000

#endif // COMMONDEFINES_H

