/*
 * func_port.h
 *
 * Created: 2017/02/04 17:23:03
 * Project: motordecoder_simple_v1
 * Version: 1.00
 *	Target:	ATtiny45
 *	Author: Y.Tsurui
 */ 

#define FUNC_PORT_PORT			PORTB
#define FUNC_PORT_DDR			DDRB

#define FUNC_PORT_PIN_1			PB5

#define FUNC_PORT_PIN_1_MASK	(1 << FUNC_PORT_PIN_1)

#define FUNC_PORT_DDR_MASK		FUNC_PORT_PIN_1_MASK

void init_func_port();
void func_ctrl(uint8_t func_stat);
