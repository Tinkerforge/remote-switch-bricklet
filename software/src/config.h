/* remote-switch-bricklet
 * Copyright (C) 2013 Olaf Lüke <olaf@tinkerforge.com>
 *
 * config.h: Remote Switch Bricklet specific configuration
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>

#include "bricklib/drivers/board/sam3s/SAM3S.h"

#include "remote-switch.h"

#define LOGGING_LEVEL LOGGING_DEBUG
#define DEBUG_BRICKLET 0

#define BRICKLET_FIRMWARE_VERSION_MAJOR 2
#define BRICKLET_FIRMWARE_VERSION_MINOR 0
#define BRICKLET_FIRMWARE_VERSION_REVISION 1

#define BRICKLET_HARDWARE_VERSION_MAJOR 1
#define BRICKLET_HARDWARE_VERSION_MINOR 0
#define BRICKLET_HARDWARE_VERSION_REVISION 0

#define BRICKLET_DEVICE_IDENTIFIER 235

#define PIN_MISO (BS->pin1_ad)
#define PIN_NSS  (BS->pin2_da)
#define PIN_SCK  (BS->pin3_pwm)
#define PIN_MOSI (BS->pin4_io)

#define BOARD_MCK 64000000

#define BRICKLET_NO_OFFSET
#define INVOCATION_IN_BRICKLET_CODE

//#define RFM69_MESSAGE_LENGTH 16
#define RFM69_MESSAGE_LENGTH_MAX 64

typedef struct {
	uint8_t rfm69_data[RFM69_MESSAGE_LENGTH_MAX];
	int32_t wait;
	int32_t wait_packet;
	RFState state;
	uint8_t num_send;
	bool switching_done;

	SwitchingType current_type;
	uint8_t current_packet_length;
} BrickContext;

#endif
