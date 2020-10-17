/*
 * bemf_port.c
 *
 * Created: 2017/02/04 19:40:02
 * Project: motordecoder_simple_v1
 * Version: 1.00
 *	Target:	ATtiny45
 *	Author: Y.Tsurui
 */ 

#include <avr/io.h>
#include "bemf_port.h"

void init_bemf_port(void)
{
	//Set BEMF Port as Input, no pull-up
	BEMF_PORT_DDR &= ~BEMF_PORT_PIN_MASK;
	BEMF_PORT_PORT &= ~BEMF_PORT_PIN_MASK;
}

