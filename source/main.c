/*
 * main.c
 *
 * Created: 2017/02/04 17:21:00
 * Project: motordecoder_simple_v1
 * Version: 1.00
 *	Target:	ATtiny45
 *	Author: Y.Tsurui
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "peripheral/dcc_poller.h"
#include "peripheral/func_port.h"
#include "peripheral/pwm_motor_ctrl.h"
#include "peripheral/bemf_port.h"

#include "dcc/dcc_phy.h"
#include "dcc/dcc_logic.h"
#include "dcc/cv.h"

#include "app/train_ctrl.h"


int main(void)
{
    uint16_t dcc_timer_count_tmp;

    //uint8_t spd_count = 0;
    //uint8_t spd_tmp;
	
	uint8_t change_freq_count = 0;
	uint8_t change_freq_count_2 = 0;
	
	//uint8_t pwm_now_spd = 0;
	//uint8_t pwm_flag = 0;
	
	//uint8_t int_t = 0;
	
    
    init_dcc_poller();
    //init_func_port();
    init_pwm_motor_ctrl();
	
	load_cv();
	
	sei();
	
	while (1)
	{
		//Check DCC Packet Received
		dcc_timer_count_tmp = get_dcc_timer();
		if (dcc_timer_count_tmp) {
			receive_dcc_pulse(dcc_timer_count_tmp);
		}
		
		if (get_dcc_packet_cache_filled()) dcc_exec();
		
		if (get_event_timer()) {
			//1ms Event Driver
			clock_receiver_train_ctrl();
			clock_receiver_dcc_exec();
			clockReveicerAnalogChecker();
			/*
			spd_count++;
			if (spd_count >= 10) {
				spd_count = 0;
				spd_tmp++;
			}
			*/
			
			if (CV29 & 0x04) {
				// Enabled Analog Mode
				if (chk_analog_direction()) {
					setspeed_analog(chk_analog_direction());
				}
			}
			
			if (CV47 & 0x80) {
				change_freq_count++;
				if (change_freq_count >= 10) {
					change_freq_count = 0;
					change_freq_count_2++;
				}
				if (change_freq_count_2 >= (CV47 & 0x3F)) {
					change_freq_count_2 = 0;
					if (CV47 & 0x40) {
						//60Hz
						pwm_auto_change_freq(PWM_FREQ_60HZ);
					} else {
						//120Hz
						pwm_auto_change_freq(PWM_FREQ_120HZ);
					}
				}
			}
			
			
		}
	}
	
}

