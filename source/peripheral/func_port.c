/*
 * func_port.c
 *
 * Created: 2017/02/04 17:22:36
 * Project: motordecoder_simple_v1
 * Version: 1.00
 *	Target:	ATtiny45
 *	Author: Y.Tsurui
 */ 

#include <avr/io.h>
#include "func_port.h"

void init_func_port(void)
{
	FUNC_PORT_DDR |= FUNC_PORT_DDR_MASK;
	FUNC_PORT_PORT &= ~FUNC_PORT_DDR_MASK;
}

void func_ctrl(uint8_t func_stat)
{
	if (func_stat) {
		FUNC_PORT_PORT |= FUNC_PORT_PIN_1_MASK;
	} else {
		FUNC_PORT_PORT &= ~FUNC_PORT_PIN_1_MASK;
	}
}
