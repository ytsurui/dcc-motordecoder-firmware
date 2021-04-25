/*
 * dcc_logic.c
 *
 * Created: 2016/12/13 20:17:37
 * Project: motordecoder_simple_v1
 * Version: 1.00
 *	Target:	ATtiny45
 *	Author: Y.Tsurui
 */ 

#include <avr/io.h>

#include "cv.h"

#include "../app/train_ctrl.h"
#include "../peripheral/pwm_motor_ctrl.h"
#include "../app/delay.h"


#define DCC_PACKET_LENGTH	8

uint8_t packet_cache[DCC_PACKET_LENGTH];
uint8_t packet_cache_length;
uint8_t packet_cache_enable_flag;

uint8_t packet_prog_cache[DCC_PACKET_LENGTH];

uint8_t service_mode_flag;

uint8_t basic_ack_counter;
uint8_t basic_ack_flag = 0;
#define BASIC_ACK_COUNT_MAX	12

volatile uint8_t page_r = 0;
volatile uint8_t page_verify_r, page_verify_r_flag = 0;

uint8_t consist_addr_cache = 0xFF;
uint8_t consist_addr_count = 0;
#define CONSIST_ADDR_COUNT_MAX 100

uint8_t speed_128step_cache = 0xFF;
uint8_t speed_128step_count = 0;
uint8_t speed_28step_cache = 0xFF;
uint8_t speed_28step_count = 0;
#define SPEED_RECV_COUNT_MAX 100

uint8_t ops_mode_cache[3];
uint8_t ops_mode_recv_count = 0;
#define OPS_MODE_RECV_COUNT_MAX 100

uint8_t service_page_reg_flag = 0;

//uint8_t debug_cv_write_pos = 130;

void basic_ack(void)
{
	//pwm_set_direction(PWM_DIRECTION_FOR);
	//pwm_set_spd(255);
	pwm_prog_mode(PWM_PROG_STAT_ON);
	//delay_ms(BASIC_ACK_COUNT_MAX);
	//pwm_prog_mode(PWM_PROG_STAT_OFF);
	basic_ack_counter = 1;
	basic_ack_flag = 1;
}

void clear_dcc_packet_cache(void)
{
	packet_cache_length = 0;
	packet_cache_enable_flag = 0;
}

void put_dcc_packet_cache(uint8_t data)
{
	if (packet_cache_length >= DCC_PACKET_LENGTH) return;

	packet_cache[packet_cache_length] = data;
	packet_cache_length++;
}

void set_dcc_packet_cache_filled_flag(void)
{
	packet_cache_enable_flag = 1;
}

uint8_t get_dcc_packet_cache_filled(void)
{
	if (packet_cache_enable_flag) {
		packet_cache_enable_flag = 0;
		return (1);
	}
	return (0);
}
/*
void debug_write(void)
{
	//write_cv_byte(debug_cv_write_pos, packet_prog_cache[0]);
	//debug_cv_write_pos++;
	//write_cv_byte(debug_cv_write_pos, packet_prog_cache[1]);
	//debug_cv_write_pos++;
	//write_cv_byte(debug_cv_write_pos, packet_prog_cache[2]);
	//debug_cv_write_pos++;
}
*/

uint8_t compareIntArray(uint8_t *ar1, uint8_t *ar2, uint8_t length)
{
	uint8_t i;
	for (i = 0; i < length; i++) {
		if (ar1[i] != ar2[i]) return (0);
	}
	return (1);
}

void dcc_prog_paged(void)
{
	uint16_t cv_addr;
	uint8_t byte_tmp;

	//if (compareIntArray(packet_cache, packet_prog_cache, 3)) {
	if ((packet_cache[0] == packet_prog_cache[0]) && (packet_cache[1] == packet_prog_cache[1]) && (packet_cache[2] == packet_prog_cache[2])) {
		//if (basic_ack_counter == 0xFF) return;
		//if (basic_ack_flag) return;

		if ((packet_prog_cache[0] & 0x04) == 0) {
			byte_tmp = packet_prog_cache[0] & 0x03;
			cv_addr = ((page_r - 1) * 4 + byte_tmp + 1);
			
			//write_cv_byte(130, cv_addr & 0x00FF);
			//write_cv_byte(131, byte_tmp);
			
			if (packet_prog_cache[0] & 0x08) {
				//Write
				
				//if (~service_page_reg_flag & 0x04) {
					write_cv_byte(cv_addr, packet_prog_cache[1]);
					service_page_reg_flag |= 0x04;
					basic_ack();
				//}
				
			} else {
				//Verify
				page_verify_r = read_cv(cv_addr);

				if (page_verify_r == packet_prog_cache[1]) {
					//Data match
					//if (~service_page_reg_flag & 0x02) {
						service_page_reg_flag |= 0x02;
						basic_ack();
					//}
				}
			}
			
			//debug_write();
			
			return;
		} else if ((packet_prog_cache[0] & 0x07) == 0x05) {
			//Paged Mode, Write Page Register 
			
			if (~service_page_reg_flag & 0x01) {
				page_r = packet_prog_cache[1];
				
				service_page_reg_flag |= 0x01;
				basic_ack();
				
				//write_cv_byte(129, page_r);
			}
			
			//debug_write();
			
			return;
		} else if ((packet_prog_cache[0] & 0x07) == 0x04) {
			//Phys Mode, CV #29 (Basic Configuration)
			if (packet_prog_cache[0] & 0x08) {
				//Write
				write_cv_byte(29, packet_prog_cache[1]);
				if (~service_page_reg_flag & 0x04) {
					service_page_reg_flag |= 0x04;
					basic_ack();
				}
			} else {
				//Verify
				page_verify_r = read_cv(29);
				if (page_verify_r == packet_prog_cache[1]) {
					if (~service_page_reg_flag & 0x02) {
						service_page_reg_flag |= 0x02;
						basic_ack();
					}
				}
			}
			
			//debug_write();
			
		} else if ((packet_prog_cache[0] & 0x07) == 0x06) {
			//Phys Mode, CV #7 (Version Number)
			if (packet_prog_cache[0] & 0x08) {
				//Write
				write_cv_byte(7, packet_prog_cache[1]);
				if (~service_page_reg_flag & 0x04) {
					service_page_reg_flag |= 0x04;
					basic_ack();
				}
			} else {
				//Verify
				page_verify_r = read_cv(7);
				if (page_verify_r == packet_prog_cache[1]) {
					if (~service_page_reg_flag & 0x02) {
						service_page_reg_flag |= 0x02;
						basic_ack();
					}
				}
			}
			
			//debug_write();
			
		} else if ((packet_prog_cache[0] & 0x07) == 0x07) {
			//Phys Mode, CV #8 (Manufacturer ID Number)
			if (packet_prog_cache[0] & 0x08) {
				//Write
				write_cv_byte(8, packet_prog_cache[1]);
				if (~service_page_reg_flag & 0x04) {
					service_page_reg_flag |= 0x04;
					basic_ack();
				}
			} else {
				//Verify
				page_verify_r = read_cv(8);
				if (page_verify_r == packet_prog_cache[1]) {
					if (~service_page_reg_flag & 0x02) {
						service_page_reg_flag |= 0x02;
						basic_ack();
					}
				}
			}
			
			//debug_write();
			
		}
	} else {
		packet_prog_cache[0] = packet_cache[0];
		packet_prog_cache[1] = packet_cache[1];
		packet_prog_cache[2] = packet_cache[2];
		//basic_ack_counter = 0;
		basic_ack_flag = 0;
		/*
		write_cv_byte(debug_cv_write_pos, packet_prog_cache[0]);
		debug_cv_write_pos++;
		write_cv_byte(debug_cv_write_pos, packet_prog_cache[1]);
		debug_cv_write_pos++;
		write_cv_byte(debug_cv_write_pos, packet_prog_cache[2]);
		debug_cv_write_pos++;
		*/
	}
}

void dcc_prog_direct(void)
{
	uint16_t cv_addr;
	uint8_t int_t;

	//if (compareIntArray(packet_cache, packet_prog_cache, 4)) {
	if ((packet_cache[0] == packet_prog_cache[0]) && (packet_cache[1] == packet_prog_cache[1]) && (packet_cache[2] == packet_prog_cache[2]) && (packet_cache[3] == packet_prog_cache[3])) {
		//if (basic_ack_counter == 0xFF) return;
		if (basic_ack_flag) return;

		cv_addr = (packet_prog_cache[0] & 0x03) + packet_prog_cache[1] + 1;

		switch (packet_prog_cache[0] & 0x0C) {
			case 0x08:
				//Bit Manipulation
				if (packet_prog_cache[2] & 0x10) {
					//Write Bit
					write_cv_bit(cv_addr, packet_prog_cache[2]);
					basic_ack();
				} else {
					//Verify Bit

				}
				break;
			case 0x04:
				//Verify Byte
				int_t = read_cv(cv_addr);

				if (int_t == packet_prog_cache[2]) {
					basic_ack();
				}
				break;
			case 0x0C:
				//Write Byte
				write_cv_byte(cv_addr, packet_prog_cache[2]);
				basic_ack();
				break;
			case 0x00:
				//Reserved
				break;
		}
	} else {
		packet_prog_cache[0] = packet_cache[0];
		packet_prog_cache[1] = packet_cache[1];
		packet_prog_cache[2] = packet_cache[2];
		packet_prog_cache[3] = packet_cache[3];
		//basic_ack_counter = 0;
		basic_ack_flag = 0;
	}
}

void dcc_normal_operation(void)
{
	uint8_t consist_flag = 0;
	uint8_t packet_read_start_pos;
	uint8_t byte_tmp;
	uint8_t direction_flag;
	
	uint8_t i;

	//PORTB |= 0x08;

	if (packet_cache_length == 3) {
		if ((packet_cache[0] == 0) && (packet_cache[1] == 0) && (packet_cache[2] == 0)) {
			//Reset
			service_mode_flag = 30;
			service_page_reg_flag = 0;
			pwm_set_prog_mode(1);

			return;
		} else if ((packet_cache[0] == 0xFF) && (packet_cache[1] == 0x00) && (packet_cache[2] == 0xFF)) {
			//Idle, Exit Service-Mode
			service_mode_flag = 0;
			
			return;
		}
	}
	
	if (CV19 != 0) {
		//Consist Mode
		if (((packet_cache[0] & 0x80) == 0) && ((packet_cache[1] & 0xE0) != 0xE0)) {
			consist_flag = CV19;
		}
	}
	
	if (consist_flag != 0) {
		if ((packet_cache[0] != 0) && (consist_flag == (packet_cache[0] & 0x7F))) {
			//7-Bit Addr Consist Mode
			packet_read_start_pos = 1;
		} else {
			return;
		}
	} else if (((packet_cache[0] & 0x80) == 0) && (CV29 & 0x20) == 0) {
		//7-Bit Addr
		if (packet_cache[0] != CV1_6[0]) {
		//	if ((CV19 & 0x7F) == 0) return;
		//	if (((packet_cache[1] & 0xE0) != 0x00) && ((packet_cache[1] & 0xE0) != 0xE0)) {
		//		consist_flag = 0xFF;
		//	}
			return;
		}
		packet_read_start_pos = 1;
	} else if (((packet_cache[0] & 0xC0) == 0xC0) && (CV29 & 0x20)) {
		//14-Bit Addr
		if ((packet_cache[0] != CV17) || (packet_cache[1] != CV18)) {
		//	if ((CV19 & 0x7F) == 0) return;
		//	if ((packet_cache[1] & 0xE0) != 0x00) return;
			return;
		}
		packet_read_start_pos = 2;
	} else {
		//Not for Mobile Decoder Packet
		
		return;
	}

	
	/*
	if (consist_flag) {
		//Consist Mode
		if ((CV19 & 0x7F) != packet_cache[0]) {
			//Not match consist Address
			return;
		}
	}
	*/

	byte_tmp = packet_cache[packet_read_start_pos] & 0xE0;

	switch (byte_tmp) {
		case 0x00:
			/*
			if (packet_cache[packet_read_start_pos] & 0x10) {
				//Consist Control
				if (packet_cache[packet_read_start_pos] & 0x03) {
					packet_cache[packet_read_start_pos + 1] |= 0x80;
				}
				
				if (consist_addr_cache == packet_cache[packet_read_start_pos + 1]) {
					write_cv_byte(19, packet_cache[packet_read_start_pos + 1]);
					consist_addr_cache = 0xFF;
					consist_addr_count = 0;
				} else {
					consist_addr_cache = packet_cache[packet_read_start_pos + 1];
				}
			} else {
				//Decoder Control
			}
			*/
			break;
		case 0x20:
			if (packet_cache_length != (packet_read_start_pos + 3)) {
				return;
			}
			
			if ((packet_cache[packet_read_start_pos] & 0x1F) != 0x1F) return;
			
			//Advanced Instructions (128-step)
			if (1) {
			//if ((packet_cache[packet_read_start_pos] & 0x1F) == 0x1F) {
				byte_tmp = packet_cache[packet_read_start_pos + 1] & 0x7F;

				if (speed_128step_cache == byte_tmp) {
					if (packet_cache[packet_read_start_pos + 1] & 0x80) {
						//Forward
						if (CV29 & 0x01) {
							direction_flag = 2;
						} else {
							direction_flag = 1;
						}
					} else {
						//Backward
						if (CV29 & 0x01) {
							direction_flag = 1;
						} else {
							direction_flag = 2;
						}
					}
					
					setspeed_128step(direction_flag, byte_tmp);
					speed_128step_cache = 0xFF;
					speed_128step_count = 0;
				} else {
					speed_128step_cache = byte_tmp;
					speed_128step_count = 1;
				}
			}
			break;
		/*
		case 0x40:
			if (speed_28step_cache == packet_cache[packet_read_start_pos]) {
				speed_28step_cache = 0xFF;
				speed_28step_count = 0;
				
				if (CV29 & 0x01) {
					//direction_flag = 0;
					setspeed_28step(1, packet_cache[packet_read_start_pos]);
				} else {
					//direction_flag = 1;
					setspeed_28step(2, packet_cache[packet_read_start_pos]);
				}
			} else {
				speed_28step_cache = packet_cache[packet_read_start_pos];
				speed_28step_count = 1;
			}
			
			break;
		case 0x60:
			//Forward Speed Instruction (14-step, 28-step)
			if (speed_28step_cache == packet_cache[packet_read_start_pos]) {
				speed_28step_cache = 0xFF;
				speed_28step_count = 0;
				
				if (CV29 & 0x01) {
					//direction_flag = 1;
					setspeed_28step(2, packet_cache[packet_read_start_pos]);
				} else {
					//direction_flag = 0;
					setspeed_28step(1, packet_cache[packet_read_start_pos]);
				}
			} else {
				speed_28step_cache = packet_cache[packet_read_start_pos];
				speed_28step_count = 1;
			}
			break;
		*/
		case 0x40:
		case 0x60:
			// Speed Instruction (14-step, 28-step)
			if (packet_cache_length != (packet_read_start_pos + 2)) {
				return;
			}
			
			if (1) {
			//if (speed_28step_cache == packet_cache[packet_read_start_pos]) {
				speed_28step_cache = 0xFF;
				speed_28step_count = 0;
				
				if (byte_tmp & 0x20) {
					if (CV29 & 0x01) {
						direction_flag = 2;
					} else {
						direction_flag = 1;
					}
				} else {
					if (CV29 & 0x01) {
						direction_flag = 1;
					} else {
						direction_flag = 2;
					}
				}
				
				setspeed_28step(direction_flag, packet_cache[packet_read_start_pos]);
			} else {
				speed_28step_cache = packet_cache[packet_read_start_pos];
				speed_28step_count = 1;
			}
			break;
		/*
		case 0x80:
			//Function1 (F0-F4)

			break;
		case 0xA0:
			//Function2, Function3 (F5-F8, F9-F12)

			break;
		case 0xC0:
			//Future Expansion Instruction
			//Function4 (F13-F20), Function5 (F21-F28)

			break
		*/
		case 0xE0:
			//Operation Mode CV Instruction
			for (i = 0; i < 3; i++) {
				if (ops_mode_cache[i] != packet_cache[packet_read_start_pos + i]) {
					ops_mode_cache[0] = packet_cache[packet_read_start_pos];
					ops_mode_cache[1] = packet_cache[packet_read_start_pos + 1];
					ops_mode_cache[2] = packet_cache[packet_read_start_pos + 2];
					ops_mode_recv_count = 1;
					return;
				}
			}
			
			ops_mode_recv_count = 0;
			
			if (packet_cache[packet_read_start_pos] & 0x08) {
				//Write
				if (packet_cache[packet_read_start_pos] & 0x04) {
					//Byte Mode
					write_cv_byte((((packet_cache[packet_read_start_pos] & 0x03) << 8) + packet_cache[packet_read_start_pos + 1] + 1), packet_cache[packet_read_start_pos + 2]);
				} else {
					//Bit Mode
					write_cv_bit((((packet_cache[packet_read_start_pos] & 0x03) << 8) + packet_cache[packet_read_start_pos + 1] + 1), packet_cache[packet_read_start_pos + 2]);
				}
			}
			break;
	}

}

void dcc_exec(void)
{
	

	if (packet_cache_length == 2) {
		//2-Byte Packet (none)
		return;
	}

	//if (service_mode_flag) {
	if (service_mode_flag && ((packet_cache[0] & 0x70) == 0x70)) {
		//DCC Service Mode
		
		service_mode_flag = 30;

		//PORTB &= ~0x08;

		switch (packet_cache_length) {
			case 3:
				//3-Byte Packet
				if ((packet_cache[0] == 0) && (packet_cache[1] == 0) && (packet_cache[2] == 0)) {
					//Reset
					service_mode_flag = 30;
					//service_page_reg_flag = 0;
					
					return;
				} else if ((packet_cache[0] == 0xFF) && (packet_cache[1] == 0x00) && (packet_cache[2] == 0xFF)) {
					//Idle, Exit Service-Mode

					return;
				/*
				} else if ((packet_cache[0] > 0x7F) && (packet_cache[0] < 0xC0) && (packet_cache[1] & 0x80)) {
					//Accessory Addr
					return;
				*/
				} else {
					//Paged Mode, Phys Mode, Address only Mode
					dcc_prog_paged();
				}
				break;
			case 4:
				//4-Byte Packet
				//Direct Mode
				dcc_prog_direct();
				break;
		}
	} else {
		//Normal Operation Mode
		dcc_normal_operation();

		//PORTB |= 0x08;
	}
}

void clock_receiver_dcc_exec(void)
{
	uint8_t i;
	
	if (service_mode_flag) {
		service_mode_flag--;
		if (service_mode_flag == 0) pwm_set_prog_mode(0);
	}

	if (basic_ack_counter != 0) {
		basic_ack_counter++;
		if (basic_ack_counter > BASIC_ACK_COUNT_MAX) {
			//basic_ack_counter = 0xFF;
			basic_ack_counter = 0;
			basic_ack_flag = 1;
			pwm_prog_mode(PWM_PROG_STAT_OFF);
		}
	}
	
	if (consist_addr_count != 0) {
		consist_addr_count++;
		if (consist_addr_count >= CONSIST_ADDR_COUNT_MAX) {
			consist_addr_cache = 0xFF;
			consist_addr_count = 0;
		}
	}
	
	if (speed_128step_count != 0) {
		speed_128step_count++;
		if (speed_128step_count >= SPEED_RECV_COUNT_MAX) {
			speed_128step_cache = 0xFF;
			speed_128step_count = 0;
		}
	}
	
	if (speed_28step_count != 0) {
		speed_28step_count++;
		if (speed_28step_count >= SPEED_RECV_COUNT_MAX) {
			speed_28step_cache = 0xFF;
			speed_28step_count = 0;
		}
	}
	
	if (ops_mode_recv_count != 0) {
		ops_mode_recv_count++;
		if (ops_mode_recv_count >= OPS_MODE_RECV_COUNT_MAX) {
			for (i = 0; i < 3; i++) {
				ops_mode_cache[i] = 0xFF;
			}
			ops_mode_recv_count = 0;
		}
	}
}