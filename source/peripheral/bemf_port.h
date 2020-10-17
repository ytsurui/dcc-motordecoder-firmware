/*
 * bemf_port.h
 *
 * Created: 2017/02/04 19:40:27
 * Project: motordecoder_simple_v1
 * Version: 1.00
 *	Target:	ATtiny45
 *	Author: Y.Tsurui
 */ 


#define BEMF_PORT_PORT		PORTB
#define BEMF_PORT_DDR		DDRB

#define BEMF_PORT_PIN		PB2
#define BEMF_PORT_PIN_MASK	(1 << BEMF_PORT_PIN)

void init_bemf_port(void);

