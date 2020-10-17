/*
 * pwm_motor_ctrl.h
 *
 * Created: 2017/02/04 18:58:50
 * Project: motordecoder_simple_v1
 * Version: 1.00
 *	Target:	ATtiny45
 *	Author: Y.Tsurui
 */ 

/*
 *	Timer1 PWM (OC1A, OC1B)
 *		8-bit, 32kHz PWM
 *			For: PB1 (OC1A)
 *			Rev: PB4 (OC1B)
 */

#define PWM_MOTOR_CTRL_PORT			PORTB
#define PWM_MOTOR_CTRL_DDR			DDRB

#define PWM_MOTOR_CTRL_FOR_PIN		PB3
#define PWM_MOTOR_CTRL_REV_PIN		PB4
#define PWM_MOTOR_CTRL_PWM_PIN		PB1
#define PWM_MOTOR_CTRL_FOR_PIN_MASK	(1 << PWM_MOTOR_CTRL_FOR_PIN)
#define PWM_MOTOR_CTRL_REV_PIN_MASK	(1 << PWM_MOTOR_CTRL_REV_PIN)
#define PWM_MOTOR_CTRL_PWM_PIN_MASK	(1 << PWM_MOTOR_CTRL_PWM_PIN)

#define PWM_DIRECTION_FOR			1
#define PWM_DIRECTION_REV			2

#define PWM_PROG_STAT_ON			1
#define PWM_PROG_STAT_OFF			0

#define PWM_FREQ_32KHZ				0
#define PWM_FREQ_16KHZ				1
#define PWM_FREQ_60HZ				11
#define PWM_FREQ_120HZ				12

uint8_t get_event_timer(void);

void init_pwm_motor_ctrl(void);

void pwm_set_direction(uint8_t dir);
void pwm_set_spd(uint8_t spd);

void pwm_set_prog_mode(uint8_t prog_mode_enable);
void pwm_prog_mode(uint8_t prog_stat);

void pwm_change_freq(uint8_t freq_id);
void pwm_auto_change_freq(uint8_t freq);
