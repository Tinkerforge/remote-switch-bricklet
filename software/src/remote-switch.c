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
	{REG_BITRATEMSB, 0xC8/4},  // bitrate = 32000000/0xC800 = 625 byte/s
	{REG_BITRATELSB, 0x00},
	{REG_PREAMBLEMSB, 0x00}, // no preamble
	{REG_PREAMBLELSB, 0x00},
	{REG_SYNCCONFIG, RF_SYNC_OFF}, // no sync word
	{REG_PACKETCONFIG1, RF_PACKET1_FORMAT_FIXED | RF_PACKET1_DCFREE_OFF | RF_PACKET1_CRC_OFF},
	{REG_PAYLOADLENGTH, RFM69_MESSAGE_LENGTH},
	{REG_FIFOTHRESH, RF_FIFOTHRESH_TXSTART_FIFONOTEMPTY | RFM69_MESSAGE_LENGTH}, // Set fifo threshold, send when threshold reached
//	{REG_PALEVEL, RF_PALEVEL_OUTPUTPOWER_11111 | RF_PALEVEL_PA0_ON}, // w/o power amplifier
	{REG_PALEVEL, RF_PALEVEL_OUTPUTPOWER_11111 | RF_PALEVEL_PA1_ON | RF_PALEVEL_PA2_ON}, // w/ power amplifier: PA 1 and 2 on, 20dBm
	{REG_OCP, RF_OCP_OFF | RF_OCP_TRIM_120}, // OCP off, trimming at 120mA
	{REG_TESTPA1, 0x5D}, // High power mode (only in hw version)
	{REG_TESTPA2, 0x7C}, // High power mode (only in hw version)
	{REG_OPMODE, RF_OPMODE_STANDBY}, // Set mode to transmitter
	{REG_ENDOFCONFIG, 0}
};

void invocation(const ComType com, const uint8_t *data) {
	switch(((MessageHeader*)data)->fid) {
		case FID_SWITCH: {
			switch_socket(com, (SwitchSocket*)data);
			return;
		}

		case FID_GET_SWITCHING_STATE: {
			get_switching_state(com, (GetSwitchingState*)data);
			return;
		}

		case FID_SET_TRIES: {
			set_tries(com, (SetTries*)data);
			return;
		}

		case FID_GET_TRIES: {
			get_tries(com, (GetTries*)data);
			return;
		}

		default: {
			BA->com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_NOT_SUPPORTED, com);
			break;
		}
	}
}

void switch_socket(const ComType com, const SwitchSocket *data) {
	if(BC->state != RF_IDLE) {
		return;
	}

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
		BC->rfm69_data[10] = 0b10001000;
		BC->rfm69_data[11] = 0b10001110;
	} else {
		BC->rfm69_data[10] = 0b10001110;
		BC->rfm69_data[11] = 0b10001000;
	}

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

void set_tries(const ComType com, const SetTries *data) {
	BC->num_send = data->tries;
	if(BC->state == RF_IDLE) {
		BC->wait = data->tries;
	}
	BA->com_return_setter(com, data);
}

void get_tries(const ComType com, const GetTries *data) {
	GetTriesReturn gtr;
	gtr.header        = data->header;
	gtr.header.length = sizeof(GetTriesReturn);
	gtr.tries         = BC->num_send;

	BA->send_blocking_with_timeout(&gtr,
	                               sizeof(GetTriesReturn),
	                               com);
}

void rfm69_configure(void) {
	for(uint8_t i = 0; rfm69_config[i][0] != REG_ENDOFCONFIG; i++) {
		rfm69_write_register(rfm69_config[i][0], &rfm69_config[i][1], 1);
	}
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

	// set synchronization bits:
	BC->rfm69_data[RFM69_MESSAGE_LENGTH-1] = 0b00000000;
	BC->rfm69_data[RFM69_MESSAGE_LENGTH-2] = 0b00000000;
	BC->rfm69_data[RFM69_MESSAGE_LENGTH-3] = 0b00000000;
	BC->rfm69_data[RFM69_MESSAGE_LENGTH-4] = 0b10000000;

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
					rfm69_write_register(REG_FIFO, BC->rfm69_data, RFM69_MESSAGE_LENGTH);
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

	if(tick_type == TICK_TASK_TYPE_MESSAGE) {
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
