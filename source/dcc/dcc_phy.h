/*
 * dcc_phy.h
 *
 * Created: 2016/12/12 22:48:23
 * Project: motordecoder_simple_v1
 * Version: 1.01
 *	Target:	ATtiny45
 *	Author: Y.Tsurui
 */ 


void receive_dcc_pulse(uint16_t pulse_count);
uint8_t check_dcc_mac_analog_mode(void);