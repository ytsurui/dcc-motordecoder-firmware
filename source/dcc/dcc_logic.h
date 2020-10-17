/*
 * dcc_logic.h
 *
 * Created: 2016/12/13 20:18:24
 * Project: motordecoder_simple_v1
 * Version: 1.00
 *	Target:	ATtiny45
 *	Author: Y.Tsurui
 */ 

void clear_dcc_packet_cache(void);
void put_dcc_packet_cache(uint8_t data);
void set_dcc_packet_cache_filled_flag(void);
uint8_t get_dcc_packet_cache_filled(void);

void dcc_exec(void);
void clock_receiver_dcc_exec(void);
