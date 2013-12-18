/* remote-switch-bricklet
 * Copyright (C) 2013 Olaf Lüke <olaf@tinkerforge.com>
 *
 * remote-switch.c: Implementation of Remote Switch Bricklet messages
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

#include "remote-switch.h"

#include "bricklib/utility/util_definitions.h"
#include "bricklib/utility/init.h"
#include "bricklib/bricklet/bricklet_communication.h"
#include "brickletlib/bricklet_entry.h"
#include "brickletlib/bricklet_debug.h"
#include "config.h"

const uint8_t rfm69_config[][2] = {
	{REG_FRFMSB, RF_FRFMSB_433}, // 433 Mhz
	{REG_FRFMID, RF_FRFMID_433},
	{REG_FRFLSB, RF_FRFLSB_433},
//	{REG_FDEVMSB, 0x10}, // Frequency deviation
//	{REG_FDEVLSB, 0x00},
	{REG_DATAMODUL, RF_DATAMODUL_DATAMODE_PACKET | RF_DATAMODUL_MODULATIONTYPE_OOK | RF_DATAMODUL_MODULATIONSHAPING_00}, // Use OOK
	{REG_BITRATEMSB, TYPE_A_BITRATEMSB},
	{REG_BITRATELSB, TYPE_A_BITRATELSB},
	{REG_PREAMBLEMSB, 0x00}, // no preamble
	{REG_PREAMBLELSB, 0x00},
	{REG_SYNCCONFIG, RF_SYNC_OFF}, // Use empty sync to assure small pause between commands
	{REG_SYNCVALUE1, 0},
	{REG_SYNCVALUE2, 0},
	{REG_SYNCVALUE3, 0},
	{REG_SYNCVALUE4, 0},
	{REG_SYNCVALUE5, 0},
	{REG_SYNCVALUE6, 0},
	{REG_SYNCVALUE7, 0},
	{REG_SYNCVALUE8, 0},
	{REG_PACKETCONFIG1, RF_PACKET1_FORMAT_VARIABLE | RF_PACKET1_DCFREE_OFF | RF_PACKET1_CRC_OFF},
	{REG_PAYLOADLENGTH, TYPE_A_PACKET_LENGTH},
	{REG_FIFOTHRESH, RF_FIFOTHRESH_TXSTART_FIFONOTEMPTY}, // Set fifo threshold, send when threshold reached
//	{REG_PALEVEL, RF_PALEVEL_OUTPUTPOWER_11111 | RF_PALEVEL_PA0_ON}, // w/o power amplifier
	{REG_PALEVEL, RF_PALEVEL_OUTPUTPOWER_11111 | RF_PALEVEL_PA1_ON | RF_PALEVEL_PA2_ON}, // w/ power amplifier: PA 1 and 2 on, 20dBm
	{REG_OCP, RF_OCP_OFF | RF_OCP_TRIM_120}, // OCP off, trimming at 120mA
	{REG_TESTPA1, 0x5D}, // High power mode (only in hw version)
	{REG_TESTPA2, 0x7C}, // High power mode (only in hw version)
	{REG_OPMODE, RF_OPMODE_STANDBY}, // Set mode to transmitter
	{REG_ENDOFCONFIG, 0}
};

const uint8_t type_registers[NUM_TYPE_CONFIGURATIONS] = {
	REG_BITRATEMSB,
	REG_BITRATELSB,
	REG_PAYLOADLENGTH,
};

const uint8_t type_configurations[NUM_TYPES][NUM_TYPE_CONFIGURATIONS] = {
	{TYPE_A_BITRATEMSB, TYPE_A_BITRATELSB, TYPE_A_PACKET_LENGTH},
	{TYPE_B_BITRATEMSB, TYPE_B_BITRATELSB, TYPE_B_PACKET_LENGTH},
	{TYPE_B_BITRATEMSB, TYPE_B_BITRATELSB, TYPE_B_DIM_PACKET_LENGTH},
};

void invocation(const ComType com, const uint8_t *data) {
	switch(((MessageHeader*)data)->fid) {
		case FID_SWITCH: {
			switch_socket_a(com, (SwitchSocketA*)data);
			return;
		}

		case FID_GET_SWITCHING_STATE: {
			get_switching_state(com, (GetSwitchingState*)data);
			return;
		}

		case FID_SET_REPEATS: {
			set_repeats(com, (SetRepeats*)data);
			return;
		}

		case FID_GET_REPEATS: {
			get_repeats(com, (GetRepeats*)data);
			return;
		}

		case FID_SWITCH_SOCKET_A: {
			switch_socket_a(com, (SwitchSocketA*)data);
			return;
		}

		case FID_SWITCH_SOCKET_B: {
			switch_socket_b(com, (SwitchSocketB*)data);
			return;
		}

		case FID_DIM_SOCKET_B: {
			dim_socket_b(com, (DimSocketB*)data);
			return;
		}

		default: {
			BA->com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_NOT_SUPPORTED, com);
			break;
		}
	}
}

void change_type(const SwitchingType type) {
	if(type == BC->current_type || type >= NUM_TYPES) {
		return;
	}

	for(uint8_t i = 0; i < NUM_TYPE_CONFIGURATIONS; i++) {
		rfm69_write_register(type_registers[i], &type_configurations[type][i], 1);
	}

	BC->current_packet_length = type_configurations[type][2];
	BC->current_type = type;
}

void switch_socket_a(const ComType com, const SwitchSocketA *data) {
	if(BC->state != RF_IDLE) {
		return;
	}

	change_type(TYPE_A);

	const uint8_t house_code = data->house_code;
	for(uint8_t i  = 0; i < 5; i++) {
		if(house_code & (1 << i)) {
			BC->rfm69_data[i] = RFM69_DATA_ON;
		} else {
			BC->rfm69_data[i] = RFM69_DATA_FLOAT;
		}
	}

	const uint8_t receiver_code = data->receiver_code;
	for(uint8_t i  = 0; i < 5; i++) {
		if(receiver_code & (1 << i)) {
			BC->rfm69_data[i+5] = RFM69_DATA_ON;
		} else {
			BC->rfm69_data[i+5] = RFM69_DATA_FLOAT;
		}
	}

	if(data->switch_to == RFM69_SWITCH_TO_ON) {
		BC->rfm69_data[10] = RFM69_DATA_ON;
		BC->rfm69_data[11] = RFM69_DATA_FLOAT;
	} else {
		BC->rfm69_data[10] = RFM69_DATA_FLOAT;
		BC->rfm69_data[11] = RFM69_DATA_ON;
	}


	// Sync
	BC->rfm69_data[12] = 0b10000000;
	BC->rfm69_data[13] = 0b00000000;
	BC->rfm69_data[14] = 0b00000000;
	BC->rfm69_data[15] = 0b00000000;

	BC->state = RF_SENDING;
	BA->com_return_setter(com, data);
}

void switch_socket_b(const ComType com, const SwitchSocketB *data) {
	if(BC->state != RF_IDLE) {
		return;
	}

	change_type(TYPE_B);

	// Sync
	BC->rfm69_data[0] = 0b00000100;
	BC->rfm69_data[1] = 0b00000000;

	// Address
	for(uint8_t i = 0; i < 26; i++) {
		if(data->address & (1 << i)) {
			BC->rfm69_data[i+2] = RFM69_DATA_B_1;
		} else {
			BC->rfm69_data[i+2] = RFM69_DATA_B_0;
		}
	}

	// Switch All
	if(data->unit == 255) {
		BC->rfm69_data[28] = RFM69_DATA_B_1;
	} else {
		BC->rfm69_data[28] = RFM69_DATA_B_0;
	}

	// On/Off
	if(data->switch_to == RFM69_SWITCH_TO_ON) {
		BC->rfm69_data[29] = RFM69_DATA_B_1;
	} else {
		BC->rfm69_data[29] = RFM69_DATA_B_0;
	}

	// Unit
	for(uint8_t i = 0; i < 4; i++) {
		if(data->unit & (1 << i)) {
			BC->rfm69_data[i+30] = RFM69_DATA_B_1;
		} else {
			BC->rfm69_data[i+30] = RFM69_DATA_B_0;
		}
	}

	// Sync
	BC->rfm69_data[34] = 0b10000000;
	BC->rfm69_data[35] = 0b00000000;
	BC->rfm69_data[36] = 0b00000000;
	BC->rfm69_data[37] = 0b00000000;
	BC->rfm69_data[38] = 0b00000000;

	BC->state = RF_SENDING;
	BA->com_return_setter(com, data);
}

void dim_socket_b(const ComType com, const DimSocketB *data) {
	if(BC->state != RF_IDLE) {
		return;
	}

	change_type(TYPE_B_DIM);

	// Sync
	BC->rfm69_data[0] = 0b00000100;
	BC->rfm69_data[1] = 0b00000000;

	// Address
	for(uint8_t i = 0; i < 26; i++) {
		if(data->address & (1 << i)) {
			BC->rfm69_data[i+2] = RFM69_DATA_B_1;
		} else {
			BC->rfm69_data[i+2] = RFM69_DATA_B_0;
		}
	}

	// Switch All
	if(data->unit == 255) {
		BC->rfm69_data[28] = RFM69_DATA_B_1;
	} else {
		BC->rfm69_data[28] = RFM69_DATA_B_0;
	}

	// Dim bit (we have to shift everything by 4 to the left after here
	BC->rfm69_data[29] = 0b10100000;

	// Unit
	for(uint8_t i = 0; i < 4; i++) {
		if(data->unit & (1 << i)) {
			TYPE_B_DIM_SHIFT(RFM69_DATA_B_1, BC->rfm69_data, i+30);
		} else {
			TYPE_B_DIM_SHIFT(RFM69_DATA_B_0, BC->rfm69_data, i+30);
		}
	}

	// Dim value
	for(uint8_t i = 0; i < 4; i++) {
		if(data->dim_value & (1 << (3-i))) {
			TYPE_B_DIM_SHIFT(RFM69_DATA_B_1, BC->rfm69_data, i+34);
		} else {
			TYPE_B_DIM_SHIFT(RFM69_DATA_B_0, BC->rfm69_data, i+34);
		}
	}

	// Sync
	BC->rfm69_data[37] |= 0b0001000;
	BC->rfm69_data[38] = 0b00000000;
	BC->rfm69_data[39] = 0b00000000;
	BC->rfm69_data[40] = 0b00000000;
	BC->rfm69_data[41] = 0b00000000;
	BC->rfm69_data[42] = 0b00000000;

	BC->state = RF_SENDING;
	BA->com_return_setter(com, data);
}

void get_switching_state(const ComType com, const GetSwitchingState *data) {
	GetSwitchingStateReturn gssr;
	gssr.header        = data->header;
	gssr.header.length = sizeof(GetSwitchingStateReturn);
	if(BC->state == RF_IDLE) {
		gssr.state         = RFM69_READY;
	} else {
		gssr.state         = RFM69_BUSY;
	}

	BA->send_blocking_with_timeout(&gssr,
	                               sizeof(GetSwitchingStateReturn),
	                               com);
}

void set_repeats(const ComType com, const SetRepeats *data) {
	BC->num_send = data->repeats;
	if(BC->state == RF_IDLE) {
		BC->wait = data->repeats;
	}
	BA->com_return_setter(com, data);
}

void get_repeats(const ComType com, const GetRepeats *data) {
	GetRepeatsReturn grr;
	grr.header        = data->header;
	grr.header.length = sizeof(GetRepeatsReturn);
	grr.repeats       = BC->num_send;

	BA->send_blocking_with_timeout(&grr,
	                               sizeof(GetRepeatsReturn),
	                               com);
}

void rfm69_configure(void) {
	for(uint8_t i = 0; rfm69_config[i][0] != REG_ENDOFCONFIG; i++) {
		rfm69_write_register(rfm69_config[i][0], &rfm69_config[i][1], 1);
	}

	// Default configuration is for type a
	BC->current_type = TYPE_A;
	BC->current_packet_length = TYPE_A_PACKET_LENGTH;
}

void constructor(void) {
	_Static_assert(sizeof(BrickContext) <= BRICKLET_CONTEXT_MAX_SIZE, "BrickContext too big");

	PIN_NSS.type = PIO_OUTPUT_1;
	PIN_NSS.attribute = PIO_DEFAULT;
	BA->PIO_Configure(&PIN_NSS, 1);

	PIN_SCK.type = PIO_OUTPUT_1;
	PIN_SCK.attribute = PIO_DEFAULT;
	BA->PIO_Configure(&PIN_SCK, 1);

	PIN_MOSI.type = PIO_OUTPUT_1;
	PIN_MOSI.attribute = PIO_DEFAULT;
	BA->PIO_Configure(&PIN_MOSI, 1);

	PIN_MISO.type = PIO_INPUT;
	PIN_MISO.attribute = PIO_DEFAULT;
	BA->PIO_Configure(&PIN_MISO, 1);

	rfm69_configure();

	BC->num_send = NUM_SEND_DEFAULT;
	BC->wait = BC->num_send;
	BC->state = RF_IDLE;
	BC->switching_done = false;
}

void destructor(void) {
	PIN_NSS.type = PIO_INPUT;
	PIN_NSS.attribute = PIO_PULLUP;
	BA->PIO_Configure(&PIN_NSS, 1);

	PIN_SCK.type = PIO_INPUT;
	PIN_SCK.attribute = PIO_PULLUP;
	BA->PIO_Configure(&PIN_SCK, 1);

	PIN_MOSI.type = PIO_INPUT;
	PIN_MOSI.attribute = PIO_PULLUP;
	BA->PIO_Configure(&PIN_MOSI, 1);

	PIN_MISO.type = PIO_INPUT;
	PIN_MISO.attribute = PIO_PULLUP;
	BA->PIO_Configure(&PIN_MISO, 1);
}

void tick(const uint8_t tick_type) {
	if(tick_type & TICK_TASK_TYPE_CALCULATION) {
		switch(BC->state) {
			case RF_IDLE: {
				break;
			}

			case RF_SENDING: {
				uint8_t data_opmode;
				rfm69_read_register(REG_OPMODE, &data_opmode, 1);
				if(!(data_opmode & RF_OPMODE_STANDBY)) {
					uint8_t data_irqflags2;
					rfm69_read_register(REG_IRQFLAGS2, &data_irqflags2, 1);
					if((!(data_irqflags2 & RF_IRQFLAGS2_FIFONOTEMPTY))) {
						uint8_t data = RF_OPMODE_STANDBY;
						rfm69_write_register(REG_OPMODE, &data, 1);
					}
				} else {
					BC->state = RF_SEND;
				}
				break;
			}

			case RF_SEND: {
				uint8_t data_irqflags2;
				rfm69_read_register(REG_IRQFLAGS2, &data_irqflags2, 1);
				if((!(data_irqflags2 & RF_IRQFLAGS2_FIFONOTEMPTY))) {
					rfm69_write_register(REG_FIFO, BC->rfm69_data, BC->current_packet_length);
					uint8_t data = RF_OPMODE_TRANSMITTER;
					rfm69_write_register(REG_OPMODE, &data, 1);
					BC->wait--;
					if(BC->wait <= 0) {
						BC->wait = NUM_WAIT_AFTER_SEND;
						BC->state = RF_AFTER_SEND;
					} else {
						BC->state = RF_SENDING;
					}
				}
				break;
			}

			case RF_AFTER_SEND: {
				uint8_t data_opmode;
				rfm69_read_register(REG_OPMODE, &data_opmode, 1);
				if(!(data_opmode & RF_OPMODE_STANDBY)) {
					uint8_t data_irqflags2;
					rfm69_read_register(REG_IRQFLAGS2, &data_irqflags2, 1);
					if((!(data_irqflags2 & RF_IRQFLAGS2_FIFONOTEMPTY))) {
						uint8_t data = RF_OPMODE_STANDBY;
						rfm69_write_register(REG_OPMODE, &data, 1);
					}
				} else {
					BC->state = RF_WAIT_AFTER_SEND;
				}

				break;
			}

			case RF_WAIT_AFTER_SEND: {
				BC->wait--;
				if(BC->wait <= 0) {
					BC->wait = BC->num_send;
					BC->state = RF_IDLE;
					BC->switching_done = true;
				}
				break;
			}

			default: {
				// TODO: Error?
				break;
			}
		}
	}

	if(tick_type & TICK_TASK_TYPE_MESSAGE) {
		if(BC->switching_done) {
			BC->switching_done = false;
			SwitchingDone sd;
			BA->com_make_default_header(&sd, BS->uid, sizeof(SwitchingDone), FID_SWITCHING_DONE);

			BA->send_blocking_with_timeout(&sd,
										   sizeof(SwitchingDone),
										   *BA->com_current);
		}
	}
}

uint8_t spibb_transceive_byte(const uint8_t value) {
	uint8_t recv = 0;

	for(int8_t i = 7; i >= 0; i--) {
		PIN_SCK.pio->PIO_CODR = PIN_SCK.mask;
		if((value >> i) & 1) {
			PIN_MOSI.pio->PIO_SODR = PIN_MOSI.mask;
		} else {
			PIN_MOSI.pio->PIO_CODR = PIN_MOSI.mask;
		}

		SLEEP_US(1);
		if(PIN_MISO.pio->PIO_PDSR & PIN_MISO.mask) {
			recv |= (1 << i);
		}

		PIN_SCK.pio->PIO_SODR = PIN_SCK.mask;
		SLEEP_US(1);
	}

	return recv;
}

void rfm69_write_register(const uint8_t reg, const uint8_t *data, const uint8_t length) {
	SLEEP_US(1);
	PIN_NSS.pio->PIO_CODR = PIN_NSS.mask;
	spibb_transceive_byte(reg | REG_WRITE);
	for(uint8_t i = 0; i < length; i++) {
		spibb_transceive_byte(data[i]);
	}
	PIN_NSS.pio->PIO_SODR = PIN_NSS.mask;
}

void rfm69_read_register(const uint8_t reg, uint8_t *data, const uint8_t length) {
	SLEEP_US(1);
	PIN_NSS.pio->PIO_CODR = PIN_NSS.mask;
	spibb_transceive_byte(reg);
	for(uint8_t i = 1; i <= length; i++) {
		data[length - i] = spibb_transceive_byte(reg + i);
	}
	PIN_NSS.pio->PIO_SODR = PIN_NSS.mask;
}
