/*
 * cv.h
 *
 * Created: 2016/12/13 21:29:50
 * Project: motordecoder_simple_v1
 * Version: 1.00
 *	Target:	ATtiny45
 *	Author: Y.Tsurui
 */ 

/*
uint8_t CV1;
uint8_t CV2;
uint8_t CV3;
uint8_t CV4;
uint8_t CV5;
uint8_t CV6;
*/
uint8_t CV1_6[6];

uint8_t CV17;
uint8_t CV18;
uint8_t CV19;
uint8_t CV29;
uint8_t CV34;
uint8_t CV36;
uint8_t CV37;
uint8_t CV47;
uint8_t CV56;
uint8_t CV58;
uint8_t CV59;

uint8_t CV67_94[28];


uint8_t read_cv(uint16_t);
void load_cv(void);
void write_cv_byte(uint16_t, uint8_t);
void write_cv_bit(uint16_t, uint8_t);
