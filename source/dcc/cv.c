/*
 * cv.c
 *
 * Created: 2016/12/13 21:22:24
 * Project: motordecoder_simple_v1
 * Version: 1.01
 *	Target:	ATtiny45
 *	Author: Y.Tsurui
 */ 

#include <avr/io.h>
#include <avr/eeprom.h>
#include "cv.h"

#define CV_COUNT 255

#define HARDWARE_NUM	0x03
#define MANUFACTURENUM	0x0D

#define CV1_DEFAULT		3		//7-Bit Addr
#define CV2_DEFAULT		0		//Vstart
#define CV3_DEFAULT		0		//Acceleration Rate
#define CV4_DEFAULT		0		//Deaceleration Rate
#define CV5_DEFAULT		255		//Vmax
#define CV6_DEFAULT		127		//Vmid
#define CV15_DEFAULT	0		// Decoder Lock
#define CV16_DEFAULT	0		// Decoder Lock
#define CV17_DEFAULT	192		//14-Bit Addr
#define CV18_DEFAULT	0		//14-Bit Addr
#define CV29_DEFAULT	0x06	//Basic Configuration
#define CV34_DEFAULT	0x06	//Roomlight Illumination Control FX
#define CV36_DEFAULT	0x01	//Consist FX Configuration
//#define CV37_DEFAULT	0x01	//FX1 Configuration (0-28,bit0-4, 30:Always Off, 31:Always On) / Bit5: Direction Enabled / Bit6: DirF(0:For/1:Rev) / Bit7: Analog Enabled
#define CV37_DEFAULT	0xFF	//FX1 Configuration (0xFF: Motor_Ctrl, Bit0-4: FX Addr (0-28, 30:Always Off, 31: Always On), Bit5: Direction Reverse, Bit7: Disable FX Bit
#define CV47_DEFAULT	0xCA	//PWM KickStart Configuration (Bit0-5: Timing, Bit6: Freq (0: 120Hz / 1: 60Hz), Bit7: Enable
#define CV56_DEFAULT	0x04	//PWM Frequency Configuration (TCCR1 CS13-CS10 Configuration RAW Data)
#define CV58_DEFAULT	0x7F	// Analog Max Speed
#define CV59_DEFAULT	0x03	// Analog Acceleration Rate
#define CV112_DEFAULT	0x5F	//FX1 Illumination Value / FX Classification Value
#define CV117_DEFAULT	0x04	//Roomlight Illumination Value

/*
FX_Classification_Table
0: Normal
1: Headlight
2: Taillight
3: Taillight
4: Roomlight
5: Roomlight
6: Headlight (Fading-in)
*/

uint8_t __attribute__((section(".eeprom"))) val[] =
/*  CV */ {
/*	 0 */	0xFF,			// Configure Area
/*   1 */	CV1_DEFAULT,	//7-bit Addr (1-127)
/*   2 */	CV2_DEFAULT,	//Vstart (0-255)
/*   3 */	CV3_DEFAULT,	//Acceleration Rate	
/*   4 */	CV4_DEFAULT,	//Deaceleration Rate
/*   5 */	CV5_DEFAULT,	//Vhigh (0-255)
/*   6 */	CV6_DEFAULT,	//Vmid	(0-255)
/*   7 */	HARDWARE_NUM,	//Version Number
/*   8 */	MANUFACTURENUM,	//Manufacture Number (Public Domain & Do-It-Yourself Decoders: 0x0D)
/*   9 */	0x00,
/*  10 */	0x00,
/*  11 */	0x00,
/*  12 */	0x00,
/*  13 */	0x00,
/*  14 */	0x00,
/*  15 */	0x00,			//Decoder Lock Value
/*  16 */	0x00,			//Decoder Lock Value
/*  17 */	CV17_DEFAULT,	//14-bit Addr MSB
/*  18 */	CV18_DEFAULT,	//14-bit Addr LSB
/*  19 */	0x00,			//Consist Addr
/*  20 */	0x00,
/*  21 */	0x00,
/*  22 */	0x00,
/*  23 */	0x00,
/*  24 */	0x00,
/*  25 */	0x00,
/*  26 */	0x00,
/*  27 */	0x00,
/*  28 */	0x00,
/*  29 */	CV29_DEFAULT,	//Bit0: Swap Forward/Reverse / bit1: 7-bit Addr / bit2: Analog Convert / bit5: 14-bit Addr
/*  30 */	0x00,
/*  31 */	0x00,
/*  32 */	0x00,
/*  33 */	0x00,			//Headlight FX Configuration (F0-F28)(0-28,bit0-4)
/*  34 */	CV34_DEFAULT,	//Roomlight FX Configuration (F0-F28)(0-28,bit0-4)
/*  35 */	0x00,
/*  36 */	CV36_DEFAULT,	//Consist FX Configuration (Bit0: FX1 / Bit1: FX2)
/*  37 */	CV37_DEFAULT,	//FX1 Configuration (F0-F28)(0-28,bit0-4, 30:Always Off, 31:Always On) / Bit5: Direction Enabled / Bit6: DirF(0:For/1:Rev) / Bit7: Analog Enabled
/*  38 */	0x00,			//FX2 Configuration (F0-F28)
/*  39 */	0x00,
/*  40 */	0x00,
/*  41 */	0x00,
/*  42 */	0x00,
/*  43 */	0x00,
/*  44 */	0x00,
/*  45 */	0x00,
/*  46 */	0x00,
/*  47 */	CV47_DEFAULT,	//PWM KickStart Configuration (Bit0-5: Timing, Bit6: Freq (0: 120Hz / 1: 60Hz), Bit7: Enable
///*  47 */	0x00,
/*  48 */	0x00,
/*  49 */	0x00,
/*  50 */	0x00,
/*  51 */	0x00,
/*  52 */	0x00,
/*  53 */	0x00,
/*  54 */	0x00,
/*  55 */	0x00,
/*  56 */	CV56_DEFAULT,	//PWM Frequency Configuration (TCCR1 CS13-CS10 Configuration RAW Data)
/*  57 */	0x00,
/*  58 */	CV58_DEFAULT,	// Analog Max Speed (0-255)
/*  59 */	CV59_DEFAULT,	// Analog Acceleration Rate
/*  60 */	0x00,
/*  61 */	0x00,
/*  62 */	0x00,
/*  63 */	0x00,
/*  64 */	0x00,
/*  65 */	0x00,
/*  66 */	0x00,
/*  67 */	9,		//Speed Table
/*  68 */	18,		//Speed Table
/*  69 */	27,		//Speed Table
/*  70 */	36,		//Speed Table
/*  71 */	46,		//Speed Table
/*  72 */	55,		//Speed Table
/*  73 */	64,		//Speed Table
/*  74 */	73,		//Speed Table
/*  75 */	82,		//Speed Table
/*  76 */	91,		//Speed Table
/*  77 */	100,	//Speed Table
/*  78 */	109,	//Speed Table
/*  79 */	118,	//Speed Table
/*  80 */	128,	//Speed Table
/*  81 */	137,	//Speed Table
/*  82 */	146,	//Speed Table
/*  83 */	155,	//Speed Table
/*  84 */	164,	//Speed Table
/*  85 */	173,	//Speed Table
/*  86 */	182,	//Speed Table
/*  87 */	191,	//Speed Table
/*  88 */	200,	//Speed Table
/*  89 */	209,	//Speed Table
/*  90 */	219,	//Speed Table
/*  91 */	228,	//Speed Table
/*  92 */	237,	//Speed Table
/*  93 */	246,	//Speed Table
/*  94 */	255,	//Speed Table
/*  95 */	0x00,
/*  96 */	0x00,
/*  97 */	0x00,
/*  98 */	0x00,
/*  99 */	0x00,
/* 100 */	0x00,
/* 101 */	0x00,
/* 102 */	0x00,
/* 103 */	0x00,
/* 104 */	0x00,
/* 105 */	0x00,
/* 106 */	0x00,
/* 107 */	0x00,
/* 108 */	0x00,
/* 109 */	0x00,
/* 110 */	0x00,
/* 111 */	0x00,
/* 112 */	0x00,
/* 113 */	0x00,
/* 114 */	0x00,
/* 115 */	0x00,
/* 116 */	0x00,
/* 117 */	0x00,
/* 118 */	0x00,
/* 119 */	0x00,
/* 120 */	0x00,
/* 121 */	0x00,
/* 122 */	0x00,
/* 123 */	0x00,
/* 124 */	0x00,
/* 125 */	0x00,
/* 126 */	0x00,
/* 127 */	0x00,
/* 128 */	0x00,
/* 129 */	0x00,
/* 130 */	0x00,
/* 131 */	0x00,
/* 132 */	0x00,
/* 133 */	0x00,
/* 134 */	0x00,
/* 135 */	0x00,
/* 136 */	0x00,
/* 137 */	0x00,
/* 138 */	0x00,
/* 139 */	0x00,
/* 140 */	0x00,
/* 141 */	0x00,
/* 142 */	0x00,
/* 143 */	0x00,
/* 144 */	0x00,
/* 145 */	0x00,
/* 146 */	0x00,
/* 147 */	0x00,
/* 148 */	0x00,
/* 149 */	0x00,
/* 150 */	0x00,
/* 151 */	0x00,
/* 152 */	0x00,
/* 153 */	0x00,
/* 154 */	0x00,
/* 155 */	0x00,
/* 156 */	0x00,
/* 157 */	0x00,
/* 158 */	0x00,
/* 159 */	0x00,
/* 160 */	0x00,
/* 161 */	0x00,
/* 162 */	0x00,
/* 163 */	0x00,
/* 164 */	0x00,
/* 165 */	0x00,
/* 166 */	0x00,
/* 167 */	0x00,
/* 168 */	0x00,
/* 169 */	0x00,
/* 170 */	0x00,
/* 171 */	0x00,
/* 172 */	0x00,
/* 173 */	0x00,
/* 174 */	0x00,
/* 175 */	0x00,
/* 176 */	0x00,
/* 177 */	0x00,
/* 178 */	0x00,
/* 179 */	0x00,
/* 180 */	0x00,
/* 181 */	0x00,
/* 182 */	0x00,
/* 183 */	0x00,
/* 184 */	0x00,
/* 185 */	0x00,
/* 186 */	0x00,
/* 187 */	0x00,
/* 188 */	0x00,
/* 189 */	0x00,
/* 190 */	0x00,
/* 191 */	0x00,
/* 192 */	0x00,
/* 193 */	0x00,
/* 194 */	0x00,
/* 195 */	0x00,
/* 196 */	0x00,
/* 197 */	0x00,
/* 198 */	0x00,
/* 199 */	0x00,
/* 200 */	0x00,
/* 201 */	0x00,
/* 202 */	0x00,
/* 203 */	0x00,
/* 204 */	0x00,
/* 205 */	0x00,
/* 206 */	0x00,
/* 207 */	0x00,
/* 208 */	0x00,
/* 209 */	0x00,
/* 210 */	0x00,
/* 211 */	0x00,
/* 212 */	0x00,
/* 213 */	0x00,
/* 214 */	0x00,
/* 215 */	0x00,
/* 216 */	0x00,
/* 217 */	0x00,
/* 218 */	0x00,
/* 219 */	0x00,
/* 220 */	0x00,
/* 221 */	0x00,
/* 222 */	0x00,
/* 223 */	0x00,
/* 224 */	0x00,
/* 225 */	0x00,
/* 226 */	0x00,
/* 227 */	0x00,
/* 228 */	0x00,
/* 229 */	0x00,
/* 230 */	0x00,
/* 231 */	0x00,
/* 232 */	0x00,
/* 233 */	0x00,
/* 234 */	0x00,
/* 235 */	0x00,
/* 236 */	0x00,
/* 237 */	0x00,
/* 238 */	0x00,
/* 239 */	0x00,
/* 240 */	0x00,
/* 241 */	0x00,
/* 242 */	0x00,
/* 243 */	0x00,
/* 244 */	0x00,
/* 245 */	0x00,
/* 246 */	0x00,
/* 247 */	0x00,
/* 248 */	0x00,
/* 249 */	0x00,
/* 250 */	0x00,
/* 251 */	0x00,
/* 252 */	0x00,
/* 253 */	0x00,
/* 254 */	0x00,
/* 255 */	0x00
};

//uint8_t __attribute__((section(".eeprom"))) eemem_init_flag = 0xFF;


uint8_t read_cv_raw(uint8_t CVnum)
{
	return (eeprom_read_byte(val + CVnum));
}

uint8_t read_cv(uint16_t CVnum)
{
	if (CVnum > CV_COUNT) return (0);
	
	return (read_cv_raw(CVnum & 0x00FF));
}

void write_eeprom(uint8_t addr, uint8_t data)
{
	eeprom_busy_wait();
	//eeprom_write_byte(val + addr, data);
	eeprom_update_byte(val + addr, data);
}

void initialize_speed_table(void)
{
	/*
	eeprom_write_byte(66, 9);
	eeprom_write_byte(67, 18);
	eeprom_write_byte(68, 27);
	eeprom_write_byte(69, 36);
	eeprom_write_byte(70, 46);
	eeprom_write_byte(71, 55);
	eeprom_write_byte(72, 64);
	eeprom_write_byte(73, 73);
	eeprom_write_byte(74, 82);
	eeprom_write_byte(75, 91);
	eeprom_write_byte(76, 100);
	eeprom_write_byte(77, 109);
	eeprom_write_byte(78, 118);
	eeprom_write_byte(79, 128);
	eeprom_write_byte(80, 137);
	eeprom_write_byte(81, 146);
	eeprom_write_byte(82, 155);
	eeprom_write_byte(83, 164);
	eeprom_write_byte(84, 173);
	eeprom_write_byte(85, 182);
	eeprom_write_byte(86, 191);
	eeprom_write_byte(87, 200);
	eeprom_write_byte(88, 209);
	eeprom_write_byte(89, 219);
	eeprom_write_byte(90, 228);
	eeprom_write_byte(91, 237);
	eeprom_write_byte(92, 246);
	eeprom_write_byte(93, 255);
	*/
	
	write_eeprom(67, 9);
	write_eeprom(68, 18);
	write_eeprom(69, 27);
	write_eeprom(70, 36);
	write_eeprom(71, 46);
	write_eeprom(72, 55);
	write_eeprom(73, 64);
	write_eeprom(74, 73);
	write_eeprom(75, 82);
	write_eeprom(76, 91);
	write_eeprom(77, 100);
	write_eeprom(78, 109);
	write_eeprom(79, 118);
	write_eeprom(80, 128);
	write_eeprom(81, 137);
	write_eeprom(82, 146);
	write_eeprom(83, 155);
	write_eeprom(84, 164);
	write_eeprom(85, 173);
	write_eeprom(86, 182);
	write_eeprom(87, 191);
	write_eeprom(88, 200);
	write_eeprom(89, 209);
	write_eeprom(90, 219);
	write_eeprom(91, 228);
	write_eeprom(92, 237);
	write_eeprom(93, 246);
	write_eeprom(94, 255);
	
}

void initialize_cv(void)
{
	uint8_t i;
	
	/*
	write_cv_byte(1, 3);
	write_cv_byte(2, 0x00);
	write_cv_byte(5, 0xFF);
	write_cv_byte(17, 192);
	write_cv_byte(18, 0);		//CV18
	write_cv_byte(29, 6);		//CV29
	write_cv_byte(37, 0x20);	//CV37 (FX1 Configuration)
	write_cv_byte(38, 0x60);	//CV38 (FX2 Configuration)
	write_cv_byte(39, 0x01);	//CV39 (FX3 Configuration)
	write_cv_byte(40, 0x02);	//CV40 (FX4 Configuration)
	*/
	
	for (i = 1; i < CV_COUNT; i++) {
		write_eeprom(i, 0);
	}
	
	
	/*
	eeprom_write_byte(0, CV1_DEFAULT);		//CV1 (7-Bit Addr)
	eeprom_write_byte(1, CV2_DEFAULT);		//CV2 (Vstart)
	eeprom_write_byte(2, CV3_DEFAULT);		//CV3 (Acceleration Rate)
	eeprom_write_byte(3, CV4_DEFAULT);		//CV4 (Deceleration Rate)
	eeprom_write_byte(4, CV5_DEFAULT);		//CV5 (Vmid)
	eeprom_write_byte(5, CV6_DEFAULT);		//CV6 (Vmax)
	eeprom_write_byte(16, CV17_DEFAULT);	//CV17 (14-Bit Addr)
	eeprom_write_byte(17, CV18_DEFAULT);	//CV18 (14-Bit Addr)
	eeprom_write_byte(18, 0);				//CV19 (Consist Addr)
	eeprom_write_byte(28, CV29_DEFAULT);	//CV29 (Basic Configuration)
	eeprom_write_byte(33, CV34_DEFAULT);	//CV34 (Roomlight FX Configuration)
	eeprom_write_byte(35, CV36_DEFAULT);	//CV36 (Consist FX Configuration)
	eeprom_write_byte(36, CV37_DEFAULT);	//CV37 (FX1 Configuration)
	eeprom_write_byte(46, CV47_DEFAULT);	//CV47 (PWM KickStart Configuration)
	eeprom_write_byte(55, CV56_DEFAULT);	//CV56 (PWM Frequency Configuration)
	*/
	
	write_eeprom(1, CV1_DEFAULT);		//CV1 (7-Bit Addr)
	write_eeprom(2, CV2_DEFAULT);		//CV2 (Vstart)
	write_eeprom(3, CV3_DEFAULT);		//CV3 (Acceleration Rate)
	write_eeprom(4, CV4_DEFAULT);		//CV4 (Deceleration Rate)
	write_eeprom(5, CV5_DEFAULT);		//CV5 (Vmid)
	write_eeprom(6, CV6_DEFAULT);		//CV6 (Vmax)
	write_eeprom(7, HARDWARE_NUM);
	write_eeprom(8, MANUFACTURENUM);
	write_eeprom(17, CV17_DEFAULT);	//CV17 (14-Bit Addr)
	write_eeprom(18, CV18_DEFAULT);	//CV18 (14-Bit Addr)
	write_eeprom(19, 0);				//CV19 (Consist Addr)
	write_eeprom(29, CV29_DEFAULT);	//CV29 (Basic Configuration)
	write_eeprom(34, CV34_DEFAULT);	//CV34 (Roomlight FX Configuration)
	write_eeprom(36, CV36_DEFAULT);	//CV36 (Consist FX Configuration)
	write_eeprom(37, CV37_DEFAULT);	//CV37 (FX1 Configuration)
	write_eeprom(47, CV47_DEFAULT);	//CV47 (PWM KickStart Configuration)
	write_eeprom(56, CV56_DEFAULT);	//CV56 (PWM Frequency Configuration)
	write_eeprom(58, CV58_DEFAULT); // CV58 (Analog Max Speed)
	write_eeprom(59, CV59_DEFAULT);	// CV59 (Analog Acceleration Rate)
	
	
	initialize_speed_table();

	write_eeprom(0, 0);

	load_cv();
}







void load_cv(void)
{
	uint8_t i;
	uint8_t CV0;

	eeprom_busy_wait();
	
	CV0 = read_cv_raw(0);
	
	if (CV0 == 0xFF) {
		//Initialize
		initialize_cv();
		return;
	}

	/*
	CV1 = read_cv_raw(1);
	CV2 = read_cv_raw(2);
	CV3 = read_cv_raw(3);
	CV4 = read_cv_raw(4);
	CV5 = read_cv_raw(5);
	CV6 = read_cv_raw(6);
	*/
	for (i = 0; i < 6; i++) {
		CV1_6[i] = read_cv_raw(1 + i);
	}
	/*
	if ((CV1 == 0xFF) && (CV2 == 0xFF) && (CV3 == 0xFF) && (CV4 == 0xFF) && (CV5 == 0xFF) & (CV6 == 0xFF)) {
		//Initialize
		initialize_cv();
		
		CV1 = read_cv_raw(1);
		CV2 = read_cv_raw(2);
		CV3 = read_cv_raw(3);
		CV4 = read_cv_raw(4);
		CV5 = read_cv_raw(5);
		CV6 = read_cv_raw(6);
	}
	*/
	CV17 = read_cv_raw(17);
	CV18 = read_cv_raw(18);
	CV19 = read_cv_raw(19);
	CV29 = read_cv_raw(29);
	CV34 = read_cv_raw(34);
	CV36 = read_cv_raw(36);
	CV37 = read_cv_raw(37);
	CV47 = read_cv_raw(47);
	CV56 = read_cv_raw(56);
	CV58 = read_cv_raw(58);
	CV59 = read_cv_raw(59);
	
	for (i = 0; i < 28; i++) {
		//CV67_94[i] = eeprom_read_byte(66 + i);
		CV67_94[i] = read_cv_raw(67 + i);
	}
	
}

void write_cv_byte(uint16_t CVnum, uint8_t data)
{
	uint8_t CV15, CV16, CVnum2;
	
	if (CVnum > CV_COUNT) return;
	
	CVnum2 = CVnum & 0x00FF;
	
	if ((CVnum2 != 15) && (CVnum2 != 16)) {
		CV15 = read_cv_raw(15);
		CV16 = read_cv_raw(16);
		
		if (CV15 != CV16) return;
	}
	/*
	if (CVnum2 == 1) CV1 = data;
	else if (CVnum2 == 2) CV2 = data;
	else if (CVnum2 == 3) CV3 = data;
	else if (CVnum2 == 4) CV4 = data;
	else if (CVnum2 == 5) CV5 = data;
	else if (CVnum2 == 6) CV6 = data;
	else if (CVnum2 == 7) return;
	else if (CVnum2 == 8) {
		if ((data == MANUFACTURENUM) || (data == 8)) {
			initialize_cv();
		}
		return;
	}
	else if (CVnum2 == 17) CV17 = data;
	else if (CVnum2 == 18) CV18 = data;
	else if (CVnum2 == 19) CV19 = data;
	else if (CVnum2 == 29) CV29 = data;
	*/
	
	switch (CVnum2) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			CV1_6[CVnum2 - 1] = data;
			break;
	/*
		case 1:
			CV1 = data;
			break;
		case 2:
			CV2 = data;
			break;
		case 3:
			CV3 = data;
			break;
		case 4:
			CV4 = data;
			break;
		case 5:
			CV5 = data;
			break;
		case 6:
			CV6 = data;
			break;
	*/
		case 7: //CV7 Read-Only
			return;
		case 8: //CV8 Read-Only
			if ((data == MANUFACTURENUM) || (data == 8)) {
				initialize_cv();
			}
			return;
		case 17:
			CV17 = data;
			break;
		case 18:
			CV18 = data;
			break;
		case 19:
			CV19 = data;
			break;
		case 29:
			CV29 = data;
			break;
		case 34:
			CV34 = data;
			break;
		case 36:
			CV36 = data;
			break;
		case 37:
			CV37 = data;
			break;
		case 47:
			CV47 = data;
			break;
		case 56:
			CV56 = data;
			break;
		case 58:
			CV58 = data;
			break;
		case 59:
			CV59 = data;
			break;
		case 67:
		case 68:
		case 69:
		case 70:
		case 71:
		case 72:
		case 73:
		case 74:
		case 75:
		case 76:
		case 77:
		case 78:
		case 79:
		case 80:
		case 81:
		case 82:
		case 83:
		case 84:
		case 85:
		case 86:
		case 87:
		case 88:
		case 89:
		case 90:
		case 91:
		case 92:
		case 93:
		case 94:
			CV67_94[CVnum2 - 67] = data;
			break;
	}
	
	/*
	eeprom_busy_wait();
	eeprom_write_byte(CVnum2 - 1, data);
	eeprom_busy_wait();
	*/
	write_eeprom(CVnum2, data);
}

void write_cv_bit(uint16_t CVnum, uint8_t data)
{
	uint8_t int_t1, int_t2;
	
	int_t1 = data & 0x07;
	int_t2 = 1 << int_t1;
	int_t1 = read_cv(CVnum);

	if (data & 0x08) {
		//Bit = 1
		int_t1 |= int_t2;
	} else {
		//Bit = 0
		int_t1 &= ~int_t2;
	}

	write_cv_byte(CVnum, int_t1);
}



