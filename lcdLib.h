#ifndef LCDLIB_H
#define LCDLIB_H

//constants
#define address 0x3F 	// address of LCD 
#define ROWS 4 			// number of rows on LCD
#define COLS 20 		// number of columns on LCD

#define Bl 0x08			// Backlight bit (must be sent with each signal to keep the backlight on)
#define En 0x04  		// Enable bit (used to clear the busy flag between sends)
#define Rw 0x02  		// Read/Write bit (not used since we never read from the screen)
#define Rs 0x01  		// Register select bit (used during write functions)
#define Cmd 0x00 		// command (used during non-write commands)
#define MICRODELAY 1


const uint8_t SET_CONTRAST =0x81;
const uint8_t SET_NORM_INV =0xa6 | 0;
const uint8_t SET_DISP =0xae;
const uint8_t SET_SCAN_DIR=0xc0;
const uint8_t SET_SEG_REMAP =0xa0;
const uint8_t LOW_COLUMN_ADDRESS  = 0x00;
const uint8_t HIGH_COLUMN_ADDRESS =0x10;
const uint8_t SET_PAGE_ADDRESS = 0xB0;
const uint8_t SET_START_LINE = 0x40 | 0;
const uint8_t SET_OFFSET_A = 0xD3;
const uint8_t SET_OFFSET_B = 0x00;
const uint8_t SET_RATION = 0xA8;
const uint8_t SET_NORMAL = 0xA6 | 0;
const uint8_t SET_SCAN = 0xC8;
const uint8_t SET_CONTRAST_A = 0x81;
const uint8_t SET_CONTRAST_B = 0xFF;
const uint8_t SET_REMAP = 0xA0 | 1;
const uint8_t SET_FULL_ON = 0xA4;

const uint8_t GRAPHIC_ADDR = 0x3C;


//structs
struct LCDelem //struct to hold positioning/length of each element
{		
	uint8_t row;
	uint8_t col;
	uint8_t len;
};

//user-friendly function declarations
void initLCD(); //set everything up, write the titles to the screen
void __attribute__(( noinline )) writeStr(uint8_t row, uint8_t col, uint8_t len, char *str);		//write to the LCD
//void __attribute__(( noinline )) writeScreen();
void __attribute__(( noinline )) setCursorPosition(uint8_t row, uint8_t col);
void toggleBlinkCursor(uint8_t on);
void toggleBackLight(uint8_t on);										
void toggleDisplay(uint8_t on);											
void clearDisplay();
void  __attribute__(( noinline )) updateLCDelems(uint8_t first, uint8_t last);
void __attribute__(( noinline )) checkWriteElem();

//lower-level function declarations
void sendByte(uint8_t data, uint8_t mode);							//sends two sets of 4bits and clears the flag after each
void send4BitsAndClear(uint8_t data);								//sends 4bits and clears the flag
void clearFlag(uint8_t data);										//clears the busy flag between sends
void send4Bits(uint8_t data);	
void __attribute__(( noinline )) pitchStr(char *str, int32_t pitch, uint8_t isNum, uint8_t center, int32_t note);
void __attribute__(( noinline )) pitchNumtoStr(int16_t num, char *str);
void __attribute__(( noinline )) strCatCat(char *cat1, char *cat2, char *str);
//void __attribute__(( noinline )) oscStr(uint8_t osc, char *str);
void __attribute__(( noinline )) arpEnvStr(uint8_t env, char *str);
void __attribute__(( noinline )) intToStr(char *before, int16_t num, uint8_t digitsOut, int16_t center, uint8_t leadingSign, char *str);
void __attribute__(( noinline )) writeBasicInt(int16_t num, uint8_t digits, uint8_t leadingSign, uint8_t row, uint8_t col);
//void numSlashNumStr(uint8_t res, uint8_t numA, uint8_t numB, char *str);
void __attribute__(( noinline )) writeNumSpaceStr(uint8_t num, char *str, uint8_t row, uint8_t col, uint8_t len);

#include "./lcdLib.c"


#endif 