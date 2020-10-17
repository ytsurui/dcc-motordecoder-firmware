/*
 * pwm_motor_ctrl.c
 *
 * Created: 2017/02/04 18:58:39
 * Project: motordecoder_simple_v1
 * Version: 1.00
 *	Target:	ATtiny45
 *	Author: Y.Tsurui
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "pwm_motor_ctrl.h"

uint8_t now_pwm_direction = 0;
uint8_t now_pwm_spd = 0;

uint8_t event_timer_counter;
#define EVENT_TIMER_MAX		32
uint8_t event_timer_max_count = EVENT_TIMER_MAX;

uint8_t event_timer_match_count = 0;

uint8_t pwm_prog_mode_flag = 0;

uint8_t pwm_freq_change_flag = 0;

ISR(TIMER1_OVF_vect)
{
	if (event_timer_counter < event_timer_max_count) {
	//if (event_timer_counter < EVENT_TIMER_MAX) {
		event_timer_counter++;
		//return;
	}
	
	if (pwm_freq_change_flag != 0) {
		if (pwm_freq_change_flag == 1) {
			//Set Freq Default
			pwm_change_freq(PWM_FREQ_32KHZ);
			pwm_freq_change_flag = 0;
		} else {
			//Set Freq Low (60Hz / 120Hz)
			pwm_change_freq(pwm_freq_change_flag);
			pwm_freq_change_flag = 1;
		} 
	}
	
}

ISR(TIMER1_COMPB_vect)
{
	if (event_timer_match_count != 0) {
		event_timer_counter = 1;
		
		if ((0xFF - OCR1B) < event_timer_match_count) {
			OCR1B = 0;
		} else {
			OCR1B += event_timer_match_count;
		}
	}
}

uint8_t get_event_timer(void)
{
	//if (event_timer_counter >= EVENT_TIMER_MAX) {
	if (event_timer_counter >= event_timer_max_count) {
		event_timer_counter = 0;
		return (1);
	}
	return (0);
}

void init_pwm_motor_ctrl(void)
{
	//Set PWM Ports Output
	PWM_MOTOR_CTRL_DDR |= (PWM_MOTOR_CTRL_FOR_PIN_MASK | PWM_MOTOR_CTRL_REV_PIN_MASK | PWM_MOTOR_CTRL_PWM_PIN_MASK);
	PWM_MOTOR_CTRL_PORT &= ~(PWM_MOTOR_CTRL_FOR_PIN_MASK | PWM_MOTOR_CTRL_REV_PIN_MASK | PWM_MOTOR_CTRL_PWM_PIN_MASK);
	
	//Initialize PWM Timer
	
	pwm_change_freq(PWM_FREQ_32KHZ);
	// Fast PWM-Mode, Prescaler = 1 (No Prescaling, 32kHz @ 8MHz), OC1A (PB1) as Output port
	//TCCR1 = (0 << CTC1) | (1 << PWM1A) | (1 << COM1A1) | (0 << COM1A0) | (0 << CS13) | (0 << CS12) | (0 << CS11) | (1 << CS10);
	//GTCCR = (0 << PWM1B) | (0 << COM1B1) | (0 << COM1B0) | (0 << FOC1B) | (0 << FOC1A) | (0 << PSR1);
	
	//TIMSK |= (1 << TOIE1);
	
	TCNT1 = 0;
	
	OCR1A = 0;
	OCR1B = 0;
	OCR1C = 0xFF;
	
}

void pwm_set_direction(uint8_t dir)
{
	if ((now_pwm_direction == dir) || (pwm_prog_mode_flag)) {
		return;
	}
	
	now_pwm_direction = dir;
	//TCCR1 &= ~((1 << PWM1A) | (1 << COM1A1) | (1 << COM1A0));
	//GTCCR &= ~((1 << PWM1B) | (1 << COM1B1) | (1 << COM1B0));
	PWM_MOTOR_CTRL_PORT &= ~(PWM_MOTOR_CTRL_FOR_PIN_MASK | PWM_MOTOR_CTRL_REV_PIN_MASK);
	
	if (dir == PWM_DIRECTION_FOR) {
		//For: PB1 (OC1A)
		//TCCR1 |= (1 << PWM1A) | (1 << COM1A1) | (0 << COM1A0);
		//OCR1A = OCR1B;
		//OCR1B = 0;
		PWM_MOTOR_CTRL_PORT |= PWM_MOTOR_CTRL_FOR_PIN_MASK;
	} else if (dir == PWM_DIRECTION_REV) {
		//Rev: PB4 (OC1B)
		//GTCCR |= (1 << PWM1B) | (1 << COM1B1) | (0 << COM1B0);
		//OCR1B = OCR1A;
		//OCR1A = 0;
		PWM_MOTOR_CTRL_PORT |= PWM_MOTOR_CTRL_REV_PIN_MASK;
	}
}

void pwm_set_spd(uint8_t spd)
{
	if ((now_pwm_spd == spd) || (pwm_prog_mode_flag)) {
		return;
	}

	now_pwm_spd = spd;
	/*
	if (now_pwm_direction == PWM_DIRECTION_FOR) {
		OCR1A = spd;
		OCR1B = 0;
	} else if (now_pwm_direction == PWM_DIRECTION_REV) {
		OCR1A = 0;
		OCR1B = spd;
	}
	*/
	OCR1A = spd;
}

void pwm_set_prog_mode(uint8_t prog_mode_enable)
{
	/*
	if (prog_mode_enable) {
		if (pwm_prog_mode_flag == 0) {
			//Disable PWM Output
			TCCR1 &= ~((1 << PWM1A) | (1 << COM1A1) | (1 << COM1A0));
			GTCCR &= ~((1 << PWM1B) | (1 << COM1B1) | (1 << COM1B0));
		
			//Disable Direction Output
			PWM_MOTOR_CTRL_PORT &= ~(PWM_MOTOR_CTRL_FOR_PIN_MASK | PWM_MOTOR_CTRL_REV_PIN_MASK);
		
			//PWM Port Output
			PWM_MOTOR_CTRL_PORT |= PWM_MOTOR_CTRL_PWM_PIN_MASK;
		
			pwm_prog_mode_flag = 1;
		}
	} else {
		//Forward Output Off
		PWM_MOTOR_CTRL_PORT &= ~PWM_MOTOR_CTRL_FOR_PIN_MASK;
		PWM_MOTOR_CTRL_PORT &= ~PWM_MOTOR_CTRL_PWM_PIN_MASK;
		
		//Disable Direction Output
		PWM_MOTOR_CTRL_PORT &= ~(PWM_MOTOR_CTRL_FOR_PIN_MASK | PWM_MOTOR_CTRL_REV_PIN_MASK);
		
		//Disable PWM Port Output
		PWM_MOTOR_CTRL_PORT &= ~PWM_MOTOR_CTRL_PWM_PIN_MASK;
		
		pwm_prog_mode_flag = 0;
	}
	*/
}

void pwm_prog_mode(uint8_t prog_stat)
{
	/*
	if (pwm_prog_mode_flag) {
		if (prog_stat == PWM_PROG_STAT_ON) {
			//Forward Output
			PWM_MOTOR_CTRL_PORT &= ~PWM_MOTOR_CTRL_REV_PIN_MASK;
			PWM_MOTOR_CTRL_PORT |= PWM_MOTOR_CTRL_FOR_PIN_MASK;
		} else if (prog_stat == PWM_PROG_STAT_OFF) {
			//Forward Output Off
			PWM_MOTOR_CTRL_PORT &= ~PWM_MOTOR_CTRL_FOR_PIN_MASK;
			PWM_MOTOR_CTRL_PORT &= ~PWM_MOTOR_CTRL_PWM_PIN_MASK;
		}
	}
	*/
	
	if (prog_stat == PWM_PROG_STAT_ON) {
		//Disable PWM Output
		TCCR1 &= ~((1 << PWM1A) | (1 << COM1A1) | (1 << COM1A0));
		GTCCR &= ~((1 << PWM1B) | (1 << COM1B1) | (1 << COM1B0));

		//Forward Output
		PWM_MOTOR_CTRL_PORT &= ~PWM_MOTOR_CTRL_REV_PIN_MASK;
		PWM_MOTOR_CTRL_PORT |= PWM_MOTOR_CTRL_FOR_PIN_MASK;

		//PWM Port Output
		PWM_MOTOR_CTRL_PORT |= PWM_MOTOR_CTRL_PWM_PIN_MASK;

		pwm_prog_mode_flag = 1;
	} else if (prog_stat == PWM_PROG_STAT_OFF) {
		//Forward Output Off
		//PWM_MOTOR_CTRL_PORT &= ~PWM_MOTOR_CTRL_FOR_PIN_MASK;
		PWM_MOTOR_CTRL_PORT &= ~PWM_MOTOR_CTRL_PWM_PIN_MASK;
		
		pwm_prog_mode_flag = 0;
	}
	
}


void pwm_change_freq(uint8_t freq_id)
{
	GTCCR = (0 << PWM1B) | (0 << COM1B1) | (0 << COM1B0) | (0 << FOC1B) | (0 << FOC1A) | (0 << PSR1);
	
	if (freq_id == PWM_FREQ_32KHZ) {
		// Fast PWM-Mode, Prescaler = 1 (No Prescaling, 32kHz @ 8MHz), OC1A (PB1) as Output port
		TCCR1 = (0 << CTC1) | (1 << PWM1A) | (1 << COM1A1) | (0 << COM1A0) | (0 << CS13) | (0 << CS12) | (0 << CS11) | (1 << CS10);
		event_timer_max_count = 32;
		event_timer_match_count = 0;
		TIMSK &= ~(1 << OCIE1B);
		TIMSK |= (1 << TOIE1);
		//GTCCR = (0 << PWM1B) | (0 << COM1B1) | (0 << COM1B0) | (0 << FOC1B) | (0 << FOC1A) | (0 << PSR1);
	} else if (freq_id == PWM_FREQ_16KHZ) {
		// Fast PWM-Mode, Prescaler = 2 (Prescaler / 2, 16kHz @ 8MHz), OC1A (PB1) as Output port
		TCCR1 = (0 << CTC1) | (1 << PWM1A) | (1 << COM1A1) | (0 << COM1A0) | (0 << CS13) | (0 << CS12) | (1 << CS11) | (0 << CS10);
		event_timer_max_count = 16;
		event_timer_match_count = 0;
		TIMSK &= ~(1 << OCIE1B);
		TIMSK |= (1 << TOIE1);
		//GTCCR = (0 << PWM1B) | (0 << COM1B1) | (0 << COM1B0) | (0 << FOC1B) | (0 << FOC1A) | (0 << PSR1);
	} else if (freq_id == PWM_FREQ_120HZ) {
		// Fast PWM-Mode, Prescaler = 9 (Prescaler / 256, 120Hz @ 8MHz), OC1A (PB1) as Output port
		TCCR1 = (0 << CTC1) | (1 << PWM1A) | (1 << COM1A1) | (0 << COM1A0) | (1 << CS13) | (0 << CS12) | (0 << CS11) | (1 << CS10);
		event_timer_max_count = 1;
		event_timer_match_count = 64;
		//GTCCR = (0 << PWM1B) | (0 << COM1B1) | (0 << COM1B0) | (0 << FOC1B) | (0 << FOC1A) | (0 << PSR1);
		TIMSK |= (1 << TOIE1) | (1 << OCIE1B);
	} else if (freq_id == PWM_FREQ_60HZ) {
		// Fast PWM-Mode, Prescaler = 9 (Prescaler / 256, 120Hz @ 8MHz), OC1A (PB1) as Output port
		TCCR1 = (0 << CTC1) | (1 << PWM1A) | (1 << COM1A1) | (0 << COM1A0) | (1 << CS13) | (0 << CS12) | (1 << CS11) | (0 << CS10);
		event_timer_max_count = 1;
		event_timer_match_count = 32;
		//GTCCR = (0 << PWM1B) | (0 << COM1B1) | (0 << COM1B0) | (0 << FOC1B) | (0 << FOC1A) | (0 << PSR1);
		TIMSK |= (1 << TOIE1) | (1 << OCIE1B);
	}
	OCR1B = event_timer_match_count;
	
}

void pwm_auto_change_freq(uint8_t freq)
{
	if (pwm_prog_mode_flag == 0) pwm_freq_change_flag = freq;
}
